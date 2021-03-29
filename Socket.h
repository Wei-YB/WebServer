#pragma once

#include "InetAddress.h"

START_NAMESPACE

int Socket(int domain, int type, int protocol);

int NonblockInetSocket();

void Bind(int fd, InetAddress& address);

void Listen(int fd, int backlog);

int Accept(int fd, InetAddress& address);

void ShutdownWrite(int fd);

void setNoDelay(int fd, bool on);

int eventFd();

void Close(int fd);

END_NAMESPACE
