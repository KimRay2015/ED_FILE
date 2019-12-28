/*
*Function:
*Programed by:
*Complete date:
*Modified by:
*Modified date:
*Version:
*Remarks:
*/

#include "sm_plus.h"
#include "bsp.h"
#include "App_Language.h"
#include "rng.h"
#include "App_Timer.h"

int test_Num;
FIL logtext;


PRINTINFO PrintInfo;
u32 ReadChar = 0;
u32 CPUID[3];
u32 sm_version = 100;
u32 wifi_version = 100;


float x[3] = {40.0, 160.0, 100.0};
float y[3] = {20.0, 20.0, 140.0};
//float z[3]={0.0,3.3,2.3};
float z[3] = {0.0, 0.0, 0.0};

static bool extruder_temp_stable_flag[2] = {false, false}; //挤出头温度稳定
static bool bed_temp_stable_flag = false;                //热床温度稳定

//u8 INVERT_X_DIR=false;
u8 INVERT_X_DIR = true;
u8 INVERT_Y_DIR = true;
u8 INVERT_Z_DIR = true;
u8 INVERT_E0_DIR = false;
u8 INVERT_E1_DIR = false;
u8 INVERT_E2_DIR = false;

u8 X_ENDSTOPS_INVERTING = 1;
u8 Y_ENDSTOPS_INVERTING = 1;
u8 Z_ENDSTOPS_INVERTING = 1;
u8 U_ENDSTOPS_INVERTING = 1;
u8 V_ENDSTOPS_INVERTING = 0;
u8 W_ENDSTOPS_INVERTING = 1;

float ParaVersion = 0.01;	//配置文件版本

long e_steps_count = 0;
long z_setps_count = 0;
extern volatile unsigned char block_buffer_head;           // Index of the next block to be pushed
extern volatile unsigned char block_buffer_tail;           // Index of the block to process now
extern block_t *current_block;

//===================ADD BY KIM=============================
extern uint32_t CurrentFileSize,CurrentReadSize;
//==========================================================


