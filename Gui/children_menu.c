#include "children_menu.h"
#include "sm_plus.h"
#include "guix.h"
#include <math.h>
#include "bsp.h"
#include "stdlib.h"
#include "App_Language.h"
#include "App_Timer.h"
#include "ffconf.h"
#include "ff.h"

extern USR_TIMER usr_timer;

static uint32_t tp_scan_next_refresh_millis=0;
static uint32_t display_next_refresh_millis=0;
static uint32_t delay_time_millis=0;
//static u8   language_flag = CHINESE;
#if MACHINE_MODE==1
menuFunc_t CurrentMenu = start_menu;
menuFunc_t lastMenu=start_menu;
menuFunc_t tempMenu=start_menu;
#elif MACHINE_MODE==2
menuFunc_t CurrentMenu = start_diy;
menuFunc_t lastMenu=start_diy;
menuFunc_t tempMenu=start_diy;
#endif

bool redraw_menu = TRUE;
_btn_obj **key_group;

CHILD_DATA ChildData;
static uint32_t tp_scan_next_update_millis=0;
static uint32_t display_next_update_millis=0;
u8* PrintPoint=NULL;		 //弹框显示文字指针
bool windows_flag = FALSE;

float speed_temprature_factor;//温度 速度对应参数 target_tempratue = target_tempratue*speed_temprature_factor*(feedmultiply-100)/100+target_tempratue;

extern block_t *current_block;  // A pointer to the block currently being traced
//访问SD卡中各个目录下文件
#define GCODE_FILE_SHOW_NUM 6
#define FILE_BASE "0:"
#define OK        0
#define CANT_DEEP 1

bool    file_path_status = OK;
u8 file_name_buf[6][100];
u8 gcodefile_index_base = 0;
u8 gcodefile_index_sub  = 0;
gcodefile_s gcodefile_t[100];
u32 gcodefile_index = 0;
u8 gcodefile_path[100] = "0:";//屏幕显示文件路径
bool btnshow_change_flag = FALSE;
u8 gcodefile_num = 0;

//===================ADD BY KIM=============================
uint32_t CurrentFileSize = 0,CurrentReadSize = 0;
//==========================================================


u8* BMP_START=		"1:/child_icon/000.bmp";	//欢迎图片
u8* BMP_XHM=		"1:/child_icon/001.bmp";	//小海马
u8* BMP_WIFI_A=		"1:/child_icon/002.bmp";	//WIFI ok
u8* BMP_WIFI_B=		"1:/child_icon/004.bmp";	//WIFI 没网
u8* BMP_WIFI_C=		"1:/child_icon/003.bmp";	//WIFI 没连接
u8* BMP_CM_K=		"1:/child_icon/005.bmp";	//舱门开
u8* BMP_CM_G=		"1:/child_icon/006.bmp";	//舱门关
u8* BMP_SZ=			"1:/child_icon/007.bmp";	//设置
u8* BMP_SZ_WIFI=	"1:/child_icon/008.bmp";	//wifi设置
u8* BMP_SZ_CL=		"1:/child_icon/009.bmp";	//更换打印材料
u8* BMP_SZ_YY=		"1:/child_icon/0010.bmp";	//语言切换
u8* BMP_SZ_JZ=		"1:/child_icon/0011.bmp";	//屏幕校准
u8* BMP_SZ_XX=		"1:/child_icon/0012.bmp";	//机器信息
u8* BMP_SZ_FH=		"1:/child_icon/0013.bmp";	//返回
u8* BMP_CZMM=		"1:/child_icon/0021.bmp";	//重置wifi密码
u8* BMP_DY_ZT=		"1:/child_icon/0031.bmp";	//打印暂停
u8* BMP_DY_TZ=		"1:/child_icon/0032.bmp";	//打印停止
u8* BMP_DY_HF=		"1:/child_icon/0033.bmp";	//打印恢复
u8* BMP_DY_KCM=		"1:/child_icon/0034.bmp";	//打印过程开舱门提醒
u8* BMP_DY_FR=		"1:/child_icon/0035.bmp";	//线材耗尽
u8* BMP_DY_DR=		"1:/child_icon/0036.bmp";	//打印时舱门打开
u8* BMP_DY_LP1=		"1:/child_icon/0037.bmp";	//料盘1红亮
u8* BMP_DY_LP2=		"1:/child_icon/0038.bmp";	//料盘2红亮
u8* BMP_DY_LP3=		"1:/child_icon/0039.bmp";	//料盘3红亮
u8* BMP_DY_FQDY=	"1:/child_icon/0040.bmp";	//放弃打印
u8* BMP_DY_GHLP=	"1:/child_icon/0041.bmp";	//更换料盘
u8* BMP_DY_DYWC=	"1:/child_icon/0042.bmp";	//打印完成提示图片
u8* BMP_DY_DYLQ=	"1:/child_icon/0043.bmp";	//打印完成冷却
u8* BMP_DY_FH=		"1:/child_icon/0044.bmp";	//返回
u8* BMP_DY_QRQC=	"1:/child_icon/0044.bmp";	//确认取出
u8* BMP_GHHC_LP1A=	"1:/child_icon/0050.bmp";	//料盘1号未选中
u8* BMP_GHHC_LP1B=	"1:/child_icon/0051.bmp";	//料盘1号选中
u8* BMP_GHHC_LP2A=	"1:/child_icon/0052.bmp";	//料盘2号未选中
u8* BMP_GHHC_LP2B=	"1:/child_icon/0053.bmp";	//料盘2号选中
u8* BMP_GHHC_LP3A=	"1:/child_icon/0054.bmp";	//料盘3号未选中
u8* BMP_GHHC_LP3B=	"1:/child_icon/0055.bmp";	//料盘3号选中
u8* BMP_GHHC_FH=	"1:/child_icon/0057.bmp";	//返回
u8* BMP_GHHC_XYB=	"1:/child_icon/0058.bmp";	//下一步
u8* BMP_GHHC_TC=	"1:/child_icon/0059.bmp";	//退出打印材料
u8* BMP_GHHC_CR=	"1:/child_icon/0060.bmp";	//插入打印材料
u8* BMP_GHHC_SYT1=	"1:/child_icon/0061.bmp";	//插入示意图1
u8* BMP_GHHC_SYT2=	"1:/child_icon/0062.bmp";	//插入示意图2
u8* BMP_GHHC_JL=	"1:/child_icon/0063.bmp";	//正在挤料


u8* BMP_DY_LP1G=	"1:/child_icon/0070.bmp";	//料盘1正常
u8* BMP_DY_LP2G=	"1:/child_icon/0071.bmp";	//料盘2正常
u8* BMP_DY_LP3G=	"1:/child_icon/0072.bmp";	//料盘3正常
u8* BMP_WIFI_LJ=	"1:/child_icon/0080.bmp";	//链接wifi
u8* BMP_WIFI_BD=	"1:/child_icon/0081.bmp";	//绑定打印机
u8* BMP_SLT=		"1:/child_icon/0090.bmp";	//打印缩略图






u8* TEXT_A= "         打印机舱门关闭       ";
u8* TEXT_B= "         打印机舱门打开       ";
u8* TEXT_C= "         网络连接正常！       ";
u8* TEXT_D= "  无法访问互联网，请检查网络！";
u8* TEXT_E= "请在设置中重置密码或检查网络！";
u8* TEXT_F= "         打印机舱门开启       ";
u8* TEXT_G= "         请手动关闭舱门       ";
u8* TEXT_H= "打印中。。。。";
u8* TEXT_I= "打印暂停。。。";
u8* TEXT_J= "确认停止当前打印？";
u8* TEXT_K= "请确认已经将材料插入图示位置至少x厘米";
u8* TEXT_L= "海马3D打印机";

u8* TEXT_VERSION = "VERSION:0.3.9";
/*
VERSION:0.3.8 : 将儿童打印机和diy打印工程融合到一起
VERSION:0.3.9 : 更新通信协议

*/
void gecodelist_screen(void);

u8 PrinterMode = 0;	//打印机类型 0:3d打印 1：激光打印

void lcd_update(void)
{
	FunctionCode = FUN_8;
	if(tp_scan_next_refresh_millis < millis())
	{
	  tp_dev.scan(0);
	  in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);
	  tp_scan_next_refresh_millis = millis() + 10;
	  if(TE==0)
	  {
	  	(*CurrentMenu)();
	  }
	}
}

void start_menu(void)
{
	
	if(ChildData.wifi_stat==2)//未检测到wifi连接状态 进入到wifi连接界面
	{
		CurrentMenu = wifi2_menu;
		redraw_menu = TRUE;
		return;
	}
	ChildData.wifi_stat = 2;
	//加载开机欢迎界面 显示2s
	
	ai_load_picfile(BMP_START,0,0,320,240,1);
	u8 i;
	for(i=0;i<20;i++)
	{
		delay_ms(100);
	}
	//进入主菜单界面
	CurrentMenu = main_menu;
	redraw_menu = TRUE;
}
static CHILD_DATA oldChildData={0,0};
void main_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 3;

	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//memcpy(oldChildData,ChildData,sizeof(CHILD_DATA));
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//显示小海马
		minibmp_decode(BMP_XHM,108,20,91,118,0,0);
		display_next_refresh_millis = millis();
		//显示版本号
		gui_show_strmid(20,5,104,16,GOLDEN,16,TEXT_VERSION,0);
		//显示三个图标 wifi图标和舱门图标根据当前状态显示
		
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
     	key_group[0]=btn_creat(20,185,76,44,0,1);
			key_group[1]=btn_creat(120,185,76,44,1,1);
			key_group[2]=btn_creat(220,185,76,44,2,1);

			if(ChildData.wifi_stat==1)	//wifi信号判断
			{
				key_group[0]->picbtnpathu=(u8*)BMP_WIFI_B;
				key_group[0]->picbtnpathd=(u8*)BMP_WIFI_B;
			}
			else if(ChildData.wifi_stat==2)
			{
				key_group[0]->picbtnpathu=(u8*)BMP_WIFI_C;
				key_group[0]->picbtnpathd=(u8*)BMP_WIFI_C;
			}
			else
			{
				key_group[0]->picbtnpathu=(u8*)BMP_WIFI_A;
				key_group[0]->picbtnpathd=(u8*)BMP_WIFI_A;
			}

			if(ChildData.door_stat==0)
			{
				key_group[1]->picbtnpathu=(u8*)BMP_CM_G;
				key_group[1]->picbtnpathd=(u8*)BMP_CM_G;
				gui_show_strmid(20,160,280,16,GOLDEN,16,TEXT_A,0);
			}
			else
			{
				key_group[1]->picbtnpathu=(u8*)BMP_CM_K;
				key_group[1]->picbtnpathd=(u8*)BMP_CM_K;
				gui_show_strmid(20,160,280,16,GOLDEN,16,TEXT_B,0);
			}
			key_group[2]->picbtnpathu=(u8*)BMP_SZ;
			key_group[2]->picbtnpathd=(u8*)BMP_SZ;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
	}
	if(display_next_refresh_millis<millis())	//更新wifi图标状态和舱门图标状态
	{
		display_next_refresh_millis = millis() + 500;
		//延时显示wifi时间检测
		if(delay_time_millis>0)
		{
			if((millis()-delay_time_millis) > 3000)
			{
				delay_time_millis = 0;
				gui_draw_rectangle(20,160,280,16,gui_phy.back_color);
				if(oldChildData.door_stat)
				{
					gui_show_strmid(20,160,280,16,GOLDEN,16,TEXT_B,0);
				}
				else
				{
					gui_show_strmid(20,160,280,16,GOLDEN,16,TEXT_A,0);
				}
			}
		}
		//舱门状态检测
		if(oldChildData.door_stat != ChildData.door_stat)
		{
			oldChildData.door_stat = ChildData.door_stat;
			if(oldChildData.door_stat)
			{
				key_group[1]->picbtnpathu=(u8*)BMP_CM_K;
				key_group[1]->picbtnpathd=(u8*)BMP_CM_K;
				gui_show_strmid(20,160,280,16,GOLDEN,16,TEXT_B,0);
			}
			else
			{
				key_group[1]->picbtnpathu=(u8*)BMP_CM_G;
				key_group[1]->picbtnpathd=(u8*)BMP_CM_G;
				gui_show_strmid(20,160,280,16,GOLDEN,16,TEXT_A,0);
			}
			btn_draw(key_group[1]);
		}

		//wifi状态检测
		if(oldChildData.wifi_stat != ChildData.wifi_stat)
		{
			oldChildData.wifi_stat = ChildData.wifi_stat;
			if(oldChildData.wifi_stat==0)
			{
				key_group[0]->picbtnpathu=(u8*)BMP_WIFI_A;
				key_group[0]->picbtnpathd=(u8*)BMP_WIFI_A;
			}
			else if(oldChildData.wifi_stat==1)
			{
				key_group[0]->picbtnpathu=(u8*)BMP_WIFI_B;
				key_group[0]->picbtnpathd=(u8*)BMP_WIFI_B;
			}
			else
			{
				key_group[0]->picbtnpathu=(u8*)BMP_WIFI_C;
				key_group[0]->picbtnpathd=(u8*)BMP_WIFI_C;
			}
			btn_draw(key_group[0]);
		}			
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	gui_draw_rectangle(20,160,280,16,gui_phy.back_color);
			delay_time_millis = millis();
		  	if(oldChildData.wifi_stat==0)
		  	{
				gui_show_strmid(20,160,280,16,GREEN,16,TEXT_C,0);
			}
			else if(oldChildData.wifi_stat==1)
			{
				gui_show_strmid(20,160,280,16,RED,16,TEXT_D,0);
			}
			else if(oldChildData.wifi_stat==2)
			{
				gui_show_strmid(20,160,280,16,RED,16,TEXT_E,0);
			}
		  break;
		  case 1:
		  	gui_draw_rectangle(20,160,280,16,gui_phy.back_color);
			delay_time_millis = millis();
		  	usart2_send("M4004\r\n");	//开启状态
		  	gui_show_strmid(20,160,280,16,RED,16,TEXT_F,0);		
		  	/*
		  	if(oldChildData.door_stat==0)
		  	{
				//发送舱门打开指令

				gui_show_strmid(20,160,280,16,RED,16,TEXT_F,0);				
			}
			else
			{
				gui_show_strmid(20,160,280,16,RED,16,TEXT_G,0);
			}
			*/
		  break;
		 case 2:
		 	redraw_menu = TRUE;
			CurrentMenu = set_menu;		//设置界面
		 	break;
		  default: break;
		}
	}

	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
	
}

void set_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 6;
	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
     		key_group[0]=btn_creat(5,5,42,34,0,1);			//返回按钮
			key_group[1]=btn_creat(15,40,168,99,1,1);		//wifi设置
			key_group[2]=btn_creat(190,40,112,99,2,1);		//更换打印材料
			key_group[3]=btn_creat(15,145,92,88,3,1);		//语言切换
			key_group[4]=btn_creat(115,145,92,89,4,1);		//屏幕校准
			key_group[5]=btn_creat(214,145,91,88,5,1);		//机器信息
				
			key_group[0]->picbtnpathd = (u8*)BMP_SZ_FH;
			key_group[0]->picbtnpathu = (u8*)BMP_SZ_FH;
			key_group[1]->picbtnpathd = (u8*)BMP_SZ_WIFI;
			key_group[1]->picbtnpathu = (u8*)BMP_SZ_WIFI;
			key_group[2]->picbtnpathd = (u8*)BMP_SZ_CL;
			key_group[2]->picbtnpathu = (u8*)BMP_SZ_CL;
			key_group[3]->picbtnpathd = (u8*)BMP_SZ_YY;
			key_group[3]->picbtnpathu = (u8*)BMP_SZ_YY;
			key_group[4]->picbtnpathd = (u8*)BMP_SZ_JZ;
			key_group[4]->picbtnpathu = (u8*)BMP_SZ_JZ;
			key_group[5]->picbtnpathd = (u8*)BMP_SZ_XX;
			key_group[5]->picbtnpathu = (u8*)BMP_SZ_XX;
			for(i=0;i<6;i++)
			{
				btn_draw(key_group[i]);
			}
		}
	}

	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	CurrentMenu = main_menu;
			redraw_menu=TRUE;
		  break;
		  case 1:
		  	CurrentMenu = QRSelect_menu;
			redraw_menu=TRUE;
		  break;
		  case 2:
		  	lastMenu = CurrentMenu;
			tempMenu = CurrentMenu;
		  	CurrentMenu = filamentstep1_menu;
			redraw_menu=TRUE;
		  break;
		  case 3:
		  	//CurrentMenu = language_menu;
		  	CurrentMenu = gecodelist_screen;
			redraw_menu=TRUE;
		  break;
		  case 4:
		  	TP_Adjust();
			redraw_menu=TRUE;
		  break;
		  case 5:
		  	CurrentMenu = about_menu;
			redraw_menu=TRUE;
		  break;
		  default:
		  break;
		}
	}

	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}

}

