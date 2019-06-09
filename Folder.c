/*************************************************
Copyright (C), 2019-2019, www.xuanmoo.com
File name: Folder.c 
Author:	www.xuanmoo.com
Version:	0.0.1.0
Date: 2019.6.4
Description: GDFAT 文件夹底层驱动
Others: 
Function List: 

History: 

*************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Folder.h"


#ifndef	NULL
#define	NULL	((void *)0)
#endif



/*************************************************
Function: creatdirnode
Description: 建立一个子文件链的节点，并初始化节点信息 
Calls: 
Called By: 

Input: struct DirLinkNode *dnode
Output: 
Return: 成功返回0;否则返回-1; 
Others: 
*************************************************/
struct DirLinkNode* creatdirnode(void)
{
	struct DirLinkNode *p1;
	p1		=	malloc(sizeof(struct DirLinkNode));
	if(p1	==	NULL)return NULL;
	memset((void *)p1,'\0',sizeof(struct DirLinkNode));
//	printf("d node %p\n",p1);
	p1->next			=	NULL;

	return p1;
}

/*************************************************
Function: destroydirnode
Description: 释放一个子文件链节点内存 
Calls: 
Called By: 

Input: struct DirLinkNode *dnode
Output: 
Return: 成功返回0
Others: 
*************************************************/
int destroydirnode(struct DirLinkNode *dnode)
{
	if(dnode	!= NULL)	free(dnode);

	dnode		=	NULL;
	
	return 0;
}

/*************************************************
Function: creatdirlink
Description: 建立一个子文件链，并初始化节点信息 
Calls: 
Called By: 

Input: struct DirLink *dlink
Output: 
Return: 成功返回0;否则返回-1; 
Others: 
*************************************************/
int creatdirlink(struct DirLink *dlink)
{
	dlink->len	=	0;
	dlink->dircollect	=	0;
	dlink->filecollect	=	0;
	
	dlink->head	=	creatdirnode();
//	printf("d link %p\n",dlink->head);
	return 0;
}

/*************************************************
Function: destroydirlink
Description: 销毁一个子文件链
Calls: 
Called By: 

Input: struct DirLink *dlink
Output: 
Return: 成功返回0;出错返回-1;
Others: 
*************************************************/
int destroydirlink(struct DirLink *dlink)
{

	struct DirLinkNode *p1,*p2;
	
	p1 = dlink->head->next;
	
	while(p1 != NULL)
	{
		p2 = p1->next;
		destroydirnode(p1);
		dlink->len--;
		p1 = p2;
	}

	if(dlink->len < 0)return -1;
	
	return 0;
}

//未测试 
int paddingdlinknode(struct DirLink *dlink,	struct DirLinkNode *Lnode)
{
	struct DirLinkNode *p1,*p2;
	p1	=	dlink->head;
	while(p1->next != NULL)p1 = p1->next;

	p1->next	=	Lnode;

	return 0;
}


