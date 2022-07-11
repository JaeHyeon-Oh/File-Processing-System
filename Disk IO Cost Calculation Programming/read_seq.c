#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include "record.h"


int main(int argc, char **argv)
{
	struct timeval start, end;
	long int diff;
	int fd; 
	char *fname;
	int  i;
	int num_of_records;
	char buf[250];
	Student info;

	if (argc != 2) {
		fprintf(stderr, "Not satisfied condition\n");
		exit(1);	}

	fname = argv[1];


	if ((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);}
	read(fd,&num_of_records,4);

	gettimeofday(&start, NULL);

	for (i=0; i<num_of_records; i++) {
		read(fd, &info, 250);}	
	gettimeofday(&end, NULL);

	diff = 1000000*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
	printf("\n#records: %d elapsed_time: %lu us\n",num_of_records, diff);

	return 0;
}

