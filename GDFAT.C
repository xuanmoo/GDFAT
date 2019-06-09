/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: GDFAT.c
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT 底层驱动 
Others: 
Function List: 

History: 

*************************************************/

#include "base.h"
#include "GDFAT.h"
#include "FileSystem.h"
#include <string.h>


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

extern long blocksize;
/*************************************************
Function: getDBRInfo
Description: 获取DBR扇区数据,解码与文件系统相关的信息 
Calls: 
Called By: 

Input: unsigned long DBRSector, struct DBR *DBRinfo

Output: 
Return: 成功返回0;否则返回-1; 
Others: 
*************************************************/
short getDBRInfo(unsigned long DBRSector, struct DBR *DBRinfo )
{
		unsigned char	i;
		fillbuffer(DBRSector);
		if(buffer_sec[510] != 0x55 || buffer_sec[511] != 0xaa)
		{
			fillbuffer(DBRSector<<9);
			if(buffer_sec[510] == 0x55 && buffer_sec[511] == 0xaa)blocksize = 512;
		}

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

			return 0;
		}

	return -1; 
}

/*************************************************
Function: getFSInfo
Description: 获取FSInfo扇区 簇总数\剩余簇\下一个可用簇等 
Calls: 
Called By: 

Input: unsigned long FsinfoSector,struct FSINFO *FSinfo

Output: 
Return: 成功返回0;否则返回-1或-2; 
Others: 
*************************************************/
short getFSInfo(unsigned long FsinfoSector,struct FSINFO *FSinfo)
{
	fillbuffer(FsinfoSector);
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

Input: struct DPInfo dpi, long CurrentCluster
		需从读取FAT扇区入口,需要当前簇号

Output: 
Return: 若文件有下一个簇,将返回下一个簇号
		若文件无下一个簇,返回-1;
		若该簇可用,返回0;保留簇返回-3;坏簇返回-2;
Others: 
*************************************************/
long getNextCluster(struct DPInfo dpi, long CurrentCluster)
{
	long fatoffset,value;//fatsector,
	
	fatoffset	=	(CurrentCluster % (dpi.sectorsize / 4)) * 4;
//	fatsector	=	CurrentCluster * 4 / dpi.sectorsize;

	fillfatbuffer(dpi.fatsector);
	value		=	uc_long(buffer_fat[fatoffset],buffer_fat[fatoffset + 1],\
					buffer_fat[fatoffset + 2],buffer_fat[fatoffset + 3]);
	
	switch(value)
	{
		case	0x0:
					return  0;	//可用簇
//					break;
		case	0x1:
					return	-3;	//保留簇
//					break;
		case	0x0FFFFFF0:
		case	0x0FFFFFF1:
		case	0x0FFFFFF2:
		case	0x0FFFFFF3:
		case	0x0FFFFFF4:
		case	0x0FFFFFF5:
		case	0x0FFFFFF6:
					return -3;	//保留簇
//					break;
		case	0x0FFFFFF7:
					return	-2;	//坏簇
//					break;
		case	0x0FFFFFF8:
		case	0x0FFFFFF9:
		case	0x0FFFFFFA:
		case	0x0FFFFFFB:
		case	0x0FFFFFFC:
		case	0x0FFFFFFD:
		case	0x0FFFFFFE:
		case	0x0FFFFFFF:
					return	-1;	//最后一个簇
//					break;
		default:
					return	value;
//					break;
	}
}


/*************************************************
Function: getShortDirItem
Description: 获取一个短文件名的目录或文件项,即某个
			文件或目录的相关属性 
Calls: 
Called By: 

Input: long offset , struct ShortDirItem * SDirItem
		需偏移地址,以及返回短文件名的结构体 

Output: 
Return: 返回32 短文件名项占用32字节 
Others: 
*************************************************/
short	getShortDirItem(long offset , struct ShortDirItem * SDirItem)
{
	unsigned char	i;
	for(i = 0; i < 8; i++)
	{
		if(buffer_sec[offset + i] == 0x0)break;
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

Input: long offset , struct LongDirItem * LDirItem
		需要扇区号及偏移地址,以及返回长文件名的结构体 

Output: 
Return: 返回0
Others: 
*************************************************/
unsigned char	buffer_sec2[512];		//若文件项延续至下一扇区 
short	getLongDirItem(long offset, struct LongDirItem *LDirItem)
{
	unsigned char	i,j,k=0;
	char	unicode_end	=	1;

	LDirItem->num						=	(buffer_sec[offset]&0x1f);
	
	for(i = 0; i < 4; i++)
	{
		LDirItem->lfdt[i].flag			=	buffer_sec[offset + (i * 32)];
		
		for(j = 0; j < 10; j+=2)
		{
			if(!unicode_end) break;
			if(buffer_sec[(offset + (i * 32)) + 1 + j] == 0xff) unicode_end--;
			if(!unicode_end) break;

			LDirItem->longname[k++]	=	buffer_sec[(offset + (i * 32)) + 1 + j];
			LDirItem->longname[k++]	=	buffer_sec[(offset + (i * 32)) + 2 + j];
		}
		
		LDirItem->lfdt[i].checksum		=	buffer_sec[offset + (i * 32) + 0xd];

		for(j = 0; j < 12; j+=2)
		{
			if(!unicode_end) break;
			if(buffer_sec[(offset + (i * 32)) + 0xe + j] == 0xff) unicode_end--;
			if(!unicode_end) break;
			LDirItem->longname[k++]	=	buffer_sec[(offset + (i * 32)) + 0xe + j];
			LDirItem->longname[k++]	=	buffer_sec[(offset + (i * 32)) + 0xe + j+1];
		}
		
		LDirItem->lfdt[i].rel_cluster[0]=	buffer_sec[offset + (i * 32) + 0x1a];
		LDirItem->lfdt[i].rel_cluster[1]=	buffer_sec[offset + (i * 32) + 0x1b];
		
		for(j = 0; j < 4; j+=2)
		{
			if(!unicode_end) break;
			if(buffer_sec[(offset + (i * 32)) + 0x1c + j] == 0xff) unicode_end--;
			if(!unicode_end) break;
			LDirItem->longname[k++]	=	buffer_sec[(offset + (i * 32)) + 0x1c + j];
			LDirItem->longname[k++] =	buffer_sec[(offset + (i * 32)) + 0x1c + j +1];
		}
		
		if((LDirItem->lfdt[i].flag & 0x40) == 0x40 )break;
		
	}


	if((LDirItem->lfdt[LDirItem->num-1].flag & 0x40) == 0x40)
	{
		getShortDirItem((offset +(LDirItem->num)*32),&(LDirItem->sfdt));
	}
	
	return ((LDirItem->num+1) * 32);
}


/*************************************************
Function: getDirItem0
Description: 获取一个目录或文件项,即某个
			文件或目录的相关属性 
Calls: 
Called By: 

Input: long offset , struct DirItem * DirItem
		需要偏移地址,以及返回目录或文件结构体

Output: 
Return: 返回0
Others: 
*************************************************/
short	getDirItem0(long offset, struct DirItem *DirItem)
{
	/*长文件名项对应的文件被删除,则顺序字节被置为0xe5*/
	if( buffer_sec[offset] == 0xe5 && buffer_sec[offset + 11] == 0x0f)
	{
		return 32;
	}
	else if( buffer_sec[offset + 11] == 0x0f)
	{
		DirItem->short_long		=	1;
		return getLongDirItem(offset,&(DirItem->longdiritem));
	}
	else
	{
		DirItem->short_long		=	0;
		return getShortDirItem(offset,&(DirItem->shortdiritem));
	}
//	return 0;
}

/*************************************************
Function: getRootInfo
Description: 获取及解读根目录扇区信息 
Calls: 
Called By: 

Input: unsigned long RootEntrance,	struct DirItem *DirItem
		需获取根目录扇区入口及返回信息的结构体 

Output: 
Return: 返回0
Others: 
*************************************************/
short	getRootInfo(unsigned long RootEntrance,	struct DirItem *DirItem)//, struct ROOT *RootInfo)
{
	//struct DirItem DirItem;
	unsigned short	offset = 0;
	fillbuffer(RootEntrance);
	offset += getDirItem0( 32 * 1, DirItem);
	return 0;
}


/*************************************************
Function: ListDir
Description: 列目录清单 
Calls: 
Called By: 

Input: struct DPInfo dpi, unsigned long DirCluster
		

Output: 
Return: 返回0
Others: 
*************************************************/
short ListDir(struct DPInfo dpi, unsigned long DirCluster)
{
	unsigned long cur;
	unsigned short data;
	unsigned short time;
	unsigned long fsize,sondircluster;
	unsigned short i,j;
	unsigned long r = DirCluster;
	do
	{
		cur =  getClusterSector(dpi,r);
		printf("\n");
		for(i = 0; i < (dpi.SectorPerCluster); i++)
		{
			fillbuffer(cur + i);

			for(j = 0; j < 512; j+=32)
			{
				if(buffer_sec[j + 0xb] == 0xf)
				{
					if( buffer_sec[j] == 0xe5)continue;//长目录项删除
					else
					{
						continue;//长目录项
					}
				}
				else if(buffer_sec[j + 0xb] == 0x0)
				{
					continue;//空目录项
				}
				else if(buffer_sec[j + 0xb] == 0x08)
				{
					continue;//分区名
				}
				else	//短目录项 
				{
					data	=	uc_short(buffer_sec[j + 0x10],buffer_sec[j + 0x11]);
					time	=	uc_short(buffer_sec[j + 0xe],buffer_sec[j + 0xf]);
					fsize	=	uc_long(buffer_sec[j + 0x1c],buffer_sec[j + 0x1d],buffer_sec[j + 0x1e],buffer_sec[j + 0x1f]);
					sondircluster	=	uc_long(buffer_sec[j + 0x1a],buffer_sec[j + 0x1b],\
										buffer_sec[j + 0x14],buffer_sec[j + 0x15]);
					printf("%.4d\/%.2d\/%.2d  ",(1980 + (data>>9&0x7f) ), data>>5 & 0xf, data&0x1f);
					printf("%.2d:%.2d\t",((time >>11) &0x1f),time>>5&0x3f,(time&0x1f)*2 + buffer_sec[j + 0xd]/100);
					if(buffer_sec[j + 0xb]&0x18)//目录 
					{
						printf("<DIR>\t");
						printf("\t%.11s\n",buffer_sec + j);

					}
					else //文件 
					{
						printf("\t%d\t",fsize);
						printf("%.8s.%3s\n",buffer_sec + j,buffer_sec + j+8);
						
					}
				} 
				 
			}
		}
	}while((r = getNextCluster(dpi, r))  > 2);
	return 0; 
}

/*************************************************
Function: getDirItem
Description: 获取目录项 
Calls: 
Called By: 

Input: struct DPInfo dpi, unsigned long DirCluster
		

Output: 
Return: 返回0
Others: 
*************************************************/
short getDirItem(struct DPInfo dpi, unsigned long DirCluster)
{
	unsigned long cur;
	unsigned short data;
	unsigned short time;
	unsigned long fsize,sondircluster;
	unsigned short i,j;
	unsigned long r = DirCluster;
	do
	{
		cur =  getClusterSector(dpi,r);
		printf("\n");
		for(i = 0; i < (dpi.SectorPerCluster); i++)
		{
			fillbuffer(cur + i);

			for(j = 0; j < 512; j+=32)
			{
				if(buffer_sec[j + 0xb] == 0xf)
				{
					if( buffer_sec[j] == 0xe5)continue;//长目录项删除
					else
					{
						continue;//长目录项
					}
				}
				else if(buffer_sec[j + 0xb] == 0x0)
				{
					continue;//空目录项
				}
				else if(buffer_sec[j + 0xb] == 0x08)
				{
					continue;//分区名
				}
				else	//短目录项 
				{
					data	=	uc_short(buffer_sec[j + 0x10],buffer_sec[j + 0x11]);
					time	=	uc_short(buffer_sec[j + 0xe],buffer_sec[j + 0xf]);
					fsize	=	uc_long(buffer_sec[j + 0x1c],buffer_sec[j + 0x1d],buffer_sec[j + 0x1e],buffer_sec[j + 0x1f]);
					sondircluster	=	uc_long(buffer_sec[j + 0x1a],buffer_sec[j + 0x1b],\
										buffer_sec[j + 0x14],buffer_sec[j + 0x15]);
					printf("%.4d\/%.2d\/%.2d  ",(1980 + (data>>9&0x7f) ), data>>5 & 0xf, data&0x1f);
					printf("%.2d:%.2d\t",((time >>11) &0x1f),time>>5&0x3f,(time&0x1f)*2 + buffer_sec[j + 0xd]/100);
					if(buffer_sec[j + 0xb]&0x18)//目录 
					{
						printf("<DIR>\t");
						printf("\t%.11s\n",buffer_sec + j);

					}
					else //文件 
					{
						printf("\t%d\t",fsize);
						printf("%.8s.%3s\n",buffer_sec + j,buffer_sec + j+8);
						
					}
				} 
				 
			}
		}
	}while((r = getNextCluster(dpi, r))  > 2);
	return 0;
}



