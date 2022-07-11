#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<time.h>
#include "record.h"

int main(int argc,char *argv[]){
	Student info[3]={{"20172647","Oh","Texas","The University of Texas at Austin","computer engineering","junior","what's up"}
		,{"20162727","ho","Newyork","The Newyork University","Math","senior","hi"},
		{"20142525","you","Seoul","Seoul University","Physics","sophomore","I'am so hungry"}};
	Student *record;

	int fd;
	int i,j;
	int numOfRecord;
	
	srand((unsigned)time(NULL));
	numOfRecord=atoi(argv[1]);
	record=(Student *)malloc(sizeof(Student)*numOfRecord);

	if(numOfRecord==0){
		printf("Record Number over 0\n");
		exit(1);}

	for(i=0;i<numOfRecord;i++){
		j=rand()%3;
		record[i]=info[j];}
	
	if((fd=open(argv[2],O_RDWR|O_CREAT|O_TRUNC,0644))<0){
		fprintf(stderr,"create error %s\n",argv[2]);
		exit(1);}
	write(fd,&numOfRecord,sizeof(numOfRecord));
	
	srand((unsigned)time(NULL));
	for(i=0;i<numOfRecord;i++){
		write(fd,(char *)&record[i].id,10);
		write(fd,(char *)&record[i].name,30);
		write(fd,(char *)&record[i].address,50);
		write(fd,(char *)&record[i].university,50);
		write(fd,(char *)&record[i].major,20);
		write(fd,(char *)&record[i].grade,20);
		write(fd,(char *)&record[i].intro,70);}
	return 0;
}
