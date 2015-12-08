/* =============================================================================
**  This file is part of the mmg software package for the tetrahedral
**  mesh modification.
**  Copyright (c) Inria - IMB (Université de Bordeaux) - LJLL (UPMC), 2004- .
**
**  mmg is free software: you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published
**  by the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  mmg is distributed in the hope that it will be useful, but WITHOUT
**  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
**  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
**  License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License and of the GNU General Public License along with mmg (in
**  files COPYING.LESSER and COPYING). If not, see
**  <http://www.gnu.org/licenses/>. Please read their terms carefully and
**  use this copy of the mmg distribution only if you accept them.
** =============================================================================
*/
#ifndef _MMG2D_H
#define _MMG2D_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <float.h>

#include "libmmg2d.h"

/* constantes */
#define M_VER   "2.0"
#define M_REL   "SEPTEMBER 2014"
#define M_CPY   "Copyright (c) LJLL/IMB, 2007-"
#define M_STR   "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"

#define M_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define M_MIN(a,b) (((a) < (b)) ? (a) : (b))

#define M_LAMBDA  0.34
#define M_MU      0.33

#define EPS30  1.e-30
#define EPSD   1.e-10 //e-20??
#define EPSA   1.e-12
#define TGV    1.e15
#define PRECI  1.
#define SIZE    0.75
#define COS90   0.0
#define ALPHA  0.28867513459
#define ALPHAD 3.464101615137755   /* 6.0 / sqrt(3.0)  */
#define MMG2_LONMAX 1024
#define _MMG5_BADKAL    0.2

#define M_NUL      (1 << 6)
#define M_BDRY     (1 << 1)
#define M_SD       (1 << 5)
#define M_CORNER   (1 << 4) 
#define M_REQUIRED (1 << 3)
#define M_MOVE     (1 << 2)

#define _MMG2D_NPMAX   50000
#define _MMG2D_NEDMAX  100000
#define _MMG2D_NEMAX   100000
#define LMAX   1024

#define M_VOK(ppt)    (ppt && (ppt->tag < M_NUL))
#define M_EOK(pt)     (pt && (pt->v[0] > 0))

/** Free allocated pointers of mesh and sol structure and return value val */
#define _MMG2D_RETURN_AND_FREE(mesh,met,val)do       \
  {                                                 \
    MMG2D_Free_all(mesh,met);                        \
    return(val);                                    \
  }while(0)

/**
 * \param sigid signal number.
 *
 * Signal handling: specify error messages depending from catched signal.
 *
 */
static inline 
void _MMG2_excfun(int sigid) {
  fprintf(stdout,"\n Unexpected error:");  fflush(stdout);
  switch(sigid) {
  case SIGABRT:
    fprintf(stdout,"  Abnormal stop\n");  exit(1);
  case SIGFPE:
    fprintf(stdout,"  Floating-point exception\n"); exit(1);
  case SIGILL:
    fprintf(stdout,"  Illegal instruction\n"); exit(1);
  case SIGSEGV:
    fprintf(stdout,"  Segmentation fault\n");  exit(1);
  case SIGTERM:
  case SIGINT:
    fprintf(stdout,"  Program killed\n");  exit(1);
  }
  exit(1);
}


/* /\* data structures *\/ */
/* typedef struct { */
/*   double    c[2]; */
/*   int       tmp,mark,tas; */
/*   int       ref; */
/*   int       tag,tagdel; */
/*   char      flag; */
/* } Point; */
/* typedef Point * MMG5_pPoint; */

/* typedef struct { */
/*   double    qual; */
/*   int       v[3],ref,flag,mark; */
/*   int       ned[3]; //if 0 no bdry edge else edg number */
/* } Tria; */
/* typedef Tria * MMG5_pTria; */

/* typedef struct { */
/*   int       v[2],ref,flag,mark;  */
/*   double    t0[2],t1[2]; //tangents */
/*   char	    tag;  */
/* } Edge; */
/* typedef Edge * pEdge; */

/* typedef struct { */
/*   double   delta,hgrad,ridge,ang; */
/*   double   min[2],max[2],qdegrad[2]; */
/*   int      mem,bucket,nsd,msh; */
/*   char     imprim,option,ddebug,noswap,nomove,nr,noinsert,per; */
/* } Info; */

