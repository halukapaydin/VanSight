/**
 * @file lv_drv_conf.h
 * Configuration file for LVGL drivers
 */

#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include "lv_conf.h"

/*********************
 * DELAY INTERFACE
 *********************/
#define LV_DRV_DELAY_INCLUDE  <stdint.h>            /* Dummy include by default */
#define LV_DRV_DELAY_US(us)  /* delay_us(us) */    /* Delay the given number of microseconds */
#define LV_DRV_DELAY_MS(ms)  /* delay_ms(ms) */    /* Delay the given number of milliseconds */

/*********************
 * DISPLAY INTERFACE
 *********************/

/* SDL based drivers for display, mouse, mousewheel and keyboard */
#ifndef USE_SDL
#define USE_SDL 1
#endif

/* SDL2 based drivers for display, mouse, mousewheel and keyboard */
#ifndef USE_SDL_GPU
#define USE_SDL_GPU 0
#endif

#if USE_SDL || USE_SDL_GPU
#define SDL_HOR_RES     800
#define SDL_VER_RES     480
#define SDL_ZOOM        1       /* Zoom the display by this factor (useful for high DPI screens) */
#define SDL_INCLUDE_PATH    <SDL2/SDL.h>

/* Open two windows to test multi display support */
#define SDL_DUAL_DISPLAY            0
#endif

/*-----------------------------------------
 * Mouse or touchpad as input device
 *-----------------------------------------*/

#ifndef USE_EVDEV
#define USE_EVDEV           0
#endif
#ifndef USE_BSD_EVDEV
#define USE_BSD_EVDEV       0
#endif

#if USE_EVDEV || USE_BSD_EVDEV
#define EVDEV_NAME   "/dev/input/event0"        /* You can use the "evtest" Linux tool to get the list of devices and test them */
#define EVDEV_SWAP_AXES         0               /* Swap the x and y axes of the touchscreen */

#define EVDEV_CALIBRATE         0               /* Scale and offset the touchscreen coordinates by using maximum and minimum values for each axis */

#if EVDEV_CALIBRATE
#define EVDEV_HOR_MIN         0                 /* to invert axis swap EVDEV_XXX_MIN by EVDEV_XXX_MAX */
#define EVDEV_HOR_MAX      4096                 /* "evtest" Linux tool can help to get the correct calibraion values>*/
#define EVDEV_VER_MIN         0
#define EVDEV_VER_MAX      4096
#endif  /*EVDEV_CALIBRATE*/
#endif  /*USE_EVDEV*/

/*-----------------------------------------
 * Mouse or touchpad as input device (using libinput)
 *-----------------------------------------*/

#ifndef USE_LIBINPUT
#define USE_LIBINPUT           0
#endif

#if USE_LIBINPUT
#define LIBINPUT_NAME   "/dev/input/event0"        /* You can use the "evtest" Linux tool to get the list of devices and test them */
#endif  /*USE_LIBINPUT*/

/*-----------------------------------------
 * Mouse wheel as encoder input device
 *-----------------------------------------*/

#ifndef USE_XKB
#define USE_XKB           0
#endif

#if USE_LIBINPUT || USE_XKB
#define LIBINPUT_ENABLE_TOUCH_SCREEN    1
#define LIBINPUT_ENABLE_TOUCH_PAD       1
#define LIBINPUT_ENABLE_POINTER         1
#define LIBINPUT_ENABLE_KEYBOARD        1
#endif

/*-----------------------------------------
 * Keyboard of a PC (using SDL)
 *-----------------------------------------*/

/*No settings*/

#endif  /*LV_DRV_CONF_H*/
