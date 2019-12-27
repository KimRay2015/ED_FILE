#include "lcdmenu.h"
#include "ConfigurationStore.h"
#include "lcd.h"
#include "touch.h"
#include "stm32f4xx.h"
#include "stmflash.h"
//#include "beep.h" 
#include "24cxx.h" 
#include "sm_plus.h"


const u8* PATH_XT="1:/icon/xitong.bin";
const u8* PATH_GJ="1:/icon/gongju.bin";
const u8* PATH_DY="1:/icon/dayin.bin";
const u8* PATH_ZT="1:/icon/0201.bin";
const u8* PATH_JQXX="1:/icon/0202.bin";
const u8* PATH_YY="1:/icon/0203.bin";
const u8* PATH_CCSZ="1:/icon/0204.bin";
const u8* PATH_PMJZ="1:/icon/0205.bin";
const u8* PATH_WIFI="1:/icon/0206.bin";
const u8* PATH_FH="1:/icon/0108.bin";
const u8* PATH_SD="1:/icon/0101.bin";
const u8* PATH_YR="1:/icon/0102.bin";
const u8* PATH_ZXHC="1:/icon/0103.bin";
const u8* PATH_TP="1:/icon/0104.bin";
const u8* PATH_FS="1:/icon/0105.bin";
const u8* PATH_JJTZ="1:/icon/0106.bin";
const u8* PATH_SHZC="1:/icon/0107.bin";
const u8* PATH_HM="1:/icon/010201.bin";			//火苗
const u8* PATH_JCT1="1:/icon/010202.bin";		//挤出头1
const u8* PATH_JCT2="1:/icon/010203.bin";		//挤出头2	
const u8* PATH_ZFX="1:/icon/010204.bin";		//左方向
const u8* PATH_YFX="1:/icon/010205.bin";		//右方向	
const u8* PATH_EU="1:/icon/010101.bin";			//E上方向
const u8* PATH_YU="1:/icon/010102.bin";			//Y上方向	
const u8* PATH_ZU="1:/icon/010103.bin";			//Z上方向	
const u8* PATH_XL="1:/icon/010104.bin";			//X左方向	
const u8* PATH_HOME="1:/icon/010105.bin";		//HOME
const u8* PATH_XR="1:/icon/010106.bin";			//X右方向	
const u8* PATH_ED="1:/icon/010107.bin";			//E下方向	
const u8* PATH_YD="1:/icon/010108.bin";			//Y下方向	
const u8* PATH_ZD="1:/icon/010109.bin";			//Z下方向	
const u8* PATH_01MM="1:/icon/010110.bin";		//0.1MM	
const u8* PATH_1MM="1:/icon/010111.bin";		//1MM	
const u8* PATH_10MM="1:/icon/010112.bin";		//10MM

const u8* PATH_E1D="1:/icon/010301.bin";		//E1下
const u8* PATH_E1U="1:/icon/010302.bin";		//E1上
const u8* PATH_E2D="1:/icon/010303.bin";		//E2下
const u8* PATH_E2U="1:/icon/010304.bin";		//E2上

const u8* PATH_YJT="1:/icon/010401.bin";		//右箭头

const u8* PATH_FAN1="1:/icon/010501.bin";		//风扇1
const u8* PATH_FAN2="1:/icon/010502.bin";		//风扇1
const u8* PATH_BOARD="1:/icon/010503.bin";		//主板

const u8* PATH_XMM="1:/icon/020101.bin";		//XMM
const u8* PATH_YMM="1:/icon/020102.bin";		//YMM
const u8* PATH_ZMM="1:/icon/020103.bin";		//ZMM
const u8* PATH_EX1="1:/icon/020104.bin";		//挤出头1
const u8* PATH_EX2="1:/icon/020105.bin";		//挤出头2
const u8* PATH_FIRE="1:/icon/020106.bin";		//火苗


const u8* PATH_PP="1:/icon/020201.bin";			//品牌
const u8* PATH_ID="1:/icon/020202.bin";			//ID
const u8* PATH_BB="1:/icon/020203.bin";			//VERSION
const u8* PATH_UI="1:/icon/020204.bin";			//UI

const u8* PATH_IP="1:/icon/020601.bin";			//IP
const u8* PATH_WIFIMZ="1:/icon/020602.bin";		//WIFI NAME
const u8* PATH_MIMA="1:/icon/020603.bin";		//MIMA
const u8* PATH_WIFIXH="1:/icon/020604.bin";		//WIFI SIGNAL
const u8* PATH_WIFILJ="1:/icon/020605.bin";		//WIFI connect
const u8* PATH_WIFIDK="1:/icon/020606.bin";		//WIFI disconnect

const u8* WARNING_01="加热器或传感器故障！";
const u8* WARNING_02="调平完成！！";
const u8* WARNING_03="是否更新程序？";
u8* PrintPoint=NULL;		//弹框显示文字指针


//typedef void (*menuFunc_t)();
static uint32_t tp_scan_next_update_millis=0;
static uint32_t display_next_update_millis=0;

static u8 base_manual=1;	// 代表每次移动0.1mm


menuFunc_t currentMenu = smmain_screen;
menuFunc_t lastMenu;
menuFunc_t tempMenu=NULL;

bool redraw_screen = true;
u8 test_data_2=12;
int test_data_1=11;
bool windows_flag = false;

u8 ioc_key=0x00;
_data_input input_data;
_filelistbox_obj * flistbox;
_filelistbox_list * filelistx;

_btn_obj **screen_key_group;


float manual_move_length=6;
u32 manual_move_xy_speed=60;
u32 manual_move_z_speed=8;

float preheat_e0_length=1;
u32 preheat_e0_speed=5;

float heater_0_temp = 0;
float bed_temp = 0;
int fanSpeed_temp=0;


void lcd_update(void)
{ 

	
	if (tp_scan_next_update_millis < millis())
	{
	  tp_dev.scan(0);
	  in_obj.get_key(&tp_dev,IN_TYPE_TOUCH);
	  tp_scan_next_update_millis = millis() + 10;
	  //if(TE==1)
	  {
	  	(*currentMenu)();
	  }
	//printf("lcd_update\r\n");
	}
}
void menu_action_gcode(char* pgcode)
{
    enquecommand(pgcode);
}

void start_screen (void)
{
    gui_show_strmid(0,60,lcddev.width,40,RED,16,(u8*)"深马3D打印",1);
	gui_show_strmid(0,100,lcddev.width,40,RED,16,(u8*)"V013 2019-01-07",1);
	gui_show_strmid(180,200,120,40,RED,16,(u8*)"By SM 3Dmaker",1);
}

void main_screen (void)
{
	  u8 selx=0XFF;
	  u8 i;
	  u8 num = 2; 
	  char TempBuffer[32];

	  gui_phy.back_color=LIGHTGRAY;	

		if(redraw_screen==true)				
		{
			redraw_screen=false;			
		
			gui_fill_rectangle(0,0,lcddev.width,lcddev.height,LIGHTGRAY );
			gui_draw_rectangle(0,0,lcddev.width,lcddev.height,LIGHTGRAY);
			
			gui_show_strmid(180,200,120,40,RED,16,(u8*)"By 3D Binmaker",1);

			

			screen_key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
			if(screen_key_group)
			{
         		screen_key_group[0]=btn_creat(40,160,100,40,0,4);
				screen_key_group[1]=btn_creat(190,160,100,40,1,4);
				for(i=0;i<num;i++)
				{
					screen_key_group[i]->bcfucolor=BLACK;
					screen_key_group[i]->bcfdcolor=WHITE;
					screen_key_group[i]->bkctbl[0]=CYAN;
					screen_key_group[i]->bkctbl[1]=RED;
					screen_key_group[i]->bkctbl[2]=CYAN;
					screen_key_group[i]->bkctbl[3]=RED;
				}

				screen_key_group[0]->caption="装卸耗材";
				screen_key_group[1]->caption="SD卡打印";
				for(i=0;i<num;i++)
				{
					btn_draw(screen_key_group[i]);
				}
			}

		}
		
		
			if(display_next_update_millis<millis())
			{
				display_next_update_millis = millis() + 500;
				
				sprintf(TempBuffer, "E0-TEMP:%3.0f/%-3.0f",degHotend(0),degTargetHotend(0));
				gui_show_strmid(0,50,lcddev.width,16,BLACK,16,(u8 *)TempBuffer,0);
				
				sprintf(TempBuffer, "BED-TEMP:%3.0f/%-3.0f",degBed(),degTargetBed());
				gui_show_strmid(0,100,lcddev.width,16,BLACK,16,(u8 *)TempBuffer,0);
			}
		

 		selx=screen_key_chk(screen_key_group,&in_obj,num);
		if(selx&(1<<6))
		{
			switch(selx & ~(3<<6))
					{   
				      case 0: 
								      currentMenu = preheat_screen;
						          redraw_screen=true;
											break;
							case 1: 
										  currentMenu = gecodelist_screen;
						          redraw_screen=true;
											break;									
					   	default: break;
					}
		}

	  if(redraw_screen)
		{
			for(i=0;i<num;i++)
			{	
				btn_delete(screen_key_group[i]);
			}
			gui_memin_free(screen_key_group);
			display_next_update_millis=0;	
		}
}




