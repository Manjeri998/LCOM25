#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int time_h_id=0;
int counter=0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq < 19 || freq > 1193182) return 1;
  uint8_t con;
  if (timer_get_conf(timer, &con) != 0) return 0;
  con = (con & 0x0F) | (BIT(4) | BIT(5));
  if (timer == 1) {
    con = con | BIT(6);
  }
  else if (timer == 2) {
    con = con | BIT(7);
  }
  uint32_t n = 1193182 / freq;
  uint8_t MSB, LSB;
  if (util_get_LSB(n, &LSB) != 0) return 1;
  if (util_get_MSB(n, &MSB) != 0) return 1;
  if (sys_outb(0x43, con) != 0) return 1;
  if (sys_outb(0x40 + timer, LSB) != 0) return 1;
  if (sys_outb(0x40 + timer, MSB) != 0) return 1;
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(time_h_id);
  if (sys_irqsetpolicy(0, IRQ_REENABLE, &time_h_id) != 0) return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
   if (sys_irqrmpolicy(&time_h_id) != 0) return 1; 
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (st == NULL) return 1;
  uint8_t Command = BIT(7)| BIT(6)| BIT(5)|BIT(timer + 1);
  if (sys_outb(0x43,Command) != 0) return 1;
  if (util_sys_inb(0x40 + timer, st) != 0) return 1;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val v;
  switch(field) {
    case tsf_all:
      v.byte = st;  
      break;
    case tsf_initial:
      st = (st >> 4) & 0x03;
      switch ( st) {
        case 1:
          v.in_mode = LSB_only;
          break;
        case 2:
          v.in_mode = MSB_only;
          break;
        case 3:
          v.in_mode = MSB_after_LSB;
          break;
        default:
          v.in_mode = INVAL_val;
          break;
      }
      break;
    case tsf_base:
      v.bcd = st & 0x01;
      break;
    case tsf_mode:
      st = (st >> 1) & 0x07;
      if (st == 6) v.count_mode = 2;
      else if (st == 7) v.count_mode = 3;
      else v.count_mode = st;
  }

  if (timer_print_config(timer, field, v) != 0) return 1;
  return 0;
}
