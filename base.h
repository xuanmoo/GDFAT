/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: base.h
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT 基础函数定义 
Others: 
Function List: 

History: 

*************************************************/
#ifndef	_BASE_H_
#define	_BASE_H_

short uc_short(unsigned char lb, unsigned char hb);
long uc_long(unsigned char ll, unsigned char lh, unsigned char hl, unsigned char hh);
void ReadSector(long sector, unsigned char * buff);
void fillbuffer(long sector);

#endif
