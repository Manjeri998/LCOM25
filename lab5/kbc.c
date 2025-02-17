#include "kbc.h"

int kb_hook_id = 1;


void(kbc_ih)() {
  if (get_scancode() != 0) printf("ERROR");
}

int (get_scancode)() {
  uint8_t status;
  int trys = 10;
  while(trys > 0) {
    aux_sys_inb(KBC_ST_REG, &status); 
    if( status & KBC_OBF ) {
      if (aux_sys_inb(KBC_OUT_BUF, &scan_code) == OK); 
      if ( (status &(KBC_PAR_ERR | KBC_TO_ERR)) == 0 )
        return 0;
      else
        return 1;
    }
  tickdelay(micros_to_ticks(20000)); 
  trys--;
  }
  return 1;
}


int (kb_sub_int)(uint8_t *mask) {
  if (mask == NULL) return 1;
  *mask = BIT(kb_hook_id);
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &kb_hook_id) != 0) return 1;
  return 0;
}
int (kb_unsub_int)() {
  if (sys_irqrmpolicy(&kb_hook_id) != 0) return 1;
  return 0;
}
int (process_and_print_data)(uint16_t data) {
  return 1;
}
int (keyboard_restore)(uint8_t command) {
  return 1;
}

int (write_command_byte)(uint8_t port, uint8_t command) {
  uint8_t status;
  int trys = 10;
  while( trys > 0 ) {
    if(aux_sys_inb(KBC_ST_REG, &status) != 0) return 1; 
    if( (status & KBC_IBF) == 0 ) {
      if (sys_outb(port, command) != 0) return 1; 
      return 0;
    }
    tickdelay(micros_to_ticks(20000)); 
    trys--;
  }
  return 1;
}

int (read_command_byte)(uint8_t* command) {
  uint8_t status;
  int trys = 10;
  while(trys > 0) {
    aux_sys_inb(KBC_ST_REG, &status); 
    if( status & KBC_OBF ) {
      if (aux_sys_inb(KBC_OUT_BUF, command) == OK); 
      if ( (status &(KBC_PAR_ERR | KBC_TO_ERR)) == 0 ) {
        return 0;
      }
      else
        return 1;
    }
  tickdelay(micros_to_ticks(20000)); 
  trys--;
  }
  return 1;
}
