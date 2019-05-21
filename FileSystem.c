/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: FileSystem.c
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT �ײ����� 
Others: 
Function List: 

History: 

*************************************************/

#include "base.h"
#include "FileSystem.h"


extern	unsigned char	buffer_sec[512];


/*************************************************
Function: getMBRInfo
Description: ��ȡMBR��������,�������������Ҫ���� 
Calls: 
Called By: 

Input: struct MBR *MBRinfo �뷵�ص�DBR������ڼ�������(�������������ͱ�������) 
Output: 
Return: �ɹ�����0;���򷵻�-1; 
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
Description: ��ȡDBR��������,�������ļ�ϵͳ��ص���Ϣ 
Calls: 
Called By: 

Input: struct MBR *MBRinfo, struct DBR *DBRinfo ��Ҫ���벿����Ϣ��
		DBR����Ҫ��Ϣ����MBR�ṹ����,��������������� 
Output: 
Return: �ɹ�����0;���򷵻�-1; 
Others: 
*************************************************/
short getDBRInfo(struct MBR *MBRinfo, struct DBR *DBRinfo )
{
		unsigned char	i; 
		fillbuffer(MBRinfo->DBRSector);
		if(buffer_sec[510] == 0x55 && buffer_sec[511] == 0xaa)
		{
		
			DBRinfo->BytePerSector		=	uc_short(buffer_sec[11],buffer_sec[12]);	//11-12	ÿ�����ֽ���
			DBRinfo->SectorPerCluster	=	(short)buffer_sec[13];						//13	ÿ��ռ������
			DBRinfo->SystemSector		=	uc_short(buffer_sec[14],buffer_sec[15]);	//14-15	����������
			DBRinfo->FatCount			=	(short)buffer_sec[16];						//16	FAT����

			DBRinfo->HideSector			=	uc_long(buffer_sec[28],buffer_sec[29],\
											buffer_sec[30],buffer_sec[31]);				//28-31	����������
			DBRinfo->DPartTotalSector	=	uc_long(buffer_sec[32],buffer_sec[33],\
											buffer_sec[34],buffer_sec[35]);				//32-35	������������
			DBRinfo->SectorPerFat		=	uc_long(buffer_sec[36],buffer_sec[37],\
											buffer_sec[38],buffer_sec[39]);				//36-39	����FATռ������
			DBRinfo->Backup				=	uc_short(buffer_sec[40],buffer_sec[41]);	//40-41	�Ƿ��� 00����,01�Ǿ���
			DBRinfo->RootCluset			=	uc_long(buffer_sec[44],buffer_sec[45],\
											buffer_sec[46],buffer_sec[47]);				//44-47	��Ŀ¼��ʼ��
			DBRinfo->FsinfoSector		=	uc_short(buffer_sec[48],buffer_sec[49]);	//48-49	FSInfo������
			DBRinfo->Dbr2Sector			=	uc_short(buffer_sec[50],buffer_sec[51]);	//50-51	DBR����������

			if(buffer_sec[66]==0x29)
			{
				DBRinfo->DpiFlag	=	1;
				for(i = 0; i < 11; i++)
				{
					DBRinfo->DPName[i]	=	buffer_sec[71+i]; 	//71	�����־Ϊ0x29�ɶ� ������� 
				}
			}
			else DBRinfo->DpiFlag	=	0;
			
			
			//��Ҫ��Ϣ����
			MBRinfo->BytePerCluster		=	DBRinfo->BytePerSector * DBRinfo->SectorPerCluster;			//ÿ���ش�С byte
			MBRinfo->SectorPerCluster	=	DBRinfo->SectorPerCluster;
			MBRinfo->BytePerSector		=	DBRinfo->BytePerSector;
			MBRinfo->DPartTotalSector	=	DBRinfo->DPartTotalSector;
			
			MBRinfo->FATEntrance		=	DBRinfo->HideSector + DBRinfo->SystemSector;			//fat�����
			MBRinfo->RootEntrance		=	MBRinfo->FATEntrance + DBRinfo->FatCount * DBRinfo->SectorPerFat;			//��Ŀ¼���
			MBRinfo->LogicEntrance		=	MBRinfo->DBRSector;
			MBRinfo->FsinfoSector		=	DBRinfo->FsinfoSector;
			MBRinfo->Dbr2Sector			=	DBRinfo->Dbr2Sector;
			return 0;
		}
	return -1; 
}

