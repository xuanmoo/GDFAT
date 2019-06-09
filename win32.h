#ifdef	WIN32
void ReadSector(long sector, unsigned char * buff)//pc test need
{
	int	offset	=	sector	* 512;
	FILE *fp	=	fopen("SD","rb");
	
	fseek(fp,offset,SEEK_SET);
	fread((void *)buff, 1, 512, fp);
	fclose(fp);
}
#endif
