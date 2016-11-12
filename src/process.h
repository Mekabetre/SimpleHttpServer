#ifndef _PROCESS_
#define _PROCESS_

const char * get_file_type(const char * filename);
int get_file_content(char * filename, char ** content);
int get_content(char * filename, char ** content);
void get_filename(char * http_head, char * file_name);
void * process_request(void * st);

#endif
