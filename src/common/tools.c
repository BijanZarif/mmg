/* =============================================================================
**  This file is part of the mmg software package for the tetrahedral
**  mesh modification.
**  Copyright (c) Bx INP/Inria/UBordeaux/UPMC, 2004- .
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

/**
 * \file common/tools.c
 * \brief Various tools for the mmg applications.
 * \author Charles Dapogny (UPMC)
 * \author Cécile Dobrzynski (Bx INP/Inria/UBordeaux)
 * \author Pascal Frey (UPMC)
 * \author Algiane Froehly (Inria/UBordeaux)
 * \version 5
 * \copyright GNU Lesser General Public License.
 * \todo doxygen documentation.
 */

#include "mmgcommon.h"

/**
 * \param mesh pointer toward the mesh stucture.
 * \param ip1 first point of face.
 * \param ip2 second point of face.
 * \param ip3 third point of face.
 * \param n pointer to store the computed normal.
 * \return 1
 *
 * Compute non-normalized face normal given three points on the surface.
 *
 */
inline int _MMG5_nonUnitNorPts(MMG5_pMesh mesh,
                                int ip1,int ip2, int ip3,double *n) {
  MMG5_pPoint   p1,p2,p3;
  double        abx,aby,abz,acx,acy,acz;

  p1 = &mesh->point[ip1];
  p2 = &mesh->point[ip2];
  p3 = &mesh->point[ip3];

  /* area */
  abx = p2->c[0] - p1->c[0];
  aby = p2->c[1] - p1->c[1];
  abz = p2->c[2] - p1->c[2];

  acx = p3->c[0] - p1->c[0];
  acy = p3->c[1] - p1->c[1];
  acz = p3->c[2] - p1->c[2];

  n[0] = aby*acz - abz*acy;
  n[1] = abz*acx - abx*acz;
  n[2] = abx*acy - aby*acx;

  return(1);
}

/**
 * \param mesh pointer toward the mesh stucture.
 * \param pt triangle for which we compute the surface.
 * \return the computed surface
 *
 * Compute non-oriented surface area of a triangle.
 *
 */
inline double _MMG5_nonorsurf(MMG5_pMesh mesh,MMG5_pTria pt) {
  double   n[3];
  int      ip1, ip2, ip3;

  ip1 = pt->v[0];
  ip2 = pt->v[1];
  ip3 = pt->v[2];

  _MMG5_nonUnitNorPts(mesh,ip1,ip2,ip3,n);

  return(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
}
/**
 * \param mesh pointer toward the mesh stucture.
 * \param ip1 first point of face.
 * \param ip2 second point of face.
 * \param ip3 third point of face.
 * \param n pointer to store the computed normal.
 * \return 1
 *
 * Compute normalized face normal given three points on the surface.
 *
 */
inline int _MMG5_norpts(MMG5_pMesh mesh,int ip1,int ip2, int ip3,double *n) {
  double   dd,det;

  _MMG5_nonUnitNorPts(mesh,ip1,ip2,ip3,n);

  det  = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];

  if ( det < _MMG5_EPSD2 )  return(0);

  dd = 1.0 / sqrt(det);
  n[0] *= dd;
  n[1] *= dd;
  n[2] *= dd;

  return(1);
}

/**
 * \param mesh pointer toward the mesh stucture.
 * \param pt pointer toward the triangle structure.
 * \param n pointer to store the computed normal.
 * \return 1
 *
 * Compute triangle normal.
 *
 */
inline int _MMG5_nortri(MMG5_pMesh mesh,MMG5_pTria pt,double *n) {

  return(_MMG5_norpts(mesh,pt->v[0],pt->v[1],pt->v[2],n));

}

