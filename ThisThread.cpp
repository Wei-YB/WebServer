#include "ThisThread.h"
#include <pthread.h>
#include <Thread.h>
#include <unistd.h>

USE_NAMESPACE

void ThisThread::cacheTid(){
	t_cachedTid = pthread_self();
	t_tidStringSize = sprintf(t_tidString, "%5d", t_cachedTid);
	t_threadName = Thread.name_;
}

int ThisThread::tid()
{
	if (t_cachedTid == 0)
		cacheTid();
	return t_cachedTid;
}

const char* ThisThread::tidString()
{
	if (t_tidString == nullptr) {
		cacheTid();
	}
	return t_tidString;
}

int ThisThread::tidStringLength()
{
	if (t_tidStringSize == 0) {
		cacheTid();
	}
	return t_tidStringSize;
}

const char* ThisThread::name()
{
	if (t_threadName == nullptr) {
		cacheTid();
	}
	return t_threadName;
}

bool ThisThread::isMainThread()
{
	return getpid() == tid();
}


