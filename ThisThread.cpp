
#include <pthread.h>
#include <unistd.h>

#include "Thread.h"
#include "ThisThread.h"
USE_NAMESPACE

__thread int ThisThread::t_cachedTid;
__thread char ThisThread::t_tidString[32];
__thread size_t ThisThread::t_tidStringSize;
__thread const char* ThisThread::t_threadName;

void ThisThread::cacheTid(){
	t_cachedTid = ::gettid();
	t_tidStringSize = sprintf(t_tidString, "%5d", t_cachedTid);
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


