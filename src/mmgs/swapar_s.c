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
 * \file mmgs/swapar_s.c
 * \brief Functions for swapping process.
 * \author Charles Dapogny (UPMC)
 * \author Cécile Dobrzynski (Bx INP/Inria/UBordeaux)
 * \author Pascal Frey (UPMC)
 * \author Algiane Froehly (Inria/UBordeaux)
 * \version 5
 * \copyright GNU Lesser General Public License.
 * \todo Doxygen documentation
 */

#include "mmgs.h"

/* Check whether edge i of triangle k should be swapped for geometric approximation purposes */
int chkswp(MMG5_pMesh mesh,MMG5_pSol met,int k,int i,char typchk) {
  MMG5_pTria    pt,pt0,pt1;
  MMG5_pPoint   p[3],q;
  MMG5_pPar      par;
  double   np[3][3],nq[3],*nr1,*nr2,nt[3],ps,ps2,*n1,*n2,dd,c1[3],c2[3],hausd;
  double   cosn1,cosn2,calnat,calchg,cal1,cal2,cosnat,coschg,ux,uy,uz,ll,loni,lona;
  int     *adja,j,kk,l,ip0,ip1,ip2,iq,isloc;
  char     ii,i1,i2,jj;

  pt0 = &mesh->tria[0];
  pt  = &mesh->tria[k];
  i1 = _MMG5_inxt2[i];
  i2 = _MMG5_iprv2[i];
  if ( MG_EDG(pt->tag[i]) || MS_SIN(pt->tag[i]) )  return(0);
  else if ( MS_SIN(pt->tag[i1]) )  return(0);

  ip0  = pt->v[i];
  ip1  = pt->v[i1];
  ip2  = pt->v[i2];
  p[0] = &mesh->point[ip0];
  p[1] = &mesh->point[ip1];
  p[2] = &mesh->point[ip2];

  adja = &mesh->adja[3*(k-1)+1];
  if ( !adja[i] )  return(0);

  kk = adja[i] / 3;
  ii = adja[i] % 3;
  jj = _MMG5_inxt2[ii];
  pt1 = &mesh->tria[kk];
  if ( MS_SIN(pt1->tag[jj]) )  return(0);

  iq = pt1->v[ii];
  q  = &mesh->point[iq];

  /* local hausdorff for triangle */
  hausd = mesh->info.hausd;
  isloc = 0;
  for (l=0; l<mesh->info.npar; l++) {
    par = &mesh->info.par[l];
    if ( ((par->elt == MMG5_Triangle) &&
          ( (pt->ref == par->ref) || (pt1->ref == par->ref) )) /* ||
         ( (par->elt == MMG5_Vertex) &&
         ((p[1]->ref == par->ref ) || (p[2]->ref == par->ref)) ) */ ) {
      if ( !isloc ) {
        hausd = par->hausd;
        isloc = 1;
      }
      else {
        // take the minimum value between the two local hausdorff number asked
        // by the user.
        hausd = MG_MIN(hausd,par->hausd);
      }
    }
  }

  /* check length */
  if ( typchk == 2 && met->m ) {
    loni = _MMG5_lenSurfEdg(mesh,met,ip1,ip2,0);
    lona = _MMG5_lenSurfEdg(mesh,met,ip0,iq,0);
    if ( loni > 1.0 )  loni = MG_MIN(1.0 / loni,LSHRT);
    if ( lona > 1.0 )  lona = 1.0 / lona;
    if ( lona < loni )  return(0);
  }

  /* check non convexity */
  _MMG5_norpts(mesh,ip0,ip1,iq,c1);
  _MMG5_norpts(mesh,ip0,iq,ip2,c2);
  ps = c1[0]*c2[0] + c1[1]*c2[1] + c1[2]*c2[2];
  if ( ps < _MMG5_ANGEDG )   return(0);

  /* normal recovery at points p[0],p[1],p[2],q */
  for (j=0; j<3; j++) {
    if ( MS_SIN(p[j]->tag) ) {
      _MMG5_nortri(mesh,pt,np[j]);
    }
    else if ( MG_EDG(p[j]->tag) ) {
      _MMG5_nortri(mesh,pt,nt);
      nr1  = &mesh->xpoint[p[j]->xp].n1[0];
      nr2  = &mesh->xpoint[p[j]->xp].n2[0];
      ps  = nr1[0]*nt[0] + nr1[1]*nt[1] + nr1[2]*nt[2];
      ps2 = nr2[0]*nt[0] + nr2[1]*nt[1] + nr2[2]*nt[2];
      if ( fabs(ps) > fabs(ps2) )
        memcpy(&np[j],nr1,3*sizeof(double));
      else
        memcpy(&np[j],nr2,3*sizeof(double));
    }
    else
      memcpy(&np[j],p[j]->n,3*sizeof(double));
  }

  if ( MS_SIN(q->tag) ) {
    _MMG5_nortri(mesh,pt,nq);
  }
  else if ( MG_EDG(q->tag) ) {
    _MMG5_nortri(mesh,pt,nt);
    nr1  = &mesh->xpoint[q->xp].n1[0];
    nr2  = &mesh->xpoint[q->xp].n2[0];
    ps  = nr1[0]*nt[0] + nr1[1]*nt[1] + nr1[2]*nt[2];
    ps2 = nr2[0]*nt[0] + nr2[1]*nt[1] + nr2[2]*nt[2];
    if ( fabs(ps) > fabs(ps2) )
      memcpy(&nq,nr1,3*sizeof(double));
    else
      memcpy(&nq,nr2,3*sizeof(double));
  }
  else
    memcpy(&nq,q->n,3*sizeof(double));

  /* Estimate of the Hausdorff distance between approximation and underlying surface
     when using the 'natural' edge [i1,i2] */
  ux = p[2]->c[0] - p[1]->c[0];
  uy = p[2]->c[1] - p[1]->c[1];
  uz = p[2]->c[2] - p[1]->c[2];

  ll = ux*ux + uy*uy + uz*uz;
  if ( ll < _MMG5_EPS )  return(0); /* no change for short edge */

  n1 = np[1];
  n2 = np[2];

  ps = ux*n1[0] + uy*n1[1] + uz*n1[2];
  c1[0] = (2.0*p[1]->c[0] + p[2]->c[0] - ps*n1[0]) / 3.0 - p[1]->c[0];
  c1[1] = (2.0*p[1]->c[1] + p[2]->c[1] - ps*n1[1]) / 3.0 - p[1]->c[1];
  c1[2] = (2.0*p[1]->c[2] + p[2]->c[2] - ps*n1[2]) / 3.0 - p[1]->c[2];

  ps = -(ux*n2[0] + uy*n2[1] + uz*n2[2]);
  c2[0] = (2.0*p[2]->c[0] + p[1]->c[0] - ps*n2[0]) / 3.0 - p[2]->c[0];
  c2[1] = (2.0*p[2]->c[1] + p[1]->c[1] - ps*n2[1]) / 3.0 - p[2]->c[1];
  c2[2] = (2.0*p[2]->c[2] + p[1]->c[2] - ps*n2[2]) / 3.0 - p[2]->c[2];

  /* squared cosines */
  ps = c1[0]*ux + c1[1]*uy + c1[2]*uz;
  ps *= ps;
  dd = c1[0]*c1[0] + c1[1]*c1[1] + c1[2]*c1[2];
  cosn1  =  ps / (dd*ll);
  cosn1 *= (1.0-cosn1);
  cosn1 *= (0.25*ll);

  ps = -c2[0]*ux - c2[1]*uy - c2[2]*uz;
  ps *= ps;
  dd = c2[0]*c2[0]+c2[1]*c2[1]+c2[2]*c2[2];
  cosn2  =  ps / (dd*ll);
  cosn2 *= (1.0-cosn2);
  cosn2 *= (0.25*ll);

  cosnat = MG_MAX(fabs(cosn1),fabs(cosn2));
  cosnat = cosnat < _MMG5_EPS ? 0.0 : cosnat;

  /* Estimate of the Hausdorff distance between approximation and underlying surface
     when using the 'swapped' edge [i0,q] */
  ux = q->c[0] - p[0]->c[0];
  uy = q->c[1] - p[0]->c[1];
  uz = q->c[2] - p[0]->c[2];

  ll = ux*ux + uy*uy + uz*uz;
  if ( ll < _MMG5_EPS )  return(0);

  n1 = np[0];
  n2 = nq;

  ps = ux*n1[0] + uy*n1[1] + uz*n1[2];
  c1[0] = (2.0*p[0]->c[0] + q->c[0] - ps*n1[0]) / 3.0 - p[0]->c[0];
  c1[1] = (2.0*p[0]->c[1] + q->c[1] - ps*n1[1]) / 3.0 - p[0]->c[1];
  c1[2] = (2.0*p[0]->c[2] + q->c[2] - ps*n1[2]) / 3.0 - p[0]->c[2];

  ps = -(ux*n2[0] + uy*n2[1] + uz*n2[2]);
  c2[0] = (2.0*q->c[0] + p[0]->c[0] - ps*n2[0]) / 3.0 - q->c[0];
  c2[1] = (2.0*q->c[1] + p[0]->c[1] - ps*n2[1]) / 3.0 - q->c[1];
  c2[2] = (2.0*q->c[2] + p[0]->c[2] - ps*n2[2]) / 3.0 - q->c[2];

  /* squared cosines */
  ps = c1[0]*ux + c1[1]*uy + c1[2]*uz;
  ps *= ps;
  dd = c1[0]*c1[0] + c1[1]*c1[1] + c1[2]*c1[2];
  cosn1  =  ps / (dd*ll);
  cosn1 *= (1.0-cosn1);
  cosn1 *= (0.25*ll);

  ps = -c2[0]*ux - c2[1]*uy - c2[2]*uz;
  ps *= ps;
  dd = c2[0]*c2[0]+c2[1]*c2[1]+c2[2]*c2[2];
  cosn2  =  ps / (dd*ll);
  cosn2 *= (1.0-cosn2);
  cosn2 *= (0.25*ll);

  coschg = MG_MAX(fabs(cosn1),fabs(cosn2));
  coschg = coschg < _MMG5_EPS ? 0.0 : coschg;

  /* swap if Hausdorff contribution of the swapped edge is less than existing one */
  if ( coschg > hausd*hausd )  return(0);

  if ( typchk == 2 && met->m ) {
    /* initial quality */
    pt0->v[0]= ip0;  pt0->v[1]= ip1;  pt0->v[2]= ip2;
    pt0->tag[0] = pt->tag[i];
    pt0->tag[1] = pt->tag[i1];
    pt0->tag[2] = pt->tag[i2];
    cal1 = _MMG5_calelt(mesh,met,pt0);

    pt0->v[0]= ip1;  pt0->v[1]= iq;   pt0->v[2]= ip2;
    pt0->tag[0] = pt->tag[i1];
    pt0->tag[1] = pt->tag[ii];
    pt0->tag[2] = pt->tag[i2];
    cal2 = _MMG5_calelt(mesh,met,pt0);

    calnat = MG_MIN(cal1,cal2);
    assert(calnat > 0.);

    /* quality after swap */
    pt0->v[0]= ip0;  pt0->v[1]= ip1;  pt0->v[2]= iq;
    pt0->tag[0] = pt->tag[i];
    pt0->tag[1] = pt->tag[i1];
    pt0->tag[2] = pt->tag[ii];
    cal1 = _MMG5_calelt(mesh,met,pt0);

    pt0->v[0]= ip0;  pt0->v[1]= iq;   pt0->v[2]= ip2;
    pt0->tag[0] = pt->tag[i];
    pt0->tag[1] = pt->tag[ii];
    pt0->tag[2] = pt->tag[i2];
    cal2 = _MMG5_calelt(mesh,met,pt0);

    calchg = MG_MIN(cal1,cal2);
  }
  else {
    // warning if typchk==1 iso??
    pt0->v[0]= ip0;  pt0->v[1]= ip1;  pt0->v[2]= ip2;
    cal1 = _MMG5_caltri_iso(mesh,NULL,pt0);
    pt0->v[0]= ip1;  pt0->v[1]= iq;   pt0->v[2]= ip2;
    cal2 = _MMG5_caltri_iso(mesh,NULL,pt0);
    calnat = MG_MIN(cal1,cal2);
    pt0->v[0]= ip0;  pt0->v[1]= ip1;  pt0->v[2]= iq;
    cal1 = _MMG5_caltri_iso(mesh,NULL,pt0);
    pt0->v[0]= ip0;  pt0->v[1]= iq;   pt0->v[2]= ip2;
    cal2 = _MMG5_caltri_iso(mesh,NULL,pt0);
    calchg = MG_MIN(cal1,cal2);
  }

  /* if the quality is very bad, don't degrade it, even to improve the surface
   * approx. */
  if ( calchg < _MMG5_EPS && calnat >= calchg ) return(0);

  /* else we can degrade the quality to improve the surface approx. */
  if ( coschg < hausd*hausd && cosnat > hausd*hausd )  return(1);

  return(calchg > 1.01 * calnat);
}

