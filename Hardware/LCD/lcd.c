#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"

				 
//LCD的画笔颜色和背景色	   
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

#define MODE_DATA 1 //1 1data模式 0 2data模式


u8 SOFT_SPI_RWLcd(u8 cd,u8 byte)
{
   u8 i=0;
	TCLK=0;		//先拉低时钟
	TDIN=0;		//拉低数据线
	LCS=0;		//选中xpt2046
	//先发送C/D bit
	if(cd)
		TDIN=1;
	else
		TDIN=0;
//	TCLK=0;			//下降沿有效
	TCLK=1; 
    for(i=0;i<8;i++)                         // 循环8次
    {
    	TCLK=0;			//拉低时钟
      if(byte&0x80)
				TDIN=1;                             //若最到位为高，则输出高
      else
				TDIN=0;                             //若最到位为低，则输出低
			
      byte <<= 1;                           // 低一位移位到最高位		
			TCLK=1;                          
    }
	LCS=1;		//失能
    return 0;                              //返回数据
}

	 
//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(u8 regval)
{ 
	//LCD->LCD_REG=regval;//写入要写的寄存器序号	 
	SOFT_SPI_RWLcd(0,regval);
}
//写LCD数据
//data:要写入的值
void LCD_WR_DATA(u8 data)
{										    	   
	//LCD->LCD_RAM=data;
	SOFT_SPI_RWLcd(1,data);
}
u8 LCD_WR_DATA2(u16 data)
{
	u8 i=0;
	u8 SDA=(u8)(data>>8);
	u8 WRX=(u8)data;
	TCLK=0;		//先拉低时钟
	TDIN=0;		//拉低数据线
	LCD_DIN2=0;
	LCS=0;		//选中xpt2046
	TDIN=1;
	LCD_DIN2=1;
//	delay_us(1);
	TCLK=1; 
    for(i=0;i<8;i++)                         // 循环8次
    {
    	TCLK=0;			//拉低时钟
        if(SDA&0x80) 
			TDIN=1;                             //若最到位为高，则输出高
        else          
			TDIN=0;                             //若最到位为低，则输出低
        SDA <<= 1;                           	//低一位移位到最高位		

		if(WRX&0x80) 
			LCD_DIN2=1;                            
        else          
			LCD_DIN2=0;                          
        WRX <<= 1;                           			 

//		delay_us(1);
		TCLK=1;                          
    }
	LCS=1;		//失能
    return 0;  
}
//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{										    	   
	//return LCD->LCD_RAM;
		return 0;
}					   
//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
//	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号	 
//	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
	LCD_WR_REG((u8)LCD_Reg);
	LCD_WR_DATA((u8)(LCD_RegValue>>8));
	LCD_WR_DATA((u8)LCD_RegValue);
}	   
//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	delay_us(5);		  
	return LCD_RD_DATA();		//返回读到的值
}   
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	//LCD->LCD_REG=lcddev.wramcmd;
 	LCD_WR_REG(lcddev.wramcmd);
}	 
//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{							    
	//LCD->LCD_RAM = RGB_Code;//写十六位GRAM
	LCD_WR_DATA((u8)(RGB_Code>>8));
	LCD_WR_DATA((u8)RGB_Code);
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 
//当mdk -O1时间优化时需要设置
//延时i
void opt_delay(u8 i)
{
	while(i--);
}
#if 1
//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
u16 LCD_ReadPoint(u16 x,u16 y)
{
/*
 	u16 r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	LCD_SetCursor(x,y);	    
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X2E);//9341/6804/3510 发送读GRAM指令
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 发送读GRAM指令
	else LCD_WR_REG(R34);      		 				//其他IC发送读GRAM指令
 	if(lcddev.id==0X9320)opt_delay(2);				//FOR 9320,延时2us	    
	if(LCD->LCD_RAM)r=0;							//dummy Read	   
	opt_delay(2);	  
 	r=LCD->LCD_RAM;  		  						//实际坐标颜色
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)		//9341/NT35310/NT35510要分2次读出
 	{
		opt_delay(2);	  
		b=LCD->LCD_RAM; 
		g=r&0XFF;		//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
		g<<=8;
	}else if(lcddev.id==0X6804)r=LCD->LCD_RAM;		//6804第二次读取的才是真实值 
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505)return r;	//这几种IC直接返回颜色值
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
	else return LCD_BGR2RGB(r);						//其他IC
*/
	return 0xffff;
}	
#endif
//LCD开启显示
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29);	//开启显示
}	 
//LCD关闭显示
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X28);	//关闭显示 
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 	    
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(Xpos>>8); 
	LCD_WR_DATA(Xpos&0XFF);	 
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(Ypos>>8); 
	LCD_WR_DATA(Ypos&0XFF);	 
} 		 
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510等IC已经实际测试	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  

	switch(dir)
	{
		case L2R_U2D://从左到右,从上到下
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://从左到右,从下到上
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://从右到左,从上到下
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://从右到左,从下到上
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://从上到下,从左到右
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://从上到下,从右到左
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://从下到上,从左到右
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://从下到上,从右到左
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	if(lcddev.id==0X5510)dirreg=0X3600;
	else dirreg=0X36;
		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510不需要BGR   
	if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	   
	LCD_WriteReg(dirreg,regval);
	if((regval&0X20)||lcddev.dir==1)
	{
		if(lcddev.width<lcddev.height)//交换X,Y
		{
			temp=lcddev.width;
			lcddev.width=lcddev.height;
			lcddev.height=temp;
		}
	}else  
	{
		if(lcddev.width>lcddev.height)//交换X,Y
		{
			temp=lcddev.width;
			lcddev.width=lcddev.height;
			lcddev.height=temp;
		}
	}  
	if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
		LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
		LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
		LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
		LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);
	}else
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(0);LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(0);LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
	}
	
}   
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
#if MODE_DATA
	LCD_WR_DATA((u8)(POINT_COLOR>>8));
	LCD_WR_DATA((u8)POINT_COLOR);
