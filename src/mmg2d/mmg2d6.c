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
#include "mmg2d.h"


int MMG2_mmg2d6(MMG5_pMesh mesh, MMG5_pSol sol) {
  MMG5_pTria  pt;
  MMG5_pPoint ppt;
  double maxls;
  int k,i;

  if ( abs(mesh->info.imprim) > 3 )
    fprintf(stdout,"  ** ISOSURFACE EXTRACTION\n");

 /* base used vertices */
  for (k=1; k<=mesh->nt; k++) {
    pt = &mesh->tria[k];
    
    if ( !M_EOK(pt) )  continue;
    for (i=0; i<3; i++) {
      ppt = &mesh->point[ pt->v[i] ];
      ppt->tag &= ~M_NUL;
    }
  }       
  
  /*delete triangles which are inside the obstacles*/
   for (k=1; k<=mesh->nt; k++) {
    pt = &mesh->tria[k];
    
    if ( !M_EOK(pt) )  continue;
    maxls = -10000;
    for (i=0; i<3; i++) {
      maxls = M_MAX(sol->m[pt->v[i]],maxls);
    }
    if(!(maxls > 1e-4)) {
      _MMG2D_delElt(mesh,k);
    }
  }   

   if ( !MMG2_hashel(mesh) )  return(1);    
 
   for(k=1 ; k<=mesh->na ; k++) {
     if(!mesh->edge[k].ref) {
       mesh->edge[k].ref = 4;
     }

   }
  for (k=1; k<=mesh->np; k++) {
    ppt = &mesh->point[k];
    
    ppt->tag |= M_NUL;
    
  }       

  for (k=1; k<=mesh->nt; k++) {
    pt = &mesh->tria[k];
    
    if ( !M_EOK(pt) )  continue;
    for (i=0; i<3; i++) {
      ppt = &mesh->point[ pt->v[i] ];
      ppt->tag &= ~M_NUL;
    }
  }

  /* Clean memory */
  _MMG5_DEL_MEM(mesh,sol->m,(sol->size*(sol->npmax+1))*sizeof(double));

  return(1);
}

