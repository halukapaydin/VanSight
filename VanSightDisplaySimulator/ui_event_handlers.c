#include "ui_event_handlers.h"
#include <stdio.h>
#include "ui/src/ui.h"

// SquareLine Studio Event Handler Implementations
// Bu dosya SquareLine Studio tarafından yönetilmez, güvenle düzenleyebilirsiniz

void onBtnHomeClick(lv_event_t * e)
{
    printf("Home button clicked!\n");
    fflush(stdout);  // Terminal çıktısını hemen göster
    
    // Set the clean water level bar to 26%
    lv_bar_set_value(ui_barCleanWaterLevel, 26, LV_ANIM_ON);
    
    // Update the percentage label
    lv_label_set_text(ui_lblCleanWaterPercentage, "26%");
    
    printf("Clean water level set to 26%%\n");
    fflush(stdout);
}

void onBtnLeftChairClick(lv_event_t * e)
{
    printf("Left Chair button clicked!\n");
    fflush(stdout);
    // TODO: Implement left chair relay control
}

void onBtnRightChairClick(lv_event_t * e)
{
    printf("Right Chair button clicked!\n");
    fflush(stdout);
    // TODO: Implement right chair relay control
}

void onBtnKitchenClick(lv_event_t * e)
{
    printf("Kitchen button clicked!\n");
    fflush(stdout);
    // TODO: Implement kitchen relay control
}

void onBtnToiletClick(lv_event_t * e)
{
    printf("Toilet button clicked!\n");
    fflush(stdout);
    // TODO: Implement toilet relay control
}

void onBtnBathClick(lv_event_t * e)
{
    printf("Bath button clicked!\n");
    fflush(stdout);
    // TODO: Implement bath relay control
}

void onBtnBedClick(lv_event_t * e)
{
    printf("Bed button clicked!\n");
    fflush(stdout);
    // TODO: Implement bed relay control
}

void onBtnBalkonyUpClick(lv_event_t * e)
{
    printf("Balkony Up button clicked!\n");
    fflush(stdout);
    // TODO: Implement balkony up relay control
}

void onBtnBalkonyDownClick(lv_event_t * e)
{
    printf("Balkony Down button clicked!\n");
    fflush(stdout);
    // TODO: Implement balkony down relay control
}

void onBtnProjectorClick(lv_event_t * e)
{
    printf("Projector button clicked!\n");
    fflush(stdout);
    // TODO: Implement projector relay control
}

void onBtnReloadInformationClick(lv_event_t * e)
{
    printf("Reload Information button clicked!\n");
    fflush(stdout);
    // TODO: Implement information reload logic
}

void onBtnCloseAllClick(lv_event_t * e)
{
    printf("Close All button clicked!\n");
    fflush(stdout);
    // TODO: Implement close all relays logic
}
