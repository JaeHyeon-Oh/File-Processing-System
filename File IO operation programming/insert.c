#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(int argc,char*argv[]){
        FILE* f=fopen(argv[1],"r+");
        int offset=0,size=0;
        char *data,*buf;

        if(argc!=4){
                printf("Condition can't be satisfied\n");
                return 1;
        }
        offset=atoi(argv[2]);
        data=argv[3];
	fseek(f,offset,SEEK_END);
	size=ftell(f);
	buf=(char *)malloc(size+1);
        
	fseek(f,offset,SEEK_SET);
	fread(buf,size,1,f);
	
        fseek(f,offset,SEEK_SET);
        fwrite(data,strlen(data),1,f);
	fwrite(buf,strlen(buf),1,f);
        fclose(f);
        return 0;
}
	
