#ifndef __CHILDREN_MENU_H__
#define __CHILDREN_MENU_H__

#include "touch.h"
#include "guix.h"
#include "gui.h"
#include "piclib.h"
#include "temperature.h"
#include "gcodeplayer.h"
#include "button.h"
#include "filelistbox.h"
#include "gcodeplayer.h"
#include "stepper.h"

//#define CHINESE 0   /*  界面中文显示      */
//#define ENGLISH 1   /*  界面英文显示      */
typedef void (*menuFunc_t)();
typedef struct{
	u8 wifi_stat;	//wifi状态 0正常 1连接但无互联网 2未连接 3未绑定手机
	u8 door_stat;	//打印机舱门状态 0关闭 1打开
//	u8 filament_stat;	//低三位代表三个料盘状态 0正常 1缺料
//	u8 print_present;	//打印百分比
//	u8 print_stat;		//打印状态 0未打印 1打印中 2打印完成
//	u32 print_time;		//打印时间
	u8 model_stat;		//打印模型状态 0底板上没检测到有模型存在 1有模型存在
}CHILD_DATA;
extern CHILD_DATA ChildData;
extern bool windows_flag;
extern menuFunc_t CurrentMenu;
extern menuFunc_t tempMenu;
extern bool redraw_menu;

extern float speed_temprature_factor;

#define MAX_GCODE_NUM 100
//文件类型
typedef enum{
    GCODE = 0,
    FOLDER,
}file_type_enum;
//可用文件的结构体 gcode 和 文件夹
typedef struct {
    file_type_enum file_type;//true Gcode  false 文件夹
	u8  file_index;//文件索引
}gcodefile_s;
extern gcodefile_s gcodefile_t[100];



extern u8 gcodefile_index_base;
extern u8 gcodefile_index_sub;
extern gcodefile_s gcodefile_t[100];
#define EPSINON 0.00001
//#define FAN3_SHOW //风扇3显示打开
void lcd_update(void);
void start_menu(void);
void main_menu(void);
void set_menu(void);
void wifi_menu(void);
void filamentstep1_menu(void);
void filamentstep2_menu(void);
void filamentstep3_menu(void);
void filamentlaststep_menu(void);
void language_menu(void);
void about_menu(void);
void adjscreen_menu(void);
void print_menu(void);
void filament_error(void);
void door_error(void);
void extruder_cool(void);
void printcomplete_menu(void);
void wifi2_menu(void);
void progress_bar(u16 sx,u16 sy,u8 progress);
void model_check(void);
void popout_screen(void);
void stop_print(void);
void QRSelect_menu(void);
void reprint_menu(void);

void reprint_menu(void);


void start_diy(void);
void main_diy(void);
void set_diy(void);
void control_3d_diy(void);
void control_laser_diy(void);
void preheat_diy(void);
void move1_diy(void);
void zero_diy(void);
void extruder_diy(void);
void leveling_diy(void);
void fan_diy(void);
void about_diy(void);
void language_diy(void);
void status_diy(void);
void wifi_diy(void);
void print_contrl_diy(void);
void speed_diy(void);
void gecodelist_diy(void);
void getgcodetab_diy(void);
void print_diy(void);
void stop_diy(void);
void demo_diy(void);
void manual_stop_diy(void);
void extrudemultiply_diy(void);
u8 check_sd_allfile(u8 *fp);
void readindex_file(u8 file_index_,u8* file_name,u8 *fp);

#endif

