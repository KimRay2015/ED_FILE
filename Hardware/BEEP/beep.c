#include "beep.h"


//初始化PB8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能GPIOE端口时钟
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //BEEP-->PE.4 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);	 //根据参数初始化GPIOE.4
 
 GPIO_ResetBits(GPIOE,GPIO_Pin_4);//输出0，关闭蜂鸣器输出


}
void beep(void)
{	BEEP=1;  
	delay_ms(3);
	BEEP=0;	
	delay_ms(3);
}