/* Compute product R*M*tR when M is symmetric */
inline int _MMG5_rmtr(double r[3][3],double m[6], double mr[6]){
  double n[3][3];

  n[0][0] = m[0]*r[0][0] + m[1]*r[0][1] + m[2]*r[0][2];
  n[1][0] = m[1]*r[0][0] + m[3]*r[0][1] + m[4]*r[0][2];
  n[2][0] = m[2]*r[0][0] + m[4]*r[0][1] + m[5]*r[0][2];

  n[0][1] = m[0]*r[1][0] + m[1]*r[1][1] + m[2]*r[1][2];
  n[1][1] = m[1]*r[1][0] + m[3]*r[1][1] + m[4]*r[1][2];
  n[2][1] = m[2]*r[1][0] + m[4]*r[1][1] + m[5]*r[1][2];

  n[0][2] = m[0]*r[2][0] + m[1]*r[2][1] + m[2]*r[2][2];
  n[1][2] = m[1]*r[2][0] + m[3]*r[2][1] + m[4]*r[2][2];
  n[2][2] = m[2]*r[2][0] + m[4]*r[2][1] + m[5]*r[2][2];

  mr[0] = r[0][0]*n[0][0] + r[0][1]*n[1][0] + r[0][2]*n[2][0];
  mr[1] = r[0][0]*n[0][1] + r[0][1]*n[1][1] + r[0][2]*n[2][1];
  mr[2] = r[0][0]*n[0][2] + r[0][1]*n[1][2] + r[0][2]*n[2][2];
  mr[3] = r[1][0]*n[0][1] + r[1][1]*n[1][1] + r[1][2]*n[2][1];
  mr[4] = r[1][0]*n[0][2] + r[1][1]*n[1][2] + r[1][2]*n[2][2];
  mr[5] = r[2][0]*n[0][2] + r[2][1]*n[1][2] + r[2][2]*n[2][2];

  return(1);
}

/**
 * \param n pointer toward the vector that we want to send on the third vector
 * of canonical basis.
 * \param r computed rotation matrix.
 *
 * Compute rotation matrix that sends vector \a n to the third vector of
 * canonical basis.
 *
 */
inline int _MMG5_rotmatrix(double n[3],double r[3][3]) {
  double aa,bb,ab,ll,l,cosalpha,sinalpha;

  aa = n[0]*n[0];
  bb = n[1]*n[1];
  ab = n[0]*n[1];
  ll = aa+bb;
  cosalpha = n[2];
  sinalpha = sqrt(1.0- MG_MIN(1.0,cosalpha*cosalpha));

  /* No rotation needed in this case */
  if ( ll < _MMG5_EPS ) {
    if ( n[2] > 0.0 ) {
      r[0][0] = 1.0 ; r[0][1] = 0.0 ; r[0][2] = 0.0;
      r[1][0] = 0.0 ; r[1][1] = 1.0 ; r[1][2] = 0.0;
      r[2][0] = 0.0 ; r[2][1] = 0.0 ; r[2][2] = 1.0;
    }
    else {
      r[0][0] = -1.0 ; r[0][1] = 0.0 ; r[0][2] = 0.0;
      r[1][0] = 0.0 ; r[1][1] = 1.0 ; r[1][2] = 0.0;
      r[2][0] = 0.0 ; r[2][1] = 0.0 ; r[2][2] = -1.0;
    }
  }
  else {
    l = sqrt(ll);

    r[0][0] = (aa*cosalpha + bb)/ll;
    r[0][1] = ab*(cosalpha-1)/ll;
    r[0][2] = -n[0]*sinalpha/l;
    r[1][0] = r[0][1];
    r[1][1] = (bb*cosalpha + aa)/ll;
    r[1][2] = -n[1]*sinalpha/l;
    r[2][0] = n[0]*sinalpha/l;
    r[2][1] = n[1]*sinalpha/l;
    r[2][2] = cosalpha;
  }
  return(1);
}

/**
 * \param m pointer toward a 3x3 symetric matrix
 * \param mi pointer toward the computed 3x3 matrix.
 *
 * Invert \a m (3x3 symetric matrix) and store the result on \a mi
 *
 */
