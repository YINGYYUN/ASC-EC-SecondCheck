#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

//声明，方便外部调用

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFlag;//此flag不封装,参与主函数的处理

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array,uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number,uint8_t Length);
void Serial_Printf(char*format, ...);

uint8_t Serial_GetRxFlag(void);

#endif
