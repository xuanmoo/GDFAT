/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: FileSystem.h
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.5.20
Description: GDFAT �ײ����� ���ݽṹ���������� 
Others: 
Function List: 

History: 

*************************************************/

#ifndef	_FILESYSTEM_H_
#define	_FILESYSTEM_H_


struct MBR{
	long	DBRSector;
	long	TotalSector;
	
	//�������
	long		FATEntrance;			//fat�����
	long		RootEntrance;			//��Ŀ¼���
	long		LogicEntrance;			//�߼�0�������������� 
	
	//����ռ�
	long		AllSize;				//�ܿռ�
	long		DPSize;					//�����ܿռ�
	long		UseSize;				//���ÿռ�
	long		FreeSize;				//ʣ��ռ�
	long		SectorPerRoot;				//��Ŀ¼��С 
	
	//����ϵ��
	long		BytePerCluster;			//ÿ���ش�С byte
	short		SectorPerCluster;		//ÿ��ռ������
	short		BytePerSector;			//ÿ�����ֽ���
	long		DPartTotalSector;		//������������
	short		FsinfoSector;			//FSInfo������
	short		Dbr2Sector;				//DBR����������

};

struct DBR{
	short		BytePerSector;			//11-12	ÿ�����ֽ���
	short		SectorPerCluster;		//13	ÿ��ռ������
	short		SystemSector;			//14-15	����������
	short		FatCount;				//16	FAT����

	long		HideSector;				//28-31	����������
	long		DPartTotalSector;		//32-35	������������
	long		SectorPerFat;			//36-39	����FATռ������
	short		Backup;					//40-41	�Ƿ��� 00����,01�Ǿ���
	long		RootCluset;				//44-47	��Ŀ¼��ʼ��
	short		FsinfoSector;			//48-49	FSInfo������
	short		Dbr2Sector;				//50-51	DBR����������
	short		DpiFlag;				//66	��չ��Ϣ��־ 0x29Ϊ��
	unsigned char	DPName[11];			//71	�����־Ϊ0x29�ɶ� ������� 

}; 

struct FSINFO{
	long		AllFreeCluster;			//ͳ��Ŀǰ���п��õĴ������������ʣ����������
	long		NextFreeCluster;		//��һ�����õĴغš�
};


struct ShortDirItem
{
	unsigned char	strFilename[8];			//�ļ���
	unsigned char	strExtension[3];		//��չ��
	unsigned char	attribute;				//����
	unsigned char	reserved;				//����
	unsigned char	millisecond;			//�������� 10����λ
	unsigned short	createTime;		//����ʱ��
	unsigned short	createDate;		//��������
	unsigned short	accessDate;		//��������
	unsigned short	highWordCluster;//��ʼ�ظ�16λ
	unsigned short	updateTime;		//����ʱ��
	unsigned short	updateDate;		//��������
	unsigned short	lowWordCluster;	//��ʼ�ص�16λ
	unsigned long	filesize;		//�ļ���С
	unsigned long	Cluster;		//��ʼ�غ�
};

struct LongFDT
{
	unsigned char flag;//�����0x4*,��6λ��λ�ˣ�˵�������һ�����ļ�Ŀ¼����λ�����滹�м���
	unsigned char name1[10];
	unsigned char attr;//����ǳ��ļ���������������һ��������0F
	unsigned char reserve;
	unsigned char checksum;
	unsigned char name2[12];
	unsigned char rel_cluster[2];//�����ʼ�غ�
	unsigned char name3[4];
};
struct LongDirItem
{
	struct LongFDT lfdt[4];
}; 

struct DirItem{
	char	short_long; 
	struct ShortDirItem	shortdiritem;
	struct LongDirItem	longdiritem;
	
};



//funsion
short	getMBRInfo(struct MBR *MBRinfo);
short	getDBRInfo(struct MBR *MBRinfo, struct DBR *DBRinfo );
short	getFSInfo(struct MBR *MBRinfo,struct FSINFO *FSinfo);
long	getNextCluster(struct MBR MBRinfo, long CurrentCluster);
short	getShortDirItem(long sector, long offset , struct ShortDirItem * DirItem);
short	getLongDirItem(long sector, long offset, struct LongDirItem *DirItem);
short	getRootInfo(struct MBR MBRinfo, struct DirItem *DirItem);//, struct ROOT *RootInfo);


#endif