/*
*从sd卡更新图标或者字库到spiflash中
*/
void update_icon(void)
{
	//1 打开sd卡icon文件夹
	u8 res;
	u8 *fn;   /* This function is assuming non-Unicode cfg. */
	u8 *namebuf = NULL;
	//	u16 filesize;
	FILINFO *pfno;
	#if _USE_LFN
	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN, fileinfo.lfsize);
	#endif
	namebuf = (u8 *)mymalloc(SRAMIN, 128);
	pfno = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));
	if(namebuf == NULL || pfno == NULL) {
		myfree(SRAMIN, fileinfo.lfname);
		myfree(SRAMIN, namebuf);
		myfree(SRAMIN, pfno);
		return;
	}
	res = f_opendir(&dir, "0:\\icon");
	if(res)
		return;
	res = f_stat("1:icon", pfno);
	if(res) {
		res = f_mkdir("1:icon");
	}

	if(res == FR_OK) {

		while(1) {
			res = f_readdir(&dir, &fileinfo);
			if(res != FR_OK || fileinfo.fname[0] == 0) break;
			#if _USE_LFN
			fn = (u8 *)(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
			#else
			fn = (u8 *)(fileinfo.fname);
			#endif
			//	printf("%s/", path);
			res = f_typetell(fn);
			if((res & 0XF0) == 0) {	//文件类型为bin
				printf("%s\n",  fn);
				//复制文件到spiflash
				sprintf(namebuf, "0:/icon/%s", fn);
				res = f_open(file, namebuf, FA_READ | FA_OPEN_EXISTING);
				if(res)
					return;
				memset(namebuf, 0, sizeof(namebuf));

				sprintf(namebuf, "1:/icon/%s", fn);
				res = f_open(ftemp, namebuf, FA_CREATE_ALWAYS | FA_WRITE);
				if(res)
					return;
				while(res == 0) {
					memset(namebuf, 0, sizeof(namebuf));
					res = f_read(file, namebuf, 128, &br);
					if(res || br == 0)
						break;
					res = f_write(ftemp, namebuf, br, &bw);
					if(res || bw == 0)
						break;
				}
				f_close(file);
				f_close(ftemp);

			}
		}
	}
	//读取spiflash中的文件
	res = f_opendir(&dir, "1:\\icon");
	if(res)
		return;
	while(1) {
		res = f_readdir(&dir, &fileinfo);
		if(res != FR_OK || fileinfo.fname[0] == 0) break;
		#if _USE_LFN
		fn = (u8 *)(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
		#else
		fn = (u8 *)(fileinfo.fname);
		#endif
		//	printf("%s/", path);
		res = f_typetell(fn);
		if((res & 0XF0) == 0) {	//文件类型为bin
			USR_UsrLog("%s size=%d\r\n", fn, fileinfo.fsize);

		}
	}
	myfree(SRAMIN, fileinfo.lfname);
	myfree(SRAMIN, namebuf);
	myfree(SRAMIN, pfno);
	//2 遍历所有文件并复制到spiflash中去


}


void qlzdecompressed(u8 *name)
{
	FRESULT res;
	u8 *file_data = NULL, *decompressed = NULL;
	char *point;
	u32 i, d, c;
	qlz_state_decompress *state_decompress = (qlz_state_decompress *)mymalloc(SRAMIN, sizeof(qlz_state_decompress));
	file_data = (u8 *)mymalloc(SRAMIN, 10240 + 400);
	decompressed = (u8 *)mymalloc(SRAMIN, 10240);
	if(state_decompress == NULL || file_data == NULL || decompressed == NULL)
		goto exit1;
	memset(state_decompress, 0, sizeof(qlz_state_decompress));
	//1.打开文件
	memset(pname, 0, sizeof(pname));
	strcpy((char *)pname, "0:/GCODE/");
//		strcpy((char *)pname, "0:");
	strcat((char *)pname, (const char *)name);
	res = f_open(file, pname, FA_READ | FA_OPEN_EXISTING);
	if(res)
		goto exit1;
	point = strchr(pname, '.');
	strcpy(point, ".gcode");
	res = f_open(ftemp, pname, FA_CREATE_ALWAYS | FA_WRITE);
	if(res)
		goto exit1;
	if(f_read(file, file_data, 9, &br))
		goto exit1;
	while(br != 0) {
		c = qlz_size_compressed(file_data);
		if(f_read(file, file_data + 9, c - 9, &i))
			goto exit1;
		d = qlz_decompress(file_data, decompressed, state_decompress);
		printf("%u bytes decompressed into %u\n", (u32)c, (u32)d);
		if(f_write(ftemp, decompressed, d, &bw))
			goto exit1;
		if(f_read(file, file_data, 9, &br))
			//goto exit1;
			;

	}
	f_close(file);
	f_close(ftemp);
	//选中解压后的文件
	memset(fatbuf, 0, sizeof(fatbuf));
	sprintf(fatbuf, "M23 %s", pname);
	enquecommand(fatbuf);

exit1:
	myfree(SRAMIN, state_decompress);
	myfree(SRAMIN, file_data);
	myfree(SRAMIN, decompressed);
	return;

}

void qlzcompress(u8 *path)
{
	FRESULT res;
	u8 *file_data = NULL, *compressed = NULL;
	char *point;
	u32 d, c;
	qlz_state_compress *state_compress = (qlz_state_compress *)mymalloc(SRAMIN, sizeof(qlz_state_compress));
	file_data = (u8 *)mymalloc(SRAMIN, 10000);
	compressed = (u8 *)mymalloc(SRAMIN, 10000 + 400);
	if(state_compress == NULL || file_data == NULL || compressed == NULL)
		goto exit1;
	memset(state_compress, 0, sizeof(qlz_state_decompress));
	//1.打开文件
	memset(pname, 0, sizeof(pname));
	strcpy((char *)pname, "0:/GCODE/");
//		strcpy((char *)pname, "0:");
	strcat((char *)pname, (const char *)path);
	res = f_open(file, pname, FA_READ | FA_OPEN_EXISTING);
	if(res)
		goto exit1;
	point = strchr(pname, '.');
	strcpy(point, ".lz");

	res = f_open(ftemp, pname, FA_CREATE_ALWAYS | FA_WRITE);
	if(res)
		goto exit1;
	if(f_read(file, file_data, rand() % 10000 + 1, &d))
		goto exit1;
	while(d != 0) 
	{
		c = qlz_compress(file_data, compressed, d, state_compress);
		USR_UsrLog("%u bytes compressed into %u\n", (unsigned int)d, (unsigned int)c);

		if(f_write(ftemp, compressed, c, &bw))
			//goto exit1;
			;
		if(f_read(file, file_data, rand() % 10000 + 1, &d))
			goto exit1;

	}
	f_close(file);
	f_close(ftemp);

exit1:
	myfree(SRAMIN, state_compress);
	myfree(SRAMIN, file_data);
	myfree(SRAMIN, compressed);
	return;
}




/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
u8 sendFlg = 0;			//主动发送打印状态命令
u8 oldsdprint = 0;
u8 BoardCheckFlg = 0;		//执行底板检测命令标志   0未执行 1正在执行 2执行完成
u8 FilamentLoadFlg = 0;	//装料命令标志位0未执行 1正在执行
u8 FilamentUnloadFlg = 0;	//卸料命令标志位0未执行 1正在执行
u8 old_block_buffer_head;
static u8 M4009per = 0;
extern USR_TIMER usr_timer;
void printinfo_update(void)
{
	u8 per;
	u32 time_remain;
	float persent_f;
	static u8 oldper = 0;
	float print_percent = 0;

	//计算打印百分比
	//per = (ReadChar * 100) / PrintInfo.filesize;					//Read Data With f_read
	if(usr_timer.sec % 30 == 0)															//every 30 sec check once print percent
	{
		CurrentReadSize = f_tell(&card.fgcode);
		//per = (f_tell(&card.fgcode)*100)/PrintInfo.filesize;					//Read Data With f_gets
		print_percent = ((float)CurrentReadSize/ PrintInfo.filesize)*100;					//Read Data With f_gets
		PrintInfo.printper = (uint8_t)print_percent;
		sendFlg = 1;
		oldper = per;
	}
	//更新打印时间
	if(PrintInfo.printsd == TRUE) 
	{
		PrintInfo.printtime = (u32)((millis() - starttime) / 1000);
	}
	if(sendFlg) 
	{
		sendFlg = 0;
		memset(fatbuf, 0, sizeof(fatbuf));
		sprintf(fatbuf, "UNSOL:M4001:%01d/%s/%02d/%d/%d\r\n", \
		PrintInfo.printsd, PrintInfo.printfile, PrintInfo.printper, PrintInfo.filesize, PrintInfo.printtime);
		usart2_send(fatbuf);
		usart1_send(fatbuf);
		//计算剩余时间
		if(ReadChar) 
		{
			time_remain = PrintInfo.printtime * PrintInfo.filesize / ReadChar;	//打印总时长
			time_remain -= PrintInfo.printtime;								//总时长 - 已用时长
		} 
		else
		{
			time_remain = 0;
		}
		USR_DbgLog("time_remain = %d\r\n", time_remain);
	}

	if(FilamentLoadFlg && current_block != NULL) 
	{
		persent_f = (float)e_steps_count / (float)current_block->step_event_count;
		persent_f *= 100;
		per = (u8)persent_f;
		if(per > M4009per && (per % 10 == 0)) 
		{
			M4009per = per;
			memset(fatbuf, 0, sizeof(fatbuf));
			sprintf(fatbuf, "UNSOL:M4010:%d\r\n", M4009per);
			usart2_send(fatbuf);
		}
		if(per >= 98) 
		{
			FilamentLoadFlg = 0;
			M4009per = 0;
			usart2_send("UNSOL:M4010:100\r\n");
		}
	}

	if(FilamentUnloadFlg && current_block != NULL) 
	{
		persent_f = (float)e_steps_count / (float)current_block->step_event_count;
		persent_f *= 100;
		per = (u8)persent_f;
		if(per > M4009per && (per % 10 == 0)) 
		{
			M4009per = per;
			memset(fatbuf, 0, sizeof(fatbuf));
			sprintf(fatbuf, "UNSOL:M4009:%d\r\n", M4009per);
			usart2_send(fatbuf);
		}
		if(per >= 99) 
		{
			FilamentUnloadFlg = 0;
			M4009per = 0;
			usart2_send("UNSOL:M4009:100\r\n");
		}
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:System Power Up Printing Infor Init
*/
void printinfo_init(void)
{
	PrintInfo.filesize = 0;
	strcpy(PrintInfo.printfile, "null");
	PrintInfo.printper = 0;
	PrintInfo.printsd = 0;
	PrintInfo.printtime = 0;
}



void qrencode_test(u8 *str)
{
	int x, y;

	QRcode *qrcode; //最后结果

	qrcode = QRcode_encodeString(str, 2, QR_ECLEVEL_L, QR_MODE_8, 0);
	USR_UsrLog("====version=%d========\r\n", qrcode->version);
	for(y = 0; y < qrcode->width; y++) 
{
		for(x = 0; x < qrcode->width; x++) 
		{
			if(qrcode->data[y * qrcode->width + x] & 0x01) 
			{
				printf("#");
				LCD_Fast_DrawPoint(x, y, BLACK);
				// LCM_SetPixel(x,y,1);
			} 
			else 
			{
				printf(" ");
				LCD_Fast_DrawPoint(x, y, WHITE);
				// LCM_SetPixel(x,y,0);
			}
		}
		USR_UsrLog("Test...");
	}
	USR_UsrLog("Test...");

	//DISPLAY_RENCODE_TO_TFT(0,0,2);
	QRcode_free(qrcode);

}
//显示几个固定尺寸二维码
/*
string:需要转成二维码信息的字符串
sx sy:二维码显示的起始位置
mode:显示的固定尺寸 0 原始大小 1 扩大14倍
*/
#define MAG_TIME 4
void qrencode_display(const u8 *string, u16 sx, u16 sy, u8 mode)
{
	int x, y;

	QRcode *qrcode; //最后结果

	qrcode = QRcode_encodeString(string, 2, QR_ECLEVEL_L, QR_MODE_8, 0);
	printf("====version=%d========\r\n", qrcode->version);
	for(y = 0; y < qrcode->width; y++) {
		for(x = 0; x < qrcode->width; x++) {
			if(qrcode->data[y * qrcode->width + x] & 0x01) {
				if(mode) {	//放大模式
					gui_fill_rectangle(sx + x * MAG_TIME, sy + y * MAG_TIME, MAG_TIME, MAG_TIME, BLACK);
				} else
					LCD_Fast_DrawPoint(x + sx, y + sy, BLACK);
			} else {
				if(mode) {	//放大模式
					gui_fill_rectangle(sx + x * MAG_TIME, sy + y * MAG_TIME, MAG_TIME, MAG_TIME, WHITE);
				} else
					LCD_Fast_DrawPoint(x + sx, y + sy, BLACK);
			}
		}
	}
	QRcode_free(qrcode);
}


//#if define AUTO_LEVELING
extern volatile long count_position[NUM_AXIS];
extern volatile long endstops_trigsteps[3];

void bl_touch_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef BL_TOUCH_TIM3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);		//BL_TOUCH
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseStructure.TIM_Period = 199;
	TIM_TimeBaseStructure.TIM_Prescaler = 8399;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);


	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = BL_UP;//CCR
	TIM_OCInitStructure.TIM_OCPolarity	 = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE); //ARPE 使能

	TIM_Cmd(TIM3, ENABLE);  //使能TIM1
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	#endif
	//输入引脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM8);		//BL_TOUCH
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_DeInit(TIM8);

	//2.7K
	TIM_TimeBaseStructure.TIM_Period = 199;
	TIM_TimeBaseStructure.TIM_Prescaler = 16799;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	/* Output Compare Active Mode configuration: Channel4 */

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;

	TIM_OCInitStructure.TIM_Pulse = BL_UP;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//输出同相，
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Reset;
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM8, ENABLE);
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
}