int _MMG5_invmat(double *m,double *mi) {
  double  aa,bb,cc,det,vmin,vmax,maxx;
  int     k;

  /* check diagonal matrices */
  vmax = fabs(m[1]);
  maxx = fabs(m[2]);
  if( maxx > vmax ) vmax = maxx;
  maxx = fabs(m[4]);
  if( maxx > vmax ) vmax = maxx;
  if ( vmax < _MMG5_EPS ) {
    mi[0]  = 1./m[0];
    mi[3]  = 1./m[3];
    mi[5]  = 1./m[5];
    mi[1] = mi[2] = mi[4] = 0.0;
    return(1);
  }

  /* check ill-conditionned matrix */
  vmin = vmax = fabs(m[0]);
  for (k=1; k<6; k++) {
    maxx = fabs(m[k]);
    if ( maxx < vmin )  vmin = maxx;
    else if ( maxx > vmax )  vmax = maxx;
  }
  if ( vmax == 0.0 )  return(0);
  /* compute sub-dets */
  aa  = m[3]*m[5] - m[4]*m[4];
  bb  = m[4]*m[2] - m[1]*m[5];
  cc  = m[1]*m[4] - m[2]*m[3];
  det = m[0]*aa + m[1]*bb + m[2]*cc;
  if ( fabs(det) < _MMG5_EPS3 )  return(0);
  det = 1.0 / det;

  mi[0] = aa*det;
  mi[1] = bb*det;
  mi[2] = cc*det;
  mi[3] = (m[0]*m[5] - m[2]*m[2])*det;
  mi[4] = (m[1]*m[2] - m[0]*m[4])*det;
  mi[5] = (m[0]*m[3] - m[1]*m[1])*det;

  return(1);
}

/**
 * \param m initial matrix.
 * \param mi inverted matrix.
 *
 * Invert 3x3 non-symmetric matrix.
 *
 */
int _MMG5_invmatg(double m[9],double mi[9]) {
  double  aa,bb,cc,det,vmin,vmax,maxx;
  int     k;

  /* check ill-conditionned matrix */
  vmin = vmax = fabs(m[0]);
  for (k=1; k<9; k++) {
    maxx = fabs(m[k]);
    if ( maxx < vmin )  vmin = maxx;
    else if ( maxx > vmax )  vmax = maxx;
  }
  if ( vmax == 0.0 )  return(0);

  /* compute sub-dets */
  aa = m[4]*m[8] - m[5]*m[7];
  bb = m[5]*m[6] - m[3]*m[8];
  cc = m[3]*m[7] - m[4]*m[6];
  det = m[0]*aa + m[1]*bb + m[2]*cc;
  if ( fabs(det) < _MMG5_EPSD )  return(0);
  det = 1.0 / det;

  mi[0] = aa*det;
  mi[3] = bb*det;
  mi[6] = cc*det;
  mi[1] = (m[2]*m[7] - m[1]*m[8])*det;
  mi[4] = (m[0]*m[8] - m[2]*m[6])*det;
  mi[7] = (m[1]*m[6] - m[0]*m[7])*det;
  mi[2] = (m[1]*m[5] - m[2]*m[4])*det;
  mi[5] = (m[2]*m[3] - m[0]*m[5])*det;
  mi[8] = (m[0]*m[4] - m[1]*m[3])*det;

  return(1);
}

/**
 * \param a matrix to invert.
 * \param b last member.
 * \param r vector of unknowns.
 * \return 0 if fail, 1 otherwise.
 *
 * Solve \f$ 3\times 3\f$ symmetric system \f$ A . r = b \f$.
 *
 */