/*************************************************
Function: folderDirCollect
Description: 目录文件收集器 
Calls: 
Called By: 

Input: struct DPInfo dpi,struct DirLink *dlink,unsigned long dircluster
Output: 
Return: 返回0
Others: 
*************************************************/
int folderDirCollect(struct DPInfo dpi,struct DirLink *dlink,unsigned long dircluster)
{
	
	unsigned short i,j,k;
	unsigned long r = dircluster;
	unsigned long dirstartsector;
	
	if(creatdirlink(dlink) != 0)return -1;

	struct DirLinkNode *p1,*p2;

	p1 = dlink->head;
	
	do
	{
		dirstartsector =  getClusterSector(dpi,r);

		for(i = 0; i < (dpi.SectorPerCluster); i++)
		{
			fillbuffer(dirstartsector + i);

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
					break;//空目录项
					//continue;
				}
				else if(buffer_sec[j + 0xb] == 0x08)
				{
					continue;//分区名
				}
				else	//短目录项
				{
					p2	=	creatdirnode();
					p1->next	=	p2;

					if(p1	==	NULL)
					{
						printf("dir collecter malloc error\n");
						continue;
					}

					p2->short_long	=	0;	//short flag
					for(k = 0; k < 8; k++)
						p2->strFilename[k]	= buffer_sec[j + k];
					for(k = 0; k < 3; k++)
						p2->strExtension[k]	= buffer_sec[j + k +8];
					p2->attribute		=	buffer_sec[j + 0xb];
					p2->millisecond	=	buffer_sec[j + 0xd];
					p2->createTime	=	uc_short(buffer_sec[j + 0xe],buffer_sec[j + 0xf]);
					p2->createDate	=	uc_short(buffer_sec[j + 0x10],buffer_sec[j + 0x11]);
					p2->accessDate	=	uc_short(buffer_sec[j + 0x12],buffer_sec[j + 0x13]);
					p2->highWordCluster	=	uc_short(buffer_sec[j + 0x14],buffer_sec[j + 0x15]);
					p2->updateTime	=	uc_short(buffer_sec[j + 0x16],buffer_sec[j + 0x17]);
					p2->updateDate	=	uc_short(buffer_sec[j + 0x18],buffer_sec[j + 0x19]);
					p2->lowWordCluster	=	uc_short(buffer_sec[j + 0x1a],buffer_sec[j + 0x1b]);
					p2->filesize		=	uc_long(buffer_sec[j + 0x1c],buffer_sec[j + 0x1d],\
										buffer_sec[j + 0x1e],buffer_sec[j + 0x1f]);
					p2->Cluster		=	uc_long(buffer_sec[j + 0x1a],buffer_sec[j + 0x1b],\
										buffer_sec[j + 0x14],buffer_sec[j + 0x15]);

					if(buffer_sec[j + 0xb]&0x18)//目录 
						p2->dirflag	=	1;
					else //文件 
						p2->dirflag	=	0;
					p1 = p1->next;
				} 
				 
			}
		}
	}while((r = getNextCluster(dpi, r))  > 2);
	
	return 0;
}

/*************************************************
Function: folderTraver
Description: ls 
Calls: 
Called By: 

Input: struct DirLink *dlink
Output: 
Return: 返回0
Others: 
*************************************************/
int folderTraver(struct DirLink *dlink)
{
	struct DirLinkNode * p1,*p2;
	
	if(dlink == NULL)return -1;
	
	p1 = dlink->head->next;
	p2 = p1;

	while(p2 != NULL)
	{
		p1 = p2;
		if(p2->short_long)
		{
			continue;//长目录项
		}
		else	//短目录项 
		{

				printf("%.4d\/%.2d\/%.2d  ",(1980 + (p2->createDate>>9&0x7f) ), \
						p2->createDate>>5 & 0xf, p2->createDate&0x1f);
				printf("%.2d:%.2d:%.2d\t",((p2->createTime >>11) &0x1f),\
						p2->createTime>>5&0x3f,(p2->createTime&0x1f)*2 + p2->millisecond/100);
				if(p2->dirflag)//目录 
				{
					printf("<DIR>\t");
					printf("\t%.11s\n",p2->strFilename);
		
				}
				else //文件 
				{
					printf("\t%d\t",p2->filesize);
					printf("%.8s.%3s\n",p2->strFilename,p2->strExtension);
					
				}			
		}

		p2 = p1->next;
	}
	
	return 0;
}

//未完成 
int FindFile(struct DirLink *dlink, const char *fname)
{
	struct DirLinkNode * p1,*p2;
	if(dlink == NULL)return -1;
	p1 = dlink->head->next;
	p2 = p1;
	
	//get extname;
	//get filename; 

	while(p2 != NULL)
	{
		p1 = p2;
		if(p2->short_long)
		{
			continue;//长目录项
		}
		else	//短目录项 
		{
				if(p2->dirflag)//目录 
				{
					continue;
				}
				else //文件 
				{
					//对比扩展名
					
					//对比文件名 
				}			
		}

		p2 = p1->next;
	}
	

	return 0;
}
