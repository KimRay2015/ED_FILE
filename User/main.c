/*
*Function:
*Programed by:
*Complete date:
*Modified by:
*Modified date:
*Version:
*Remarks:
*/
/* Includes ------------------------------------------------------------------*/
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


#include "lcd.h"
//#include "lcdmenu.h"
#include "piclib.h"	
//#include "lcdmenu.h"
#include "children_menu.h"
#include "text.h"
#include "guix.h"

#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "quicklz.h"
#include "bsp_spi_flash.h"
#include "sm_plus.h" 
#include "mmc_sd.h"
#include "sd_spi.h"
#include "rng.h"
#include "bsp.h"

#include "App_ADC.h"
#include "App_Timer.h"
#include "App_InfShow.h"
#include "App_SystemPortConfig.h"
#include "App_Language.h"


static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

RCC_ClocksTypeDef get_rcc_clock;    //????????

/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//????
	FILINFO tfileinfo;	//??????	
	u8 *fn;	 			 			   			     
  res=f_opendir(&tdir,(const TCHAR*)path); 	//????
  tfileinfo.lfsize=_MAX_LFN*2+1;				//????????
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);//???????????
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//?????????
		{
	    res=f_readdir(&tdir,&tfileinfo);       		//??????????
	    if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//???/????,??		  
     	fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//????,?????????	
			{
				rval++;//???????1
			}	    
		}  
	} 
	return rval;
}
/*
//??????SD?????
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//???ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//?????
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//????
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//?????
}
*/


