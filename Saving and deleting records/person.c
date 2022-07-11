#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum) {
	fseek(fp,pagenum* PAGE_SIZE+16,SEEK_SET); 
	fread(pagebuf,PAGE_SIZE,1,fp);
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum) {
	fseek(fp,pagenum* PAGE_SIZE+16 ,SEEK_SET);
	fwrite(pagebuf,PAGE_SIZE,1,fp);
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char *recordbuf, const Person *p){
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#",
			p->id,
			p->name,
			p->age,
			p->addr,
			p->phone,
			p->email);
}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(const char *recordbuf, Person *p) {
	sscanf(recordbuf, "%[^#]%[^#]%[^#]%[^#]%[^#]%[^#]",
			p->id,
			p->name,
			p->age,
			p->addr,
			p->phone,
			p->email);
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//
void add(FILE *fp, const Person *p) {
	char* recordbuf = (char*)malloc(sizeof(char)*MAX_RECORD_SIZE);
	char* pagebuf = (char*)malloc(sizeof(char)*PAGE_SIZE);
	int next_page, next_record;
	int before_page=-1,before_record=-1;
	int count_page;
	int first_pages, first_records;
	int first_tmp ,first_offset,first;
	int flag=0;
	int offset,length,temp_offset;
	int current_page,current_record;
	char header_record[16];
	char header_area[HEADER_AREA_SIZE]; // header area 선언
	char temp_pagebuf[PAGE_SIZE];

	pack(recordbuf,p); 
	rewind(fp); 
	
	fread(header_record,sizeof(header_record),1,fp); // file의 header record를 읽는다.
	memcpy(&count_page,header_record,4);
	memcpy(&next_page,header_record + 8 ,4); // header record에서 nextpage를 읽는다.
	memcpy(&next_record,header_record + 12 ,4); // header record에서 nextrecord를 읽는다.
	
	if(count_page == 0) {
		first_pages = 1;
		first_records = 1;
		first_tmp = 1; // page의 총 레코드 갯수. 
		first_offset = 0; // 첫번째 page의 첫번째 record의 offset은 0
		first = strlen(recordbuf);

		rewind(fp); // fp 초기화.
		memcpy(header_record,&first_pages,sizeof(int));
		memcpy(header_record + 4,&first_records,sizeof(int));
		fwrite(header_record,sizeof(header_record),1,fp);
		
		memcpy(pagebuf,&first_tmp,sizeof(int));
		memcpy(pagebuf + 4,&first_offset,sizeof(int));

		memcpy(pagebuf +  8, &first,sizeof(int));
		memcpy(pagebuf + HEADER_AREA_SIZE,recordbuf,strlen(recordbuf));
		writePage(fp,pagebuf,first_pages-1);
		return;
	}

	while(next_page != -1 && next_record != -1) { 
		readPage(fp,pagebuf,next_page); 
		memcpy(header_area,pagebuf,HEADER_AREA_SIZE); 
			
		current_page = next_page;  
		current_record = next_record;

		memcpy(&offset,header_area + 8 * current_record + 4,4); // offset + length = 8 이므로 8 * recordnum을 하면 해당 record의 offset과 length에 접근
		memcpy(&length,header_area + 8 * current_record + 8,4); // length 읽기.  
		
		memcpy(&next_page,pagebuf + (HEADER_AREA_SIZE + offset + 1) ,4); // 만약, 현재 page에 담을수 없으면 next로 넘어가야 하기때문에 읽어둔다.
		memcpy(&next_record,pagebuf + (HEADER_AREA_SIZE + offset + 5) ,4); // recordnum도 읽는다.
		
		if(strlen(recordbuf) <= length) { // 만약 현재 page의 record의 쓸 수 있다면,
			memcpy(pagebuf + (offset + HEADER_AREA_SIZE),recordbuf,strlen(recordbuf)); // 해당 page의 record자리에 방문해서 record에 작성.
			writePage(fp,pagebuf,current_page); // page업데이트
			if(before_page == -1 && before_record == -1) {
				rewind(fp); // 헤더레코드 접근을 위해 fp를 초기화 .
				memcpy(header_record + 8,&next_page,sizeof(int));
				memcpy(header_record + 12,&next_record,sizeof(int));
				fwrite(header_record,sizeof(header_record),1,fp);	
			}
			else {  
				readPage(fp,temp_pagebuf,before_page);
				memcpy(&temp_offset,temp_pagebuf + 4 + before_record * 8,4);
				memcpy(temp_pagebuf + temp_offset + 1 + HEADER_AREA_SIZE,&next_page,4);
				memcpy(temp_pagebuf + temp_offset + 5 + HEADER_AREA_SIZE,&next_record,4);
			}	

			flag = 1;
			break;
		}
		before_page = current_page;
		before_record = current_record;
	}


	if(flag != 1) { 
		int num_pages = 0; // header record
		int num_records = 0; // header record
		int i, len,tmp;
		int temp = 0, total = 0,temp_off = 0;

		memcpy(&num_pages,header_record,sizeof(int));
		memcpy(&num_records,header_record + 4,sizeof(int));

		readPage(fp,pagebuf,num_pages-1);	
		memcpy(&temp,pagebuf,sizeof(int));
		
		
		for(i = 0; i < temp; i++){
			memcpy(&len,pagebuf + (i + 1) * 8 ,sizeof(int));
			total = total + len;
		}
		if(total + strlen(recordbuf) <= DATA_AREA_SIZE && 8 <= HEADER_AREA_SIZE - (temp * 8) - 4) { // append 가능.
			temp++;
			temp_off = total;
		}
		else {
			temp = 1;
			temp_off = 0;
			num_pages++;
		}
		
		num_records++;
		
		memcpy(header_record,&num_pages,sizeof(int));
		memcpy(header_record + 4,&num_records,sizeof(int));
		fseek(fp,SEEK_SET,SEEK_SET);
		fwrite(header_record,sizeof(header_record),1,fp);

		memcpy(pagebuf,&temp,sizeof(int));
		memcpy(pagebuf + (temp * 8) - 4,&temp_off,sizeof(int));
		
		tmp = strlen(recordbuf);

		memcpy(pagebuf + (temp * 8), &tmp,sizeof(int));
		memcpy(pagebuf + HEADER_AREA_SIZE + temp_off,recordbuf,strlen(recordbuf));
		writePage(fp,pagebuf,num_pages-1);
	}	
}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *id) {
	char header_record[16];
	char mark = '*';
	int num_records, num_pages,i,j;
	int recent_delete_page,recent_delete_record;
	int in_page_records;
	int offset,len;

	rewind(fp);
	fread(header_record,sizeof(header_record),1,fp);
	memcpy(&num_pages,header_record,sizeof(int));
	memcpy(&recent_delete_page,header_record + 8,sizeof(int));
	memcpy(&recent_delete_record,header_record + 12,sizeof(int));
	
	if(num_pages == 0) { // data page 가 없을 경우 삭제 레코드를 찾을수없기에 에러처리
		printf("data page is not exist\n");
		return;
	}
	else { // data page가 한개라도 있다면 page를 읽어서 레코드를 찾아야함.
		char * pagebuf = (char*)malloc(sizeof(char) * PAGE_SIZE);
		for(i = 0; i < num_pages; i++) {
			char *header_area = (char*)malloc(sizeof(char)*HEADER_AREA_SIZE);
			readPage(fp,pagebuf,i); 
			memcpy(header_area,pagebuf,HEADER_AREA_SIZE);
			memcpy(&in_page_records,header_area,sizeof(int)); 
			
			
			for(j = 0; j < in_page_records; j++) { // 각 record에 접근해서 내용을 가져온다.
				memcpy(&offset,header_area + 8 * j + 4,4); // offset + length = 8 이므로 8 * recordnum을 하면 해당 record의 offset과 length에 접근
				memcpy(&len,header_area + 8 * j + 8,4); // length 읽기.
				
				char * temp_record = (char*)malloc(sizeof(char)*MAX_RECORD_SIZE);
				
				memcpy(temp_record,pagebuf + (offset + HEADER_AREA_SIZE),len); // 해당레코드가져오기
				
				
				Person* p = (Person*)malloc(sizeof(Person));
				unpack(temp_record,p);
				if (strncmp(temp_record,id,strlen(p->id)) == 0 ) {// id값을 비교. 같다면 해당 레코드 삭제해야함
					memcpy(pagebuf + (offset + HEADER_AREA_SIZE),&mark,sizeof(char)); // mark표시해주고 
					memcpy(pagebuf + (offset + HEADER_AREA_SIZE + 1),&recent_delete_page,sizeof(int)); 
					memcpy(pagebuf + (offset + HEADER_AREA_SIZE + 5),&recent_delete_record,sizeof(int));
					writePage(fp,pagebuf,i);
					memcpy(header_record + 8,&i,sizeof(int));
					memcpy(header_record + 12,&j,sizeof(int));
					fseek(fp,SEEK_SET,SEEK_SET);
					fwrite(header_record,sizeof(header_record),1,fp);
					break;		
			}
		}
	}
}
	


}

