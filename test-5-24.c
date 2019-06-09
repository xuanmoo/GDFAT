#include <stdio.h>
#include "base.h"
#include "base.c"
#include "GDFAT.h"
#include "GDFAT.c"
#include "FileSystem.h"
#include "FileSystem.c"
#include "Folder.h"
#include "Folder.c"

extern	unsigned char	buffer_sec[512];

int main(int argc, char **argv)
{
	int i,j,r;
	struct DPInfo dpi;
	struct DirItem dir;
	struct DirLink folder;
	if(LoadDiskDart(&dpi) != 0)printf("load dp error.\n");
	
	printf("----------------DP Info--------\n");
	printf("logicsector:\t%ld\n",	dpi.logicsector);
	
	printf("totalsector:\t%ld\n",	dpi.totalsector);
	printf("sectorsize:\t%ld\n",	dpi.sectorsize);
	printf("clustersize:\t%ld\n",	dpi.clustersize);
	printf("fatsector:\t%ld\n",		dpi.fatsector);
	printf("rootsector:\t%ld\n",	dpi.rootsector);
	printf("freecluster:\t%ld\n",	dpi.freecluster);
	printf("usedcluster:\t%ld\n",	dpi.usedcluster);
	printf("fsinfoSector:\t%ld\n",	dpi.fsinfoSector);
	printf("nextfreeclus:\t%ld\n",	dpi.nextfreecluster);
	printf("-------------------------------\n\n");
	
	FindDpName(&dpi);
	printf("----------------get dpname-----\n");
	if(dpi.DPname[0] == 0x0)
		printf("dpname:\t%s\n","NoName");
	else
		printf("dpname:\t%.11s\n",	dpi.DPname);
	printf("-------------------------------\n\n");
	
	printf("----------------get next-------\n");
	printf("rootnxcluster:\t%ld\n",	getNextCluster(dpi, 93));
	printf("root sector:\t%ld\n",	getClusterSector(dpi,2));
	printf("-------------------------------\n\n");
	

	printf("----------------dir list-------\n");
	ListDir(dpi,2);
	printf("-------------------------------\n\n");

	

	folderDirCollect(dpi,&folder,46);
	folderTraver(&folder);
	
	
	
	
	return 0; 	
};
