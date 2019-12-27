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
#define SW_VERSION								"软件版本:V2.05.03"
#define WIFI_VERSION							"WIFI版本:v0.1.1"
#define LASR_VERSION							"软件版本:v0.1.1激光打印"
#define CO_TD_INFO								"深圳市胜马三维科技有限公司"


#define	SK_NAME										"SKU-007A"


//#define MSG_AUTHOR								"Kim"
#define MSG_CONTACT								"Ray_DK@163.com"
#define MSG_COTD									"http://www.KimAuto.com.cn"

//#define __TIM2_CCRV_MOTO					//步进电机使用比较中断进行打印处理
#define __TMC_DRIVER							//使用TMC电机驱动模块进行电机驱动
#define __USR_SYS_CONFIG_SPEED		//使用系统相应参数,用户不能修改此部分参数

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








