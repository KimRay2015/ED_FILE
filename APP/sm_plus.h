#ifndef __SM_PLUS_H__
#define __SM_PLUS_H__

#include "main.h"
#include "usart.h"
#include "exfuns.h"
#include "sys.h"
#include "delay.h"	
#include "adc.h"
#include "24cxx.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "ff.h"
#include "exfuns.h"
#include "sm_firmware.h" 
#include "Configuration.h"


#include "lcd.h"
#include "piclib.h"	
//#include "lcdmenu.h"
#include "children_menu.h"
#include "text.h"
#include "guix.h"

#include <stdio.h>
#include <stdlib.h>

#include "quicklz.h"
#include "bsp_spi_flash.h"

//二维码生成依赖文件
#include "qrencode.h"
#include "qrspec.h"
#include "mqrspec.h"
#include "bitstream.h"
#include "qrinput.h"
#include "rsecc.h"
#include "split.h"
#include "mask.h"
#include "mmask.h"


typedef struct
{
	u8 printsd;		//0空闲 1正在打印 2打印暂停 3准备打印
	u8 filament;	//bit0~bit3 分别代表料盘1,2,3；其中0代表料盘正常 1代表缺料
	u8 print_prepare;	//准备百分比
	u8 printfile[50];	//当前打印文件名 文件名最长10字节
	u8 printper;	//打印百分比
	u32 filesize;	//文件总大小
	u32 printtime;	//打印时间
}PRINTINFO;


extern PRINTINFO PrintInfo;
extern u32 ReadChar;
extern u32 CPUID[3];
extern u32 sm_version;
extern u32 wifi_version;
extern u8 sendFlg;
extern float A,B,C,D;
extern float x[3];
extern float y[3];
extern float z[3];

extern u8 INVERT_X_DIR;
extern u8 INVERT_Y_DIR;
extern u8 INVERT_Z_DIR;
extern u8 INVERT_E0_DIR;
extern u8 INVERT_E1_DIR;
extern u8 INVERT_E2_DIR;

extern u8 X_ENDSTOPS_INVERTING;
extern u8 Y_ENDSTOPS_INVERTING;
extern u8 Z_ENDSTOPS_INVERTING;
extern u8 U_ENDSTOPS_INVERTING;
extern u8 V_ENDSTOPS_INVERTING;
extern u8 W_ENDSTOPS_INVERTING;

extern u8 BoardCheckFlg;
extern u8 FilamentLoadFlg;
extern u8 FilamentUnloadFlg;

extern u8 LeveingComplete;
extern FIL logtext;
extern float ParaVersion;

void update_icon(void);
void qlzdecompressed(u8* name);
void qlzcompress(u8* path);
void printinfo_update(void);
void printinfo_init(void);
void equation_plan(float *x,float *y,float *z);
void qrencode_test(u8* str);
void qrencode_display(const u8* string,u16 sx,u16 sy,u8 mode);



#define BLTOUCH_UD TIM1->CCR1


#define BL_UP	15
#define BL_DOWN	6


//#define BLTOUCH_LIMIT PDin(11)
#define BLTOUCH_LIMIT PBin(1)


extern u8 old_block_buffer_head;
extern long e_steps_count;
extern long z_setps_count;


extern float show_extruder_temperature[2];
extern float show_bed_temperature;

extern u32 timer_now,timer_old;


void show_temperature(void);


void bl_touch_init(void);
void copyfiles(void);
void ls_spi_icon(void);
void copy_font(void);
void filament_lack(void);
void sd_check(void);

void laser_init(void);

void copy_diy_icons(void);
void ls_spi_diy_icon(void);


//#endif	//#ifdef AUTO_LEVELING
#if 1
enum {
	RDCONFIG_OK = 0,
	NON_CONFIGFILE, //文件不存在
	FILE_MISMATCH,//文件不匹配
	PARAM_NONALL//配置文件参数不全
};

typedef struct {
	char 			*name;		//参数名
	unsigned char	para_Num;	//个数
	float  		    Float_Data[4];
} Para_T;

u16 ConfigurationWrite2EEPROM(void);
void save_configfile(void);


#endif


#endif



