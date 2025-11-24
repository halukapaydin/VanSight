#include "ui_event_handlers.h"
#include <Arduino.h>
#include <ui.h>
#include "ESPNowManager.h"
#include "espnow_config.h"

// SquareLine Studio Event Handler Implementations
// Bu dosya SquareLine Studio tarafından yönetilmez, güvenle düzenleyebilirsiniz

void onBtnHomeClick(lv_event_t * e)
{
    Serial.println("Home button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_HOME]);
}

void onBtnLeftChairClick(lv_event_t * e)
{
    Serial.println("Left Chair button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_LEFT_CHAIR]);
}

void onBtnRightChairClick(lv_event_t * e)
{
    Serial.println("Right Chair button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_RIGHT_CHAIR]);
}

void onBtnKitchenClick(lv_event_t * e)
{
    Serial.println("Kitchen button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_KITCHEN]);
}

void onBtnToiletClick(lv_event_t * e)
{
    Serial.println("Toilet button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_TOILET]);
}

void onBtnBathClick(lv_event_t * e)
{
    Serial.println("Bath button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_BATH]);
}

void onBtnBedClick(lv_event_t * e)
{
    Serial.println("Bed button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_BED]);
}

void onBtnBalkonyUpClick(lv_event_t * e)
{
    Serial.println("Balkony Up button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_BALKONY_UP]);
}

void onBtnBalkonyDownClick(lv_event_t * e)
{
    Serial.println("Balkony Down button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_BALKONY_DOWN]);
}

void onBtnProjectorClick(lv_event_t * e)
{
    Serial.println("Projector button clicked!");
    ESPNowManager::getInstance().sendRelayToggle(BUTTON_TO_RELAY_MAP[BTN_PROJECTOR]);
}

void onBtnReloadInformationClick(lv_event_t * e)
{
    Serial.println("Reload Information button clicked!");
    ESPNowManager::getInstance().requestAllStatus();
}

void onBtnCloseAllClick(lv_event_t * e)
{
    Serial.println("Close All button clicked!");
    ESPNowManager::getInstance().sendAllRelaysOff();
}
