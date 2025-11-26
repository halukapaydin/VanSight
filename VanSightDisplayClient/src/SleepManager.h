#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>
#include <esp_sleep.h>

/**
 * @brief Sleep Manager for automatic display sleep mode
 * 
 * Manages display backlight based on user activity
 */
class SleepManager {
public:
    /**
     * @brief Get singleton instance
     */
    static SleepManager& getInstance();
    
    /**
     * @brief Initialize sleep manager
     * @param panel Pointer to ESP_Panel instance
     * @param expander Pointer to IO Expander instance
     */
    void init(ESP_Panel* panel, ESP_IOExpander* expander);
    
    /**
     * @brief Update sleep state (call in loop)
     */
    void update();
    
    /**
     * @brief Reset inactivity timer
     * Call this on any user interaction
     */
    void resetTimer();
    
    /**
     * @brief Force sleep mode
     */
    void sleep();
    
    /**
     * @brief Force wake mode
     */
    void wake();
    
    /**
     * @brief Check if display is sleeping
     */
    bool isSleeping() const { return _sleeping; }

private:
    SleepManager();
    ~SleepManager();
    
    // Prevent copying
    SleepManager(const SleepManager&) = delete;
    SleepManager& operator=(const SleepManager&) = delete;
    
    ESP_Panel* _panel;
    ESP_IOExpander* _expander;
    unsigned long _lastActivityTime;
    unsigned long _sleepStartTime;
    bool _sleeping;
    bool _initialized;
    
    static const unsigned long SLEEP_TIMEOUT_MS = 60000; // 60 seconds
    static const int PIN_LCD_BL = 2; // Backlight pin on IO Expander
};

#endif // SLEEP_MANAGER_H