float get_middle(float a, float b, float c)
{
	if((b - a) * (a - c) >= 0.0) 
	{
		return a;
	} 
	else if((a - b) * (b - c) >= 0.0) 
	{
		return b;
	} 
	else 
	{
		return c;
	}
}

void copyfiles(void)
{
	u8 res;
	u8 *fn;   /* This function is assuming non-Unicode cfg. */
	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN, fileinfo.lfsize);
	DIR dir_des;
	res = f_opendir(&dir, "0:\\child_icon");
	if(res == FR_OK) 
	{
		//打开或创建目标文件夹
		res = f_opendir(&dir_des, "1:\\child_icon");
		if(res) 
		{
			printf("error res=%d\r\n", res);
			res = f_mkdir("1:\\child_icon");
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}
		}
		res = f_closedir(&dir_des);
		while(1) 
		{
			res = f_readdir(&dir, &fileinfo);
			if(res != FR_OK || fileinfo.fname[0] == 0) break;
			fn = (u8 *)(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);

			memset(pname, 0, sizeof(pname));
			sprintf(pname, "0:/child_icon/%s", fn);
			//从SD卡复制到SPI_FLASH

			res = f_open(file, pname, FA_READ | FA_OPEN_EXISTING);
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}

			memset(pname, 0, sizeof(pname));
			sprintf(pname, "1:/child_icon/%s", fn);
			res = f_open(ftemp, pname, FA_WRITE | FA_CREATE_ALWAYS);
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}
			while(res == 0) 
			{
				res = f_read(file, fatbuf, 512, (UINT *)&br); //源头读出512字节
				if(res || br == 0)
					break;
				res = f_write(ftemp, fatbuf, (UINT)br, &bw);
				if(res || bw < br)
					break;
			}
			//打印spi_flash中的文件信息
			printf("file name=%s\r\n", pname);
			printf("file size=%d\r\n", file->fsize);

			f_close(file);
			f_close(ftemp);
		}
		myfree(SRAMIN, fileinfo.lfname);
	}
}
/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:copy icons from TF to spi flash
*/
void copy_diy_icons(void)
{
	u8 res;
	u8 *fn;   /* This function is assuming non-Unicode cfg. */
	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN, fileinfo.lfsize);
	DIR dir_des;
	res = f_opendir(&dir, "0:\\diy_icon");
	if(res == FR_OK) 
	{
		//打开或创建目标文件夹
		res = f_opendir(&dir_des, "1:\\diy_icon");
		if(res) 
		{
			printf("error res=%d\r\n", res);
			res = f_mkdir("1:\\diy_icon");
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}
		}
		res = f_closedir(&dir_des);
		while(1) 
		{
			res = f_readdir(&dir, &fileinfo);
			if(res != FR_OK || fileinfo.fname[0] == 0) break;
			fn = (u8 *)(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);

			memset(pname, 0, sizeof(pname));
			sprintf(pname, "0:/diy_icon/%s", fn);
			//从SD卡复制到SPI_FLASH

			res = f_open(file, pname, FA_READ | FA_OPEN_EXISTING);
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}

			memset(pname, 0, sizeof(pname));
			sprintf(pname, "1:/diy_icon/%s", fn);
			res = f_open(ftemp, pname, FA_WRITE | FA_CREATE_ALWAYS);
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}
			while(res == 0) 
			{
				res = f_read(file, fatbuf, 512, (UINT *)&br); //源头读出512字节
				if(res || br == 0)
					break;
				res = f_write(ftemp, fatbuf, (UINT)br, &bw);
				if(res || bw < br)
					break;
			}
			//打印spi_flash中的文件信息
			printf("file name=%s\r\n", pname);
			printf("file size=%d\r\n", file->fsize);

			f_close(file);
			f_close(ftemp);
		}
		myfree(SRAMIN, fileinfo.lfname);
	}
}

