#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
#include "usart.h"	 
#include "delay.h"

				 
//LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

#define MODE_DATA 1 //1 1dataģʽ 0 2dataģʽ


u8 SOFT_SPI_RWLcd(u8 cd,u8 byte)
{
   u8 i=0;
	TCLK=0;		//������ʱ��
	TDIN=0;		//����������
	LCS=0;		//ѡ��xpt2046
	//�ȷ���C/D bit
	if(cd)
		TDIN=1;
	else
		TDIN=0;
//	TCLK=0;			//�½�����Ч
	TCLK=1; 
    for(i=0;i<8;i++)                         // ѭ��8��
    {
    	TCLK=0;			//����ʱ��
      if(byte&0x80)
				TDIN=1;                             //���λΪ�ߣ��������
      else
				TDIN=0;                             //���λΪ�ͣ��������
			
      byte <<= 1;                           // ��һλ��λ�����λ		
			TCLK=1;                          
    }
	LCS=1;		//ʧ��
    return 0;                              //��������
}

	 
//д�Ĵ�������
//regval:�Ĵ���ֵ
void LCD_WR_REG(u8 regval)
{ 
	//LCD->LCD_REG=regval;//д��Ҫд�ļĴ������	 
	SOFT_SPI_RWLcd(0,regval);
}
//дLCD����
//data:Ҫд���ֵ
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
	TCLK=0;		//������ʱ��
	TDIN=0;		//����������
	LCD_DIN2=0;
	LCS=0;		//ѡ��xpt2046
	TDIN=1;
	LCD_DIN2=1;
//	delay_us(1);
	TCLK=1; 
    for(i=0;i<8;i++)                         // ѭ��8��
    {
    	TCLK=0;			//����ʱ��
        if(SDA&0x80) 
			TDIN=1;                             //���λΪ�ߣ��������
        else          
			TDIN=0;                             //���λΪ�ͣ��������
        SDA <<= 1;                           	//��һλ��λ�����λ		

		if(WRX&0x80) 
			LCD_DIN2=1;                            
        else          
			LCD_DIN2=0;                          
        WRX <<= 1;                           			 

//		delay_us(1);
		TCLK=1;                          
    }
	LCS=1;		//ʧ��
    return 0;  
}
//��LCD����
//����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{										    	   
	//return LCD->LCD_RAM;
		return 0;
}					   
//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{	
//	LCD->LCD_REG = LCD_Reg;		//д��Ҫд�ļĴ������	 
//	LCD->LCD_RAM = LCD_RegValue;//д������	    		 
	LCD_WR_REG((u8)LCD_Reg);
	LCD_WR_DATA((u8)(LCD_RegValue>>8));
	LCD_WR_DATA((u8)LCD_RegValue);
}	   
//���Ĵ���
//LCD_Reg:�Ĵ�����ַ
//����ֵ:����������
u16 LCD_ReadReg(u16 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		//д��Ҫ���ļĴ������
	delay_us(5);		  
	return LCD_RD_DATA();		//���ض�����ֵ
}   
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
 	//LCD->LCD_REG=lcddev.wramcmd;
 	LCD_WR_REG(lcddev.wramcmd);
}	 
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{							    
	//LCD->LCD_RAM = RGB_Code;//дʮ��λGRAM
	LCD_WR_DATA((u8)(RGB_Code>>8));
	LCD_WR_DATA((u8)RGB_Code);
}
//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(u8 i)
{
	while(i--);
}
#if 1
//��ȡ��ĳ�����ɫֵ	 
//x,y:����
//����ֵ:�˵����ɫ
u16 LCD_ReadPoint(u16 x,u16 y)
{
/*
 	u16 r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//�����˷�Χ,ֱ�ӷ���		   
	LCD_SetCursor(x,y);	    
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X2E);//9341/6804/3510 ���Ͷ�GRAMָ��
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 ���Ͷ�GRAMָ��
	else LCD_WR_REG(R34);      		 				//����IC���Ͷ�GRAMָ��
 	if(lcddev.id==0X9320)opt_delay(2);				//FOR 9320,��ʱ2us	    
	if(LCD->LCD_RAM)r=0;							//dummy Read	   
	opt_delay(2);	  
 	r=LCD->LCD_RAM;  		  						//ʵ��������ɫ
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)		//9341/NT35310/NT35510Ҫ��2�ζ���
 	{
		opt_delay(2);	  
		b=LCD->LCD_RAM; 
		g=r&0XFF;		//����9341/5310/5510,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
		g<<=8;
	}else if(lcddev.id==0X6804)r=LCD->LCD_RAM;		//6804�ڶ��ζ�ȡ�Ĳ�����ʵֵ 
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505)return r;	//�⼸��ICֱ�ӷ�����ɫֵ
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
	else return LCD_BGR2RGB(r);						//����IC
*/
	return 0xffff;
}	
#endif
//LCD������ʾ
void LCD_DisplayOn(void)
{					   
	LCD_WR_REG(0X29);	//������ʾ
}	 
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{	   
	LCD_WR_REG(0X28);	//�ر���ʾ 
}   
//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	 	    
	LCD_WR_REG(lcddev.setxcmd); 
	LCD_WR_DATA(Xpos>>8); 
	LCD_WR_DATA(Xpos&0XFF);	 
	LCD_WR_REG(lcddev.setycmd); 
	LCD_WR_DATA(Ypos>>8); 
	LCD_WR_DATA(Ypos&0XFF);	 
} 		 
//����LCD���Զ�ɨ�跽��
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510��IC�Ѿ�ʵ�ʲ���	   	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;  

	switch(dir)
	{
		case L2R_U2D://������,���ϵ���
			regval|=(0<<7)|(0<<6)|(0<<5); 
			break;
		case L2R_D2U://������,���µ���
			regval|=(1<<7)|(0<<6)|(0<<5); 
			break;
		case R2L_U2D://���ҵ���,���ϵ���
			regval|=(0<<7)|(1<<6)|(0<<5); 
			break;
		case R2L_D2U://���ҵ���,���µ���
			regval|=(1<<7)|(1<<6)|(0<<5); 
			break;	 
		case U2D_L2R://���ϵ���,������
			regval|=(0<<7)|(0<<6)|(1<<5); 
			break;
		case U2D_R2L://���ϵ���,���ҵ���
			regval|=(0<<7)|(1<<6)|(1<<5); 
			break;
		case D2U_L2R://���µ���,������
			regval|=(1<<7)|(0<<6)|(1<<5); 
			break;
		case D2U_R2L://���µ���,���ҵ���
			regval|=(1<<7)|(1<<6)|(1<<5); 
			break;	 
	}
	if(lcddev.id==0X5510)dirreg=0X3600;
	else dirreg=0X36;
		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510����ҪBGR   
	if(lcddev.id==0X6804)regval|=0x02;//6804��BIT6��9341�ķ���	   
	LCD_WriteReg(dirreg,regval);
	if((regval&0X20)||lcddev.dir==1)
	{
		if(lcddev.width<lcddev.height)//����X,Y
		{
			temp=lcddev.width;
			lcddev.width=lcddev.height;
			lcddev.height=temp;
		}
	}else  
	{
		if(lcddev.width>lcddev.height)//����X,Y
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
//����
//x,y:����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
#if MODE_DATA
	LCD_WR_DATA((u8)(POINT_COLOR>>8));
	LCD_WR_DATA((u8)POINT_COLOR);
#else
	LCD_WR_DATA2(POINT_COLOR);
#endif
}
//���ٻ���
//x,y:����
//color:��ɫ
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


