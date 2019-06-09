/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: FileSystem.h
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT ʵ�ֲ� ���ݽṹ����������
Others: 
Function List: 

History: 

*************************************************/

#ifndef	_FILESYSTEM_H_
#define	_FILESYSTEM_H_
#include "GDFAT.h"
#include "FileSystem.h"

struct DPInfo{
	unsigned long	logicsector;		//�߼�0������Ӧ��������,��DBR����
	unsigned long	totalsector;		//������������
	unsigned long	usedcluster;		//���ô�
	unsigned long	freecluster;		//ʣ���
	unsigned long	sectorsize;			//������С
	unsigned long	clustersize;		//�ش�С
	unsigned long	SectorPerCluster;		//ÿ��ռ������
	unsigned long	fatsector;			//FAT����
	unsigned long	rootsector;			//��Ŀ¼����
	unsigned char	DPname[12];			//���
	
	unsigned long	fsinfoSector;			//FSInfo������
	unsigned long	dbr2Sector;				//DBR����������
	unsigned long	nextfreecluster;		//��һ�����õĴغš�
};



//FUN
short LoadDiskDart(struct DPInfo *dpi);
short FindDpName(struct DPInfo *dpi);
unsigned long getClusterSector(struct DPInfo dpi,unsigned long Cluster);
//short ListRoot(struct DPInfo dpi, struct DirItem *dir);
#endif
