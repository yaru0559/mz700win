//
#if _MSC_VER < 1900			// Visual Studio 2015 未満だったら DIRECTSOUNDVERSIONを0x0900に設定
#define DIRECTSOUND_VERSION 0x0900  // Version 9.0
#endif

//
#include	<windows.h>
#include	<dsound.h>
#include	"mz700win.h"
#include	"DSOUND.H"
#include	"win.h"
#include	"mzbeep.h"
#include	"sn76489an.h"

extern	HWND	hwndApp;

static const WORD num_blocks = 5;
static const WORD timer_resolution = 20;

static short			playing = FALSE;
static short			mixalways = FALSE;
static LPDIRECTSOUND		lpds = 0;
static LPDIRECTSOUNDBUFFER	lpdsb = 0;
static LPDIRECTSOUNDBUFFER	lpdsb_primary = 0;
static WORD				dstimerid = 0;
static DWORD			userid = 0;
static DWORD			buffer_length = 0;
static short			sampleshift = 2;
static DWORD			sending = FALSE;
static DWORD			buffersize = 0;
static DWORD			nextwrite = 0;
static BOOL				DS_MuteMode = FALSE;

//extern	BYTE	OPM_SW;
//extern	void FASTCALL PSG_Update(WORD* buf, DWORD length);

BOOL DSound_Init(DWORD rate, DWORD buflen)
{
	DSBUFFERDESC dsbd;
	WAVEFORMATEX wf;
	HRESULT res;

	if (playing) {
		return FALSE;
	}

	if (!rate) {				// Rate=0 : サウンドOFFの時
		dstimerid = 0;
		lpdsb = 0;
		lpdsb_primary = 0;
		lpds = 0;
		return TRUE;
	}

	buffer_length = buflen;
	sampleshift = 2;
	DS_MuteMode = 0;

	buffersize = (rate * 4 * buffer_length / 1000) & ~7;

	if (FAILED(DirectSoundCreate(0, &lpds, 0)))
		return FALSE;
	if (DS_OK != IDirectSound_SetCooperativeLevel(lpds, hwndApp, DSSCL_PRIORITY)) {
		if (DS_OK != IDirectSound_SetCooperativeLevel(lpds, hwndApp, DSSCL_NORMAL)) {
			return FALSE;
		}
	}
	
	memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0; 
	dsbd.lpwfxFormat = 0;
	if (DS_OK != IDirectSound_CreateSoundBuffer(lpds, &dsbd, &lpdsb_primary, 0)) {
		return FALSE;
	}

	memset(&wf, 0, sizeof(WAVEFORMATEX));
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 2;
	wf.nSamplesPerSec = rate;
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

	IDirectSoundBuffer_SetFormat(lpdsb_primary, &wf);

	// セカンダリバッファ作成
	memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLDEFAULT | DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;

	dsbd.dwBufferBytes = buffersize; 
	dsbd.lpwfxFormat = &wf;
    
	res = IDirectSound_CreateSoundBuffer(lpds, &dsbd, &lpdsb, NULL); 
	if (DS_OK != res) {
		return FALSE;
	}

	IDirectSoundBuffer_Play(lpdsb, 0, 0, DSBPLAY_LOOPING);

	timeBeginPeriod(buffer_length / num_blocks);
	userid = (DWORD) &DSound_Init;
	dstimerid = timeSetEvent(buffer_length / num_blocks, timer_resolution,
		DSound_TimeProc, userid, TIME_PERIODIC);
	nextwrite = 1 << sampleshift;
	
	if (!dstimerid)	{
		timeEndPeriod(buffer_length / num_blocks);
		return FALSE;
	}

	playing = TRUE;
	return TRUE;
}

// ---------------------------------------------------------------------------
//  後片付け
// ---------------------------------------------------------------------------

BOOL DSound_Cleanup()
{
	playing = FALSE;
	if (dstimerid)
	{
		timeKillEvent(dstimerid);
		timeEndPeriod(buffer_length / num_blocks);
		dstimerid = 0;
	}
	if (lpdsb)
	{
		IDirectSoundBuffer_Stop(lpdsb);
		IDirectSoundBuffer_Release(lpdsb);
		lpdsb = 0;
	}
	if (lpdsb_primary)
	{
		IDirectSoundBuffer_Release(lpdsb_primary);
		lpdsb_primary = 0;
	}
	if (lpds)
	{
		IDirectSound_Release(lpds);
		lpds = 0;
	}
	return TRUE;
}

// ---------------------------------------------------------------------------
//  タイマーが呼ばれたら
// ---------------------------------------------------------------------------
void CALLBACK DSound_TimeProc(unsigned int uid, unsigned int tmp, DWORD user, DWORD tmp0, DWORD tmp1)
{
	if (user == userid) {
		DSound_Send();			// IDを確認して飛ぶ。
	}
}

// ---------------------------------------------------------------------------
//  サウンドのミュート
// ---------------------------------------------------------------------------
void DSound_Mute(BOOL sw)
{
	DS_MuteMode = sw;
}

// ---------------------------------------------------------------------------
//  タイマー時の本体
// ---------------------------------------------------------------------------
void DSound_Send(void)
{
	DWORD cplay;
	DWORD cwrite;
	DWORD status;
	short restored;
	int writelength;
	void* a1, * a2;
	DWORD al1, al2;

	if (playing && !InterlockedExchange(&sending, TRUE)) {
		restored = FALSE;

		IDirectSoundBuffer_GetStatus(lpdsb, &status);
		if (DSBSTATUS_BUFFERLOST & status) {
			if (DS_OK != IDirectSoundBuffer_Restore(lpdsb))
				goto ret;
			nextwrite = 0;
			restored = TRUE;
		}

		IDirectSoundBuffer_GetCurrentPosition(lpdsb, &cplay, &cwrite);

		if (cplay == nextwrite && !restored)
			goto ret;

		if (cplay < nextwrite)
			writelength = cplay + buffersize - nextwrite;
		else
			writelength = cplay - nextwrite;

		writelength &= -1 << sampleshift;

		if (writelength <= 0)
			goto ret;

		{
			if (DS_OK != IDirectSoundBuffer_Lock(lpdsb, nextwrite, writelength,
						 (void**) &a1, &al1, (void**) &a2, &al2, 0))
				goto ret;

			if (DS_MuteMode)	// メニューとか出てたら音を止める
			{
				// DSバッファを無音で埋める
				if (a1) ZeroMemory(a1, al1);
				if (a2) ZeroMemory(a2, al2);
			}
			else
			{
				// 通常の処理
				if (a1) ZeroMemory(a1, al1);
				if (a2) ZeroMemory(a2, al2);

				if (a1) mz8253beep_update((WORD*)a1, al1 >> sampleshift);
				if (a2) mz8253beep_update((WORD*)a2, al2 >> sampleshift);

				if (a1) sn76489an_update(0, (WORD*)a1, al1 >> sampleshift);
				if (a2) sn76489an_update(0, (WORD*)a2, al2 >> sampleshift);

				if (a1) sn76489an_update(1, (WORD*)a1, al1 >> sampleshift);
				if (a2) sn76489an_update(1, (WORD*)a2, al2 >> sampleshift);
			}
			
			// Unlock
			IDirectSoundBuffer_Unlock(lpdsb, a1, al1, a2, al2);
		}

		nextwrite += writelength;
		if (nextwrite >= buffersize)
			nextwrite -= buffersize;

		if (restored)
			IDirectSoundBuffer_Play(lpdsb, 0, 0, DSBPLAY_LOOPING);

ret:
		InterlockedExchange(&sending, FALSE);
	}
	return;
}
