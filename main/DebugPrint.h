#ifndef DEBUGPRINT_H
#define DEBUGPRINT_H

#include "SerialPrintf.h"
#include <Arduino.h>

// #define _DEBUG

#ifdef _DEBUG
#define DEBUG_PRINTF(...) SerialPrintf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#endif