/* typedef struct { */
/*   int      np,ver; */
/*   double   *mv; */
/*   short    *alpha;   */
/* } Displ; */

/* typedef struct { */
/*   int        np,nt,ned,npfixe,nedfixe,ntfixe,npmax,nedmax,ntmax; */
/*   int        npnil,nednil,ntnil,ver,dim,mark; */
/*   int       *adja; */
/*   MMG5_pPoint     point; */
/*   Displ      disp; */
/*   MMG5_pTria      tria; */
/*   pEdge		 edge; */
/*   Info       info; */
/*   char       flag; */
/*   char      *namein,*nameout,*namedep; */
/* } Mesh; */
/* typedef Mesh * MMG5_pMesh; */

/* typedef struct { */
/*   double    *met,hmin,hmax; */
/*   int        np,dim,ver,type,size,typtab[20]; */
/*   char      *name; */
/* } Sol; */
/* typedef Sol * MMG5_pSol; */

typedef struct squeue {
  int    *stack,cur;
} Queue;
typedef Queue * pQueue;

typedef struct {
  int     size;
  int    *head;
  int    *link;
} Bucket;
typedef Bucket * pBucket;

typedef struct {
  int      min,max,iel,nxt;
} Hedge;

typedef struct {
  int      size,nxtmax,hnxt;
  Hedge    *item;
} HashTable;
typedef HashTable * pHashTable;

static const int MMG2_iare[3][2] = {{1,2},{2,0},{0,1}};
static const int MMG2_iopp[3][2] = {{1,2},{0,2},{0,1}};
static const unsigned int MMG2_idir[5] = {0,1,2,0,1};
static const unsigned int MMG2_inxt[5] = {1,2,0,1,2};


/** Reallocation of point table and sol table and creation
    of point ip with coordinates o and tag tag*/
#define _MMG5_POINT_REALLOC(mesh,sol,ip,wantedGap,law,o,tag ) do        \
  {                                                                     \
    int klink;                                                          \
                                                                        \
    _MMG5_TAB_RECALLOC(mesh,mesh->point,mesh->npmax,wantedGap,MMG5_Point, \
                       "larger point table",law);                       \
                                                                        \
    mesh->npnil = mesh->np+1;                                           \
    for (klink=mesh->npnil; klink<mesh->npmax-1; klink++)               \
      mesh->point[klink].tmp  = klink+1;                                \
                                                                        \
    /* solution */                                                      \
    if ( sol->m ) {                                                     \
      _MMG5_ADD_MEM(mesh,(sol->size*(mesh->npmax-sol->npmax))*sizeof(double), \
                    "larger solution",law);                             \
      _MMG5_SAFE_REALLOC(sol->m,sol->size*(mesh->npmax+1),double,"larger solution"); \
    }                                                                   \
    sol->npmax = mesh->npmax;                                           \
                                                                        \
    /* We try again to add the point */                                 \
    ip = _MMG2D_newPt(mesh,o,tag);                                       \
    if ( !ip ) {law;}                                                   \
  }while(0)

/** Reallocation of tria table and creation
    of tria jel */
#define _MMG5_TRIA_REALLOC(mesh,jel,wantedGap,law ) do                 \
  {                                                                     \
    int klink,oldSiz;                                                   \
                                                                        \
    oldSiz = mesh->ntmax;                                               \
    _MMG5_TAB_RECALLOC(mesh,mesh->tria,mesh->ntmax,wantedGap,MMG5_Tria, \
                       "larger tria table",law);                       \
                                                                        \
    mesh->nenil = mesh->nt+1;                                           \
    for (klink=mesh->nenil; klink<mesh->ntmax-1; klink++)               \
      mesh->tria[klink].v[2]  = klink+1;                               \
                                                                        \
    if ( mesh->adja ) {                                                 \
      /* adja table */                                                  \
      _MMG5_ADD_MEM(mesh,3*(mesh->ntmax-oldSiz)*sizeof(int),            \
                    "larger adja table",law);                           \
      _MMG5_SAFE_RECALLOC(mesh->adja,3*mesh->nt+5,3*mesh->ntmax+5,int   \
                          ,"larger adja table");                        \
    }                                                                   \
                                                                        \
    /* We try again to add the point */                                 \
    jel = _MMG2D_newElt(mesh);                                           \
    if ( !jel ) {law;}                                                  \
  }while(0)