void preheat_screen (void)
{	
		u8 selx=0XFF;
		u8 i;
	  u8 num = 3; 
	 char TempBuffer[32];

	  gui_phy.back_color=LIGHTGRAY;	

		if(redraw_screen==true)				
		{
			redraw_screen=false;			
		
			gui_fill_rectangle(0,0,lcddev.width,lcddev.height,LIGHTGRAY );
			gui_fill_rectangle(0,0,lcddev.width,40,LIGHTBLUE);
			gui_draw_hline(0,40,lcddev.width,GRAY);
			gui_show_strmid(0,0,320,40,WHITE,16,(u8*)"装卸耗材",1);
//			gui_show_strmid(0,0,320,40,WHITE,16,(u8*)"SD card print",1);

			gui_show_strmid(180,200,120,40,RED,16,(u8*)"By 3D Binmaker",1);

			

			screen_key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
			if(screen_key_group)
			{
          		screen_key_group[0]=btn_creat(40,140,100,40,0,4);
				screen_key_group[1]=btn_creat(190,140,100,40,1,4);
				  screen_key_group[2]=btn_creat(8,8,50,25,2,4);
						for(i=0;i<num;i++)
						{
							screen_key_group[i]->bcfucolor=BLACK;
							screen_key_group[i]->bcfdcolor=WHITE;
							screen_key_group[i]->bkctbl[0]=CYAN;
							screen_key_group[i]->bkctbl[1]=RED;
							screen_key_group[i]->bkctbl[2]=CYAN;
							screen_key_group[i]->bkctbl[3]=RED;
						}


						screen_key_group[0]->caption="加热";
						screen_key_group[1]->caption="挤出";
						screen_key_group[2]->caption="返回";
						
//						screen_key_group[0]->caption="heat";
//						screen_key_group[1]->caption="extrusion";
//						screen_key_group[2]->caption="return";
						
						for(i=0;i<num;i++)
						{
							btn_draw(screen_key_group[i]);
						}
			}

	 }
		
		
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 500;

 		sprintf(TempBuffer, "E0-TEMP:%3.0f/%-3.0f",degHotend(0),degTargetHotend(0));
				gui_show_strmid(0,80,lcddev.width,16,BLACK,16,(u8 *)TempBuffer,0);
	}
		
		
 		selx=screen_key_chk(screen_key_group,&in_obj,num);
		if(selx&(1<<6))
		{
			switch(selx & ~(3<<6))
			{   
		      case 0: 
		        if(target_temperature[0] == 0)
				{
					target_temperature[0] = 210;
					sprintf(TempBuffer, "M104 S%.1f",(float)target_temperature[0]);
					menu_action_gcode(TempBuffer);

					screen_key_group[0]->bkctbl[0]=RED;
					screen_key_group[0]->bkctbl[1]=CYAN;
					screen_key_group[0]->bkctbl[2]=RED;
					screen_key_group[0]->bkctbl[3]=CYAN;
					btn_draw(screen_key_group[0]);
				}
				else
				{
				  target_temperature[0] = 0;
					sprintf(TempBuffer, "M104 S%.1f",(float)target_temperature[0]);
					menu_action_gcode(TempBuffer);

					screen_key_group[0]->bkctbl[0]=CYAN;
					screen_key_group[0]->bkctbl[1]=RED;
					screen_key_group[0]->bkctbl[2]=CYAN;
					screen_key_group[0]->bkctbl[3]=RED;
					btn_draw(screen_key_group[0]);
				}
				break;
				case 1: 
			  	 menu_action_gcode("G91");
				 sprintf(TempBuffer, "G1 E%.1f F%d",10.0,1200);
				 menu_action_gcode(TempBuffer);
				 menu_action_gcode("G90");
				break;	
 				 case 2: 
		      		target_temperature[0] = 0;
					sprintf(TempBuffer, "M104 S%.1f",(float)target_temperature[0]);
					menu_action_gcode(TempBuffer);

					screen_key_group[0]->bkctbl[0]=CYAN;
					screen_key_group[0]->bkctbl[1]=RED;
					screen_key_group[0]->bkctbl[2]=CYAN;
					screen_key_group[0]->bkctbl[3]=RED;
					btn_draw(screen_key_group[0]);
				 	currentMenu = main_screen;
        			redraw_screen=true;
				break;							
			   	default: break;
			}
		}

	  if(redraw_screen)
		{
			for(i=0;i<num;i++)
			{	
				btn_delete(screen_key_group[i]);
			}
			gui_memin_free(screen_key_group);
			display_next_update_millis=0;	
		}
}



void gecodelist_screen(void)
{ 
	u8 selx=0XFF;
	u8 i;	
//	u8 *pname=0;
 	_filelistbox_list * filelisttemp;	
 	if(windows_flag==false)
	{	 
		if(redraw_screen==true)			
		{ 
			redraw_screen=false;		

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
					currentMenu = smmain_screen;
					redraw_screen=true ;
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
				redraw_screen=true;
				windows_flag=true;
			}
		}
	}
	else 
	{	
		if(redraw_screen==true)
		{ 
			redraw_screen=false;
		
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
						
							currentMenu = print_screen;
							redraw_screen=true;
							windows_flag=false;	
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
					windows_flag=false;
					filelistbox_draw_listbox(flistbox);
				break;						
			   	default: break;
			}
		}		
	}
	
}





void print_screen(void)
{
	u8 i;
	u8 selx=0XFF;
	char TempBuffer[32];
	u8 num=3;

	if(redraw_screen==true)
	{
		redraw_screen=false;
		display_next_update_millis=0;
  		gui_phy.back_color=LIGHTGRAY;	
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,LIGHTGRAY );
		gui_fill_rectangle(0,0,lcddev.width,40,LIGHTBLUE);  
		gui_draw_hline(0,40,lcddev.width,GRAY);
		
   		gui_show_strmid(0,0,lcddev.width,40,WHITE,16,"打印中",1);
	  //gui_show_strmid(0,0,lcddev.width,40,WHITE,16,"PRINTING",1);
		screen_key_group =(_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);	
		if(screen_key_group)
		{
      		screen_key_group[0]=btn_creat(40,180,100,40,0,4);
			screen_key_group[1]=btn_creat(190,180,100,40,1,4);
			screen_key_group[2]=btn_creat(190,130,100,40,2,4);
			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bcfucolor=BLACK;
				screen_key_group[i]->bcfdcolor=WHITE;
				screen_key_group[i]->bkctbl[0]=CYAN;
				screen_key_group[i]->bkctbl[1]=RED;
			}

			screen_key_group[0]->caption="暂停";
			screen_key_group[1]->caption="急停";
			screen_key_group[2]->caption="温度设置";

			
			
			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}
	}
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 500;

		sprintf(TempBuffer, "X:%8.3fmm",current_position[X_AXIS]);
		 gui_show_strmid(20,70,120,16,BLACK,16,(u8 *)TempBuffer,0);
		sprintf(TempBuffer, "Y:%8.3fmm",current_position[Y_AXIS]);
		 gui_show_strmid(20,100,120,16,BLACK,16,(u8 *)TempBuffer,0);
		sprintf(TempBuffer, "Z:%8.3fmm",current_position[Z_AXIS]);
		 gui_show_strmid(20,130,120,16,BLACK,16,(u8 *)TempBuffer,0);
		
 		sprintf(TempBuffer, "E0-TEMP:%3.0f/%-3.0f",degHotend(0),degTargetHotend(0));
		gui_show_strmid(180,70,120,16,BLACK,16,(u8 *)TempBuffer,0);
		
 		sprintf(TempBuffer, "BED-TEMP:%3.0f/%-3.0f",degBed(),degTargetBed());
		gui_show_strmid(180,100,120,16,BLACK,16,(u8 *)TempBuffer,0);  
		 if(card_eof())
		 {

		  currentMenu = smmain_screen;
		  redraw_screen=true ;
		 }

	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{ 
			case 0: 
				if(card.sdprinting == true)
				{
					screen_key_group[0]->caption="继续";
					btn_draw(screen_key_group[0]);
					card.sdprinting = false;
				}
				else
				{
					screen_key_group[0]->caption="暂停";
					btn_draw(screen_key_group[0]);
					card.sdprinting = true;
					TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);
				} 
			break;
			case 1: 
				quickStop(); 
				card_closefile();
				starttime=0;
				card.sdprinting = false;
				AT24CXX_WriteOneByte(E_SDPRINT,false);
				autotempShutdown();
				setTargetHotend(0,active_extruder);
				heater_0_temp = 0;
				bed_temp = 0;

		        disable_x(); 
				disable_y(); 
				disable_z(); 
				disable_e0(); 
				disable_e1(); 
			    menu_action_gcode("M84") ;
				redraw_screen=true;
				currentMenu = smmain_screen;
			break;	
			case 2:
				redraw_screen=true;
				lastMenu = currentMenu;
				currentMenu = smpreheat_screen;
			break;
			default: break;
		}	
	}
	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;		
	}
}


