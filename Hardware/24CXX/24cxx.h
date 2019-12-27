#ifndef __24CXX_H
#define __24CXX_H
#include "iic.h" 
#include "sys.h"


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C64

//定义存储空间分布
#define EEPROM_OFFSET_CFG 	100		//触摸屏校正值
#define E_SDPRINT			1000	//sd卡打印标志
#define E_FILENAME			1001	//保存的打印文件名
#define E_BEDTEMPERATURE	1100	//设置的热床温度值
#define E_EXTRUDERTEMPERATURE 1104	//设置的挤出头温度值
#define E_G_CODE_LINE		1108	//G-code行数
#define E_G_CODE_EAXIS		1112	//E轴绝对坐标值


#define E_G_CODE_FEED		1130	//打印速率	
#define E_G_CODE_VALUE		1140	//G-code命令值
#ifdef MESH_BED_LEVELING
#define MBL_OFFSET			1200	//调平偏差值存储地址
#define MBL_VALUE			1204	//9个点的采样值存储的起始地址
#endif
u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//指定地址读取一个字节
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					//指定地址开始读取指定长度数据
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据

u8 AT24CXX_Check(void);  //检查器件
#endif