inline int _MMG5_sys33sym(double a[6], double b[3], double r[3]){
  double ia[6],as[6],det,m;
  int    i;

  /* Multiply matrix by a constant coefficient for stability purpose (because of the scaling) */
  m = fabs(a[0]);
  for(i=1;i<6;i++){
    if(fabs(a[i])<m){
      m = fabs(a[i]);
    }
  }

  if(m < _MMG5_EPSD)
    return(0);

  m = 1.0/m;

  for(i=0;i<6;i++){
    as[i] = a[i]*m;
  }

  det = as[0]*(as[3]*as[5]-as[4]*as[4]) - as[1]*(as[1]*as[5]-as[2]*as[4]) \
    + as[2]*(as[1]*as[4]-as[2]*as[3]);

  if(fabs(det) < _MMG5_EPSD)
    return(0);

  det = 1.0/det;

  ia[0] = (as[3]*as[5]-as[4]*as[4]);
  ia[1] = - (as[1]*as[5]-as[2]*as[4]);
  ia[2] = (as[1]*as[4]-as[2]*as[3]);
  ia[3] = (as[0]*as[5]-as[2]*as[2]);
  ia[4] = -(as[0]*as[4]-as[2]*as[1]);
  ia[5] = (as[0]*as[3]-as[1]*as[1]);

  r[0] = ia[0]*b[0] + ia[1]*b[1] + ia[2]*b[2];
  r[1] = ia[1]*b[0] + ia[3]*b[1] + ia[4]*b[2];
  r[2] = ia[2]*b[0] + ia[4]*b[1] + ia[5]*b[2];

  r[0]*=(det*m);
  r[1]*=(det*m);
  r[2]*=(det*m);

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param fileName pointer toward the file name.
 *
 * Debug function (not use in clean code): write mesh->tria structure in file.
 *
 */
void _MMG5_printTria(MMG5_pMesh mesh,char* fileName) {
  MMG5_pTria ptt;
  int   k;
  FILE  *inm;

  inm = fopen(fileName,"w");

  fprintf(inm,"----------> %d TRIANGLES <----------\n",mesh->nt);
  for(k=1; k<=mesh->nt; k++) {
    ptt = &mesh->tria[k];
    fprintf(inm,"num %d -> %d %d %d\n",k,ptt->v[0],ptt->v[1],
            ptt->v[2]);
    fprintf(inm,"ref   -> %d\n",ptt->ref);
    fprintf(inm,"tag   -> %d %d %d\n",ptt->tag[0],ptt->tag[1],ptt->tag[2]);
    fprintf(inm,"edg   -> %d %d %d\n",ptt->edg[0],ptt->edg[1],ptt->edg[2]);
    fprintf(inm,"\n");
  }
  fprintf(inm,"---------> END TRIANGLES <--------\n");
  fclose(inm);
}

/**
 * \return the available memory size of the computer.
 *
 * Compute the available memory size of the computer.
 *
 */
long long _MMG5_memSize (void) {
  long long mem;

#if (defined(__APPLE__) && defined(__MACH__))
  size_t size;

  size = sizeof(mem);
  if ( sysctlbyname("hw.memsize",&mem,&size,NULL,0) == -1)
    return(0);

#elif defined(__unix__) || defined(__unix) || defined(unix)
  mem = ((long long)sysconf(_SC_PHYS_PAGES))*
    ((long long)sysconf(_SC_PAGE_SIZE));
#elif defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  // status.ullTotalPhys is an unsigned long long: we must check that it fits inside a long long
  mem = status.ullTotalPhys & LLONG_MAX;
  if (mem == status.ullTotalPhys) return(mem);
  else return(LLONG_MAX);
#else
  printf("  ## WARNING: UNKNOWN SYSTEM, RECOVER OF MAXIMAL MEMORY NOT AVAILABLE.\n");
  return(0);
#endif

  return(mem);
}

/**
*Safe cast into a long */
inline
long _MMG5_safeLL2LCast(long long val)
{
  long tmp_l;

  tmp_l  = (long)(val);

  if ( (long long)(tmp_l) != val ) {
        fprintf(stdout,"  ## Error:");
        fprintf(stdout," unable to cast value.n");
		exit(EXIT_FAILURE);
		}
  return(tmp_l);
}