void wifi_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2;
	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//显示绑定手机二维码
		memset(fatbuf,0,sizeof(fatbuf));
		sprintf(fatbuf,"%08X%08X%08X",CPUID[0],CPUID[1],CPUID[2]);
		qrencode_display(fatbuf,110,40,1);
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			key_group[0]=btn_creat(5,5,42,34,0,1);			//返回按钮
			key_group[1]=btn_creat(98,150,124,44,1,1);		//wifi设置密码
			gui_show_strmid(0,5,320,16,GOLDEN,16,"扫码绑定打印机",0);
			gui_show_strmid(0,200,320,16,WHITE,16,"如果路由器密码改变",0);
			gui_show_strmid(0,220,320,16,WHITE,16,"请重置本机wifi密码以连接网络",0);
			key_group[0]->picbtnpathd = (u8*)BMP_SZ_FH;
			key_group[0]->picbtnpathu = (u8*)BMP_SZ_FH;
			key_group[1]->picbtnpathd = (u8*)BMP_CZMM;
			key_group[1]->picbtnpathu = (u8*)BMP_CZMM;
			for(i=0;i<2;i++)
			{
				btn_draw(key_group[i]);
			}
		}
	}

	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			CurrentMenu = set_menu;
			redraw_menu=TRUE;
		  break;
		  case 1:
			CurrentMenu = wifi2_menu;
			redraw_menu=TRUE;
		  break;
		  default:
		  break;
		}
	}

	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
}
u8 FilamentNum=0;	//00 1号料盘 01 2号料盘 02 3号料盘
//更换打印材料
void filamentstep1_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 5;

	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			//gui_show_strmid(0,50,320,16,WHITE,16,"请点击下面的按钮选择你想要",0);
			gui_show_string("第一步：退出旧打印材料",72,10,176,16,16,GOLDEN);
			gui_show_strmid(0,50,320,16,WHITE,16,"请点击下面的按钮选择你想要",0);
			gui_show_strmid(0,68,320,16,WHITE,16,"更换的料盘来退出这些料盘的旧打印材料",0);

			FilamentNum=0;
			key_group[0]=btn_creat(5,5,66,65,0,1);			//返回按钮
			key_group[0]->picbtnpathd = (u8*)BMP_SZ_FH;
			key_group[0]->picbtnpathu = (u8*)BMP_SZ_FH;
			//三个料盘选择按钮
			key_group[1]=btn_creat(23,130,72,56,1,1);			//1号料盘
			key_group[1]->picbtnpathd = (u8*)BMP_GHHC_LP1B;
			key_group[1]->picbtnpathu = (u8*)BMP_GHHC_LP1B;
			
			key_group[2]=btn_creat(122,130,72,56,2,1);			//2号料盘
			key_group[2]->picbtnpathd = (u8*)BMP_GHHC_LP2A;
			key_group[2]->picbtnpathu = (u8*)BMP_GHHC_LP2A;
			
			key_group[3]=btn_creat(221,130,72,56,3,1);			//3号料盘
			key_group[3]->picbtnpathd = (u8*)BMP_GHHC_LP3A;
			key_group[3]->picbtnpathu = (u8*)BMP_GHHC_LP3A;

			key_group[4]=btn_creat(260,200,36,35,4,1);			//下一步
			key_group[4]->picbtnpathd = (u8*)BMP_GHHC_XYB;
			key_group[4]->picbtnpathu = (u8*)BMP_GHHC_XYB;
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
	}

	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			CurrentMenu = lastMenu;
			redraw_menu=TRUE;
		  break;
		  case 1:
		  	FilamentNum=0;
			key_group[1]->picbtnpathd = (u8*)BMP_GHHC_LP1B;
			key_group[1]->picbtnpathu = (u8*)BMP_GHHC_LP1B;
			key_group[2]->picbtnpathd = (u8*)BMP_GHHC_LP2A;
			key_group[2]->picbtnpathu = (u8*)BMP_GHHC_LP2A;
			key_group[3]->picbtnpathd = (u8*)BMP_GHHC_LP3A;
			key_group[3]->picbtnpathu = (u8*)BMP_GHHC_LP3A;
			btn_draw(key_group[1]);
			btn_draw(key_group[2]);
			btn_draw(key_group[3]);
		  	break;
		  case 2:
		  	FilamentNum=1;
			key_group[1]->picbtnpathd = (u8*)BMP_GHHC_LP1A;
			key_group[1]->picbtnpathu = (u8*)BMP_GHHC_LP1A;
			key_group[2]->picbtnpathd = (u8*)BMP_GHHC_LP2B;
			key_group[2]->picbtnpathu = (u8*)BMP_GHHC_LP2B;
			key_group[3]->picbtnpathd = (u8*)BMP_GHHC_LP3A;
			key_group[3]->picbtnpathu = (u8*)BMP_GHHC_LP3A;
			btn_draw(key_group[1]);
			btn_draw(key_group[2]);
			btn_draw(key_group[3]);
		  	break;
		  case 3:
		  	FilamentNum=2;
			key_group[1]->picbtnpathd = (u8*)BMP_GHHC_LP1A;
			key_group[1]->picbtnpathu = (u8*)BMP_GHHC_LP1A;
			key_group[2]->picbtnpathd = (u8*)BMP_GHHC_LP2A;
			key_group[2]->picbtnpathu = (u8*)BMP_GHHC_LP2A;
			key_group[3]->picbtnpathd = (u8*)BMP_GHHC_LP3B;
			key_group[3]->picbtnpathu = (u8*)BMP_GHHC_LP3B;
			btn_draw(key_group[1]);
			btn_draw(key_group[2]);
			btn_draw(key_group[3]);
		  	break;
		  case 4:
		  	CurrentMenu = filamentstep2_menu;
			redraw_menu=TRUE;
		  
		  	break;
		  default:
		  break;
		}
	}

	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}	
}
void filamentstep2_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	u32 percent=0;
	static u8 cmd_flg=0;	//退出材料标志	
	float temperature_temp;
	static float oldtemp=0;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			key_group[0]=btn_creat(5,5,66,65,0,1);			//返回按钮
			key_group[0]->picbtnpathd = (u8*)BMP_SZ_FH;
			key_group[0]->picbtnpathu = (u8*)BMP_SZ_FH;
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_show_string("第一步：退出旧打印材料",72,10,176,16,16,GOLDEN);
		gui_show_strmid(0,65,320,16,WHITE,16,"正在退出旧打印材料",0);
		display_next_refresh_millis = millis();
		oldtemp = target_temperature[FilamentNum];		//更新挤出头温度 使其达到190℃以上
		if(target_temperature[FilamentNum]<190)
		{
			target_temperature[FilamentNum] = 190;
		}
	}

	if(display_next_refresh_millis<millis())	//等待温度达到
	{
		display_next_refresh_millis+=200;
		if(cmd_flg==0)
		{
			temperature_temp = target_temperature[FilamentNum];
			temperature_temp = temperature_temp - current_temperature[FilamentNum];
			temperature_temp = fabs(temperature_temp);
			if(temperature_temp<5.0)
			{
				cmd_flg = 1;
				enquecommand("G91");
				if(FilamentNum==1)
					enquecommand("T0");
				if(FilamentNum==2)
					enquecommand("T1");
				if(FilamentNum==3)
					enquecommand("T2");
				enquecommand("G1 E-500 F1200");
				enquecommand("G90");
			}
		}
		//显示退出百分比
#if 1
		if(current_block!=NULL)
		{
			temperature_temp = (float)e_steps_count/(float)current_block->step_event_count;
			temperature_temp *= 100;
			percent = (u8)temperature_temp;
		}
#endif
		progress_bar(10,155,percent);
		//等待旧料完全退出
		if(percent>=95)
		{
			target_temperature[FilamentNum] = oldtemp;
			redraw_menu = TRUE;
			CurrentMenu = filamentstep3_menu;
			cmd_flg =0;
		}
	}
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			CurrentMenu = lastMenu;
			redraw_menu=TRUE;
		  break;
		  default:
		  break;
		}
	}
	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
}
void filamentstep3_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;		
	static u8 flg=0;
	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//填充底色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		gui_show_string("第二步：放入新打印材料",5,5,176,16,16,GOLDEN);
		gui_show_strmid(0,50,320,16,WHITE,16,"请按照下图所示更换料盘，",0);
		gui_show_strmid(0,70,320,16,WHITE,16,"然后将新材料插入图示位置至少x厘米。",0);
		gui_show_strmid(0,90,320,16,RED,16,"（注意料盘编号和插入口编号要一致）",0);
		display_next_refresh_millis = millis();
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			key_group[0]=btn_creat(260,200,36,35,0,1);			//下一步
			key_group[0]->picbtnpathd = (u8*)BMP_GHHC_XYB;
			key_group[0]->picbtnpathu = (u8*)BMP_GHHC_XYB;
			btn_draw(key_group[0]);
		}
	}
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
		    CurrentMenu = filamentlaststep_menu;
			redraw_menu = TRUE;
		  break;
		  default:
		  break;
		}
	}
	//刷新插入线材示意图
	if(display_next_refresh_millis<millis())
	{
	    //beep();
		display_next_refresh_millis+=3000;	//每3s刷新一次
		if(flg)
		{
			flg=0;
			ai_load_picfile(BMP_GHHC_SYT1,110,120,100,100,1);
		}
		else
		{
			flg=1;
			ai_load_picfile(BMP_GHHC_SYT2,110,120,100,100,1);
		}
	}
	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
	
}
void filamentlaststep_menu()
{
	u32 percent=0;
	float temp_f;
	static u8 cmd_flg=0;	//退出材料标志	
	float temperature_temp;
	static float oldtemp=0;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//stdbmp_decode(BMP_GHHC_JL);
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		gui_show_string("最后一步",5,5,176,16,16,GOLDEN);
		gui_show_strmid(0,50,320,16,WHITE,16,"正在挤出新材料",0);
		gui_show_strmid(0,70,320,16,WHITE,16,"就要完成了",0);
		gui_show_strmid(0,90,320,16,RED,16,"请稍后",0);
		display_next_refresh_millis = millis();
		oldtemp = target_temperature[FilamentNum];		//更新挤出头温度 使其达到190℃以上
		if(target_temperature[FilamentNum]<190)
		{
			target_temperature[FilamentNum] = 190;
		}	
	}
	if(display_next_update_millis<millis())
	{
		display_next_update_millis += 200;

		if(cmd_flg==0)
		{
			temperature_temp = target_temperature[FilamentNum];
			temperature_temp = temperature_temp - current_temperature[FilamentNum];
			temperature_temp = fabs(temperature_temp);
			if(temperature_temp<5.5)
			{
				cmd_flg = 1;
				enquecommand("G91");
				if(FilamentNum==1)
					enquecommand("T0");
				if(FilamentNum==2)
					enquecommand("T1");
				if(FilamentNum==3)
					enquecommand("T2");
				enquecommand("G1 E500 F600");
				enquecommand("G90");
			}
		}
		//显示退出百分比
#if 1
		if(current_block!=NULL)
		{
			temp_f = (float)e_steps_count/(float)current_block->step_event_count;
			temp_f *= 100;
			percent = (u8)temp_f;
		}
#endif
		progress_bar(10,155,percent);
		//完全挤出判断
		if(percent>=95)
		{
			CurrentMenu = tempMenu;
			lastMenu = NULL;
			redraw_menu = TRUE;
			PrintInfo.filament = 0;	//强制更新料盘状态
			target_temperature[FilamentNum] = oldtemp;
			cmd_flg =0;
		}
	}
}
//语言切换
void language_menu(void)
{
	CurrentMenu = set_menu;
	redraw_menu = TRUE;
}
//机器信息
void about_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		gui_show_strmid(80,10,160,16,WHITE,16,TEXT_L,0);
		minibmp_decode(BMP_XHM,108,30,91,118,0,0);
		gui_show_string("固件版本: V1.2",108,160,150,16,16,WHITE);
		gui_show_string("软件版本: V1.1",108,180,150,16,16,WHITE);
		gui_show_string("地址:中航国际交流中心B座",108,200,208,16,16,WHITE);
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
		if(key_group)
		{
     	
			key_group[0]=btn_creat(5,5,42,34,0,1);			//返回按钮


			key_group[0]->picbtnpathd = (u8*)BMP_SZ_FH;
			key_group[0]->picbtnpathu = (u8*)BMP_SZ_FH;
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
	}

	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	CurrentMenu = set_menu;
			redraw_menu=TRUE;
		  break;
		  default:
		  break;
		}
	}

	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
}
//屏幕校准
void adjscreen_menu(void)
{
	CurrentMenu = set_menu;
	redraw_menu = TRUE;
}
u8 OldPruse=1;	//之前的暂停恢复状态 1为正在打印中 2为暂停中
void print_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2;		
	static u8 oldpresent=0;
	u32 printtime;
	u8 hour,min,sec;
	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//打印状态文字显示
		if(card.sdprinting==TRUE)
		{
			gui_show_strmid(10,10,300,16,GREEN,16,TEXT_H,0);
		}
		else
		{
			gui_show_strmid(10,10,300,16,GREEN,16,TEXT_I,0);
		}
		//绘制打印缩略图
		ai_load_picfile(BMP_SLT,110,30,100,100,1);
		//创建 暂停和停止两个按键
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			key_group[0]=btn_creat(80,190,42,42,0,1);			//打印暂停
			if(PrintInfo.printsd==2)
			{
				key_group[0]->picbtnpathd = (u8*)BMP_DY_HF;
				key_group[0]->picbtnpathu = (u8*)BMP_DY_HF;
				OldPruse = 2;
			}
			else
			{
				key_group[0]->picbtnpathd = (u8*)BMP_DY_ZT;
				key_group[0]->picbtnpathu = (u8*)BMP_DY_ZT;
				OldPruse = 1;
			}
			btn_draw(key_group[0]);
			key_group[1]=btn_creat(190,190,42,42,1,1);			//打印停止
			key_group[1]->picbtnpathd = (u8*)BMP_DY_TZ;
			key_group[1]->picbtnpathu = (u8*)BMP_DY_TZ;
			btn_draw(key_group[1]);
		}
		progress_bar(5,155,PrintInfo.printper);
		//显示打印时间
//		printtime = PrintInfo.printtime;
//		hour = printtime/3600;
//		min = printtime/60%60;
//		sec = printtime%60;
		
		hour = usr_timer.hour;
		min = usr_timer.min;
		sec = usr_timer.sec;
		
		memset(pname,0,sizeof(pname));
		sprintf(pname,"打印时长：%02d时%02d分%02d秒",hour,min,sec);
		gui_show_strmid(72,135,180,16,RED,16,pname,0);
		display_next_update_millis = millis();
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:	//打印暂停/恢复
			if(card.sdprinting==TRUE)
			{
				card.sdprinting = FALSE;
				PrintInfo.printsd = 2;		//打印暂停
				Z_EN(1);									//打印暂停时Z轴不能动
			}
			else
			{
				card.sdprinting = TRUE;
				PrintInfo.printsd = 1;		//正在打印状态
				
				
			}
			sendFlg = 1;
			
			break;
		  case 1:	//打印停止
		  	lastMenu = CurrentMenu;
			PrintPoint = TEXT_J;	
			CurrentMenu = popout_screen;
			tempMenu = stop_print;
			redraw_menu=TRUE;
		  break;
		  default:
		  break;
		}
	}
	//暂停继续图标检测
	if(OldPruse!=PrintInfo.printsd)
	{
		OldPruse = PrintInfo.printsd;
		if(PrintInfo.printsd==1)
		{
			key_group[0]->picbtnpathd = (u8*)BMP_DY_ZT;
			key_group[0]->picbtnpathu = (u8*)BMP_DY_ZT;
			gui_show_strmid(10,10,300,16,GREEN,16,TEXT_H,0);
		}
		else
		{
			key_group[0]->picbtnpathd = (u8*)BMP_DY_HF;
			key_group[0]->picbtnpathu = (u8*)BMP_DY_HF;
			gui_show_strmid(10,10,300,16,GREEN,16,TEXT_I,0);
		}
		btn_draw(key_group[0]);
	}
	//断料检测 舱门打开检测
	if(PrintInfo.filament)	//检测到缺料了
	{
		//暂停打印
		card.sdprinting = FALSE;
		//X Y轴归零避免影响打印模型
		enquecommand("G28 XY");
		if(PrintInfo.filament&0x01)
			FilamentNum = 0;
		else if(PrintInfo.filament&0x02)
			FilamentNum = 1;
		else if(PrintInfo.filament&0x04)
			FilamentNum = 2;
		//界面切换到更换线材
		tempMenu = CurrentMenu;
		lastMenu = CurrentMenu;
		CurrentMenu = filament_error;
		redraw_menu=TRUE;
	}
	if(ChildData.door_stat)	//检测到舱门被打开
	{
		//暂停打印
		card.sdprinting = FALSE;
		//切换到舱门打开提醒界面
		lastMenu = CurrentMenu;
		CurrentMenu = door_error;
		redraw_menu=TRUE;
	}
	//更新打印时长
	if(display_next_update_millis<millis())
	{
		display_next_update_millis += 3000;		//3S更新一次
		//显示打印时间
//		printtime = PrintInfo.printtime;
//		hour = printtime/3600;
//		min = printtime/60%60;
//		sec = printtime%60;
		hour = usr_timer.hour;
		min = usr_timer.min;
		sec = usr_timer.sec;
		memset(pname,0,sizeof(pname));
		sprintf(pname,"打印时长：%02d时%02d分%02d秒",hour,min,sec);
		gui_show_strmid(72,135,180,16,RED,16,pname,0);
	}
	//绘制进度条和更新打印进度
	if(card.sdprinting==TRUE && (PrintInfo.printper>oldpresent))
	{
		oldpresent = PrintInfo.printper;
		progress_bar(5,155,PrintInfo.printper);
		
	}
	if(PrintInfo.printsd==0 && card.sdprinting==0)	//打印完成
	{
		CurrentMenu = extruder_cool;	//跳转到挤出头冷却界面
		redraw_menu=TRUE;
		oldpresent = 0;
		PrintInfo.printsd = 0;
		sendFlg = 1;
	}
	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
	
}
void filament_error(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2;
	u8* lp1=NULL;
	u8* lp2=NULL;
	u8* lp3=NULL;
	lp1 = BMP_DY_LP1G;
	lp2 = BMP_DY_LP2G;
	lp3 = BMP_DY_LP3G;
	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//画中间框图
		gui_fill_rectangle(20,50,280,150,GREEN_CHILD);
		gui_show_strmid(0,10,320,16,GREEN,16,"打印耗材耗尽，请更换料盘后继续打印",0);

		//显示线材耗尽图片
		if(PrintInfo.filament&0x01)	//1号料盘
		{
			lp1 = BMP_DY_LP1;
		}
		if(PrintInfo.filament&0x02)	//2号料盘
		{
			lp2 = BMP_DY_LP2;
		}
		if(PrintInfo.filament&0x04)
		{
			lp3 = BMP_DY_LP3;
		}
		ai_load_picfile(lp1,40,85,66,65,1);
		ai_load_picfile(lp2,126,85,66,65,1);
		ai_load_picfile(lp3,212,85,66,65,1);
		//生成按钮
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			key_group[0]=btn_creat(55,161,87,33,0,1);			//放弃打印按钮
			key_group[0]->picbtnpathd = (u8*)BMP_DY_FQDY;
			key_group[0]->picbtnpathu = (u8*)BMP_DY_FQDY;
			btn_draw(key_group[0]);
			key_group[1]=btn_creat(177,161,87,33,1,1);			//去更换
			key_group[1]->picbtnpathd = (u8*)BMP_DY_GHLP;
			key_group[1]->picbtnpathu = (u8*)BMP_DY_GHLP;
			btn_draw(key_group[1]);
		}
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			quickStop(); 
			card_closefile();
			starttime=0;
			card.sdprinting = FALSE;
			AT24CXX_WriteOneByte(E_SDPRINT,FALSE);
			autotempShutdown();
			setTargetHotend(0,active_extruder);
	    disable_x(); 
			disable_y(); 
			disable_z(); 
			disable_e0(); 
			disable_e1(); 
		    enquecommand("M84");
			redraw_menu=TRUE;
			CurrentMenu = main_menu;
		  break;
		  case 1:
			redraw_menu=TRUE;
			lastMenu = CurrentMenu;
			CurrentMenu = filamentstep2_menu;
		  break;
		  default:
		  break;
		}
	}
	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
}
void door_error(void)
{
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		ai_load_picfile(BMP_DY_DR,0,0,320,240,1);
	}
	//等待舱门关闭
	if(ChildData.door_stat == FALSE)
	{
		card.sdprinting = TRUE;
		CurrentMenu = print_menu;
		redraw_menu = TRUE;
	}
}
void stop_print(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1; 
	gui_phy.back_color=WHITE;	
	if(redraw_menu==TRUE)
	{ 
		redraw_menu=FALSE;
		gui_draw_arcrectangle(75,80,180,40,6,1,GRAYBLUE,GRAYBLUE);
		gui_fill_rectangle(75,110,180,70,WHITE );
		gui_draw_rectangle(75,110,180,70,GRAYBLUE);
		gui_show_strmid(75,80,180,30,BLACK,16,(u8*)PrintPoint,1);	
		key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(key_group)
		{ 			
			key_group[0]=btn_creat(90,125,60,40,0,2);
			key_group[0]->bkctbl[0]=0X8452;
			key_group[0]->bkctbl[1]=0XAD97;			
			key_group[0]->bkctbl[2]=0XAD97;
			key_group[0]->bkctbl[3]=0X8452;
			key_group[0]->caption=(u8*)"确定";
			btn_draw(key_group[0]);
		}
	}	
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{	
		switch(selx & ~(3<<6))//
		{   
			case 0:	
				quickStop(); 
            	card_closefile();
            	starttime=0;
            	card.sdprinting = FALSE;
            	PrintInfo.printsd = 0;
            	AT24CXX_WriteOneByte(E_SDPRINT,FALSE);
            	autotempShutdown();
            	setTargetHotend(0,active_extruder);	
            	setTargetBed(0); 
                disable_x(); 
            	disable_y(); 
            	disable_z(); 
            	disable_e0(); 
            	disable_e1();
            	memset(CmdBuff.cmdbuffer,0,sizeof(CmdBuff.cmdbuffer));
                //enquecommand("M84");
            	//enquecommand("G28 XY");
            	bufindr = 0;
            	bufindw = 0;
            	buflen = 0;
                enquecommand("G28 XY");
                enquecommand("G0 Z10");
                /*
            	enquecommand("G91");
            	enquecommand("G0 Z10 F1600");	//Z轴向上抬10mm
            	enquecommand("G90");
            	*/
            //	G28_process(0x03);

            	
            	redraw_menu=TRUE;
            	sendFlg = TRUE;
            	CurrentMenu = main_menu;		
			    break;
			default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
	
}
#define COOLTEMP 50	//冷却温度值
u8 fake_temperature=0;
void extruder_cool(void)
{
	static float persent=0;
	static u8 oldtemper=0;
	u8 temp_u8;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//stdbmp_decode(BMP_DY_DYLQ);
		ai_load_picfile(BMP_DY_DYLQ,0,0,320,240,1);
		oldtemper = (u8)current_temperature[active_extruder];
		fake_temperature = oldtemper;
		persent = current_temperature[0] - COOLTEMP;
		persent = 100.0/persent;
		display_next_update_millis = millis();
	}
	if(display_next_update_millis<millis())
	{
		display_next_update_millis=millis()+10;	//10ms
		fake_temperature--;
		//更新冷却进度条
		//temp_u8 = oldtemper - (u8)current_temperature[active_extruder];
		temp_u8 = oldtemper - fake_temperature;
		temp_u8 *= persent;
		progress_bar(5,186,temp_u8);
		//判断是否冷却到指定温度
		//if(current_temperature[0]<=COOLTEMP)
		if(fake_temperature<=COOLTEMP)
		{
			redraw_menu = TRUE;
			CurrentMenu = printcomplete_menu;
		}
	}

}
void printcomplete_menu(void)
{
	u32 usetime;
	//打印完成图片显示5s
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		delay_time_millis = millis();
		//显示背景图
		ai_load_picfile(BMP_DY_DYWC,0,0,320,240,1);
		//显示模型缩略图
		ai_load_picfile(BMP_SLT,110,70,100,100,1);
	}
	usetime = millis() - delay_time_millis;
	if(usetime>5000)
	{
		usetime=0;
		redraw_menu = TRUE;
		PrintInfo.printsd = 0;
		CurrentMenu = main_menu;
	}
}
void wifi2_menu(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	gui_phy.back_color=BACK_CHILD;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//显示绑定手机二维码
		memset(fatbuf,0,sizeof(fatbuf));
		sprintf(fatbuf,"%s::%08x%08x%08x",SSID,CPUID[0],CPUID[1],CPUID[2]);
		qrencode_display(fatbuf,110,60,1);
		gui_show_strmid(0,180,320,16,GOLDEN,16,"请扫码连接打印机设置WIFI",0);
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			key_group[0]=btn_creat(5,5,63,46,0,1);			//返回按钮
			key_group[0]->picbtnpathd = (u8*)BMP_SZ_FH;
			key_group[0]->picbtnpathu = (u8*)BMP_SZ_FH;
			btn_draw(key_group[0]);
		}
	}

	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			CurrentMenu = set_menu;
			redraw_menu=TRUE;
		  break;
		  default:
		  break;
		}
	}

	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
}

//进度条绘制
/*
进度条固定长度为270 宽度为30
sx 进度条起始x坐标
sy 进度条起始y坐标
progress 百分比
*/
void progress_bar(u16 sx,u16 sy,u8 progress)
{
	int lenth;
	u8 str[5];
	if(progress>100)
		progress=100;
	//绘制已完成的矩形框
	lenth = 2.7*progress; // 270/100*progress
	gui_fill_rectangle(sx,sy,lenth,30,GREEN);
	//绘制剩余长度
	gui_fill_rectangle(sx+lenth,sy,270-lenth,30,WHITE);
	//显示百分比
	sprintf(str,"%d%%",progress);
	gui_show_strmid(sx+270,sy,32,30,RED,16,str,0);
}


//打印任务开始前的模型是否取出确认
void model_check(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2;
	if(redraw_menu==TRUE)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,BACK_CHILD);
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建按钮
		if(key_group)
		{
			key_group[0]=btn_creat(22,184,103,36,0,1);			//返回按钮
			key_group[0]->picbtnpathd = (u8*)BMP_DY_FH;
			key_group[0]->picbtnpathu = (u8*)BMP_DY_FH;
			btn_draw(key_group[0]);
			key_group[1]=btn_creat(194,184,103,36,1,1);			//确认取出
			key_group[1]->picbtnpathd = (u8*)BMP_DY_QRQC;
			key_group[1]->picbtnpathu = (u8*)BMP_DY_QRQC;
			btn_draw(key_group[0]);
		}
	}

	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
		  	card.sdprinting = FALSE;
			CurrentMenu = main_menu;
			redraw_menu=TRUE;
		  break;
		  case 1:
		  	CurrentMenu = print_menu;
			redraw_menu=TRUE;
		  default:
		  break;
		}
	}

	//切换界面时回收内存
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}	
}



void popout_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2; 
	gui_phy.back_color=WHITE;	
	if(redraw_menu==TRUE)
	{ 
		redraw_menu=FALSE;
		gui_draw_arcrectangle(75,80,180,40,6,1,GRAYBLUE,GRAYBLUE);
		gui_fill_rectangle(75,110,180,70,WHITE );
		gui_draw_rectangle(75,110,180,70,GRAYBLUE);
		gui_show_strmid(75,80,180,30,BLACK,16,(u8*)PrintPoint,1);	
		key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(key_group)
		{ 			
			key_group[0]=btn_creat(90,125,60,40,0,2);
			key_group[0]->bkctbl[0]=0X8452;
			key_group[0]->bkctbl[1]=0XAD97;			
			key_group[0]->bkctbl[2]=0XAD97;
			key_group[0]->bkctbl[3]=0X8452;
			key_group[0]->caption=(u8*)"确定";
			btn_draw(key_group[0]);
			
			key_group[1]=btn_creat(180,125,60,40,1,2);
			key_group[1]->bkctbl[0]=0X8452;
			key_group[1]->bkctbl[1]=0XAD97;			
			key_group[1]->bkctbl[2]=0XAD97;
			key_group[1]->bkctbl[3]=0X8452;
			key_group[1]->caption=(u8*)"取消";
			btn_draw(key_group[1]);
			
		}
	}	
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{	
		switch(selx & ~(3<<6))//
		{   
			case 0:	
				
				if(tempMenu==NULL)
				{
					CurrentMenu = lastMenu;
				}
				else
				{
					CurrentMenu = tempMenu;
					tempMenu = NULL;
				}				
			break;
			case 1:
				CurrentMenu = lastMenu;
				redraw_menu=TRUE;
			default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;	
	}
}

