#ifndef __VIDEO_GRAPHICS_H
#define __VIDEO_GRAPHICS_H

#include <lcom/lcf.h>

typedef enum {ONE, TWO} buffer_num;
buffer_num current_buffer;
vbe_mode_info_t mode_conf;
uint8_t *frame_buffer;
uint8_t *frame_buffer2;


int (init_video)(uint16_t mode);
int (set_frame_buffer)();
int (set_double_frame_buffer)();
int (flip_page)();
int (get_mode_conf)(uint16_t mode);
int (draw_pixel) (uint32_t color, uint16_t x, uint16_t y);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

#endif
