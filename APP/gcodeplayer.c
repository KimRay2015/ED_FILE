#include "gcodeplayer.h"
#include "sm_firmware.h"	 
#include "sdio_sdcard.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "string.h"
#include "language.h"
#include "temperature.h"
#include "stepper.h"
#include "24cxx.h" 
#include "sm_plus.h"
#include "sd_spi.h"
#include "mmc_sd.h"

#include "App_Timer.h"
extern USR_TIMER usr_timer;
extern uint32_t CurrentFileSize,CurrentReadSize;

#ifdef SDSUPPORT

void gcode_ls(u8 * path)
{
	u8 res;	  
    u8 *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(SRAMIN,fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path);
    if (res == FR_OK) 
	{	

		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);               
	        if (res != FR_OK || fileinfo.fname[0] == 0) break; 
	        //if (fileinfo.fname[0] == '.') continue;       
#if _USE_LFN
        	fn =(u8*)( *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
#else							   
        	fn =(u8*)( fileinfo.fname);
#endif	                                         
		//	printf("%s/", path);
	 	    res=f_typetell(fn);	
			if((res&0XF0)==0X60)
		  	{ printf("%s\n",  fn);
			}
		} 
    }	  
	myfree(SRAMIN,fileinfo.lfname);
  //  return res;	  
}

void card_ls(void)
{ 
  if(card.lsAction==LS_Count)
 // nrFiles=0;
  card.lsAction=LS_SerialPrint;
  gcode_ls("0:\\GCODE");
//  gcode_ls("0:");
}

void card_initsd(void)
{ card.cardOK = false;
  SERIAL_ECHO_START;
  if(SD_Initialize())	
	{ printf(MSG_SD_INIT_FAIL);	
	}
	 else 
	{ 	card.cardOK = true;
	 	card.cardrelase = 0;
	 	if(f_mount(fs[0],"0:",1))
	 	{
			printf("f_mount spi sdcard error\r\n");
		}
		else
			printf(MSG_SD_CARD_OK);
	}
}
void card_release(void)
{ card.sdprinting = false;
  card.cardOK = false;
  card.cardrelase = 1;
  f_mount(NULL,"0:",1);	
}


u8 *pname;
u8 card_openFile(char* fname,bool read) 
{
  u8 res;
  if(!card.cardOK)
    return 1;
 // file.close();
// printf("--");
  card.sdprinting = false;

  if(read)
  {
  memset(pname,0,sizeof(pname));
  strcpy((char*)pname,"0:/GCODE/");
//		  strcpy((char*)pname,"0:");
  strcat((char*)pname,(const char*)fname); 
  res=f_open(&card.fgcode,(const TCHAR*)pname,FA_READ);
    if (res==FR_OK)
    {
      printf(MSG_SD_FILE_OPENED);
      printf(fname);
      printf(MSG_SD_SIZE);
      printf("%ld",f_size(&card.fgcode));
	  printf("\n");
		strcpy(card.filename,fname);
			CurrentFileSize = f_size(&card.fgcode);
//	  PrintInfo.printfile;
	  //更新打印文件信息
//	  strcpy((char*)PrintInfo.printfile,(const char*)fname);
//	  PrintInfo.filesize = f_size(&card.fgcode);
//	  PrintInfo.printper = 0;
//      card.sdpos = 0;    
      printf(MSG_SD_FILE_SELECTED);
      //lcd_setstatus(fname);
      return 0;
    }
    else
    {
      printf(MSG_SD_OPEN_FILE_FAIL);
      printf(fname);
      printf(".");
			return 1;
    }
  }
  else 
  { res=f_open(&card.fgcode,(const TCHAR*)fname,FA_WRITE|FA_OPEN_ALWAYS);
    if (res!=FR_OK)
    {
      printf(MSG_SD_OPEN_FILE_FAIL);
      printf(fname);
      printf(".");
    }
    else
    {
      card.saving = true;
      printf(MSG_SD_WRITE_TO_FILE);
      printf(fname);
     // lcd_setstatus(fname);
    }
	 printf("\n");
  }
  //myfree(SRAMIN,pname);		
  //myfree(SRAMIN,fgcode);
	return(0);
}

void card_removeFile(char* fname)
{  u8 res;
  res = f_unlink(fname);
  if (res==FR_OK) 
    {
      printf("File deleted:");
      printf(fname);
//      card.sdpos = 0;
    }
    else
    {
      printf("Deletion failed, File: ");
      printf(fname);
      printf(".\n");
    }
}