#else
	LCD_WR_DATA2(POINT_COLOR);
#endif
}
//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{	   
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(x>>8); 
	LCD_WR_DATA(x&0XFF);
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(y>>8); 
	LCD_WR_DATA(y&0XFF);	
	LCD_WR_REG(lcddev.wramcmd);
#if MODE_DATA
	LCD_WR_DATA((u8)(color>>8));
	LCD_WR_DATA((u8)color);
#else
	LCD_WR_DATA2(color);
#endif
}	 


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		lcddev.wramcmd=0X2C;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 

		
	}else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		lcddev.wramcmd=0X2C;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 

	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{   
//	u8 hsareg,heareg,vsareg,veareg;
//	u16 hsaval,heaval,vsaval,veaval; 
	width=sx+width-1;
	height=sy+height-1;

	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(sx>>8); 
	LCD_WR_DATA(sx&0XFF);	 
	LCD_WR_DATA(width>>8); 
	LCD_WR_DATA(width&0XFF);  
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(sy>>8); 
	LCD_WR_DATA(sy&0XFF); 
	LCD_WR_DATA(height>>8); 
	LCD_WR_DATA(height&0XFF);
	
} 
#if 0
/*之前用的ic初始化代码*/
void LCD_Init(void)
{
	//读ID

	//使用例程里LCD 9314初始化代码
	LCD_WR_REG(0xCF);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0xC1); 
	LCD_WR_DATA(0X30); 
	LCD_WR_REG(0xED);  
	LCD_WR_DATA(0x64); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0X12); 
	LCD_WR_DATA(0X81); 
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA(0x85); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x7A); 
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x34); 
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA(0x20); 
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA(0x1B);   //VRH[5:0] 
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA(0x30); 	 //3F
	LCD_WR_DATA(0x30); 	 //3C
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA(0XB7); 
	LCD_WR_REG(0x36);    // Memory Access Control 