void smmain_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 3; 
	char TempBuffer[32];
	gui_phy.back_color=WHITE;	

	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		
		gui_show_strmid(180,200,120,40,RED,16,(u8*)"By SM_3DPrinter",1);
		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(55,27,80,96,0,5);
			screen_key_group[1]=btn_creat(55,130,80,96,1,5);
			screen_key_group[2]=btn_creat(166,66,80,96,2,5);
			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=LGRAY;
				screen_key_group[i]->caption_top = 78;
				screen_key_group[i]->icon_height = 78;
				screen_key_group[i]->icon_width = 80;
			}
			screen_key_group[0]->bkctbl[2]=CYAN;
			screen_key_group[0]->bkctbl[3]=CYAN;
			screen_key_group[0]->caption="系统";
			screen_key_group[0]->picbtnpathu = (u8*)PATH_XT;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_XT;
			
			screen_key_group[1]->bkctbl[2]=GREEN;
			screen_key_group[1]->bkctbl[3]=GREEN;
			screen_key_group[1]->caption="工具";
			screen_key_group[1]->picbtnpathu = (u8*)PATH_GJ;
			screen_key_group[1]->picbtnpathd = (u8*)PATH_GJ;

			screen_key_group[2]->bkctbl[2]=BLUE;
			screen_key_group[2]->bkctbl[3]=BLUE;
			screen_key_group[2]->caption="打印";
			screen_key_group[2]->picbtnpathu = (u8*)PATH_DY;
			screen_key_group[2]->picbtnpathd = (u8*)PATH_DY;

			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}
	}
		
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
		  	gui_show_strmid(180,200,128,40,RED,16,(u8*)"触发《系统》按钮",0);
			currentMenu = sys_screen;
			redraw_screen=true;
		  break;
		  case 1:
		  	gui_show_strmid(180,200,128,40,RED,16,(u8*)"触发《工具》按钮",0);
			currentMenu = tool_screen;
			redraw_screen=true;
		  break;
		  case 2:
		  	gui_show_strmid(180,200,128,40,RED,16,(u8*)"触发《打印》按钮",0);
			currentMenu = gecodelist_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	  if(redraw_screen)
		{
			for(i=0;i<num;i++)
			{	
				btn_delete(screen_key_group[i]);
			}
			gui_memin_free(screen_key_group);
			display_next_update_millis=0;	
		}
}

void sys_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 7; 
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		
		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(12,33,64,71,0,5);
			screen_key_group[1]=btn_creat(89,33,64,71,1,5);
			screen_key_group[2]=btn_creat(166,33,64,71,2,5);
			screen_key_group[3]=btn_creat(243,33,64,71,3,5);
			screen_key_group[4]=btn_creat(12,137,64,71,4,5);
			screen_key_group[5]=btn_creat(89,137,64,71,5,5);
			screen_key_group[6]=btn_creat(243,137,64,71,6,5);
			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=LGRAY;
				screen_key_group[i]->caption_top = 55;
				screen_key_group[i]->icon_height = 55;
				screen_key_group[i]->icon_width = 56;
				screen_key_group[i]->icon_left = 4;
			}
			screen_key_group[0]->bkctbl[2]=CYAN;
			screen_key_group[0]->bkctbl[3]=CYAN;
			screen_key_group[0]->caption="状态";
			screen_key_group[0]->picbtnpathu = (u8*)PATH_ZT;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_ZT;
			
			screen_key_group[1]->bkctbl[2]=GREEN;
			screen_key_group[1]->bkctbl[3]=GREEN;
			screen_key_group[1]->caption="机器信息";
			screen_key_group[1]->picbtnpathu = (u8*)PATH_JQXX;
			screen_key_group[1]->picbtnpathd = (u8*)PATH_JQXX;

			screen_key_group[2]->bkctbl[2]=BLUE;
			screen_key_group[2]->bkctbl[3]=BLUE;
			screen_key_group[2]->caption="语言";
			screen_key_group[2]->picbtnpathu = (u8*)PATH_YY;
			screen_key_group[2]->picbtnpathd = (u8*)PATH_YY;

			screen_key_group[3]->bkctbl[2]=BRED;
			screen_key_group[3]->bkctbl[3]=BRED;
			screen_key_group[3]->caption="程序更新";
			screen_key_group[3]->picbtnpathu = (u8*)PATH_CCSZ;
			screen_key_group[3]->picbtnpathd = (u8*)PATH_CCSZ;

			screen_key_group[4]->bkctbl[2]=GRED;
			screen_key_group[4]->bkctbl[3]=GRED;
			screen_key_group[4]->caption="屏幕校正";
			screen_key_group[4]->picbtnpathu = (u8*)PATH_PMJZ;
			screen_key_group[4]->picbtnpathd = (u8*)PATH_PMJZ;

			screen_key_group[5]->bkctbl[2]=GBLUE;
			screen_key_group[5]->bkctbl[3]=GBLUE;
			screen_key_group[5]->caption="WIFI";
			screen_key_group[5]->picbtnpathu = (u8*)PATH_WIFI;
			screen_key_group[5]->picbtnpathd = (u8*)PATH_WIFI;

			screen_key_group[6]->bkctbl[2]=BLUE;
			screen_key_group[6]->bkctbl[3]=BLUE;
			screen_key_group[6]->caption="返回";
			screen_key_group[6]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[6]->picbtnpathd = (u8*)PATH_FH;

			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}

	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"状态",0);
			currentMenu = status_screen;
			redraw_screen=true;
		  break;
		  case 1:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"机器信息",0);
			currentMenu = about_screen;
			redraw_screen=true;
		  break;
		  case 2:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"语言",0);
			PrintPoint = (u8*)WARNING_01;
			lastMenu = currentMenu;
		  	currentMenu = popout_screen;
			redraw_screen = true;
		  break;
		  case 3:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"出厂设置",0);
			PrintPoint = (u8*)WARNING_03;
			lastMenu = currentMenu;
		  	currentMenu = popout_screen;
			tempMenu = default_screen;
			redraw_screen = true;
		  break;
		  case 4:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"屏幕校正",0);
			TP_Adjust();
			redraw_screen=true;
		  break;
		  case 5:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"WIFI",0);
			currentMenu = wifi_screen;
			redraw_screen=true;
		  break;		  	
		  case 6:
		  	currentMenu = smmain_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}
}


