/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: FileSystem.c
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT 底层驱动 
Others: 
Function List: 

History: 

*************************************************/

#include "base.h"
#include "FileSystem.h"


extern	unsigned char	buffer_sec[512];


/*************************************************
Function: getMBRInfo
Description: 获取MBR扇区数据,解码非引导的主要内容 
Calls: 
Called By: 

Input: struct MBR *MBRinfo 须返回的DBR扇区入口及总扇区(不含隐藏扇区和保留扇区) 
Output: 
Return: 成功返回0;否则返回-1; 
Others: 
*************************************************/
short getMBRInfo(struct MBR *MBRinfo)
{
	fillbuffer(0);
	if(buffer_sec[510] == 0x55 && buffer_sec[511] == 0xaa)
	{
			MBRinfo->DBRSector		=	uc_long(buffer_sec[454],\
				buffer_sec[455],buffer_sec[456],buffer_sec[457]);
			MBRinfo->TotalSector	=	uc_long(buffer_sec[458],\
				buffer_sec[459],buffer_sec[460],buffer_sec[461]);
			return 0;
	}
	else 
		return -1;

}

/*************************************************
Function: getDBRInfo
Description: 获取DBR扇区数据,解码与文件系统相关的信息 
Calls: 
Called By: 

Input: struct MBR *MBRinfo, struct DBR *DBRinfo 需要解码部分信息和
		DBR中重要信息存在MBR结构体中,方便后续函数调用 
Output: 
Return: 成功返回0;否则返回-1; 
Others: 
*************************************************/
short getDBRInfo(struct MBR *MBRinfo, struct DBR *DBRinfo )
{
		unsigned char	i; 
		fillbuffer(MBRinfo->DBRSector);
		if(buffer_sec[510] == 0x55 && buffer_sec[511] == 0xaa)
		{
		
			DBRinfo->BytePerSector		=	uc_short(buffer_sec[11],buffer_sec[12]);	//11-12	每扇区字节数
			DBRinfo->SectorPerCluster	=	(short)buffer_sec[13];						//13	每簇占扇区数
			DBRinfo->SystemSector		=	uc_short(buffer_sec[14],buffer_sec[15]);	//14-15	保留扇区数
			DBRinfo->FatCount			=	(short)buffer_sec[16];						//16	FAT个数

			DBRinfo->HideSector			=	uc_long(buffer_sec[28],buffer_sec[29],\
											buffer_sec[30],buffer_sec[31]);				//28-31	隐藏扇区数
			DBRinfo->DPartTotalSector	=	uc_long(buffer_sec[32],buffer_sec[33],\
											buffer_sec[34],buffer_sec[35]);				//32-35	分区总扇区数
			DBRinfo->SectorPerFat		=	uc_long(buffer_sec[36],buffer_sec[37],\
											buffer_sec[38],buffer_sec[39]);				//36-39	单个FAT占扇区数
			DBRinfo->Backup				=	uc_short(buffer_sec[40],buffer_sec[41]);	//40-41	是否镜像 00镜像,01非镜像
			DBRinfo->RootCluset			=	uc_long(buffer_sec[44],buffer_sec[45],\
											buffer_sec[46],buffer_sec[47]);				//44-47	根目录起始簇
			DBRinfo->FsinfoSector		=	uc_short(buffer_sec[48],buffer_sec[49]);	//48-49	FSInfo扇区号
			DBRinfo->Dbr2Sector			=	uc_short(buffer_sec[50],buffer_sec[51]);	//50-51	DBR备份扇区号

			if(buffer_sec[66]==0x29)
			{
				DBRinfo->DpiFlag	=	1;
				for(i = 0; i < 11; i++)
				{
					DBRinfo->DPName[i]	=	buffer_sec[71+i]; 	//71	如果标志为0x29可读 分区卷标 
				}
			}
			else DBRinfo->DpiFlag	=	0;
			
			
			//重要信息解码
			MBRinfo->BytePerCluster		=	DBRinfo->BytePerSector * DBRinfo->SectorPerCluster;			//每个簇大小 byte
			MBRinfo->SectorPerCluster	=	DBRinfo->SectorPerCluster;
			MBRinfo->BytePerSector		=	DBRinfo->BytePerSector;
			MBRinfo->DPartTotalSector	=	DBRinfo->DPartTotalSector;
			
			MBRinfo->FATEntrance		=	DBRinfo->HideSector + DBRinfo->SystemSector;			//fat表入口
			MBRinfo->RootEntrance		=	MBRinfo->FATEntrance + DBRinfo->FatCount * DBRinfo->SectorPerFat;			//根目录入口
			MBRinfo->LogicEntrance		=	MBRinfo->DBRSector;
			MBRinfo->FsinfoSector		=	DBRinfo->FsinfoSector;
			MBRinfo->Dbr2Sector			=	DBRinfo->Dbr2Sector;
			return 0;
		}
	return -1; 
}

