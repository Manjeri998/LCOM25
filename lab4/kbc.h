
#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>
#include "macros.h"
#include "func_aux.h"

uint8_t scan_code;

void (kbc_ih)();
int (get_scancode)();
int (kb_sub_int)(uint8_t *mask);
int (kb_unsub_int)();
int (process_and_print_data)(uint16_t data);
int (read_command_byte)(uint8_t* command);
int (write_command_byte)(uint8_t port, uint8_t command);

#endif 