void tool_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 8; 
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		
		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(13,32,64,72,0,5);
			screen_key_group[1]=btn_creat(90,32,64,72,1,5);
			screen_key_group[2]=btn_creat(167,32,64,72,2,5);			
			screen_key_group[3]=btn_creat(244,32,64,72,3,5);
			
			screen_key_group[4]=btn_creat(13,136,64,72,4,5);
			screen_key_group[5]=btn_creat(90,136,64,72,5,5);			
			screen_key_group[6]=btn_creat(167,136,64,72,6,5);
			screen_key_group[7]=btn_creat(244,136,64,72,7,5);

			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=LGRAY;
				screen_key_group[i]->caption_top = 55;
				screen_key_group[i]->icon_height = 55;
				screen_key_group[i]->icon_width = 56;
				screen_key_group[i]->icon_left = 4;
			}
			screen_key_group[0]->bkctbl[2]=CYAN;
			screen_key_group[0]->bkctbl[3]=CYAN;
			screen_key_group[0]->caption="手动";
			screen_key_group[0]->picbtnpathu = (u8*)PATH_SD;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_SD;
			
			screen_key_group[1]->bkctbl[2]=GREEN;
			screen_key_group[1]->bkctbl[3]=GREEN;
			screen_key_group[1]->caption="预热";
			screen_key_group[1]->picbtnpathu = (u8*)PATH_YR;
			screen_key_group[1]->picbtnpathd = (u8*)PATH_YR;

			screen_key_group[2]->bkctbl[2]=BLUE;
			screen_key_group[2]->bkctbl[3]=BLUE;
			screen_key_group[2]->caption="装卸耗材";
			screen_key_group[2]->picbtnpathu = (u8*)PATH_ZXHC;
			screen_key_group[2]->picbtnpathd = (u8*)PATH_ZXHC;

			screen_key_group[3]->bkctbl[2]=BRED;
			screen_key_group[3]->bkctbl[3]=BRED;
			screen_key_group[3]->caption="调平";
			screen_key_group[3]->picbtnpathu = (u8*)PATH_TP;
			screen_key_group[3]->picbtnpathd = (u8*)PATH_TP;

			screen_key_group[4]->bkctbl[2]=GRED;
			screen_key_group[4]->bkctbl[3]=GRED;
			screen_key_group[4]->caption="风扇";
			screen_key_group[4]->picbtnpathu = (u8*)PATH_FS;
			screen_key_group[4]->picbtnpathd = (u8*)PATH_FS;

			screen_key_group[5]->bkctbl[2]=GBLUE;
			screen_key_group[5]->bkctbl[3]=GBLUE;
			screen_key_group[5]->caption="紧急停止";
			screen_key_group[5]->picbtnpathu = (u8*)PATH_JJTZ;
			screen_key_group[5]->picbtnpathd = (u8*)PATH_JJTZ;

			screen_key_group[6]->bkctbl[2]=BLUE;
			screen_key_group[6]->bkctbl[3]=BLUE;
			screen_key_group[6]->caption="售后支持";
			screen_key_group[6]->picbtnpathu = (u8*)PATH_SHZC;
			screen_key_group[6]->picbtnpathd = (u8*)PATH_SHZC;

			screen_key_group[7]->bkctbl[2]=BROWN;
			screen_key_group[7]->bkctbl[3]=BROWN;
			screen_key_group[7]->caption="返回";
			screen_key_group[7]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[7]->picbtnpathd = (u8*)PATH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}

	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"手动",0);
			currentMenu = manual_screen;
			redraw_screen=true;
		  break;
		  case 1:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"预热",0);
			lastMenu = currentMenu;
			currentMenu = smpreheat_screen;
			redraw_screen=true;
		  break;
		  case 2:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"装卸耗材",0);
			tempMenu = currentMenu;
			currentMenu = filament_screen;
			redraw_screen=true;
		  break;
		  case 3:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"调平",0);
			currentMenu = levelling_screen;
			redraw_screen=true;
		  break;
		  case 4:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"风扇",0);
			currentMenu = fan_screen;
			redraw_screen=true;
		  break;
		  case 5:
		  	gui_show_strmid(180,200,64,40,RED,16,(u8*)"紧急停止",0);
		  break;
		  case 6:
			gui_show_strmid(180,200,64,40,RED,16,(u8*)"售后支持",0);
			currentMenu = support_screen;
			redraw_screen=true;
		  break;		  	
		  case 7:
		  	currentMenu = smmain_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}
}

void smpreheat_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 10; 
	char TempBuffer[64];
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;	
		display_next_update_millis = millis() + 500;
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);

		//画图标
		gui_draw_single_color_icos(19,5,32,50,(u8*)PATH_HM,RED,WHITE);
		gui_draw_single_color_icos(19,65,32,50,(u8*)PATH_JCT1,MAGENTA,WHITE);
		gui_draw_single_color_icos(19,125,32,50,(u8*)PATH_JCT2,MAGENTA,WHITE);
		
		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(100,9,32,42,0,5);
			screen_key_group[1]=btn_creat(147,9,100,42,1,4);
			screen_key_group[2]=btn_creat(262,9,32,42,2,5);
			
			screen_key_group[3]=btn_creat(100,69,32,42,3,5);
			screen_key_group[4]=btn_creat(147,69,100,42,4,4);
			screen_key_group[5]=btn_creat(262,69,32,42,5,5);

			screen_key_group[6]=btn_creat(100,129,32,42,6,5);
			screen_key_group[7]=btn_creat(147,129,100,42,7,4);
			screen_key_group[8]=btn_creat(262,129,32,42,8,5);
			
			screen_key_group[9]=btn_creat(250,183,56,55,9,5);
			
			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=GRAY;
				screen_key_group[i]->bcfucolor = BLACK;
				screen_key_group[i]->bcfdcolor = BLACK;
			}
			for(i=0;i<3;i++)
			{
				screen_key_group[i*3]->bkctbl[2]=GREEN;
				screen_key_group[i*3]->bkctbl[3]=GREEN;
				screen_key_group[i*3]->picbtnpathu = (u8*)PATH_ZFX;
				screen_key_group[i*3]->picbtnpathd = (u8*)PATH_ZFX;

				screen_key_group[2+i*3]->bkctbl[2]=BLUE;
				screen_key_group[2+i*3]->bkctbl[3]=BLUE;
				screen_key_group[2+i*3]->picbtnpathu = (u8*)PATH_YFX;
				screen_key_group[2+i*3]->picbtnpathd = (u8*)PATH_YFX;	

				screen_key_group[1+i*3]->bkctbl[0]=LGRAY;
			}
			gui_phy.back_color=LIGHTGRAY;
			gui_fill_rectangle(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,LGRAY);
			sprintf(TempBuffer, "%d/%-d",(int)degBed(),(int)degTargetBed());
			if(degTargetBed()  < BED_MINTEMP)
			{
			//	screen_key_group[1]->bcfucolor = BLACK;
			//	screen_key_group[1]->bcfdcolor = BLACK;
				gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,BLACK,16,TempBuffer,0);
			}
			else
			{
			//	screen_key_group[1]->bcfucolor = RED;
			//	screen_key_group[1]->bcfdcolor = RED;
			gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,RED,16,TempBuffer,1);
			}

			gui_fill_rectangle(screen_key_group[4]->left,screen_key_group[4]->top,screen_key_group[4]->width,screen_key_group[4]->height,LGRAY);
			sprintf(TempBuffer, "%d/%-d",(int)degHotend(0),(int)degTargetHotend(0));
			if(degTargetHotend(0) < HEATER_0_MINTEMP)
			{
			//	screen_key_group[4]->bcfucolor = BLACK;
			//	screen_key_group[4]->bcfdcolor = BLACK;
				gui_show_strmid(screen_key_group[4]->left,screen_key_group[4]->top,screen_key_group[4]->width,screen_key_group[4]->height,BLACK,16,TempBuffer,1);
			}
			else
			{
			//	screen_key_group[4]->bcfucolor = RED;
			//	screen_key_group[4]->bcfdcolor = RED;
			gui_show_strmid(screen_key_group[4]->left,screen_key_group[4]->top,screen_key_group[4]->width,screen_key_group[4]->height,RED,16,TempBuffer,1);
			}
			
		//	sprintf(TempBuffer, "%d/%-d",(int)degBed(),(int)degTargetBed());
		//	screen_key_group[1]->caption=TempBuffer;
		//	sprintf(TempBuffer, "%d/%-d",(int)degHotend(0),(int)degTargetHotend(0));
		//	screen_key_group[4]->caption=TempBuffer;
			
			screen_key_group[7]->caption="--/--";

			screen_key_group[9]->bkctbl[2]=BROWN;
			screen_key_group[9]->bkctbl[3]=BROWN;
			screen_key_group[9]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[9]->picbtnpathd = (u8*)PATH_FH;
			gui_phy.back_color=WHITE;	
			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}

	}

	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 500;
//		sprintf(TempBuffer, "%d/%-d",(int)current_temperature_bed,(int)target_temperature_bed);
//		screen_key_group[1]->caption=TempBuffer;
//		memset(TempBuffer,0,sizeof(TempBuffer));
//		sprintf(TempBuffer, "%d/%-d",(int)current_temperature[0],(int)current_temperature[0]);
//		screen_key_group[4]->caption=TempBuffer;

		gui_phy.back_color=LIGHTGRAY;	
		gui_fill_rectangle(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,LGRAY);
		sprintf(TempBuffer, "%d/%-d",(int)current_temperature_bed,(int)target_temperature_bed);
		if(degTargetBed() < BED_MINTEMP)
		{
		//	screen_key_group[1]->bcfucolor = BLACK;
		//	screen_key_group[1]->bcfdcolor = BLACK;
			gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,BLACK,16,TempBuffer,1);
		}
		else
		{
		//	screen_key_group[1]->bcfucolor = RED;
		//	screen_key_group[1]->bcfdcolor = RED;
			gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,RED,16,TempBuffer,1);
		}
		
		memset(TempBuffer,0,sizeof(TempBuffer));
		gui_fill_rectangle(screen_key_group[4]->left,screen_key_group[4]->top,screen_key_group[4]->width,screen_key_group[4]->height,LGRAY);
		sprintf(TempBuffer, "%d/%-d",(int)current_temperature[0],(int)target_temperature[0]);
		if(degTargetHotend(0) < HEATER_0_MINTEMP)
		{
		//	screen_key_group[4]->bcfucolor = BLACK;
		//	screen_key_group[4]->bcfdcolor = BLACK;
		gui_show_strmid(screen_key_group[4]->left,screen_key_group[4]->top,screen_key_group[4]->width,screen_key_group[4]->height,BLACK,16,TempBuffer,1);
		}
		else
		{
		//	screen_key_group[4]->bcfucolor = RED;
		//	screen_key_group[4]->bcfdcolor = RED;
		gui_show_strmid(screen_key_group[4]->left,screen_key_group[4]->top,screen_key_group[4]->width,screen_key_group[4]->height,RED,16,TempBuffer,1);
		}
		
