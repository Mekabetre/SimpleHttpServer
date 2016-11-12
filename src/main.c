#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shelp.h"

int main(int argc, char * argv[]){
	int port = atoi(argv[1]);
	int socket = create_listen_socket(port);
	
	if(socket == 0)
		exit(-1);
	
	accept_st(socket);
}
