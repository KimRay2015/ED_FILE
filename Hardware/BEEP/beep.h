#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
#include "delay.h"

//�������˿ڶ���
#define BEEP PEout(4)	// BEEP,�������ӿ�		   

void BEEP_Init(void);	//��ʼ��
void beep(void);		 				    
#endif
