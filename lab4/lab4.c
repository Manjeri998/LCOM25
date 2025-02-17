// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab4.h>

#include <stdbool.h>
#include <stdint.h>
#include <lcom/timer.h>


#include "mouse.h"


extern bool packet_complete;
extern struct packet p;
typedef enum {INIT, DRAW_UP, VERTICIE, DRAW_DOWN, COMP} state_t;
typedef enum {RDOWN, LDOWN, INVALID, IDL} action_t;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt ) {
  uint8_t mouse_mask;
  uint32_t packet_cnt = 0;
  
  if (sub_mouse_int(&mouse_mask) != 0) return 1;
  if (mouse_enable_data_reporting() != 0) return 1;
  int ipc_status;
  message msg;
  while( packet_cnt < cnt) { 
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_mask) {
            mouse_ih();
            if (packet_complete) {
              packet_cnt++;
              mouse_print_packet(&p);
            }
          }
          break;
        default:
          break;
    }
    }
  }
  if (write_mouse_command(DISABLE_DR) != 0) return 1;
  if (unsub_mouse_int() != 0) return 1;
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  uint8_t mouse_mask;
  uint8_t timer_mask;
  uint8_t cou = 0;

  if (timer_subscribe_int(&timer_mask) != 0) return 1;
  if (sub_mouse_int(&mouse_mask) != 0) return 1;
  if (write_mouse_command(ENABLE_DR) != 0) return 1;
  uint8_t frq = sys_hz();
  int ipc_status;
  message msg;
  while( (cou / frq) < idle_time) { 
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_mask) {
            cou = 0;
            mouse_ih();
            if (packet_complete) {
              mouse_print_packet(&p);
            }
          }
          if (msg.m_notify.interrupts & timer_mask) {
            timer_int_handler();
            cou++;
          }
          break;
        default:
          break;
    }
    }
  }
  if (write_mouse_command(DISABLE_DR) != 0) return 1;
  if (unsub_mouse_int() != 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  return 0;
}



int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  uint8_t mouse_mask;
  uint32_t packet_cnt = 0;
  int y_l= 0;
  int x_l = 0;
  state_t state = INIT;
  action_t action;
  
  if (sub_mouse_int(&mouse_mask) != 0) return 1;
  if (mouse_enable_data_reporting() != 0) return 1;
  int ipc_status;
  message msg;
  while( (state != COMP) && (p.mb == 0)) { 
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_mask) {
            mouse_ih();
            if (packet_complete) {
              packet_cnt++;
              mouse_print_packet(&p);
              if ((p.lb == 1) && (p.mb == 0) && (p.rb == 0)) {
                action = LDOWN;
              }
              else if ((p.lb == 0) && (p.mb == 0) && (p.rb == 1)) {
                action = RDOWN;
              }
              else if ((p.lb == 0) && (p.mb == 0) && (p.rb == 0)) {
                action = IDL;
              }
              else {
                action = INVALID;
              }
            }
          }
          break;
        default:
          break;
    }
    }
    if (packet_complete) {
      switch (state) {
      case INIT:
        if (action == LDOWN) {
          state = DRAW_UP;
        }
        break;
      case DRAW_UP:
        if ((action == INVALID) || (action == RDOWN) || ((action == IDL) && ((x_l < x_len) || ((y_l / x_l) < 1)))) {
          x_l = 0;
          y_l = 0;
          state = INIT;
        }
        else if ( (p.delta_x < -tolerance) || (p.delta_y < -tolerance) ) {
          y_l = 0;
          x_l = 0;
        }
        else if ((x_l >= x_len) && (action == IDL) && ((y_l / x_l) >= 1)) {
          state = VERTICIE;
          x_l = 0;
          y_l = 0;
        }
        else if (action == LDOWN) {
          x_l += p.delta_x;
          y_l += p.delta_y;
        }
        break;
      case VERTICIE:
        if (action == IDL) {
          if ( !((-tolerance < p.delta_x < tolerance) && (-tolerance < p.delta_y < tolerance))) {
            state = INIT;
          }
        }
        else if (action == RDOWN) {
          state = DRAW_DOWN;
        }
        else {
          state = INIT;
        }
        break;
      case DRAW_DOWN:
        if ((action == INVALID) || (action == LDOWN) || ((action == IDL) && ((x_l < x_len) || ((-y_l / x_l) < 1)))) {
          x_l = 0;
          y_l = 0;
          state = INIT;
        }
        else if ( (p.delta_x < -tolerance)  && (p.delta_y > tolerance) ) {
          y_l = 0;
          x_l = 0;
          state = INIT;
        }
        else if ((x_l >= x_len) && (action == IDL) && ((-y_l / x_l) >= 1)) {
          state = COMP;
        }
        else if (action == RDOWN) {
          x_l += p.delta_x;
          y_l += p.delta_y;
        }
        break;
      case COMP:
        break;
    }
    }
  }
  if (write_mouse_command(DISABLE_DR) != 0) return 1;
  if (unsub_mouse_int() != 0) return 1;
  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  return 0;
}