void QRSelect_menu(void)
{
	if(ChildData.wifi_stat==2)
	{
		CurrentMenu = wifi2_menu;
	}
	else
	{
		CurrentMenu = wifi_menu;
	}
	redraw_menu=TRUE;
}
_btn_obj **screen_key_group;

void reprint_menu(void)
{
	u32 time=0xfffff;
	u8 selx=0XFF;
	u8 i;
	u8 num = 2; 
	gui_phy.back_color=WHITE;
	gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
	gui_draw_arcrectangle(75,80,180,40,6,1,GRAYBLUE,GRAYBLUE);
	gui_fill_rectangle(75,110,180,70,WHITE );
	gui_draw_rectangle(75,110,180,70,GRAYBLUE);
	gui_show_strmid(75,80,180,30,BLACK,16,(u8*)"是否需要续打？",1);	
	screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
	if(screen_key_group)
	{ 			
		screen_key_group[0]=btn_creat(90,125,60,40,0,2);
		screen_key_group[0]->bkctbl[0]=0X8452;
		screen_key_group[0]->bkctbl[1]=0XAD97;
		screen_key_group[0]->bkctbl[2]=0XAD97;
		screen_key_group[0]->bkctbl[3]=0X8452;
		screen_key_group[0]->caption=(u8*)"确定";
		btn_draw(screen_key_group[0]);
		
		screen_key_group[1]=btn_creat(180,125,60,40,1,2);
		screen_key_group[1]->bkctbl[0]=0X8452;
		screen_key_group[1]->bkctbl[1]=0XAD97;			
		screen_key_group[1]->bkctbl[2]=0XAD97;
		screen_key_group[1]->bkctbl[3]=0X8452;
		screen_key_group[1]->caption=(u8*)"取消";
		btn_draw(screen_key_group[1]);
		
	}
	PrintInfo.printsd = 2;	//如果有未打印完成的则将状态改为暂停打印
	while(time--)
	{

		manage_heater();
    	manage_inactivity();
		if(buflen < (BUFSIZE-1))
		{ 
			get_command();
			get_command2();
		}
		if(buflen)
		{
			process_commands();  
			if(buflen > 0)                                                                                               
				buflen = (buflen-1);
			bufindr = (bufindr + 1)%BUFSIZE;
		}

		printinfo_update();
		if (tp_scan_next_update_millis < millis())
		{
		  tp_dev.scan(0);
		  in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);
		  tp_scan_next_update_millis = millis() + 10;
		}
		selx=screen_key_chk(screen_key_group,&in_obj,num);
		if(selx&(1<<6))
		{	
			switch(selx & ~(3<<6))
			{   
				case 0:	
					NeedRePrintFlg = TRUE;			
				break;
				case 1:
					NeedRePrintFlg = FALSE;
					AT24CXX_WriteOneByte(E_SDPRINT,FALSE);
				break;
				default: break;
			}
			for(i=0;i<num;i++)
			{	
				btn_delete(screen_key_group[i]);
			}
			gui_memin_free(screen_key_group);
			break;
		}
	}
}
#if 1
_filelistbox_obj * flistbox;
void gecodelist_screen(void)						//打印文件显示列表
{ 
	u8 selx=0XFF;
	u8 i;	
 	_filelistbox_list * filelisttemp;	
 	if(windows_flag==FALSE)
	{	 
		if(redraw_menu==TRUE)			
		{ 
			redraw_menu=FALSE;		

			gui_fill_rectangle(0,0,lcddev.width,lcddev.height,LIGHTGRAY );
			gui_fill_rectangle(0,0,lcddev.width,40,LIGHTBLUE);  
			gui_draw_hline(0,40,lcddev.width,GRAY);
			gui_show_strmid(0,0,320,40,WHITE,16,(u8*)"SD卡打印",1);
			//gui_show_strmid(0,0,320,40,WHITE,16,(u8*)"SD card print",1);
			//画背景				
			
			screen_key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*3);
			flistbox=filelistbox_creat(2,44,lcddev.width-4,192,1,16);
			if(screen_key_group&&flistbox)
			{		
              	screen_key_group[0]=btn_creat(8,8,50,25,0,4);
				screen_key_group[0]->bcfucolor=BLACK;
				screen_key_group[0]->bcfdcolor=WHITE;
				screen_key_group[0]->bkctbl[0]=CYAN;
				screen_key_group[0]->bkctbl[1]=RED;
				screen_key_group[0]->bkctbl[2]=CYAN;
				screen_key_group[0]->bkctbl[3]=RED;
			 	screen_key_group[0]->caption="返回";
				for(i=1;i<3;i++)
				{	
					screen_key_group[i]=btn_creat(95+(i-1)*80,125,60,40,i,2);
					
					screen_key_group[i]->bkctbl[0]=0X8452;
					screen_key_group[i]->bkctbl[1]=0XAD97;			
					screen_key_group[i]->bkctbl[2]=0XAD97;
					screen_key_group[i]->bkctbl[3]=0X8452;
				}
				screen_key_group[1]->caption=(u8*)"打印";
				screen_key_group[2]->caption=(u8*)"返回";
			
				flistbox->fliter=FLBOX_FLT_GCODE;
				flistbox->type=0;
				flistbox->path="0:\\GCODE";	
//				flistbox->path="0:";				
				filelistbox_scan_filelist(flistbox);
				filelistbox_draw_listbox(flistbox);		
				
				for(i=0;i<1;i++)
				{	
					btn_draw(screen_key_group[i]);
				}
			}
		}


		selx=filelistbox_check(flistbox,&in_obj);		
		selx=screen_key_chk(screen_key_group,&in_obj,1);
		
		if(selx&(1<<6))
		{	
			switch(selx & ~(3<<6))//
			{  
				case 0: 
					CurrentMenu = set_menu;
					redraw_menu=TRUE ;
					for(i=0;i<3;i++)
					{	
						btn_delete(screen_key_group[i]);
					}
					gui_memin_free(screen_key_group); 
					filelistbox_delete(flistbox);
									
				break;			   
			   	default: break;
			}
		}
		else
		{
			if(flistbox->dbclick==0X81)
			{ 
				flistbox->dbclick=0x00;
				redraw_menu=TRUE;
				windows_flag=TRUE;
			}
		}
	}
	else 
	{	
		if(redraw_menu==TRUE)
		{ 
			redraw_menu=FALSE;
		
			gui_draw_arcrectangle(75,80,180,40,6,1,GRAYBLUE,GRAYBLUE);
			gui_fill_rectangle(75,110,180,70,WHITE );//
			gui_draw_rectangle(75,110,180,70,GRAYBLUE);
			filelisttemp=filelist_search(flistbox->list,flistbox->selindex);
			gui_show_strmid(75,80,180,30,BLACK,16,filelisttemp->name,1);	
			
			if(screen_key_group)
			{ 					
				for(i=1;i<3;i++)
				{	
					btn_draw(screen_key_group[i]);
				}
			}
		}	
		selx=screen_key_chk(screen_key_group+1,&in_obj,2);
		if(selx&(1<<6))
		{	
			switch(selx & ~(3<<6))//
			{   
				case 0:	
					filelisttemp=filelist_search(flistbox->list,flistbox->selindex);
					strcpy((char *)card.filename,(const char*)filelisttemp->name);
					if(pname)  
					{	
						pname=gui_path_name(pname,flistbox->path,filelisttemp->name);
						selx=f_open(&card.fgcode,(const TCHAR*)pname,FA_READ);
						if (selx==FR_OK)
						{
							
							strcpy(PrintInfo.printfile,filelisttemp->name);//更新打印文件名
							PrintInfo.filesize =card.fgcode.fsize;			//更新打印文件大小
							printf("printfile is %s\r\n",PrintInfo.printfile);
							printf("filesize is %d\r\n",PrintInfo.filesize);
						
							CurrentMenu = print_menu;
							redraw_menu=TRUE;
							windows_flag=FALSE;	
							for(i=0;i<3;i++)
							{	
								btn_delete(screen_key_group[i]);
							}
							gui_memin_free(screen_key_group);
							filelistbox_delete(flistbox);												
							card_startFileprint();
							starttime=millis();
						}
					}
				break;
				case 1:		
					windows_flag=FALSE;
					filelistbox_draw_listbox(flistbox);
				break;						
			   	default: break;
			}
		}		
	}
	
}
#endif







#define BACK_DIY 		 0x1210	//DIY打印机背景色	 
#define BACK2_DIY		0x3b16	//按钮背景色

/*logo*/
u8* PTH_LOGO = "1:/diy_icon/logo.bmp";

/*主界面*/
u8* PTH_ZY_DY = "1:/diy_icon/000.bmp";		//打印图标
u8* PTH_ZY_KZ = "1:/diy_icon/001.bmp";		//控制图标
u8* PTH_ZY_SD = "1:/diy_icon/002.bmp";		//设置图标
u8* PTH_ZY_3DP = "1:/diy_icon/003.bmp";		//3d打印图片	
u8* PTH_ZY_LASER = "1:/diy_icon/004.bmp";	//激光打印图片
/*控制*/
u8* PTH_KZ1_010 = "1:/diy_icon/010.bmp";		//预热
u8* PTH_KZ1_011 = "1:/diy_icon/011.bmp";		//移动
u8* PTH_KZ1_012 = "1:/diy_icon/012.bmp";		//归零
u8* PTH_KZ1_013 = "1:/diy_icon/013.bmp";		//挤出
u8* PTH_KZ1_014 = "1:/diy_icon/014.bmp";		//调平
u8* PTH_KZ1_015 = "1:/diy_icon/015.bmp";		//风扇
u8* PTH_KZ1_016 = "1:/diy_icon/016.bmp";		//紧急停止
u8* PTH_KZ1_017 = "1:/diy_icon/017.bmp";		//3D打印图标
u8* PTH_KZ1_005 = "1:/diy_icon/005.bmp";		//控制图标背景

/*激光控制*/
u8* PTH_FH = "1:/diy_icon/018.bmp";			//返回
u8* PTH_KZ2_020 = "1:/diy_icon/020.bmp";		//移动及原点设置
u8* PTH_KZ2_021 = "1:/diy_icon/021.bmp";		//打开关闭激光头
u8* PTH_KZ2_022 = "1:/diy_icon/022.bmp";		//风扇控制
/*关于*/
u8* PTH_SZ_000 = "1:/diy_icon/030.bmp";		//关于
u8* PTH_SZ_001 = "1:/diy_icon/031.bmp";		//语言
u8* PTH_SZ_002 = "1:/diy_icon/032.bmp";		//屏幕校准
u8* PTH_SZ_003 = "1:/diy_icon/033.bmp";		//状态
u8* PTH_SZ_004 = "1:/diy_icon/034.bmp";		//机器信息
u8* PTH_SZ_005 = "1:/diy_icon/035.bmp";		//wifi

u8* PTH_YR_009 = "1:/diy_icon/036.bmp";		//挤出头1小图标
u8* PTH_YR_010 = "1:/diy_icon/037.bmp";		//挤出头2小图标
u8* PTH_YR_011 = "1:/diy_icon/038.bmp";		//底板小图标
u8* PTH_YR_000 = "1:/diy_icon/040.bmp";		//减
u8* PTH_YR_001 = "1:/diy_icon/041.bmp";		//加
u8* PTH_YR_002 = "1:/diy_icon/042.bmp";		//喷头1
u8* PTH_YR_003 = "1:/diy_icon/043.bmp";		//喷头2
u8* PTH_YR_004 = "1:/diy_icon/044.bmp";		//底板
u8* PTH_YR_005 = "1:/diy_icon/045.bmp";		//温度1
u8* PTH_YR_006 = "1:/diy_icon/046.bmp";		//停止
u8* PTH_YR_007 = "1:/diy_icon/047.bmp";		//温度5
u8* PTH_YR_008 = "1:/diy_icon/048.bmp";		//温度10
u8* PTH_YR_012 = "1:/diy_icon/049.bmp";     //温度等级背景
//移动界面
u8* PTH_YD_000 = "1:/diy_icon/050.bmp";		//E上
u8* PTH_YD_001 = "1:/diy_icon/051.bmp";		//Y上
u8* PTH_YD_002 = "1:/diy_icon/052.bmp";		//Z上
u8* PTH_YD_003 = "1:/diy_icon/053.bmp";		//E1
u8* PTH_YD_004 = "1:/diy_icon/054.bmp";		//E2
u8* PTH_YD_005 = "1:/diy_icon/055.bmp";		//X左
u8* PTH_YD_006 = "1:/diy_icon/056.bmp";		//归零
u8* PTH_YD_007 = "1:/diy_icon/057.bmp";		//x右
u8* PTH_YD_008 = "1:/diy_icon/058.bmp";		//E下
u8* PTH_YD_009 = "1:/diy_icon/059.bmp";		//Y下
u8* PTH_YD_010 = "1:/diy_icon/060.bmp";		//z下
u8* PTH_YD_011 = "1:/diy_icon/061.bmp";		//1mm
u8* PTH_YD_012 = "1:/diy_icon/062.bmp";		//5mm
u8* PTH_YD_013 = "1:/diy_icon/063.bmp";		//10mm

//归零
u8* PTH_GL_000 = "1:/diy_icon/070.bmp";		//X归零
u8* PTH_GL_001 = "1:/diy_icon/071.bmp";		//Y归零
u8* PTH_GL_002 = "1:/diy_icon/072.bmp";		//Z归零
u8* PTH_GL_003 = "1:/diy_icon/073.bmp";		//归零全部
u8* PTH_GL_004 = "1:/diy_icon/074.bmp";		//停止归零

//挤出
u8* PTH_JC_010 = "1:/diy_icon/075.bmp";		//背景图1
u8* PTH_JC_011 = "1:/diy_icon/076.bmp";		//背景图2
u8* PTH_JC_012 = "1:/diy_icon/077.bmp";     //进料出料按键背景
u8* PTH_JC_000 = "1:/diy_icon/080.bmp";		//小挤出头1
u8* PTH_JC_001 = "1:/diy_icon/081.bmp";		//小挤出头2
u8* PTH_JC_002 = "1:/diy_icon/082.bmp";		//大挤出头1
u8* PTH_JC_003 = "1:/diy_icon/083.bmp";		//大挤出头2
u8* PTH_JC_004 = "1:/diy_icon/084.bmp";		//进料
u8* PTH_JC_005 = "1:/diy_icon/085.bmp";		//退料
u8* PTH_JC_006 = "1:/diy_icon/086.bmp";		//1mm
u8* PTH_JC_007 = "1:/diy_icon/087.bmp";		//5mm
u8* PTH_JC_008 = "1:/diy_icon/088.bmp";		//10mm
u8* PTH_JC_009 = "1:/diy_icon/089.bmp";		//停止


//风扇
u8* PTH_FS_000 = "1:/diy_icon/090.bmp";		//风扇小图标1
u8* PTH_FS_001 = "1:/diy_icon/091.bmp";		//风扇1
u8* PTH_FS_002 = "1:/diy_icon/092.bmp";		//风扇2
u8* PTH_FS_003 = "1:/diy_icon/093.bmp";		//风扇3
u8* PTH_FS_004 = "1:/diy_icon/094.bmp";		//速度1
u8* PTH_FS_005 = "1:/diy_icon/095.bmp";		//速度2
u8* PTH_FS_006 = "1:/diy_icon/096.bmp";		//速度3
u8* PTH_FS_007 = "1:/diy_icon/097.bmp";		//停止 
u8* PTH_FS_008 = "1:/diy_icon/098.bmp";		//风扇小图标2
u8* PTH_FS_009 = "1:/diy_icon/099.bmp";		//风扇小图标3


//语言
u8* PTH_YY_000 = "1:/diy_icon/100.bmp";		//简体中文
u8* PTH_YY_001 = "1:/diy_icon/101.bmp";		//繁体中文
u8* PTH_YY_002 = "1:/diy_icon/102.bmp";		//英文
u8* PTH_YY_003 = "1:/diy_icon/103.bmp";		//五星
 
//状态
u8* PTH_ZT_000 = "1:/diy_icon/110.bmp";		//挤出头1
u8* PTH_ZT_001 = "1:/diy_icon/111.bmp";		//挤出头2
u8* PTH_ZT_002 = "1:/diy_icon/112.bmp";		//底板
u8* PTH_ZT_003 = "1:/diy_icon/113.bmp";		//框图

//打印
u8* PTH_DY_000 = "1:/diy_icon/120.bmp";		//缩略图
u8* PTH_DY_001 = "1:/diy_icon/121.bmp";		//打印时长
u8* PTH_DY_002 = "1:/diy_icon/122.bmp";		//挤出头1温度
u8* PTH_DY_003 = "1:/diy_icon/123.bmp";		//底板温度
u8* PTH_DY_004 = "1:/diy_icon/124.bmp";		//挤出头2温度
u8* PTH_DY_005 = "1:/diy_icon/125.bmp";		//打印速度
u8* PTH_DY_006 = "1:/diy_icon/126.bmp";		//打印暂停
u8* PTH_DY_007 = "1:/diy_icon/127.bmp";		//打印继续
u8* PTH_DY_008 = "1:/diy_icon/128.bmp";		//打印停止	
u8* PTH_DY_009 = "1:/diy_icon/129.bmp";		//打印控制
u8* PTH_DY_011 = "1:/diy_icon/130.bmp";		//矩形框


//打印控制
u8* PTH_DYKZ_001 = "1:/diy_icon/132.bmp";		//加热控制
u8* PTH_DYKZ_002 = "1:/diy_icon/133.bmp";		//速度控制
u8* PTH_DYKZ_003 = "1:/diy_icon/134.bmp";		//背景图
u8* PTH_DYKZ_004 = "1:/diy_icon/135.bmp";		//打印速度
u8* PTH_DYKZ_005 = "1:/diy_icon/136.bmp";		//挤出头1
u8* PTH_DYKZ_006 = "1:/diy_icon/137.bmp";		//挤出头2
u8* PTH_DYKZ_007 = "1:/diy_icon/138.bmp";		//底板
u8* PTH_DYKZ_008 = "1:/diy_icon/139.bmp";		//风扇1
u8* PTH_DYKZ_009 = "1:/diy_icon/140.bmp";		//风扇2
u8* PTH_DYKZ_010 = "1:/diy_icon/141.bmp";		//风扇3
u8* PTH_DYKZ_000 = "1:/diy_icon/142.bmp";		//风扇控制
u8* PTH_DYKZ_011 = "1:/diy_icon/143.bmp";		//挤出比
u8* PTH_DYKZ_012 = "1:/diy_icon/144.bmp";		//挤出比控制
u8* PTH_DYKZ_013 = "1:/diy_icon/145.bmp";		//控制按键背景

//打印速度
u8* PTH_DYSD_000 = "1:/diy_icon/150.bmp";		//速度标志
u8* PTH_DYSD_001 = "1:/diy_icon/151.bmp";		//10
u8* PTH_DYSD_002 = "1:/diy_icon/152.bmp";		//50
u8* PTH_DYSD_003 = "1:/diy_icon/153.bmp";		//100
u8* PTH_DYSD_004 = "1:/diy_icon/154.bmp";		//恢复常速
//返回按钮
u8* PTH_DY_FH = "1:/diy_icon/155.bmp";		//恢复常速

u8* PTY_DY_BG = "1:/diy_icon/156.bmp";      //速度等级背景

//文件选择
u8* PTH_FILE_000 = "1:/diy_icon/160.bmp";	    //文件背景
u8* PTH_FILE_001 = "1:/diy_icon/161.bmp";       //文件夹背景
u8* PTH_FILE_002 = "1:/diy_icon/162.bmp";	    //向上翻页
u8* PTH_FILE_003 = "1:/diy_icon/163.bmp";       //向下翻页
u8* PTH_FILE_004 = "1:/diy_icon/164.bmp";       //文件/夹到尾



u8 display_redraw=0;

void start_diy(void)
{

	//加载开机欢迎界面 显示2s
	
	ai_load_picfile(PTH_LOGO,0,0,320,240,1);
	u8 i;
	for(i=0;i<20;i++)
	{
		delay_ms(100);
	}
	//进入主菜单界面
	CurrentMenu = main_diy;
	redraw_menu = TRUE;
}


void main_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 3;

	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//显示机器类型
		PrinterMode==0 ? minibmp_decode(PTH_ZY_3DP,169,30,134,188,0,0) :minibmp_decode(PTH_ZY_LASER,169,30,134,188,0,0);
		//显示三个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
     		key_group[0]=btn_creat(17,30,144,58,0,1);
			key_group[1]=btn_creat(17,95,144,58,1,1);
			key_group[2]=btn_creat(17,159,144,58,2,1);
			
			key_group[0]->picbtnpathu=(u8*)PTH_ZY_DY;
			key_group[0]->picbtnpathd=(u8*)PTH_ZY_DY;
			
			key_group[1]->picbtnpathu=(u8*)PTH_ZY_KZ;
			key_group[1]->picbtnpathd=(u8*)PTH_ZY_KZ;
			
			key_group[2]->picbtnpathu=(u8*)PTH_ZY_SD;
			key_group[2]->picbtnpathd=(u8*)PTH_ZY_SD;

			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		//显示文字
		gui_phy.back_color = BACK2_DIY;	//改为按键背景色
    	gui_show_ptstr(117,51,117+32,51+16,0,WHITE,16,"打印",0);
    	gui_show_ptstr(117,116,117+32,116+16,0,WHITE,16,"控制",0);
    	gui_show_ptstr(117,180,117+32,180+16,0,WHITE,16,"设置",0);
		
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	redraw_menu = TRUE;
			CurrentMenu = getgcodetab_diy;		//选择打印界面
			PRINTFLINE;
		  break;
		  case 1:
		  	redraw_menu = TRUE;
			if(PrinterMode==0)
				CurrentMenu = control_3d_diy;		//3d打印控制界面
			else if(PrinterMode==1)
				CurrentMenu = control_3d_diy;		//激光打印控制界面
				PRINTFLINE;
		  break;
		 case 2:
		 	redraw_menu = TRUE;
			CurrentMenu = set_diy;		//设置界面
			PRINTFLINE;
		 	break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}

