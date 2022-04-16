![Logo](https://github.com/luckythandel/loki/blob/main/assets/cover/loki.png)
# Loki
[![forthebadge](https://forthebadge.com/images/badges/open-source.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/made-with-c.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![forthebadge](https://forthebadge.com/images/badges/makes-people-smile.svg)](https://www.linkedin.com/in/lucky-thandel-26391b16a/)   

## About the Loki
Loki is a simple tcp server which can send files to multipal clients simulteniously. It serves raw TCP requests and HTTP GET requests as well which means you can use `wget`, `curl`, `axel` and such tools as a client to connect with this. You can use `netcat`/`nc`/`ncat` or you can write your own socket script to work with its raw TCP functionality. And since the Loki is made in C with pure syscalls and proper logic, you will find it really fast. Loki can be used while playing HackTheBox machines and VulnHub machines in place of a python/php HTTP server. 

## Features
- easy synthax
- faster than any other tool
- auditing functions (requests.log)
- multithreaded
- accept raw TCP & HTTP GET requests
- memory-efficient
- installation is easy
- easy to modify

## Compilation
* clone the repo
```sh
❯ git clone https://github.com/luckythandel/loki.git
```
* compile it
```sh
❯ make
```
* compiled binaries in `bin/` directory

### modiftion before compilaton
* change the maximum request length in `server.h` (line 1)
```c
#define REQUEST_LEN 1000
```
* change the maximum threads (server handling amount of clinet at the same time, line 2)
```c
#define MAX_THREADS 2000
```
* proper request sanitization by the server. prevents from attacks like LFI but you can intentionally enable it (line 4)
```c
#define INPUT_VALIDATION 0
```
* file content size is 10mb by default (line 3)
```c
#define FILE_CONTENT_SIZE 10000000 
```
