#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(int argc,char*argv[]){
        FILE* f=fopen(argv[1],"r+");
	int offset=0,deleteByte=0;
        char *buf1,*buf2;
	int loc=0;
	int sizeFile=0,size=0;
        if(argc!=4){
                printf("Condition can't be satisfied\n");
                return 1;
        }
        offset=atoi(argv[2]);
        deleteByte=atoi(argv[3]);
	loc=offset+deleteByte;
	fseek(f,0,SEEK_END);
	sizeFile=ftell(f);
	
	
	size=sizeFile-deleteByte;
	buf1=(char*)malloc(offset+1);
	buf2=(char*)malloc(loc+1);

        fseek(f,0,SEEK_SET);
        fread(buf1,offset,1,f);
	
        fseek(f,loc,SEEK_SET);
        fread(buf2,size,1,f);
	fclose(f);
	FILE* f1=fopen(argv[1],"w");
	fwrite(buf1,strlen(buf1),1,f);
	fwrite(buf2,strlen(buf2),1,f);
	
	fclose(f1);
        return 0;
}

