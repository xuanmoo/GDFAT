#include <stdio.h>
#include "base.h"
#include "base.c"
#include "FileSystem.h"
#include "FileSystem.c"

extern	unsigned char	buffer_sec[512];

int main(int argc, char **argv)
{
	int i;
/*
//fillbuffer test
	fillbuffer(0);
	
	for(i = 0; i < 512; i++)
	{
		printf("%.2x\t",buffer_sec[i]);
	}
*/
	struct MBR MBRinfo;
 	struct DBR DBRinfo;
 	struct FSINFO FSinfo;
	struct DirItem DirItem;

	getMBRInfo(&MBRinfo);
	printf("----------------MBR Info----------------\n");
	printf("DBR Sector:\t%d\nTotalSector:\t%d\n",MBRinfo.DBRSector,MBRinfo.TotalSector);
	printf("----------------------------------------\n\n");
	
	getDBRInfo(&MBRinfo, &DBRinfo);
	printf("----------------DBR Info----------------\n");
	printf("BytePerSector:\t\t%d\nSectorPerCluster:\t%d\nSystemSector:\t\t%d\nFatCount:\t\t%d\nHideSector:\t\t%d\nDPartTotalSector:\t%d\nSectorPerFat:\t\t%d\nBackup:\t\t\t%.2x\nRootCluset:\t\t%d\nFsinfoSector:\t\t%d\nDbr2Sector:\t\t%d\nDpiFlag:\t\t%d\n",DBRinfo.BytePerSector,\
			DBRinfo.SectorPerCluster,DBRinfo.SystemSector,\
			DBRinfo.FatCount,DBRinfo.HideSector,DBRinfo.DPartTotalSector,\
			DBRinfo.SectorPerFat,DBRinfo.Backup,DBRinfo.RootCluset,\
			DBRinfo.FsinfoSector,DBRinfo.Dbr2Sector,DBRinfo.DpiFlag);
	printf("DPName:\t\t\t%.11s\n",DBRinfo.DPName);
	printf("********MBR********\n");
	printf("BytePerCluster:\t\t%d\nBytePerSector:\t\t%d\nDPartTotalSector:\t%d\nFATEntrance:\t\t%d\nRootEntrance:\t\t%d\nLogicEntrance:\t\t%d\n",\
			MBRinfo.BytePerCluster,MBRinfo.BytePerSector,\
			MBRinfo.DPartTotalSector,MBRinfo.FATEntrance,MBRinfo.RootEntrance,\
			MBRinfo.LogicEntrance);
	printf("----------------------------------------\n\n");
				
	getFSInfo(&MBRinfo, &FSinfo);
	printf("----------------FSI Info----------------\n");
	printf("AllFreeCluster:\t\t%d\nNextFreeCluster:\t%d\n",FSinfo.AllFreeCluster,FSinfo.NextFreeCluster);
	printf("AllSize:\t\t%d\nDPSize:\t\t\t%d\nFreeSize:\t\t%d\nUseSize:\t\t%d\n",\
			MBRinfo.AllSize,MBRinfo.DPSize,MBRinfo.FreeSize,MBRinfo.UseSize);
	
	printf("AllSize:\t\t%dmb\nAllSize:\t\t%.2fgb\n",MBRinfo.AllSize/1024,(float)MBRinfo.AllSize/(1024*1024));
	printf("----------------------------------------\n\n");
	
	printf("----------------NXT Info----------------\n");
	printf("NextCluster:\t\t%ld\n",getNextCluster(MBRinfo,4));
	printf("----------------------------------------\n\n");

	int attributeall	=	1;
	getRootInfo( MBRinfo,&DirItem);
	printf("----------------DIR Info----------------\n");
	if(DirItem.short_long	==	0)	//短文件
	{
		printf("%d\n",DirItem.short_long);
		if(DirItem.shortdiritem.strExtension[0] == 0x44 && DirItem.shortdiritem.strExtension[1] == 0x49 && DirItem.shortdiritem.strExtension[2] == 0x52) 
			printf("Dirname:\t\t%.8s\n",DirItem.shortdiritem.strFilename);
		else
		{
			printf("Filename:\t\t%.8s.%.3s\n",DirItem.shortdiritem.strFilename,DirItem.shortdiritem.strExtension);
		}
		printf("attribute:\t\t%x\n",DirItem.shortdiritem.attribute);
		if(attributeall == 1)
		{
			printf("\tread_write:\t%s\n",((DirItem.shortdiritem.attribute &0x1) == 0x1 )? "yes" : "no");
			printf("\tread_only:\t%s\n",((DirItem.shortdiritem.attribute &0x2) == 0x2 )? "yes" : "no");
			printf("\thide:\t\t%s\n",((DirItem.shortdiritem.attribute &0x4) == 0x4 )? "yes" : "no");
			printf("\tsystem:\t\t%s\n",((DirItem.shortdiritem.attribute &0x8) == 0x8 )? "yes" : "no");
			printf("\tlable:\t\t%s\n",((DirItem.shortdiritem.attribute &0x10) == 0x10 )? "yes" : "no");
			printf("\tfile:\t\t%s\n",((DirItem.shortdiritem.attribute &0x20) == 0x20 )? "yes" : "no");
		}
		printf("millisecond:\t\t%d\n",DirItem.shortdiritem.millisecond*10);
		printf("createTime:\t\t%d:%d:%d\n",((DirItem.shortdiritem.createTime >> 11) &0x1f),((DirItem.shortdiritem.createTime >> 5) &0x3f),((DirItem.shortdiritem.createTime >> 0) &0x1f)*2);
		printf("createDate:\t\t%d-%d-%d\n",((DirItem.shortdiritem.createDate >> 9) &0x3f)+1980,((DirItem.shortdiritem.createDate >> 5) &0xf),((DirItem.shortdiritem.createDate >> 0) &0x1f));
		printf("accessDate:\t\t%d-%d-%d\n",((DirItem.shortdiritem.accessDate >> 9) &0x3f)+1980,((DirItem.shortdiritem.accessDate >> 5) &0xf),((DirItem.shortdiritem.accessDate >> 0) &0x1f));
		printf("updateTime:\t\t%d:%d:%d\n",((DirItem.shortdiritem.updateTime >> 11) &0x1f),((DirItem.shortdiritem.updateTime >> 5) &0x3f),((DirItem.shortdiritem.updateTime >> 0) &0x1f)*2);
		printf("updateDate:\t\t%d-%d-%d\n",((DirItem.shortdiritem.updateDate >> 9) &0x3f)+1980,((DirItem.shortdiritem.updateDate >> 5) &0xf),((DirItem.shortdiritem.updateDate >> 0) &0x1f));
		printf("filesize:\t\t%d\n",DirItem.shortdiritem.filesize);
		printf("Cluster:\t\t%d\n",DirItem.shortdiritem.Cluster);
		printf("lowWordCluster:\t\t%d\n",DirItem.shortdiritem.lowWordCluster);
	}
	else	//长文件 
	{
		
	}

	printf("----------------------------------------\n\n");
	return 0;
}
