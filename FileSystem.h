/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: FileSystem.h
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT 实现层 数据结构及函数定义
Others: 
Function List: 

History: 

*************************************************/

#ifndef	_FILESYSTEM_H_
#define	_FILESYSTEM_H_
#include "GDFAT.h"
#include "FileSystem.h"

struct DPInfo{
	unsigned long	logicsector;		//逻辑0扇区对应物理扇区,即DBR扇区
	unsigned long	totalsector;		//分区总扇区数
	unsigned long	usedcluster;		//已用簇
	unsigned long	freecluster;		//剩余簇
	unsigned long	sectorsize;			//扇区大小
	unsigned long	clustersize;		//簇大小
	unsigned long	SectorPerCluster;		//每簇占扇区数
	unsigned long	fatsector;			//FAT扇区
	unsigned long	rootsector;			//根目录扇区
	unsigned char	DPname[12];			//卷标
	
	unsigned long	fsinfoSector;			//FSInfo扇区号
	unsigned long	dbr2Sector;				//DBR备份扇区号
	unsigned long	nextfreecluster;		//下一个可用的簇号。
};



//FUN
short LoadDiskDart(struct DPInfo *dpi);
short FindDpName(struct DPInfo *dpi);
unsigned long getClusterSector(struct DPInfo dpi,unsigned long Cluster);
//short ListRoot(struct DPInfo dpi, struct DirItem *dir);
#endif
