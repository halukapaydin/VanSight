#ifndef UI_EVENT_HANDLERS_H
#define UI_EVENT_HANDLERS_H

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// SquareLine Studio Event Handler Implementations
// Bu dosya SquareLine Studio tarafından yönetilmez, güvenle düzenleyebilirsiniz

void onBtnHomeClick(lv_event_t * e);
void onBtnLeftChairClick(lv_event_t * e);
void onBtnRightChairClick(lv_event_t * e);
void onBtnKitchenClick(lv_event_t * e);
void onBtnToiletClick(lv_event_t * e);
void onBtnBathClick(lv_event_t * e);
void onBtnBedClick(lv_event_t * e);
void onBtnBalkonyUpClick(lv_event_t * e);
void onBtnBalkonyDownClick(lv_event_t * e);
void onBtnProjectorClick(lv_event_t * e);
void onBtnReloadInformationClick(lv_event_t * e);
void onBtnCloseAllClick(lv_event_t * e);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // UI_EVENT_HANDLERS_H