void copy_font(void)
{
	u8 res;
	u8 *fn;   /* This function is assuming non-Unicode cfg. */
	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN, fileinfo.lfsize);
	res = f_opendir(&dir, "0:/SYSTEM/FONT");
	if(res == FR_OK) {
		PRINTFLINE;
		while(1) {
			res = f_readdir(&dir, &fileinfo);
			if(res != FR_OK || fileinfo.fname[0] == 0) break;
			fn = (u8 *)(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);

			memset(pname, 0, sizeof(pname));
			sprintf(pname, "0:/SYSTEM/FONT/%s", fn);
			//从SD卡复制到SPI_FLASH
			res = f_open(file, pname, FA_READ | FA_OPEN_EXISTING);
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}
			memset(pname, 0, sizeof(pname));
			sprintf(pname, "1:/%s", fn);
			res = f_open(ftemp, pname, FA_WRITE | FA_CREATE_ALWAYS);
			if(res) 
			{
				myfree(SRAMIN, fileinfo.lfname);
				PRINTFLINE;
				return;
			}
			while(res == 0)
			{
				res = f_read(file, fatbuf, 512, (UINT *)&br); //源头读出512字节
				if(res || br == 0)
					break;
				res = f_write(ftemp, fatbuf, (UINT)br, &bw);
				if(res || bw < br)
					break;
			}
			//打印spi_flash中的文件信息
			printf("file name=%s\r\n", pname);
			printf("file size=%d\r\n", file->fsize);

			f_close(file);
			f_close(ftemp);
		}
		myfree(SRAMIN, fileinfo.lfname);
	}
}
void ls_spi_icon(void)
{
	u8 res;
	u8 *fn;   /* This function is assuming non-Unicode cfg. */
	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN, fileinfo.lfsize);

	res = f_opendir(&dir, "1:\\child_icon");
	if(res) 
	{
		myfree(SRAMIN, fileinfo.lfname);
		PRINTFLINE;
		return;
	}
	while(1) 
	{
		res = f_readdir(&dir, &fileinfo);
		if(res != FR_OK || fileinfo.fname[0] == 0) break;
		fn = (u8 *)(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
		printf("%s\n",	fn);
		printf("%d\n", fileinfo.fsize);
	}
	myfree(SRAMIN, fileinfo.lfname);

}

