#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define BUFSIZE 512

int main(int argc,char* argv[])	{
	char buf[BUFSIZE];
	FILE *f=fopen(argv[1],"r");
	int offset,readByte;
	if(argc!=4){
		printf("Condition can't be satisfied\n");
	        return 1;
	}
	offset=atoi(argv[2]);
	readByte=atoi(argv[3]);
	
	fseek(f,offset,SEEK_SET);
	fread(buf,readByte,1,f);
	printf("%s\n",buf);
	memset(buf,0,BUFSIZE);

	fclose(f);
	return 0;
}
	

