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

//#define CHINESE 0   /*  ����������ʾ      */
//#define ENGLISH 1   /*  ����Ӣ����ʾ      */
typedef void (*menuFunc_t)();
typedef struct{
	u8 wifi_stat;	//wifi״̬ 0���� 1���ӵ��޻����� 2δ���� 3δ���ֻ�
	u8 door_stat;	//��ӡ������״̬ 0�ر� 1��
//	u8 filament_stat;	//����λ������������״̬ 0���� 1ȱ��
//	u8 print_present;	//��ӡ�ٷֱ�
//	u8 print_stat;		//��ӡ״̬ 0δ��ӡ 1��ӡ�� 2��ӡ���
//	u32 print_time;		//��ӡʱ��
	u8 model_stat;		//��ӡģ��״̬ 0�װ���û��⵽��ģ�ʹ��� 1��ģ�ʹ���
}CHILD_DATA;
extern CHILD_DATA ChildData;
extern bool windows_flag;
extern menuFunc_t CurrentMenu;
extern menuFunc_t tempMenu;
extern bool redraw_menu;

extern float speed_temprature_factor;

#define MAX_GCODE_NUM 100
//�ļ�����
typedef enum{
    GCODE = 0,
    FOLDER,
}file_type_enum;
//�����ļ��Ľṹ�� gcode �� �ļ���
typedef struct {
    file_type_enum file_type;//true Gcode  false �ļ���
	u8  file_index;//�ļ�����
}gcodefile_s;
extern gcodefile_s gcodefile_t[100];



extern u8 gcodefile_index_base;
extern u8 gcodefile_index_sub;
extern gcodefile_s gcodefile_t[100];
#define EPSINON 0.00001
//#define FAN3_SHOW //����3��ʾ��
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