/**
 * \param mesh poiner toward the mesh structure.
 * \param k elt index.
 * \param i index of the elt edge to swap.
 * \return 1
 *
 * \warning the quality of the resulting triangles is not checked here... It
 * must be checked outside to prevent the creation of empty elts.
 *
 */
int swapar(MMG5_pMesh mesh,int k,int i) {
  MMG5_pTria    pt,pt1;
  int     *adja,adj,k11,k21;
  char     i1,i2,j,jj,j2,v11,v21;

  pt   = &mesh->tria[k];
  if ( MG_EDG(pt->tag[i]) || MS_SIN(pt->tag[i]) )  return(0);

  adja = &mesh->adja[3*(k-1)+1];
  assert(adja[i]);

  adj = adja[i] / 3;
  j   = adja[i] % 3;
  pt1 = &mesh->tria[adj];

  /* simulation */
  i1 = _MMG5_inxt2[i];
  i2 = _MMG5_iprv2[i];

  /* update structure */
  k11 = adja[i1] / 3;
  v11 = adja[i1] % 3;
  if ( k11 < 1 )  return(0);
  adja = &mesh->adja[3*(adj-1)+1];
  jj  = _MMG5_inxt2[j];
  j2  = _MMG5_iprv2[j];
  k21 = adja[jj] / 3;
  v21 = adja[jj] % 3;
  if ( k21 < 1 )  return(0);

  pt->v[i2]  = pt1->v[j];
  pt1->v[j2] = pt->v[i];

  /* update info */
  pt->tag[i] = pt1->tag[jj];
  pt->edg[i] = pt1->edg[jj];
  pt->base   = mesh->base;
  pt1->tag[j] = pt->tag[i1];
  pt1->edg[j] = pt->edg[i1];
  pt->tag[i1] = 0;
  pt->edg[i1] = 0;
  pt1->tag[jj] = 0;
  pt1->edg[jj] = 0;
  pt1->base    = mesh->base;

  /* update adjacent */
  mesh->adja[3*(k-1)+1+i]     = 3*k21+v21;
  mesh->adja[3*(k21-1)+1+v21] = 3*k+i;
  mesh->adja[3*(k-1)+1+i1]    = 3*adj+jj;
  mesh->adja[3*(adj-1)+1+jj]  = 3*k+i1;
  mesh->adja[3*(k11-1)+1+v11] = 3*adj+j;
  mesh->adja[3*(adj-1)+1+j]   = 3*k11+v11;

  return(1);
}


