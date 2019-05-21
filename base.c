/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: base.c
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT 基础函数
Others: 
Function List: 

History: 

*************************************************/
#include "base.h"
#include <stdio.h>
#include <stdlib.h>

/*************************************************
Function: uc_short
Description: 无符号双字节转short类型值 
Calls: 
Called By: 

Input: unsigned char lb, unsigned char hb
		低8位所在的无符号字符及高8位所在的无符号字符 
Output: 
Return: 返回转换后的值 
Others: 
*************************************************/
short uc_short(unsigned char lb, unsigned char hb)
{
	short ret = 0;
	ret = (short)hb&0xff;
	ret = ret<<8;
	ret +=(short)lb&0xff;
	return ret;
}

/*************************************************
Function: uc_long
Description: 无符号四字节转long类型值 
Calls: 
Called By: 

Input: unsigned char ll, unsigned char lh, 
	unsigned char hl, unsigned char hh
		所需的四个字节 
Output: 
Return: 返回转换后的值 
Others: 
*************************************************/
long uc_long(unsigned char ll, unsigned char lh, unsigned char hl, unsigned char hh)
{
	long ret = 0;
	ret = (long)hh&0xff;
	ret = ret<<8;
	ret +=(long)hl&0xff;
	ret = ret<<8;
	ret +=(long)lh&0xff;
	ret = ret<<8;
	ret +=(long)ll&0xff;
		
	return ret;
}


/*************************************************
Function: fillbuffer
Description: 从tf卡中获取一个扇区的内容到buff_sec中 
Calls: 
Called By: 

Input: long sector 扇区号 
Output: 
Return: 
Others: 
*************************************************/
unsigned char	buffer_sec[512];
void fillbuffer(long sector)
{
	 ReadSector(sector,  buffer_sec);
}

/*************************************************
Function: ReadSector 
Description: 读一个扇区到相应的buff 
Calls: 
Called By: 

Input: long sector, unsigned char * buff 扇区号和buff 
Output: 
Return: 
Others: 
*************************************************/
#ifdef	WIN32
void ReadSector(long sector, unsigned char * buff)//pc test need
{
	int	offset	=	sector	* 512;
	FILE *fp	=	fopen("SD","rb");
	
	fseek(fp,offset,SEEK_SET);
	fread((void *)buff, 1, 512, fp);
	fclose(fp);
}
#endif