//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//����
	{
		lcddev.dir=0;	//����
		lcddev.width=240;
		lcddev.height=320;
		lcddev.wramcmd=0X2C;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 

		
	}else 				//����
	{	  				
		lcddev.dir=1;	//����
		lcddev.width=320;
		lcddev.height=240;
		lcddev.wramcmd=0X2C;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 

	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}	 
//���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ�Ⱥ͸߶�,�������0!!
//�����С:width*height.
//68042,����ʱ��֧�ִ�������!! 
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
/*֮ǰ�õ�ic��ʼ������*/
void LCD_Init(void)
{
	//��ID

	//ʹ��������LCD 9314��ʼ������
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
//	LCD_WR_DATA(0x30);	//��Һ������ֵ
//	LCD_WR_DATA(0x48);
//	LCD_WR_DATA(0x38);	
//	LCD_WR_DATA(0x08);
//	LCD_WR_DATA(0x18);
//	LCD_WR_DATA(0x28);
//	LCD_WR_DATA(0xa8);
//	LCD_WR_DATA(0x78);
//	LCD_WR_DATA(0x7b);
	LCD_WR_DATA(0x68);	//��Һ������
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

	//��ʼ��LCDDev�ṹ��
	lcddev.dir = 0;		//����
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
/*ȥ����׸��ĳ�ʼ��ic����*/
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

	//LCD_WR_DATA(0x60);			//���� 
	LCD_WR_DATA(0xb0);				//�������µߵ�
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
	//��ʼ��LCDDev�ṹ��
	lcddev.dir = 0;		//����
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

//��������
//color:Ҫ���������ɫ
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//�õ��ܵ���
	LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	 	  
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
	totalpoint*=lcddev.height; 			//�õ��ܵ���
	totalpoint = totalpoint/2;
	LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	 	  
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
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{          
	u16 i,j;
	u16 xlen=0;
//	u16 temp;
	
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
	{ 
	 	LCD_SetCursor(sx,i);      				//���ù��λ�� 
		LCD_WriteRAM_Prepare();     			//��ʼд��GRAM	  
		for(j=0;j<xlen;j++)
		{
#if MODE_DATA
			LCD_WR_DATA((u8)(color>>8));	//���ù��λ�� 
			LCD_WR_DATA((u8)color);
#else
			LCD_WR_DATA2(color);
#endif
		}
	}
		 
}  
//��ָ�����������ָ����ɫ��			 
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{  
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			//�õ����Ŀ��
	height=ey-sy+1;			//�߶�
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM
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
//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
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
//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
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
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	//���ô���		   
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	if(!mode) //�ǵ��ӷ�ʽ
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //����1206����
			else temp=asc2_1608[num][t];		 //����1608���� 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)POINT_COLOR=colortemp;
				else POINT_COLOR=BACK_COLOR;
				LCD_DrawPoint(x,y);	
				temp<<=1;
				y++;
				if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//��������
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//��������
					break;
				}
			}  	 
	    }    
	}
	else//���ӷ�ʽ
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //����1206����
			else temp=asc2_1608[num][t];		 //����1608���� 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)LCD_DrawPoint(x,y); 
				temp<<=1;
				y++;
				if(x>=lcddev.height){POINT_COLOR=colortemp;return;}//��������
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){POINT_COLOR=colortemp;return;}//��������
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
}   
//m^n����
//����ֵ:m^n�η�.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
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
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
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
//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
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
		LCD_SetCursor(0,i);			//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		for(j=0;j<240;j++)
		{
			//LCD_WR_DATA2(color);
			LCD_WR_DATA((u8)(color>>8));
			LCD_WR_DATA((u8)color);
			delay_ms(1);
		}
	}
}
























