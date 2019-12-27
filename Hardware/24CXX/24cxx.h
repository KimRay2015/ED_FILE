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
//ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02
#define EE_TYPE AT24C64

//����洢�ռ�ֲ�
#define EEPROM_OFFSET_CFG 	100		//������У��ֵ
#define E_SDPRINT			1000	//sd����ӡ��־
#define E_FILENAME			1001	//����Ĵ�ӡ�ļ���
#define E_BEDTEMPERATURE	1100	//���õ��ȴ��¶�ֵ
#define E_EXTRUDERTEMPERATURE 1104	//���õļ���ͷ�¶�ֵ
#define E_G_CODE_LINE		1108	//G-code����
#define E_G_CODE_EAXIS		1112	//E���������ֵ


#define E_G_CODE_FEED		1130	//��ӡ����	
#define E_G_CODE_VALUE		1140	//G-code����ֵ
#ifdef MESH_BED_LEVELING
#define MBL_OFFSET			1200	//��ƽƫ��ֵ�洢��ַ
#define MBL_VALUE			1204	//9����Ĳ���ֵ�洢����ʼ��ַ
#endif
u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//ָ����ַ��ȡһ���ֽ�
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//ָ����ַд��һ���ֽ�
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					//ָ����ַ��ʼ��ȡָ����������
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����

u8 AT24CXX_Check(void);  //�������
#endif
