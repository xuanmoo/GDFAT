#ifndef _FOLDER_H_
#define	_FOLDER_H_

#define	SHORTDIR	0
#define	LONGDIR		1

struct DirLinkNode{
	char	short_long;			//短文件目录置0，长文件目录记录项数 
	unsigned char	nametemp[4][20];
	unsigned char	longname[120];


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

	struct DirLinkNode *next;
};

struct DirLink{
	struct DirLinkNode *head;
	long len;
	unsigned long	dircollect;
	unsigned long	filecollect;
};


#endif