void ls_spi_diy_icon(void)
{
	u8 res;
	u8 *fn;   /* This function is assuming non-Unicode cfg. */
	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN, fileinfo.lfsize);

	res = f_opendir(&dir, "1:\\diy_icon");
	if(res) 
	{
		myfree(SRAMIN, fileinfo.lfname);
		PRINTFLINE;
		return;
	}
	while(1) 
	{
		res = f_readdir(&dir, &fileinfo);
		if(res != FR_OK || fileinfo.fname[0] == 0) break;
		fn = (u8 *)(*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
		printf("%s\n",	fn);
		printf("%d\n", fileinfo.fsize);
	}
	myfree(SRAMIN, fileinfo.lfname);

}


void filament_lack(void)
{
	//1、停止打印
	card.sdprinting = false;
	//2、xy轴归零
	enquecommand("G28 XY");
	//3、跳转到更换耗材界面
	tempMenu = print_menu;
	CurrentMenu = filamentstep1_menu;
	redraw_menu = true;
}

/*sd 卡检测*/
void sd_check(void)
{
	if(card.cardOK == true && SD_CD == 1) 
	{	//卡被拔出
		card_release();
		USR_UsrLog("SD card pull out\r\n");
	}
	if(card.cardOK == false && SD_CD == 0) 
	{
		card_initsd();
		USR_UsrLog("SD card insert\r\n");
	}
}

void laser_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
}

u8 i_buff = 0, show_delay_time = 0;
u8 index_buff=0;	//新采样温度值存放位置
#define SampleTime 20
#define TEMP_DELAY_TIME 50                                 //减少温度跳变速度
float temperature_buff[EXTRUDERS + 1][SampleTime] = {0.0};
float show_extruder_temperature[2] = { 0 };
float show_bed_temperature = 0;

