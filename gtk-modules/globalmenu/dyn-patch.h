#ifndef __DYN_PATCH_H__
#define __DYN_PATCH_H__
#include <dyn-patch-utils.h>
#include <dyn-patch-vfunc.h>

void dyn_patch_init(void);
void dyn_patch_uninit_vfuncs(void);
void dyn_patch_uninit_final(void);
#endif
