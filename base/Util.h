#pragma once

#include <iostream>

#define PROJECT_NAME webServer

#define START_NAMESPACE namespace PROJECT_NAME{

#define END_NAMESPACE   };

#define NAMESPACE   ::PROJECT_NAME::

#define USE_NAMESPACE   using namespace PROJECT_NAME;

#define likely(x) __builtin_expect(!!(x), 1)

#define unlikely(x) __builtin_expect(!!(x), 0)