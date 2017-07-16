#pragma once

#ifndef __mzbeep_h__
#define __mzbeep_h__

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

void mz8253beep_init(int freq);
void mz8253beep_clean();
void mz8253beep_stop();
void mz8253beep_setFreq(int arg);
void mz8253beep_update(short* ptr, int cou);

#ifdef __cplusplus
}
#endif

#endif // __mzbeep_h__
