#ifndef __KEY_H
#define __KEY_H

//宏定义调换按键数量
#define KEY_COUNT					4

//用宏定义替换按键索引号
//上键
#define KEY_NAME_UP 				0	
//下键
#define KEY_NAME_DOWN 				1	
//确认键
#define KEY_NAME_COMFIRM			2	
//返回键
#define KEY_NAME_BACK 				3	


//用宏定义替换按键标志位的位掩码，使程序的意义更清晰
#define KEY_HOLD					0x01
#define KEY_DOWN					0x02
#define KEY_UP						0x04
#define KEY_SINGLE					0X08
#define KEY_DOUBLE					0x10//实际上已删除双击功能的相关代码
#define KEY_LONG					0x20
#define KEY_REPEAT					0x40

void Key_Init(void);
uint8_t Key_Check(uint8_t n, uint8_t Flag);
void Key_Tick(void);

#endif