/** Reallocation of edge table and creation
    of edge jel */
#define _MMG5_EDGE_REALLOC(mesh,jel,wantedGap,law ) do                 \
  {                                                                     \
    int klink;                                                          \
                                                                        \
    _MMG5_TAB_RECALLOC(mesh,mesh->edge,mesh->namax,wantedGap,MMG5_Edge, \
                       "larger edge table",law);                       \
                                                                        \
    mesh->nanil = mesh->na+1;                                           \
    for (klink=mesh->nanil; klink<mesh->namax-1; klink++)               \
      mesh->edge[klink].b  = klink+1;                               \
                                                                        \
                                                                        \
    /* We try again to add the point */                                 \
    jel = _MMG5_newEdge(mesh);                                           \
    if ( !jel ) {law;}                                                  \
  }while(0)


/* prototypes */
/*zaldy*/
int _MMG2D_newPt(MMG5_pMesh mesh,double c[2],int tag);
void _MMG2D_delPt(MMG5_pMesh mesh,int ip) ;
int _MMG5_newEdge(MMG5_pMesh mesh);
void _MMG5_delEdge(MMG5_pMesh mesh,int iel);
int _MMG2D_newElt(MMG5_pMesh mesh);
void _MMG2D_delElt(MMG5_pMesh mesh,int iel);
int _MMG5_getnElt(MMG5_pMesh mesh,int n);
int MMG2_zaldy(MMG5_pMesh mesh);
long long _MMG5_memSize(void);
void _MMG2D_memOption(MMG5_pMesh mesh);

int MMG2_scaleMesh(MMG5_pMesh ,MMG5_pSol );
int MMG2_unscaleMesh(MMG5_pMesh ,MMG5_pSol );
void MMG2_outqua(MMG5_pMesh ,MMG5_pSol );
int MMG2_mmg2d0(MMG5_pMesh ,MMG5_pSol );
int MMG2_mmg2d1(MMG5_pMesh ,MMG5_pSol );
int MMG2_split(MMG5_pMesh ,MMG5_pSol ,int ,int ,int );
int MMG2_splitbdry(MMG5_pMesh ,MMG5_pSol ,int ,int ,int,double*);
int MMG2_colpoi(MMG5_pMesh ,MMG5_pSol , int ,int ,int ,int ,double );
int MMG2_colpoibdry(MMG5_pMesh ,MMG5_pSol , int ,int ,int ,int ,double );


int MMG2_mmg2d2(MMG5_pMesh , MMG5_pSol);
int MMG2_mmg2d6(MMG5_pMesh ,MMG5_pSol );
int MMG2_mmg2d9(MMG5_pMesh ,MMG5_pSol );
int MMG2_cendel(MMG5_pMesh ,MMG5_pSol ,double ,int );
int MMG2_swapar(MMG5_pMesh ,MMG5_pSol ,int ,int ,double ,int *);
int _MMG5_mmg2dChkmsh(MMG5_pMesh , int, int );
int MMG2_boulep(MMG5_pMesh , int , int , int * );
int MMG2_markBdry(MMG5_pMesh ); 
int MMG2_doSol(MMG5_pMesh ,MMG5_pSol );
int MMG2_prilen(MMG5_pMesh ,MMG5_pSol );

void MMG2_coorbary(MMG5_pMesh ,MMG5_pTria ,double c[2],double* ,double* ,double* );
int MMG2_isInTriangle(MMG5_pMesh ,int,double c[2]);
int MMG2_cutEdge(MMG5_pMesh ,MMG5_pTria ,MMG5_pPoint ,MMG5_pPoint );
int MMG2_cutEdgeTriangle(MMG5_pMesh ,int ,int ,int );
int MMG2_findTria(MMG5_pMesh ,int );  
int MMG2_findpos(MMG5_pMesh ,MMG5_pTria ,int ,int ,int ,int ,int );
int MMG2_locateEdge(MMG5_pMesh ,int ,int ,int* ,int* ) ;
int MMG2_bdryenforcement(MMG5_pMesh ,MMG5_pSol);
int MMG2_insertpoint(MMG5_pMesh ,MMG5_pSol );
int MMG2_settagtriangles(MMG5_pMesh ,MMG5_pSol );
int MMG2_findtrianglestate(MMG5_pMesh ,int ,int ,int ,int ,int ,int );

