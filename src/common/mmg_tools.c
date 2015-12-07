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

/**
 * \file common/mmg_tools.c
 * \brief Various tools for the mmg applications.
 * \author Charles Dapogny (LJLL, UPMC)
 * \author Cécile Dobrzynski (Inria / IMB, Université de Bordeaux)
 * \author Pascal Frey (LJLL, UPMC)
 * \author Algiane Froehly (Inria / IMB, Université de Bordeaux)
 * \version 5
 * \copyright GNU Lesser General Public License.
 * \todo doxygen documentation.
 */

#include "mmg.h"


/**
 * \param mesh pointer toward the mesh stucture.
 * \param ip1 first point of face.
 * \param ip2 second point of face.
 * \param ip3 third point of face.
 * \param n pointer to store the computed normal.
 * \return 1
 *
 * Compute face normal given three points on the surface.
 *
 */
inline int _MMG5_norpts(MMG5_pMesh mesh,int ip1,int ip2, int ip3,double *n) {
  MMG5_pPoint   p1,p2,p3;
  double   dd,abx,aby,abz,acx,acy,acz,det;

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
 * \param filename pointer toward the file name.
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
#else
  printf("  ## WARNING: UNKNOWN SYSTEM, RECOVER OF MAXIMAL MEMORY NOT AVAILABLE.\n");
  return(0);
#endif

  return(mem);
}

/**
 * \param a real coefficient of the degree 3 polynomial.
 * \param r computed complex roots.
 * \return number of roots, counted with multiplicity.
 *
 * Compute the 3 complex roots of a degree 3 polynomial with real coefficients
 * \f$a[3]T^3 + ... + a[0]\f$ By convention, the real roots are stored first (same
 * thing for multiple roots): return value = number of roots, counted with
 * multiplicity.
 *
 */
int _MMG5_rootDeg3(double a[4],double complex r[3]) {
  double p,q,Delta,u,v,pi,b[4];
  double complex i,j,t,tbar;

  pi = 3.14159;
  i = _Complex_I;
  j = cos(2.0*_MMG5_ATHIRD*pi)+I*sin(2.0*_MMG5_ATHIRD*pi);

  /* Case when the polynomial is actually second order */
  if( fabs(a[3]) < _MMG5_EPSD ) {
    /* Case it is first order : return 0.0 when root does not exist */
    if( fabs(a[2]) < _MMG5_EPSD ) {
      if( fabs(a[1]) < _MMG5_EPSD ) {
        r[0] = r[1] = r[2] = 0.0;
        return(0);
      }
      else{
        r[0] = r[1] = r[2] = -a[0];
        return(1);
      }
    }
    else{
      Delta = a[1]*a[1]-4.0*a[2]*a[0];
      if( Delta > 0.0 ) {
        r[0] = 0.5/a[2]*(-a[1] - sqrt(Delta));
        r[1] = r[2] = 0.5/a[2]*(-a[1] + sqrt(Delta));
      }
      else if( Delta == 0.0 ) {
        r[0] = r[1] = -0.5*a[1]/a[2];
      }
      else{
        r[0] = 0.5/a[2]*(-a[1] - i*sqrt(-Delta));
        r[1] = r[2] = 0.5/a[2]*(-a[1] + i*sqrt(-Delta));
      }
      return(2);
    }
  }

  /* Normalize coefficients */
  b[3] = 1.0;
  b[2] = a[2]/a[3];
  b[1] = a[1]/a[3];
  b[0] = a[0]/a[3];

  p = b[1] - _MMG5_ATHIRD*b[2]*b[2];
  q = b[0] - _MMG5_ATHIRD*b[2]*b[1] + 2.0/27.0*b[2]*b[2]*b[2];
  Delta = 4.0/27.0*p*p*p+q*q;

  if( Delta>0.0 ) {
    /* Polynomial T^2 +qT -p^3/27 admits two real roots u and v */
    u = 0.5*(-q - sqrt(Delta));
    v = 0.5*(-q + sqrt(Delta));

    u = u < 0.0 ? - pow(fabs(u),_MMG5_ATHIRD) :pow(fabs(u),_MMG5_ATHIRD);
    v = v < 0.0 ? - pow(fabs(v),_MMG5_ATHIRD) :pow(fabs(v),_MMG5_ATHIRD);

    r[0] = u+v;
    r[1] = j*j*u + j*v;
    r[2] = j*u + j*j*v;

  }
  else if (Delta<0.0){
    /* Polynomial T^2 +qT -p^3/27 admits two complex conjuguate roots u and v */
    t = -0.5*q - 0.5*i*sqrt(-Delta);
    t = cpow(t,_MMG5_ATHIRD);
    tbar = conj(t);

    /* Theoretically speaking, the 3 roots are real... But to make sure... */
    r[0] = creal(t+tbar);
    r[1] = creal(j*t+j*j*tbar);
    r[2] = creal(j*j*t+j*tbar);
  }
  else{
    /* Polynomial T^2 +qT -p^3/27 admits one double real root */
    u = -0.5*q;
    u = pow(u,_MMG5_ATHIRD);

    r[0] = -u;
    r[1] = -u;
    r[2] = 2.0*u;

  }

  r[0] -= _MMG5_ATHIRD*b[2];
  r[1] -= _MMG5_ATHIRD*b[2];
  r[2] -= _MMG5_ATHIRD*b[2];

  return(3);
}
