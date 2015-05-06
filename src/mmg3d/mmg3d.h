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

#ifndef _MMG3D_H
#define _MMG3D_H

#include "libmmg3d.h"

#define MG_SMSGN(a,b)  (((double)(a)*(double)(b) > (0.0)) ? (1) : (0))

/** Reallocation of point table, sol table and bucket table and creation
    of point ip with coordinates o and tag tag*/
#define _MMG5_POINT_AND_BUCKET_REALLOC(mesh,sol,ip,wantedGap,law,o,tag ) do \
  {                                                                     \
    int klink,gap;                                                      \
                                                                        \
    if ( (mesh->memMax-mesh->memCur) <                                  \
         (long long) (wantedGap*mesh->npmax*                            \
                      (sizeof(MMG5_Point)+sol->size*sizeof(int))) ) {   \
      gap = (int)((mesh->memMax-mesh->memCur)/                          \
                  (sizeof(MMG5_Point)+sol->size*sizeof(int)));          \
      if(gap < 1) {                                                     \
        fprintf(stdout,"  ## Error:");                                  \
        fprintf(stdout," unable to allocate %s.\n","larger point/bucket table"); \
        fprintf(stdout,"  ## Check the mesh size or ");                 \
        fprintf(stdout,"increase maximal authorized memory with the -m option.\n"); \
        law;                                                            \
      }                                                                 \
    }                                                                   \
    else                                                                \
      gap = (int)(wantedGap*mesh->npmax);                               \
                                                                        \
    _MMG5_ADD_MEM(mesh,gap*(sizeof(MMG5_Point)+sizeof(int)),            \
                  "point and bucket",law);                              \
    _MMG5_SAFE_RECALLOC(mesh->point,mesh->npmax+1,                      \
                        mesh->npmax+gap+1,MMG5_Point,"larger point table"); \
    _MMG5_SAFE_RECALLOC(bucket->link,mesh->npmax+1,                     \
                        mesh->npmax+gap+1,int,"larger bucket table");   \
    mesh->npmax = mesh->npmax+gap;                                      \
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
    ip = _MMG5_newPt(mesh,o,tag);                                       \
    if ( !ip ) {law;}                                                   \
  }while(0)

/** Reallocation of tetra table and creation
    of tetra jel */
#define _MMG5_TETRA_REALLOC(mesh,jel,wantedGap,law ) do                 \
  {                                                                     \
    int klink,oldSiz;                                                   \
                                                                        \
    oldSiz = mesh->nemax;                                               \
    _MMG5_TAB_RECALLOC(mesh,mesh->tetra,mesh->nemax,wantedGap,MMG5_Tetra, \
                       "larger tetra table",law);                       \
                                                                        \
    mesh->nenil = mesh->ne+1;                                           \
    for (klink=mesh->nenil; klink<mesh->nemax-1; klink++)               \
      mesh->tetra[klink].v[3]  = klink+1;                               \
                                                                        \
    if ( mesh->adja ) {                                                 \
      /* adja table */                                                  \
      _MMG5_ADD_MEM(mesh,4*(mesh->nemax-oldSiz)*sizeof(int),            \
                    "larger adja table",law);                           \
      _MMG5_SAFE_RECALLOC(mesh->adja,4*mesh->ne+5,4*mesh->nemax+5,int   \
                          ,"larger adja table");                        \
    }                                                                   \
                                                                        \
    /* We try again to add the point */                                 \
    jel = _MMG5_newElt(mesh);                                           \
    if ( !jel ) {law;}                                                  \
  }while(0)

/* numerical accuracy */
#define _MMG5_ALPHAD    20.7846096908265    //0.04811252243247      /* 12*sqrt(3) */
#define _MMG5_LLONG     2.5//2.0   // 1.414213562373
#define _MMG5_LSHRT     0.3  // 0.707106781186
#define _MMG5_LOPTL     1.3
#define _MMG5_LOPTS     0.6
#define _MMG5_LMAX      10240
#define _MMG5_BADKAL    0.2
#define _MMG5_NULKAL    1.e-30
#ifdef SINGUL
#define _MMG5_EPS2      1.e-12
#endif

#define _MMG5_NPMAX  1000000 //200000
#define _MMG5_NAMAX   200000 //40000
#define _MMG5_NTMAX  2000000 //400000
#define _MMG5_NEMAX  6000000 //1200000

#define _MMG5_BOXSIZE 500


