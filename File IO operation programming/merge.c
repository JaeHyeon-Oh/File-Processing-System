#include<stdio.h>
#include<string.h>
#define BUFSIZE 512
int main(int argc,char* argv[]){
	FILE*f1,*f2,*f3;
	char buf[BUFSIZE];
	int size,sizef3; 

	if(argc!=4){
		printf("Condition can't be satisfied\n");
		return 1;
	}
	f1=fopen(argv[1],"w");
	f2=fopen(argv[2],"r");
	f3=fopen(argv[3],"r");
        
	fseek(f2,0,SEEK_END);
        size=ftell(f2);
	fseek(f2,0,SEEK_SET);

	fread(buf,size,1,f2);
	fwrite(buf,size,1,f1);
	memset(buf,0,size+1);
	
	fseek(f3,0,SEEK_END);
        sizef3=ftell(f3);
	fseek(f3,0,SEEK_SET);

	fread(buf,sizef3,1,f3);
        fwrite(buf,sizef3,1,f1);
	memset(buf,0,sizef3+1);

	fclose(f1);
	fclose(f2);
	fclose(f3);
	 
	return 0;
}
