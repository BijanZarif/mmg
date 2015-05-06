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
 * \file common/scalem.c
 * \brief Scale and unscale mesh and solution.
 * \author Charles Dapogny (LJLL, UPMC)
 * \author Cécile Dobrzynski (Inria / IMB, Université de Bordeaux)
 * \author Pascal Frey (LJLL, UPMC)
 * \author Algiane Froehly (Inria / IMB, Université de Bordeaux)
 * \version 5
 * \copyright GNU Lesser General Public License.
 */

#include "mmg.h"

/**
 * \param mesh pointer toward the mesh structure.
 * \return 1 if success, 0 if fail (computed bounding box too small).
 *
 * Compute the mesh bounding box and fill the \a min, \a max and \a delta fields
 * of the \ref _MMG5_info structure.
 *
 */
int _MMG5_boundingBox(MMG5_pMesh mesh) {
  MMG5_pPoint    ppt;
  int            k,i;
  double         dd;

  /* compute bounding box */
  for (i=0; i<3; i++) {
    mesh->info.min[i] =  DBL_MAX;
    mesh->info.max[i] = -DBL_MAX;
  }
  for (k=1; k<=mesh->np; k++) {
    ppt = &mesh->point[k];
    if ( !MG_VOK(ppt) )  continue;
    for (i=0; i<3; i++) {
      if ( ppt->c[i] > mesh->info.max[i] )  mesh->info.max[i] = ppt->c[i];
      if ( ppt->c[i] < mesh->info.min[i] )  mesh->info.min[i] = ppt->c[i];
    }
    ppt->tmp = 0;
  }
  mesh->info.delta = 0.0;
  for (i=0; i<3; i++) {
    dd = mesh->info.max[i] - mesh->info.min[i];
    if ( dd > mesh->info.delta )  mesh->info.delta = dd;
  }
  if ( mesh->info.delta < _MMG5_EPSD ) {
    fprintf(stdout,"  ## Unable to scale mesh.\n");
    return(0);
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward the metric or solution structure.
 * \return 1 if success, 0 if fail (computed bounding box too small).
 *
 * Scale the mesh and the size informations between 0 and 1.
 *
 */
int _MMG5_scaleMesh(MMG5_pMesh mesh,MMG5_pSol met
#ifdef SINGUL
                    , MMG5_pSingul sing
#endif
) {
  MMG5_pPoint    ppt;
  double         dd,d1;
  int            k,sethmin,sethmax;
  MMG5_pPar      par;
#ifdef SINGUL
  MMG5_psPoint   ppts;
  double         deltb,delta[3];
  int            i;
#endif


  /* compute bounding box */
  if ( ! _MMG5_boundingBox(mesh) ) return(0);

  /* normalize coordinates */
  dd = 1.0 / mesh->info.delta;
  for (k=1; k<=mesh->np; k++) {
    ppt = &mesh->point[k];
    if ( !MG_VOK(ppt) )  continue;
    ppt->c[0] = dd * (ppt->c[0] - mesh->info.min[0]);
    ppt->c[1] = dd * (ppt->c[1] - mesh->info.min[1]);
    ppt->c[2] = dd * (ppt->c[2] - mesh->info.min[2]);
  }

  /* normalize values */
  sethmin = 0;
  sethmax = 0;
  if ( mesh->info.hmin > 0. ) {
    mesh->info.hmin  *= dd;
    sethmin = 1;
  }
  else mesh->info.hmin  = 0.01;

  if ( mesh->info.hmax > 0. ) {
    mesh->info.hmax  *= dd;
    sethmax = 1;
  }
  else mesh->info.hmax  = 1.;

  if ( mesh->info.hmax < mesh->info.hmin ) {
    if ( sethmin && sethmax ) {
      fprintf(stdout,"  ## ERROR: MISMATCH PARAMETERS:"
              "MINIMAL MESH SIZE LARGER THAN MAXIMAL ONE.\n");
      fprintf(stdout,"  Exit program.\n");
      exit(EXIT_FAILURE);
    }
    else if ( sethmin )
      mesh->info.hmax = 100. * mesh->info.hmin;
    else
      mesh->info.hmin = 0.01 * mesh->info.hmax;
  }


  mesh->info.hausd *= dd;

  /* normalize sizes */
  if ( met->m ) {
    if ( met->size == 1 ) {
      for (k=1; k<=mesh->np; k++)    met->m[k] *= dd;
    }
    else if ( met->size==3 ){
      for (k=1; k<=mesh->np; k++) {
        met->m[3*k]   *= dd;
        met->m[3*k+1] *= dd;
        met->m[3*k+2] *= dd;
      }
    } else { //met->size==6
      d1 = 1.0 / (dd*dd);
      for (k=6; k<6*(mesh->np+1); k++)  met->m[k] *= d1;
    }
  }

#ifdef SINGUL
  /* 2nd mesh (sing) is quarter sized */
  /* Get the size of sing in every direction */
  if ( mesh->info.sing && sing->ns ) {
    deltb = 0.0;

    for (i=0; i<mesh->dim; i++) {
      delta[i] = fabs(sing->max[i]-sing->min[i]);
      if ( delta[i] > deltb )  deltb = delta[i];   // deltb = max dimension
    }
    if ( deltb < _MMG5_EPSD ) {
      fprintf(stdout,"  ## Unable to scale mesh\n");
      return(0);
    }

    /* centering */
    dd = 1.0 / deltb;
    for (k=1; k<=sing->ns; k++) {
      ppts = &sing->point[k];
      for (i=0; i<mesh->dim; i++) {
        ppts->c[i] = MMG5_SIZE * (dd * (ppts->c[i]-sing->min[i])) +
          0.5 * (1.0 - MMG5_SIZE*dd*delta[i]);
      }
    }
  }
#endif

  /* normalize local parameters */
  for (k=0; k<mesh->info.npar; k++) {
    par = &mesh->info.par[k];
    par->hmin  *= dd;
    par->hmax  *= dd;
    par->hausd *= dd;
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward the metric or solution structure.
 * \return 1.
 *
 * Unscale the mesh and the size informations to their initial sizes.
 *
 */
int _MMG5_unscaleMesh(MMG5_pMesh mesh,MMG5_pSol met) {
  MMG5_pPoint     ppt;
  double          dd;
  int             k,i;
  MMG5_pPar       par;

  /* de-normalize coordinates */
  dd = mesh->info.delta;
  for (k=1; k<=mesh->np; k++) {
    ppt = &mesh->point[k];
    if ( !MG_VOK(ppt) )  continue;
    ppt->c[0] = ppt->c[0] * dd + mesh->info.min[0];
    ppt->c[1] = ppt->c[1] * dd + mesh->info.min[1];
    ppt->c[2] = ppt->c[2] * dd + mesh->info.min[2];
  }

  /* unscale sizes */
  if ( met->m ) {
    if ( met->size == 6 ) {
      dd = 1.0 / (dd*dd);
      for (k=1; k<=mesh->np; k++) {
        ppt = &mesh->point[k];
        if ( !MG_VOK(ppt) )  continue;
        for (i=0; i<6; i++)  met->m[6*k+i] *= dd;
      }
    }
    else {
      for (k=1; k<=mesh->np ; k++) {
        ppt = &mesh->point[k];
        if ( MG_VOK(ppt) )  met->m[k] *= dd;
      }
    }
  }

  /* unscale paramter values */
  mesh->info.hmin  *= dd;
  mesh->info.hmax  *= dd;
  mesh->info.hausd *= dd;

  /* normalize local parameters */
  for (k=0; k<mesh->info.npar; k++) {
    par = &mesh->info.par[k];
    par->hausd *= dd;
  }

  return(1);
}