/* mmg3d tags */
#ifdef SINGUL
#define  MG_SGL       (1 << 7)        /*!< 128 inserted singularity */
#endif

/* Domain refs in iso mode */
#define MG_PLUS    2
#define MG_MINUS   3

extern unsigned char _MMG5_inxt3[7];   /*!< next vertex of tetra: {1,2,3,0,1,2,3} */
extern unsigned char _MMG5_iprv3[7];   /*!< previous vertex of tetra: {3,0,1,2,3,0,1} */
extern unsigned char _MMG5_idir[4][3]; /*!< idir[i] : vertices of face opposite to vertex i */
extern          char _MMG5_idirinv[4][4]; /*!< idirinv[i][j] : num of the jth point in the ith face */
extern unsigned char _MMG5_iarf[4][3]; /*!< iarf[i]: edges of face opposite to vertex i */
extern unsigned char _MMG5_iarfinv[4][6]; /*!< num of the j^th edge in the i^th face */
extern unsigned char _MMG5_iare[6][2]; /*!< vertices of extremities of the edges of the tetra */
extern unsigned char _MMG5_ifar[6][2]; /*!< ifar[i][]: faces sharing the ith edge of the tetra */
extern unsigned char _MMG5_isar[6][2]; /*!< isar[i][]: vertices of extremities of the edge opposite to the ith edge */
extern unsigned char _MMG5_arpt[4][3]; /*!< arpt[i]: edges passing through vertex i */


typedef struct {
  int     size;
  int    *head;
  int    *link;
} _MMG5_Bucket;
typedef _MMG5_Bucket * _MMG5_pBucket;

#ifdef SINGUL
typedef struct {
  double   c[3],cb[4]; /**< c: coor of entrance/exit point, cb: bary coor of entrance/exit */
  int      kel,key; /**< kel: elt in which we travel, key: location of entrance or exit */
  int      np; /**< global indice of entrance point */
  char     tag; /**< tag of edge */

} _MMG5_Travel;
#endif

/* bucket */
_MMG5_pBucket _MMG5_newBucket(MMG5_pMesh ,int );
int     _MMG5_addBucket(MMG5_pMesh ,_MMG5_pBucket ,int );
int     _MMG5_delBucket(MMG5_pMesh ,_MMG5_pBucket ,int );
int     _MMG5_buckin_iso(MMG5_pMesh mesh,MMG5_pSol sol,_MMG5_pBucket bucket,int ip);
/* prototypes */
double _MMG5_det3pt1vec(double c0[3],double c1[3],double c2[3],double v[3]);
double _MMG5_det4pt(double c0[3],double c1[3],double c2[3],double c3[3]);
double _MMG5_orvol(MMG5_pPoint point,int *v);
int _MMG5_directsurfball(MMG5_pMesh mesh, int ip, int *list, int ilist, double n[3]);

int  _MMG5_newPt(MMG5_pMesh mesh,double c[3],char tag);
int  _MMG5_newElt(MMG5_pMesh mesh);
void _MMG5_delElt(MMG5_pMesh mesh,int iel);
void _MMG5_delPt(MMG5_pMesh mesh,int ip);
int  _MMG5_zaldy(MMG5_pMesh mesh);
void _MMG5_freeXTets(MMG5_pMesh mesh);
char _MMG5_chkedg(MMG5_pMesh mesh,MMG5_pTria pt,char ori);
int  _MMG5_chkNumberOfTri(MMG5_pMesh mesh);
void _MMG5_tet2tri(MMG5_pMesh mesh,int k,char ie,MMG5_Tria *ptt);
int    _MMG5_mmg3dBezierCP(MMG5_pMesh mesh,MMG5_Tria *pt,_MMG5_pBezier pb,char ori);
int    _MMG5_BezierTgt(double c1[3],double c2[3],double n1[3],double n2[3],double t1[3],double t2[3]);
double _MMG5_BezierGeod(double c1[3], double c2[3], double t1[3], double t2[3]);
int  _MMG5_bezierInt(_MMG5_pBezier pb,double uv[2],double o[3],double no[3],double to[3]);
int  _MMG5_BezierReg(MMG5_pMesh mesh,int ip0, int ip1, double s, double v[3], double *o, double *no);
int  _MMG5_BezierRef(MMG5_pMesh mesh,int ip0, int ip1, double s, double *o, double *no, double *to);
int  _MMG5_BezierEdge(MMG5_pMesh mesh,int ip0, int ip1, double b0[3], double b1[3],char isrid, double v[3]);
int  _MMG5_BezierRidge(MMG5_pMesh mesh,int ip0, int ip1, double s, double *o, double *no1, double *no2, double *to);
int  _MMG5_BezierNom(MMG5_pMesh mesh,int ip0,int ip1,double s,double *o,double *no,double *to);
int  _MMG5_norface(MMG5_pMesh mesh ,int k, int iface, double v[3]);
int  _MMG5_boulenm(MMG5_pMesh mesh, int start, int ip, int iface, double n[3],double t[3]);
int  _MMG5_boulevolp(MMG5_pMesh mesh, int start, int ip, int * list);
int  _MMG5_boulesurfvolp(MMG5_pMesh mesh,int start,int ip,int iface,int *listv,
                         int *ilistv,int *lists,int*ilists, int isnm);
