/* Simple firmware for a ESP32 displaying a static image on an EPaper Screen.
 *
 * Write an image into a header file using a 3...2...1...0 format per pixel,
 * for 4 bits color (16 colors - well, greys.) MSB first.  At 80 MHz, screen
 * clears execute in 1.075 seconds and images are drawn in 1.531 seconds.
 */

#include <esp_heap_caps.h>
#include <esp_log.h>
#include <esp_sleep.h>
#include <esp_timer.h>
#include <esp_types.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include <epdiy.h>

#include "sdkconfig.h"

#include "firasans_12.h"
#include "firasans_20.h"
#include "img_beach.h"
#include "img_board.h"
#include "img_zebra.h"

#define WAVEFORM EPD_BUILTIN_WAVEFORM

// choose the default demo board depending on the architecture
#ifdef CONFIG_IDF_TARGET_ESP32
#define DEMO_BOARD epd_board_v6
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define DEMO_BOARD epd_board_v7
#endif

EpdiyHighlevelState hl;

void idf_setup() {
    epd_init(&DEMO_BOARD, &EC080SC2, EPD_LUT_64K);
    // Set VCOM for boards that allow to set this in software (in mV).
    // This will print an error if unsupported. In this case,
    // set VCOM using the hardware potentiometer and delete this line.
    epd_set_vcom(2360);

    hl = epd_hl_init(WAVEFORM);

    heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);
    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
}

#ifndef ARDUINO_ARCH_ESP32
void delay(uint32_t millis) {
    vTaskDelay(millis / portTICK_PERIOD_MS);
}
#endif

static inline void checkError(enum EpdDrawError err) {
    if (err != EPD_DRAW_SUCCESS) {
        ESP_LOGE("demo", "draw error: %X", err);
    }
}

void draw_progress_bar(int x, int y, int width, int percent, uint8_t* fb) {
    const uint8_t white = 0xFF;
    const uint8_t black = 0x0;

    EpdRect border = {
        .x = x,
        .y = y,
        .width = width,
        .height = 20,
    };
    epd_fill_rect(border, white, fb);
    epd_draw_rect(border, black, fb);
    

    EpdRect bar = {
        .x = x + 5,
        .y = y + 5,
        .width = (width - 10) * percent / 100,
        .height = 10,
    };

    epd_fill_rect(bar, black, fb);

    checkError(epd_hl_update_area(&hl, MODE_DU, epd_ambient_temperature(), border));
}

void idf_loop() {
    // select the font based on display width
    const EpdFont* font;
    if (epd_width() < 1000) {
        font = &FiraSans_12;
    } else {
        font = &FiraSans_20;
    }

    uint8_t* fb = epd_hl_get_framebuffer(&hl);

    
    int temperature = epd_ambient_temperature();

    
    for(int i = 0; i < 600; i++) {
        epd_draw_line(3*i, 0, 3*i, 800, 0, fb);
        epd_draw_line(3*i+1, 0, 3*i+1, 800, 0xff, fb);
        epd_draw_line(3*i+2, 0, 3*i+2, 800, 0xff, fb);
    }
    epd_poweron();
    checkError(epd_hl_update_screen(&hl, MODE_GC16, temperature));
    epd_poweroff();
    
    printf("draw line 0\n");
    delay(1000);

    epd_poweron();
    epd_fill_rect(epd_full_screen(), 0xFF, fb);
    epd_clear();
    epd_poweroff();
    
    
    for(int i = 0; i < 600; i++) {
        epd_draw_line(3*i+1, 0, 3*i+1, 800, 0, fb);
        epd_draw_line(3*i, 0, 3*i, 800, 0xff, fb);
        epd_draw_line(3*i+2, 0, 3*i+2, 800, 0xff, fb);
        
    }
    epd_poweron();
    checkError(epd_hl_update_screen(&hl, MODE_GC16, temperature));
    epd_poweroff();
    printf("draw line 1\n");
    delay(1000);

    epd_poweron();
    epd_fill_rect(epd_full_screen(), 0xFF, fb);
    epd_clear();
    epd_poweroff();

    
    for(int i = 0; i < 600; i++) {
        epd_draw_line(3*i+2, 0, 3*i+2, 800, 0, fb);
        epd_draw_line(3*i+1, 0, 3*i+1, 800, 0xff, fb);
        epd_draw_line(3*i, 0, 3*i, 800, 0xff, fb);
    }
    epd_poweron();
    checkError(epd_hl_update_screen(&hl, MODE_GC16, temperature));
    epd_poweroff();
    printf("draw line 2\n");
    delay(1000);

    epd_poweron();
    epd_fill_rect(epd_full_screen(), 0xFF, fb);
    epd_clear();
    epd_poweroff();
}

#ifndef ARDUINO_ARCH_ESP32
void app_main() {
    idf_setup();

    while (1) {
        idf_loop();      
    };
}
#endif