//		btn_draw(screen_key_group[1]);
//		btn_draw(screen_key_group[4]);
	}
	
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
		  	if(degTargetBed()<20)
				target_temperature_bed = 0;
			else if(degTargetBed()<BED_MAXTEMP)
				target_temperature_bed -= 5;
			sprintf(TempBuffer, "M140 S%.1f",(float)target_temperature_bed);
			menu_action_gcode(TempBuffer);
		  break;
		  case 1:
		  	if(degTargetBed()<40)
				target_temperature_bed = DEFAULT_BED_TEMP;
			else 
				target_temperature_bed = 0;
			sprintf(TempBuffer, "M140 S%.1f",(float)target_temperature_bed);
			menu_action_gcode(TempBuffer);
		  break;
		  case 2:
		  	if(degTargetBed()<40)
				target_temperature_bed = 40;
			else if(degTargetBed()<BED_MAXTEMP)
				target_temperature_bed += 5;
			sprintf(TempBuffer, "M140 S%.1f",(float)target_temperature_bed);
			menu_action_gcode(TempBuffer);
		  break;
		  case 3:
		  	if(degTargetHotend(0)<30)
				target_temperature[0] = 0;
			else if(degTargetHotend(0)<HEATER_0_MAXTEMP)
				target_temperature[0] -= 5;
			sprintf(TempBuffer, "M104 S%.1f",(float)target_temperature[0]);
			menu_action_gcode(TempBuffer);
		  break;
		  case 4:
		  	if(degTargetHotend(0)<40)
				target_temperature[0] = DEFAULT_HEATER_TEMP;
			else
				target_temperature[0] = 0;
			sprintf(TempBuffer, "M104 S%.1f",(float)target_temperature[0]);
			menu_action_gcode(TempBuffer);
		  break;
		  case 5:
		  	if(degTargetHotend(0)<60)
				target_temperature[0] = 60;
			else if(degTargetHotend(0)<HEATER_0_MAXTEMP)
				target_temperature[0] += 5; 
			sprintf(TempBuffer, "M104 S%.1f",(float)target_temperature[0]);
			menu_action_gcode(TempBuffer);
		  break;
		  case 6:
			;
		  break;		  	
		  case 7:
			;
		  break;
		  case 8:
			;
		  break;
		  case 9:
		  	//currentMenu = tool_screen;
		  	currentMenu = lastMenu;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);

		display_next_update_millis=0;	
		
	}

}

