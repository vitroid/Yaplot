#ifndef _UNIXWRAP_H
#define _UNIXWRAP_H
#include "common.h"
void resettimer();
int deltatime_obsolete();
int deltatime2();
Winfo *GetOptions(int argc,char *argv[],Ainfo *a,Ginfo *g);
int ePushView(Ginfo *g,Winfo w[],int i);
int ePopView(Ginfo *g,Winfo w[],int i);
#endif