int  _MMG5_bouletrid(MMG5_pMesh,int,int,int,int *,int *,int *,int *,int *,int *);
int  _MMG5_startedgsurfball(MMG5_pMesh mesh,int nump,int numq,int *list,int ilist);
int  _MMG5_srcbdy(MMG5_pMesh mesh,int start,int ia);
int  _MMG5_coquil(MMG5_pMesh mesh, int start, int ia, int * list);
int  _MMG5_coquilface(MMG5_pMesh mesh, int start, int ia, int * list, int * it1, int *it2);
int  _MMG5_settag(MMG5_pMesh,int,int,int,int);
int  _MMG5_chkcol_int(MMG5_pMesh ,MMG5_pSol met,int,char,char,int *,char typchk);
int  _MMG5_chkcol_bdy(MMG5_pMesh,MMG5_pSol met,int,char,char,int *);
int  _MMG5_chkmanicoll(MMG5_pMesh mesh,int k,int iface,int iedg,int ndepmin,int ndepplus,char isminp,char isplp);
int  _MMG5_chkmani(MMG5_pMesh mesh);
int  _MMG5_colver(MMG5_pMesh,MMG5_pSol,int *,int,char);
int  _MMG5_analys(MMG5_pMesh mesh);
int  _MMG5_hashTetra(MMG5_pMesh mesh, int pack);
int  _MMG5_hashTria(MMG5_pMesh mesh);
int  _MMG5_hashPop(_MMG5_Hash *hash,int a,int b);
int  _MMG5_hPop(MMG5_HGeom *hash,int a,int b,int *ref,char *tag);
int  _MMG5_hTag(MMG5_HGeom *hash,int a,int b,int ref,char tag);
int  _MMG5_hGet(MMG5_HGeom *hash,int a,int b,int *ref,char *tag);
void _MMG5_hEdge(MMG5_pMesh mesh,int a,int b,int ref,char tag);
int  _MMG5_hNew(MMG5_HGeom *hash,int hsiz,int hmax,int secure);
int  _MMG5_hGeom(MMG5_pMesh mesh);
int  _MMG5_bdryTria(MMG5_pMesh );
int  _MMG5_bdryIso(MMG5_pMesh );
int  _MMG5_bdrySet(MMG5_pMesh );
int  _MMG5_bdryUpdate(MMG5_pMesh );
int  _MMG5_bdryPerm(MMG5_pMesh );
int  _MMG5_chkfemtopo(MMG5_pMesh mesh);
int  _MMG5_cntbdypt(MMG5_pMesh mesh, int nump);
long long _MMG5_memSize(void);
void _MMG5_memOption(MMG5_pMesh mesh);
int  _MMG5_mmg3d1_pattern(MMG5_pMesh ,MMG5_pSol );
int  _MMG5_mmg3d1_delone(MMG5_pMesh ,MMG5_pSol );
int  _MMG5_mmg3d2(MMG5_pMesh ,MMG5_pSol );
int  _MMG5_mmg3d3(MMG5_pMesh ,MMG5_pSol );
int  _MMG5_mmg3dChkmsh(MMG5_pMesh,int,int);
int  _MMG5_split1_sim(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split1(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
int  _MMG5_split1b(MMG5_pMesh mesh,MMG5_pSol met,int *list,int ret,int ip,int cas);
int  _MMG5_split2sf_sim(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split2sf(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split2(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
int  _MMG5_split3_sim(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split3(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split3cone(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split3op(MMG5_pMesh mesh, MMG5_pSol met, int k, int vx[6]);
void _MMG5_split4sf(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split4op(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split5(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
void _MMG5_split6(MMG5_pMesh mesh,MMG5_pSol met,int k,int vx[6]);
int  _MMG5_split4bar(MMG5_pMesh mesh,MMG5_pSol met,int k);
int  _MMG5_simbulgept(MMG5_pMesh mesh,MMG5_pSol met, int *list, int ilist, double o[3]);
int  _MMG5_dichoto1b(MMG5_pMesh mesh,MMG5_pSol met,int *list,int ret,double o[3],double ro[3]);
void _MMG5_nsort(int ,double *,char *);
double _MMG5_orcal(MMG5_pMesh mesh,MMG5_pSol met,int iel);
int    _MMG5_movintpt(MMG5_pMesh ,MMG5_pSol, int *, int , int );
int    _MMG5_movbdyregpt(MMG5_pMesh, MMG5_pSol, int*, int, int*, int);
int    _MMG5_movbdyrefpt(MMG5_pMesh, MMG5_pSol, int*, int, int*, int);
int    _MMG5_movbdynompt(MMG5_pMesh, MMG5_pSol, int*, int, int*, int);
int    _MMG5_movbdyridpt(MMG5_pMesh, MMG5_pSol, int*, int, int*, int);
int  _MMG5_chkswpbdy(MMG5_pMesh, MMG5_pSol,int*, int, int, int);
int  _MMG5_swpbdy(MMG5_pMesh mesh,MMG5_pSol met,int *list,int ret,int it1,_MMG5_pBucket bucket);
int  _MMG5_swpgen(MMG5_pMesh mesh,MMG5_pSol met,int nconf, int ilist, int *list,_MMG5_pBucket bucket);
int  _MMG5_chkswpgen(MMG5_pMesh mesh,MMG5_pSol met, int start, int ia, int *ilist, int *list,double crit);
int  _MMG5_srcface(MMG5_pMesh mesh,int n0,int n1,int n2);
int _MMG5_chkptonbdy(MMG5_pMesh,int);
double _MMG5_orcal_poi(double a[3],double b[3],double c[3],double d[3]);
int _MMG5_countelt(MMG5_pMesh mesh,MMG5_pSol sol, double *weightelt, long *npcible);
int _MMG5_trydisp(MMG5_pMesh,double *,short);
int _MMG5_dichodisp(MMG5_pMesh,double *);
int _MMG5_lapantilap(MMG5_pMesh,double *);
int _MMG5_ppgdisp(MMG5_pMesh,double *);
int _MMG5_denoisbdy(MMG5_pMesh);
void _MMG5_outqua(MMG5_pMesh mesh,MMG5_pSol met);
int  _MMG5_badelt(MMG5_pMesh mesh,MMG5_pSol met);
int _MMG5_prilen(MMG5_pMesh mesh,MMG5_pSol met);
int _MMG5_DoSol(MMG5_pMesh mesh,MMG5_pSol met);
void _MMG5_defaultValues(MMG5_pMesh);
/* useful functions to debug */
int  _MMG5_indElt(MMG5_pMesh mesh,int kel);
int  _MMG5_indPt(MMG5_pMesh mesh,int kp);
void _MMG5_printTetra(MMG5_pMesh mesh,char* fileName);
int  _MMG5_saveAllMesh(MMG5_pMesh mesh);
int  _MMG5_saveLibraryMesh(MMG5_pMesh mesh);

#ifdef USE_SCOTCH
int _MMG5_mmg3dRenumbering(int vertBoxNbr, MMG5_pMesh mesh, MMG5_pSol sol);
#endif

#ifdef SINGUL
int  _MMG5_inserSingul(MMG5_pMesh mesh, MMG5_pSol met, MMG5_pSingul singul);
int  _MMG5_creaEdge(MMG5_pMesh mesh, MMG5_pSol met, _MMG5_Travel *trav, char tag);
int  _MMG5_creaPoint(MMG5_pMesh mesh, MMG5_pSol met,int iel, double c[3], double cb[4], char tag);
int  _MMG5_colSing(MMG5_pMesh mesh,MMG5_pSol met);
int  _MMG5_seekEdge(MMG5_pMesh mesh, MMG5_pSol met, MMG5_psPoint ppt0, MMG5_psPoint ppt1,
                    _MMG5_Travel *trav, int *lastet,int*,int*);
int  _MMG5_seekPoint(MMG5_pMesh mesh, MMG5_psPoint ppt, double cb[4],int*,int*);
int  _MMG5_solveUnsignedTet(MMG5_pMesh mesh,MMG5_pSol met);
int  _MMG5_split3cb(MMG5_pMesh mesh, MMG5_pSol met, int k, int ifac, double o[3],double cb[4], int *ip);
int  _MMG5_split4cb(MMG5_pMesh mesh, MMG5_pSol met, int k, double o[3], double cb[4],int *ip);
int  _MMG5_swap23(MMG5_pMesh mesh,int k, int ip);
#endif

int    _MMG5_meancur(MMG5_pMesh mesh,int np,double c[3],int ilist,int *list,double h[3]);
double _MMG5_surftri(MMG5_pMesh,int,int);
double _MMG5_timestepMCF(MMG5_pMesh,double);
int    _MMG5_bdyMCF(MMG5_pMesh);
double _MMG5_volint(MMG5_pMesh);

/* Delaunay functions*/
int _MMG5_delone(MMG5_pMesh mesh,MMG5_pSol sol,int ip,int *list,int ilist);
int _MMG5_cavity(MMG5_pMesh mesh,MMG5_pSol sol,int iel,int ip,int *list,int lon);
int _MMG5_cenrad_iso(MMG5_pMesh mesh,double *ct,double *c,double *rad);

/*mmg3d1.c*/
void _MMG5_tet2tri(MMG5_pMesh mesh,int k,char ie,MMG5_Tria *ptt);
int  _MMG5_dichoto(MMG5_pMesh mesh,MMG5_pSol met,int k,int *vx);
/** Find acceptable position for split1b, passing the shell of considered edge, starting from o */
int  _MMG5_dichoto1b(MMG5_pMesh mesh,MMG5_pSol met,int *list,int ret,double o[3],double ro[3]);
/** return edges of (virtual) triangle pt that need to be split w/r Hausdorff criterion */
char _MMG5_chkedg(MMG5_pMesh mesh,MMG5_Tria *pt,char ori);
int  _MMG5_anatet(MMG5_pMesh mesh,MMG5_pSol met,char typchk, int patternMode) ;
int  _MMG5_movtet(MMG5_pMesh mesh,MMG5_pSol met,int maxitin);
int  _MMG5_swpmsh(MMG5_pMesh mesh,MMG5_pSol met,_MMG5_pBucket bucket);
int  _MMG5_swptet(MMG5_pMesh mesh,MMG5_pSol met,double crit,_MMG5_pBucket bucket);

/* pointers */
/* init structures */
void  _MMG5_Init_parameters(MMG5_pMesh mesh);
/* iso/aniso computations */
double _MMG5_caltet_ani(MMG5_pMesh mesh,MMG5_pSol met,int ia,int ib,int ic,int id);
double _MMG5_caltet_iso(MMG5_pMesh mesh,MMG5_pSol met,int ia,int ib,int ic,int id);
double _MMG5_lenedgCoor_ani(double*, double*, double*, double*);
double _MMG5_lenedgCoor_iso(double*, double*, double*, double*);
int    _MMG5_intmetvol_ani(double*,double*,double*,double );
int    _MMG5_interp4bar_ani(MMG5_pMesh,MMG5_pSol,int,int,double *);
int    _MMG5_interp4bar_iso(MMG5_pMesh,MMG5_pSol,int,int,double *);
int    _MMG5_defsiz_iso(MMG5_pMesh,MMG5_pSol );
int    _MMG5_defsiz_ani(MMG5_pMesh ,MMG5_pSol );
int    _MMG5_gradsiz_iso(MMG5_pMesh ,MMG5_pSol );
int    _MMG5_gradsiz_ani(MMG5_pMesh ,MMG5_pSol );

double (*_MMG5_caltet)(MMG5_pMesh mesh,MMG5_pSol met,int ia,int ib,int ic,int id);
double (*_MMG5_caltri)(MMG5_pMesh mesh,MMG5_pSol met,MMG5_pTria ptt);
int    (*_MMG5_defsiz)(MMG5_pMesh ,MMG5_pSol );
int    (*_MMG5_gradsiz)(MMG5_pMesh ,MMG5_pSol );
int    (*_MMG5_intmetvol)(double*,double*,double*,double );
int    (*_MMG5_interp4bar)(MMG5_pMesh,MMG5_pSol,int,int,double *);

void   _MMG5_Set_commonFunc();

#endif
