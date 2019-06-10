/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: FileSystem.c
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT 实现文件系统 
Others: 
Function List: 

History: 

*************************************************/


#include "base.h"
#include "GDFAT.h"
#include "FileSystem.h"

/*************************************************
Function: LoadDiskPart
Description: 获取分区信息 
Calls: 
Called By: 

Input: struct DPInfo *dpi  

Output: 
Return: 成功返回0;否则返回-1;
Others: 
*************************************************/
short LoadDiskPart(struct DPInfo *dpi)
{
	struct MBR MBRinfo;
 	struct DBR DBRinfo;
 	struct FSINFO FSinfo;

	if(getMBRInfo(&MBRinfo) == -1) return -1;
	dpi->logicsector	=	MBRinfo.DBRSector;
	dpi->totalsector	=	MBRinfo.TotalSector;

	if(getDBRInfo(dpi->logicsector, &DBRinfo) == -1) return -1;
	dpi->sectorsize		=	DBRinfo.BytePerSector;
	dpi->clustersize	=	DBRinfo.SectorPerCluster * DBRinfo.BytePerSector;
	dpi->fatsector		=	DBRinfo.HideSector + DBRinfo.SystemSector;
	dpi->fsinfoSector	=	DBRinfo.FsinfoSector + dpi->logicsector;
	dpi->rootsector		=	dpi->fatsector + DBRinfo.FatCount * DBRinfo.SectorPerFat;
	dpi->SectorPerCluster	=	DBRinfo.SectorPerCluster;
	
	if(getFSInfo(dpi->fsinfoSector, &FSinfo) != 0) return -1;
	dpi->freecluster	=	FSinfo.AllFreeCluster;
	dpi->usedcluster	=	dpi->totalsector /(dpi->clustersize/dpi->sectorsize) - dpi->freecluster;
	dpi->nextfreecluster	=	FSinfo.NextFreeCluster;
	
	FindDpName(dpi);
	return 0;
}

unsigned long getClusterSector(struct DPInfo dpi,unsigned long Cluster)
{
	//簇 起始扇区计算
	//	= RootEntrance + (N - 2) * SectorPerCluster
	return (dpi.rootsector + (Cluster -2)*dpi.SectorPerCluster);
}


short ListRoot(struct DPInfo dpi, struct DirItem *dir)
{
	unsigned long	clusterlist[10];
	unsigned long	i,j;
	unsigned long	roottotalsector	=	1, sector_cur;
//	struct DirItem dir;
	clusterlist[0]	=	2;
	for(i = 1; i < 10; i++)
	{
		clusterlist[i]	=	getNextCluster(dpi, clusterlist[i-1]);
		if(clusterlist[i] < 2) break;
		else roottotalsector++;
	}
	roottotalsector *= dpi.SectorPerCluster;
	
	for(i = 0; i < roottotalsector; i++)
	{
		if(i % (dpi.SectorPerCluster) == 0)sector_cur = getClusterSector(dpi, clusterlist[i]);
		fillbuffer(sector_cur);
		
		for(j = 0; j < 512; )
		{
			j+=getDirItem0(j,dir);
		}
		dir->short_long	=	0;//test
	}
	return 0;
}

void FindMP3Files()
{
	
}

void Path()
{
	
}


short FindDpName(struct DPInfo *dpi)
{
	unsigned char	i,k;
	unsigned short	j;
	unsigned char	get = 0;
//	long sector	=	dpi->rootsector;
	struct ShortDirItem SDirItem;
	for(i = 0; i < 4; i++)
	{
		fillbuffer(dpi->rootsector + i);
		
		for(j = 0; j < 512; j += 32)
		{
			getShortDirItem(j, &SDirItem);
			if(SDirItem.attribute	==	0x08)
			{
				for(k = 0; k < 11; k++)
				{
					dpi->DPname[k]	=	SDirItem.strFilename[k];
				}
				get	=	1;
				break;
			}
		}

		if(get == 1)break;	
	}
	if(get	==	1)return 0;
	for(k = 0; k < 11; k++)
	{
		dpi->DPname[k]	=	'\0';
	}
	return -1;
}


