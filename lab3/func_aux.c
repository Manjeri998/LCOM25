#include "func_aux.h"


int (aux_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = val & 0xFF;
  return 0;
}

int (aux_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = (val >> 8) & 0xFF;
  return 0;
}

int (aux_sys_inb)(int port, uint8_t *value) {
  if ( value == NULL) return 1;
  uint32_t i;
  int ret = sys_inb(port, &i);
  #ifdef LAB3
  count++;
  #endif
  *value = i & 0xFF;
  return ret;
}
