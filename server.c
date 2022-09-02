#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include "structs.h"

#define MAX 5

typedef struct DB{
	int ID;
	int size;
	char* string;
}DB;

DB rows[MAX];
int cursor = 0;

void insert(int, char*);
void DBinsert();
void DBshow();
int findID(int);
void ignore(int);
void finish(int);

int main(){

	int fd, fd1, fd2;
	Server serverMsg;
	Client clientMsg;
	char* serverQueue = "sQueue";

	DBinsert();
	DBshow();

	signal(SIGHUP, ignore);
	signal(SIGTERM, ignore);
	signal(SIGUSR1, finish);

	unlink(serverQueue);
	if(mkfifo(serverQueue, 0666) < 0){ //create que
		perror(serverQueue);
		exit(1);
	}

	if((fd = open(serverQueue, O_RDONLY, 0)) < 0){
		perror(serverQueue);
		exit(1);
	}

	if((fd1 = open(serverQueue, O_WRONLY, 0)) < 0){ //additional que open
		perror(serverQueue);                    //not to recieve '\0' sign
		exit(1);
	} 
	
	while(1){
		if(read(fd, &clientMsg, sizeof(clientMsg)) == -1){
			printf("Cannot read client message\n");
			exit(1);
		}
		char* clientQueue = (char*) malloc(clientMsg.size);
		if(read(fd, clientQueue, clientMsg.size) == -1){
			printf("Cannot open client queue\n");
			exit(1);
		}
		
		//printf("id: %d, size %d\n", clientMsg.ID, clientMsg.size);
		//printf("%s\n", clientQueue);
		
		int j = findID(clientMsg.ID);
		if((fd2 = open(clientQueue, O_WRONLY, 0)) != -1){
			sleep(5);
			if(j != -1){
				serverMsg.size = rows[j].size;
				
				char* buff = (char*)malloc(sizeof(serverMsg) + serverMsg.size);
				memcpy(buff, &serverMsg, sizeof(serverMsg));
				memcpy(buff + sizeof(serverMsg), rows[j].string, serverMsg.size);
				if(write(fd2, buff, serverMsg.size + sizeof(serverMsg)) == -1){
					printf("Saving message fail\n");
					exit(1);
				}
			}
			else{
				char* empty = "None";
				serverMsg.size = strlen(empty);
				char* buff = (char*) malloc(sizeof(serverMsg) + serverMsg.size);
				memcpy(buff, &serverMsg, sizeof(serverMsg));
				memcpy(buff + sizeof(serverMsg), empty, serverMsg.size);
				if(write(fd2, buff, serverMsg.size + sizeof(serverMsg)) == -1){
					printf("Saving message fail\n");
					exit(1);
				}
				
			}
			close(fd2);
		}
		else{
			printf("Cannot open client queue\n");
			exit(1);
		}

	}

	return 0;
}

void insert(int ID, char* string){
	if(cursor < MAX){
		DB row = {ID, strlen(string), string};
		rows[cursor++] = row;
	}
}

void DBinsert(){
	insert(0, "BIKE");
	insert(1, "LAMP");
	insert(2, "MOUSE");
	insert(3, "MUG");
	insert(4, "TIN");
}

void DBshow(){
	for(int i = 0; i < cursor; i++){
		//printf("ID: %d, size: %d, string: %s\n", rows[i].ID, rows[i].size, rows[i].string);
		printf("ID: %d, name: %s\n", rows[i].ID, rows[i].string); 
	}
}

int findID(int ID){
	for(int i = 0; i < cursor; i++){
		if(rows[i].ID == ID){
			return rows[i].ID;
		}
	}
	return -1;
}
void ignore(int signum){
	printf("\nsignal %d, IGNORE\n", signum);
}
void finish(int signum){
	printf("\nsignal %d, END\n", signum);
	exit(1);
}
