// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "kbc.h"
#include "video_graphics.h"
#include <lcom/timer.h>
#include "cursor.xpm"
#include "sprite.h"

extern uint8_t scan_code;
extern vbe_mode_info_t mode_conf;
extern int counter;
Sprite* cursor;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (init_video(mode) != 0) return 1;
  sleep(delay);
  if(vg_exit() != 0) return 1;
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  if (get_mode_conf(mode) != 0) return 1;
  printf("mode");
  if (set_frame_buffer() != 0) return 1;
  printf("frame");
  if (init_video(mode) != 0) return 1;
  printf("video");
  if (vg_draw_rectangle(x, y, width, height, color) != 0) return 1;
  printf("rectangle");
  uint8_t irq_mask;
  if (kb_sub_int(&irq_mask) != 0) return 1;
  int ipc_status;
  message msg;
  while( scan_code != ESC_BREAK ) { 
    printf("loop");
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_mask) {
            kbc_ih();
          }
          break;
        default:
          break;
    }
    }
  }
  if (kb_unsub_int()!= 0) return 1;
  if (vg_exit() != 0) return 1;
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (get_mode_conf(mode) != 0) return 1;
  uint32_t R_First = (BIT(mode_conf.RedMaskSize) - 1) & (first >> mode_conf.RedFieldPosition);
  uint32_t G_First = (BIT(mode_conf.GreenMaskSize) - 1) & (first >> mode_conf.GreenFieldPosition);
  uint32_t B_First = (BIT(mode_conf.BlueMaskSize) - 1) & (first >> mode_conf.BlueFieldPosition);
  uint32_t new_color;
  uint8_t x_ = mode_conf.XResolution / no_rectangles;
  uint8_t y_ = mode_conf.YResolution / no_rectangles;
  if (set_frame_buffer() != 0) return 1;
  if (init_video(mode) != 0) return 1;
  for (int i = 0; i < no_rectangles; i++) {
    for (int j = 0; j < no_rectangles; j++) {
      if (mode == INDEXED_MODE) {
        new_color = (first + (i * no_rectangles + j) * step) % (1 << mode_conf.BitsPerPixel);
      }
      else {
        uint32_t R = (R_First + j * step) % (1 << mode_conf.RedMaskSize);
        uint32_t G = (G_First + i * step) % (1 << mode_conf.GreenMaskSize);
        uint32_t B = (B_First + (i + j) * step) % (1 << mode_conf.BlueMaskSize);
        new_color = ((R << mode_conf.RedFieldPosition) | (G << mode_conf.GreenFieldPosition) | (B << mode_conf.BlueFieldPosition));
      }
      if (vg_draw_rectangle((j * x_), (i * y_), x_, y_, new_color) != 0) return 1;
    }
  }
  uint8_t irq_mask;
  if (kb_sub_int(&irq_mask) != 0) return 1;
  int ipc_status;
  message msg;
  while( scan_code != ESC_BREAK ) { 
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_mask) {
            kbc_ih();
          }
          break;
        default:
          break;
    }
    }
  }
  if (kb_unsub_int()!= 0) return 1;
  if (vg_exit() != 0) return 1;
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (get_mode_conf(INDEXED_MODE) != 0) return 1;
  if (set_frame_buffer() != 0) return 1;
  if (init_video(INDEXED_MODE) != 0) return 1;
  if (draw_xpm(xpm, x, y) != 0) return 1;
  uint8_t irq_mask;
  if (kb_sub_int(&irq_mask) != 0) return 1;
  int ipc_status;
  message msg;
  while( scan_code != ESC_BREAK ) { 
    printf("loop");
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_mask) {
            kbc_ih();
          }
          break;
        default:
          break;
    }
    }
  }
  if (kb_unsub_int()!= 0) return 1;
  if (vg_exit() != 0) return 1;
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  uint16_t x = xi;
  uint16_t y = yi;                  
  if (get_mode_conf(0x115) != 0) return 1;
  if (set_double_frame_buffer() != 0) return 1;
  if (init_video(0x115) != 0) return 1;
  uint8_t irq_mask;
  uint8_t time_mask;
  if (kb_sub_int(&irq_mask) != 0) return 1;
  if (timer_subscribe_int(&time_mask) != 0) return 1;
  if (timer_set_frequency(0, fr_rate) != 0) return 1;
  int ipc_status;
  message msg;
  while( scan_code != ESC_BREAK ) { 
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_mask) {
            kbc_ih();
          }
          if (msg.m_notify.interrupts & time_mask) {
            if ( x < xf) {
              x += speed;
              if (x > xf) x = xf;
            }
            else if ( x > xf) {
              x -= speed;
              if (x < xf) x = xf;
            }
            if ( y < yf) {
              y += speed;
              if (y > yf) y = yf;
            }
            else if ( y > yf) {
              y -= speed;
              if (y < yf) y = yf;
            }
            if (draw_xpm(minix3_xpm, x, y) != 0) return 1;
            if (flip_page() != 0) {
              printf("FlipWrong");
              return 1;
            }
            if (vg_draw_rectangle(0, 0, 800, 600, 0x00000000) != 0) return 1;
          }
    }
    }
  }
  free(frame_buffer);
  free(frame_buffer2);
  if (timer_unsubscribe_int() != 0) return 1;
  if (kb_unsub_int()!= 0) return 1;
  if (vg_exit() != 0) return 1;
  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}

