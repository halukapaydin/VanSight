#include "ui_event_handlers.h"
#include <Arduino.h>
#include <ui.h>

// SquareLine Studio Event Handler Implementations
// Bu dosya SquareLine Studio tarafından yönetilmez, güvenle düzenleyebilirsiniz

void onBtnHomeClick(lv_event_t * e)
{
    Serial.println("Home button clicked!");
    
    // Set the clean water level bar to 26%
    lv_bar_set_value(ui_barCleanWaterLevel, 26, LV_ANIM_ON);
    
    // Update the percentage label
    lv_label_set_text(ui_lblCleanWaterPercentage, "26%");
}

void onBtnLeftChairClick(lv_event_t * e)
{
    Serial.println("Left Chair button clicked!");
    // TODO: Implement left chair relay control
}

void onBtnRightChairClick(lv_event_t * e)
{
    Serial.println("Right Chair button clicked!");
    // TODO: Implement right chair relay control
}

void onBtnKitchenClick(lv_event_t * e)
{
    Serial.println("Kitchen button clicked!");
    // TODO: Implement kitchen relay control
}

void onBtnToiletClick(lv_event_t * e)
{
    Serial.println("Toilet button clicked!");
    // TODO: Implement toilet relay control
}

void onBtnBathClick(lv_event_t * e)
{
    Serial.println("Bath button clicked!");
    // TODO: Implement bath relay control
}

void onBtnBedClick(lv_event_t * e)
{
    Serial.println("Bed button clicked!");
    // TODO: Implement bed relay control
}

void onBtnBalkonyUpClick(lv_event_t * e)
{
    Serial.println("Balkony Up button clicked!");
    // TODO: Implement balkony up relay control
}

void onBtnBalkonyDownClick(lv_event_t * e)
{
    Serial.println("Balkony Down button clicked!");
    // TODO: Implement balkony down relay control
}

void onBtnProjectorClick(lv_event_t * e)
{
    Serial.println("Projector button clicked!");
    // TODO: Implement projector relay control
}

void onBtnReloadInformationClick(lv_event_t * e)
{
    Serial.println("Reload Information button clicked!");
    // TODO: Implement information reload logic
}

void onBtnCloseAllClick(lv_event_t * e)
{
    Serial.println("Close All button clicked!");
    // TODO: Implement close all relays logic
}
