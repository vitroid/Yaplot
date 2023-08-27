#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "common.h"
#include "yaplot.h"
#include "unixwrap.h"
#include "gui.h"

extern int debug;
extern float Eyex,Eyey,Eyez;
extern float Lookx,Looky,Lookz;
extern float Upx,Upy,Upz;

struct itimerval now;

void resettimer()
{
  now.it_interval.tv_sec=100000000L;
  now.it_interval.tv_usec=0;
  now.it_value.   tv_sec=100000000L;
  now.it_value.tv_usec=0;
  setitimer(ITIMER_REAL,&now,NULL);
}



/*usec timer*/
int deltatime2()
{
  int delta;
  struct itimerval now2;
  getitimer(ITIMER_REAL,&now2);
  delta = now.it_value.tv_usec - now2.it_value.tv_usec;
  delta += (now.it_value.tv_sec - now2.it_value.tv_sec)*1000000;
  /*itimer is a decremental timer*/
  /*reset interval*/
  now=now2;
  return delta;
}


FILE *FindPalette(char *filename)
{
  char *env;
  char palettefile[1024];
  FILE *file;
  if(debug)fprintf(stderr,"FindPalette0::\n");
    
  file=fopen(filename,"r");
  if(file!=NULL)
    return file;

  file=fopen("yaplot.col","r");
  if(file!=NULL)
    return file;

  env=getenv("YAPLOTCOL");
  if(env!=NULL)
    {
      file=fopen(env,"r");
      if(file!=NULL)
	return file;
    }

  strcpy(palettefile,getenv("HOME"));
  strcat(palettefile,"/yaplot.col");
  file=fopen(palettefile,"r");
  if(file!=NULL)
    return file;

  file=fopen(DEFAULTPALETTE,"r");
  if(debug)fprintf(stderr,"Opening %s\n",DEFAULTPALETTE);
  return file;
}

Winfo *GetOptions(int argc,char *argv[],Ainfo *a,Ginfo *g)
{
  Winfo *w;
  int errflg;
  extern char *optarg;
  extern int optind, optopt;
  int i,nstdin;
  char *b,*palfile=NULL,c;
  errflg=0;
    
  G_Init0(&argc,&argv);
  while ((c = getopt(argc, argv, ":de:l:u:c:q:")) != EOF)
    switch (c) {
    case 'e':
      b=optarg;
      Eyex=atof(strtok(b,","));
      b=NULL;
      Eyey=atof(strtok(b,","));
      Eyez=atof(strtok(b,","));
      break;
    case 'l':
      b=optarg;
      Lookx=atof(strtok(b,","));
      b=NULL;
      Looky=atof(strtok(b,","));
      Lookz=atof(strtok(b,","));
      break;
    case 'u':
      b=optarg;
      Upx=atof(strtok(b,","));
      b=NULL;
      Upy=atof(strtok(b,","));
      Upz=atof(strtok(b,","));
      break;
    case 'c':
      palfile=optarg;
      break;
    case 'q':
      a->cachesize=atoi(optarg);
      break;
    case 'd':
      debug++;
      break;
    case ':':		/* -f or -o without arguments */
      fprintf(stderr, "Option -%c requires an argument\n",
	      optopt);
      errflg++;
      break;
    case '?':
      fprintf(stderr, "Unrecognized option: - %c\n",
	      optopt);
      errflg++;
    }
  if(debug)fprintf(stderr,"GetOptions1::\n");
  g->palettefile = FindPalette(palfile);
  if(g->palettefile==NULL)
    errflg++;
  if(argc==optind)
    errflg++;
  if (errflg) {
    fprintf(stderr,"Yaplot ver. %s \n\n",VERSION);
    fprintf(stderr,"usage : %s [options] infile [infile ...]\n",argv[0]);
    fprintf(stderr,"\t-e x,y,z\tset eyepoint\n");
    fprintf(stderr,"\t-l x,y,z\tset lookpoint\n");
    fprintf(stderr,"\t-u x,y,z\tset upvector\n");
    fprintf(stderr,"\t-q n    \tset cache size to n frames (default=1)\n");
    fprintf(stderr,"\t-c filename\tspecify palette file(default: yaplot.col)\n");
    fprintf(stderr,"\t If no palette file is found, envvar YAPLOTCOL is checked,\n");
    fprintf(stderr,"\t and then $HOME/yaplot.col is searched.\n");
    g->nwindow=1;
    w=W_Init(1);
    strcpy(w[0].inputfilename,HELPFILE);
	
  }
  else{
    g->nwindow=argc-optind;
    w=W_Init(g->nwindow);
    for(i=0;i<g->nwindow;i++){
      strcpy(w[i].inputfilename,argv[i+optind]);
    }
  }
  strcpy(g->commandname,argv[0]);
  nstdin=0;
  for(i=0;i<g->nwindow;i++){
    if(strcmp(w[i].inputfilename,"-")==0)
      {
	w[i].inputfile=stdin;
	nstdin++;
	if(nstdin>1){
	  fprintf(stderr,"%s : Two \"-\"s are specified at a time.\n",g->commandname);
	  exit(-1);
	}
      }
    else
      if((w[i].inputfile = fopen(w[i].inputfilename,"r"))==NULL)
	{
	  fprintf(stderr,"%s : Cannot find file : %s\n",g->commandname,w[i].inputfilename);
	  exit(-1);
	}
      
  }
  return w;

}

int ePushView(Ginfo *g,Winfo w[],int i)
{
  FILE *file=fopen(VIEWFILE,"a+");
  char buf[1000];
  if ( NULL == file ){
    fprintf(stderr, "Failed to open %s for writing.\n", VIEWFILE);
    return FALSE;
  }
  sprintf(buf,"%f %f %f %f %f %f %f %f %f %f %f\n",
	  w[i].fov,
	  w[i].depthratio,
	  w[i].eyep[0],
	  w[i].eyep[1],
	  w[i].eyep[2],
	  w[i].lookp[0],
	  w[i].lookp[1],
	  w[i].lookp[2],
	  w[i].up[0],
	  w[i].up[1],
	  w[i].up[2]);
  fputs(buf,file);
  printf("# view info\n%d %d ", w[i].currentframe,w[i].layermask);
  fputs(buf,stdout);
  fclose(file);
  return TRUE;
}

int ePopView(Ginfo *g,Winfo w[],int i)
{
  long pos1=-1,pos2=0;
  char buf[1024];
  FILE *file=fopen(VIEWFILE,"r+");
  /*return false if open failed.*/
  if(file==NULL){
    return FALSE;
  }
  while(NULL!=fgets(buf,sizeof(buf),file)){
    pos1=pos2;
    pos2=ftell(file);
  }
  fclose(file);
  truncate(VIEWFILE,pos1);
  if(pos1>=0){
    sscanf(buf,"%f %f %f %f %f %f %f %f %f %f %f\n",
	   &w[i].fov,
	   &w[i].depthratio,
	   &w[i].eyep[0],
	   &w[i].eyep[1],
	   &w[i].eyep[2],
	   &w[i].lookp[0],
	   &w[i].lookp[1],
	   &w[i].lookp[2],
	   &w[i].up[0],
	   &w[i].up[1],
	   &w[i].up[2]);
    W_setvectors(&w[i]);
    w[i].status|=REDRAW;
    return TRUE;
  }
  return FALSE;
}