//	LCD_WR_DATA(0x48);
//	LCD_WR_DATA(0x30);	//老液晶配置值
//	LCD_WR_DATA(0x48);
//	LCD_WR_DATA(0x38);	
//	LCD_WR_DATA(0x08);
//	LCD_WR_DATA(0x18);
//	LCD_WR_DATA(0x28);
//	LCD_WR_DATA(0xa8);
//	LCD_WR_DATA(0x78);
//	LCD_WR_DATA(0x7b);
	LCD_WR_DATA(0x68);	//新液晶配置
	LCD_WR_REG(0x3A);   
	LCD_WR_DATA(0x55); 
	LCD_WR_REG(0xB1);   
	LCD_WR_DATA(0x00);   
	LCD_WR_DATA(0x1A); 
	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x0E); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x54); 
	LCD_WR_DATA(0XA9); 
	LCD_WR_DATA(0x43); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 		 
	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x15); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x11); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x2B); 
	LCD_WR_DATA(0x56); 
	LCD_WR_DATA(0x3C); 
	LCD_WR_DATA(0x05); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);	//hight=240
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);	//wigth=320

	LCD_WR_REG(0x35);	//	Tearing Effect Line ON
	LCD_WR_DATA(0x00);	//	M=0
	
	LCD_WR_REG(0x11); //Exit Sleep
	delay_ms(120);
	LCD_WR_REG(0x29); //display on	

	//初始化LCDDev结构体
	lcddev.dir = 0;		//横屏
//	lcddev.height = 320;
//	lcddev.width = 240;
	lcddev.height = 240;
	lcddev.width = 320;

	lcddev.id = 0x9314;
	lcddev.setxcmd = 0x2A;
	lcddev.setycmd = 0x2B;
	lcddev.wramcmd = 0X2C;

//	LCD_Display_Dir(1);
	LCD_Clear(WHITE);
}
#endif
#if 1
/*去年年底给的初始化ic代码*/
void LCD_Init(void)
{

//	PDout(11) = 1;
	delay_ms(120);
//	PDout(11) = 0;
	delay_ms(100);
//	PDout(11) = 1;
	delay_ms(120);

	LCD_WR_REG(0xfe);	//Inter Register Enable1
	LCD_WR_REG(0xef);	//Inter Register Enable2
	LCD_WR_REG(0x36);	// Memory Access Control

	//LCD_WR_DATA(0x60);			//横屏 
	LCD_WR_DATA(0xb0);				//横屏上下颠倒
	LCD_WR_REG(0x3a);//Pixel Format Set
	LCD_WR_DATA(0x05);
//	LCD_WR_REG(0xe9);//SPI 2DATA control
//	LCD_WR_DATA(0x08);


	LCD_WR_REG(0xa4);//Power Control 7
	LCD_WR_DATA(0x44);
	LCD_WR_DATA(0x44);
	LCD_WR_REG(0xa5);//
	LCD_WR_DATA(0x42);
	LCD_WR_DATA(0x42);
	LCD_WR_REG(0xaa);//
	LCD_WR_DATA(0x88);
	LCD_WR_DATA(0x88);

	LCD_WR_REG(0xe8);//Frame rate=71.8hz
	LCD_WR_DATA(0x11);
	LCD_WR_DATA(0x0b);

	LCD_WR_REG(0xe3);//source ps=01
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x10);

	LCD_WR_REG(0xff);//
	LCD_WR_DATA(0x61);

	LCD_WR_REG(0xAC);//ldo enable Power Control 1
	LCD_WR_DATA(0x00);
	LCD_WR_REG(0xAd);//ldo enable
	LCD_WR_DATA(0x33);

	LCD_WR_REG(0xae);
	LCD_WR_DATA(0x2b);

	LCD_WR_REG(0xAf);//DIG_VREFAD_VRDD[2]
	LCD_WR_DATA(0x55);

	LCD_WR_REG(0xa6);
	LCD_WR_DATA(0x2a);
	LCD_WR_DATA(0x2a);//29
	LCD_WR_REG(0xa7);
	LCD_WR_DATA(0x2b);
	LCD_WR_DATA(0x2b);
	LCD_WR_REG(0xa8);
	LCD_WR_DATA(0x18);
	LCD_WR_DATA(0x18);//17
	LCD_WR_REG(0xa9);
	LCD_WR_DATA(0x2a);
	LCD_WR_DATA(0x2a);
	//======================gamma===========================
	//-----display window 240X320---------//
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);
	LCD_WR_REG(0x2c);
	//--------end display window --------------//

	LCD_WR_REG(0x35);
	LCD_WR_DATA(0x00);


	LCD_WR_REG(0x44);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x0a);


	LCD_WR_REG(0xf0);
	LCD_WR_DATA(0x2);
	LCD_WR_DATA(0x1);
	LCD_WR_DATA(0x0);
	LCD_WR_DATA(0x6);
	LCD_WR_DATA(0x9);
	LCD_WR_DATA(0xC);
	LCD_WR_REG(0xf1);
	LCD_WR_DATA(0x1);
	LCD_WR_DATA(0x3);
	LCD_WR_DATA(0x0);
	LCD_WR_DATA(0x3A);
	LCD_WR_DATA(0x3E);
	LCD_WR_DATA(0x9);
	LCD_WR_REG(0xf2);
	LCD_WR_DATA(0xC);
	LCD_WR_DATA(0x9);
	LCD_WR_DATA(0x26);
	LCD_WR_DATA(0x7);
	LCD_WR_DATA(0x7);
	LCD_WR_DATA(0x30);//2d
	LCD_WR_REG(0xf3);
	LCD_WR_DATA(0x9);
	LCD_WR_DATA(0x6);
	LCD_WR_DATA(0x57);
	LCD_WR_DATA(0x3);
	LCD_WR_DATA(0x3);
	LCD_WR_DATA(0x6b);//6e
	LCD_WR_REG(0xf4);
	LCD_WR_DATA(0xd);
	LCD_WR_DATA(0x1d);
	LCD_WR_DATA(0x1c);
	LCD_WR_DATA(0x6);//5
	LCD_WR_DATA(0x8);//3
	LCD_WR_DATA(0xF);
	LCD_WR_REG(0xf5);
	LCD_WR_DATA(0xc);
	LCD_WR_DATA(0x5);
	LCD_WR_DATA(0x6);
	LCD_WR_DATA(0x33);//32
	LCD_WR_DATA(0x31);
	LCD_WR_DATA(0xF);

	//====================end gamma=========================	
	LCD_WR_REG(0x11);
	delay_ms(120);	
	LCD_WR_REG(0x29);	
	LCD_WR_REG(0x2c);
	//初始化LCDDev结构体
	lcddev.dir = 0;		//横屏
	lcddev.height = 240;
	lcddev.width = 320;

	lcddev.id = 0x9314;
	lcddev.setxcmd = 0x2A;
	lcddev.setycmd = 0x2B;
	lcddev.wramcmd = 0X2C;

