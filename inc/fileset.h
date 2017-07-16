// fileset.h
// $Id: fileset.h 20 2009-05-25 18:20:32Z maru $

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif 

#ifndef _FILESET_H_
#define _FILESET_H_

void UpdateTapeMenu(void);
void UpdateQDMenu(void);

void mz_load_cmos(void);
void mz_save_cmos(void);

BOOL load_mzt_file(char *);
BOOL set_tape_file(char *);

BOOL OpenFileImage(void);
BOOL SetLoadFileImage(void);
BOOL SetSaveFileImage(void);

BOOL SetQDFileImage(void);
void EjectQDFileImage(void);

BOOL LoadRamFileImage(void);
BOOL SaveRamFileImage(void);

BOOL SaveStateFile(void);
BOOL LoadStateFile(void);

#endif // _FILESET_H_

#ifdef __cplusplus
}
#endif 
