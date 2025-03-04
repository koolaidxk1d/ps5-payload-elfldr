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


#include "libc.h"
#include "payload.h"
#include "syscall.h"


static int*  (*sce_errno)(void);
static void* (*sce_malloc)(size_t);
static void* (*sce_realloc)(void*, size_t);
static void  (*sce_free)(void*);
static void* (*sce_memcpy)(void*, const void*, size_t);
static void* (*sce_memset)(void*, int, size_t);
static char* (*sce_strcpy)(const char*, const char*);
static int   (*sce_strcmp)(const char*, const char*);
static int   (*sce_strncmp)(const char*, const char*, size_t);
static int   (*sce_htons)(uint16_t);
static int   (*sce_pthread_create)(pthread_t*, const pthread_attr_t*,
				   void *(*func) (void *), void *);


static intptr_t sce_syscall;
asm(".intel_syntax noprefix\n"
    ".global syscall\n"
    ".type syscall @function\n"
    "syscall:\n"
    "  mov rax, rdi\n"                      // sysno
    "  mov rdi, rsi\n"                      // arg1
    "  mov rsi, rdx\n"                      // arg2
    "  mov rdx, rcx\n"                      // arg3
    "  mov r10, r8\n"                       // arg4
    "  mov r8,  r9\n"                       // arg5
    "  mov r9,  qword ptr [rsp + 8]\n"      // arg6
    "  jmp qword ptr [rip + sce_syscall]\n" // syscall
    "  ret\n"
    );


int
libc_init(const payload_args_t *args) {
  int error;

  if((error=args->sceKernelDlsym(0x2001, "__error", &sce_errno))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2001, "htons", &sce_htons))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2001, "pthread_create", &sce_pthread_create))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2001, "getpid", &sce_syscall))) {
    return error;
  }
  sce_syscall += 0xa; // jump directly to syscall instruction

  if((error=args->sceKernelDlsym(0x2, "malloc", &sce_malloc))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2, "realloc", &sce_realloc))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2, "free", &sce_free))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2, "memset", &sce_memset))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2, "memcpy", &sce_memcpy))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2, "strcpy", &sce_strcpy))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2, "strcmp", &sce_strcmp))) {
    return error;
  }
  if((error=args->sceKernelDlsym(0x2, "strncmp", &sce_strncmp))) {
    return error;
  }

  return 0;
}


int
geterrno(void) {
  return *sce_errno();
}


ssize_t
read(int fd, void *buf, size_t cnt) {
  return syscall(SYS_read, fd, buf, cnt);
}


ssize_t
write(int fd, const void *buf, size_t cnt) {
  return syscall(SYS_write, fd, buf, cnt);
}


int
socket(int dom, int ty, int proto) {
  return (int)syscall(SYS_socket, dom, ty, proto);
}


int
setsockopt(int fd, int lvl, int name, const void *val, socklen_t len) {
  return (int)syscall(SYS_setsockopt, fd, lvl, name, val, len);
}


int
bind(int fd, const struct sockaddr_in *addr, socklen_t addr_len) {
  return (int)syscall(SYS_bind, fd, addr, addr_len);
}


int
listen(int fd, int backlog) {
  return (int)syscall(SYS_listen, fd, backlog);
}


int
accept(int fd, struct sockaddr_in *addr, socklen_t *addr_len) {
  return (int)syscall(SYS_accept, fd, addr, addr_len);
}


int
close(int fd) {
  return (int)syscall(SYS_close, fd);
}


pid_t
getpid(void) {
  return (pid_t)syscall(SYS_getpid);
}


pid_t
getppid(void) {
  return (pid_t)syscall(SYS_getppid);
}


pid_t
waitpid(pid_t pid, int *status, int opts) {
  return (pid_t)syscall(SYS_wait4, pid, status, opts, 0);
}


int
ptrace(int request, pid_t pid, intptr_t addr, int data) {
  return (int)syscall(SYS_ptrace, request, pid, addr, data);
}


int
sysctl(const int *name, size_t namelen, void *oldp, size_t *oldlenp,
       const void *newp, size_t newlen) {
  return syscall(SYS___sysctl, name, namelen, oldp, oldlenp, newp, newlen);
}


void*
malloc(size_t len) {
  return sce_malloc(len);
}


void
free(void *ptr) {
  return sce_free(ptr);
}


void*
memcpy(void *dst, const void* src, size_t len) {
  return sce_memcpy(dst, src, len);
}


void*
memset(void *dst, int c, size_t len) {
  return sce_memset(dst, c, len);
}


void*
realloc(void *ptr, size_t len) {
  return sce_realloc(ptr, len);
}


char*
strcpy(char *dst, const char *src) {
  return sce_strcpy(dst, src);
}


int
strcmp(const char* s1, const char* s2) {
  return sce_strcmp(s1, s2);
}


int
strncmp(const char *s1, const char *s2, size_t len) {
  return sce_strncmp(s1, s2, len);
}


uint16_t
htons(uint16_t val) {
  return sce_htons(val);
}


int
pthread_create(pthread_t *trd, const pthread_attr_t *attr,
	       void *(*func) (void *), void *arg) {
  return sce_pthread_create(trd, attr, func, arg);
}
