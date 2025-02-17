#ifndef _MACROS_H_
#define _MACROS_H_

#include <lcom/lcf.h>

#define ESC_BREAK 0x81
#define KBD_IRQ 1
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_PAR_ERR BIT(7)
#define KBC_TO_ERR BIT(6)
#define KBC_OBF BIT(0)
#define BREAK_CODE BIT(7)
#define KBC_IBF BIT(1)
#define RCB 0x20
#define WCB 0x60



#endif 