void set_diy(void)												//设置显示页面
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 7;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);

		//显示7个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
     		key_group[0]=btn_creat(16,52,91,55,0,1);
			key_group[1]=btn_creat(114,52,91,55,1,1);
			key_group[2]=btn_creat(212,52,91,55,2,1);
			key_group[3]=btn_creat(16,113,91,55,3,1);
			key_group[4]=btn_creat(114,113,91,55,4,1);
			key_group[5]=btn_creat(212,113,91,55,5,1);
			key_group[6]=btn_creat(0,0,320,36,5,1);
			
			key_group[0]->picbtnpathu=(u8*)PTH_SZ_000;
			key_group[0]->picbtnpathd=(u8*)PTH_SZ_000;
			key_group[1]->picbtnpathu=(u8*)PTH_SZ_001;
			key_group[1]->picbtnpathd=(u8*)PTH_SZ_001;
			key_group[2]->picbtnpathu=(u8*)PTH_SZ_002;
			key_group[2]->picbtnpathd=(u8*)PTH_SZ_002;
			key_group[3]->picbtnpathu=(u8*)PTH_SZ_003;
			key_group[3]->picbtnpathd=(u8*)PTH_SZ_003;
			key_group[4]->picbtnpathu=(u8*)PTH_SZ_004;
			key_group[4]->picbtnpathd=(u8*)PTH_SZ_004;
			key_group[5]->picbtnpathu=(u8*)PTH_SZ_005;
			key_group[5]->picbtnpathd=(u8*)PTH_SZ_005;
			key_group[6]->picbtnpathu=(u8*)PTH_FH;
			key_group[6]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		//显示标题
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"设置",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	redraw_menu = TRUE;
			CurrentMenu = about_diy;
		  break;
		  case 1:
			redraw_menu = TRUE;
			CurrentMenu = language_diy;
		  break;
		 case 2:
		 	TP_Adjust();
			redraw_menu = TRUE;
		 	break;
		 case 3:
		 	redraw_menu = TRUE;
			CurrentMenu = status_diy;
		 	break;
		 case 4:
		 	
		 	break;
		 case 5:
		 	//redraw_menu = TRUE;
			//CurrentMenu = wifi_diy;
		 	break;
		 case 6:
		 	redraw_menu = TRUE;
			CurrentMenu = main_diy;
		 	break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}	
}
void select_diy(void)
{
	
}

void control_3d_diy(void)					//控制显示界面
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 8;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		minibmp_decode(PTH_KZ1_005,16,169,91,54,0,0);
		minibmp_decode(PTH_KZ1_005,212,49,91,54,0,0);
		//显示8个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建8个图标
		if(key_group)
		{
     		key_group[0]=btn_creat(16,49,91,54,0,1);
			key_group[1]=btn_creat(114,49,91,54,1,1);
            //91 -24 /2
			key_group[2]=btn_creat(245,64,24,24,2,1);
			key_group[3]=btn_creat(16,109,91,54,3,1);
			key_group[4]=btn_creat(114,109,91,54,4,1);
			key_group[5]=btn_creat(212,109,91,54,5,1);
			key_group[6]=btn_creat(51,183,25,26,6,1);
			key_group[7]=btn_creat(0,0,320,36,7,1);
			
			key_group[0]->picbtnpathu=(u8*)PTH_KZ1_010;
			key_group[0]->picbtnpathd=(u8*)PTH_KZ1_010;
			key_group[1]->picbtnpathu=(u8*)PTH_KZ1_011;
			key_group[1]->picbtnpathd=(u8*)PTH_KZ1_011;
			key_group[2]->picbtnpathu=(u8*)PTH_KZ1_012;
			key_group[2]->picbtnpathd=(u8*)PTH_KZ1_012;
			key_group[3]->picbtnpathu=(u8*)PTH_KZ1_013;
			key_group[3]->picbtnpathd=(u8*)PTH_KZ1_013;
			key_group[4]->picbtnpathu=(u8*)PTH_KZ1_014;
			key_group[4]->picbtnpathd=(u8*)PTH_KZ1_014;
			key_group[5]->picbtnpathu=(u8*)PTH_KZ1_015;
			key_group[5]->picbtnpathd=(u8*)PTH_KZ1_015;
			key_group[6]->picbtnpathu=(u8*)PTH_KZ1_016;
			key_group[6]->picbtnpathd=(u8*)PTH_KZ1_016;
			key_group[7]->picbtnpathu=(u8*)PTH_FH;
			key_group[7]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"控制",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	redraw_menu = TRUE;
			lastMenu = CurrentMenu;
			CurrentMenu = preheat_diy;
		  break;
		  case 1:
			redraw_menu = TRUE;
			CurrentMenu = move1_diy;
		  break;
		 case 2:
		 	redraw_menu = TRUE;
			CurrentMenu = zero_diy;
		 	break;
		 case 3:
		 	redraw_menu = TRUE;
			CurrentMenu = extruder_diy;
		 	break;
		 case 4:
		 	//TP_Adjust();
			//redraw_menu=TRUE;
		 	break;
		 case 5:
		 	redraw_menu = TRUE;
			lastMenu = CurrentMenu;
			CurrentMenu = fan_diy;
		 	break;
		 case 6:
			quickStop();
		 	break;
		 case 7:
		 	redraw_menu = TRUE;
			CurrentMenu = main_diy;
		 	break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}

void control_laser_diy(void)
{
	
}
//预热
static u8 preheat_choice=0;	//加热类型选择 0：挤出头1加热 1：挤出头2加热 2：底板加热
static u8 heat_unit=1;		//加热温度单位 1度 5度 10度
void preheat_diy(void)						//加热控制显示界面
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 6;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		switch(display_redraw)
		{
			case 0:
				//画背景色
				gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
			break;
			case 1:
				minibmp_decode(PTH_YR_012,114,138,92,69,0,0);
				//显示当前温度
				memset(fatbuf,0,50);
				switch(preheat_choice)
				{
					case 0:
						sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[0],target_temperature[0]);
					break;
					case 1:
						sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[1],target_temperature[1]);
					break;
					case 2:
						sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_bed_temperature,target_temperature_bed);
					break;
					default:
					break;
				}
				gui_phy.back_color = BACK_DIY;
				gui_show_strmid(115,91,90,16,WHITE,16,fatbuf,0);
			break;
			case 2:
				//显示6个按钮图标
				key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
				if(key_group)
				{
		     		key_group[0]=btn_creat(16,49,92,69,0,1);
					key_group[1]=btn_creat(213,49,92,69,1,1);
					key_group[2]=btn_creat(16,138,92,69,2,1);
					key_group[3]=btn_creat(114,138,92,69,3,1);
					key_group[4]=btn_creat(212,138,92,69,4,1);
					key_group[5]=btn_creat(0,0,320,36,5,1);

					
					key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
					key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
					key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
					key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
					switch(preheat_choice)
					{
						case 0:	key_group[2]->picbtnpathu=(u8*)PTH_YR_002;
								key_group[2]->picbtnpathd=(u8*)PTH_YR_002;
								minibmp_decode(PTH_YR_009,146,62,27,24,0,0);
								
						break;
						case 1:	key_group[2]->picbtnpathu=(u8*)PTH_YR_003;
								key_group[2]->picbtnpathd=(u8*)PTH_YR_003;
								minibmp_decode(PTH_YR_010,146,62,27,24,0,0);
						break;
						case 2:	key_group[2]->picbtnpathu=(u8*)PTH_YR_004;
								key_group[2]->picbtnpathd=(u8*)PTH_YR_004;
								minibmp_decode(PTH_YR_011,146,62,27,24,0,0);
						break;
						default:break;
					}
		            gui_phy.back_color = BACK2_DIY;
					switch(heat_unit)
					{
						case 1:	/*
						        key_group[3]->picbtnpathu=(u8*)PTH_YR_005;
								key_group[3]->picbtnpathd=(u8*)PTH_YR_005;
								*/
								sprintf(fatbuf,"%02d℃",heat_unit);
						        gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
						break;
						case 5:	/*
						        key_group[3]->picbtnpathu=(u8*)PTH_YR_007;
								key_group[3]->picbtnpathd=(u8*)PTH_YR_007;
								*/
								sprintf(fatbuf,"%02d℃",heat_unit);
						        gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
						break;
						case 10:/*
						        key_group[3]->picbtnpathu=(u8*)PTH_YR_008;
								key_group[3]->picbtnpathd=(u8*)PTH_YR_008;
								*/
								sprintf(fatbuf,"%02d℃",heat_unit);
						        gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
						break;
						default:break;
					}
					key_group[4]->picbtnpathu=(u8*)PTH_YR_006;
					key_group[4]->picbtnpathd=(u8*)PTH_YR_006;
					key_group[5]->picbtnpathu=(u8*)PTH_FH;
					key_group[5]->picbtnpathd=(u8*)PTH_FH;
					
					for(i=0;i<num;i++)
					{
						btn_draw(key_group[i]);
					}
				}
			break;
			case 3:
				gui_phy.back_color = BACK2_DIY;
				gui_show_strmid(130,10,60,16,WHITE,16,"预热",0);
				//显示机器类型
				if(PrinterMode==0)
				{
					minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
				}
				else if(PrinterMode==1)
				{
					;
				}
			break;
			default:
			break;
		}
		if(display_redraw<3)
		{
			display_redraw++;
			return;
		}
		else
		{
			display_redraw=0;
			redraw_menu = FALSE;
		}
	#if 0
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
        
		minibmp_decode(PTH_YR_012,114,138,92,69,0,0);
		//显示当前温度
		memset(fatbuf,0,50);
		switch(preheat_choice)
		{
			case 0:
				sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[0],target_temperature[0]);
			break;
			case 1:
				sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[1],target_temperature[1]);
			break;
			case 2:
				sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_bed_temperature,target_temperature_bed);
			break;
			default:
			break;
		}
        gui_phy.back_color = BACK_DIY;
		gui_show_strmid(115,91,90,16,WHITE,16,fatbuf,0);
        //gui_phy.back_color = BACK2_DIY;
		//显示6个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
     		key_group[0]=btn_creat(16,49,92,69,0,1);
			key_group[1]=btn_creat(213,49,92,69,1,1);
			key_group[2]=btn_creat(16,138,92,69,2,1);
			key_group[3]=btn_creat(114,138,92,69,3,1);
			key_group[4]=btn_creat(212,138,92,69,4,1);
			key_group[5]=btn_creat(0,0,320,36,5,1);

			
			key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
			key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
			key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
			key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
			switch(preheat_choice)
			{
				case 0:	key_group[2]->picbtnpathu=(u8*)PTH_YR_002;
						key_group[2]->picbtnpathd=(u8*)PTH_YR_002;
						minibmp_decode(PTH_YR_009,146,62,27,24,0,0);
						
				break;
				case 1:	key_group[2]->picbtnpathu=(u8*)PTH_YR_003;
						key_group[2]->picbtnpathd=(u8*)PTH_YR_003;
						minibmp_decode(PTH_YR_010,146,62,27,24,0,0);
				break;
				case 2:	key_group[2]->picbtnpathu=(u8*)PTH_YR_004;
						key_group[2]->picbtnpathd=(u8*)PTH_YR_004;
						minibmp_decode(PTH_YR_011,146,62,27,24,0,0);
				break;
				default:break;
			}
            gui_phy.back_color = BACK2_DIY;
			switch(heat_unit)
			{
				case 1:	/*
				        key_group[3]->picbtnpathu=(u8*)PTH_YR_005;
						key_group[3]->picbtnpathd=(u8*)PTH_YR_005;
						*/
						sprintf(fatbuf,"%02d℃",heat_unit);
				        gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
				break;
				case 5:	/*
				        key_group[3]->picbtnpathu=(u8*)PTH_YR_007;
						key_group[3]->picbtnpathd=(u8*)PTH_YR_007;
						*/
						sprintf(fatbuf,"%02d℃",heat_unit);
				        gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
				break;
				case 10:/*
				        key_group[3]->picbtnpathu=(u8*)PTH_YR_008;
						key_group[3]->picbtnpathd=(u8*)PTH_YR_008;
						*/
						sprintf(fatbuf,"%02d℃",heat_unit);
				        gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
				break;
				default:break;
			}
			key_group[4]->picbtnpathu=(u8*)PTH_YR_006;
			key_group[4]->picbtnpathd=(u8*)PTH_YR_006;
			key_group[5]->picbtnpathu=(u8*)PTH_FH;
			key_group[5]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"预热",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
	#endif
	}
	//刷新温度显示
	gui_phy.back_color = BACK_DIY;
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 500;
		memset(fatbuf,0,50);
		switch(preheat_choice)
		{
			case 0:
				sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[0],target_temperature[0]);
			break;
			case 1:
				sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[1],target_temperature[1]);
			break;
			case 2:
				sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_bed_temperature,target_temperature_bed);
			break;
			default:
			break;
		}
		gui_show_strmid(115,91,90,16,WHITE,16,fatbuf,0);
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
			switch(preheat_choice)
			{
				case 0:
					if(target_temperature[0]>heat_unit)
						target_temperature[0] -= heat_unit;
					else
						target_temperature[0] = 0;
				break;
				case 1:
					if(target_temperature[1]>heat_unit)
						target_temperature[1] -= heat_unit;
					else
						target_temperature[1] = 0;
				break;
				case 2:
					if(target_temperature_bed>heat_unit)
						target_temperature_bed -= heat_unit;
					else
						target_temperature_bed = 0;
				break;
				default:
				break;
			}
			
		  break;
		  case 1:
			switch(preheat_choice)
			{
				case 0:
					if(target_temperature[0] < (HEATER_0_MAXTEMP-heat_unit))
						target_temperature[0] += heat_unit;
					else
						target_temperature[0] = HEATER_0_MAXTEMP;
				break;
				case 1:
					if(target_temperature[1] < (HEATER_1_MAXTEMP-heat_unit))
						target_temperature[1] += heat_unit;
					else
						target_temperature[1] = HEATER_1_MAXTEMP;
				break;
				case 2:
					if(target_temperature_bed < (BED_MAXTEMP-heat_unit))
						target_temperature_bed += heat_unit;
					else
						target_temperature_bed = BED_MAXTEMP;
				break;
				default:
				break;
			}
		  break;
		 case 2:
			preheat_choice += 1;	//加热设备选择
		 	preheat_choice %= 3;
			switch(preheat_choice)
			{
				case 0:	key_group[2]->picbtnpathu=(u8*)PTH_YR_002;
						key_group[2]->picbtnpathd=(u8*)PTH_YR_002;
						minibmp_decode(PTH_YR_009,146,62,27,24,0,0);
						
				break;
				case 1:	key_group[2]->picbtnpathu=(u8*)PTH_YR_003;
						key_group[2]->picbtnpathd=(u8*)PTH_YR_003;
						minibmp_decode(PTH_YR_010,146,62,27,24,0,0);
				break;
				case 2:	key_group[2]->picbtnpathu=(u8*)PTH_YR_004;
						key_group[2]->picbtnpathd=(u8*)PTH_YR_004;
						minibmp_decode(PTH_YR_011,146,62,27,24,0,0);
				break;
				default:break;
			}
			btn_draw(key_group[2]);	
		 	break;
		 case 3:
            gui_phy.back_color = BACK2_DIY;
			if(heat_unit==1)
			{
				heat_unit = 5;
				//key_group[3]->picbtnpathu=(u8*)PTH_YR_007;
				//key_group[3]->picbtnpathd=(u8*)PTH_YR_007;
				key_group[3]=btn_creat(114,138,92,69,3,1);
				sprintf(fatbuf,"%02d℃",heat_unit);
				gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
			}
			else if(heat_unit==5)
			{
				heat_unit = 10;
				//key_group[3]->picbtnpathu=(u8*)PTH_YR_008;
				//key_group[3]->picbtnpathd=(u8*)PTH_YR_008;
				sprintf(fatbuf,"%02d℃",heat_unit);
				gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
			}
			else if(heat_unit==10)
			{
				heat_unit = 1;
				//key_group[3]->picbtnpathu=(u8*)PTH_YR_005;
				//key_group[3]->picbtnpathd=(u8*)PTH_YR_005;
				sprintf(fatbuf,"%02d℃",heat_unit);
				gui_show_strmid(114+30,138+27,32,16,WHITE,16,fatbuf,0);
			}
			btn_draw(key_group[3]);	
		 	break;
		 case 4:
			switch(preheat_choice)
			{
				case 0:
					target_temperature[0] = 0;
				break;
				case 1:
					target_temperature[1] = 0;
				break;
				case 2:
					target_temperature_bed = 0;
				break;
				default:
				break;
			}
		 	break;
		 case 5:
		 	redraw_menu = TRUE;
			CurrentMenu = lastMenu;
		 	break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
u8 factor_move=1;	//移动因子 1mm 5mm 10mm
void move1_diy(void)				//移动控制显示界面
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 12;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		
		
		//显示12个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
     		key_group[0]=btn_creat(16,49,61,54,0,1);
			key_group[1]=btn_creat(86,49,61,54,1,1);
			key_group[2]=btn_creat(156,49,61,54,2,1);
			key_group[3]=btn_creat(225,49,79,53,3,1);
			key_group[4]=btn_creat(16,109,61,54,4,1);
			key_group[5]=btn_creat(86,109,61,54,5,1);
			key_group[6]=btn_creat(156,109,61,54,6,1);
			key_group[7]=btn_creat(16,168,61,54,7,1);
			key_group[8]=btn_creat(86,168,61,54,8,1);
			key_group[9]=btn_creat(156,168,61,54,9,1);
			key_group[10]=btn_creat(225,168,79,53,10,1);
			key_group[11]=btn_creat(0,0,320,36,5,1);

			
			key_group[0]->picbtnpathu=(u8*)PTH_YD_000;
			key_group[0]->picbtnpathd=(u8*)PTH_YD_000;
			key_group[1]->picbtnpathu=(u8*)PTH_YD_001;
			key_group[1]->picbtnpathd=(u8*)PTH_YD_001;
			key_group[2]->picbtnpathu=(u8*)PTH_YD_002;
			key_group[2]->picbtnpathd=(u8*)PTH_YD_002;
			if(active_extruder==0)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_YD_003;
				key_group[3]->picbtnpathd=(u8*)PTH_YD_003;
			}
			else if(active_extruder)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_YD_004;
				key_group[3]->picbtnpathd=(u8*)PTH_YD_004;
			}
			key_group[4]->picbtnpathu=(u8*)PTH_YD_005;
			key_group[4]->picbtnpathd=(u8*)PTH_YD_005;
			key_group[5]->picbtnpathu=(u8*)PTH_YD_006;
			key_group[5]->picbtnpathd=(u8*)PTH_YD_006;
			key_group[6]->picbtnpathu=(u8*)PTH_YD_007;
			key_group[6]->picbtnpathd=(u8*)PTH_YD_007;
			key_group[7]->picbtnpathu=(u8*)PTH_YD_008;
			key_group[7]->picbtnpathd=(u8*)PTH_YD_008;
			key_group[8]->picbtnpathu=(u8*)PTH_YD_009;
			key_group[8]->picbtnpathd=(u8*)PTH_YD_009;
			key_group[9]->picbtnpathu=(u8*)PTH_YD_010;
			key_group[9]->picbtnpathd=(u8*)PTH_YD_010;
			if(factor_move==1)
			{
				key_group[10]->picbtnpathu=(u8*)PTH_YD_011;
				key_group[10]->picbtnpathd=(u8*)PTH_YD_011;
			}
			else if(factor_move==5)
			{
				key_group[10]->picbtnpathu=(u8*)PTH_YD_012;
				key_group[10]->picbtnpathd=(u8*)PTH_YD_012;
			}
			else if(factor_move==10)
			{
				key_group[10]->picbtnpathu=(u8*)PTH_YD_013;
				key_group[10]->picbtnpathd=(u8*)PTH_YD_013;
			}
			key_group[11]->picbtnpathu=(u8*)PTH_FH;
			key_group[11]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"移动",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 E%.1f F%d",(float)(-factor_move),1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 1:
			enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 Y%.1f F%d",(float)(-factor_move),1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 2:
			enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 Z%.1f F%d",(float)factor_move,1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 3:
			if(active_extruder==0)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_YD_004;
				key_group[3]->picbtnpathd=(u8*)PTH_YD_004;
				active_extruder = 1;
			}
			else if(active_extruder==1)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_YD_003;
				key_group[3]->picbtnpathd=(u8*)PTH_YD_003;
				active_extruder = 0;
			}
			btn_draw(key_group[3]);
		  break;
		  case 4:
			enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 X%.1f F%d",(float)(-factor_move),1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 5:
		 	enquecommand("G28");
		  break;
		  case 6:
		 	enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 X%.1f F%d",(float)factor_move,1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 7:
		 	enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 E%.1f F%d",(float)factor_move,1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 8:
		 	enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 Y%.1f F%d",(float)factor_move,1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 9:
		 	enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 Z%.1f F%d",(float)(-factor_move),1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 10:
		 	if(factor_move==1)
		 	{
				key_group[10]->picbtnpathu=(u8*)PTH_YD_012;
				key_group[10]->picbtnpathd=(u8*)PTH_YD_012;
				factor_move = 5;
			}
			else if(factor_move==5)
		 	{
				key_group[10]->picbtnpathu=(u8*)PTH_YD_013;
				key_group[10]->picbtnpathd=(u8*)PTH_YD_013;
				factor_move = 10;
			}
			else if(factor_move==10)
		 	{
				key_group[10]->picbtnpathu=(u8*)PTH_YD_011;
				key_group[10]->picbtnpathd=(u8*)PTH_YD_011;
				factor_move = 1;
			}
			btn_draw(key_group[10]);
		  break;
		  case 11:
		 	redraw_menu = TRUE;
			enquecommand("T0");
			CurrentMenu = control_3d_diy;
		  break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
