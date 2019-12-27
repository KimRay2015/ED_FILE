#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 





//#define USART_REC_LEN  			256  	//定义最大接收字节数 256
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

#define RX_BUFFER_SIZE 128
typedef struct ring_buffer
{
  unsigned char buffer[RX_BUFFER_SIZE];
  int head;
  int tail;
}ring_buffer;
extern  ring_buffer rx_buffer;	
extern  ring_buffer rx_buffer2;	

//extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义

void checkDTR(void);
void uart1_init(u32 bound);
void checkRx(void);
unsigned int MYSERIAL_available(void);
unsigned int MYSERIAL_available2(void);

u8 MYSERIAL_read(void);
u8 MYSERIAL_read2(void);

void MYSERIAL_flush(void);

void usart1_send(u8* str);
void usart2_send(char* str);

void uart2_init(u32 bound);

#endif


