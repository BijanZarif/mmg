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
 * \file mmg3d/tools_3d.c
 * \brief Various algorithmic and algebraic tools.
 * \author Charles Dapogny (UPMC)
 * \author Cécile Dobrzynski (Bx INP/Inria/UBordeaux)
 * \author Pascal Frey (UPMC)
 * \author Algiane Froehly (Inria/UBordeaux)
 * \version 5
 * \copyright GNU Lesser General Public License.
 * \todo Doxygen documentation
 */

#include "mmg3d.h"

extern char ddb;

/** naive (increasing) sorting algorithm, for very small tabs ; permutation is stored in perm */
inline void _MMG5_nsort(int n,double *val,char *perm){
    int   i,j,aux;

    for (i=0; i<n; i++)  perm[i] = i;

    for (i=0; i<n; i++) {
        for (j=i+1; j<n; j++) {
            if ( val[perm[i]] > val[perm[j]] ) {
                aux = perm[i];
                perm[i] = perm[j];
                perm[j] = aux;
            }
        }
    }
}

/** Compute 3 * 3 determinant : det(c1-c0,c2-c0,v) */
inline double _MMG5_det3pt1vec(double c0[3],double c1[3],double c2[3],double v[3]) {
    double m00,m10,m20,m01,m11,m21,det;

    m00 = c1[0] - c0[0] ; m01 = c2[0] - c0[0];
    m10 = c1[1] - c0[1] ; m11 = c2[1] - c0[1];
    m20 = c1[2] - c0[2] ; m21 = c2[2] - c0[2];
    det = v[0]*(m10*m21 - m20*m11) -v[1]*(m00*m21-m20*m01) + v[2]*(m00*m11-m10*m01);

    return(det);
}

/** Compute 3 * 3 determinant : det(c1-c0,c2-c0,c3-c0) */
inline double _MMG5_det4pt(double c0[3],double c1[3],double c2[3],double c3[3]) {
  double m[3];

  m[0] = c3[0] - c0[0];
  m[1] = c3[1] - c0[1];
  m[2] = c3[2] - c0[2];

  return( _MMG5_det3pt1vec(c0,c1,c2,m) );
}

/** Compute oriented volume of a tetrahedron */
inline double _MMG5_orvol(MMG5_pPoint point,int *v) {
    MMG5_pPoint  p0,p1,p2,p3;

    p0 = &point[v[0]];
    p1 = &point[v[1]];
    p2 = &point[v[2]];
    p3 = &point[v[3]];

    return(_MMG5_det4pt(p0->c,p1->c,p2->c,p3->c));
}

/** Compute normal to face iface of tetra k, exterior to tetra k */
inline int _MMG5_norface(MMG5_pMesh mesh,int k,int iface,double n[3]) {
  MMG5_pTetra     pt;

  pt = &mesh->tetra[k];

  return( _MMG5_norpts(mesh,
                       pt->v[_MMG5_idir[iface][0]],
                       pt->v[_MMG5_idir[iface][1]],
                       pt->v[_MMG5_idir[iface][2]],n) );
}

/** If need be, invert the travelling sense of surfacic ball so that it is travelled in
    the direct sense with respect to direction n anchored at point ip (ip = global num.):
    return 2 = orientation reversed, 1 otherwise */
inline int _MMG5_directsurfball(MMG5_pMesh mesh, int ip, int *list, int ilist, double n[3]){
    int             j,aux,iel;
    double          nt[3],ps;
    unsigned char   iface;

    iel   = list[0] / 4;
    iface = list[0] % 4;

    if ( !_MMG5_norface(mesh,iel,iface,nt) ) return(0);
    ps = nt[0]*n[0] +  nt[1]*n[1] +  nt[2]*n[2];
    if ( ps > 0.0 )  return(1);

    for (j=1; j<=(ilist-1)/2; j++) {
        aux = list[j];
        list[j] = list[ilist -j];
        list[ilist -j] = aux;
    }

    return(2);
}

/** If need be, reorder the surfacic ball of point ip, so that its first element has
    edge (p,q) (nump,q = global num) as edge _MMG5_iprv2[ip] of face iface.
    return 2 = orientation reversed, 1 otherwise */