void show_temperature(void)
{
	u8 i,j;
	float temp;
	int   rng_num;
	for(i = 0; i < EXTRUDERS; i++) 
	{
		temperature_buff[i][i_buff] = current_temperature[i];
	}
	temperature_buff[EXTRUDERS][i_buff] = current_temperature_bed;
	if(i_buff < SampleTime - 1)
		i_buff++;
	else 
	{
		i_buff = 0;
		//冒泡法排序
		for(i = 0; i < SampleTime - 1; ++i) 
		{
			for(j = 0; j < SampleTime - 1 - i; ++j) 
			{
				if(temperature_buff[0][j] > temperature_buff[0][j + 1]) 
				{
					temp = temperature_buff[0][j];
					temperature_buff[0][j] = temperature_buff[0][j + 1];
					temperature_buff[0][j + 1] = temp;
				}
				
			}
		}
		show_delay_time++;                                                          //减少跳变速度
		//去除最大值和最小值再取平均
		temp = 0.0;
		temp = temperature_buff[0][1];
		temp += temperature_buff[0][2];
		temp += temperature_buff[0][3];
		temp = temp / 3;
		if(target_temperature[0] == 0) 
		{
			show_extruder_temperature[0] = temp;
			extruder_temp_stable_flag[0] = false;
		} 
		else if(show_extruder_temperature[0] > target_temperature[0]) 
		{            //数据已经平滑的达到目标温度 开始随机
			if(show_delay_time >= TEMP_DELAY_TIME) 
			{                                 //减少温度跳变速度
				extruder_temp_stable_flag[0] = true;                                 //
				rng_num = RNG_Get_RandomRange(1, 3);                                 //从1~3产生一个随机数
				switch(rng_num) 
				{
					case 1://减
						show_extruder_temperature[0]--;
						if(show_extruder_temperature[0] <= (target_temperature[0] - 1))show_extruder_temperature[0] = target_temperature[0] - 1;
						break;
					case 2://不变
						break;
					case 3://加
						show_extruder_temperature[0]++;
						if(show_extruder_temperature[0] >= (target_temperature[0] + 1))show_extruder_temperature[0] = target_temperature[0] + 1;
						break;
				}
			}
		} 
		else if(show_extruder_temperature[0] > (target_temperature[0] - 10)) 
		{     //低于目标温度10度以内
			if((extruder_temp_stable_flag[0] == true)) 
			{                             //数据已经平滑的到达目标温度
				if(show_delay_time >= TEMP_DELAY_TIME) 
				{
					rng_num = RNG_Get_RandomRange(1, 3);                             //从1~3产生一个随机数
					switch(rng_num) 
					{
						case 1://减
							show_extruder_temperature[0]--;
							if(show_extruder_temperature[0] <= (target_temperature[0] - 1))show_extruder_temperature[0] = target_temperature[0] - 1;
							break;
						case 2://不变
							break;
						case 3://加
							show_extruder_temperature[0]++;
							if(show_extruder_temperature[0] >= (target_temperature[0] + 1))show_extruder_temperature[0] = target_temperature[0] + 1;
							break;
					}
				}
			} 
			else 
			{ //数据平滑变化
				show_extruder_temperature[0] += 3;
			}
		}
		else 
		{
			show_extruder_temperature[0] = temp;
		}
        
#if EXTRUDERS > 1
		temp = 0.0;
		temp = temperature_buff[1][1];
		temp += temperature_buff[1][2];
		temp += temperature_buff[1][3];
		temp = temp / 3;
		if(target_temperature[1] == 0) 
		{
			show_extruder_temperature[1] = temp;
			extruder_temp_stable_flag[1] = false;
		} 
		else if(show_extruder_temperature[1] > target_temperature[1]) 
		{            //数据已经平滑的达到目标温度 开始随机
			if(show_delay_time >= TEMP_DELAY_TIME) 
			{
				extruder_temp_stable_flag[1] = true;                                 //
				rng_num = RNG_Get_RandomRange(1, 3);                                 //从1~3产生一个随机数
				switch(rng_num) 
				{
					case 1://减
						show_extruder_temperature[1]--;
						if(show_extruder_temperature[1] <= (target_temperature[1] - 1))show_extruder_temperature[1] = target_temperature[1] - 1;
						break;
					case 2://不变
						break;
					case 3://加
						show_extruder_temperature[1]++;
						if(show_extruder_temperature[1] >= (target_temperature[1] + 1))show_extruder_temperature[1] = target_temperature[1] + 1;
						break;
				}
			}
		} 
		else if(show_extruder_temperature[1] > (target_temperature[1] - 10)) 
		{     //低于目标温度10度以内
			if(extruder_temp_stable_flag[1] == true) 
			{                               //数据已经平滑的到达目标温度
				if(show_delay_time >= TEMP_DELAY_TIME) 
				{
					rng_num = RNG_Get_RandomRange(1, 3);                             //从1~3产生一个随机数
					switch(rng_num) 
					{
						case 1://减
							show_extruder_temperature[1]--;
							if(show_extruder_temperature[1] <= (target_temperature[1] - 1))show_extruder_temperature[1] = target_temperature[1] - 1;
							break;
						case 2://不变
							break;
						case 3://加
							show_extruder_temperature[1]++;
							if(show_extruder_temperature[1] >= (target_temperature[1] + 1))show_extruder_temperature[1] = target_temperature[1] + 1;
							break;
					}
				}
			}
			else 
			{ //数据平滑变化
				show_extruder_temperature[1] += 3;
			}
		} 
		else 
		{
			show_extruder_temperature[1] = temp;
		}
#elif (EXTRUDERS == 1)
        temp = 0.0;
        show_extruder_temperature[1] = temp;
#endif

		temp = 0.0;
		temp = temperature_buff[EXTRUDERS][1];
		temp += temperature_buff[EXTRUDERS][2];
		temp += temperature_buff[EXTRUDERS][3];
		temp = temp / 3;
		if(target_temperature_bed == 0) {
			show_bed_temperature = temp;
			bed_temp_stable_flag = false;
		} 
		else if(show_bed_temperature > target_temperature_bed) 
		{                   //数据已经平滑的达到目标温度 开始随机
			if(show_delay_time >= TEMP_DELAY_TIME) 
			{
				bed_temp_stable_flag = true;                                         //
				rng_num = RNG_Get_RandomRange(1, 3);                                 //从1~3产生一个随机数
				switch(rng_num) 
				{
					case 1://减
						show_bed_temperature--;
						if(show_bed_temperature <= (target_temperature_bed - 1))show_bed_temperature = target_temperature_bed - 1;
						break;
					case 2://不变
						break;
					case 3://加
						show_bed_temperature++;
						if(show_bed_temperature >= (target_temperature_bed + 1))show_bed_temperature = target_temperature_bed + 1;
						break;
				}
			}
		} 
		else if(show_bed_temperature > (target_temperature_bed - 10)) 
		{            //低于目标温度10度以内
			if((bed_temp_stable_flag == true)) 
			{                                     //数据已经平滑的到达目标温度
				if(show_delay_time >= TEMP_DELAY_TIME) 
				{
					rng_num = RNG_Get_RandomRange(1, 3);                             //从1~3产生一个随机数
					switch(rng_num) 
					{
						case 1://减
							show_bed_temperature--;
							if(show_bed_temperature <= (target_temperature_bed - 1))show_bed_temperature = target_temperature_bed - 1;
							break;
						case 2://不变
							break;
						case 3://加
							show_bed_temperature++;
							if(show_bed_temperature >= (target_temperature_bed + 1))show_bed_temperature = target_temperature_bed + 1;
							break;
					}
				}
			} 
			else 
			{                                                                 //数据平滑变化
				show_bed_temperature += 3;
			}
		} 
		else 
		{
			show_bed_temperature = temp;
		}

		if(show_delay_time >= TEMP_DELAY_TIME) show_delay_time = 0;
	}

}


