#pragma once

#include <windows.h>

/* Global interfaces for C caller */
UINT8	MZPGetStatus();
void	MZPSendCtrl(int value);
void	MZPSendData(int value);
void	MZPPaperCut();
