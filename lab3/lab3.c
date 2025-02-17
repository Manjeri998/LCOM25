#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "kbc.h"

extern int count;
extern uint16_t data;
extern uint8_t scan_code;
int size = 1;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
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
            if (scan_code == 0xE0) {
              size = 2;
            }
            else {
              if (size == 2) {
                unsigned char two_bytes[2] = {0xE0, scan_code};
                if (kbd_print_scancode(!(scan_code & BREAK_CODE), 2, two_bytes)!= 0) return 1;
                size = 1;
              }
              else {
                if (kbd_print_scancode(!(scan_code & BREAK_CODE), 1, &scan_code)!= 0) return 1;
              }
            }
          }
          break;
        default:
          break;
    }
    }
  }
  if (kb_unsub_int()!= 0) return 1;
  if (kbd_print_no_sysinb(count) != 0) return 1;
  return 0;
}

int(kbd_test_poll)() {
  count = 0;
  uint8_t command;
  while( scan_code != ESC_BREAK ) {
    if (get_scancode() == 0) {
      if (scan_code == 0xE0) {
        size = 2;
      }
      else {
        if (size == 2) {
          unsigned char two_bytes[2] = {0xE0, scan_code};
          if (kbd_print_scancode(!(scan_code & BREAK_CODE), 2, two_bytes)!= 0) return 1;
          size = 1;
        }
        else {
          if (kbd_print_scancode(!(scan_code & BREAK_CODE), 1, &scan_code)!= 0) return 1;
        }
      }
    };
  }
  kbd_print_no_sysinb(count);
  if (write_command_byte(KBC_CMD_REG, RCB) != 0) return 1;
  if (read_command_byte(&command) != 0) return 1;
  command = command | BIT(0);
  if (write_command_byte(KBC_CMD_REG ,WCB) != 0) return 1;
  if (write_command_byte(KBC_OUT_BUF, command) != 0) return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) { 
  uint8_t kbd_irq_mask, t_irq_mask;
  int time = 0;
  if (kb_sub_int(&kbd_irq_mask) != 0) return 1;
  if (timer_subscribe_int(&t_irq_mask) != 0) return 1;
  int ipc_status;
  message msg;
  while( (scan_code != ESC_BREAK ) & ((time / 60) < n)) { 
    if( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
      printf("Error");
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & kbd_irq_mask) {
            time = 0;
            kbc_ih();
            if (scan_code == 0xE0) {
              size = 2;
            }
            else {
              if (size == 2) {
                unsigned char two_bytes[2] = {0xE0, scan_code};
                if (kbd_print_scancode(!(scan_code & BREAK_CODE), 2, two_bytes)!= 0) return 1;
                size = 1;
              }
              else {
                if (kbd_print_scancode(!(scan_code & BREAK_CODE), 1, &scan_code)!= 0) return 1;
              }
            }
          }
          if (msg.m_notify.interrupts & t_irq_mask) {
            timer_int_handler();
            time++;
          }
          break;
        default:
          break;
    }
    }
  }
  if (kb_unsub_int()!= 0) return 1;
  if (timer_unsubscribe_int() != 0) return 1;
  if (kbd_print_no_sysinb(count) != 0) return 1;
  return 0;
}
