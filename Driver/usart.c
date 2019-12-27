/*
*Function:
*Programed by:Ray_DK@163.com
*Complete date:
*Modified by:
*Modified date:
*Version:
*Remarks:
*/

#include "sys.h"
#include "usart.h"	
#include "pins.h"
#include "ConfigurationStore.h"

#include "bsp.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{   
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET){};//�ȴ����ͽ��� 
	USART1->DR = (ch & (uint16_t)0x01FF);
		return ch;
	
}
#endif  

/*USART Receive Data Buffer*/
ring_buffer rx_buffer  =  { { 0 }, 0, 0 };
ring_buffer rx_buffer2 = { { 0 }, 0, 0 };
void store_char(unsigned char c)
{
  int i = (unsigned int)(rx_buffer.head + 1) % RX_BUFFER_SIZE;
  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != rx_buffer.tail) 
	{
    rx_buffer.buffer[rx_buffer.head] = c;
    rx_buffer.head = i;
  }
}
void store_char2(unsigned char c)
{
  int i = (unsigned int)(rx_buffer2.head + 1) % RX_BUFFER_SIZE;

  if (i != rx_buffer2.tail) 
	{
    rx_buffer2.buffer[rx_buffer2.head] = c;
    rx_buffer2.head = i;
  }
}

void checkRx(void)
{ unsigned char c;
	unsigned int i;
      if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	    { c =USART_ReceiveData(USART1);
         i = (unsigned int)(rx_buffer.head + 1) % RX_BUFFER_SIZE;
        // if we should be storing the received character into the location
        // just before the tail (meaning that the head would advance to the
        // current location of the tail), we're about to overflow the buffer
        // and so we don't write the character or advance the head.
        if (i != rx_buffer.tail) 
		    {
          rx_buffer.buffer[rx_buffer.head] = c;
          rx_buffer.head = i;
        }
      }
}
unsigned int MYSERIAL_available(void)
{
   return (unsigned int)(RX_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % RX_BUFFER_SIZE;
}
unsigned int MYSERIAL_available2(void)
{
   return (unsigned int)(RX_BUFFER_SIZE + rx_buffer2.head - rx_buffer2.tail) % RX_BUFFER_SIZE;
}

u8 MYSERIAL_read(void)
{ u8 c;
  // if the head isn't ahead of the tail, we don't have any characters
  if (rx_buffer.head == rx_buffer.tail) 
	{
    return 0;
  } 
	else 
	{
     c = rx_buffer.buffer[rx_buffer.tail];
    rx_buffer.tail = (unsigned int)(rx_buffer.tail + 1) % RX_BUFFER_SIZE;
    return c;
  }
}
u8 MYSERIAL_read2(void)
{ 
	u8 c;
	if (rx_buffer2.head == rx_buffer2.tail) 
	{
		return 0;
	} 
	else 
	{
		c = rx_buffer2.buffer[rx_buffer2.tail];
		rx_buffer2.tail = (unsigned int)(rx_buffer2.tail + 1) % RX_BUFFER_SIZE;
		return c;
	}
}

void MYSERIAL_flush(void)
{
  // don't reverse this or there may be problems if the RX interrupt
  // occurs after reading the value of rx_buffer_head but before writing
  // the value to rx_buffer_tail; the previous value of rx_buffer_head
  // may be written to rx_buffer_tail, making it appear as if the buffer
  // don't reverse this or there may be problems if the RX interrupt
  // occurs after reading the value of rx_buffer_head but before writing
  // the value to rx_buffer_tail; the previous value of rx_buffer_head
  // may be written to rx_buffer_tail, making it appear as if the buffer
  // were full, not empty.
  rx_buffer.head = rx_buffer.tail;
}		


void checkDTR(void)
{
	static bool DTR_state = true;
	static bool old_DTR_state = true;
	FunctionCode = FUN_7;
//	 DTR_state = UART_DTR_PIN;
	if(!DTR_state && old_DTR_state) 
	{
		Config_PrintSettings();
	}
  old_DTR_state = DTR_state;
}

//��ʼ��IO ����1 
//bound:������
void uart1_init(u32 bound)
{
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	#if EN_USART1_RX		  //���ʹ���˽���  
	NVIC_InitTypeDef NVIC_InitStructure;
	#endif
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);		//ʹ��USART1��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	USART_DeInit(USART1);  //��λ����1

 //USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 		//PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//���츴����� 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 			//��ʼ��PA9
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10


	//���Ÿ�����ӳ������
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
 //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������
#if EN_USART1_RX		  //���ʹ���˽���  
   //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
#endif
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

//�����ַ���
void usart1_send(u8* str)
{
	int i=0;
	while(str[i]!='\0' && i<512)
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    	//USART_SendData(USART1,(uint8_t)str[i++]);
		USART1->DR = ((uint8_t)str[i++] & (uint16_t)0x01FF);
	}
}

#if EN_USART1_RX   //���ʹ���˽���
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
//	s8 a,b,c;
//	static u8 rxcount=0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d //0x0a��β)
	{
		Res = (uint16_t)(USART1->DR & (uint16_t)0x01FF);//USART_ReceiveData(USART1);//(USART2->DR);	//��ȡ���յ�������
		store_char(Res);
		/*
		if(Res!='\r' && rxcount<49)
		{
			rxbuff[rxcount++] = Res;
		}
		else if(Res=='\r')	//������������һ������
		{
			a = memcmp(rxbuff,"M4000",5);
			b = memcmp(rxbuff,"M4001",5);
			c = memcmp(rxbuff,"M4004",5);
			if((a==0)||(b==0)||(c==0))
			{
				;
			}
			else
			{
				rxbuff[rxcount] = ':';
				rxbuff[rxcount] = 'O';
				rxbuff[rxcount] = 'K';
				rxbuff[rxcount] = '\r';
				rxbuff[rxcount] = '\n';
				usart1_send(rxbuff);
			}
			memset(rxbuff,0,sizeof(rxbuff));
		}
		*/
	} 

} 

#endif	

//��ʼ��IO ����2 
//bound:������
void uart2_init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);		//ʹ��USART1��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	USART_DeInit(USART2);  //��λ����1
	//���Ÿ�����ӳ������
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2);
	 //USART2_TX   PD.5 PD.6
	
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 	//���츴����� 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//���ù���
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 		
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

   //USART2 ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������
   //Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}


void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d //0x0a��β)
	{
		Res = (uint16_t)(USART2->DR & (uint16_t)0x01FF);//USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
		store_char2(Res);  		 
  } 
} 

//�����ַ���
void usart2_send(char* str)
{
	int i=0;
	while(str[i]!='\0' && i<512)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 
    	//USART_SendData(USART2,(uint8_t)str[i++]);
		USART2->DR = ((uint8_t)str[i++] & (uint16_t)0x01FF);
		
	}
}



