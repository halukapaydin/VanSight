#include "UIStateManager.h"
#include "config.h"

UIStateManager& UIStateManager::getInstance() {
    static UIStateManager instance;
    return instance;
}

UIStateManager::UIStateManager()
    : _lockFunc(nullptr),
      _unlockFunc(nullptr),
      _initialized(false) {
}

UIStateManager::~UIStateManager() {
}

void UIStateManager::init(void (*lockFunc)(int), void (*unlockFunc)(void)) {
    _lockFunc = lockFunc;
    _unlockFunc = unlockFunc;
    _initialized = true;
    Serial.println("[UI] State Manager initialized");
}

void UIStateManager::updateButtonState(lv_obj_t* buttonObj, bool isActive) {
    if (!_initialized || !buttonObj) {
        return;
    }
    
    if (_lockFunc) _lockFunc(-1);
    
    lv_color_t color = isActive ? COLOR_BUTTON_ACTIVE : COLOR_BUTTON_INACTIVE;
    lv_opa_t opa = isActive ? 255 : 0;
    lv_obj_set_style_bg_color(buttonObj, color, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(buttonObj,  opa, LV_PART_MAIN | LV_STATE_DEFAULT); // Ensure opacity is set
    
    if (_unlockFunc) _unlockFunc();
}

void UIStateManager::updateSensorLevel(lv_obj_t* barObj, lv_obj_t* labelObj, int level) {
    if (!_initialized || !barObj || !labelObj) {
        return;
    }
    
    // Clamp level to 0-100
    level = constrain(level, 0, 100);
    
    if (_lockFunc) _lockFunc(-1);
    
    // Update bar value
    lv_bar_set_value(barObj, level, LV_ANIM_ON);
    
    // Update label text
    char text[8];
    snprintf(text, sizeof(text), "%d%%", level);
    lv_label_set_text(labelObj, text);
    
    if (_unlockFunc) _unlockFunc();
    
    Serial.printf("[UI] Sensor updated: %d%%\n", level);
}

void UIStateManager::updateAllRelayStates(bool relayStates[]) {
    if (!_initialized) {
        return;
    }
    
    Serial.println("[UI] Updating all relay states...");
    
    // Update each button based on relay state
    for (uint8_t relayNum = 0; relayNum < 10; relayNum++) {  // Relays 1-10 (excluding special buttons)
        updateRelayButton(relayNum, relayStates[relayNum]);
    }
}

void UIStateManager::updateAllSensorLevels(int sensorLevels[]) {
    if (!_initialized) {
        return;
    }
    
    Serial.println("[UI] Updating all sensor levels...");
    
    // Update each sensor bar
    updateSensorLevel(ui_barCleanWaterLevel, ui_lblCleanWaterPercentage, sensorLevels[SENSOR_CLEAN_WATER]);
    updateSensorLevel(ui_barGrayWaterLevel, ui_lblGrayWaterPercentage, sensorLevels[SENSOR_GRAY_WATER]);
    updateSensorLevel(ui_barBlackWaterLevel, ui_lblBlackWaterPercentage, sensorLevels[SENSOR_BLACK_WATER]);
}

void UIStateManager::updateRelayButton(uint8_t relayNum, bool state) {
    lv_obj_t* buttonObj = getButtonByRelayNum(relayNum);
    if (buttonObj) {
        updateButtonState(buttonObj, state);
        Serial.printf("[UI] Relay %d button: %s\n", relayNum, state ? "ACTIVE" : "INACTIVE");
    }
}
void UIStateManager::updateConnectionStatus(bool connected)
{
    if (_lockFunc) _lockFunc(-1);

    lv_color_t color = connected ? COLOR_CONNECTION_ACTIVE : COLOR_CONNECTION_LOST;
    lv_obj_set_style_text_color(ui_lblTitle, color, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(ui_lblTitle, 255, LV_PART_MAIN | LV_STATE_DEFAULT); // Ensure opacity is set

    if (_unlockFunc) _unlockFunc();
}

lv_obj_t* UIStateManager::getButtonByRelayNum(uint8_t relayNum) {
    // Map relay number to button object
    switch (relayNum) {
        case 0: return ui_btnHome;
        case 1: return ui_btnLeftChair;
        case 2: return ui_btnRightChair;
        case 3: return ui_btnKitchen;
        case 4: return ui_btnToilet;
        case 5: return ui_btnBath;
        case 6: return ui_btnBed;
        case 7: return ui_btnBalkonyUp;
        case 8: return ui_btnBalkonyDown;
        case 9: return ui_btnProjector;
        default: return nullptr;
    }
}

void UIStateManager::getSensorObjects(uint8_t sensorNum, lv_obj_t** barObj, lv_obj_t** labelObj) {
    switch (sensorNum) {
        case SENSOR_CLEAN_WATER:
            *barObj = ui_barCleanWaterLevel;
            *labelObj = ui_lblCleanWaterPercentage;
            break;
        case SENSOR_GRAY_WATER:
            *barObj = ui_barGrayWaterLevel;
            *labelObj = ui_lblGrayWaterPercentage;
            break;
        case SENSOR_BLACK_WATER:
            *barObj = ui_barBlackWaterLevel;
            *labelObj = ui_lblBlackWaterPercentage;
            break;
        default:
            *barObj = nullptr;
            *labelObj = nullptr;
            break;
    }
}
