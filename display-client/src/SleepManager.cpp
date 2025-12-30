#include "SleepManager.h"


SleepManager& SleepManager::getInstance() {
    static SleepManager instance;
    return instance;
}

SleepManager::SleepManager()
    : _panel(nullptr),
      _expander(nullptr),
      _lastActivityTime(0),
      _sleepStartTime(0),
      _sleeping(false),
      _initialized(false) {
}

SleepManager::~SleepManager() {
}

void SleepManager::init(ESP_Panel* panel, ESP_IOExpander* expander) {
    if (!panel || !expander) {
        Serial.println("[Sleep] Error: Panel or Expander is null!");
        return;
    }
    
    _panel = panel;
    _expander = expander;
    _lastActivityTime = millis();
    _sleeping = false;
    _initialized = true;
    
    Serial.printf("[Sleep] Initialized (timeout: %lu ms)\n", SLEEP_TIMEOUT_MS);
}

void SleepManager::update() {
    if (!_initialized) {
        return;
    }
    
    unsigned long now = millis();
    
    if (_sleeping) {
        // COOL DOWN PERIOD: Ignore all touches for 1 second after sleeping
        // This prevents immediate wake-up due to backlight power noise
        if (millis() - _sleepStartTime < 1000) {
            return;
        }
        
        // Check for touch (polling)
        _panel->getLcdTouch()->readData();
        if (_panel->getLcdTouch()->getTouchState()) {
            // Double check to confirm it's a real touch (debounce)
            delay(50); // Debounce time
            _panel->getLcdTouch()->readData();
            
            if (_panel->getLcdTouch()->getTouchState()) {
                TouchPoint p = _panel->getLcdTouch()->getPoint();
                Serial.printf("[Sleep] Wake-up touch detected at %d, %d\n", p.x, p.y);
                wake();
            }
        }
        return;
    }
    
    unsigned long inactiveTime = now - _lastActivityTime;
    
    // Check if we should sleep
    if (inactiveTime >= SLEEP_TIMEOUT_MS) {
        sleep();
    }
}

void SleepManager::resetTimer() {
    if (!_initialized) {
        return;
    }
    
    _lastActivityTime = millis();
    
    // Wake up if sleeping
    if (_sleeping) {
        wake();
    }
}

void SleepManager::sleep() {
    if (!_initialized || _sleeping) {
        return;
    }
    
    Serial.println("[Sleep] Going to sleep...");
    
    // Turn off backlight via IO Expander
    _expander->digitalWrite(PIN_LCD_BL, LOW);
    
    _sleeping = true;
    _sleepStartTime = millis(); // Record sleep start time
}

void SleepManager::wake() {
    if (!_initialized || !_sleeping) {
        return;
    }
    
    Serial.println("[Sleep] Waking up...");
    
    // Turn on backlight via IO Expander
    _expander->digitalWrite(PIN_LCD_BL, HIGH);
    _sleeping = false;
    _lastActivityTime = millis();
}
