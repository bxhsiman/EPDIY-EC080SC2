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
#include "test800.h"

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

    epd_clear();
    
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

uint8_t epd_get_pixel_color(int x, int y, int fb_width, int fb_height, const uint8_t *framebuffer) {
    if (x < 0 || x >= fb_width) {
        return 0;
    }
    if (y < 0 || y >= fb_height) {
        return 0;
    }
    int fb_width_bytes = fb_width / 2 + fb_width % 2;
    uint8_t buf_val = framebuffer[y * fb_width_bytes + x / 2];
    //printf("x:%d,y:%d,fb_width:%d,fb_height:%d,fb_width_bytes:%d,buf_val:%d\n",x,y,fb_width,fb_height,fb_width_bytes,buf_val);
    if (!(x % 2)) {
        buf_val = (buf_val & 0xF0) >> 4;
        // printf("First pixel %d \n",buf_val);
    } else {
        buf_val = (buf_val & 0x0F);
        // printf("Second pixel %d \n",buf_val);
    }
    

    // epd_draw_pixel needs a 0 -> 255 value
    // printf("Finall pixel %d \n \n \n",buf_val<<4);
    return buf_val<<4;
    
}

void epd_draw_color_image(EpdRect image_area, const uint8_t *image_buffer, uint8_t *framebuffer){
    uint16_t x_offset = 0;
    uint16_t y_offset = 0;
    uint8_t pixel_color;
    for (uint16_t y = 0; y < image_area.height; y++) {
        for (uint16_t x = 0; x < image_area.width; x++) {
          x_offset = image_area.x + x;
          y_offset = image_area.y + y;
          if (x_offset >= epd_rotated_display_width()) continue;
          if (y_offset >= epd_rotated_display_height()) continue;
          pixel_color = epd_get_pixel_color(x, y, image_area.width, image_area.height, image_buffer);
          
          epd_draw_pixel(
          x_offset,
          y_offset,
          pixel_color,
          framebuffer);
        
        }
    }
}

void idf_loop() {
    // select the font based on display width
    uint8_t* fb = epd_hl_get_framebuffer(&hl);
    int temperature = 23;
    epd_set_rotation(0);
    // 清理屏
    epd_poweron();
    epd_fullclear(&hl, temperature);
    epd_poweroff();
    
    // delay(5000);


    //循环显示图片
    EpdRect beach_area = {
        .x = 30,
        .y = 300,
        .width = img_beach_width,
        .height = img_beach_height,
    };
    EpdRect zebra_area = {
        .x = 30,
        .y = 300,
        .width = img_zebra_width,
        .height = img_zebra_height,
    };

    EpdRect test800_area = {
        .x = 0,
        .y = 0,
        .width = test800_width,
        .height = test800_height,
    };
    //epd_draw_rotated_image(test800_area,test800_data, fb);
    epd_draw_color_image(test800_area,test800_data, fb);
    epd_poweron();
    checkError(epd_hl_update_screen(&hl, MODE_GC16, temperature));
    epd_poweroff();
    
    delay(5000);
}

#ifndef ARDUINO_ARCH_ESP32
void app_main() {   
    idf_setup();

    while (1) {
        idf_loop();      
    };
}
#endif