/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
void App_FORMAT_SPI_FLASH(void)
{
	int res;
	FRESULT res_sd;
	printf("System Start Format SPI FLASH...");
	res_sd = f_mount(NULL,"1:",1);			  //unmount spi_flash 
	res = f_mount(fs[1],"1:",1);					//mount spi flash 
	
	//=========format spi flash
	res_sd=f_mkfs("1:",0,0);							//format spi flash
    res_sd = f_mount(NULL,"1:",1);			  //unmount spi_flash 		
	res_sd = f_mount(fs[1],"1:",1); 			//remount spi_flash
	//============================
	res = f_mount(fs[1],"1:",1);					// 
	if(res==FR_NO_FILESYSTEM)
	{
		printf(">>SPI FLASH还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_sd=f_mkfs("1:",0,0);							
		
		if(res_sd == FR_OK)
		{
			printf(">>SPI FLASH已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_sd = f_mount(NULL,"1:",1);			
      /* 重新挂载	*/			
			res_sd = f_mount(fs[1],"1:",1);
		}
		else
		{
			printf("<<格式化失败.>>\r\n");
			while(1);
		}
	}
	printf("System SPI FLASH Format Completed...");
}

/*
*Function:
*Input Paragramed:
*Output Paragramed:
*Remarks:
*/
int main(void)
{
	int res;

	u32 total,free;

	//FRESULT res_sd;
	CPUID[0] = *(u32*)0x1FFF7A10;
	CPUID[1] = *(u32*)0x1FFF7A14;
	CPUID[2] = *(u32*)0x1FFF7A18;
	//这是一句测试注释代码
	qlz_state_compress *state_compress = (qlz_state_compress *)mymalloc(SRAMIN,sizeof(qlz_state_compress));
	qlz_state_decompress *state_decompress = (qlz_state_decompress *)mymalloc(SRAMIN,sizeof(qlz_state_decompress));

	SCB->VTOR = FLASH_BASE | 0x11000;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//???????????2
	delay_init();
	
	/*Reset All GPIO Port Config*/
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	GPIO_DeInit(GPIOD);
	GPIO_DeInit(GPIOE);
	GPIO_DeInit(GPIOF);
	
	TIM6_Init(9,8399);
	
	uart1_init(115200);

	uart2_init(250000);
	mem_init(SRAMIN);		//internal SRAM Init
	exfuns_init();
	
	App_StartInf();
	ADC1_DMA_Config();
	App_SystemPortConfig();
	//GPIO_SetBits(GPIOD,GPIO_Pin_2);					//关闭LCD背光;

	printinfo_init();
	//初始化sd卡座探测引脚
	//gpio_sddet_init();
	//初始化SD卡文件系统
	res = f_mount(fs[0],"0:",1);					//mount TF Card
	if(res)
	{
		printf("f_mount filed error=0x%02x\r\n",res);
	}
	else
	{
		printf("f_mount successed\r\n");
		card.cardOK = TRUE;
	}
	if(exf_getfree("0",&total,&free))	//getc sd card free size
	{
		printf("SD Card Fatfs Error!");
	}
	else
	{		
		printf("SD Total Size:%d\r\n",total>>10);
		printf("SD Free Size:%d\r\n",free>>10);
	}
	
	
	//Init spiflash file system
	res = f_mount(fs[1],"1:",1);					// 
	if(res==FR_NO_FILESYSTEM)
	{
		App_FORMAT_SPI_FLASH();
	}
	else if(res==FR_OK)
		printf("f_mount spiflash successed\r\n");
	else
		printf("f_mount spiflash filed (%d)\r\n",res);
	
	if(exf_getfree("1",&total,&free))	//getc spi flash free size
	{
		printf("SPI Flash Fatfs Error!");
	}
	else
	{
		
		printf("SPI Flash Total Size:%d\r\n",total>>10);
		printf("SPI Flash Free Size:%d\r\n",free>>10);
	}

	RCC_GetClocksFreq(&get_rcc_clock);
	printf("SYSCLK_Frequency=%d\r\n",get_rcc_clock.SYSCLK_Frequency);
	printf("HCLK_Frequency=%d\r\n",get_rcc_clock.HCLK_Frequency);
	printf("PCLK1_Frequency=%d\r\n",get_rcc_clock.PCLK1_Frequency);
	printf("PCLK2_Frequency=%d\r\n",get_rcc_clock.PCLK2_Frequency);

	//soft_spi_init();	//LCD SPI Init
	IIC_Init();					//IIC_Config Init
	LCD_Init();					//LCD Display Init
	tp_dev.init();			//Touch screen Config
	gui_init();					//gui_system config
	piclib_init();			//Picture display Init
	
#if 0
	u8 i;
	while(1)
	{
		for(i=1;i<7;i++)
		{
			memset(fatbuf,0,50);
			sprintf(fatbuf,"0:/DIY_UI/%d.bmp",i);
			ai_load_picfile(fatbuf,0,0,320,240,1);
			delay_second(10);
		}
	}
#endif
	GPIO_SetBits(GPIOD,GPIO_Pin_2); 				//打开LCD背光;
	setup();
#if MACHINE_MODE == 1
	//read file name
	res = AT24CXX_ReadOneByte(E_FILENAME);
	AT24CXX_Read(E_FILENAME + 1, pname, res);
	memcpy(PrintInfo.printfile,pname+9,res);	//不要路径
#endif
//read config files
	USR_UsrLog("Start Read EEPROM Config Data...");
	res = ConfigurationWrite2EEPROM();
	if(res==0 || res==3)
	{
		save_configfile();
		Config_StoreSettings();
		Config_RetrieveSettings();
		Config_PrintSettings();
	}
	
//	reprintcheck();
	bl_touch_init();
  RNG_Init();
#if LASER	
	laser_init();
#endif
//	WWDG_Init(0x7F,0X7F,WWDG_Prescaler_8); 	//计数器值为7f,窗口寄存器为5f,分频数为8	
	usart2_send("M4016");
	{
        DIR src_dir;
        res = f_opendir(&src_dir,"0:/SYSTEM");  //查询是否有字库
        if(res == FR_OK)//存在字库
				{
            f_closedir(&src_dir);//close files
            copy_font();
            f_rename("0:/SYSTEM","0:/OLDSYSTEM");
        }
        else
				{
            USR_ErrLog("字库文件不存在");
        }
        res = f_opendir(&src_dir,"0:/diy_icon");  //查询是否有字库
        if(res == FR_OK)
				{
					f_closedir(&src_dir);//close files
					copy_diy_icons();
					ls_spi_diy_icon();
					f_rename("0:/diy_icon","0:/OLDdiy_icon");
        }
        else
				{
            USR_ErrLog("图片资源不存在");
        }
    }
		 /*Add By Kim 20191220*/
	App_TIM7_Init();
	App_TIM2_Config();
	loop();
	
}


/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