int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터
	int header_num_page; // header record
	int header_record; // header record
	int recently_del_page; // header record
	int recently_del_record; // header record	
	char Pagebuf[PAGE_SIZE];
	char headerrecord [16];
	char delete_id[14];

	if(argc<3){
		fprintf(stderr,"invalid input\n");
		exit(1);}

	if(access(argv[2],F_OK) == 0) { // file이 기존에 존재할떄.
		fp = fopen(argv[2],"r+");
	}
	else if(access(argv[2],F_OK)< 0){ // file이 기존에 존재하지 않을때
		fp = fopen(argv[2],"w+"); // 초기화 과정 필요.
		header_num_page = 0;
		header_record = 0;
		recently_del_page = -1;
		recently_del_record = -1;

		memcpy(headerrecord,&header_num_page,sizeof(int));
		memcpy(headerrecord + 4,&header_record,sizeof(int));
		memcpy(headerrecord + 8,&recently_del_page,sizeof(int));
		memcpy(headerrecord + 12,&recently_del_record,sizeof(int));
		
		fwrite(headerrecord,sizeof(headerrecord),1,fp);
	} 
	

	if(!strcmp(argv[1],"a")) { // add 기능.
		Person* p = (Person*)malloc(sizeof(Person));
		strncpy(p->id,argv[3] ,strlen(argv[3])); 
		strncpy(p->name,argv[4],strlen(argv[4]));
		strncpy(p->age,argv[5] ,strlen(argv[5]));
		strncpy(p->addr,argv[6] ,strlen(argv[6]));
		strncpy(p->phone,argv[7] ,strlen(argv[7]));
		strncpy(p->email,argv[8] ,strlen(argv[8]));
		 
		add(fp,p);
	}
	else if(!strcmp(argv[1],"d")) { //delete 기능
		strncpy(delete_id,argv[3],strlen(argv[3]));
		delete(fp,delete_id);
	}
	else {
		printf("NO Option.\n");
	}

	return 0;
}