//	LCD_Display_Dir(1);
	LCD_Clear(WHITE);
}
#endif

//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
#if MODE_DATA
		LCD_WR_DATA((u8)(color>>8));
		LCD_WR_DATA((u8)color);
#else
		LCD_WR_DATA2(color);
#endif
	}
}  
void LCD_half(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	totalpoint = totalpoint/2;
	LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
#if MODE_DATA
		LCD_WR_DATA((u8)(color>>8));
		LCD_WR_DATA((u8)color);
#else
		LCD_WR_DATA2(color);
#endif
	}
}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
//	u16 temp;
	
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{ 
	 	LCD_SetCursor(sx,i);      				//设置光标位置 
		LCD_WriteRAM_Prepare();     			//开始写入GRAM	  
		for(j=0;j<xlen;j++)
		{
#if MODE_DATA
			LCD_WR_DATA((u8)(color>>8));	//设置光标位置 
			LCD_WR_DATA((u8)color);
#else
			LCD_WR_DATA2(color);
#endif
		}
	}
		 
}  
//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)
		{
#if MODE_DATA
			LCD_WR_DATA((u8)(color[i*height+j]>>8));
			LCD_WR_DATA((u8)color[i*height+j]);
#else
			LCD_WR_DATA2(color[i*height+j]);
#endif
		}
	}	  
}  
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_DrawPoint(x,y);	
				temp<<=1;
				y++;
				if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }    
	}
	else//叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)LCD_DrawPoint(x,y); 
				temp<<=1;
				y++;
				if(x>=lcddev.height){POINT_COLOR=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   
//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}





void lcdtest(u16 color)
{
	int i,j;
	for(i=0;i<320;i++)
	{
		LCD_SetCursor(0,i);			//设置光标位置 
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		for(j=0;j<240;j++)
		{
			//LCD_WR_DATA2(color);
			LCD_WR_DATA((u8)(color>>8));
			LCD_WR_DATA((u8)color);
			delay_ms(1);
		}
	}
}
