void zero_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 6;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		
		
		//显示6个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
			key_group[0]=btn_creat(16,62,92,69,0,1);
			key_group[1]=btn_creat(114,62,92,69,1,1);
			key_group[2]=btn_creat(212,62,92,69,2,1);
			key_group[3]=btn_creat(16,149,92,69,3,1);
			key_group[4]=btn_creat(212,149,92,69,4,1);
			key_group[5]=btn_creat(0,0,320,36,5,1);

			key_group[0]->picbtnpathu=(u8*)PTH_GL_000;
			key_group[0]->picbtnpathd=(u8*)PTH_GL_000;
			key_group[1]->picbtnpathu=(u8*)PTH_GL_001;
			key_group[1]->picbtnpathd=(u8*)PTH_GL_001;
			key_group[2]->picbtnpathu=(u8*)PTH_GL_002;
			key_group[2]->picbtnpathd=(u8*)PTH_GL_002;
			key_group[3]->picbtnpathu=(u8*)PTH_GL_003;
			key_group[3]->picbtnpathd=(u8*)PTH_GL_003;
			key_group[4]->picbtnpathu=(u8*)PTH_GL_004;
			key_group[4]->picbtnpathd=(u8*)PTH_GL_004;
			key_group[5]->picbtnpathu=(u8*)PTH_FH;
			key_group[5]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"归零",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			enquecommand("G28 X");
		  break;
		  case 1:
			enquecommand("G28 Y");
		  break;
		  case 2:
		  	enquecommand("G28 Z");
		  break;
		  case 3:
			enquecommand("G28");
		  break;
		  case 4:
			quickStop();
		  break;
		  case 5:
			redraw_menu = TRUE;
			CurrentMenu = control_3d_diy;
		  break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}

}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
static u8 factor_extruder=1;//挤出头值 1mm 5mm 10mm 
void extruder_diy(void)					//挤出头控制显示界面
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 6;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//画状态背景图
		minibmp_decode(PTH_JC_010,15,49,134,109,0,0);
		//画两个挤出头
		minibmp_decode(PTH_JC_000,27,58,20,35,0,0);
		minibmp_decode(PTH_JC_001,27,108,20,35,0,0);
		//画挤出头背景图
		minibmp_decode(PTH_JC_010,170,49,134,109,0,0);
        minibmp_decode(PTH_JC_012,16,174,69,48,0,0);
        minibmp_decode(PTH_JC_012,89,174,69,48,0,0);

        
		memset(fatbuf,0,50);
		sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[0],target_temperature[0]);	
        gui_phy.back_color = BACK_DIY;
		gui_show_ptstr(53,69,53+88,69+16,0,WHITE,16,fatbuf,0);

		memset(fatbuf,0,50);
		sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[1],target_temperature[1]);			
		gui_show_ptstr(53,119,53+88,119+16,0,WHITE,16,fatbuf,0);
        gui_phy.back_color = BACK2_DIY;
		//显示6个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建六个图标
		if(key_group)
		{
			key_group[0]=btn_creat(207,79,59,50,0,1);
			key_group[1]=btn_creat(16+23,174,23,21,1,1);
			key_group[2]=btn_creat(89+23,174,22,21,2,1);
			key_group[3]=btn_creat(162,174,69,48,3,1);
			key_group[4]=btn_creat(234,174,69,48,4,1);
			key_group[5]=btn_creat(0,0,320,36,5,1);
			if(active_extruder==0)
			{
				key_group[0]->picbtnpathu=(u8*)PTH_JC_002;
				key_group[0]->picbtnpathd=(u8*)PTH_JC_002;
				
			}
			else if(active_extruder==1)
			{
				key_group[0]->picbtnpathu=(u8*)PTH_JC_003;
				key_group[0]->picbtnpathd=(u8*)PTH_JC_003;
				enquecommand("T0");
			}
			key_group[1]->picbtnpathu=(u8*)PTH_JC_004;
			key_group[1]->picbtnpathd=(u8*)PTH_JC_004;
			key_group[2]->picbtnpathu=(u8*)PTH_JC_005;
			key_group[2]->picbtnpathd=(u8*)PTH_JC_005;
			if(factor_extruder==1)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_JC_006;
				key_group[3]->picbtnpathd=(u8*)PTH_JC_006;
			}
			else if(factor_extruder==5)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_JC_007;
				key_group[3]->picbtnpathd=(u8*)PTH_JC_007;
			}
			else if(factor_extruder==10)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_JC_008;
				key_group[3]->picbtnpathd=(u8*)PTH_JC_008;
			}

			key_group[4]->picbtnpathu=(u8*)PTH_JC_009;
			key_group[4]->picbtnpathd=(u8*)PTH_JC_009;
			key_group[5]->picbtnpathu=(u8*)PTH_FH;
			key_group[5]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"挤出",0);
        gui_show_strmid(16,174+25,69,16,WHITE,16,"进料",0);
        gui_show_strmid(89,174+25,69,16,WHITE,16,"出料",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}

		
	}
	//刷新温度显示
	gui_phy.back_color = BACK_DIY;
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 500;
		memset(fatbuf,0,50);
		sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[0],target_temperature[0]);			
		gui_show_ptstr(53,69,53+88,69+16,0,WHITE,16,fatbuf,0);

		memset(fatbuf,0,50);
		sprintf(fatbuf,"%03d℃/%03d℃",(s16)show_extruder_temperature[1],target_temperature[1]);			
		gui_show_ptstr(53,119,53+88,119+16,0,WHITE,16,fatbuf,0);
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
		  	if(active_extruder==0)
		  	{
				key_group[0]->picbtnpathu=(u8*)PTH_JC_003;
				key_group[0]->picbtnpathd=(u8*)PTH_JC_003;
				active_extruder = 1;
			}
			else if(active_extruder==1)
			{
				key_group[0]->picbtnpathu=(u8*)PTH_JC_002;
				key_group[0]->picbtnpathd=(u8*)PTH_JC_002;
				active_extruder = 0;
				
			}
			btn_draw(key_group[0]);
		  break;
		  case 1:
		  	enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 E%.1f F%d",(float)factor_extruder,600);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 2:
		  	enquecommand("G91");
			memset(fatbuf,0,50);
		    sprintf(fatbuf, "G1 E%.1f F%d",(float)(-factor_extruder),1200);
			enquecommand(fatbuf);
			enquecommand("G90");
		  break;
		  case 3:
		  	if(factor_extruder==1)
		  	{
				factor_extruder = 5;
				key_group[3]->picbtnpathu=(u8*)PTH_JC_007;
				key_group[3]->picbtnpathd=(u8*)PTH_JC_007;
			}
			else if(factor_extruder==5)
		  	{
				factor_extruder = 10;
				key_group[3]->picbtnpathu=(u8*)PTH_JC_008;
				key_group[3]->picbtnpathd=(u8*)PTH_JC_008;
			}
			else if(factor_extruder==10)
		  	{
				factor_extruder = 1;
				key_group[3]->picbtnpathu=(u8*)PTH_JC_006;
				key_group[3]->picbtnpathd=(u8*)PTH_JC_006;
			}
			btn_draw(key_group[3]);
		  break;
		  case 4:
			quickStop();
		  break;
		  case 5:
		  	enquecommand("T0");
			redraw_menu = TRUE;
			CurrentMenu = control_3d_diy;
		  break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
void leveling_diy(void)
{

}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
static u8 fan_diy_num=0;	//控制风扇号 0 1 2
static u8 fan_diy_speed=10;	//控制风扇速度 10 20 50
#define FAN_SPEED_MIN 50//风扇最低速度
void fan_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 6;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		switch(display_redraw)
		{
			case 0:
				//画背景色
				gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
			break;
			case 1:
				//显示当前风扇速度值
				gui_fill_rectangle(150,96,20,16,BACK_DIY);
				switch(fan_diy_num)
				{
					case 0:
						sprintf(fatbuf,"%03d",fanSpeed);
						gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
						//显示风扇小图标
						minibmp_decode(PTH_FS_000,148,63,24,26,0,0);
					break;
					case 1:
						sprintf(fatbuf,"%03d",fanSpeed1);
						gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
						minibmp_decode(PTH_FS_008,148,63,24,26,0,0);
					break;
		        #ifdef FAN3_SHOW
					case 2:
						sprintf(fatbuf,"%03d",fanSpeed2);
						gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
						minibmp_decode(PTH_FS_009,148,63,24,26,0,0);
					break;
		        #endif
					default:break;
				}
			break;
			case 2:
				//显示6个按钮图标
				key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
				if(key_group)
				{
		     		key_group[0]=btn_creat(16,49,92,69,0,1);
					key_group[1]=btn_creat(212,49,92,69,1,1);
					key_group[2]=btn_creat(16,143,92,69,2,1);
					key_group[3]=btn_creat(114,143,92,69,3,1);
					key_group[4]=btn_creat(212,143,92,69,4,1);
					key_group[5]=btn_creat(0,0,320,36,5,1);

					
					key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
					key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
					key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
					key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
					if(fan_diy_num==0)
					{
						key_group[2]->picbtnpathu=(u8*)PTH_FS_001;
						key_group[2]->picbtnpathd=(u8*)PTH_FS_001;
					}
					else if(fan_diy_num==1)
					{
						key_group[2]->picbtnpathu=(u8*)PTH_FS_002;
						key_group[2]->picbtnpathd=(u8*)PTH_FS_002;
					}
					else if(fan_diy_num==2)
					{
						key_group[2]->picbtnpathu=(u8*)PTH_FS_003;
						key_group[2]->picbtnpathd=(u8*)PTH_FS_003;
					}
					if(fan_diy_speed==10)
					{
						key_group[3]->picbtnpathu=(u8*)PTH_FS_004;
						key_group[3]->picbtnpathd=(u8*)PTH_FS_004;
					}
					else if(fan_diy_speed==20)
					{
						key_group[3]->picbtnpathu=(u8*)PTH_FS_005;
						key_group[3]->picbtnpathd=(u8*)PTH_FS_005;
					}
					else if(fan_diy_speed==50)
					{
						key_group[3]->picbtnpathu=(u8*)PTH_FS_006;
						key_group[3]->picbtnpathd=(u8*)PTH_FS_006;
					}
					key_group[4]->picbtnpathu=(u8*)PTH_FS_007;
					key_group[4]->picbtnpathd=(u8*)PTH_FS_007;
					key_group[5]->picbtnpathu=(u8*)PTH_FH;
					key_group[5]->picbtnpathd=(u8*)PTH_FH;
					
					for(i=0;i<num;i++)
					{
						btn_draw(key_group[i]);
					}
				}
			break;
			case 3:
				gui_phy.back_color = BACK2_DIY;
				gui_show_strmid(130,10,60,16,WHITE,16,"风扇",0);
				//显示机器类型
				if(PrinterMode==0)
				{
					minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
				}
				else if(PrinterMode==1)
				{
					;
				}
			break;
			default:
			break;
		}
		if(display_redraw<3)
		{
			display_redraw++;
			return;
		}
		else
		{
			display_redraw=0;
			redraw_menu = FALSE;
		}
	#if 0
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		
		
		//显示当前风扇速度值
		gui_fill_rectangle(150,96,20,16,BACK_DIY);
		switch(fan_diy_num)
		{
			case 0:
				sprintf(fatbuf,"%03d",fanSpeed);
				gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
				//显示风扇小图标
				minibmp_decode(PTH_FS_000,148,63,24,26,0,0);
			break;
			case 1:
				sprintf(fatbuf,"%03d",fanSpeed1);
				gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
				minibmp_decode(PTH_FS_008,148,63,24,26,0,0);
			break;
        #ifdef FAN3_SHOW
			case 2:
				sprintf(fatbuf,"%03d",fanSpeed2);
				gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
				minibmp_decode(PTH_FS_009,148,63,24,26,0,0);
			break;
        #endif
			default:break;
		}
		//显示6个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
		if(key_group)
		{
     		key_group[0]=btn_creat(16,49,92,69,0,1);
			key_group[1]=btn_creat(212,49,92,69,1,1);
			key_group[2]=btn_creat(16,143,92,69,2,1);
			key_group[3]=btn_creat(114,143,92,69,3,1);
			key_group[4]=btn_creat(212,143,92,69,4,1);
			key_group[5]=btn_creat(0,0,320,36,5,1);

			
			key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
			key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
			key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
			key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
			if(fan_diy_num==0)
			{
				key_group[2]->picbtnpathu=(u8*)PTH_FS_001;
				key_group[2]->picbtnpathd=(u8*)PTH_FS_001;
			}
			else if(fan_diy_num==1)
			{
				key_group[2]->picbtnpathu=(u8*)PTH_FS_002;
				key_group[2]->picbtnpathd=(u8*)PTH_FS_002;
			}
			else if(fan_diy_num==2)
			{
				key_group[2]->picbtnpathu=(u8*)PTH_FS_003;
				key_group[2]->picbtnpathd=(u8*)PTH_FS_003;
			}
			if(fan_diy_speed==10)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_FS_004;
				key_group[3]->picbtnpathd=(u8*)PTH_FS_004;
			}
			else if(fan_diy_speed==20)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_FS_005;
				key_group[3]->picbtnpathd=(u8*)PTH_FS_005;
			}
			else if(fan_diy_speed==50)
			{
				key_group[3]->picbtnpathu=(u8*)PTH_FS_006;
				key_group[3]->picbtnpathd=(u8*)PTH_FS_006;
			}
			key_group[4]->picbtnpathu=(u8*)PTH_FS_007;
			key_group[4]->picbtnpathd=(u8*)PTH_FS_007;
			key_group[5]->picbtnpathu=(u8*)PTH_FH;
			key_group[5]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"风扇",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		
	#endif	
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		gui_fill_rectangle(150,96,20,16,BACK_DIY);
		switch(selx & ~(3<<6))
		{   
		  case 0:
			switch(fan_diy_num)
			{
				case 0:
					if(((fanSpeed - FAN_SPEED_MIN) > fan_diy_speed) && (fanSpeed > FAN_SPEED_MIN))
						fanSpeed -= fan_diy_speed;
                    else if((fanSpeed >= 0) && (fanSpeed < FAN_SPEED_MIN)){
                        ;
                    }
					else fanSpeed=FAN_SPEED_MIN;
					
					sprintf(fatbuf,"%03d",fanSpeed);
				break;
				case 1:
					if(((fanSpeed1 - FAN_SPEED_MIN) > fan_diy_speed) && (fanSpeed1 > FAN_SPEED_MIN))
						fanSpeed1 -= fan_diy_speed;
                    else if((fanSpeed1 >= 0) && (fanSpeed1 < FAN_SPEED_MIN)){
                        ;
                    }
					else fanSpeed1=FAN_SPEED_MIN;
					sprintf(fatbuf,"%03d",fanSpeed1);
				break;
            #ifdef FAN3_SHOW
				case 2:
					if(((fanSpeed2 - FAN_SPEED_MIN) > fan_diy_speed) && (fanSpeed2 > FAN_SPEED_MIN))
						fanSpeed2 -= fan_diy_speed;
                    else if((fanSpeed2 >= 0) && (fanSpeed2 < FAN_SPEED_MIN)){
                        ;
                    }
					else fanSpeed2=FAN_SPEED_MIN;
					sprintf(fatbuf,"%03d",fanSpeed2);
				break;
            #endif
				default:
				break;
			}
			gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
		  break;
		  case 1:
		  	gui_fill_rectangle(150,96,20,16,BACK_DIY);
			switch(fan_diy_num)
			{
				case 0:
					if(fanSpeed < (255-fan_diy_speed))
						fanSpeed += fan_diy_speed;
					else fanSpeed = 255;
					sprintf(fatbuf,"%03d",fanSpeed);
				break;
				case 1:
					if(fanSpeed1 < (255-fan_diy_speed))
						fanSpeed1 += fan_diy_speed;
					else fanSpeed1 = 255;
					sprintf(fatbuf,"%03d",fanSpeed1);
				break;
				case 2:
					if(fanSpeed2 < (255-fan_diy_speed))
						fanSpeed2 += fan_diy_speed;
					else fanSpeed2 = 255;
					sprintf(fatbuf,"%03d",fanSpeed2);
				break;
				default:
				break;
			}
			gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
		  break;
		 case 2:
			fan_diy_num += 1;	
            
         #ifdef FAN3_SHOW   
		 	fan_diy_num %= 3;
         #else
            fan_diy_num %= 2;
         #endif
         
		switch(fan_diy_num)
		{
			case 0:
				sprintf(fatbuf,"%03d",fanSpeed);
				gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
				key_group[2]->picbtnpathu=(u8*)PTH_FS_001;
				key_group[2]->picbtnpathd=(u8*)PTH_FS_001;
				//显示风扇小图标
				minibmp_decode(PTH_FS_000,148,63,24,26,0,0);
			break;
			case 1:
				sprintf(fatbuf,"%03d",fanSpeed1);
				gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
				key_group[2]->picbtnpathu=(u8*)PTH_FS_002;
				key_group[2]->picbtnpathd=(u8*)PTH_FS_002;
				minibmp_decode(PTH_FS_008,148,63,24,26,0,0);
			break;
        #ifdef FAN3_SHOW
			case 2:
				sprintf(fatbuf,"%03d",fanSpeed2);
				gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
				key_group[2]->picbtnpathu=(u8*)PTH_FS_003;
				key_group[2]->picbtnpathd=(u8*)PTH_FS_003;
				minibmp_decode(PTH_FS_009,148,63,24,26,0,0);
			break;
        #endif
			default:break;
		}
			gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
			btn_draw(key_group[2]);	
		 break;
		 case 3:
			if(fan_diy_speed==10)
			{
				fan_diy_speed = 20;
				key_group[3]->picbtnpathu=(u8*)PTH_FS_005;
				key_group[3]->picbtnpathd=(u8*)PTH_FS_005;
			}
			else if(fan_diy_speed==20)
			{
				fan_diy_speed = 50;
				key_group[3]->picbtnpathu=(u8*)PTH_FS_006;
				key_group[3]->picbtnpathd=(u8*)PTH_FS_006;
			}
			else if(fan_diy_speed==50)
			{
				fan_diy_speed = 10;
				key_group[3]->picbtnpathu=(u8*)PTH_FS_004;
				key_group[3]->picbtnpathd=(u8*)PTH_FS_004;
			}
            gui_show_strmid(148,96,24,16,WHITE,16,fatbuf,0);
			btn_draw(key_group[3]);	
		 break;
		 case 4:
			gui_show_strmid(148,96,24,16,WHITE,16,"000",0);
			switch(fan_diy_num)
			{
				case 0:
					fanSpeed = 0;
				break;
				case 1:
					fanSpeed1 = 0;
				break;
            #ifdef FAN3_SHOW
				case 2:
					fanSpeed2 = 0;
				break;
            #endif
				default:
				break;
			}
		 	break;
		 case 5:
		 	redraw_menu = TRUE;
			CurrentMenu = lastMenu;
		 break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
void about_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//fill back color
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//========================================================================
		gui_show_ptstr(100,60,260,116,0,WHITE,16,(u8*)COM_PILE_DATE,1);
		gui_show_ptstr(100,80,260,116,0,WHITE,16,(u8*)COM_PILE_TIME,1);
		//========================================================================
		//显示信息
#ifdef LASER
		gui_show_ptstr(100,100,260,116,0,WHITE,16,(u8*)LASR_VERSION,1);
#else
		
		gui_show_ptstr(100,100,260,116,0,WHITE,16,(u8*)SW_VERSION,1);
#endif
		gui_show_ptstr(100,120,260,136,0,WHITE,16,(u8*)WIFI_VERSION,1);
		gui_show_strmid(0,140,320,16,WHITE,16,(u8*)CO_TD_INFO,0);				//生产公司名称
		
		//显示6个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{

			key_group[0]=btn_creat(0,0,320,36,5,1);
			key_group[0]->picbtnpathu=(u8*)PTH_FH;
			key_group[0]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"关于",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		
		  case 0:
			redraw_menu = TRUE;
			CurrentMenu = set_diy;
		  break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
void language_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		//显示三个可选图标
		minibmp_decode(PTH_YY_000,18,65,286,36,0,0);
		minibmp_decode(PTH_YY_001,18,115,286,36,0,0);
		minibmp_decode(PTH_YY_002,18,164,286,36,0,0);
		minibmp_decode(PTH_YY_003,18,215,18,18,0,0);
		//display one buttun picture
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//create three button picture
		if(key_group)
		{
			key_group[0]=btn_creat(0,0,320,36,5,1);
			key_group[0]->picbtnpathu=(u8*)PTH_FH;
			key_group[0]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"语言",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			redraw_menu = TRUE;
			CurrentMenu = set_diy;
		  break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
//状态
void status_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	
	gui_phy.back_color = BACK2_DIY;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		//画背景色
		gui_phy.back_color = BACK_DIY;
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		gui_phy.back_color = BACK2_DIY;
		//画框图
		minibmp_decode(PTH_ZT_003,16,49,289,173,0,0);
		//显示图标
		minibmp_decode(PTH_ZT_000,16+16,108,17,29,0,0);//103
		minibmp_decode(PTH_ZT_001,16+16,148,17,29,0,0);
		minibmp_decode(PTH_ZT_002,16+16,188,21,19,0,0);
        
        //minibmp_decode(PTH_ZT_001,16,148,17,29,0,0);
		//minibmp_decode(PTH_ZT_002,16,188,21,19,0,0);
        minibmp_decode(PTH_DYKZ_008,223-30,108,26,26,0,0);
        minibmp_decode(PTH_DYKZ_009,223-30,148,26,26,0,0);
       
        minibmp_decode(PTH_DYKZ_004,223-30,188,26,26,0,0);
		//显示坐标
		memset(fatbuf,0,100);
		sprintf(fatbuf,"X:%3.1f Y:%3.1f Z:%3.1f",current_position[0],current_position[1],current_position[2]);			
		gui_show_ptstr(96,86,292,116,0,WHITE,16,fatbuf,0);
		//显示挤出头底板温度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃/%03d℃",(int)show_extruder_temperature[0],target_temperature[0]);			
		gui_show_ptstr(32+20,116,52+100,116+16,0,WHITE,16,fatbuf,0);

		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃/%03d℃",(int)show_extruder_temperature[1],target_temperature[1]);			
		gui_show_ptstr(32+20,158,52+100,158+16,0,WHITE,16,fatbuf,0);

		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃/%03d℃",(int)show_bed_temperature,target_temperature_bed);			
		gui_show_ptstr(32+20,194,52+100,194+16,0,WHITE,16,fatbuf,0);
        
        //风扇1
        memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d",(int)fanSpeed);			
		gui_show_ptstr(223,116,289,116+16,0,WHITE,16,fatbuf,0);
        //风扇2
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d",(int)fanSpeed1);			
		gui_show_ptstr(223,158,289,158+16,0,WHITE,16,fatbuf,0);
        //速度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d%%",(int)feedmultiply);			
		gui_show_ptstr(223,194,289,194+16,0,WHITE,16,fatbuf,0);
        
		//显示6个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
			key_group[0]=btn_creat(0,0,320,36,5,1);
			key_group[0]->picbtnpathu=(u8*)PTH_FH;
			key_group[0]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"状态",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
			gui_show_ptstr(98,64,260,100,0,WHITE,16,"当前模式：3D打印",1);
		}
		else if(PrinterMode==1)
		{
			;
		}
	}
	//刷新温度显示
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 500;
		memset(fatbuf,0,100);
		sprintf(fatbuf,"X:%3.1f Y:%3.1f Z:%3.1f",current_position[0],current_position[1],current_position[2]);			
		gui_show_ptstr(96,86,292,116,0,WHITE,16,fatbuf,0);
		//显示挤出头底板温度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃/%03d℃",(int)show_extruder_temperature[0],target_temperature[0]);			
		gui_show_ptstr(32+20,116,52+100,116+16,0,WHITE,16,fatbuf,0);

		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃/%03d℃",(int)show_extruder_temperature[1],target_temperature[1]);			
		gui_show_ptstr(32+20,158,52+100,158+16,0,WHITE,16,fatbuf,0);

		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃/%03d℃",(int)show_bed_temperature,target_temperature_bed);			
		gui_show_ptstr(32+20,194,52+100,194+16,0,WHITE,16,fatbuf,0);

         //风扇1
        memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d%100",(int)fanSpeed);			
		gui_show_ptstr(223,116,289,116+16,0,WHITE,16,fatbuf,0);
        //风扇2
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d%100",(int)fanSpeed1);			
		gui_show_ptstr(223,158,289,158+16,0,WHITE,16,fatbuf,0);
        //速度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d%100",(int)feedmultiply);			
		gui_show_ptstr(223,194,289,194+16,0,WHITE,16,fatbuf,0);
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			redraw_menu = TRUE;
			CurrentMenu = set_diy;
		  break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
