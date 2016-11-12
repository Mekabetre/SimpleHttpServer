#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "process.h"
#include "shelp.h"

int create_listen_socket(int port){
	int st = socket(AF_INET, SOCK_STREAM, 0);
	if(st == -1){
		perror("create st failed.");
		return 0;
	}
	int on = 1;
	if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int))){
		perror("set st failed");
		return 0;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_addr.s_addr = inet_addr("192.168.1.201");
	
	if(bind(st, (struct sockaddr *)&addr, sizeof(addr)) == -1){
		perror("bind failed");
		return 0;
	}
	if(listen(st, 100) == -1){
		perror("listen failed");
		return 0;
	}

	return st;
}

void accept_st(int st){
	pthread_t thread;
	int client_st = 0;
	struct sockaddr_in client_addr;
	int addr_length = sizeof(client_addr);

	while(1){
		memset(&client_addr, 0, sizeof(client_addr));
		client_st = accept(st, (struct sockaddr *)&client_addr, &addr_length);
		if(client_st == -1){
			perror("accept error");
			break;
		}else{
			int * temp_st = malloc(sizeof(int));
			*temp_st = client_st;
			pthread_create(&thread, NULL, process_request, temp_st);
			pthread_detach(thread);
		}
	}
}
