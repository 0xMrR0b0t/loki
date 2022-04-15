#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "server.h"


void free_mem(char* pointer)
{
  free(pointer);
}

int listener(int port)
{
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }
    listen(socket_desc , 3);
    c = sizeof(struct sockaddr_in);
	  pthread_t thread_id;
	  int thread_count = 0;

    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        if(thread_count > MAX_THREADS)
          continue;
        if( pthread_create( &thread_id , NULL, serve, (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        thread_count++;
        sleep(1);
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}

int file_exists(char* request)
{
  if(access(request, F_OK) != 0){
    my_log(request, 2);
    return -1;
  }
  return 0;
}

int http_send(int client_sock, char* request)
{
  if(file_exists(request) == -1)
    return 0;
  my_log(request, 1);
  FILE *ptr;
  ptr = fopen(request, "r");
  char* file_content;
  char http_response[] = "HTTP/1.0 200 OK\r\nServer: LuckyThandel/kali\r\n\r\n\0";
  int i = 0;
  fseek(ptr, 0L, SEEK_END);
  long long int file_size = ftell(ptr);
  fseek(ptr, 0L, SEEK_SET);
  if(file_size > FILE_CONTENT_SIZE)
    return -1;
  file_size = file_size + strlen(http_response);
  file_content = (char *)malloc(sizeof(char) * file_size);
  while(1)
  {
    if(i < strlen(http_response))
    {
      file_content[i] = http_response[i];
      i++;
      continue;
    }
    char c = fgetc(ptr);
    if(feof(ptr))
      break;
    file_content[i] = c;
    i++;
  }
  send(client_sock, file_content, file_size, 0);
  fclose(ptr);
  free_mem(file_content);
}

int input_validate(char* request)
{
  char* pwd = getenv("PWD");
  char bad_chars[] = {'$', '\'', '"', '&', '!', '`', '\\', '|', ';'};
  char* lfi = "../";
  if(strstr(request, lfi)){
    my_log(request, 3);
    return -1;
  }
  
  for(int i=0; i<sizeof(bad_chars); i++){
    if(strchr(request, bad_chars[i])){
      my_log(request, 3);
      return -1;
    }
  }
  if(request[5] == '/'){
    my_log(request, 3);
    return -1;
  }
  return 0;
}

void my_log(char* input, int x)
{
  /* `x` has multipal values
   * 1 -> FILE_OK
   * 2 -> FILE_404
   * 3 -> FILE_403
   */
  if(x == 1)
  {
    char value[100] = "[\033[32;1;1mInfo\033[0m] /";
    printf("%s%s ~ 200 OK\n", value,input);
  }
  else if(x == 2)
  {
    char value[100] = "[\033[31;1;1mInfo\033[0m] /";
    printf("%s%s ~ 404 NOT FOUND\n",value, input);
  }
  else if(x == 3)
  {
    
    char value[100] = "[\033[33;1;1mDetected!\033[0m] ";
    printf("%s malicious request\n",value);
  }
  FILE *ptr;
  ptr = fopen("./requests.log", "a");
  if(ptr == NULL)
  {
    perror("something went wrong\n");
  }
  char* time_buffer;
  time_t raw_time;
  struct tm* timeinfo;
  time(&raw_time);
  timeinfo = localtime(&raw_time);
  time_buffer = asctime(timeinfo);
  time_buffer[strlen(time_buffer) - 1] = ' ';
  strcat(time_buffer, input);
  if(x == 1)
    strcat(time_buffer, " ~ 200 OK\n");
  if(x == 2)
    strcat(time_buffer, " ~ 404 NOT FOUND\n");
  if(x == 3)
    strcat(time_buffer, " ~ malicious request\n");
  fprintf(ptr, "%s", time_buffer);
  fclose(ptr);
}

int raw_send(int client_sock, char* request)
{
  //remove newline from the file
  if(request[strlen(request) - 1] == '\n') 
    request[strlen(request) - 1] = '\0';
  if(request[strlen(request) - 1] == '\r')
    request[strlen(request)- 1] = '\0';
  
  if(file_exists(request) == -1)
    return 0;
  my_log(request, 1);
  FILE *ptr;
  ptr = fopen(request, "r");
  fseek(ptr, 0L, SEEK_END);
  long long int file_size = ftell(ptr);
  fseek(ptr, 0L, SEEK_SET);
  if(file_size > FILE_CONTENT_SIZE)
    return -1;
  char* file_content;
  file_content = (char *)malloc(sizeof(char)* file_size);
  long long int i = 0;
  while(1)
  {
    char c = fgetc(ptr);
    if(feof(ptr))
      break;
    file_content[i] = c;
    i++;
  }
  send(client_sock, file_content, file_size, 0);
  fclose(ptr);
  free_mem(file_content);
  return 0;
}

int checker(struct SERVE_THREAD THREAD)
{
  /* leaving the security for input validation
   * for the users for the intentional use of the server
   * since it is considered that server is being used for personal use only
   * if you want input validation in `request` change `INPUT_VALIDATION` to 1 (line 14)
   * and compile it again
   */
  if(INPUT_VALIDATION){
   if(input_validate(THREAD.request))
     return -1;
  }
  
  char *request = THREAD.request;
  int client_sock = THREAD.client_sock;
  char* HTTP_GET = "GET /";

  if(!strlen(request))
    return -1;
  
  if(!strncmp(HTTP_GET, request, strlen(HTTP_GET)))
  {
    int HTTP_METHOD_LEN = strlen(HTTP_GET);
    for(int j=0; j<HTTP_METHOD_LEN; j++)
      request = request+1;
    char* temp = strtok(request, "\r");
    int temp_len = strlen(temp);
    int c = 9; //remove chars HTTP/1.1\r\n
    while(c != 0)
    {
      temp[temp_len-1] = '\0';
      temp_len--;
      c--;
    }
    http_send(client_sock, request);
    return 0;
  }
  raw_send(client_sock, request);
  return -1;
}

void serve(void *client_st)
{
  int client_sock = *(int*) client_st;
  struct SERVE_THREAD THREAD;
  THREAD.request = (char*) malloc(sizeof(char) * REQUEST_LEN); // request length
  if(recv(client_sock, THREAD.request, REQUEST_LEN, 0) == -1)
  {
    perror("Bad file descriptor");
    close(client_sock);
  }
  THREAD.client_sock = client_sock;
  if(checker(THREAD) == -1)
  {
    close(THREAD.client_sock);
  }
  close(THREAD.client_sock);
}

void main(int argc, char* argv[])
{
  char *creator = "[\033[35;1;1m@LuckyThandel\033[0m]";
  char *help_synthx = "[\033[31;1;1mSynthx\033[0m] ./aka port\n";
  char *warning = "[\033[33;1;1mWarning\033[0m] default port 81\n";
  if(argc >= 2){
    printf("%s\n", creator);
    printf("[\033[37;1;1mwaiting\033[0m] connection port ~ %d\n", atoi(argv[1]));
    listener(atoi(argv[1]));
  }
  else{
    printf("%s", help_synthx);
    printf("%s", warning);
    listener(PORT);
  }
}