/*************************************************
Function: getFSInfo
Description: ��ȡFSInfo���� ������\ʣ���\��һ�����ôص� 
Calls: 
Called By: 

Input: struct MBR *MBRinfo,struct FSINFO *FSinfo ���MBR
		�ж�ȡ��DBR��ת�����ںͽ����㲿����ֵ�浽MBR�ṹ���� 

Output: 
Return: �ɹ�����0;���򷵻�-1��-2; 
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
		
			//��Ҫ��Ϣ����
			MBRinfo->AllSize		=	MBRinfo->TotalSector/1024 * MBRinfo->BytePerSector;
			MBRinfo->DPSize			=	MBRinfo->DPartTotalSector/1024 * MBRinfo->BytePerSector;
			MBRinfo->FreeSize		=	FSinfo->AllFreeCluster/1024 * MBRinfo->BytePerCluster;
			MBRinfo->UseSize		=	MBRinfo->DPSize - MBRinfo->FreeSize;
			return 0; 
		}
		else	return -1;	//fsinfo��־ʶ�𲻵� 
		
	}
	
	return -2;		//������־ʶ�𲻵� 
}

/*************************************************
Function: getNextCluster
Description: ��ȡFAT����,��ȡһ����ָ�����һ���� 
			����ĳ���ص�״̬,���û�ռ�û򻵴� 
Calls: 
Called By: 

Input: struct MBR MBRinfo, long CurrentCluster
		���MBR�ж�ȡFAT�������,��Ҫ��ǰ�غ� 

Output: 
Return: ���ļ�����һ����,��������һ���غ�
		���ļ�����һ����,����-1;
		���ôؿ���,����0;�����ط���-3;���ط���-2;
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
					return  0;	//���ô�
					break;
		case	0x1:
					return	-3;	//������
					break;
		case	0x0FFFFFF0:
		case	0x0FFFFFF1:
		case	0x0FFFFFF2:
		case	0x0FFFFFF3:
		case	0x0FFFFFF4:
		case	0x0FFFFFF5:
		case	0x0FFFFFF6:
					return -3;	//������
					break;
		case	0x0FFFFFF7:
					return	-2;	//����
					break;
		case	0x0FFFFFF8:
		case	0x0FFFFFF9:
		case	0x0FFFFFFA:
		case	0x0FFFFFFB:
		case	0x0FFFFFFC:
		case	0x0FFFFFFD:
		case	0x0FFFFFFE:
		case	0x0FFFFFFF:
					return	-1;	//���һ����
					break;
		default:
					return	value;
					break;
	}
}


/*************************************************
Function: getShortDirItem
Description: ��ȡһ�����ļ�����Ŀ¼���ļ���,��ĳ��
			�ļ���Ŀ¼��������� 
Calls: 
Called By: 

Input: long sector, long offset , struct ShortDirItem * SDirItem
		��Ҫ�����ż�ƫ�Ƶ�ַ,�Լ����ض��ļ����Ľṹ�� 

Output: 
Return: ����32 ���ļ�����ռ��32�ֽ� 
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
	return 32;		//��һ����λ�� 
}

/*************************************************
Function: getLongDirItem
Description: ��ȡһ�����ļ�����Ŀ¼���ļ���,��ĳ��
			�ļ���Ŀ¼��������� 
Calls: 
Called By: 

Input: long sector, long offset , struct LongDirItem * LDirItem
		��Ҫ�����ż�ƫ�Ƶ�ַ,�Լ����س��ļ����Ľṹ�� 

Output: 
Return: ����0
Others: 
*************************************************/
unsigned char	buffer_sec2[512];		//���ļ�����������һ���� 
short	getLongDirItem(long sector, long offset, struct LongDirItem *LDirItem)
{
	
	return 0;
}


/*************************************************
Function: getDirItem
Description: ��ȡһ��Ŀ¼���ļ���,��ĳ��
			�ļ���Ŀ¼��������� 
Calls: 
Called By: 

Input: long sector, long offset , struct DirItem * DirItem
		��Ҫ�����ż�ƫ�Ƶ�ַ,�Լ�����Ŀ¼���ļ��ṹ�� 

Output: 
Return: ����0
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
Description: ��ȡ�������Ŀ¼������Ϣ 
Calls: 
Called By: 

Input: struct MBR MBRinfo, struct ROOT *RootInfo
		���MBR�л�ȡ��Ŀ¼������ڼ�������Ϣ�Ľṹ�� 

Output: 
Return: ����0
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

//�� ��ʼ��������
//	= RootEntrance + (N - 2) * SectorPerCluster

