#ifndef USER_SHELL_H
#define USER_SHELL_H

#include "shell.h"
#include "stdint.h"

extern Shell shell;

extern uint8_t rxBuf[64];
extern uint8_t rxFlag;

void userShellInit(void);

#endif