//wifi
void wifi_diy(void)
{

}

void progressbar_diy(u16 sx,u16 sy,u8 progress)
{
	int lenth;
	u8 str[5];
	if(progress>100)
		progress=100;
	//绘制已完成的矩形框
	lenth = 1.1*progress; // 110/100*progress
	gui_fill_rectangle(sx,sy,lenth,16,GREEN);
	//绘制剩余长度
	gui_fill_rectangle(sx+lenth,sy,110-lenth,16,BACK_DIY);
	//显示百分比
	sprintf(str,"%d%%",progress);
	gui_show_strmid(sx+110,sy,32,16,WHITE,16,str,0);
}

/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
u8 old_print_stat=0;
u8 old_print_pross=0;	//历史打印进度
u8 display_switch=0;
uint8_t last_bedtem = 0,last_min = 0;							//used to refresh temperature
uint16_t last_temp1 = 0,last_temp2 = 0;
float last_zlength = 0;														//used to refresh printing high
void print_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 3;
	
	gui_phy.back_color = BACK2_DIY;
	if(redraw_menu)
	{
		switch(display_redraw)
		{
			case 0:
				gui_phy.back_color = BACK_DIY;
				//画背景色
				gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
			break;
			case 1:
				//显示缩略图
				minibmp_decode(PTH_DY_000,17,24,127,138,0,0);
				//显示矩形框
				minibmp_decode(PTH_DY_011,151,24,153,138,0,0);
			break;
			case 2:
				//显示打印进度
				progressbar_diy(154,34,PrintInfo.printper);
			break;
			case 3:
				//显示状态信息
				minibmp_decode(PTH_DY_001,155,59,23,23,0,0);
				minibmp_decode(PTH_DY_002,235,59,23,23,0,0);
				minibmp_decode(PTH_DY_003,155,98,23,23,0,0);
				minibmp_decode(PTH_DY_004,235,98,23,23,0,0);
				minibmp_decode(PTH_DY_005,155,135,23,23,0,0);
			break;
			case 4:
				gui_phy.back_color = BACK2_DIY;
				old_print_stat = card.sdprinting;
				old_print_pross=0;
				display_switch=0;
				//打印时长
				memset(fatbuf,0,100);
				//sprintf(fatbuf,"%dh%02dm",PrintInfo.printtime/3600,PrintInfo.printtime/60%60);	
				sprintf(fatbuf,"%dh%02dm",(usr_timer.date *24 + usr_timer.hour),usr_timer.min);
				gui_show_ptstr(180,67,180+64,67+16,0,WHITE,16,fatbuf,0);
				//显示挤出头1温度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[0]);			
				gui_show_ptstr(260,67,260+40,67+16,0,WHITE,16,fatbuf,0);
				//底板温度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d℃",(int)show_bed_temperature);			
				gui_show_ptstr(180,103,180+40,103+16,0,WHITE,16,fatbuf,0);
				//挤出头2温度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[1]);			
				gui_show_ptstr(260,103,260+40,103+16,0,WHITE,16,fatbuf,0);
				//打印速度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d%%",(int)feedmultiply);			
				gui_show_ptstr(180,140,180+40,140+16,0,WHITE,16,fatbuf,0);
				//打印Z轴高度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"Z:%.2fmm",current_position[Z_AXIS]);			
				gui_show_ptstr(225,140,235+88,140+16,0,WHITE,16,fatbuf,0);
			break;
			case 5:
				//显示3个按钮图标
				key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
				if(key_group)
				{
					key_group[0]=btn_creat(14,184,90,47,5,1);
					if(card.sdprinting==TRUE)
					{
						key_group[0]->picbtnpathu=(u8*)PTH_DY_006;
						key_group[0]->picbtnpathd=(u8*)PTH_DY_006;
		                //gui_show_ptstr(14+48,184+8,62+16,192+16,0,BLUE,16,"暂停",0);
					}
					else
					{
						key_group[0]->picbtnpathu=(u8*)PTH_DY_007;
						key_group[0]->picbtnpathd=(u8*)PTH_DY_007;
		                //gui_show_ptstr(14+48,184+8,62+16,192+16,0,BLUE,16,"打印",0);
					}
					
					key_group[1]=btn_creat(114,184,90,47,5,1);
					key_group[1]->picbtnpathu=(u8*)PTH_DY_008;
					key_group[1]->picbtnpathd=(u8*)PTH_DY_008;
		            //gui_show_ptstr(114+48,184+8,162+16,192+16,0,BLUE,16,"停止",0);
					key_group[2]=btn_creat(213,184,90,47,5,1);
					key_group[2]->picbtnpathu=(u8*)PTH_DY_009;
					key_group[2]->picbtnpathd=(u8*)PTH_DY_009;	
		            //gui_show_ptstr(213+48,184+8,261+16,192+16,0,BLUE,16,"控制",0);
					for(i=0;i<num;i++)
					{
						btn_draw(key_group[i]);
					}
		            
		            if(card.sdprinting==TRUE)
		                gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"暂停",0);
		            else
		                gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"打印",0);
		            
		            gui_show_ptstr(114+48,184+16,186+32,200+16,0,WHITE,16,"停止",0);
		            gui_show_ptstr(213+48,184+16,285+32,200+16,0,WHITE,16,"控制",0);
				}
			break;
			default:
			break;
		}
		if(display_redraw<5)
		{
			display_redraw++;
			return;
		}
		else
		{
			display_redraw=0;
			redraw_menu = FALSE;
		}
		#if 0
		redraw_menu = FALSE;
		gui_phy.back_color = BACK_DIY;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		gui_phy.back_color = BACK2_DIY;
		//显示缩略图
		minibmp_decode(PTH_DY_000,17,24,127,138,0,0);

		//显示矩形框
		minibmp_decode(PTH_DY_011,151,24,153,138,0,0);
		//显示打印进度
		progressbar_diy(154,34,PrintInfo.printper);
		//显示状态信息
		minibmp_decode(PTH_DY_001,155,59,23,23,0,0);
		minibmp_decode(PTH_DY_002,235,59,23,23,0,0);
		minibmp_decode(PTH_DY_003,155,98,23,23,0,0);
		minibmp_decode(PTH_DY_004,235,98,23,23,0,0);
		minibmp_decode(PTH_DY_005,155,135,23,23,0,0);

		
		old_print_stat = card.sdprinting;
		old_print_pross=0;
		display_switch=0;
		//打印时长
		memset(fatbuf,0,100);
		///sprintf(fatbuf,"%dh%02dm",PrintInfo.printtime/3600,PrintInfo.printtime/60%60);		
		sprintf(fatbuf,"%dh%02dm",(usr_timer.date *24 + usr_timer.hour),usr_timer.min);		
		gui_show_ptstr(180,67,180+64,67+16,0,WHITE,16,fatbuf,0);
		//显示挤出头1温度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[0]);			
		gui_show_ptstr(260,67,260+40,67+16,0,WHITE,16,fatbuf,0);
		//底板温度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃",(int)show_bed_temperature);			
		gui_show_ptstr(180,103,180+40,103+16,0,WHITE,16,fatbuf,0);
		//挤出头2温度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[1]);			
		gui_show_ptstr(260,103,260+40,103+16,0,WHITE,16,fatbuf,0);
		//打印速度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d%%",(int)feedmultiply);			
		gui_show_ptstr(180,140,180+40,140+16,0,WHITE,16,fatbuf,0);
		//打印Z轴高度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"Z:%.2fmm",current_position[Z_AXIS]);			
		gui_show_ptstr(225,140,235+88,140+16,0,WHITE,16,fatbuf,0);
		//显示3个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
			key_group[0]=btn_creat(14,184,90,47,5,1);
			if(card.sdprinting==TRUE)
			{
				key_group[0]->picbtnpathu=(u8*)PTH_DY_006;
				key_group[0]->picbtnpathd=(u8*)PTH_DY_006;
                //gui_show_ptstr(14+48,184+8,62+16,192+16,0,BLUE,16,"暂停",0);
			}
			else
			{
				key_group[0]->picbtnpathu=(u8*)PTH_DY_007;
				key_group[0]->picbtnpathd=(u8*)PTH_DY_007;
                //gui_show_ptstr(14+48,184+8,62+16,192+16,0,BLUE,16,"打印",0);
			}
			
			key_group[1]=btn_creat(114,184,90,47,5,1);
			key_group[1]->picbtnpathu=(u8*)PTH_DY_008;
			key_group[1]->picbtnpathd=(u8*)PTH_DY_008;
            //gui_show_ptstr(114+48,184+8,162+16,192+16,0,BLUE,16,"停止",0);
			key_group[2]=btn_creat(213,184,90,47,5,1);
			key_group[2]->picbtnpathu=(u8*)PTH_DY_009;
			key_group[2]->picbtnpathd=(u8*)PTH_DY_009;	
            //gui_show_ptstr(213+48,184+8,261+16,192+16,0,BLUE,16,"控制",0);
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
            
            if(card.sdprinting==TRUE)
                gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"暂停",0);
            else
                gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"打印",0);
            
            gui_show_ptstr(114+48,184+16,186+32,200+16,0,WHITE,16,"停止",0);
            gui_show_ptstr(213+48,184+16,285+32,200+16,0,WHITE,16,"控制",0);
		}
		#endif
	}
	//刷新温度显示
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 200;						//every 500ms refresh pringing data
		//断料检测
		if(PrintInfo.filament)	//检测到断料
		{
			card.sdprinting = FALSE;
			PrintInfo.printsd = 2;		//打印暂停
		}
		//暂停 继续图标刷新
		if(old_print_stat != card.sdprinting)
		{
			if(card.sdprinting==TRUE)
			{
				key_group[0]->picbtnpathu=(u8*)PTH_DY_006;
				key_group[0]->picbtnpathd=(u8*)PTH_DY_006;
                //gui_show_ptstr(14+48,184+8,30+16,192+16,0,BLUE,16,"暂停",0);
			}
			else
			{
				key_group[0]->picbtnpathu=(u8*)PTH_DY_007;
				key_group[0]->picbtnpathd=(u8*)PTH_DY_007;
                //gui_show_ptstr(14+48,184+8,30+16,192+16,0,BLUE,16,"打印",0);
				enquecommand("G28 XY");
			}
			old_print_stat = card.sdprinting;
			btn_draw(key_group[0]);
            
            if(card.sdprinting==TRUE)
                gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"暂停",0);
            else
                gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"打印",0);
            
		}
		if(PrintInfo.printper > old_print_pross)
		{
			progressbar_diy(154,34,PrintInfo.printper);
			old_print_pross = PrintInfo.printper;
		}
		switch(display_switch)
		{
			case 0:
			//打印时长
			memset(fatbuf,0,100);
			//sprintf(fatbuf,"%dh%02dm",PrintInfo.printtime/3600,PrintInfo.printtime/60%60);	
			sprintf(fatbuf,"%dh%02dm",(usr_timer.date *24 + usr_timer.hour),usr_timer.min);
			if(last_min != usr_timer.min)
			{
				last_min = usr_timer.min;
				gui_show_ptstr(180,67,180+64,67+16,0,WHITE,16,fatbuf,0);
			}
			break;
			case 1:
			//显示挤出头1温度
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[0]);	
			if(last_temp1 != (int)show_extruder_temperature[0])
			{
				last_temp1 = (int)show_extruder_temperature[0];
				gui_show_ptstr(260,67,260+40,67+16,0,WHITE,16,fatbuf,0);
			}
			break;
			case 2:
			//底板温度
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d℃",(int)show_bed_temperature);		
			if(last_bedtem != (int)show_bed_temperature)
			{
				last_bedtem = (int)show_bed_temperature;
				gui_show_ptstr(180,103,180+40,103+16,0,WHITE,16,fatbuf,0);
			}
			break;
			case 3:
			//挤出头2温度
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[1]);	
			if(last_temp2 != (int)show_extruder_temperature[1])
			{
				last_temp2 = (int)show_extruder_temperature[1];
				gui_show_ptstr(260,103,260+40,103+16,0,WHITE,16,fatbuf,0);
			}
			break;
			case 4:
			//打印Z轴高度
			memset(fatbuf,0,100);
			sprintf(fatbuf,"Z:%.2fmm",current_position[Z_AXIS]);	
			if(last_zlength != current_position[Z_AXIS])
			{
				last_zlength = current_position[Z_AXIS];
				gui_show_ptstr(225,140,235+88,140+16,0,WHITE,16,fatbuf,0);
			}
			break;
		}
		display_switch++;
		display_switch = display_switch%5; 
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
			if(card.sdprinting==TRUE)
			{
				card.sdprinting = FALSE;
				PrintInfo.printsd = 2;		//打印暂停
				gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"暂停",0);
				
			}
			else
			{
				card.sdprinting = TRUE;
				PrintInfo.printsd = 1;		//正在打印状态
				gui_show_ptstr(14+48,184+16,62+32,200+16,0,WHITE,16,"打印",0);
			}
		  break;
		  case 1:
		  	lastMenu = CurrentMenu;
			PrintPoint = TEXT_J;	
			CurrentMenu = popout_screen;
			tempMenu = manual_stop_diy;
			redraw_menu=TRUE;
			
		  break;
		  case 2:
		  	redraw_menu = TRUE;
		  	lastMenu = CurrentMenu;
			CurrentMenu = print_contrl_diy;
		  break;
		 
		  default: break;
		}
	}
	
	if(PrintInfo.printsd==0 && card.sdprinting==0)	//打印完成
	{
		CurrentMenu = stop_diy;	//跳转到主界面
		redraw_menu=TRUE;
		PrintInfo.printsd = 0;
	}
	
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
void print_contrl_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 5;
	
	gui_phy.back_color = BACK2_DIY;
	if(redraw_menu)
	{			
		switch(display_redraw)
		{
			case 0:
				gui_phy.back_color = BACK_DIY;
				//画背景色
				gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
				gui_phy.back_color = BACK2_DIY;
			break;
			case 1:
				//显示信息背景图
				minibmp_decode(PTH_DYKZ_003,16,47,158,162,0,0);
			break;
			case 2:
				//显示状态信息
				//minibmp_decode(PTH_DYKZ_004,19,54,26,26,0,0);
				minibmp_decode(PTH_DYKZ_005,19,95,26,26,0,0);
				minibmp_decode(PTH_DYKZ_006,100,95,26,26,0,0);
				minibmp_decode(PTH_DYKZ_007,19,137,26,26,0,0);
				minibmp_decode(PTH_DYKZ_008,100,137,26,26,0,0);
				minibmp_decode(PTH_DYKZ_009,19,179,26,26,0,0);
		    minibmp_decode(PTH_DYKZ_011,100,179,23,29,0,0);
			#ifdef FAN3_SHOW    
				minibmp_decode(PTH_DYKZ_010,100,179,26,26,0,0);
    		#endif
			break;
			case 3:
				display_switch=0;
				//速度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"打印速度:%04d%%",(int)feedmultiply);			
				gui_show_ptstr(47,63,47+72+40,63+16,0,WHITE,16,fatbuf,0);
				//挤出头1温度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[0]);			
				gui_show_ptstr(47,95+5,47+40,100+16,0,WHITE,16,fatbuf,0);
				//挤出头2温度
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[1]);
				gui_show_ptstr(128,95+5,128+40,100+16,0,WHITE,16,fatbuf,0);
				//底板
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d℃",(int)show_bed_temperature);			
				gui_show_ptstr(47,137+5,47+40,142+16,0,WHITE,16,fatbuf,0);
				//风扇1
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d",fanSpeed);
				gui_show_ptstr(128,137+5,128+40,142+16,0,WHITE,16,fatbuf,0);
				//风扇2
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d",fanSpeed1);
				gui_show_ptstr(47,179+5,47+40,184+16,0,WHITE,16,fatbuf,0);

		        //挤出比
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d",extrudemultiply);
				gui_show_ptstr(128,179+5,128+40,184+16,0,WHITE,16,fatbuf,0);
		    #ifdef FAN3_SHOW
				//风扇3
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%03d",fanSpeed2);			
				gui_show_ptstr(128,179+5,128+40,184+16,0,WHITE,16,fatbuf,0);
		    #endif
			
			break;
			case 4:
				//显示5个按钮图标
				key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
				if(key_group)
				{
					key_group[0]=btn_creat(187,47,117,37,0,1);
					key_group[0]->picbtnpathu=(u8*)PTH_DYKZ_013;
					key_group[0]->picbtnpathd=(u8*)PTH_DYKZ_013;
					key_group[1]=btn_creat(187,88,117,37,1,1);
					key_group[1]->picbtnpathu=(u8*)PTH_DYKZ_013;
					key_group[1]->picbtnpathd=(u8*)PTH_DYKZ_013;
					key_group[2]=btn_creat(187,130,117,37,2,1);//190
					key_group[2]->picbtnpathu=(u8*)PTH_DYKZ_013;
					key_group[2]->picbtnpathd=(u8*)PTH_DYKZ_013;
					key_group[3]=btn_creat(0,0,320,36,5,1);
					key_group[3]->picbtnpathu=(u8*)PTH_FH;
					key_group[3]->picbtnpathd=(u8*)PTH_FH;
		      key_group[4]=btn_creat(187,172,117,37,2,1);//190
					key_group[4]->picbtnpathu=(u8*)PTH_DYKZ_013;
					key_group[4]->picbtnpathd=(u8*)PTH_DYKZ_013;
					
					for(i=0;i<num;i++)
					{
						btn_draw(key_group[i]);
					}
				}
			break;
			case 5:
				minibmp_decode(PTH_DYKZ_000,187,47+5,29,26,0,0);//控制按键背景
				minibmp_decode(PTH_DYKZ_001,187,88+5,29,26,0,0);//控制按键背景
				minibmp_decode(PTH_DYKZ_002,187,130+5,29,26,0,0);//控制按键背景
				minibmp_decode(PTH_DYKZ_012,187,172+5,29,26,0,0);//控制按键背景
			break;
			case 6:
				gui_phy.back_color = BACK2_DIY;
				gui_show_strmid(187+42,47+10,32,16,WHITE,16,"风扇",0);
				gui_show_strmid(187+42,88+10,32,16,WHITE,16,"温度",0);
				gui_show_strmid(187+42,130+10,32,16,WHITE,16,"速度",0);
				gui_show_strmid(187+42,172+10,32,16,WHITE,16,"挤出",0);
				gui_show_strmid(130,10,64,16,WHITE,16,"打印控制",0);
				//显示机器类型
				if(PrinterMode==0)
				{
					minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
				}
				else if(PrinterMode==1)
				{
					;
				}
			break;
			default:
			break;
		}
		if(display_redraw<6)
		{
			display_redraw++;
			return;
		}
		else
		{
			display_redraw=0;
			redraw_menu = FALSE;
		}
		#if 0
		redraw_menu = FALSE;
		gui_phy.back_color = BACK_DIY;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		gui_phy.back_color = BACK2_DIY;		
		//显示信息背景图
		minibmp_decode(PTH_DYKZ_003,16,47,158,162,0,0);
				
		//显示状态信息
		//minibmp_decode(PTH_DYKZ_004,19,54,26,26,0,0);
		minibmp_decode(PTH_DYKZ_005,19,95,26,26,0,0);
		minibmp_decode(PTH_DYKZ_006,100,95,26,26,0,0);
		minibmp_decode(PTH_DYKZ_007,19,137,26,26,0,0);
		minibmp_decode(PTH_DYKZ_008,100,137,26,26,0,0);
		minibmp_decode(PTH_DYKZ_009,19,179,26,26,0,0);
    minibmp_decode(PTH_DYKZ_011,100,179,23,29,0,0);
        
    #ifdef FAN3_SHOW    
		minibmp_decode(PTH_DYKZ_010,100,179,26,26,0,0);
    #endif
		display_switch=0;
		//速度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"打印速度:%04d%%",(int)feedmultiply);			
		gui_show_ptstr(47,63,47+72+40,63+16,0,WHITE,16,fatbuf,0);
		//挤出头1温度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[0]);			
		gui_show_ptstr(47,95+5,47+40,100+16,0,WHITE,16,fatbuf,0);
		//挤出头2温度
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[1]);
		gui_show_ptstr(128,95+5,128+40,100+16,0,WHITE,16,fatbuf,0);
		//底板
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d℃",(int)show_bed_temperature);			
		gui_show_ptstr(47,137+5,47+40,142+16,0,WHITE,16,fatbuf,0);
		//风扇1
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d",fanSpeed);
		gui_show_ptstr(128,137+5,128+40,142+16,0,WHITE,16,fatbuf,0);
		//风扇2
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d",fanSpeed1);
		gui_show_ptstr(47,179+5,47+40,184+16,0,WHITE,16,fatbuf,0);

        //挤出比
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d",extrudemultiply);
		gui_show_ptstr(128,179+5,128+40,184+16,0,WHITE,16,fatbuf,0);
    #ifdef FAN3_SHOW
		//风扇3
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%03d",fanSpeed2);			
		gui_show_ptstr(128,179+5,128+40,184+16,0,WHITE,16,fatbuf,0);
    #endif
		
		//显示5个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
			key_group[0]=btn_creat(187,47,117,37,0,1);
			key_group[0]->picbtnpathu=(u8*)PTH_DYKZ_013;
			key_group[0]->picbtnpathd=(u8*)PTH_DYKZ_013;
			key_group[1]=btn_creat(187,88,117,37,1,1);
			key_group[1]->picbtnpathu=(u8*)PTH_DYKZ_013;
			key_group[1]->picbtnpathd=(u8*)PTH_DYKZ_013;
			key_group[2]=btn_creat(187,130,117,37,2,1);//190
			key_group[2]->picbtnpathu=(u8*)PTH_DYKZ_013;
			key_group[2]->picbtnpathd=(u8*)PTH_DYKZ_013;
			key_group[3]=btn_creat(0,0,320,36,5,1);
			key_group[3]->picbtnpathu=(u8*)PTH_FH;
			key_group[3]->picbtnpathd=(u8*)PTH_FH;
      key_group[4]=btn_creat(187,172,117,37,2,1);//190
			key_group[4]->picbtnpathu=(u8*)PTH_DYKZ_013;
			key_group[4]->picbtnpathd=(u8*)PTH_DYKZ_013;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
		}

		minibmp_decode(PTH_DYKZ_000,187,47+5,29,26,0,0);//控制按键背景
		minibmp_decode(PTH_DYKZ_001,187,88+5,29,26,0,0);//控制按键背景
		minibmp_decode(PTH_DYKZ_002,187,130+5,29,26,0,0);//控制按键背景
		minibmp_decode(PTH_DYKZ_012,187,172+5,29,26,0,0);//控制按键背景
        
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(187+42,47+10,32,16,WHITE,16,"风扇",0);
		gui_show_strmid(187+42,88+10,32,16,WHITE,16,"温度",0);
		gui_show_strmid(187+42,130+10,32,16,WHITE,16,"速度",0);
		gui_show_strmid(187+42,172+10,32,16,WHITE,16,"挤出",0);
		gui_show_strmid(130,10,64,16,WHITE,16,"打印控制",0);
		//显示机器类型
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
		#endif
	}
	//刷新温度显示
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 200;
		switch(display_switch)
		{
		case 0:
			//速度
			memset(fatbuf,0,100);
			sprintf(fatbuf,"打印速度:%04d%%",(int)feedmultiply);			
		    gui_show_ptstr(47,63,47+72+40,63+16,0,WHITE,16,fatbuf,0);
		break;
		case 1:
			//挤出头1温度
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[0]);
			if(last_temp1 != (int)show_extruder_temperature[0])
			{
				last_temp1 = (int)show_extruder_temperature[0];
				gui_show_ptstr(47,95+5,47+40,100+16,0,WHITE,16,fatbuf,0);
			}
		break;
		case 2:
			//挤出头2温度
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d℃",(int)show_extruder_temperature[1]);
			if(last_temp2 != (int)show_extruder_temperature[1])
			{
				last_temp2 = (int)show_extruder_temperature[1];
				gui_show_ptstr(128,95+5,128+40,100+16,0,WHITE,16,fatbuf,0);
			}
		break;
		case 3:
			//底板
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d℃",(int)show_bed_temperature);		
			if(last_bedtem != (int)show_bed_temperature)
			{
				last_bedtem = (int)show_bed_temperature;
				gui_show_ptstr(47,137+5,47+40,142+16,0,WHITE,16,fatbuf,0);
			}
		break;
		case 4:
			//风扇1
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d",fanSpeed);
			gui_show_ptstr(128,137+5,128+40,142+16,0,WHITE,16,fatbuf,0);
		break;
		case 5:
			//风扇2
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d",fanSpeed1);
			gui_show_ptstr(47,179+5,47+40,184+16,0,WHITE,16,fatbuf,0);
		break;
    #ifdef FAN3_SHOW
		case 6:
			//风扇3
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%03d",fanSpeed2);			
			gui_show_ptstr(128,179+5,128+40,184+16,0,WHITE,16,fatbuf,0);	
		break;
    #endif
		}
		display_switch++;
		display_switch = display_switch%7;
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0:
				lastMenu = CurrentMenu;
				CurrentMenu = fan_diy;
		  	redraw_menu = TRUE;
				break;
		  case 1:
		  	lastMenu = CurrentMenu;
				CurrentMenu = preheat_diy;
		  	redraw_menu = TRUE;
				break;
		  case 2:
		  	lastMenu = CurrentMenu;
				CurrentMenu = speed_diy;
		  	redraw_menu = TRUE;
				break;
		  case 3:
		  	redraw_menu = TRUE;
				CurrentMenu = print_diy;
				break;
      case 4:
        lastMenu = CurrentMenu;
				CurrentMenu = extrudemultiply_diy;
		  	redraw_menu = TRUE;
				break;
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
static u8 speed_factor_diy=50;		// 10 50 100
void speed_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 5;
//	float copy_temperature;
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		switch(display_redraw)
		{
			case 0:
				//画背景色
				gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
				break;
			case 1:
					//显示速度小图标
					minibmp_decode(PTH_DYSD_000,146,59,26,26,0,0);
					//速度等级背景
					minibmp_decode(PTY_DY_BG,16,143,127,69,0,0);
					//显示当前打印速度值
					memset(fatbuf,0,100);
					sprintf(fatbuf,"%04d%%",feedmultiply);			
					gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
			break;
			case 2:
				//显示5个按钮图标
				key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
				if(key_group)
				{
		     	key_group[0]=btn_creat(16,59,92,72,0,1);
					key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
					key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
					key_group[1]=btn_creat(220,59,92,72,1,1);
					key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
					key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
					
					key_group[2]=btn_creat(16,143,127,69,2,1);
		      gui_phy.back_color = BACK2_DIY;
					if(speed_factor_diy==10)
					{
						sprintf(fatbuf,"%03d",speed_factor_diy);
						gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
						/*
						key_group[2]->picbtnpathu=(u8*)PTH_DYSD_001;
						key_group[2]->picbtnpathd=(u8*)PTH_DYSD_001;
						*/
					}
					else if(speed_factor_diy==50)
					{
						sprintf(fatbuf,"%03d",speed_factor_diy);
						gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
						/*
						key_group[2]->picbtnpathu=(u8*)PTH_DYSD_002;
						key_group[2]->picbtnpathd=(u8*)PTH_DYSD_002;
						*/
					}
					else if(speed_factor_diy==100)
					{
						sprintf(fatbuf,"%03d",speed_factor_diy);
						gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
						/*
						key_group[2]->picbtnpathu=(u8*)PTH_DYSD_003;
						key_group[2]->picbtnpathd=(u8*)PTH_DYSD_003;
						*/
					}
					
					key_group[3]=btn_creat(185,143,127,69,3,1);
					key_group[3]->picbtnpathu=(u8*)PTH_DYSD_004;
					key_group[3]->picbtnpathd=(u8*)PTH_DYSD_004;
					
					key_group[4]=btn_creat(0,0,320,36,5,1);
					key_group[4]->picbtnpathu=(u8*)PTH_FH;
					key_group[4]->picbtnpathd=(u8*)PTH_FH;
					
					for(i=0;i<num;i++)
					{
						btn_draw(key_group[i]);
					}
					//显示机器类型
					if(PrinterMode==0)
					{
						minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
					}
					else if(PrinterMode==1)
					{
						;
					}
				}
				break;
			case 3:
				//显示机器类型
				gui_phy.back_color = BACK2_DIY;
				gui_show_strmid(130,10,60,16,WHITE,16,"速度",0);
				if(PrinterMode==0)
				{
					minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
				}
				else if(PrinterMode==1)
				{
					;
				}
				break;
			default:
			break;
		}
		if(display_redraw<3)
		{
			display_redraw++;
			return;
		}
		else
		{
			display_redraw=0;
			redraw_menu=FALSE;
		}
	#if 0
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		
		//显示速度小图标
		minibmp_decode(PTH_DYSD_000,146,59,26,26,0,0);
        //速度等级背景
        minibmp_decode(PTY_DY_BG,16,143,127,69,0,0);
        
		//显示当前打印速度值
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%04d%%",feedmultiply);			
		gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
        

		//显示5个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
		if(key_group)
		{
     		key_group[0]=btn_creat(16,59,92,72,0,1);
			key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
			key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
			key_group[1]=btn_creat(220,59,92,72,1,1);
			key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
			key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
			
			key_group[2]=btn_creat(16,143,127,69,2,1);
            gui_phy.back_color = BACK2_DIY;
			if(speed_factor_diy==10)
			{
			    sprintf(fatbuf,"%03d",speed_factor_diy);
                gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			    /*
				key_group[2]->picbtnpathu=(u8*)PTH_DYSD_001;
				key_group[2]->picbtnpathd=(u8*)PTH_DYSD_001;
				*/
			}
			else if(speed_factor_diy==50)
			{
			    sprintf(fatbuf,"%03d",speed_factor_diy);
                gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			    /*
				key_group[2]->picbtnpathu=(u8*)PTH_DYSD_002;
				key_group[2]->picbtnpathd=(u8*)PTH_DYSD_002;
				*/
			}
			else if(speed_factor_diy==100)
			{
			    sprintf(fatbuf,"%03d",speed_factor_diy);
                gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			    /*
				key_group[2]->picbtnpathu=(u8*)PTH_DYSD_003;
				key_group[2]->picbtnpathd=(u8*)PTH_DYSD_003;
				*/
			}
			
			key_group[3]=btn_creat(185,143,127,69,3,1);
			key_group[3]->picbtnpathu=(u8*)PTH_DYSD_004;
			key_group[3]->picbtnpathd=(u8*)PTH_DYSD_004;
			
			key_group[4]=btn_creat(0,0,320,36,5,1);
			key_group[4]->picbtnpathu=(u8*)PTH_FH;
			key_group[4]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
			//显示机器类型
			if(PrinterMode==0)
			{
				minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
			}
			else if(PrinterMode==1)
			{
				;
			}
		}
		//显示机器类型
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"速度",0);
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
	#endif	
		
	}
	//按键检测