/*************************************************
Function: getFSInfo
Description: 获取FSInfo扇区 簇总数\剩余簇\下一个可用簇等 
Calls: 
Called By: 

Input: struct MBR *MBRinfo,struct FSINFO *FSinfo 需从MBR
		中读取从DBR中转存的入口和将计算部分数值存到MBR结构体中 

Output: 
Return: 成功返回0;否则返回-1或-2; 
Others: 
*************************************************/
short getFSInfo(struct MBR *MBRinfo,struct FSINFO *FSinfo)
{
	fillbuffer(MBRinfo->LogicEntrance + MBRinfo->FsinfoSector);
	if(buffer_sec[510] == 0x55 && buffer_sec[511] == 0xaa)
	{
		if(buffer_sec[0] == 0x52 && buffer_sec[1] == 0x52 \
		&& buffer_sec[2] == 0x61 && buffer_sec[3] == 0x41 \
		&& buffer_sec[484] == 0x72 && buffer_sec[485] == 0x72 \
		&& buffer_sec[486] == 0x41 && buffer_sec[487] == 0x61 \
		)
		{
			FSinfo->AllFreeCluster		= uc_long(buffer_sec[488],\
					buffer_sec[489],buffer_sec[490],buffer_sec[491]);
					
			FSinfo->NextFreeCluster		= uc_long(buffer_sec[492],\
					buffer_sec[493],buffer_sec[494],buffer_sec[495]);
		
			//重要信息解码
			MBRinfo->AllSize		=	MBRinfo->TotalSector/1024 * MBRinfo->BytePerSector;
			MBRinfo->DPSize			=	MBRinfo->DPartTotalSector/1024 * MBRinfo->BytePerSector;
			MBRinfo->FreeSize		=	FSinfo->AllFreeCluster/1024 * MBRinfo->BytePerCluster;
			MBRinfo->UseSize		=	MBRinfo->DPSize - MBRinfo->FreeSize;
			return 0; 
		}
		else	return -1;	//fsinfo标志识别不到 
		
	}
	
	return -2;		//扇区标志识别不到 
}

/*************************************************
Function: getNextCluster
Description: 读取FAT扇区,获取一个簇指向的下一个簇 
			或检查某个簇的状态,可用或被占用或坏簇 
Calls: 
Called By: 

Input: struct MBR MBRinfo, long CurrentCluster
		需从MBR中读取FAT扇区入口,需要当前簇号 

Output: 
Return: 若文件有下一个簇,将返回下一个簇号
		若文件无下一个簇,返回-1;
		若该簇可用,返回0;保留簇返回-3;坏簇返回-2;
Others: 
*************************************************/
long getNextCluster(struct MBR MBRinfo, long CurrentCluster)
{
	long fatsector,fatoffset,value;
	
	fatoffset	=	(CurrentCluster % (MBRinfo.BytePerSector / 4)) * 4;
	fatsector	=	CurrentCluster * 4 / MBRinfo.BytePerSector;

	fillbuffer(MBRinfo.FATEntrance + fatsector);
	value		=	uc_long(buffer_sec[fatoffset],buffer_sec[fatoffset + 1],\
					buffer_sec[fatoffset + 2],buffer_sec[fatoffset + 3]);
	
	switch(value)
	{
		case	0x0:
					return  0;	//可用簇
					break;
		case	0x1:
					return	-3;	//保留簇
					break;
		case	0x0FFFFFF0:
		case	0x0FFFFFF1:
		case	0x0FFFFFF2:
		case	0x0FFFFFF3:
		case	0x0FFFFFF4:
		case	0x0FFFFFF5:
		case	0x0FFFFFF6:
					return -3;	//保留簇
					break;
		case	0x0FFFFFF7:
					return	-2;	//坏簇
					break;
		case	0x0FFFFFF8:
		case	0x0FFFFFF9:
		case	0x0FFFFFFA:
		case	0x0FFFFFFB:
		case	0x0FFFFFFC:
		case	0x0FFFFFFD:
		case	0x0FFFFFFE:
		case	0x0FFFFFFF:
					return	-1;	//最后一个簇
					break;
		default:
					return	value;
					break;
	}
}


