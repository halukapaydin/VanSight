#include "ui_event_handlers.h"
#include <Arduino.h>
#include <VanSightLib.h>
#include <ui.h>

using namespace VanSight;

// Relay button mapping (button index to relay number)
// Using 1-based relay numbers (Relay 1-10)
const uint8_t RELAY_MAP[] = {
    1,  // Home
    2,  // Left Chair
    3,  // Right Chair
    4,  // Kitchen
    5,  // Toilet
    6,  // Bath
    7,  // Bed
    8,  // Balkony Up
    9,  // Balkony Down
    10  // Projector
};

// Debounce mechanism to prevent double events
static unsigned long lastClickTime[10] = {0};
const unsigned long DEBOUNCE_DELAY = 300; // 300ms debounce

// Helper function to handle relay toggle with debounce
static void handleRelayToggle(uint8_t buttonIndex, const char* buttonName) {
    unsigned long now = millis();
    if (now - lastClickTime[buttonIndex] < DEBOUNCE_DELAY) {
        return; // Ignore duplicate event
    }
    lastClickTime[buttonIndex] = now;
    
    Serial.printf("%s button clicked!\n", buttonName);
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[buttonIndex]);
}

void onBtnHomeClick(lv_event_t * e)
{
    handleRelayToggle(0, "Home");
}

void onBtnLeftChairClick(lv_event_t * e)
{
    handleRelayToggle(1, "Left Chair");
}

void onBtnRightChairClick(lv_event_t * e)
{
    handleRelayToggle(2, "Right Chair");
}

void onBtnKitchenClick(lv_event_t * e)
{
    handleRelayToggle(3, "Kitchen");
}

void onBtnToiletClick(lv_event_t * e)
{
    handleRelayToggle(4, "Toilet");
}

void onBtnBathClick(lv_event_t * e)
{
    handleRelayToggle(5, "Bath");
}

void onBtnBedClick(lv_event_t * e)
{
    handleRelayToggle(6, "Bed");
}

void onBtnBalkonyUpClick(lv_event_t * e)
{
    handleRelayToggle(7, "Balkony Up");
}

void onBtnBalkonyDownClick(lv_event_t * e)
{
    handleRelayToggle(8, "Balkony Down");
}

void onBtnProjectorClick(lv_event_t * e)
{
    handleRelayToggle(9, "Projector");
}

void onBtnReloadInformationClick(lv_event_t * e)
{
    Serial.println("Reload Information button clicked!");
    BleCommandManager::getInstance().requestStatus();
}

void onBtnCloseAllClick(lv_event_t * e)
{
    Serial.println("Close All button clicked!");
    BleCommandManager::getInstance().allRelaysOff();
}
