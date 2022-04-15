#define REQUEST_LEN 1000
#define MAX_THREADS 2000
#define FILE_CONTENT_SIZE 10000000
#define PORT 81
#define INPUT_VALIDATION 0 //set this to `1` if you want to sanitize the request

struct SERVE_THREAD
{
  char* request;
  int client_sock;
};

void serve(void* client_st); // Capture the request & serve according to that request.
int checker(struct SERVE_THREAD THREAD); // check if we are able to fulfil the request (HTTP/ RAW).
int listener(int port); // listener for incoming connections.
void free_mem(char *pointer); //free the heap
int file_exists(char* request); // checks if we are able to fulfill the request
int http_send(int client_sock, char* request); // serve to http clients like curl, wget etc
int raw_send(int client_sock, char* request); // serve the raw TCP requests from netcat, telnet etc
int input_validate(char* request); // checks if the request is malicious 
void my_log(char* input, int x);