/*************************************************
Function: getShortDirItem
Description: 获取一个短文件名的目录或文件项,即某个
			文件或目录的相关属性 
Calls: 
Called By: 

Input: long sector, long offset , struct ShortDirItem * SDirItem
		需要扇区号及偏移地址,以及返回短文件名的结构体 

Output: 
Return: 返回32 短文件名项占用32字节 
Others: 
*************************************************/
short	getShortDirItem(long sector, long offset , struct ShortDirItem * SDirItem)
{
	unsigned char	i;
	for(i = 0; i < 8; i++)
	{
		SDirItem->strFilename[i]	=	buffer_sec[offset + i];
	}
	for(i = 0; i < 3; i++)
	{
		SDirItem->strExtension[i]	=	buffer_sec[offset + 8 + i];
	}
	SDirItem->attribute				=	buffer_sec[offset + 0xb];
	SDirItem->millisecond			=	buffer_sec[offset + 0xd];
	SDirItem->createTime			=	uc_short(buffer_sec[offset + 0xe],buffer_sec[offset + 0xf]);
	SDirItem->createDate			=	uc_short(buffer_sec[offset + 0x10],buffer_sec[offset + 0x11]);
	SDirItem->accessDate			=	uc_short(buffer_sec[offset + 0x12],buffer_sec[offset + 0x13]);
	SDirItem->highWordCluster		=	uc_short(buffer_sec[offset + 0x14],buffer_sec[offset + 0x15]);
	SDirItem->updateTime			=	uc_short(buffer_sec[offset + 0x16],buffer_sec[offset + 0x17]);
	SDirItem->updateDate			=	uc_short(buffer_sec[offset + 0x18],buffer_sec[offset + 0x19]);
	SDirItem->lowWordCluster		=	uc_short(buffer_sec[offset + 0x1a],buffer_sec[offset + 0x1b]);
	SDirItem->filesize				=	uc_long(buffer_sec[offset + 0x1c],buffer_sec[offset + 0x1d],\
										buffer_sec[offset + 0x1e],buffer_sec[offset + 0x1f]);
	SDirItem->Cluster				=	uc_long(buffer_sec[offset + 0x1a],buffer_sec[offset + 0x1b],\
										buffer_sec[offset + 0x14],buffer_sec[offset + 0x15]);
	return 32;		//下一个项位移 
}

/*************************************************
Function: getLongDirItem
Description: 获取一个长文件名的目录或文件项,即某个
			文件或目录的相关属性 
Calls: 
Called By: 

Input: long sector, long offset , struct LongDirItem * LDirItem
		需要扇区号及偏移地址,以及返回长文件名的结构体 

Output: 
Return: 返回0
Others: 
*************************************************/
unsigned char	buffer_sec2[512];		//若文件项延续至下一扇区 
short	getLongDirItem(long sector, long offset, struct LongDirItem *LDirItem)
{
	
	return 0;
}


/*************************************************
Function: getDirItem
Description: 获取一个目录或文件项,即某个
			文件或目录的相关属性 
Calls: 
Called By: 

Input: long sector, long offset , struct DirItem * DirItem
		需要扇区号及偏移地址,以及返回目录或文件结构体 

Output: 
Return: 返回0
Others: 
*************************************************/
short	getDirItem(long sector, long offset, struct DirItem *DirItem)
{
	//printf("-----%x,%d\n",buffer_sec[offset + 11],offset + 11);
	if(buffer_sec[offset + 11] == 0x0f)
	{
		DirItem->short_long		=	1;
		return getLongDirItem(sector,offset,&(DirItem->longdiritem));
	}
	else
	{
		DirItem->short_long		=	0;
		return getShortDirItem(sector,offset,&(DirItem->shortdiritem));
	}
	return 0;
}

/*************************************************
Function: getRootInfo
Description: 获取及解读根目录扇区信息 
Calls: 
Called By: 

Input: struct MBR MBRinfo, struct ROOT *RootInfo
		需从MBR中获取根目录扇区入口及返回信息的结构体 

Output: 
Return: 返回0
Others: 
*************************************************/
short	getRootInfo(struct MBR MBRinfo,	struct DirItem *DirItem)//, struct ROOT *RootInfo)
{
	//struct DirItem DirItem;
	unsigned short	offset = 0; 
	fillbuffer(MBRinfo.RootEntrance);
	offset += getDirItem(MBRinfo.RootEntrance, 96, DirItem);
	
	
	return 0; 
}

//簇 起始扇区计算
//	= RootEntrance + (N - 2) * SectorPerCluster

