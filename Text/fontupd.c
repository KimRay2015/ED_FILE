#include "fontupd.h"
#include "ff.h"	  
//#include "flash.h"   
#include "lcd.h"  
#include "malloc.h"










						   
//字库、单色图标 信息结构体. 
//用来保存字库基本信息，地址，大小等
_font_info ftinfo;

//在sd卡中的路径
const u8 *GBK16_SDPATH="0:/SYSTEM/FONT/GBK16.FON";		//GBK16的存放位置
const u8 *GBK12_SDPATH="0:/SYSTEM/FONT/GBK12.FON";		//GBK12的存放位置
const u8 *UNIGBK_SDPATH="0:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN的存放位置

const u8 *ICON_CODE_SDPATH="0:/SYSTEM/FONT/ICON_CODE.BIN";	//icon_code.bin的存放位置//20150909

//在25Qxx中的路径
const u8 *GBK16_25QPATH="1:/SYSTEM/FONT/GBK16.FON";		//GBK16的存放位置
const u8 *GBK12_25QPATH="1:/SYSTEM/FONT/GBK12.FON";		//GBK12的存放位置
const u8 *UNIGBK_25QPATH="1:/SYSTEM/FONT/UNIGBK.BIN";	//UNIGBK.BIN的存放位置

//显示当前字体更新进度
//x,y:坐标
//size:字体大小
//fsize:整个文件大小
//pos:当前文件指针位置
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
/*
	float prog;
	u8 t=0XFF;
	prog=(float)pos/fsize;
	prog*=100;
	if(t!=prog)
	{
		LCD_ShowString(x+3*size/2,y,240,320,size,"%");		
		t=prog;
		if(t>100)t=100;
		LCD_ShowNum(x,y,t,3,size);//显示数值
	}
	return 0;	
*/
} 
//更新某一个
//x,y:坐标
//size:字体大小
//fxpath:路径
//fx:更新的内容 0,ungbk;1,gbk12;2,gbk16;
//返回值:0,成功;其他,失败.
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)//5,  0,  12,  "1:/SYSTEM/FONT/UNIGBK.BIN";   ，0
{	
	/*
	u32 flashaddr=0;								    
	FIL * fftemp;
	u8 *tempbuf;
 	u8 res;	
	u16 bread;
	u32 offx=0;
	u8 rval=0;	  
	//分配俩内存
	fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存	  为目标文件结构体分配内存
	if(fftemp==NULL)rval=1;
	tempbuf=mymalloc(SRAMIN,4096);	//分配4096个字节空间   为要写入Flash中的字节BUF分配4096字节的内存
	if(tempbuf==NULL)rval=1;
	
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); //从SD卡中打开文件  获取文件的各个字体的信息（包括大小）
 	if(res)rval=2;//打开文件失败  
 	if(rval==0)	 
	{
		if(fx==0)		//更新UNIGBK.BIN
		{
			ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);//信息头之后，紧跟UNIGBK转换码表
  			ftinfo.ugbksize=fftemp->fsize;				//UNIGBK大小
 			flashaddr=ftinfo.ugbkaddr;
		}else if(fx==1)	//GBK12
		{				  
			ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;		//UNIGBK之后，紧跟GBK12字库
			ftinfo.gbk12size=fftemp->fsize;						//GBK12字库大小
			flashaddr=ftinfo.f12addr;							//GBK12的起始地址
		}else	if(fx==2)		//GBK16
		{
			ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;		//GBK12之后，紧跟GBK16字库
			ftinfo.gbk16size=fftemp->fsize;						//GBK16字库大小
			flashaddr=ftinfo.f16addr;							//GBK16的起始地址
		}else	if(fx==3)		//ICON_CODE                                                                             //20150909
		{
			ftinfo.icon_codeaddr=ICON_START_ADDR;//GBK16之后，紧跟ICON_CODE字库
			ftinfo.icon_codesize=fftemp->fsize;						//ICON_CODE字库大小
			flashaddr=ftinfo.icon_codeaddr;							//ICON_CODE的起始地址
		}	   
		while(res==FR_OK)//死循环执行
		{
	 		res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//从SD卡文件中读取数据，获取 已经读取的字节数bread
			if(res!=FR_OK)break;								//执行错误
			SPI_Flash_Write(tempbuf,offx+flashaddr,4096);		//从0+FONTINFOADDR+sizeof(ftinfo) 开始写入4096个数据
	  		offx+=bread;
			fupd_prog(x,y,size,fftemp->fsize,offx);	 			//进度显示
			if(bread!=4096)break;								//读完了.
	 	}
		f_close(fftemp);
	}
	myfree(SRAMIN,fftemp);	 //释放内存
	myfree(SRAMIN,tempbuf);	 //释放内存
	return res;
	*/
}

