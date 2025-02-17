#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "kbc.h"
#include "macros.h"

bool packet_complete;
struct packet p;

int (sub_mouse_int)(uint8_t* mask);
int (unsub_mouse_int)();
void (mouse_ih)();
int (write_mouse_command)(uint8_t command);
int (read_packet_byte)();
int (read_output)(uint8_t* response);

#endif 