#if 1

//读取配置文件

#define PRINTF_ERROR(Str) printf("ERROR: %s \n",Str)
#define PRINTF_STATE(Str) printf("STATE: %s \n",Str)

#define CONFIGURATION_FPATH "0:Configuration.txt"
//文件表头
#define FILE_HEADER 		"#Parameter_List\r\n"

#define STRING_LEN  255

Para_T Para[] = {
	{"axis_steps_per_unit",							4	, {0, 0, 0, 0}}, //1	移动1mm的step数
	{"max_feedrate",								4	, {0, 0, 0, 0}}, //2	可允许的最大进料速度
	{"max_acceleration_units_per_sq_second",		4	, {0, 0, 0, 0}}, //3	最大加速度
	{"acceleration",								1	, {0, 0, 0, 0}}, //4	加速度
	{"retract_acceleration",						1	, {0, 0, 0, 0}}, //5	收回加速度
	{"minimumfeedrate",								1	, {0, 0, 0, 0}}, //6	最小进料速度
	{"minsegmenttime",								1	, {0, 0, 0, 0}}, //7	如果缓冲区被清空，移动所需的最小时间(以微秒为单位)。
	{"max_xy_jerk",									1	, {0, 0, 0, 0}}, //8	可以立即停止的x,y轴速度
	{"max_z_jerk",									1	, {0, 0, 0, 0}}, //9	可以立即停止的z轴速度
	{"max_e_jerk",									1	, {0, 0, 0, 0}}, //10	可以立即停止的e轴速度
	{"kp",											1	, {0, 0, 0, 0}}, //11	PID比例参数
	{"ki",											1	, {0, 0, 0, 0}}, //12	PID积分参数
	{"kd",											1	, {0, 0, 0, 0}}, //13	PID微分参数
	{"INVERT_X_DIR",								1	, {0, 0, 0, 0}}, //14	反转X的方向？
	{"INVERT_Y_DIR",								1	, {0, 0, 0, 0}}, //15	反转Y的方向？
	{"INVERT_Z_DIR",								1	, {0, 0, 0, 0}}, //16	反转Z的方向？
	{"INVERT_E0_DIR",								1	, {0, 0, 0, 0}}, //17	反转E0的方向？
	{"INVERT_E1_DIR",								1	, {0, 0, 0, 0}}, //18	反转E1的方向？
	{"INVERT_E2_DIR",								1	, {0, 0, 0, 0}}, //19	反转E2的方向？
	{"X_ENDSTOPS_INVERTING",						1	, {0, 0, 0, 0}}, //20	结束停止X轴转向?
	{"Y_ENDSTOPS_INVERTING",						1	, {0, 0, 0, 0}}, //21	结束停止Y轴转向?
	{"Z_ENDSTOPS_INVERTING",						1	, {0, 0, 0, 0}}, //22	结束停止Z轴转向?
	{"FILAMENT_INVERTING",							1	, {0, 0, 0, 0}}, //22	结束停止Z轴转向?
	{"speed_temprature_factor",                     1	, {0, 0, 0, 0}}, //23   温度倍速关系系数
	{"Machine_Size",                     			3	, {0, 0, 0, 0}}, //24	机器打印尺寸 
	{"Para_Version",                     			1	, {0, 0, 0, 0}}, //25	配置文件版本
	//{"retract_feedrate",							1	, {0, 0, 0, 0}}, //23	缩进进料速度?
	//{"retract_zlift",								1	, {0, 0, 0, 0}}, //24	Z轴抬起？
	//{"retract_recover_length",						1	, {0, 0, 0, 0}}, //25	长度收缩恢复？
	{"speed_temprature_factor",                     1, {0, 0, 0, 0}},    //26   温度倍速关系系数
};

int Move_Fp(int Fp_Index, char *str)
{
	while((str[Fp_Index] == '\t') || (str[Fp_Index] == ' ')) {
		Fp_Index++;
	}
	//printf("File_Index = %d\n", Fp_Index);
	return Fp_Index;
}

