﻿#pragma once

#ifndef __FILEIO_H__
#define __FILEIO_H__

#ifdef __cplusplus
extern "C"
{
#endif 

typedef HANDLE FILE_HDL;

#define FILE_VAL_ERROR 	INVALID_HANDLE_VALUE	// Win32:Error
// SEEK用
#define FILE_SEEK_SET	FILE_BEGIN
#define FILE_SEEK_CUR	FILE_CURRENT
#define FILE_SEEK_END	FILE_END

//
FILE_HDL FILE_OPEN(LPCSTR filename);
FILE_HDL FILE_ROPEN(LPCSTR filename);
FILE_HDL FILE_COPEN(LPCSTR filename);
FILE_HDL FILE_AOPEN(LPCSTR filename);
int FILE_READ(FILE_HDL fh, void * ptr, int sz);
int FILE_WRITE(FILE_HDL fh, void * ptr, int sz);
BOOL FILE_CLOSE(FILE_HDL fh);
int FILE_SEEK(FILE_HDL fh, long pointer, int mode);

//
BOOL FileExists(LPCSTR filename);
short FILE_ATTR(LPCSTR filename);

#ifdef __cplusplus
}
#endif 

#endif	//__FILEIO_H__