//	float speed_temp;
	
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	
			if(feedmultiply>=(speed_factor_diy+50))
			{
				feedmultiply -= speed_factor_diy;
			}

			memset(fatbuf,0,100);
			sprintf(fatbuf,"%04d%%",feedmultiply);			
			gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
		  break;
		  case 1:				//倍数限制,最高到500%
		  	if(feedmultiply <=(DEFAULT_MAX_FEEDMULTIPLY-speed_factor_diy))
		  	{
					feedmultiply += speed_factor_diy;
				}
			
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%04d%%",feedmultiply);			
			gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
		  break;
		 case 2:
       gui_phy.back_color = BACK2_DIY;
			if(speed_factor_diy==10)
			{
				speed_factor_diy = 50;
				sprintf(fatbuf,"%03d",speed_factor_diy);
        gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			}
			else if(speed_factor_diy==50)
			{
				speed_factor_diy = 100;
				sprintf(fatbuf,"%03d",speed_factor_diy);
        gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			}
			else if(speed_factor_diy==100)
			{
				speed_factor_diy = 10;
				sprintf(fatbuf,"%03d",speed_factor_diy);
        gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			}
			btn_draw(key_group[2]);
		 break;
		 case 3:
			feedmultiply = DEFAULT_FEEDMULTIPLY;						//RETURN TO MIN FEEDMULTIPLY
			gui_fill_rectangle(150,96,20,16,BACK_DIY);
			memset(fatbuf,0,100);
			sprintf(fatbuf,"%04d%%",feedmultiply);			
			gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
		 	break;
		 case 4:
		 	redraw_menu = TRUE;
			CurrentMenu = print_contrl_diy;
		 	break;
		  default: break;
		}
	}
	if(speed_temprature_factor > EPSINON)
	{  
			target_temperature[0] = 185*speed_temprature_factor*(feedmultiply-100)/100+185;
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
static u8 extrudemulti_factor_diy=50;		// 10 50 100
void extrudemultiply_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 5;
	gui_phy.back_color = BACK_DIY;
	if(redraw_menu)
	{
		switch(display_redraw)
		{
			case 0:
				//画背景色
				gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
				break;
			case 1:
				//显示速度小图标
				minibmp_decode(PTH_DYSD_000,146,59,26,26,0,0);
				//速度等级背景
				minibmp_decode(PTY_DY_BG,16,143,127,69,0,0);
				//显示当前打印速度值
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%04d%%",extrudemultiply);			
				gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
				break;
			case 2:
				//显示5个按钮图标
				key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
				if(key_group)
				{
		     	key_group[0]=btn_creat(16,59,92,72,0,1);
					key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
					key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
					key_group[1]=btn_creat(220,59,92,72,1,1);
					key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
					key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
					
					key_group[2]=btn_creat(16,143,127,69,2,1);
		      gui_phy.back_color = BACK2_DIY;
					if(extrudemulti_factor_diy==10)
					{
						sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
						gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
						/*
						key_group[2]->picbtnpathu=(u8*)PTH_DYSD_001;
						key_group[2]->picbtnpathd=(u8*)PTH_DYSD_001;
						*/
					}
					else if(extrudemulti_factor_diy==50)
					{
						sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
						gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
						/*
						key_group[2]->picbtnpathu=(u8*)PTH_DYSD_002;
						key_group[2]->picbtnpathd=(u8*)PTH_DYSD_002;
						*/
					}
					else if(extrudemulti_factor_diy==100)
					{
						sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
						gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
						/*
						key_group[2]->picbtnpathu=(u8*)PTH_DYSD_003;
						key_group[2]->picbtnpathd=(u8*)PTH_DYSD_003;
						*/
					}
					
					key_group[3]=btn_creat(185,143,127,69,3,1);
					key_group[3]->picbtnpathu=(u8*)PTH_DYSD_004;
					key_group[3]->picbtnpathd=(u8*)PTH_DYSD_004;
					
					key_group[4]=btn_creat(0,0,320,36,5,1);
					key_group[4]->picbtnpathu=(u8*)PTH_FH;
					key_group[4]->picbtnpathd=(u8*)PTH_FH;
					
					for(i=0;i<num;i++)
					{
						btn_draw(key_group[i]);
					}
					//显示机器类型
					if(PrinterMode==0)
					{
						minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
					}
					else if(PrinterMode==1)
					{
						;
					}
				}
				break;
			case 3:
					//显示机器类型
					gui_phy.back_color = BACK2_DIY;
					gui_show_strmid(130,10,60,16,WHITE,16,"挤出比",0);
					if(PrinterMode==0)
					{
						minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
					}
					else if(PrinterMode==1)
					{
						;
					}
				break;
			default:
				break;
			
		}
		if(display_redraw<3)
		{
			display_redraw++;
			return;
		}
		else
		{
			display_redraw=0;
			redraw_menu = FALSE;
		}
	#if 0
		redraw_menu = FALSE;
		//画背景色
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
		
		//显示速度小图标
		minibmp_decode(PTH_DYSD_000,146,59,26,26,0,0);
        //速度等级背景
        minibmp_decode(PTY_DY_BG,16,143,127,69,0,0);
        
		//显示当前打印速度值
		memset(fatbuf,0,100);
		sprintf(fatbuf,"%04d%%",extrudemultiply);			
		gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
        

		//显示5个按钮图标
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
		if(key_group)
		{
     		key_group[0]=btn_creat(16,59,92,72,0,1);
			key_group[0]->picbtnpathu=(u8*)PTH_YR_000;
			key_group[0]->picbtnpathd=(u8*)PTH_YR_000;
			key_group[1]=btn_creat(220,59,92,72,1,1);
			key_group[1]->picbtnpathu=(u8*)PTH_YR_001;
			key_group[1]->picbtnpathd=(u8*)PTH_YR_001;
			
			key_group[2]=btn_creat(16,143,127,69,2,1);
            gui_phy.back_color = BACK2_DIY;
			if(extrudemulti_factor_diy==10)
			{
			    sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
                gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			    /*
				key_group[2]->picbtnpathu=(u8*)PTH_DYSD_001;
				key_group[2]->picbtnpathd=(u8*)PTH_DYSD_001;
				*/
			}
			else if(extrudemulti_factor_diy==50)
			{
			    sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
                gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			    /*
				key_group[2]->picbtnpathu=(u8*)PTH_DYSD_002;
				key_group[2]->picbtnpathd=(u8*)PTH_DYSD_002;
				*/
			}
			else if(extrudemulti_factor_diy==100)
			{
			    sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
                gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
			    /*
				key_group[2]->picbtnpathu=(u8*)PTH_DYSD_003;
				key_group[2]->picbtnpathd=(u8*)PTH_DYSD_003;
				*/
			}
			
			key_group[3]=btn_creat(185,143,127,69,3,1);
			key_group[3]->picbtnpathu=(u8*)PTH_DYSD_004;
			key_group[3]->picbtnpathd=(u8*)PTH_DYSD_004;
			
			key_group[4]=btn_creat(0,0,320,36,5,1);
			key_group[4]->picbtnpathu=(u8*)PTH_FH;
			key_group[4]->picbtnpathd=(u8*)PTH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(key_group[i]);
			}
			//显示机器类型
			if(PrinterMode==0)
			{
				minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
			}
			else if(PrinterMode==1)
			{
				;
			}
		}
		//显示机器类型
		gui_phy.back_color = BACK2_DIY;
		gui_show_strmid(130,10,60,16,WHITE,16,"挤出比",0);
		if(PrinterMode==0)
		{
			minibmp_decode(PTH_KZ1_017,280,13,22,22,0,0);
		}
		else if(PrinterMode==1)
		{
			;
		}
	#endif	
		
	}
	//按键检测
//	float speed_temp;
	
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
				if(extrudemultiply>=(extrudemulti_factor_diy+50))
				{
					extrudemultiply -= extrudemulti_factor_diy;
				}

				memset(fatbuf,0,100);
				sprintf(fatbuf,"%04d%%",extrudemultiply);			
				gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
				break;
		  case 1:
		  	if(extrudemultiply <=(1000-extrudemulti_factor_diy))
		  	{
					extrudemultiply += extrudemulti_factor_diy;
				}
			
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%04d%%",extrudemultiply);			
				gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
				break;
		 case 2:
				gui_phy.back_color = BACK2_DIY;
				if(extrudemulti_factor_diy==10)
				{
					extrudemulti_factor_diy = 50;
					sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
					gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
				}
				else if(extrudemulti_factor_diy==50)
				{
					extrudemulti_factor_diy = 100;
					sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
					gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
				}
				else if(extrudemulti_factor_diy==100)
				{
					extrudemulti_factor_diy = 10;
					sprintf(fatbuf,"%03d",extrudemulti_factor_diy);
					gui_show_strmid(67,169,24,16,WHITE,16,fatbuf,0);
				}
				btn_draw(key_group[2]);
				break;
		 case 3:
				extrudemultiply = 100;
				gui_fill_rectangle(150,96,20,16,BACK_DIY);
				memset(fatbuf,0,100);
				sprintf(fatbuf,"%04d%%",extrudemultiply);			
				gui_show_strmid(140,96,40,16,WHITE,16,fatbuf,0);
				break;
		 case 4:
				redraw_menu = TRUE;
				CurrentMenu = print_contrl_diy;
				break;
		  default: break;
		}
	}
    if(speed_temprature_factor > EPSINON)
		{  
        target_temperature[0] = 185*speed_temprature_factor*(extrudemultiply-100)/100+185;
    }
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}