//更新字体文件,UNIGBK,GBK12,GBK16一起更新
//x,y:提示信息的显示地址
//size:字体大小
//提示信息字体大小
//src:0,从SD卡更新.
//	  1,从25QXX更新											  
//返回值:0,更新成功;
//		 其他,错误代码.	  
u8 update_font(u16 x,u16 y,u8 size,u8 src)//5,0,12,0
{	
/*
	u8 *gbk16_path;
	u8 *gbk12_path;
	u8 *unigbk_path;
	u8 *icon_code_path;
	u8 res;		  
	if(src)//从25qxx更新
	{
		unigbk_path=(u8*)UNIGBK_25QPATH;
		gbk12_path=(u8*)GBK12_25QPATH;
		gbk16_path=(u8*)GBK16_25QPATH;
		gbk16_path=(u8*)GBK16_25QPATH;
	}else//从sd卡更新
	{
		unigbk_path=(u8*)UNIGBK_SDPATH;
		gbk12_path=(u8*)GBK12_SDPATH;
		gbk16_path=(u8*)GBK16_SDPATH;
		icon_code_path=(u8*)ICON_CODE_SDPATH;
	}
 	res=0XFF;
	ftinfo.fontok=0XFF;
  	SPI_Flash_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//再次写入0xFF， 即使之前是0xAA也不怕都全部重新更新字库  清除之前字库成功的标志.防止更新到一半重启,导致的字库部分数据丢失.
 	SPI_Flash_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//重新读出ftinfo结构体数据
	
 	LCD_ShowString(x,y,240,320,size,"Updating UNIGBK.BIN  ");
	res=updata_fontx(x+20*size/2,y,size,unigbk_path,0);			//更新UNIGBK.BIN
	if(res)return 1;
	
 	LCD_ShowString(x,y,240,320,size,"Updating GBK12.BIN");
	res=updata_fontx(x+20*size/2,y,size,gbk12_path,1);			//更新GBK12.FON
	if(res)return 2;
	
	LCD_ShowString(x,y,240,320,size,"Updating GBK16.BIN  ");
	res=updata_fontx(x+20*size/2,y,size,gbk16_path,2);			//更新GBK16.FON
	if(res)return 3;
	
	LCD_ShowString(x,y,240,320,size,"Updating ICON_CODE.BIN  "); //20150909
	res=updata_fontx(x+20*size/2,y,size,icon_code_path,3);			//更新ICON_CODE.FON
	if(res)return 4;
	
	//全部更新好了
	ftinfo.fontok=0XAA;
  	SPI_Flash_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//保存字库信息
	return 0;//无错误.	
*/
}
//初始化字体
//返回值:0,字库完好.
//		 其他,字库丢失
u8 font_init(void)
{
/*
//	SPI_Flash_Init();
//	FONTINFOADDR=(1024*6+500)*1024;			//W25Q64,6M以后	 
	ftinfo.ugbkaddr=FONTINFOADDR+33;		//UNICODEGBK 表存放首地址固定地址//20150909   25--->33
	SPI_Flash_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//读出ftinfo结构体数据   ftinfo虽然是结构体，但是它里面的元素地址也是按地址顺序存储的，正好25个字节
	if(ftinfo.fontok!=0XAA)return 1;		//字库错误.
	return 0;	
*/
}





























