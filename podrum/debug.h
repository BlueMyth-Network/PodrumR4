#ifndef PODRUM_DEBUG_H
#define PODRUM_DEBUG_H

#ifdef _WIN32

#define USE_LEAK_DETECTOR 0

#if USE_LEAK_DETECTOR 1

#include <vld.h>

#endif

#endif

#endif