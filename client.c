#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include "structs.h"

int main(){
	int fd, fd1;
	Server serverMsg;
	Client clientMsg;
	char* serverQueue = "sQueue";
	char* clientQueue = "c1Queue";

	if(mkfifo(clientQueue, 0666) < 0){
		perror(clientQueue);
		exit(1);
	}

	if((fd = open(serverQueue, O_WRONLY, 0)) == -1){
		printf("Cannot open server queue\n");
		exit(1);
	}

	int ID;
	scanf("%d", &ID);

	clientMsg.size = strlen(clientQueue);
	clientMsg.ID = ID;

	char* buff = (char*) malloc(sizeof(clientMsg) + strlen(clientQueue));
	memcpy(buff, &clientMsg, sizeof(clientMsg));
	memcpy(buff + sizeof(clientMsg), clientQueue, strlen(clientQueue));

	if(write(fd, buff, clientMsg.size + sizeof(clientMsg)) == -1){
		printf("Communication error\n");
		exit(1);
	}

	close(fd);
	if((fd1 = open(clientQueue, O_RDONLY, 0)) < 0){
		printf("Cannot open client queue\n");
		exit(1);
	}
	
	if(read(fd1, &serverMsg, sizeof(serverMsg)) == -1){
		printf("Reading from server error\n");
		exit(1);
	}
	char* string = (char*) malloc(serverMsg.size);
	if(read(fd1, string, serverMsg.size) == -1){
		printf("Queue error\n");
		exit(1);
	}

	printf("%s\n", string);

	close(fd1);
	unlink(clientQueue);
	return 0;
}
