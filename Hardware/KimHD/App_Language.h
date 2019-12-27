/*
*Function:
*Programed by:Ray_DK@163.com
*Complete date:
*Modified by:
*Modified date:
*Remarks:
*/
#ifndef __APP_LANGUAGE_H_
#define __APP_LANGUAGE_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "stm32f4xx.h"

#define COM_PILE_DATE							__DATE__
#define COM_PILE_TIME							__TIME__
#define FW_VERSION								"3D Printer"
#define SW_VERSION								"����汾:V2.05.03"
#define WIFI_VERSION							"WIFI�汾:v0.1.1"
#define LASR_VERSION							"����汾:v0.1.1�����ӡ"
#define CO_TD_INFO								"������ʤ����ά�Ƽ����޹�˾"


#define	SK_NAME										"SKU-007A"


//#define MSG_AUTHOR								"Kim"
#define MSG_CONTACT								"Ray_DK@163.com"
#define MSG_COTD									"http://www.KimAuto.com.cn"

//#define __TIM2_CCRV_MOTO					//�������ʹ�ñȽ��жϽ��д�ӡ����
#define __TMC_DRIVER							//ʹ��TMC�������ģ����е������
#define __USR_SYS_CONFIG_SPEED		//ʹ��ϵͳ��Ӧ����,�û������޸Ĵ˲��ֲ���

#define USER_DEBUG_LEVEL 1

#if (USER_DEBUG_LEVEL > 0)

#define  USR_ErrLog(...)    printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
														printf("\n");
#else
#define USR_ErrorLog(...)
#endif

#if (USER_DEBUG_LEVEL > 1)
#define  USR_UsrLog(...)    printf(__VA_ARGS__);\
														printf("\n");
#else
#define USR_UsrLog(...)
#endif

#if(USER_DEBUG_LEVEL > 2)
#define  USR_DbgLog(...)    printf("DBUG: ");\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USR_DbgLog(...)
#endif

#ifdef __cplusplus
}
#endif				//End of __cplusplus

#endif				//End of files








