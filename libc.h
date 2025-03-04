/* Copyright (C) 2023 John Törnblom

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */

#pragma once


#define	AF_INET             2
#define	AF_INET6            28

#define	SOCK_STREAM         1
#define	SOCK_DGRAM          2

#define IPPROTO_UDP         17
#define IPPROTO_IPV6        41

#define IPV6_2292PKTOPTIONS 25
#define IPV6_PKTINFO        46
#define IPV6_TCLASS         61

#define SOL_SOCKET   0xffff
#define SO_REUSEADDR 0x0004

#define errno (geterrno())


typedef char  int8_t;
typedef short int16_t;
typedef int   int32_t;
typedef long  int64_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;

typedef uint64_t intptr_t;
typedef uint64_t off_t;
typedef uint64_t size_t;
typedef int64_t  ssize_t;
typedef uint32_t socklen_t;
typedef uint32_t in_addr_t;
typedef int32_t  pid_t;
typedef int32_t  uid_t;
typedef int32_t  gid_t;

struct pthread;
struct pthread_attr;

typedef struct pthread      *pthread_t;
typedef struct pthread_attr *pthread_attr_t;

struct in_addr {
  in_addr_t s_addr;
};

struct sockaddr_in {
  uint8_t sin_len;
  uint8_t sin_family;
  uint16_t sin_port;
  struct in_addr sin_addr;
  uint8_t sin_zero[8];
};


long syscall(int, ...);
int geterrno(void);

ssize_t read(int fd, void *buf, size_t cnt);
ssize_t write(int fd, const void* buf, size_t cnt);
int close(int fd);

int socket(int dom, int ty, int proto);
int setsockopt(int fd, int lvl, int name, const void *val,
	       unsigned int len);

uint32_t htonl(uint32_t hostlong);

int listen(int fd, int backlog);
int bind(int fd, const struct sockaddr_in *addr, socklen_t addr_len);
int accept(int fd, struct sockaddr_in *addr, socklen_t *addr_len);

pid_t getpid(void);
pid_t getppid(void);
pid_t waitpid(pid_t pid, int *status, int opts);

int ptrace(int request, pid_t pid, intptr_t addr, int data);

void* malloc(size_t len);
void* realloc(void *ptr, size_t len);
void free(void *ptr);
void* memset(void *dst, int c, size_t len);
void* memcpy(void *dst, const void* src, size_t len);

char* strcpy(char *dst, const char *src);
int strcmp(const char* s1, const char* s2);
int strncmp(const char *s1, const char *s2, size_t n);

uint16_t htons(uint16_t val);

int sysctl(const int *name, size_t namelen, void *oldp, size_t *oldlenp,
	   const void *newp, size_t newlen);

int pthread_create(pthread_t *trd, const pthread_attr_t *attr,
		   void *(*func) (void *), void *arg);
