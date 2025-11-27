#ifndef UI_STATE_MANAGER_H
#define UI_STATE_MANAGER_H

#include <Arduino.h>
#include <lvgl.h>
#include <ui.h>

/**
 * @brief UI State Manager for thread-safe LVGL updates
 * 
 * Handles updating button colors and sensor bars based on ESP-NOW data
 */
class UIStateManager {
public:
    /**
     * @brief Get singleton instance
     */
    static UIStateManager& getInstance();
    
    /**
     * @brief Initialize UI state manager
     * @param lockFunc LVGL lock function
     * @param unlockFunc LVGL unlock function
     */
    void init(void (*lockFunc)(int), void (*unlockFunc)(void));
    
    /**
     * @brief Update button background color based on relay state
     * @param buttonObj LVGL button object
     * @param isActive true if relay is ON, false if OFF
     */
    void updateButtonState(lv_obj_t* buttonObj, bool isActive);
    
    /**
     * @brief Update sensor level bar and label
     * @param barObj LVGL bar object
     * @param labelObj LVGL label object for percentage
     * @param level Level percentage (0-100)
     */
    void updateSensorLevel(lv_obj_t* barObj, lv_obj_t* labelObj, int level);
    
    /**
     * @brief Update all relay button states
     * @param relayStates Array of relay states (16 elements)
     */
    void updateAllRelayStates(int relayStates[]);
    
    /**
     * @brief Update all sensor levels
     * @param sensorLevels Array of sensor levels (3 elements, 0-100)
     */
    void updateAllSensorLevels(int sensorLevels[]);
    
    /**
     * @brief Update single relay button state
     * @param relayNum Relay number (1-16)
     * @param state true if ON, false if OFF
     */
    void updateRelayButton(uint8_t relayNum, bool state);
    void updateConnectionStatus(bool connected);

private:
    UIStateManager();
    ~UIStateManager();
    
    // Prevent copying
    UIStateManager(const UIStateManager&) = delete;
    UIStateManager& operator=(const UIStateManager&) = delete;
    
    void (*_lockFunc)(int);
    void (*_unlockFunc)(void);
    bool _initialized;
    
    // Helper to get button object by relay number
    lv_obj_t* getButtonByRelayNum(uint8_t relayNum);
    
    // Helper to get bar and label objects by sensor number
    void getSensorObjects(uint8_t sensorNum, lv_obj_t** barObj, lv_obj_t** labelObj);
};

#endif // UI_STATE_MANAGER_H