pQueue MMG2_kiuini(MMG5_pMesh mesh,int nbel,double declic,int base);
void MMG2_kiufree(pQueue q);
int MMG2_kiudel(pQueue q,int iel);
int MMG2_kiuput(pQueue q,int iel);
int MMG2_kiupop(pQueue q);

pBucket MMG2_newBucket(MMG5_pMesh mesh,int nmax);
void MMG2_freeBucket(pBucket bucket);
int  MMG2_addBucket(MMG5_pMesh mesh,pBucket bucket,int ip);
int  MMG2_delBucket(MMG5_pMesh mesh,pBucket bucket,int ip);

int MMG2_hashEdge(pHashTable edgeTable,int iel,int ia, int ib);
int MMG2_hashel(MMG5_pMesh mesh);
int MMG2_hashNew(HashTable *hash,int hsize,int hmax);
int MMG2_baseBdry(MMG5_pMesh mesh);

int MMG2_invmat(double *m,double *minv);
int simred(double *m1,double *m2,double *m);

int MMG2_evalgeom(MMG5_pMesh mesh);

int _MMG2_cavity(MMG5_pMesh ,MMG5_pSol ,int ,int *);
int _MMG2_delone(MMG5_pMesh ,MMG5_pSol ,int ,int *,int );
int _MMG2_cenrad_iso(MMG5_pMesh ,double *,double *,double *);

/* functions pointers */
double long_ani(double *ca,double *cb,double *ma,double *mb);
double long_iso(double *ca,double *cb,double *ma,double *mb);
double MMG2_quickarea(double a[2],double b[2],double c[2]);
double caltri_ani(MMG5_pMesh mesh,MMG5_pSol sol,MMG5_pTria );
double caltri_iso(MMG5_pMesh mesh,MMG5_pSol sol,MMG5_pTria );
double caltri_ani_in(MMG5_pMesh mesh,MMG5_pSol sol,MMG5_pTria );
double caltri_iso_in(MMG5_pMesh mesh,MMG5_pSol sol,MMG5_pTria );
int    optlen_ani(MMG5_pMesh mesh,MMG5_pSol sol,double declic,int base);
int    optlen_iso(MMG5_pMesh mesh,MMG5_pSol sol,double declic,int base);
int    interp_ani(double *,double *,double * ,double );
int    interp_iso(double *,double *,double * ,double );
int    buckin_iso(MMG5_pMesh mesh,MMG5_pSol sol,pBucket bucket,int ip);
int    buckin_ani(MMG5_pMesh mesh,MMG5_pSol sol,pBucket bucket,int ip);
int    lissmet_iso(MMG5_pMesh mesh,MMG5_pSol sol);
int    lissmet_ani(MMG5_pMesh mesh,MMG5_pSol sol);

int MMG2_chkedg(MMG5_pMesh mesh, MMG5_pPoint ppa,MMG5_pPoint ppb) ;

double (*MMG2_length)(double *,double *,double *,double *);
double (*MMG2_caltri)(MMG5_pMesh ,MMG5_pSol ,MMG5_pTria );
double (*MMG2_caltri_in)(MMG5_pMesh ,MMG5_pSol ,MMG5_pTria );
int    (*MMG2_optlen)(MMG5_pMesh ,MMG5_pSol ,double ,int );
int    (*MMG2_interp)(double *,double *,double *,double );
int    (*MMG2_buckin)(MMG5_pMesh ,MMG5_pSol ,pBucket ,int );
int    (*MMG2_lissmet)(MMG5_pMesh ,MMG5_pSol );

int MMG2_tassage(MMG5_pMesh ,MMG5_pSol );

/* init structures */
void  _MMG2_Init_parameters(MMG5_pMesh mesh);

/**
 * Set common pointer functions between mmgs and mmg3d to the matching mmg3d
 * functions.
 */
static inline
void _MMG2D_Set_commonFunc() {
  _MMG5_chkmsh            = _MMG5_mmg2dChkmsh;
  return;
}


#endif