/* flip edge i of tria k for isotropic mesh*/
int litswp(MMG5_pMesh mesh,int k,char i,double kali) {
  MMG5_pTria    pt,pt0,pt1;
  double   kalf,kalt,ps,n1[3],n2[3];
  int     *adja,ia,ib,ic,id,kk;
  char     ii,i1,i2;

  pt0 = &mesh->tria[0];
  pt  = &mesh->tria[k];
  if ( !MG_EOK(pt) || MG_EDG(pt->tag[i]) )  return(0);

  i1 = _MMG5_inxt2[i];
  i2 = _MMG5_iprv2[i];
  ia = pt->v[i];
  ib = pt->v[i1];
  ic = pt->v[i2];

  adja = &mesh->adja[3*(k-1)+1];
  kk  = adja[i] / 3;
  ii  = adja[i] % 3;
  pt1 = &mesh->tria[kk];
  if ( MS_SIN(pt1->tag[ii]) )  return(0);
  id = pt1->v[ii];

  /* check non convexity */
  _MMG5_norpts(mesh,ia,ib,id,n1);
  _MMG5_norpts(mesh,ia,id,ic,n2);
  ps = n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2];
  if ( ps < _MMG5_ANGEDG )  return(0);

  /* check quality */
  pt0->v[0] = id;  pt0->v[1] = ic;  pt0->v[2] = ib;
  kalt = _MMG5_calelt(mesh,NULL,pt0);
  kali = MG_MIN(kali,kalt);
  pt0->v[0] = ia;  pt0->v[1] = id;  pt0->v[2] = ic;
  kalt = _MMG5_calelt(mesh,NULL,pt0);
  pt0->v[0] = ia;  pt0->v[1] = ib;  pt0->v[2] = id;
  kalf = _MMG5_calelt(mesh,NULL,pt0);
  kalf = MG_MIN(kalf,kalt);
  if ( kalf > 1.02 * kali ) {
    swapar(mesh,k,i);
    return(1);
  }
  return(0);
}


/* attempt to swap any edge below quality value
   list goes from 0 to ilist-1 */
int swpedg(MMG5_pMesh mesh,MMG5_pSol met,int *list,int ilist,char typchk) {
  int      k,ns,iel;
  char     i,i1;

  k  = 0;
  ns = 0;
  do {
    iel = list[k] / 3;
    i   = list[k] % 3;
    i1  = _MMG5_inxt2[i];
    if ( chkswp(mesh,met,iel,i1,typchk) ) {
      ns += swapar(mesh,iel,i1);
      k++;
    }
    k++;
  }
  while ( k < ilist );

  return(ns);
}
