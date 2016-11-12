#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>


#define HEAD "HTTP/1.0 200 OK\n\
Content-Type: %s\n\
Transfer-Encoding: chunked\n\
Connection: Keep-Alive\n\
Accept-Ranges:bytes\n\
Content-Length:%d\n\n"

#define TAIL "\n\n"

const char * get_file_type(const char * filename){
	int len = strlen(filename);
	int i;
	char extension[32];
	memset(extension, 0, sizeof(extension));
	for (i = 0; i < len; i++)
	{
		if (filename[i] == '.')
		{
			strncpy(extension, &filename[i + 1], sizeof(extension));
			break;
		}
	}

	if (strncmp(extension, "bmp", 3) == 0)
		return "image/bmp";

	if (strncmp(extension, "gif", 3) == 0)
		return "image/gif";

	if (strncmp(extension, "ico", 3) == 0)
		return "image/x-icon";

	if (strncmp(extension, "jpg", 3) == 0)
		return "image/jpeg";

	if (strncmp(extension, "avi", 3) == 0)
		return "video/avi";

	if (strncmp(extension, "css", 3) == 0)
		return "text/css";

	if (strncmp(extension, "dll", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(extension, "exe", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(extension, "dtd", 3) == 0)
		return "text/xml";

	if (strncmp(extension, "mp3", 3) == 0)
		return "audio/mp3";

	if (strncmp(extension, "mpg", 3) == 0)
		return "video/mpg";

	if (strncmp(extension, "png", 3) == 0)
		return "image/png";

	if (strncmp(extension, "ppt", 3) == 0)
		return "application/vnd.ms-powerpoint";

	if (strncmp(extension, "xls", 3) == 0)
		return "application/vnd.ms-excel";

	if (strncmp(extension, "doc", 3) == 0)
		return "application/msword";

	if (strncmp(extension, "mp4", 3) == 0)
		return "video/mpeg4";

	if (strncmp(extension, "ppt", 3) == 0)
		return "application/x-ppt";

	if (strncmp(extension, "wma", 3) == 0)
		return "audio/x-ms-wma";

	if (strncmp(extension, "wmv", 3) == 0)
		return "video/x-ms-wmv";

	return "text/html";
}

int get_file_content(char * filename, char ** content){
	if(*filename == 0){
		*content = malloc(3);
		*(*content) = '4';
		*(*content + 1) = '0';
		*(*content + 2) = '0';
		return 3;
	}
	struct stat t;
	memset(&t, 0, sizeof(t));
	FILE *fd = fopen(filename, "rb");
	if (fd != NULL)
	{
		stat(filename, &t);
		*content = malloc(t.st_size);
		fread(*content, t.st_size, 1, fd);
		fclose(fd);
		return t.st_size;
	}else{
		perror("open failed");
		printf("%s", filename);
		*content = malloc(3);
		*(*content) = '4';
		*(*content + 1) = '0';
		*(*content + 2) = '0';
		return 3;
	}
}

int get_content(char * filename, char ** allmsg){
	char * content;
	char head[1024];
	int content_length = get_file_content(filename, &content);
	sprintf(head, HEAD, get_file_type(filename), content_length);
	int head_length = strlen(head);
	int tail_length = strlen(TAIL);
	
	int all_length = head_length + content_length + tail_length;

	*allmsg = malloc(head_length + content_length + tail_length);

	memcpy(*allmsg, head, head_length);
	memcpy(*allmsg + head_length, content, content_length);
	memcpy(*allmsg + head_length + tail_length, TAIL, tail_length);
	free(content);
	return all_length;
}

void get_filename(char * http_head, char * file_name){
	int i;
	int start = 0;
	for(i = 0; i < strlen(http_head); i++){
		if(!start && *http_head != ' '){
			http_head++;
			continue;
		}
		if(!start && *http_head == ' '){
			start = 1;
			http_head += 2;
			continue;
		}

		if(*http_head == ' ' && start)
			break;		

		*file_name = *http_head;
		http_head++;
		file_name++;
	}
}

void * process_request(void * st){
	int socket = *(int *)st;
	char request[1024*10];

	memset(request, 0, sizeof(request));
	int length = recv(socket, request, sizeof(request), 0);

	if(length <= 0){
		perror("recv failed");
		close(socket);
		return NULL;
	}
	char * content;
	char filename[256];
	memset(filename, 0, 256);
	get_filename(request, filename);
	
	length = get_content(filename, &content);

	send(socket, content, length, 0);
	//printf("%s\n", filename);
	free(content);
	close(socket);
	//printf("%s\n", request);
}