void card_startFileprint(void)
{
  if(card.cardOK)
  {
  	card.sdprinting = true;		//使能打印标志	
  	if((PrintInfo.printsd==0) || (PrintInfo.printsd==2))
  	{
	    PrintInfo.printsd = 1;
		strcpy(PrintInfo.printfile,card.filename);	//更新打印文件名
		//PrintInfo.filesize = card.fgcode.fsize;		//打印文件大小 读取文件大小是否有问题?
		PrintInfo.filesize = f_size(&card.fgcode);	//读取文件大小
		CurrentFileSize = f_size(&card.fgcode);
		PrintInfo.printper = 0;
		PrintInfo.printtime = 0;
			
		/*Printer Timer Start*/
		usr_timer.sec = 0;
		usr_timer.min = 0;
		usr_timer.hour = 0;
		usr_timer.date = 0;
		usr_timer.mounth = 0;
			
		ReadChar = 0;
#ifdef REPRINTSUPPORT
	//save sd_card print flg
	AT24CXX_WriteOneByte(E_SDPRINT,true);
	printf("save sd_card print flg\r\n");
	//save file name
	u8 len;
	len = strlen(pname);
	len++;
	AT24CXX_WriteOneByte(E_FILENAME,len);	//文件名长度
	AT24CXX_Write(E_FILENAME+1,pname,len);	//文件名
	printf("save file lenth is %d name is %s\r\n",len,pname);
#endif
  	}
  }

}

void card_pauseSDPrint(void)
{
  if(card.sdprinting)
  {
    card.sdprinting = false;
  }
	Z_EN(1);
}

void card_setIndex(long index)
{//card.sdpos = index;
 f_lseek(&card.fgcode,index);
}

void card_getStatus(void)
{  if(card.cardOK)
  {
    printf(MSG_SD_PRINTING_BYTE);
    printf("%d",f_tell(&card.fgcode));
    printf("/");
    printf("%d",f_size(&card.fgcode));
  }
  else{
    printf(MSG_SD_NOT_PRINTING);
  }
  printf("\n");
}

void card_closefile(void)
{ f_close(&card.fgcode);
  card.saving = false; 
}

void card_write_command(char *buf)
{ 
}

void card_checkautostart()
{
	if(card.cardrelase==0)
	{
		if(!card.cardOK)
		{
		card_initsd();
		if(!card.cardOK) //fail
		  return;
		}
	}
  
}
void card_printingHasFinished(void)
{
    st_synchronize();
    quickStop();
    card_closefile();
	starttime=0;
	card.sdprinting = false;
//	strcpy(PrintInfo.printfile,"null");	//更新打印文件名
//	PrintInfo.filesize = 0;		//打印文件大小
//	PrintInfo.printper = 0;
	PrintInfo.printsd = 0;		//打印完成
//	PrintInfo.printtime = 0;
	

    if(SD_FINISHED_STEPPERRELEASE)
    {
      //  finishAndDisableSteppers();
        enquecommand(PSTR(SD_FINISHED_RELEASECOMMAND));
    }
    autotempShutdown();
#ifdef REPRINTSUPPORT
	AT24CXX_WriteOneByte(E_SDPRINT,false);
	printline = 0;
	printf("sdcard print has finished;save sd print flase flg\r\n");
#endif

//	 printf("5.\r\n");
}

bool card_eof(void)
{ return f_eof(&card.fgcode);
}

int16_t card_get(void) 
{ //card.sdpos = f_tell(&card.fgcode);
  return  file_read();
}

int16_t file_read(void)
{ 	
	u8 buffer[MAX_CMD_SIZE]; 
	u8 res;
	UINT br;
	
	res = f_read(&card.fgcode, buffer, 1, &br);

	ReadChar++;	
	if (res == FR_OK )
	{// printf("%s",buffer);
	return *buffer;//
	}else
	{ return -1;// printf("f_read() fail .. \r\n");  
	}
	
	
//	char Buffer[MAX_CMD_SIZE];
//	FIL File;
//	char *str;
//	str = f_gets(Buffer,MAX_CMD_SIZE,&File);
//	if(str != NULL)
//	{
//		return *Buffer;
//	}
//	else
//	{
//		return -1;
//	}
	

}
#endif //SDSUPPORT
