#ifndef PANEL_MANAGER_H
#define PANEL_MANAGER_H

#include <Arduino.h>
#include <lvgl.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>

/**
 * @brief Panel Manager - Handles all LVGL and display panel initialization
 * 
 * Encapsulates all display, touch, LVGL, and IO expander setup code
 */
class PanelManager {
public:
    /**
     * @brief Get singleton instance
     */
    static PanelManager& getInstance();
    
    /**
     * @brief Initialize panel, LVGL, and touch
     * @return true if successful
     */
    bool begin();
    
    /**
     * @brief Get panel pointer
     */
    ESP_Panel* getPanel() { return _panel; }
    
    /**
     * @brief Get IO expander pointer
     */
    ESP_IOExpander* getExpander() { return _expander; }
    
    /**
     * @brief Lock LVGL mutex
     * @param timeout_ms Timeout in milliseconds (-1 for infinite)
     */
    void lock(int timeout_ms = -1);
    
    /**
     * @brief Unlock LVGL mutex
     */
    void unlock();
    
    /**
     * @brief Get lock function pointer (for callbacks)
     */
    void (*getLockFunction())(int) { return lvgl_port_lock; }
    
    /**
     * @brief Get unlock function pointer (for callbacks)
     */
    void (*getUnlockFunction())() { return lvgl_port_unlock; }

private:
    PanelManager();
    ~PanelManager();
    
    // Prevent copying
    PanelManager(const PanelManager&) = delete;
    PanelManager& operator=(const PanelManager&) = delete;
    
    ESP_Panel* _panel;
    ESP_IOExpander* _expander;
    SemaphoreHandle_t _lvgl_mux;
    bool _initialized;
    
    // LVGL callbacks (static for C API)
    static void lvgl_port_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
    static bool notify_lvgl_flush_ready(void *user_ctx);
    static void lvgl_port_tp_read(lv_indev_drv_t * indev, lv_indev_data_t * data);
    static void lvgl_port_lock(int timeout_ms);
    static void lvgl_port_unlock(void);
    static void lvgl_port_task(void *arg);
    
    // Singleton instance
    static PanelManager* _instance;
    
    // Helper methods
    bool initLVGL();
    bool initPanel();
    bool initIOExpander();
};

#endif // PANEL_MANAGER_H
