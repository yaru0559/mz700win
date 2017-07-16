/* $Id: mzramem.h 1 2007-07-27 06:17:19Z maru $ */

#pragma once

#ifndef __MZRAMEM_H__
#define __MZRAMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RAM_PATCH_SIG	':TAP'
#define HED_PAT_OFS		0x40
//
void patchram_job(TMONEMU *);
void patch_ram(unsigned char *buf);
void patch_emul_ram(void);

//
void S_1Z007B_rdinf(Z80_Regs *Regs);
void S_1Z007B_rddata(Z80_Regs *Regs);
void S_1Z007B_wrinf(Z80_Regs *Regs);
void S_1Z007B_wrdata(Z80_Regs *Regs);

void S_HUBAS_rdinf(Z80_Regs *Regs);
void S_HUBAS_rddata(Z80_Regs *Regs);
void S_HUBAS_wrinf(Z80_Regs *Regs);
void S_HUBAS_wrdata(Z80_Regs *Regs);

#ifdef __cplusplus
}
#endif 

#endif	// __MZRAMEM_H__