void getgcodetab_diy(void)
{
	u8 selx = 0XFF;
	u8 i;
	u8 *oldpath;
	
//	_filelistbox_list *filelisttemp;
	if(windows_flag == false) 
	{
		if(redraw_menu == true) 
		{
			redraw_menu = false;
			gui_fill_rectangle(0,0,lcddev.width,lcddev.height,gui_phy.back_color);
			gui_fill_rectangle(0, 0, lcddev.width, 40, LIGHTBLUE);                      //名字栏部分为蓝色
			gui_draw_hline(0, 40, lcddev.width, GRAY);                                  //在名字栏最下面划一条线
			gui_show_strmid(0, 0, 320, 40, WHITE, 16, (u8 *)"SD卡打印", 1);                //名字栏TEXT显示
			minibmp_decode(PTH_FILE_002,247,45,63,82,0,0);
			minibmp_decode(PTH_FILE_003,247,142,63,82,0,0);

			//画背景
			screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *) * 11);   //分配三个按键的资源
			if(screen_key_group) 
			{                                                       //资源分配成功
				screen_key_group[2] = btn_creat(8, 8, 50, 25, 0, 4);
				screen_key_group[2]->bcfucolor = BLACK;
				screen_key_group[2]->bcfdcolor = WHITE;
				screen_key_group[2]->bkctbl[0] = CYAN;
				screen_key_group[2]->bkctbl[1] = RED;
				screen_key_group[2]->bkctbl[2] = CYAN;
				screen_key_group[2]->bkctbl[3] = RED;
				screen_key_group[2]->caption = "返回";
				for(i = 0; i < 2; i++)
				{
					screen_key_group[i] = btn_creat(95 + i * 80, 125, 60, 40, i, 2);

					screen_key_group[i]->bkctbl[0] = 0X8452;
					screen_key_group[i]->bkctbl[1] = 0XAD97;
					screen_key_group[i]->bkctbl[2] = 0XAD97;
					screen_key_group[i]->bkctbl[3] = 0X8452;
				}
				for(i = 3; i < 6; i++) 
				{
					screen_key_group[i] = btn_creat((i - 3) % 4 * 79+10, 45 + 97 * ((i - 3) / 4), 70, 82, i, BTN_TYPE_MULLINE);

					screen_key_group[i]->bkctbl[0] = LIGHTBLUE;
					screen_key_group[i]->bkctbl[1] = LIGHTBLUE;
					screen_key_group[i]->bkctbl[2] = LIGHTBLUE;
					screen_key_group[i]->bkctbl[3] = LIGHTBLUE;

				}
        for(i = 7; i < 10; i++) 
				{
					screen_key_group[i] = btn_creat((i - 3) % 4 * 79+10, 45 + 97 * ((i - 3) / 4), 70, 82, i, BTN_TYPE_MULLINE);

					screen_key_group[i]->bkctbl[0] = LIGHTBLUE;
					screen_key_group[i]->bkctbl[1] = LIGHTBLUE;
					screen_key_group[i]->bkctbl[2] = LIGHTBLUE;
					screen_key_group[i]->bkctbl[3] = LIGHTBLUE;
				}
				screen_key_group[6]=btn_creat(247,45,70,82,0,1);
				screen_key_group[6]->picbtnpathu=(u8*)PTH_FILE_002;
				screen_key_group[6]->picbtnpathd=(u8*)PTH_FILE_002;
				screen_key_group[6] = btn_creat(247, 45, 70, 82, 0, 1);

				screen_key_group[10]=btn_creat(247,142,70,82,0,1);
				screen_key_group[10]->picbtnpathu=(u8*)PTH_FILE_003;
				screen_key_group[10]->picbtnpathd=(u8*)PTH_FILE_003;

				screen_key_group[0]->caption = (u8 *)"打印";                              //打印按键
				screen_key_group[1]->caption = (u8 *)"返回";                              //返回按键

				memset(gcodefile_t,0,sizeof(gcodefile_s)*100);
				gcodefile_num = check_sd_allfile(gcodefile_path);													//gcodefile counter;
				
        gcodefile_index_sub = 0;
				for(i = 0; i < GCODE_FILE_SHOW_NUM; i++) 
				{
						if((gcodefile_index_sub + gcodefile_index_base) < gcodefile_num)
						{
								//文件已经读取完
								readindex_file(gcodefile_t[gcodefile_index_sub + gcodefile_index_base].file_index, file_name_buf[gcodefile_index_sub], gcodefile_path);
								USR_UsrLog("%d :%s\n", gcodefile_t[gcodefile_index_sub + gcodefile_index_base].file_index, file_name_buf[gcodefile_index_sub]);
								file_path_status = OK; 
						}
						else
						{
								strcpy(file_name_buf[gcodefile_index_sub],"\0");
								file_path_status = CANT_DEEP;
						}
						gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
				}
				if(6 + gcodefile_index_base >= gcodefile_num)				//next page no file did't downstears
				{
						file_path_status = CANT_DEEP;
				}
				gcodefile_index_sub = 0;
				screen_key_group[3]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                          
				gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM; 

				screen_key_group[4]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           
				gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;

				screen_key_group[5]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           
				gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;


				//screen_key_group[6]->caption = (u8 *)"上";                                       

				screen_key_group[7]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           
				gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;

				screen_key_group[8]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           
				gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;

				screen_key_group[9]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           
				gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
				gcodefile_index_sub = 0;

				//screen_key_group[10]->caption = (u8 *)"下";  
        //key 6 & 10 upkey & downkey  
				btn_draw(screen_key_group[2]);																																			//返回按键重构
				
         gcodefile_index_sub = 0;
         for(i = 3; i < 6; i++) 
				{
					 if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == GCODE)
					 {
						minibmp_decode(PTH_FILE_000,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);	//加载文件背景
					 }
					else if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == FOLDER)		//加载文件夹背景
					{
						minibmp_decode(PTH_FILE_001,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
					}
					
          gcodefile_index_sub++;

					if((gcodefile_index_sub + gcodefile_index_base) > gcodefile_num)
					{
						minibmp_decode(PTH_FILE_004,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);	//填充空背景
					}
					else{
						btn_draw(screen_key_group[i]);
					}
				}
        for(i = 7; i < 10; i++) 
				{
           if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == GCODE)
					 {
					    minibmp_decode(PTH_FILE_000,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);		//加载文件背景
					 }
					else if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == FOLDER)
					{
						 minibmp_decode(PTH_FILE_001,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);		//加载文件夹背景
					}
					btn_draw(screen_key_group[i]);
          gcodefile_index_sub++;

					if((gcodefile_index_sub + gcodefile_index_base) > gcodefile_num)
					{
						minibmp_decode(PTH_FILE_004,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
					}
					else{
						btn_draw(screen_key_group[i]);
					}
				}
			}
		}
		
		selx = screen_key_chk(screen_key_group+2, &in_obj, 9);
		if(btnshow_change_flag == true)
		{//显示文件按键改变
			btnshow_change_flag = false;
			memset(gcodefile_t,0,sizeof(gcodefile_s)*100);
			memset(file_name_buf,0,sizeof(file_name_buf));
			gcodefile_num = check_sd_allfile(gcodefile_path);
      gcodefile_index_sub = 0;
		  for(i = 0; i < GCODE_FILE_SHOW_NUM; i++) 
			{
				if((gcodefile_index_sub + gcodefile_index_base) < gcodefile_num)
				{
						readindex_file(gcodefile_t[gcodefile_index_sub + gcodefile_index_base].file_index, file_name_buf[gcodefile_index_sub], gcodefile_path);
						USR_UsrLog("%d :%s\n", gcodefile_t[gcodefile_index_sub + gcodefile_index_base].file_index, file_name_buf[gcodefile_index_sub]);
						file_path_status = OK; 
				}
				else
				{
					strcpy(file_name_buf[gcodefile_index_sub],"\0");
					file_path_status = CANT_DEEP;
				}
				gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
			}
			if(6 + gcodefile_index_base >= gcodefile_num)
				{
            file_path_status = CANT_DEEP;
        }
			gcodefile_index_sub = 0;
			screen_key_group[3]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           //打印按键
			gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
			screen_key_group[4]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           //返回按键
			gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
			screen_key_group[5]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           //打印按键
			gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
			screen_key_group[7]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           //打印按键
			gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
			screen_key_group[8]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           //返回按键
			gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
      screen_key_group[9]->caption = (u8 *)file_name_buf[gcodefile_index_sub];                           //打印按键
			gcodefile_index_sub = (gcodefile_index_sub + 1) % GCODE_FILE_SHOW_NUM;
			
			btn_draw(screen_key_group[2]);																																			//返回按键重构
			
      for(i = 3; i < 6; i++) 
			{
				if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == GCODE)
				{
					minibmp_decode(PTH_FILE_000,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
				}
				else if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == FOLDER)
				{
					minibmp_decode(PTH_FILE_001,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
				}
				gcodefile_index_sub++;

				if((gcodefile_index_sub + gcodefile_index_base) > gcodefile_num)
				{
					minibmp_decode(PTH_FILE_004,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
				}
				else{
						btn_draw(screen_key_group[i]);
				}
			}
			
      for(i = 7; i < 10; i++) 
			{
				if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == GCODE)
				{
					minibmp_decode(PTH_FILE_000,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
				}
				else if(gcodefile_t[gcodefile_index_sub+gcodefile_index_base].file_type == FOLDER)
				{
					minibmp_decode(PTH_FILE_001,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
					
				}
				gcodefile_index_sub++;

				if((gcodefile_index_sub + gcodefile_index_base) > gcodefile_num)
				{
					minibmp_decode(PTH_FILE_004,(i - 3) % 4 * 79+10,45 + 97 * ((i - 3) / 4),70,82,0,0);
				}
				else{
						btn_draw(screen_key_group[i]);
				}
		  }
    }
		
		if(selx & (1 << 6)) 
		{
      bool press_flag = FALSE;
			switch(selx & ~(3 << 6)) 
			{ //
				case 0:
					oldpath = (u8 *)gcodefile_path+100;
					while(*oldpath!=0x5c)
					{
						oldpath--;	//追述到倒数第一个"\"处 
						if(oldpath == gcodefile_path)
						{//最后一位
								break;
						}
					}
					if(oldpath != gcodefile_path)
					{//返回上一级
						*oldpath='\0';//用结束符代替"\".从而回到上一级目录
						btnshow_change_flag = true;
						redraw_menu = false;
						windows_flag = false;
						gcodefile_index_sub = 0;
						gcodefile_index_base = 0;
					}
					else if(oldpath == gcodefile_path)
					{//返回主界面
							redraw_menu=TRUE;
						CurrentMenu = main_diy;	
					}
					break;
			case 1:     
					gcodefile_index_sub = 0;
					press_flag = TRUE;
					break;
			case 2:
					gcodefile_index_sub = 1;
					press_flag = TRUE;
					break;
			case 3:
					gcodefile_index_sub = 2;
					press_flag = TRUE;
					break;
			case 4:
					if(gcodefile_index_base != 0)
					{
						btnshow_change_flag = true;
						redraw_menu = false;
						windows_flag = false;
						gcodefile_index_base -= GCODE_FILE_SHOW_NUM;    
					}
					if(file_path_status == CANT_DEEP)
					{
						file_path_status == OK;
					}
					break;
			case 5:
					gcodefile_index_sub = 3;
					press_flag = TRUE;
					break;
			case 6:
					gcodefile_index_sub = 4;
					press_flag = TRUE;
					break;
			case 7:
					gcodefile_index_sub = 5;
					press_flag = TRUE;
					break;
			case 8:
					if(file_path_status == OK)
					{//可以向下
						btnshow_change_flag = true;
						redraw_menu = false;
						windows_flag = false;
						gcodefile_index_base += GCODE_FILE_SHOW_NUM;
					}
					break;  
				default:
					break;
			}
			if(press_flag)
			{
				press_flag = 0;
				if((gcodefile_index_sub + gcodefile_index_base) < gcodefile_num)
				{
					if(gcodefile_t[gcodefile_index_sub + gcodefile_index_base].file_type == GCODE)//如果是GCODE打印
					{
						redraw_menu = true;
						windows_flag = true;
					}
					else
					{//如果是文件 1、  文件路径改变 2、按键名改变 (1、读取索引base改变 2、重新读取)
							btnshow_change_flag = true;
							redraw_menu = false;
							windows_flag = false;
							gui_path_name(gcodefile_path,gcodefile_path,file_name_buf[gcodefile_index_sub]);	//文件名加入路径 
							gcodefile_index_base = 0;
							gcodefile_index_sub = 0;
					}
				}
			}
		} 
	} 
	else 
	{
		if(redraw_menu == true) 
		{
//			for(i = 1; i < 3; i++)
//			{
//					screen_key_group[i] = btn_creat(95 + (i - 1) * 80, 125, 60, 40, i, 2);

//					screen_key_group[i]->bkctbl[0] = 0X8452;
//					screen_key_group[i]->bkctbl[1] = 0XAD97;
//					screen_key_group[i]->bkctbl[2] = 0XAD97;
//					screen_key_group[i]->bkctbl[3] = 0X8452;
//			}
//			screen_key_group[1]->caption = (u8 *)"打印";                              //打印按键
//			screen_key_group[2]->caption = (u8 *)"返回";                              //返回按键
			redraw_menu = false;
			gui_draw_arcrectangle(75, 80, 180, 40, 6, 1, GRAYBLUE, GRAYBLUE);
			gui_fill_rectangle(75, 110, 180, 70, WHITE); //
			gui_draw_rectangle(75, 110, 180, 70, GRAYBLUE);
			gui_show_strmid(75, 80, 180, 30, BLACK, 16, file_name_buf[gcodefile_index_sub], 1);

			if(screen_key_group) 
			{
				for(i = 0; i < 2; i++) 
				{
					btn_draw(screen_key_group[i]);
				}
			}
		}
		selx = screen_key_chk(screen_key_group, &in_obj, 2);
		if(selx & (1 << 6)) 
		{
			switch(selx & ~(3 << 6)) 
			{ //
				case 0:
					strcpy((char *)card.filename, (const char *)file_name_buf[gcodefile_index_sub%GCODE_FILE_SHOW_NUM]);
					if(pname) 
					{
						gui_path_name(pname, gcodefile_path, (u8 *)file_name_buf[gcodefile_index_sub%GCODE_FILE_SHOW_NUM]);
						USR_UsrLog("pname = %s",pname);
						selx = f_open(&card.fgcode, (const TCHAR *)pname, FA_READ);
						if(selx == FR_OK) 
						{
							strcpy(PrintInfo.printfile, file_name_buf[gcodefile_index_sub%GCODE_FILE_SHOW_NUM]); //更新打印文件名
							PrintInfo.filesize = card.fgcode.fsize;         //更新打印文件大小
							USR_UsrLog("printfile is %s\r\n", PrintInfo.printfile);
							USR_UsrLog("filesize is %d\r\n", PrintInfo.filesize);

							CurrentMenu = print_diy;
							redraw_menu = true;
							windows_flag = false;
							for(i = 0; i < 11; i++) 
							{
								btn_delete(screen_key_group[i]);
							}
							gui_memin_free(screen_key_group);
							card_startFileprint();
							starttime = millis();
						}
					}
					break;
				case 1:
					windows_flag = false;
                    redraw_menu = true;
					break;
				default:
					break;
			}
		}
	}
    if(redraw_menu) 
		{
			for(i = 0; i < 11; i++) 
			{
				btn_delete(screen_key_group[i]);
			}
			gui_memin_free(screen_key_group);
		}
}

void gecodelist_diy(void)
{ 
	u8 selx=0XFF;
	u8 i;	
 	_filelistbox_list * filelisttemp;	
 	if(windows_flag==FALSE)
	{	 
		if(redraw_menu==TRUE)			
		{ 
			redraw_menu=FALSE;		

			gui_fill_rectangle(0,0,lcddev.width,lcddev.height,LIGHTGRAY );
			gui_fill_rectangle(0,0,lcddev.width,40,LIGHTBLUE);  
			gui_draw_hline(0,40,lcddev.width,GRAY);
			gui_show_strmid(0,0,320,40,WHITE,16,(u8*)"SD卡打印",1);
			//gui_show_strmid(0,0,320,40,WHITE,16,(u8*)"SD card print",1);
			//画背景				
			
			screen_key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*3);
			flistbox=filelistbox_creat(2,44,lcddev.width-4,192,1,16);
			if(screen_key_group&&flistbox)
			{		
        screen_key_group[0]=btn_creat(8,8,50,25,0,4);
				screen_key_group[0]->bcfucolor=BLACK;
				screen_key_group[0]->bcfdcolor=WHITE;
				screen_key_group[0]->bkctbl[0]=CYAN;
				screen_key_group[0]->bkctbl[1]=RED;
				screen_key_group[0]->bkctbl[2]=CYAN;
				screen_key_group[0]->bkctbl[3]=RED;
			 	screen_key_group[0]->caption="返回";
				for(i=1;i<3;i++)
				{	
					screen_key_group[i]=btn_creat(95+(i-1)*80,125,60,40,i,2);
					
					screen_key_group[i]->bkctbl[0]=0X8452;
					screen_key_group[i]->bkctbl[1]=0XAD97;			
					screen_key_group[i]->bkctbl[2]=0XAD97;
					screen_key_group[i]->bkctbl[3]=0X8452;
				}
				screen_key_group[1]->caption=(u8*)"打印";
				screen_key_group[2]->caption=(u8*)"返回";
			
				flistbox->fliter=FLBOX_FLT_GCODE;
				flistbox->type=0;
				flistbox->path="0:\\GCODE";	
//				flistbox->path="0:";					
				filelistbox_scan_filelist(flistbox);
				filelistbox_draw_listbox(flistbox);		
				
				for(i=0;i<1;i++)
				{	
					btn_draw(screen_key_group[i]);
				}
			}
		}


		selx=filelistbox_check(flistbox,&in_obj);		
		selx=screen_key_chk(screen_key_group,&in_obj,1);
		
		if(selx&(1<<6))
		{	
			switch(selx & ~(3<<6))//
			{  
				case 0: 
					CurrentMenu = main_diy;
					redraw_menu=TRUE ;
					for(i=0;i<3;i++)
					{	
						btn_delete(screen_key_group[i]);
					}
					gui_memin_free(screen_key_group); 
					filelistbox_delete(flistbox);
									
				break;			   
			   	default: break;
			}
		}
		else
		{
			if(flistbox->dbclick==0X81)
			{ 
				flistbox->dbclick=0x00;
				redraw_menu=TRUE;
				windows_flag=TRUE;
			}
		}
	}
	else 
	{	
		if(redraw_menu==TRUE)
		{ 
			redraw_menu=FALSE;
		
			gui_draw_arcrectangle(75,80,180,40,6,1,GRAYBLUE,GRAYBLUE);
			gui_fill_rectangle(75,110,180,70,WHITE );//
			gui_draw_rectangle(75,110,180,70,GRAYBLUE);
			filelisttemp=filelist_search(flistbox->list,flistbox->selindex);
			gui_show_strmid(75,80,180,30,BLACK,16,filelisttemp->name,1);	
			
			if(screen_key_group)
			{ 					
				for(i=1;i<3;i++)
				{	
					btn_draw(screen_key_group[i]);
				}
			}
		}	
		selx=screen_key_chk(screen_key_group+1,&in_obj,2);
		if(selx&(1<<6))
		{	
			switch(selx & ~(3<<6))//
			{   
				case 0:	
					filelisttemp=filelist_search(flistbox->list,flistbox->selindex);
					strcpy((char *)card.filename,(const char*)filelisttemp->name);
					if(pname)  
					{	
						pname=gui_path_name(pname,flistbox->path,filelisttemp->name);
						selx=f_open(&card.fgcode,(const TCHAR*)pname,FA_READ);
						if (selx==FR_OK)
						{
							
							strcpy(PrintInfo.printfile,filelisttemp->name);//更新打印文件名
							PrintInfo.filesize =card.fgcode.fsize;			//更新打印文件大小
							USR_UsrLog("printfile is %s\r\n",PrintInfo.printfile);
							USR_UsrLog("filesize is %d\r\n",PrintInfo.filesize);
						
							CurrentMenu = print_diy;
							redraw_menu=TRUE;
							windows_flag=FALSE;	
							for(i=0;i<3;i++)
							{	
								btn_delete(screen_key_group[i]);
							}
							gui_memin_free(screen_key_group);
							filelistbox_delete(flistbox);												
							card_startFileprint();
							starttime=millis();
						}
					}
				break;
				case 1:		
					windows_flag=FALSE;
					filelistbox_draw_listbox(flistbox);
				break;						
			   	default: break;
			}
		}		
	}
	
}

void stop_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	
	gui_phy.back_color = BACK2_DIY;
    progressbar_diy(154,34,100);
	if(redraw_menu==TRUE)
	{ 
		redraw_menu=FALSE;
		gui_draw_arcrectangle(75,80,180,40,6,1,GRAYBLUE,GRAYBLUE);
		gui_fill_rectangle(75,110,180,70,WHITE );
		gui_draw_rectangle(75,110,180,70,GRAYBLUE);
		gui_show_strmid(75,80,180,30,WHITE,16,"打印完成",1);	
		key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(key_group)
		{ 			
			key_group[0]=btn_creat(135,125,60,40,0,2);
			key_group[0]->bkctbl[0]=0X8452;
			key_group[0]->bkctbl[1]=0XAD97;			
			key_group[0]->bkctbl[2]=0XAD97;
			key_group[0]->bkctbl[3]=0X8452;
			key_group[0]->caption=(u8*)"确定";
			btn_draw(key_group[0]);
		}
		
		
	}	
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{	
		switch(selx & ~(3<<6))//
		{   
			case 0:	
				quickStop(); 
            	card_closefile();
            	starttime=0;
            	card.sdprinting = FALSE;
            	PrintInfo.printsd = 0;
            	AT24CXX_WriteOneByte(E_SDPRINT,FALSE);
            	autotempShutdown();
            	setTargetHotend(0,active_extruder);	
            	setTargetBed(0); 

            	memset(CmdBuff.cmdbuffer,0,sizeof(CmdBuff.cmdbuffer));
                //enquecommand("M84");
            	//enquecommand("G28 XY");
            	bufindr = 0;
            	bufindw = 0;
            	buflen = 0;
			
							enquecommand("G92 X0 Y0 Z0 E0");
							enquecommand("G28 XY");
							enquecommand("G0 Z10");
            //	G28_process(0x03);
						
              disable_x(); 
            	disable_y(); 
            	disable_z(); 
            	disable_e0(); 
            	disable_e1();
            	
            	redraw_menu=TRUE;
            	sendFlg = TRUE;
            	CurrentMenu = main_diy;		
			    break;
			default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:手动停止打印
*/
void manual_stop_diy(void)
{
	quickStop(); 
	card_closefile();
	starttime=0;
	card.sdprinting = FALSE;
	PrintInfo.printsd = 0;
	AT24CXX_WriteOneByte(E_SDPRINT,FALSE);
	autotempShutdown();
	setTargetHotend(0,active_extruder);	
	setTargetBed(0); 
  disable_x(); 
	disable_y(); 
	disable_z(); 
	disable_e0(); 
	disable_e1();
	memset(CmdBuff.cmdbuffer,0,sizeof(CmdBuff.cmdbuffer));
    //enquecommand("M84");
	
	enquecommand("G92 X0 Y0 Z0 E0");
	enquecommand("G28 XY");
  enquecommand("G1 Z10");
	
	
	CurrentMenu = main_diy;
	redraw_menu = TRUE;
}
void demo_diy(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1;
	static u8 name=1;
	if(redraw_menu)
	{
		redraw_menu = FALSE;
		key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	//创建三个图标
		if(key_group)
		{
			key_group[0]=btn_creat(0,0,320,240,0,1);
			key_group[0]->picbtnpathu=(u8*)"1:/diy_UI/1.bmp";
			key_group[0]->picbtnpathd=(u8*)"1:/diy_UI/1.bmp";
			btn_draw(key_group[0]);
		}
	}
	//按键检测
	selx=screen_key_chk(key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	if(name>=10)
				name = 1;
			memset(fatbuf,0,50);
			sprintf(fatbuf,"1:/diy_UI/%d.bmp",name++);
			key_group[0]->picbtnpathu=(u8*)fatbuf;
			key_group[0]->picbtnpathd=(u8*)fatbuf;
			btn_draw(key_group[0]);
		  break;
		 
		  default: break;
		}
	}
	if(redraw_menu)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(key_group[i]);
		}
		gui_memin_free(key_group);
		display_next_update_millis=0;
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:通过路径获取文件夹下所有的文件及目录 返回数量
*/
u8 check_sd_allfile(u8 *fp)
{
	u8 res = 0;
	DIR flboxdir;		//flbox专用
	FILINFO flboxfinfo;
	int type = 0;
	u8 findex = 0;
	u8 filewt_idx = 0;  //写入文件索引的索引
	char *fn;   //This function is assuming non-Unicode cfg.
	//
	//gcodefile_t = (gcodefile_s*)mymalloc(SRAMIN,sizeof(gcodefile_s) * MAX_GCODE_NUM);//100个文件或文件夹
	#if _USE_LFN
	flboxfinfo.lfsize = _MAX_LFN * 2 + 1;
	flboxfinfo.lfname = mymalloc(SRAMIN, flboxfinfo.lfsize);
	memset(flboxfinfo.lfname, 0, flboxfinfo.lfsize);
	#endif
	res = f_opendir(&flboxdir, (const TCHAR *)fp); //打开一个目录
	if(flboxfinfo.fname == NULL)return 0; //申请内存失败
	if(res == FR_OK) {
		while(1) {
			findex = (u16)flboxdir.index; //记录下此文件的index值
			res = f_readdir(&flboxdir, &flboxfinfo);
			if(res != FR_OK || flboxfinfo.fname[0] == 0)break;
			#if _USE_LFN
			fn = *flboxfinfo.lfname ? flboxfinfo.lfname : flboxfinfo.fname;
			#else
			fn = flboxfinfo.fname;
			#endif

			if(flboxfinfo.fattrib & (1 << 4)) {
				USR_DbgLog("%d:  ", findex);
				USR_DbgLog("目录:");
				gcodefile_t[filewt_idx].file_index = findex;
				gcodefile_t[filewt_idx].file_type = FOLDER;
				filewt_idx++;
				USR_DbgLog("%s \n", fn);
			}
			type = f_typetell(fn);
			type >>= 4;
			if((1 << type) & (1 << 6)) {
				USR_DbgLog("%d:  ", findex);
				USR_DbgLog("Gcode:");
				gcodefile_t[filewt_idx].file_index = findex;
				gcodefile_t[filewt_idx].file_type = GCODE;
				filewt_idx++;
				USR_DbgLog("%s \n", fn);
			}

		}
	}
	f_closedir(&flboxdir);
	myfree(SRAMIN, flboxfinfo.lfname);
    return filewt_idx;
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:读取某个索引对应的文件名字
*/
void readindex_file(u8 file_index_, u8 *file_name, u8 *fp)
{

	DIR flboxdir;		//flbox专用
	FILINFO flboxfinfo;
	u8 res = 0;
//	int type = 0;
	char *fn;   //This function is assuming non-Unicode cfg.
	#if _USE_LFN
	flboxfinfo.lfsize = _MAX_LFN * 2 + 1;
	flboxfinfo.lfname = mymalloc(SRAMIN, flboxfinfo.lfsize);
	memset(flboxfinfo.lfname, 0, flboxfinfo.lfsize);
	#endif
	res = f_opendir(&flboxdir, (const TCHAR *)fp); //打开一个目录
	if(flboxfinfo.fname == NULL)return ; //申请内存失败
	dir_sdi(&flboxdir, file_index_);
	if(res == FR_OK) 
	{
		res = f_readdir(&flboxdir, &flboxfinfo);
		if(res != FR_OK || flboxfinfo.fname[0] == 0)return ;
		#if _USE_LFN
		fn = *flboxfinfo.lfname ? flboxfinfo.lfname : flboxfinfo.fname;
		#else
		fn = flboxfinfo.fname;
		#endif

	}
	strcpy(file_name, fn);
	f_closedir(&flboxdir);
	myfree(SRAMIN, flboxfinfo.lfname);
}