int _MMG5_startedgsurfball(MMG5_pMesh mesh,int nump,int numq,int *list,int ilist) {
    MMG5_pTetra          pt;
    int             iel,tmp,l;
    unsigned char   iface,ip,ipt;

    iel = list[0]/4;
    iface = list[0]%4;
    pt = &mesh->tetra[iel];

    for(ip=0;ip<4;ip++){
        if(pt->v[ip] == nump) break;
    }
    assert(ip<4);

    pt = &mesh->tetra[iel];
    ipt = _MMG5_idirinv[iface][ip]; // index of ip in face iface
    ipt = _MMG5_inxt2[ipt];         // next index in this face
    ipt = _MMG5_idir[iface][ipt];  // index of this point in local num of tetra

    if(pt->v[ipt] == numq) return(1);

    else{
        ipt = _MMG5_idir[iface][_MMG5_iprv2[_MMG5_idirinv[iface][ip]]];
        assert(pt->v[ipt] == numq);

        tmp = list[0];
        for(l=0;l<ilist-1;l++){
            list[l] = list[l+1];
        }
        list[ilist-1] = tmp;
    }

    return(2);
}

/** Compute point located at parameter value step from point ip0, as well as interpolate
    of normals, tangent for a RIDGE edge */
inline int _MMG5_BezierRidge(MMG5_pMesh mesh,int ip0,int ip1,double s,double *o,double *no1,double *no2,double *to){
    MMG5_pPoint    p0,p1;
    double    ux,uy,uz,n01[3],n02[3],n11[3],n12[3],t0[3],t1[3];
    double    ps,ps2,b0[3],b1[3],bn[3],ll,il,ntemp[3],dd,alpha;

    p0 = &mesh->point[ip0];  /* Ref point, from which step is counted */
    p1 = &mesh->point[ip1];
    if ( !p0->xp || !p1->xp )  return(0);
    else if ( !(MG_GEO & p0->tag) || !(MG_GEO & p1->tag) )  return(0);

    ux = p1->c[0] - p0->c[0];
    uy = p1->c[1] - p0->c[1];
    uz = p1->c[2] - p0->c[2];
    ll = ux*ux + uy*uy + uz*uz;
    if ( ll < _MMG5_EPSD2 )  return(0);
    il = 1.0 / sqrt(ll);

    if ( MG_SIN(p0->tag) ) {
        t0[0] = ux * il;
        t0[1] = uy * il;
        t0[2] = uz * il;
    }
    else {
      memcpy(t0,&(p0->n[0]),3*sizeof(double));
      ps = t0[0]*ux + t0[1]*uy + t0[2]*uz;
      if ( ps < 0.0 ) {
        t0[0] *= -1.0;
        t0[1] *= -1.0;
        t0[2] *= -1.0;
      }
    }
    if ( MG_SIN(p1->tag) ) {
        t1[0] = -ux * il;
        t1[1] = -uy * il;
        t1[2] = -uz * il;
    }
    else {
        memcpy(t1,&(p1->n[0]),3*sizeof(double));
        ps = - ( t1[0]*ux + t1[1]*uy + t1[2]*uz );
        if ( ps < 0.0 ) {
            t1[0] *= -1.0;
            t1[1] *= -1.0;
            t1[2] *= -1.0;
        }
    }
    alpha = _MMG5_BezierGeod(p0->c,p1->c,t0,t1);

    b0[0] = p0->c[0] + alpha * t0[0];
    b0[1] = p0->c[1] + alpha * t0[1];
    b0[2] = p0->c[2] + alpha * t0[2];

    b1[0] = p1->c[0] + alpha * t1[0];
    b1[1] = p1->c[1] + alpha * t1[1];
    b1[2] = p1->c[2] + alpha * t1[2];

    o[0] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[0] + 3.0*s*(1.0-s)*(1.0-s)*b0[0] + \
        3.0*s*s*(1.0-s)*b1[0] + s*s*s*p1->c[0];

    o[1] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[1] + 3.0*s*(1.0-s)*(1.0-s)*b0[1] + \
        3.0*s*s*(1.0-s)*b1[1] + s*s*s*p1->c[1];

    o[2] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[2] + 3.0*s*(1.0-s)*(1.0-s)*b0[2] + \
        3.0*s*s*(1.0-s)*b1[2] + s*s*s*p1->c[2];

    if ( MG_SIN(p0->tag) && MG_SIN(p1->tag) ) {
        memcpy(to,t0,3*sizeof(double));
        return(1);
    }
    else if ( MG_SIN(p0->tag) ) {
        memcpy(n11,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
        memcpy(n12,&(mesh->xpoint[p1->xp].n2[0]),3*sizeof(double));
        memcpy(n01,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
        memcpy(n02,&(mesh->xpoint[p1->xp].n2[0]),3*sizeof(double));
    }
    else if ( MG_SIN(p1->tag) ) {
        memcpy(n01,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n02,&(mesh->xpoint[p0->xp].n2[0]),3*sizeof(double));
        memcpy(n11,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n12,&(mesh->xpoint[p0->xp].n2[0]),3*sizeof(double));
    }
    else {
        memcpy(n01,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n02,&(mesh->xpoint[p0->xp].n2[0]),3*sizeof(double));
        memcpy(n11,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
        memcpy(n12,&(mesh->xpoint[p1->xp].n2[0]),3*sizeof(double));

        /* Switch normals of p1 for pairing */
        ps  = n01[0] * n11[0] + n01[1] * n11[1] + n01[2] * n11[2];
        ps2 = n01[0] * n12[0] + n01[1] * n12[1] + n01[2] * n12[2];
        if ( ps2 > ps ) {
            memcpy(ntemp,n11,3*sizeof(double));
            memcpy(n11,n12,3*sizeof(double));
            memcpy(n12,ntemp,3*sizeof(double));
        }
    }

    /* Normal n1 interpolation */
    ps = ux*(n01[0] + n11[0]) + uy*(n01[1] + n11[1]) + uz*(n01[2] + n11[2]);
    ps = 2.0*ps / ll;

    bn[0] = n01[0] + n11[0] -ps*ux;
    bn[1] = n01[1] + n11[1] -ps*uy;
    bn[2] = n01[2] + n11[2] -ps*uz;

    dd = bn[0]*bn[0] + bn[1]*bn[1] + bn[2]*bn[2];
    if ( dd > _MMG5_EPSD2 ) {
        dd = 1.0 / sqrt(dd);
        bn[0] *= dd;
        bn[1] *= dd;
        bn[2] *= dd;
    }
    no1[0] = (1.0-s)*(1.0-s)*n01[0] + 2.0*s*(1.0-s)*bn[0] + s*s*n11[0];
    no1[1] = (1.0-s)*(1.0-s)*n01[1] + 2.0*s*(1.0-s)*bn[1] + s*s*n11[1];
    no1[2] = (1.0-s)*(1.0-s)*n01[2] + 2.0*s*(1.0-s)*bn[2] + s*s*n11[2];
    dd = no1[0]*no1[0] + no1[1]*no1[1] + no1[2]*no1[2];
    if ( dd > _MMG5_EPSD2 ) {
        dd = 1.0 / sqrt(dd);
        no1[0] *= dd;
        no1[1] *= dd;
        no1[2] *= dd;
    }

    /* Normal n2 interpolation */
    ps = ux*(n02[0] + n12[0]) + uy*(n02[1] + n12[1]) + uz*(n02[2] + n12[2]);
    ps = 2.0*ps/ll;

    bn[0] = n02[0] + n12[0] -ps*ux;
    bn[1] = n02[1] + n12[1] -ps*uy;
    bn[2] = n02[2] + n12[2] -ps*uz;

    dd = bn[0]*bn[0] + bn[1]*bn[1] + bn[2]*bn[2];
    if ( dd > _MMG5_EPSD2 ) {
        dd = 1.0 / sqrt(dd);
        bn[0] *= dd;
        bn[1] *= dd;
        bn[2] *= dd;
    }
    no2[0] = (1.0-s)*(1.0-s)*n02[0] + 2.0*s*(1.0-s)*bn[0] + s*s*n12[0];
    no2[1] = (1.0-s)*(1.0-s)*n02[1] + 2.0*s*(1.0-s)*bn[1] + s*s*n12[1];
    no2[2] = (1.0-s)*(1.0-s)*n02[2] + 2.0*s*(1.0-s)*bn[2] + s*s*n12[2];
    dd = no2[0]*no2[0] + no2[1]*no2[1] + no2[2]*no2[2];
    if ( dd > _MMG5_EPSD2 ) {
        dd = 1.0 / sqrt(dd);
        no2[0] *= dd;
        no2[1] *= dd;
        no2[2] *= dd;
    }

    to[0] = no1[1]*no2[2] - no1[2]*no2[1];
    to[1] = no1[2]*no2[0] - no1[0]*no2[2];
    to[2] = no1[0]*no2[1] - no1[1]*no2[0];
    dd = to[0]*to[0] + to[1]*to[1] + to[2]*to[2];
    if ( dd > _MMG5_EPSD2 ) {
        dd = 1.0/sqrt(dd);
        to[0] *= dd;
        to[1] *= dd;
        to[2] *= dd;
    }

    return(1);
}

/** Compute point located at parameter value step from point ip0, as well as interpolate
    of normals, tangent for a REF edge */
inline int _MMG5_BezierRef(MMG5_pMesh mesh,int ip0,int ip1,double s,double *o,double *no,double *to) {
    MMG5_pPoint          p0,p1;
    double          ux,uy,uz,n01[3],n02[3],n11[3],n12[3],ntemp[3],t0[3],t1[3];
    double          ps,b0[3],b1[3],bn[3],ll,il,dd,alpha,ps2;

    p0 = &mesh->point[ip0];  /* Ref point, from which step is counted */
    p1 = &mesh->point[ip1];

    ux = p1->c[0] - p0->c[0];
    uy = p1->c[1] - p0->c[1];
    uz = p1->c[2] - p0->c[2];
    ll = ux*ux + uy*uy + uz*uz;
    if ( ll < _MMG5_EPSD2 )  return(0);
    il = 1.0 / sqrt(ll);
    assert( (MG_REF & p0->tag) && (MG_REF & p1->tag) );

    /* Coordinates of the new point */
    if ( MG_SIN(p0->tag) ) {
        t0[0] = ux * il;
        t0[1] = uy * il;
        t0[2] = uz * il;
    }
    else {
        memcpy(t0,&(p0->n[0]),3*sizeof(double));
        ps = t0[0]*ux + t0[1]*uy + t0[2]*uz;
        if ( ps < 0.0 ) {
            t0[0] *= -1.0;
            t0[1] *= -1.0;
            t0[2] *= -1.0;
        }
    }
    if ( MG_SIN(p1->tag) ) {
        t1[0] = -ux * il;
        t1[1] = -uy * il;
        t1[2] = -uz * il;
    }
    else {
        memcpy(t1,&(p1->n[0]),3*sizeof(double));
        ps = - ( t1[0]*ux + t1[1]*uy + t1[2]*uz );
        if ( ps < 0.0 ) {
            t1[0] *= -1.0;
            t1[1] *= -1.0;
            t1[2] *= -1.0;
        }
    }

    alpha = _MMG5_BezierGeod(p0->c,p1->c,t0,t1);

    b0[0] = p0->c[0] + alpha * t0[0];
    b0[1] = p0->c[1] + alpha * t0[1];
    b0[2] = p0->c[2] + alpha * t0[2];

    b1[0] = p1->c[0] + alpha * t1[0];
    b1[1] = p1->c[1] + alpha * t1[1];
    b1[2] = p1->c[2] + alpha * t1[2];

    o[0] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[0] + 3.0*s*(1.0-s)*(1.0-s)*b0[0] + \
        3.0*s*s*(1.0-s)*b1[0] + s*s*s*p1->c[0];

    o[1] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[1] + 3.0*s*(1.0-s)*(1.0-s)*b0[1] + \
        3.0*s*s*(1.0-s)*b1[1] + s*s*s*p1->c[1];

    o[2] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[2] + 3.0*s*(1.0-s)*(1.0-s)*b0[2] + \
        3.0*s*s*(1.0-s)*b1[2] + s*s*s*p1->c[2];

    /* Coordinates of the new tangent and normal */
    if ( MG_SIN(p0->tag) && MG_SIN(p1->tag) ) {
        memcpy(to,t0,3*sizeof(double));
        return(1);
    }
    else if ( MG_SIN(p0->tag) ) {
        memcpy(n11,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
        memcpy(n01,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
        memcpy(n12,&(mesh->xpoint[p1->xp].n2[0]),3*sizeof(double));
        memcpy(n02,&(mesh->xpoint[p1->xp].n2[0]),3*sizeof(double));
    }
    else if ( MG_SIN(p1->tag) ) {
        memcpy(n01,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n11,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n02,&(mesh->xpoint[p0->xp].n2[0]),3*sizeof(double));
        memcpy(n12,&(mesh->xpoint[p0->xp].n2[0]),3*sizeof(double));
    }
    else {
        memcpy(n01,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n11,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
        memcpy(n02,&(mesh->xpoint[p0->xp].n2[0]),3*sizeof(double));
        memcpy(n12,&(mesh->xpoint[p1->xp].n2[0]),3*sizeof(double));
    }

    /* Switch normals of p1 for pairing */
    ps  = n01[0] * n11[0] + n01[1] * n11[1] + n01[2] * n11[2];
    ps2 = n01[0] * n12[0] + n01[1] * n12[1] + n01[2] * n12[2];
    if ( ps2 > ps ) {
        memcpy(ntemp,n11,3*sizeof(double));
        memcpy(n11,n12,3*sizeof(double));
        memcpy(n12,ntemp,3*sizeof(double));
    }

    /* Normal interpolation */
    ps = ux*(n01[0] + n11[0]) + uy*(n01[1] + n11[1]) + uz*(n01[2] + n11[2]);
    ps = 2.0*ps/ll;

    bn[0] = n01[0] + n11[0] -ps*ux;
    bn[1] = n01[1] + n11[1] -ps*uy;
    bn[2] = n01[2] + n11[2] -ps*uz;

    dd = bn[0]*bn[0] + bn[1]*bn[1] + bn[2]*bn[2];
    if ( dd > _MMG5_EPSD ) {
        dd = 1.0 / sqrt(dd);
        bn[0] *= dd;
        bn[1] *= dd;
        bn[2] *= dd;
    }
    no[0] = (1.0-s)*(1.0-s)*n01[0] + 2.0*s*(1.0-s)*bn[0] + s*s*n11[0];
    no[1] = (1.0-s)*(1.0-s)*n01[1] + 2.0*s*(1.0-s)*bn[1] + s*s*n11[1];
    no[2] = (1.0-s)*(1.0-s)*n01[2] + 2.0*s*(1.0-s)*bn[2] + s*s*n11[2];

    dd = no[0]*no[0] + no[1]*no[1] + no[2]*no[2];
    if ( dd > _MMG5_EPSD2 ) {
        dd = 1.0/sqrt(dd);
        no[0] *= dd;
        no[1] *= dd;
        no[2] *= dd;
    }

    /* Tangent interpolation : possibly flip (back) t1 */
    ps = t0[0]*t1[0] + t0[1]*t1[1] + t0[2]*t1[2];
    if ( ps < 0.0 ) {
        t1[0] *= -1.0;
        t1[1] *= -1.0;
        t1[2] *= -1.0;
    }
    to[0] = (1.0-s)*t0[0] + s*t1[0];
    to[1] = (1.0-s)*t0[1] + s*t1[1];
    to[2] = (1.0-s)*t0[2] + s*t1[2];

    /* Projection of the tangent in the tangent plane defined by no */
    ps = to[0]*no[0] + to[1]*no[1] + to[2]*no[2];
    to[0] = to[0] -ps*no[0];
    to[1] = to[1] -ps*no[1];
    to[2] = to[2] -ps*no[2];

    dd = to[0]*to[0] + to[1]*to[1] + to[2]*to[2];
    if ( dd > _MMG5_EPSD2) {
        dd = 1.0 / sqrt(dd);
        to[0] *= dd;
        to[1] *= dd;
        to[2] *= dd;
    }

    return(1);
}

/** Compute point located at parameter value step from point ip0, as well as interpolate
    of normals, tangent for a NOM edge */
inline int _MMG5_BezierNom(MMG5_pMesh mesh,int ip0,int ip1,double s,double *o,double *no,double *to) {
    MMG5_pPoint      p0,p1;
    double      ux,uy,uz,il,ll,ps,alpha,dd;
    double      t0[3],t1[3],b0[3],b1[3],n0[3],n1[3],bn[3];

    p0 = &mesh->point[ip0];
    p1 = &mesh->point[ip1];

    ux = p1->c[0] - p0->c[0];
    uy = p1->c[1] - p0->c[1];
    uz = p1->c[2] - p0->c[2];
    ll = ux*ux + uy*uy + uz*uz;

    if(ll < _MMG5_EPSD2) return(0);
    il = 1.0 / sqrt(ll);

    assert(( p0->tag & MG_NOM ) && ( p1->tag & MG_NOM ));

    /* Coordinates of the new point */
    if ( MG_SIN(p0->tag) ) {
        t0[0] = ux * il;
        t0[1] = uy * il;
        t0[2] = uz * il;
    }
    else {
        memcpy(t0,&(p0->n[0]),3*sizeof(double));
        ps = t0[0]*ux + t0[1]*uy + t0[2]*uz;
        if ( ps < 0.0 ) {
            t0[0] *= -1.0;
            t0[1] *= -1.0;
            t0[2] *= -1.0;
        }
    }
    if ( MG_SIN(p1->tag) ) {
        t1[0] = -ux * il;
        t1[1] = -uy * il;
        t1[2] = -uz * il;
    }
    else {
        memcpy(t1,&(p1->n[0]),3*sizeof(double));
        ps = - ( t1[0]*ux + t1[1]*uy + t1[2]*uz );
        if ( ps < 0.0 ) {
            t1[0] *= -1.0;
            t1[1] *= -1.0;
            t1[2] *= -1.0;
        }
    }

    alpha = _MMG5_BezierGeod(p0->c,p1->c,t0,t1);

    b0[0] = p0->c[0] + alpha * t0[0];
    b0[1] = p0->c[1] + alpha * t0[1];
    b0[2] = p0->c[2] + alpha * t0[2];

    b1[0] = p1->c[0] + alpha * t1[0];
    b1[1] = p1->c[1] + alpha * t1[1];
    b1[2] = p1->c[2] + alpha * t1[2];

    o[0] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[0] + 3.0*s*(1.0-s)*(1.0-s)*b0[0] + \
        3.0*s*s*(1.0-s)*b1[0] + s*s*s*p1->c[0];

    o[1] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[1] + 3.0*s*(1.0-s)*(1.0-s)*b0[1] + \
        3.0*s*s*(1.0-s)*b1[1] + s*s*s*p1->c[1];

    o[2] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[2] + 3.0*s*(1.0-s)*(1.0-s)*b0[2] + \
        3.0*s*s*(1.0-s)*b1[2] + s*s*s*p1->c[2];

    /* Coordinates of the new tangent and normal */
    if ( MG_SIN(p0->tag) && MG_SIN(p1->tag) ) {  // function should not be used in that case
        memcpy(to,t0,3*sizeof(double));
        return(1);
    }
    else if ( MG_SIN(p0->tag) ) {
        memcpy(n1,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
        memcpy(n0,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
    }
    else if ( MG_SIN(p1->tag) ) {
        memcpy(n0,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n1,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
    }
    else {
        memcpy(n0,&(mesh->xpoint[p0->xp].n1[0]),3*sizeof(double));
        memcpy(n1,&(mesh->xpoint[p1->xp].n1[0]),3*sizeof(double));
    }

    /* Normal interpolation */
    ps = ux*(n0[0] + n1[0]) + uy*(n0[1] + n1[1]) + uz*(n0[2] + n1[2]);
    ps = 2.0*ps/ll;

    bn[0] = n0[0] + n1[0] -ps*ux;
    bn[1] = n0[1] + n1[1] -ps*uy;
    bn[2] = n0[2] + n1[2] -ps*uz;

    dd = bn[0]*bn[0] + bn[1]*bn[1] + bn[2]*bn[2];
    if ( dd > _MMG5_EPSD ) {
        dd = 1.0 / sqrt(dd);
        bn[0] *= dd;
        bn[1] *= dd;
        bn[2] *= dd;
    }
    no[0] = (1.0-s)*(1.0-s)*n0[0] + 2.0*s*(1.0-s)*bn[0] + s*s*n1[0];
    no[1] = (1.0-s)*(1.0-s)*n0[1] + 2.0*s*(1.0-s)*bn[1] + s*s*n1[1];
    no[2] = (1.0-s)*(1.0-s)*n0[2] + 2.0*s*(1.0-s)*bn[2] + s*s*n1[2];

    dd = no[0]*no[0] + no[1]*no[1] + no[2]*no[2];
    if ( dd > _MMG5_EPSD2 ) {
        dd = 1.0/sqrt(dd);
        no[0] *= dd;
        no[1] *= dd;
        no[2] *= dd;
    }

    /* Tangent interpolation : possibly flip (back) t1 */
    ps = t0[0]*t1[0] + t0[1]*t1[1] + t0[2]*t1[2];
    if ( ps < 0.0 ) {
        t1[0] *= -1.0;
        t1[1] *= -1.0;
        t1[2] *= -1.0;
    }
    to[0] = (1.0-s)*t0[0] + s*t1[0];
    to[1] = (1.0-s)*t0[1] + s*t1[1];
    to[2] = (1.0-s)*t0[2] + s*t1[2];

    /* Projection of the tangent in the tangent plane defined by no */
    ps = to[0]*no[0] + to[1]*no[1] + to[2]*no[2];
    to[0] = to[0] -ps*no[0];
    to[1] = to[1] -ps*no[1];
    to[2] = to[2] -ps*no[2];

    dd = to[0]*to[0] + to[1]*to[1] + to[2]*to[2];
    if ( dd > _MMG5_EPSD2) {
        dd = 1.0 / sqrt(dd);
        to[0] *= dd;
        to[1] *= dd;
        to[2] *= dd;
    }

    return(1);
}

/** Compute point located at parameter value step from point ip0, as well as interpolate
    of normals, tangent for a regular edge ; v = ref vector (normal) for choice of normals if need be */
inline int _MMG5_BezierReg(MMG5_pMesh mesh,int ip0, int ip1, double s, double v[3], double *o, double *no){
    MMG5_pPoint p0,p1;
    double b0[3],b1[3],bn[3],t0[3],t1[3],np0[3],np1[3],alpha,ux,uy,uz,ps1,ps2,ll,il,dd,*n1,*n2;

    p0 = &mesh->point[ip0];
    p1 = &mesh->point[ip1];

    ux = p1->c[0] - p0->c[0];
    uy = p1->c[1] - p0->c[1];
    uz = p1->c[2] - p0->c[2];

    ll = ux*ux + uy*uy + uz*uz;

    /* Pathological case : don't move in that case ! */
    if(((MG_SIN(p0->tag)||(p0->tag & MG_NOM)) && (MG_SIN(p1->tag) || (p1->tag & MG_NOM)))||(ll<_MMG5_EPSD)){
        o[0] = 0.5*( p0->c[0] + p1->c[0] );
        o[1] = 0.5*( p0->c[1] + p1->c[1] );
        o[2] = 0.5*( p0->c[2] + p1->c[2] );

        memcpy(no,v,3*sizeof(double));
        return(1);
    }

    il = 1.0 /sqrt(ll);

    /* Coordinates of the new tangent and normal */
    if(MG_SIN(p0->tag)||(p0->tag & MG_NOM)){
        if(p1->tag & MG_GEO){
            n1 = &mesh->xpoint[p1->xp].n1[0];
            n2 = &mesh->xpoint[p1->xp].n2[0];
            ps1 = n1[0]*v[0] + n1[1]*v[1] + n1[2]*v[2];
            ps2 = n2[0]*v[0] + n2[1]*v[1] + n2[2]*v[2];

            if(fabs(ps1) < fabs(ps2)){
                memcpy(np1,&mesh->xpoint[p1->xp].n2[0],3*sizeof(double));
            }
            else{
                memcpy(np1,&mesh->xpoint[p1->xp].n1[0],3*sizeof(double));
            }
        }
        else{
            memcpy(np1,&mesh->xpoint[p1->xp].n1[0],3*sizeof(double));
        }
        memcpy(np0,np1,3*sizeof(double));
    }
    else if(MG_SIN(p1->tag) || (p1->tag & MG_NOM)){
        if(p0->tag & MG_GEO){
            n1 = &mesh->xpoint[p0->xp].n1[0];
            n2 = &mesh->xpoint[p0->xp].n2[0];
            ps1 = n1[0]*v[0] + n1[1]*v[1] + n1[2]*v[2];
            ps2 = n2[0]*v[0] + n2[1]*v[1] + n2[2]*v[2];

            if(fabs(ps1) < fabs(ps2)){
                memcpy(np0,&mesh->xpoint[p0->xp].n2[0],3*sizeof(double));
            }
            else{
                memcpy(np0,&mesh->xpoint[p0->xp].n1[0],3*sizeof(double));
            }
        }
        else{
            memcpy(np0,&mesh->xpoint[p0->xp].n1[0],3*sizeof(double));
        }
        memcpy(np1,np0,3*sizeof(double));
    }
    else{
        if(p0->tag & MG_GEO){
            n1 = &mesh->xpoint[p0->xp].n1[0];
            n2 = &mesh->xpoint[p0->xp].n2[0];
            ps1 = n1[0]*v[0] + n1[1]*v[1] + n1[2]*v[2];
            ps2 = n2[0]*v[0] + n2[1]*v[1] + n2[2]*v[2];

            if(fabs(ps1) < fabs(ps2)){
                memcpy(np0,&mesh->xpoint[p0->xp].n2[0],3*sizeof(double));
            }
            else{
                memcpy(np0,&mesh->xpoint[p0->xp].n1[0],3*sizeof(double));
            }
        }
        else{
            memcpy(np0,&mesh->xpoint[p0->xp].n1[0],3*sizeof(double));
        }

        if(p1->tag & MG_GEO){
            n1 = &mesh->xpoint[p1->xp].n1[0];
            n2 = &mesh->xpoint[p1->xp].n2[0];
            ps1 = n1[0]*v[0] + n1[1]*v[1] + n1[2]*v[2];
            ps2 = n2[0]*v[0] + n2[1]*v[1] + n2[2]*v[2];

            if(fabs(ps1) < fabs(ps2)){
                memcpy(np1,&mesh->xpoint[p1->xp].n2[0],3*sizeof(double));
            }
            else{
                memcpy(np1,&mesh->xpoint[p1->xp].n1[0],3*sizeof(double));
            }
        }
        else{
            memcpy(np1,&mesh->xpoint[p1->xp].n1[0],3*sizeof(double));
        }
    }

    /* Normal interpolation */
    ps1 = ux*(np0[0] + np1[0]) + uy*(np0[1] + np1[1]) + uz*(np0[2] + np1[2]);
    ps1 = 2.0*ps1/ll;

    bn[0] = np0[0] + np1[0] -ps1*ux;
    bn[1] = np0[1] + np1[1] -ps1*uy;
    bn[2] = np0[2] + np1[2] -ps1*uz;

    dd = bn[0]*bn[0] + bn[1]*bn[1] + bn[2]*bn[2];
    if(dd > _MMG5_EPSD){
        dd = 1.0/sqrt(dd);
        bn[0] *= dd;
        bn[1] *= dd;
        bn[2] *= dd;
    }

    no[0] = (1.0-s)*(1.0-s)*np0[0] + 2.0*s*(1.0-s)*bn[0] + s*s*np1[0];
    no[1] = (1.0-s)*(1.0-s)*np0[1] + 2.0*s*(1.0-s)*bn[1] + s*s*np1[1];
    no[2] = (1.0-s)*(1.0-s)*np0[2] + 2.0*s*(1.0-s)*bn[2] + s*s*np1[2];

    dd = no[0]*no[0] + no[1]*no[1] + no[2]*no[2];
    if(dd > _MMG5_EPSD){
        dd = 1.0/sqrt(dd);
        no[0] *= dd;
        no[1] *= dd;
        no[2] *= dd;
    }

    /* vertex position interpolation */
    if(!_MMG5_BezierTgt(p0->c,p1->c,np0,np1,t0,t1)){
        t0[0] = ux * il;
        t0[1] = uy * il;
        t0[2] = uz * il;

        t1[0] = - ux * il;
        t1[1] = - uy * il;
        t1[2] = - uz * il;
    }

    alpha = _MMG5_BezierGeod(p0->c,p1->c,t0,t1);

    b0[0] = p0->c[0] + alpha * t0[0];
    b0[1] = p0->c[1] + alpha * t0[1];
    b0[2] = p0->c[2] + alpha * t0[2];

    b1[0] = p1->c[0] + alpha * t1[0];
    b1[1] = p1->c[1] + alpha * t1[1];
    b1[2] = p1->c[2] + alpha * t1[2];

    o[0] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[0] + 3.0*s*(1.0-s)*(1.0-s)*b0[0] + \
        3.0*s*s*(1.0-s)*b1[0] + s*s*s*p1->c[0];

    o[1] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[1] + 3.0*s*(1.0-s)*(1.0-s)*b0[1] + \
        3.0*s*s*(1.0-s)*b1[1] + s*s*s*p1->c[1];

    o[2] = (1.0-s)*(1.0-s)*(1.0-s)*p0->c[2] + 3.0*s*(1.0-s)*(1.0-s)*b0[2] + \
        3.0*s*s*(1.0-s)*b1[2] + s*s*s*p1->c[2];

    return(1);
}

/** find the element number in packed numerotation */
int _MMG3D_indElt(MMG5_pMesh mesh, int kel) {
    MMG5_pTetra pt;
    int    ne, k;

    ne = 0;
    for (k=1; k<=mesh->ne; k++) {
        pt = &mesh->tetra[k];
        if ( MG_EOK(pt) ) {
            ne++;
            if ( k == kel )  return(ne);
        }
    }
    return(0);
}

/** find the point number in packed numerotation */
int _MMG3D_indPt(MMG5_pMesh mesh, int kp) {
    MMG5_pPoint ppt;
    int         np, k;

    np = 0;
    for (k=1; k<=mesh->ne; k++) {
        ppt = &mesh->point[k];
        if ( MG_VOK(ppt) ) {
            np++;
            if ( k == kp )  return(np);
        }
    }
    return(0);
}

/** Debug function (not use in clean code): print mesh->tria structure */
void _MMG5_printTetra(MMG5_pMesh mesh,char* fileName) {
    MMG5_pTetra  pt;
    MMG5_pxTetra pxt;
    int     k;
    FILE    *inm;

    inm = fopen(fileName,"w");

    fprintf(inm,"----------> %d _MMG5_TETRAHEDRAS <----------\n",mesh->ne);
    for(k=1; k<=mesh->ne; k++) {
        pt = &mesh->tetra[k];
        fprintf(inm,"num %d -> %d %d %d %d\n",k,pt->v[0],pt->v[1],
                pt->v[2],pt->v[3]);
        fprintf(inm,"ref,tag,xt  -> %d %d %d\n",pt->ref,pt->tag,pt->xt);
        if ( pt->xt ) {
            pxt = &mesh->xtetra[pt->xt];
            fprintf(inm,"tag   -> %d %d %d %d %d %d\n",pxt->tag[0],pxt->tag[1],
                    pxt->tag[2],pxt->tag[3],pxt->tag[4],pxt->tag[5]);
            fprintf(inm,"edg   -> %d %d %d %d %d %d\n",pxt->edg[0],pxt->edg[1],
                    pxt->edg[2],pxt->edg[3],pxt->edg[4],pxt->edg[5]);
            fprintf(inm,"ftag  -> %d %d %d %d\n",pxt->ftag[0],pxt->ftag[1],
                    pxt->ftag[2],pxt->ftag[3]);
            fprintf(inm,"ref   -> %d %d %d %d\n",pxt->ref[0],pxt->ref[1],
                    pxt->ref[2],pxt->ref[3]);
            fprintf(inm,"ori   -> %d \n",pxt->ori);
        }
        fprintf(inm,"\n");
    }
    fprintf(inm,"---------> END _MMG5_TETRAHEDRAS <--------\n");
    fclose(inm);
}