void manual_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 13; 
	char TempBuffer[32];
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);

		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(32,5,64,48,0,5);
			screen_key_group[1]=btn_creat(128,5,64,48,1,5);
			screen_key_group[2]=btn_creat(224,5,64,48,2,5);
			
			screen_key_group[3]=btn_creat(70,58,48,64,3,5);
			screen_key_group[4]=btn_creat(128,58,64,64,4,5);
			screen_key_group[5]=btn_creat(202,58,48,64,5,5);

			screen_key_group[6]=btn_creat(32,127,64,48,6,5);
			screen_key_group[7]=btn_creat(128,127,64,48,7,5);
			screen_key_group[8]=btn_creat(224,127,64,48,8,5);

			screen_key_group[9]=btn_creat(78,180,48,30,9,5);
			screen_key_group[10]=btn_creat(136,180,48,30,10,5);
			screen_key_group[11]=btn_creat(194,180,48,30,10,5);

			screen_key_group[12]=btn_creat(260,180,56,55,10,5);

			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=GRAY;
				screen_key_group[i]->bcfucolor = BLACK;
				screen_key_group[i]->bcfdcolor = BLACK;
			}

			screen_key_group[0]->bkctbl[2]=GREEN;
			screen_key_group[0]->bkctbl[3]=GREEN;
			screen_key_group[0]->picbtnpathu = (u8*)PATH_EU;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_EU;

			screen_key_group[1]->bkctbl[2]=BLUE;
			screen_key_group[1]->bkctbl[3]=BLUE;
			screen_key_group[1]->picbtnpathu = (u8*)PATH_YU;
			screen_key_group[1]->picbtnpathd = (u8*)PATH_YU;

			screen_key_group[2]->bkctbl[2]=MAGENTA;
			screen_key_group[2]->bkctbl[3]=MAGENTA;
			screen_key_group[2]->picbtnpathu = (u8*)PATH_ZU;
			screen_key_group[2]->picbtnpathd = (u8*)PATH_ZU;

			screen_key_group[3]->bkctbl[2]=DCYAN;
			screen_key_group[3]->bkctbl[3]=DCYAN;
			screen_key_group[3]->picbtnpathu = (u8*)PATH_XL;
			screen_key_group[3]->picbtnpathd = (u8*)PATH_XL;

			screen_key_group[4]->bkctbl[2]=LIGHTBLUE;
			screen_key_group[4]->bkctbl[3]=LIGHTBLUE;
			screen_key_group[4]->picbtnpathu = (u8*)PATH_HOME;
			screen_key_group[4]->picbtnpathd = (u8*)PATH_HOME;

			screen_key_group[5]->bkctbl[2]=DCYAN;
			screen_key_group[5]->bkctbl[3]=DCYAN;
			screen_key_group[5]->picbtnpathu = (u8*)PATH_XR;
			screen_key_group[5]->picbtnpathd = (u8*)PATH_XR;

			screen_key_group[6]->bkctbl[2]=GREEN;
			screen_key_group[6]->bkctbl[3]=GREEN;
			screen_key_group[6]->picbtnpathu = (u8*)PATH_ED;
			screen_key_group[6]->picbtnpathd = (u8*)PATH_ED;

			screen_key_group[7]->bkctbl[2]=BLUE;
			screen_key_group[7]->bkctbl[3]=BLUE;
			screen_key_group[7]->picbtnpathu = (u8*)PATH_YD;
			screen_key_group[7]->picbtnpathd = (u8*)PATH_YD;

			screen_key_group[8]->bkctbl[2]=MAGENTA;
			screen_key_group[8]->bkctbl[3]=MAGENTA;
			screen_key_group[8]->picbtnpathu = (u8*)PATH_ZD;
			screen_key_group[8]->picbtnpathd = (u8*)PATH_ZD;

			if(base_manual==1)
			{
				screen_key_group[9]->bkctbl[2]=NAVY;
				screen_key_group[9]->bkctbl[3]=NAVY;
			}
			else
			{
				screen_key_group[9]->bkctbl[2]=LIGHTBLUE;
				screen_key_group[9]->bkctbl[3]=LIGHTBLUE;
			}
			screen_key_group[9]->picbtnpathu = (u8*)PATH_01MM;
			screen_key_group[9]->picbtnpathd = (u8*)PATH_01MM;

			if(base_manual==10)
			{
				screen_key_group[10]->bkctbl[2]=NAVY;
				screen_key_group[10]->bkctbl[3]=NAVY;
			}
			else
			{
				screen_key_group[10]->bkctbl[2]=LIGHTBLUE;
				screen_key_group[10]->bkctbl[3]=LIGHTBLUE;
			}
			screen_key_group[10]->picbtnpathu = (u8*)PATH_1MM;
			screen_key_group[10]->picbtnpathd = (u8*)PATH_1MM;

			if(base_manual==100)
			{
				screen_key_group[11]->bkctbl[2]=NAVY;
				screen_key_group[11]->bkctbl[3]=NAVY;
			}
			else
			{
				screen_key_group[11]->bkctbl[2]=LIGHTBLUE;
				screen_key_group[11]->bkctbl[3]=LIGHTBLUE;
			}
			screen_key_group[11]->picbtnpathu = (u8*)PATH_10MM;
			screen_key_group[11]->picbtnpathd = (u8*)PATH_10MM;

			screen_key_group[12]->bkctbl[2]=BROWN;
			screen_key_group[12]->bkctbl[3]=BROWN;
			screen_key_group[12]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[12]->picbtnpathd = (u8*)PATH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}

	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
		  	menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 E%.1f F%d",-0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;
		  case 1:
			menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 Y%.1f F%d",0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;
		  case 2:
		  	menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 Z%.1f F%d",0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;
		  case 3:
		  	menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 X%.1f F%d",-0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;
		  case 4:
		  	menu_action_gcode("G28");
		  break;
		  case 5:
		  	menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 X%.1f F%d",0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;
		  case 6:
			menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 E%.1f F%d",0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;		  	
		  case 7:
			menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 Y%.1f F%d",-0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;
		  case 8:
			menu_action_gcode("G91");
			sprintf(TempBuffer, "G1 Z%.1f F%d",-0.1*base_manual,1200);
			menu_action_gcode(TempBuffer);
			menu_action_gcode("G90");
		  break;
		  case 9:
			base_manual = 1;
			screen_key_group[9]->bkctbl[2]=NAVY;
			screen_key_group[9]->bkctbl[3]=NAVY;
			screen_key_group[10]->bkctbl[2]=LIGHTBLUE;
			screen_key_group[10]->bkctbl[3]=LIGHTBLUE;
			screen_key_group[11]->bkctbl[2]=LIGHTBLUE;
			screen_key_group[11]->bkctbl[3]=LIGHTBLUE;
			btn_draw(screen_key_group[9]);
			btn_draw(screen_key_group[10]);
			btn_draw(screen_key_group[11]);
		  break;
		  case 10:
			base_manual = 10;
			screen_key_group[10]->bkctbl[2]=NAVY;
			screen_key_group[10]->bkctbl[3]=NAVY;
			screen_key_group[9]->bkctbl[2]=LIGHTBLUE;
			screen_key_group[9]->bkctbl[3]=LIGHTBLUE;
			screen_key_group[11]->bkctbl[2]=LIGHTBLUE;
			screen_key_group[11]->bkctbl[3]=LIGHTBLUE;
			btn_draw(screen_key_group[9]);
			btn_draw(screen_key_group[10]);
			btn_draw(screen_key_group[11]);
		  break;
		  case 11:
			base_manual = 100;
			screen_key_group[11]->bkctbl[2]=NAVY;
			screen_key_group[11]->bkctbl[3]=NAVY;
			screen_key_group[10]->bkctbl[2]=LIGHTBLUE;
			screen_key_group[10]->bkctbl[3]=LIGHTBLUE;
			screen_key_group[9]->bkctbl[2]=LIGHTBLUE;
			screen_key_group[9]->bkctbl[3]=LIGHTBLUE;
			btn_draw(screen_key_group[9]);
			btn_draw(screen_key_group[10]);
			btn_draw(screen_key_group[11]);
		  break;
		  case 12:
		  	currentMenu = tool_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;		
	}


}
/*装卸耗材*/
void filament_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 8; 
	char TempBuffer[32];
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;
		display_next_update_millis = millis() + 500;
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		
		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(16,22,64,48,0,5);
			screen_key_group[1]=btn_creat(96,22,130,48,1,4);
			screen_key_group[2]=btn_creat(241,22,64,48,2,5);
			
			screen_key_group[3]=btn_creat(16,92,64,48,3,5);			
			screen_key_group[4]=btn_creat(96,92,130,48,4,4);
			screen_key_group[5]=btn_creat(241,92,64,48,5,5);	
			
			screen_key_group[6]=btn_creat(132,162,56,55,6,5);
			screen_key_group[7]=btn_creat(260,180,56,55,10,5);

			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=GRAY;
				screen_key_group[i]->bcfucolor = BLACK;
				screen_key_group[i]->bcfdcolor = BLACK;
			}
			
			screen_key_group[0]->bkctbl[2]=CYAN;
			screen_key_group[0]->bkctbl[3]=CYAN;
			screen_key_group[0]->picbtnpathu = (u8*)PATH_E1D;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_E1D;

			screen_key_group[1]->bkctbl[0]=LGRAY;
		
			screen_key_group[2]->bkctbl[2]=BLUE;
			screen_key_group[2]->bkctbl[3]=BLUE;
			screen_key_group[2]->picbtnpathu = (u8*)PATH_E1U;
			screen_key_group[2]->picbtnpathd = (u8*)PATH_E1U;

			screen_key_group[3]->bkctbl[2]=CYAN;
			screen_key_group[3]->bkctbl[3]=CYAN;
			screen_key_group[3]->picbtnpathu = (u8*)PATH_E2D;
			screen_key_group[3]->picbtnpathd = (u8*)PATH_E2D;

			screen_key_group[4]->caption="---/---";
			screen_key_group[4]->bkctbl[0]=LGRAY;

			screen_key_group[5]->bkctbl[2]=BLUE;
			screen_key_group[5]->bkctbl[3]=BLUE;
			screen_key_group[5]->picbtnpathu = (u8*)PATH_E2U;
			screen_key_group[5]->picbtnpathd = (u8*)PATH_E2U;

			screen_key_group[6]->bkctbl[2]=RED;
			screen_key_group[6]->bkctbl[3]=RED;
			screen_key_group[6]->picbtnpathu = (u8*)PATH_JJTZ;
			screen_key_group[6]->picbtnpathd = (u8*)PATH_JJTZ;

			screen_key_group[7]->bkctbl[2]=BROWN;
			screen_key_group[7]->bkctbl[3]=BROWN;
			screen_key_group[7]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[7]->picbtnpathd = (u8*)PATH_FH;

			gui_phy.back_color=LGRAY;
			gui_fill_rectangle(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,LGRAY);
			sprintf(TempBuffer, "%d/%-d",(int)degHotend(0),(int)degTargetHotend(0));
			if(degTargetHotend(0) < HEATER_0_MINTEMP)
			{
				gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,BLACK,16,TempBuffer,1);
			}
			else
			{
				gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,RED,16,TempBuffer,1);
			}
			gui_phy.back_color=WHITE;
			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}

	}
	if(display_next_update_millis<millis())
	{
		display_next_update_millis = millis() + 500;

		gui_phy.back_color=LIGHTGRAY;	
		gui_fill_rectangle(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,LGRAY);

		sprintf(TempBuffer, "%d/%-d",(int)current_temperature[0],(int)target_temperature[0]);
		if(degTargetHotend(0) < HEATER_0_MINTEMP)
		{
			gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,BLACK,16,TempBuffer,1);
		}
		else
		{
			gui_show_strmid(screen_key_group[1]->left,screen_key_group[1]->top,screen_key_group[1]->width,screen_key_group[1]->height,RED,16,TempBuffer,1);
		}
		
	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
			//if(abs((target_temperature[0]-current_temperature[0])<5 && (current_temperature[0]>150)))
			{
				menu_action_gcode("G91");
				menu_action_gcode("G1 E5.0 F1200");
				menu_action_gcode("G90");
			}
		  break;
		  case 1:
		  	if(degTargetHotend(0)<40)
				target_temperature[0] = DEFAULT_HEATER_TEMP;
			else
				target_temperature[0] = 0;
			sprintf(TempBuffer, "M104 S%.1f",(float)target_temperature[0]);
			menu_action_gcode(TempBuffer);
		  break;
		  case 2:
		  	//if(abs((target_temperature[0]-current_temperature[0])<5 && (current_temperature[0]>150)))
			{
				menu_action_gcode("G91");
				menu_action_gcode("G1 E-5.0 F1200");
				menu_action_gcode("G90");
			}
		  break;
		  case 3:
		  	;
		  break;
		  case 4:
		  	;
		  break;
		  case 5:
		  	;
		  break;
		  case 6:
			;
		  break;		  	
		  case 7:
		  	//currentMenu = tool_screen;
			currentMenu = tempMenu;
			if(tempMenu==print_screen)
			{
				card.sdprinting = true;
				filament_lack_flg = 0;
			}
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}	
}
/*调平*/
void levelling_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2; 
	static u8 levelling_num=0;
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);

		gui_show_strmid(0,20,320,16,BLUE,16,"请在挤出头停止后，调整平台与挤出头距离",0);
		
		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(20,160,56,55,0,5);
			screen_key_group[1]=btn_creat(240,160,56,55,1,5);

			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=GRAY;
			}
			
			screen_key_group[0]->bkctbl[2]=CYAN;
			screen_key_group[0]->bkctbl[3]=CYAN;
			screen_key_group[0]->picbtnpathu = (u8*)PATH_YJT;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_YJT;

			screen_key_group[1]->bkctbl[2]=BROWN;
			screen_key_group[1]->bkctbl[3]=BROWN;
			screen_key_group[1]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[1]->picbtnpathd = (u8*)PATH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}

	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
			switch(levelling_num)
			{
				case 0:
					levelling_num = 1;
					//归零
					menu_action_gcode("G90");
					menu_action_gcode("G28");
					menu_action_gcode("G1 Z10 F1200");
					menu_action_gcode("G1 X45 F1200");
					menu_action_gcode("G1 Y30 F1200");
					menu_action_gcode("G1 Z0 F1200");
					break;
				case 1:
					levelling_num = 2;
					//归零
					menu_action_gcode("G90");
					menu_action_gcode("G1 Z10 F1200");
					menu_action_gcode("G1 Y180 F1200");
					menu_action_gcode("G1 Z0 F1200");
					break;
				case 2:
					levelling_num = 3;
					//归零
					menu_action_gcode("G90");
					menu_action_gcode("G1 Z10 F1200");
					menu_action_gcode("G1 X195 F1200");
					menu_action_gcode("G1 Z0 F1200");
					break;
				case 3:
					levelling_num = 4;
					//归零
					menu_action_gcode("G90");
					menu_action_gcode("G1 Z10 F1200");
					menu_action_gcode("G1 Y30 F1200");
					menu_action_gcode("G1 Z0 F1200");
					break;
				case 4:
					levelling_num = 0;
				//归零
				menu_action_gcode("G90");
				menu_action_gcode("G1 Z20 F1200");
				menu_action_gcode("G1 X120 F1200");
				menu_action_gcode("G1 Y105 F1200");
				PrintPoint = (u8*)WARNING_02;
				lastMenu = currentMenu;
			  	currentMenu = popout_screen;
				redraw_screen = true;
					break;
				default:
					break;
			}
			
		  break;		  	
		  case 1:
		  	levelling_num = 0;
		  	currentMenu = tool_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		//清除文字
		gui_fill_rectangle(0,20,320,16,gui_phy.back_color);
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}	
}


