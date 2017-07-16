#pragma once

#ifdef __cplusplus
extern "C"
{
#endif 

#ifndef mz700win_dsound_h
#define mz700win_dsound_h

#define DSBCAPS_CTRLDEFAULT		(DSBCAPS_CTRLPAN|DSBCAPS_CTRLVOLUME|DSBCAPS_CTRLFREQUENCY)

BOOL DSound_Init(DWORD rate, DWORD length);
BOOL DSound_Cleanup(void);

void CALLBACK DSound_TimeProc(unsigned int uid, unsigned int tmp, DWORD user, DWORD tmp0, DWORD tmp1);
void DSound_Send(void);
void DSound_Mute(BOOL);


#endif //mz700win_dsound_h

#ifdef __cplusplus
}
#endif 
