/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: base.c
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT ��������
Others: 
Function List: 

History: 

*************************************************/
#include "base.h"
#include <stdio.h>
#include <stdlib.h>

/*************************************************
Function: uc_short
Description: �޷���˫�ֽ�תshort����ֵ 
Calls: 
Called By: 

Input: unsigned char lb, unsigned char hb
		��8λ���ڵ��޷����ַ�����8λ���ڵ��޷����ַ� 
Output: 
Return: ����ת�����ֵ 
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
Description: �޷������ֽ�תlong����ֵ 
Calls: 
Called By: 

Input: unsigned char ll, unsigned char lh, 
	unsigned char hl, unsigned char hh
		������ĸ��ֽ� 
Output: 
Return: ����ת�����ֵ 
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
Description: ��tf���л�ȡһ�����������ݵ�buff_sec�� 
Calls: 
Called By: 

Input: long sector ������ 
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
Description: ��һ����������Ӧ��buff 
Calls: 
Called By: 

Input: long sector, unsigned char * buff �����ź�buff 
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
