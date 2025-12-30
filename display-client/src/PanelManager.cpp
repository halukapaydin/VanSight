#include "PanelManager.h"
#include "SleepManager.h"
#include <ui.h>

// Pin definitions
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5

#define I2C_MASTER_NUM 0
#define I2C_MASTER_SDA_IO 8
#define I2C_MASTER_SCL_IO 9

// LVGL configuration
#define LVGL_TICK_PERIOD_MS     (2)
#define LVGL_TASK_MAX_DELAY_MS  (500)
#define LVGL_TASK_MIN_DELAY_MS  (1)
#define LVGL_TASK_STACK_SIZE    (4 * 1024)
#define LVGL_TASK_PRIORITY      (2)
#define LVGL_BUF_SIZE           (ESP_PANEL_LCD_H_RES * 20)

// Static instance
PanelManager* PanelManager::_instance = nullptr;

PanelManager& PanelManager::getInstance()
{
    static PanelManager instance;
    return instance;
}

PanelManager::PanelManager()
    : _panel(nullptr),
      _expander(nullptr),
      _lvgl_mux(nullptr),
      _initialized(false)
{
    _instance = this;
}

PanelManager::~PanelManager()
{
    if (_panel) delete _panel;
    if (_expander) delete _expander;
}

bool PanelManager::begin()
{
    if (_initialized) {
        return true;
    }
    
    Serial.println("\\n=== Initializing Panel Manager ===");
    
    // Initialize LVGL
    if (!initLVGL()) {
        Serial.println("[Panel] LVGL initialization failed");
        return false;
    }
    
    // Initialize Panel
    if (!initPanel()) {
        Serial.println("[Panel] Panel initialization failed");
        return false;
    }
    
    // Initialize IO Expander
    if (!initIOExpander()) {
        Serial.println("[Panel] IO Expander initialization failed");
        return false;
    }
    
    // Initialize UI
    lock();
    ui_init();
    unlock();
    
    // Create LVGL task AFTER everything is initialized
    xTaskCreate(lvgl_port_task, "lvgl", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL);
    Serial.println("[Panel] ✓ LVGL task created");
    
    _initialized = true;
    Serial.println("[Panel] ✓ Panel Manager initialized\\n");
    
    return true;
}

bool PanelManager::initLVGL()
{
    Serial.println("[Panel] Initializing LVGL...");
    
    String LVGL_Arduino = "Hello LVGL! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    Serial.println(LVGL_Arduino);
    
    // Initialize LVGL core
    lv_init();
    
    // Initialize LVGL buffers
    static lv_disp_draw_buf_t draw_buf;
    uint8_t *buf = (uint8_t *)heap_caps_calloc(1, LVGL_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_INTERNAL);
    if (!buf) {
        Serial.println("[Panel] Failed to allocate LVGL buffer");
        return false;
    }
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_BUF_SIZE);
    
    // Initialize the display device
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = ESP_PANEL_LCD_H_RES;
    disp_drv.ver_res = ESP_PANEL_LCD_V_RES;
    disp_drv.flush_cb = lvgl_port_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
#if ESP_PANEL_USE_LCD_TOUCH
    // Initialize the input device
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_port_tp_read;
    lv_indev_drv_register(&indev_drv);
#endif
    
    // Create LVGL mutex
    _lvgl_mux = xSemaphoreCreateRecursiveMutex();
    if (!_lvgl_mux) {
        Serial.println("[Panel] Failed to create LVGL mutex");
        return false;
    }
    
    // NOTE: LVGL task will be created after panel init
    
    Serial.println("[Panel] ✓ LVGL initialized");
    return true;
}

bool PanelManager::initPanel()
{
    Serial.println("[Panel] Initializing display panel...");
    
    _panel = new ESP_Panel();
    if (!_panel) {
        return false;
    }
    
    _panel->init();
    
#if ESP_PANEL_LCD_BUS_TYPE != ESP_PANEL_BUS_TYPE_RGB
    // Register flush ready callback for DMA
    static lv_disp_drv_t* disp_drv_ptr = lv_disp_get_default()->driver;
    _panel->getLcd()->setCallback(notify_lvgl_flush_ready, disp_drv_ptr);
#endif
    
    Serial.println("[Panel] ✓ Display panel initialized");
    return true;
}

bool PanelManager::initIOExpander()
{
    Serial.println("[Panel] Initializing IO expander...");
    
    _expander = new ESP_IOExpander_CH422G(I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000);
    if (!_expander) {
        return false;
    }
    
    _expander->init();
    _expander->begin();
    _expander->multiPinMode(TP_RST | LCD_BL | LCD_RST | SD_CS | USB_SEL, OUTPUT);
    _expander->multiDigitalWrite(TP_RST | LCD_BL | LCD_RST | SD_CS, HIGH);
    _expander->digitalWrite(USB_SEL, LOW);
    
    // Add expander to panel
    _panel->addIOExpander(_expander);
    
    // Start panel
    _panel->begin();
    
    Serial.println("[Panel] ✓ IO expander initialized");
    return true;
}

void PanelManager::lock(int timeout_ms)
{
    lvgl_port_lock(timeout_ms);
}

void PanelManager::unlock()
{
    lvgl_port_unlock();
}

// Static callbacks
#if ESP_PANEL_LCD_BUS_TYPE == ESP_PANEL_BUS_TYPE_RGB
void PanelManager::lvgl_port_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    if (_instance && _instance->_panel) {
        _instance->_panel->getLcd()->drawBitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
        lv_disp_flush_ready(disp);
    }
}
#else
void PanelManager::lvgl_port_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    if (_instance && _instance->_panel) {
        _instance->_panel->getLcd()->drawBitmap(area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    }
}

bool PanelManager::notify_lvgl_flush_ready(void *user_ctx)
{
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}
#endif

#if ESP_PANEL_USE_LCD_TOUCH
void PanelManager::lvgl_port_tp_read(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    if (!_instance || !_instance->_panel) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }
    
    _instance->_panel->getLcdTouch()->readData();
    
    bool touched = _instance->_panel->getLcdTouch()->getTouchState();
    if(!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        TouchPoint point = _instance->_panel->getLcdTouch()->getPoint();
        
        data->state = LV_INDEV_STATE_PR;
        data->point.x = point.x;
        data->point.y = point.y;
        
        Serial.printf("Touch point: x %d, y %d\\n", point.x, point.y);
        
        // Reset sleep timer on touch
        SleepManager::getInstance().resetTimer();
    }
}
#endif

void PanelManager::lvgl_port_lock(int timeout_ms)
{
    if (_instance && _instance->_lvgl_mux) {
        const TickType_t timeout_ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
        xSemaphoreTakeRecursive(_instance->_lvgl_mux, timeout_ticks);
    }
}

void PanelManager::lvgl_port_unlock(void)
{
    if (_instance && _instance->_lvgl_mux) {
        xSemaphoreGiveRecursive(_instance->_lvgl_mux);
    }
}

void PanelManager::lvgl_port_task(void *arg)
{
    Serial.println("[Panel] Starting LVGL task");
    
    uint32_t task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
    uint32_t loop_count = 0;
    
    while (1) {
        // if (loop_count % 100 == 0) {
        //     Serial.printf("[LVGL] Task loop %d\n", loop_count);
        // }
        
        lvgl_port_lock(-1);
        task_delay_ms = lv_timer_handler();
        lvgl_port_unlock();
        
        if (task_delay_ms > LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
        loop_count++;
    }
}
