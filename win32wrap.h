#ifndef _WIN32WRAP_H
#define _WIN32WRAP_H
#include "yaplot.h"
void resettimer();
int deltatime();
Winfo *GetOptions(int argc,char *argv[],Ainfo *a,Ginfo *g);
#endif