void fan_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 10; 
	char TempBuffer[32]={0};
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);

		//画图标
		gui_draw_single_color_icos(19,5,48,56,(u8*)PATH_FAN1,RED,WHITE);
		gui_draw_single_color_icos(19,65,48,56,(u8*)PATH_FAN2,MAGENTA,WHITE);
		gui_draw_single_color_icos(19,125,48,56,(u8*)PATH_BOARD,MAGENTA,WHITE);
		
		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(100,9,32,42,0,5);
			screen_key_group[1]=btn_creat(147,9,100,42,1,4);
			screen_key_group[2]=btn_creat(262,9,32,42,2,5);
			
			screen_key_group[3]=btn_creat(100,69,32,42,3,5);
			screen_key_group[4]=btn_creat(147,69,100,42,4,4);
			screen_key_group[5]=btn_creat(262,69,32,42,5,5);

			screen_key_group[6]=btn_creat(100,129,32,42,6,5);
			screen_key_group[7]=btn_creat(147,129,100,42,7,4);
			screen_key_group[8]=btn_creat(262,129,32,42,8,5);
			
			screen_key_group[9]=btn_creat(250,183,56,55,9,5);
			
			for(i=0;i<num;i++)
			{
				screen_key_group[i]->bkctbl[0]=gui_phy.back_color;
				screen_key_group[i]->bkctbl[1]=GRAY;
				screen_key_group[i]->bcfucolor = BLACK;
				screen_key_group[i]->bcfdcolor = BLACK;
			}
			for(i=0;i<3;i++)
			{
				screen_key_group[i*3]->bkctbl[2]=GREEN;
				screen_key_group[i*3]->bkctbl[3]=GREEN;
				screen_key_group[i*3]->picbtnpathu = (u8*)PATH_ZFX;
				screen_key_group[i*3]->picbtnpathd = (u8*)PATH_ZFX;
			}
			for(i=0;i<3;i++)
			{
				screen_key_group[2+i*3]->bkctbl[2]=BLUE;
				screen_key_group[2+i*3]->bkctbl[3]=BLUE;
				screen_key_group[2+i*3]->picbtnpathu = (u8*)PATH_YFX;
				screen_key_group[2+i*3]->picbtnpathd = (u8*)PATH_YFX;	
			}
			sprintf(TempBuffer, "%d",fanSpeed);
			screen_key_group[1]->caption=TempBuffer;
			screen_key_group[4]->caption="50";
			screen_key_group[7]->caption="100";
			
			screen_key_group[1]->bkctbl[0] = LGRAY;
			screen_key_group[4]->bkctbl[0] = LGRAY;
			screen_key_group[7]->bkctbl[0] = LGRAY;
			
			
			screen_key_group[9]->bkctbl[2]=BROWN;
			screen_key_group[9]->bkctbl[3]=BROWN;
			screen_key_group[9]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[9]->picbtnpathd = (u8*)PATH_FH;
			
			for(i=0;i<num;i++)
			{
				btn_draw(screen_key_group[i]);
			}
		}

	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
	      case 0: 
		  	if(fanSpeed>=5)
				fanSpeed -= 5;
			sprintf(TempBuffer, "%d",fanSpeed);
			screen_key_group[1]->caption=TempBuffer;
			btn_draw(screen_key_group[1]);
		  break;
		  case 1:
		  	if(fanSpeed<255)
				fanSpeed = 255;
			else
				fanSpeed = 0;
			sprintf(TempBuffer, "%d",fanSpeed);
			screen_key_group[1]->caption=TempBuffer;
			btn_draw(screen_key_group[1]);
		  break;
		  case 2:
		  	if(fanSpeed<=250)
				fanSpeed += 5;
			sprintf(TempBuffer, "%d",fanSpeed);
			screen_key_group[1]->caption=TempBuffer;
			btn_draw(screen_key_group[1]);
		  break;
		  case 3:
		  	;
		  break;
		  case 4:
		  	;
		  break;
		  case 5:
		  	;
		  break;
		  case 6:
			;
		  break;		  	
		  case 7:
			;
		  break;
		  case 8:
			;
		  break;
		  case 9:
		  	currentMenu = tool_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		//抹掉三个图标
		gui_fill_rectangle(19,27,48,56,gui_phy.back_color);
		gui_fill_rectangle(19,104,48,56,gui_phy.back_color);
		gui_fill_rectangle(19,131,48,56,gui_phy.back_color);
		display_next_update_millis=0;	
	}
}



void support_screen(void)
{
	u8 selx=0XFF;
	u8 num = 1; 
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_show_strmid(10,58,200,16,BLUE,16,"QQ群：",0);
		gui_show_strmid(10,94,200,16,BLUE,16,"电话：",0);
		gui_show_strmid(10,130,200,16,BLUE,16,"邮箱：",0);
		gui_show_strmid(10,166,200,16,BLUE,16,"官网：",0);

		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{
     		screen_key_group[0]=btn_creat(250,183,56,55,9,5);
			screen_key_group[0]->bkctbl[0]=gui_phy.back_color;
			screen_key_group[0]->bkctbl[1]=GRAY;
			screen_key_group[0]->bkctbl[2]=BROWN;
			screen_key_group[0]->bkctbl[3]=BROWN;
			screen_key_group[0]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_FH;			
			btn_draw(screen_key_group[0]);	
		}
	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{	
		  case 0: 
			currentMenu = tool_screen;
			redraw_screen=true;
		  break;
		}
	}
	if(redraw_screen)
	{
		btn_delete(screen_key_group[0]);
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}
}

void status_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1; 
	char TempBuffer[32];
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;
		//display_next_update_millis=0;
		display_next_update_millis=millis()+1000;
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);

		//画图标 和后面的文字
		gui_draw_single_color_icos(34,24,64,30,(u8*)PATH_XMM,GREEN,WHITE);
		sprintf(TempBuffer,"%-8.3f",current_position[X_AXIS]);
		//gui_show_string((u8*)TempBuffer,100,38,40,16,16,BLACK);
		//gui_show_strmid(100,38,60,16,BLACK,16,(u8*)TempBuffer,0);
		gui_show_ptstr(100,38,160,54,0,BLACK,16,(u8*)TempBuffer,0);
		gui_draw_single_color_icos(194,24,64,30,(u8*)PATH_YMM,GREEN,WHITE);
		sprintf(TempBuffer,"%-8.3f",current_position[Y_AXIS]);
		//gui_show_string((u8*)TempBuffer,260,38,40,16,16,BLACK);
		//gui_show_strmid(260,38,60,16,BLACK,16,(u8*)TempBuffer,0);
		gui_show_ptstr(260,38,320,54,0,BLACK,16,(u8*)TempBuffer,0);
		gui_draw_single_color_icos(34,67,64,30,(u8*)PATH_ZMM,GREEN,WHITE);
		sprintf(TempBuffer,"%-8.3f",current_position[Z_AXIS]);
		//gui_show_string((u8*)TempBuffer,100,81,40,16,16,BLACK);
		//gui_show_strmid(100,81,60,16,BLACK,16,(u8*)TempBuffer,0);
		gui_show_ptstr(100,81,160,97,0,BLACK,16,(u8*)TempBuffer,0);

		gui_draw_single_color_icos(34,110,64,50,(u8*)PATH_EX1,RED,WHITE);
		sprintf(TempBuffer, "%3.0f/%-3.0f",degHotend(0),degTargetHotend(0));
		//gui_show_string((u8*)TempBuffer,100,124,40,16,16,BLACK);
		//gui_show_strmid(100,124,60,16,BLACK,16,(u8*)TempBuffer,0);
		gui_show_ptstr(100,124,160,140,0,BLACK,16,(u8*)TempBuffer,0);
		gui_draw_single_color_icos(194,110,64,50,(u8*)PATH_EX2,RED,WHITE);
		//gui_show_string("--/--",260,124,40,16,16,BLACK);
		gui_show_strmid(260,124,60,16,BLACK,16,"--/--",0);
		gui_draw_single_color_icos(34,165,64,50,(u8*)PATH_FIRE,RED,WHITE);
		sprintf(TempBuffer, "%3.0f/%-3.0f",degBed(),degTargetBed());
		//gui_show_string((u8*)TempBuffer,100,179,40,16,16,BLACK);
		//gui_show_strmid(100,179,60,16,BLACK,16,(u8*)TempBuffer,0);
		gui_show_ptstr(100,199,160,215,0,BLACK,16,(u8*)TempBuffer,0);

		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{			
			screen_key_group[0]=btn_creat(250,183,56,55,0,5);
			screen_key_group[0]->bkctbl[0]=gui_phy.back_color;
			screen_key_group[0]->bkctbl[1]=GRAY;
			screen_key_group[0]->bkctbl[2]=BROWN;
			screen_key_group[0]->bkctbl[3]=BROWN;
			screen_key_group[0]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_FH;

			btn_draw(screen_key_group[0]);
		}
	}
