#ifndef _FOLDER_H_
#define	_FOLDER_H_

#define	SHORTDIR	0
#define	LONGDIR		1

struct DirLinkNode{
	char	short_long;			//���ļ�Ŀ¼��0�����ļ�Ŀ¼��¼���� 
	unsigned char	nametemp[4][20];
	unsigned char	longname[120];


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
	unsigned char	dirflag;		//�ļ��б�ʶ 

	struct DirLinkNode *next;
};

struct DirLink{
	struct DirLinkNode *head;
	long len;
	unsigned long	dircollect;
	unsigned long	filecollect;
};


#endif
