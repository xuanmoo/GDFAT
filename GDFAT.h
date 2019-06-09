/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: GDFAT.h
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT 底层驱动 数据结构及函数定义 
Others: 
Function List: 

History: 

*************************************************/

#ifndef	_GDFAT_H_
#define	_GDFAT_H_
#include "FileSystem.h"

#define SHORT_DIR	0
#define	LONG_DIR	1


struct MBR{
	long	DBRSector;
	long	TotalSector;
};

struct DBR{
	short		BytePerSector;			//11-12	每扇区字节数
	short		SectorPerCluster;		//13	每簇占扇区数
	short		SystemSector;			//14-15	保留扇区数
	short		FatCount;				//16	FAT个数

	long		HideSector;				//28-31	隐藏扇区数
	long		DPartTotalSector;		//32-35	分区总扇区数
	long		SectorPerFat;			//36-39	单个FAT占扇区数
	short		Backup;					//40-41	是否镜像 00镜像,01非镜像
	long		RootCluset;				//44-47	根目录起始簇
	short		FsinfoSector;			//48-49	FSInfo扇区号
	short		Dbr2Sector;				//50-51	DBR备份扇区号
	short		DpiFlag;				//66	扩展信息标志 0x29为有
	unsigned char	DPName[11];			//71	如果标志为0x29可读 分区卷标 

}; 

struct FSINFO{
	long		AllFreeCluster;			//统计目前空闲可用的簇数，计算磁盘剩余容量可用
	long		NextFreeCluster;		//下一个可用的簇号。
};


struct ShortDirItem
{
	unsigned char	strFilename[8];			//文件名
	unsigned char	strExtension[3];		//扩展名
	unsigned char	attribute;				//属性
	unsigned char	reserved;				//保留
	unsigned char	millisecond;			//创建日期 10毫秒位
	unsigned short	createTime;		//创建时间
	unsigned short	createDate;		//创建日期
	unsigned short	accessDate;		//访问日期
	unsigned short	highWordCluster;//起始簇高16位
	unsigned short	updateTime;		//更新时间
	unsigned short	updateDate;		//更新日期
	unsigned short	lowWordCluster;	//起始簇低16位
	unsigned long	filesize;		//文件大小
	unsigned long	Cluster;		//起始簇号
	unsigned char	dirflag;		//文件夹标识 
};

struct LongFDT
{
	unsigned char flag;//如果是0x4*,第6位置位了，说明是最后一个长文件目录，各位是下面还有几个
	unsigned char name1[10];
	unsigned char attr;//如果是长文件名，除了最下面一个，都是0F
	unsigned char reserve;
	unsigned char checksum;
	unsigned char name2[12];
	unsigned char rel_cluster[2];//相对起始簇号
	unsigned char name3[4];
};
struct LongDirItem
{
	struct LongFDT lfdt[4];
	struct ShortDirItem sfdt;
	unsigned char	longname[120];
	short	num;
};

struct DirItem{
	char	short_long;
	struct ShortDirItem	shortdiritem;
	struct LongDirItem	longdiritem;
//	unsigned char	longname[120];
};



//funsion
short	getMBRInfo(struct MBR *MBRinfo);
short	getDBRInfo(unsigned long DBRSector, struct DBR *DBRinfo );
short	getFSInfo(unsigned long FsinfoSector,struct FSINFO *FSinfo);
long	getNextCluster(struct DPInfo dpi, long CurrentCluster);
short	getDirItem0(long offset, struct DirItem *DirItem);
short	getShortDirItem(long offset , struct ShortDirItem * DirItem);
short	getLongDirItem(long offset, struct LongDirItem *DirItem);
short	getRootInfo(unsigned long RootEntrance,	struct DirItem *DirItem);


#endif
