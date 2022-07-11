#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include "record.h"


#define SUFFLE_NUM	10000

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv)
{
	int *read_order_list;
	int num_of_records;
	int  i, fd;
	char *fname;
	struct timeval start, end;
	long int dif;
	char buf[250];
	Student info;
	
	if (argc != 2) {
		fprintf(stderr, "Not Satisfied input\n");
		exit(1);}

	fname=argv[1];

	if ((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr,"Open file error for %s\n",fname);
		exit(1);}

	read(fd,&num_of_records,4);
	read_order_list=(int *)malloc(sizeof(int)*num_of_records);

	GenRecordSequence(read_order_list, num_of_records);

	gettimeofday(&start, NULL);

	for (i=0; i<num_of_records; i++) {
		lseek(fd, (read_order_list[i]*250)+4, SEEK_SET);
		read(fd,&info, 250);}

	gettimeofday(&end, NULL);
	dif = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("\n#records: %d elapsed_time: %ld us\n",num_of_records, dif);

	free(read_order_list);
	close(fd);

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;
	srand((unsigned int)time(0));
	
	for(i=0; i<n; i++)
	{
		list[i] = i;
	}

	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}