#if 1
	//刷新数据
	if(display_next_update_millis<millis())
	{
		display_next_update_millis += 1000;

		sprintf(TempBuffer,"%-8.3f",current_position[X_AXIS]);
		gui_show_ptstr(100,38,160,54,0,BLACK,16,(u8*)TempBuffer,0);
		sprintf(TempBuffer,"%-8.3f",current_position[Y_AXIS]);
		gui_show_ptstr(260,38,320,54,0,BLACK,16,(u8*)TempBuffer,0);
		sprintf(TempBuffer,"%-8.3f",current_position[Z_AXIS]);
		gui_show_ptstr(100,81,160,97,0,BLACK,16,(u8*)TempBuffer,0);

		//sprintf(TempBuffer, "%d/%-d",(int)degHotend(0),(int)degTargetHotend(0));
		sprintf(TempBuffer, "%3.0f/%-3.0f",degHotend(0),degTargetHotend(0));
		gui_show_ptstr(100,124,160,140,0,BLACK,16,(u8*)TempBuffer,0);
		//sprintf(TempBuffer, "%d/%-d",(int)degBed(),(int)degTargetBed());
		sprintf(TempBuffer, "%3.0f/%-3.0f",degBed(),degTargetBed());
		gui_show_ptstr(100,199,160,215,0,BLACK,16,(u8*)TempBuffer,0);
	}
#endif
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	currentMenu = sys_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}
}

void about_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 1; 
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);

		//画图标 和后面的文字
		gui_draw_single_color_icos(55,10,32,32,(u8*)PATH_PP,CYAN,WHITE);
		gui_show_string("   深马3D",87,26,100,16,16,GREEN);
		gui_draw_single_color_icos(55,47,32,32,(u8*)PATH_ID,GREEN,WHITE);
		gui_show_string("   asflsdafj",87,63,100,16,16,BLACK);
		gui_draw_single_color_icos(55,84,32,32,(u8*)PATH_BB,YELLOW,WHITE);
		gui_show_string("   V0.1",87,100,100,16,16,BLACK);

		gui_draw_single_color_icos(55,121,32,32,(u8*)PATH_UI,RED,WHITE);
		gui_show_string("   深马UI",87,137,100,16,16,BLUE);

		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{			
			screen_key_group[0]=btn_creat(250,183,56,55,0,5);
			screen_key_group[0]->bkctbl[0]=gui_phy.back_color;
			screen_key_group[0]->bkctbl[1]=GRAY;
			screen_key_group[0]->bkctbl[2]=BROWN;
			screen_key_group[0]->bkctbl[3]=BROWN;
			screen_key_group[0]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_FH;

			btn_draw(screen_key_group[0]);
		}
	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
		  	currentMenu = sys_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}
}

void wifi_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2; 
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)				
	{
		redraw_screen=false;					
		gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
		gui_draw_rectangle(0,0,lcddev.width,lcddev.height,WHITE);

		//画图标 和后面的文字
		gui_draw_single_color_icos(55,10,48,42,(u8*)PATH_IP,CYAN,WHITE);
		gui_show_string("____.____.____.____",106,36,200,16,16,BLACK);
		gui_draw_single_color_icos(55,57,48,42,(u8*)PATH_WIFIMZ,GREEN,WHITE);
		gui_show_string("---------",106,83,200,16,16,BLACK);
		gui_draw_single_color_icos(55,103,48,42,(u8*)PATH_MIMA,YELLOW,WHITE);
		gui_show_string("---------",106,129,200,16,16,BLACK);
		gui_draw_single_color_icos(55,150,48,42,(u8*)PATH_WIFIXH,RED,WHITE);
		gui_show_string("---------",96,176,200,16,16,BLACK);
		

		screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
		if(screen_key_group)
		{			
			screen_key_group[0]=btn_creat(116,183,88,55,0,5);
			screen_key_group[0]->bkctbl[0]=gui_phy.back_color;
			screen_key_group[0]->bkctbl[1]=GRAY;
			screen_key_group[0]->bkctbl[2]=BROWN;
			screen_key_group[0]->bkctbl[3]=LGRAY;
			screen_key_group[0]->picbtnpathu = (u8*)PATH_WIFILJ;
			screen_key_group[0]->picbtnpathd = (u8*)PATH_WIFIDK;

			screen_key_group[1]=btn_creat(250,183,56,55,0,5);
			screen_key_group[1]->bkctbl[0]=gui_phy.back_color;
			screen_key_group[1]->bkctbl[1]=GRAY;
			screen_key_group[1]->bkctbl[2]=BROWN;
			screen_key_group[1]->bkctbl[3]=BROWN;
			screen_key_group[1]->picbtnpathu = (u8*)PATH_FH;
			screen_key_group[1]->picbtnpathd = (u8*)PATH_FH;
			
			btn_draw(screen_key_group[0]);
			btn_draw(screen_key_group[1]);
		}
	}
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{
		switch(selx & ~(3<<6))
		{   
		  case 0:
				gui_show_strmid(180,200,64,40,RED,16,(u8*)"使能wifi",0);
		  break;
		  case 1:
		  	currentMenu = sys_screen;
			redraw_screen=true;
		  break;
		  default: break;
		}
	}

	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}
}

void popout_screen(void)
{
	u8 selx=0XFF;
	u8 i;
	u8 num = 2; 
	gui_phy.back_color=WHITE;	
	if(redraw_screen==true)
	{ 
		redraw_screen=false;
		gui_draw_arcrectangle(75,80,180,40,6,1,GRAYBLUE,GRAYBLUE);
		gui_fill_rectangle(75,110,180,70,WHITE );
		gui_draw_rectangle(75,110,180,70,GRAYBLUE);
		gui_show_strmid(75,80,180,30,BLACK,16,(u8*)PrintPoint,1);	
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
	}	
	selx=screen_key_chk(screen_key_group,&in_obj,num);
	if(selx&(1<<6))
	{	
		switch(selx & ~(3<<6))//
		{   
			case 0:	
				
				if(tempMenu==NULL)
				{
					currentMenu = lastMenu;
				}
				else
				{
					currentMenu = tempMenu;
					tempMenu = NULL;
				}				
			break;
			case 1:
				currentMenu = lastMenu;
				redraw_screen=true;
			default: break;
		}
	}
	if(redraw_screen)
	{
		for(i=0;i<num;i++)
		{	
			btn_delete(screen_key_group[i]);
		}
		gui_memin_free(screen_key_group);
		display_next_update_millis=0;	
	}
}
#define UPDATEADDR 0x08010000
void default_screen(void)
{
	u32 temp_u32 = 0x1234;
	gui_fill_rectangle(0,0,lcddev.width,lcddev.height,WHITE);
	//置位更新标志
	STMFLASH_Write(UPDATEADDR,&temp_u32,1);
	//软复位
	NVIC_SystemReset();
}

#ifdef REPRINTSUPPORT
void reprint_screen(void)
{
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
	while(1)
	{
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
					NeedRePrintFlg = true;			
				break;
				case 1:
					NeedRePrintFlg = false;
					AT24CXX_WriteOneByte(E_SDPRINT,false);
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

#endif


