#pragma once

#include <windows.h>

/* Global interfaces for C caller */
UINT8	MZPGetFlag();
void	MZPSendCtrl(int value);
void	MZPSendData(int value);
void	MZPEjectPaper();
void	MZPReset();