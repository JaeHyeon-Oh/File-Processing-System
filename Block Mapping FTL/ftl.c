
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmap.h"

int dd_read(int, char*);
int dd_write(int, char*);
int dd_erase(int);
void print_blkmaptbl();

extern FILE *flashfp;


int sparebuf[PAGES_PER_BLOCK * BLOCKS_PER_DEVICE];
char freeblock; 


const int lsn_count = PAGES_PER_BLOCK * DATABLKS_PER_DEVICE; //lsn 갯수 
const int psn_count = PAGES_PER_BLOCK * DATABLKS_PER_DEVICE; //psn 갯수 
const int lbn_count = DATABLKS_PER_DEVICE; //lbn의 갯수
const int pbn_count = BLOCKS_PER_DEVICE; //pbn의 갯수

int lbn_table[DATABLKS_PER_DEVICE]; 

int lbn; 
int pbn;
int ppn;
int offset;

struct mapping{
    int lbn;
    int pbn;
};

struct mapping mappingTable[DATABLKS_PER_DEVICE];


void ftl_open()
{
    freeblock = DATABLKS_PER_DEVICE; 
	for(int i = 0 ; i < lbn_count ; i++){
		mappingTable[i].pbn = -1; 
		
	}
	for(int ppn = 0 ; ppn < (PAGES_PER_BLOCK * BLOCKS_PER_DEVICE) ; ppn++) {
		sparebuf[ppn] = -1; 
	}

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE]; 
	lbn = lsn / PAGES_PER_BLOCK; 
	pbn = mappingTable[lbn].pbn; 
	offset = (lsn % PAGES_PER_BLOCK); 
	ppn = offset + (PAGES_PER_BLOCK * pbn) ; 
	
	printf("lbn pbn\n");
	dd_read(ppn, pagebuf); 
	memcpy(sectorbuf, pagebuf, SECTOR_SIZE); 

	print_blkmaptbl();
	return;
}



void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	lbn = lsn / PAGES_PER_BLOCK; 
	 if(mappingTable[lbn].pbn == -1 )mappingTable[lbn].pbn = lbn;
	pbn = mappingTable[lbn].pbn; 
	offset = lsn % PAGES_PER_BLOCK; 
	ppn = offset + PAGES_PER_BLOCK * pbn ; 

	
	
	dd_read(ppn, pagebuf); 


			if(pagebuf[SECTOR_SIZE] ==-1 ) { 
				
				memcpy(pagebuf,sectorbuf, SECTOR_SIZE);		
				pagebuf[SECTOR_SIZE] = lbn; 
				pagebuf[SECTOR_SIZE+4]=lsn;
				sparebuf[ppn] = lbn;
				dd_write(ppn, pagebuf);
				memset(pagebuf,0,PAGE_SIZE); 
			}

			else{ 
				for(int i=0; i < PAGES_PER_BLOCK; i++) { 
					if(i == offset){ 
						memcpy(pagebuf,sectorbuf, SECTOR_SIZE); 
						pagebuf[SECTOR_SIZE] = 0; 
						sparebuf[PAGES_PER_BLOCK * freeblock + i] = lbn;  
						dd_write(PAGES_PER_BLOCK * freeblock + i, pagebuf);
						sparebuf[pbn * PAGES_PER_BLOCK + i] = -1; 
						
					}
					
					else {					
						dd_read(pbn * PAGES_PER_BLOCK + i, pagebuf); 
						dd_write(PAGES_PER_BLOCK * freeblock + i, pagebuf); 
						sparebuf[PAGES_PER_BLOCK * freeblock + i] = sparebuf[pbn * PAGES_PER_BLOCK + i]; 
						sparebuf[pbn * PAGES_PER_BLOCK + i] = -1; 
						
					}

				}

				dd_erase(mappingTable[lbn].pbn); 
				mappingTable[lbn].pbn = freeblock; 
				freeblock = pbn; 
			
			}
	
	
	return;
}
void print_blkmaptbl()
{
    int i;

    for(i = 0; i < DATABLKS_PER_DEVICE; i++)
			printf("%3d %3d \n", i, mappingTable[i].pbn );
	printf("free block’s pbn=%d\n",freeblock);
}
