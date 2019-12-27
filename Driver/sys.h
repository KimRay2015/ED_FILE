#ifndef __SYS_H
#define __SYS_H	
#include "stm32f4xx.h"




	

#define BREAK_UINT32( var, ByteNum ) \
  (u8)((u32)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
  ((u32)((u32)((Byte0) & 0x00FF) \
    + ((u32)((Byte1) & 0x00FF) << 8) \
      + ((u32)((Byte2) & 0x00FF) << 16) \
        + ((u32)((Byte3) & 0x00FF) << 24)))

#define BUILD_UINT16(loByte, hiByte) \
  ((u16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define BUILD_UINT8(hiByte, loByte) \
  ((u8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)


#define	PSTR(s) ((const char*)(s))
#define true 0x1
#define false 0x00
#define bool u8
 
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
//#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))		//��������
#define square(x) x*x
//#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))		//Լ��

//0,��֧��ucos
//1,֧��ucos
#define SYSTEM_SUPPORT_UCOS		0		//����ϵͳ�ļ����Ƿ�֧��UCOS
																	    
	 
//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+0x14) 
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x14) 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x14) 
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x14) 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x14) 
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x14)  
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x14) 

#define GPIOA_IDR_Addr    (GPIOA_BASE+0x10) 
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x10) 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x10)
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x10)
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x10)
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x10)
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x10)
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#if 0
//#define DOUT 		PBin(5)   	//T_MISO
#define DOUT 		0			//T_MISO
#define TDIN 		PDout(7)  	//T_MOSI
#define TCLK 		PBout(4)  	//T_SCK
#define TCS  		PBout(9)  	//T_CS  
#define PEN			PEin(0)		//T_PEN

#define LCS			PDout(4)	//LCD_CS

#define TE			PDin(3)		//TE
#endif

#if 1    
#define DOUT 		PBin(5)   	//T_MISO
#define TDIN 		PDout(7)  	//T_MOSI
#define TCLK 		PBout(4)  	//T_SCK
#define TCS  		PBout(8)  	//T_CS   D4->B8
#define PEN			PDin(4)		//T_PEN  D3->D4

#define LCS			PBout(9)	//LCD_CS
#define LCD_DIN2    PBout(5)	//LCD_SDA2
#define TE			PEin(0)		//TE

#define BEEP		PDout(3)	//BEEP
#endif

//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ 




#endif