u16 ConfigurationWrite2EEPROM(void)
{
	FIL *fp = NULL;
	char Str_Buff[255];
	char *Str_Buff1;
	int  File_Index = 0;
	int  Parameter_Num = 0, i, j, res, len;
	Parameter_Num = sizeof(Para) / sizeof(Para_T);
	printf("参数个数 = %d \n", Parameter_Num);
	fp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));
	res = f_open(fp, CONFIGURATION_FPATH, FA_READ);
	if(res != FR_OK) {
		PRINTF_ERROR("File Not Open!\n");
		return NON_CONFIGFILE;//文件不存在
	} else {
		f_gets(Str_Buff, STRING_LEN, fp);
		//printf("Str_Buff:%s\n", Str_Buff);
		printf("FILE_HEADER:%s\n", FILE_HEADER);
		if(!strcmp(FILE_HEADER, Str_Buff)) { //参数表头
			for(i = 0; i < Parameter_Num; i++) {
				File_Index = 0;
				f_gets(Str_Buff, STRING_LEN, fp);
				//printf("Str_Buff:%s",Str_Buff);
				if(!f_eof(fp)) 
				{ //非结束行
					Str_Buff1 = Str_Buff;
					len = strlen(Para[i].name);
					if(!strncmp(Para[i].name, &Str_Buff1[1], len) && Str_Buff1[File_Index] == '>') { //字符串相等 返回值0
						File_Index += 1;
						//printf("参数名: %s\n", &Str_Buff1[File_Index]);
						File_Index += len;
						File_Index = Move_Fp(File_Index, Str_Buff1);
						//找到 参数
						for(j = 0; j < Para[i].para_Num ; j++) 
						{
							Para[i].Float_Data[j] = strtod(Str_Buff1 + File_Index, &Str_Buff1);
							//printf("字符部分%s \n", Str_Buff1);
							File_Index = Move_Fp(0, Str_Buff1);
							printf("%f\n", Para[i].Float_Data[j]);
						}
						if(Str_Buff1[File_Index] == '#') 
						{
							printf("注释 : %s\n", &Str_Buff1[File_Index]);
						} 
						else 
						{
							PRINTF_ERROR("无参数\n");
						}
					} 
					else if(Str_Buff1[File_Index] == '#') 
					{ //注释行
						i -= 1;
						//printf("注释行\n");
					}
					else 
					{ //空行
						i -= 1;
						//printf("空行\n");
					}
				}
				else 
				{
					f_rename("0:Configuration.txt", "0:OldConfiguration.txt");
					return PARAM_NONALL;
				}
			}
		} 
		else 
		{
			PRINTF_ERROR("参数头获取失败,文件错误\n");
			f_rename("0:Configuration.txt", "0:OldConfiguration.txt");
			return	FILE_MISMATCH;
		}
	}
	f_rename("0:Configuration.txt", "0:OldConfiguration.txt");
	f_close(fp);


	printf("文件读取完毕\n");
	return RDCONFIG_OK;
}

void save_configfile(void)
{
	//保存参数
	axis_steps_per_unit[0] = Para[0].Float_Data[0];
	axis_steps_per_unit[1] = Para[0].Float_Data[1];
	axis_steps_per_unit[2] = Para[0].Float_Data[2];
	axis_steps_per_unit[3] = Para[0].Float_Data[3];

	max_feedrate[0] = Para[1].Float_Data[0];
	max_feedrate[1] = Para[1].Float_Data[1];
	max_feedrate[2] = Para[1].Float_Data[2];
	max_feedrate[3] = Para[1].Float_Data[3];

	max_acceleration_units_per_sq_second[0] = (unsigned long)Para[2].Float_Data[0];
	max_acceleration_units_per_sq_second[1] = (unsigned long)Para[2].Float_Data[1];
	max_acceleration_units_per_sq_second[2] = (unsigned long)Para[2].Float_Data[2];
	max_acceleration_units_per_sq_second[3] = (unsigned long)Para[2].Float_Data[3];

	acceleration = Para[3].Float_Data[0];
	retract_acceleration = Para[4].Float_Data[0];
	minimumfeedrate = Para[5].Float_Data[0];
	minsegmenttime = (unsigned long)Para[6].Float_Data[0];

	max_xy_jerk = Para[7].Float_Data[0];
	max_z_jerk = Para[8].Float_Data[0];
	max_e_jerk = Para[9].Float_Data[0];

	Kp = Para[10].Float_Data[0];
	Ki = Para[11].Float_Data[0];
	Kd = Para[12].Float_Data[0];

	INVERT_X_DIR = (u8)Para[13].Float_Data[0];
	INVERT_Y_DIR = (u8)Para[14].Float_Data[0];
	INVERT_Z_DIR = (u8)Para[15].Float_Data[0];
	INVERT_E0_DIR = (u8)Para[16].Float_Data[0];
	INVERT_E1_DIR = (u8)Para[17].Float_Data[0];
	INVERT_E2_DIR = (u8)Para[18].Float_Data[0];

	X_ENDSTOPS_INVERTING = (u8)Para[19].Float_Data[0];
	Y_ENDSTOPS_INVERTING = (u8)Para[20].Float_Data[0];
	Z_ENDSTOPS_INVERTING = (u8)Para[21].Float_Data[0];
	V_ENDSTOPS_INVERTING = (u8)Para[22].Float_Data[0];
	//retract_feedrate = Para[22].Float_Data[0];
	//retract_zlift = Para[23].Float_Data[0];
	//retract_recover_length = Para[24].Float_Data[0];

	speed_temprature_factor = Para[23].Float_Data[0];
	max_pos[0] = Para[24].Float_Data[0];
	max_pos[1] = Para[24].Float_Data[1];
	max_pos[2] = Para[24].Float_Data[2];
	ParaVersion = Para[25].Float_Data[0];
}


#endif



