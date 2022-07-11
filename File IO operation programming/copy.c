#include<stdio.h>
#include<string.h>
#define BUFSIZE 512

int main(int argc,char* argv[]){
	char buf[BUFSIZE];
        FILE* f1,*f2;
	if(argc!=3){
		printf("Condition can't be satisfied\n");
		return 0;}

        f1=fopen(argv[1],"r");
	f2=fopen(argv[2],"w");
	if(f1==NULL){
		printf("File can not open\n");
		return 0;}

	while(!feof(f1)){
		fread(buf,10,1,f1);
		fwrite(buf,strlen(buf),1,f2);
		memset(buf,0,10);
	}
	fclose(f1);
	fclose(f2);

	return 0;
}
