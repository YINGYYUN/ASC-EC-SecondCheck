#ifndef __KEY_H
#define __KEY_H

extern uint8_t Menu_State;

void Key_Init(void);
uint8_t Key_GetNum(void);
void Key_Tick(void);

#endif
