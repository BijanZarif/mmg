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
 * \file mmg3d/API_functions.c
 * \brief C API functions definitions for MMG3D library.
 * \author Algiane Froehly (Inria / IMB, Université de Bordeaux)
 * \version 5
 * \date 01 2014
 * \copyright GNU Lesser General Public License.
 * \warning Use the MMG3D_ prefix: MMG5_ prefix will became obsolete soon...
 *
 * \note This file contains some internal functions for the API, see
 * the \ref mmg3d/libmmg3d.h header file for the documentation of all
 * the usefull user's API functions.
 *
 * C API for MMG3D library.
 *
 */

#include "mmg3d.h"

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward a sol structure (metric or level-set).
 * \param disp pointer toward a sol structure (displacement).
 *
 * Allocate the mesh and solutions structures at \a MMG3D format.
 *
 */
static inline
void _MMG3D_Alloc_mesh(MMG5_pMesh *mesh, MMG5_pSol *sol, MMG5_pSol *disp
  ) {

  /* mesh allocation */
  if ( *mesh )  _MMG5_SAFE_FREE(*mesh);
  _MMG5_SAFE_CALLOC(*mesh,1,MMG5_Mesh);

  /* sol allocation */
  if ( !sol ) {
    printf("  ## Error: an allocatable solution structure of type \"MMG5_pSol\""
           " is needed.\n");
    printf("            Exit program.\n");
    exit(EXIT_FAILURE);
  }

  if ( *sol )  _MMG5_DEL_MEM(*mesh,*sol,sizeof(MMG5_Sol));
  _MMG5_SAFE_CALLOC(*sol,1,MMG5_Sol);

  /* displacement allocation */
  if ( disp ) {
    if ( *disp )
      _MMG5_DEL_MEM(*mesh,*disp,sizeof(MMG5_Sol));
    _MMG5_SAFE_CALLOC(*disp,1,MMG5_Sol);
  }

  return;
}
/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward a sol structure (metric or level-set).
 * \param disp pointer toward a sol structure (displacement).
 *
 * Initialization of mesh and solution structures to their default
 * values (default names, versions, dimensions...).
 *
 */
static inline
void _MMG3D_Init_woalloc_mesh(MMG5_pMesh mesh, MMG5_pSol sol, MMG5_pSol disp
  ) {

  _MMG3D_Set_commonFunc();

  (mesh)->dim  = 3;
  (mesh)->ver  = 2;
  (sol)->dim   = 3;
  (sol)->ver   = 2;
  (sol)->size  = 1;
  if ( disp ) {
    (disp)->dim  = 3;
    (disp)->ver  = 2;
    (disp)->size = 2;
  }

  /* Default parameters values */
  MMG3D_Init_parameters(mesh);

  /* Default vaules for file names */
  MMG3D_Init_fileNames(mesh,sol);

  if ( disp ) {
    MMG3D_Set_inputSolName(mesh,disp,"");
    MMG3D_Set_outputSolName(mesh,disp,"");
  }

  return;
}
/**
 * \param mesh adress of a pointer toward a pointer toward the mesh structure.
 * \param sol adress of a pointer toward a sol structure (metric or level-set).
 * \param disp adress of a pointer toward a sol structure (displacement for
 * the lagrangian mode).
 *
 * Allocate the mesh and solution structures and initialize it to
 * their default values.
 *
 */
void MMG3D_Init_mesh(MMG5_pMesh *mesh, MMG5_pSol *sol, MMG5_pSol *disp ) {

  /* allocations */
  _MMG3D_Alloc_mesh(mesh,sol,disp);
  /* initialisations */
  if ( disp )
    _MMG3D_Init_woalloc_mesh(*mesh,*sol,*disp);
  else
    _MMG3D_Init_woalloc_mesh(*mesh,*sol,NULL);

  /* set pointer to save the mesh*/
  _MMG3D_saveMeshinternal = _MMG5_saveLibraryMesh;

  return;
}
/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 *
 * Initialize file names to their default values.
 *
 */
void MMG3D_Init_fileNames(MMG5_pMesh mesh,MMG5_pSol sol
  ) {

  MMG5_Init_fileNames(mesh,sol);
  return;
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param meshin input mesh name.
 * \return 1.
 *
 * Set the name of input mesh.
 *
 */
int MMG3D_Set_inputMeshName(MMG5_pMesh mesh, char* meshin) {

  return(MMG5_Set_inputMeshName(mesh,meshin));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param solin name of the input solution file.
 * \return 1.
 *
 * Set the name of input solution file.
 *
 */
int MMG3D_Set_inputSolName(MMG5_pMesh mesh,MMG5_pSol sol, char* solin) {
  return(MMG5_Set_inputSolName(mesh,sol,solin));
}
/**
 * \param mesh pointer toward the mesh structure.
 * \param meshout output mesh name.
 * \return 1.
 *
 * Set the name of output mesh.
 *
 */
int MMG3D_Set_outputMeshName(MMG5_pMesh mesh, char* meshout) {

  return(MMG5_Set_outputMeshName(mesh,meshout));
}
/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param solout name of the output solution file.
 * \return 0 if failed, 1 otherwise.
 *
 *  Set the name of output solution file.
 *
 */
int MMG3D_Set_outputSolName(MMG5_pMesh mesh,MMG5_pSol sol, char* solout) {
  return(MMG5_Set_outputSolName(mesh,sol,solout));
}
/**
 * \param mesh pointer toward the mesh structure.
 *
 * Initialization of the input parameters (stored in the Info structure).
 *
 */
void MMG3D_Init_parameters(MMG5_pMesh mesh) {

  /* Init common parameters for mmgs and mmg3d. */
  _MMG5_Init_parameters(mesh);

  /* default values for integers */
  /** MMG3D_IPARAM_iso = 0 */
  mesh->info.iso      =  0;  /* [0/1]    ,Turn on/off levelset meshing */
  /** MMG3D_IPARAM_lag = -1 */
  mesh->info.lag      = -1;
  /** MMG3D_IPARAM_optim = 0 */
  mesh->info.optim    =  0;
  /** MMG3D_IPARAM_nosurf = 0 */
  mesh->info.nosurf   =  0;  /* [0/1]    ,avoid/allow surface modifications */
#ifdef USE_SCOTCH
  mesh->info.renum    = 1;   /* [1/0]    , Turn on/off the renumbering using SCOTCH; */
#else
  mesh->info.renum    = 0;   /* [0]    , Turn on/off the renumbering using SCOTCH; */
#endif

  /* default values for doubles */
  mesh->info.ls       = 0.0;      /* level set value */


#ifndef PATTERN
  /** MMG3D_IPARAM_bucket = 64 */
  mesh->info.bucket = 64;
#endif
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param typEntity type of solutions entities (vertices, triangles...).
 * \param np number of solutions.
 * \param typSol type of solution (scalar, vectorial...).
 * \return 0 if failed, 1 otherwise.
 *
 * Set the solution number, dimension and type.
 *
 */
int MMG3D_Set_solSize(MMG5_pMesh mesh, MMG5_pSol sol, int typEntity, int np, int typSol) {

  if ( ( (mesh->info.imprim > 5) || mesh->info.ddebug ) && sol->m )
    fprintf(stdout,"  ## Warning: new solution\n");

  if ( typEntity != MMG5_Vertex ) {
    fprintf(stdout,"  ## Error: MMG3D5 need a solution imposed on vertices\n");
    return(0);
  }
  if ( typSol == MMG5_Scalar ) {
    sol->size = 1;
  }
  else if ( typSol == MMG5_Vector ) {
    sol->size = 3;
  }
  else if ( typSol == MMG5_Tensor ) {
    sol->size = 6;
  }
  else {
    fprintf(stdout,"  ## Error: type of solution not yet implemented\n");
    return(0);
  }

  sol->dim = 3;
  if ( np ) {
    sol->np  = np;
    sol->npi = np;
    if ( sol->m )
      _MMG5_DEL_MEM(mesh,sol->m,(sol->size*(sol->npmax+1))*sizeof(double));

    sol->npmax = mesh->npmax;
    _MMG5_ADD_MEM(mesh,(sol->size*(sol->npmax+1))*sizeof(double),"initial solution",
                  printf("  Exit program.\n");
                  exit(EXIT_FAILURE));
    _MMG5_SAFE_CALLOC(sol->m,(sol->size*(sol->npmax+1)),double);
  }
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param np number of vertices.
 * \param ne number of elements (tetrahedra).
 * \param nt number of triangles.
 * \param na number of edges.
 * \return 0 if failed, 1 otherwise.
 *
 * Set the number of vertices, tetrahedra, triangles and edges of the
 * mesh and allocate the associated tables. If call twice, reset the
 * whole mesh to realloc it at the new size
 *
 */
int MMG3D_Set_meshSize(MMG5_pMesh mesh, int np, int ne, int nt, int na) {
  int k;

  if ( ( (mesh->info.imprim > 5) || mesh->info.ddebug ) &&
       ( mesh->point || mesh->tria || mesh->tetra || mesh->edge) )
    fprintf(stdout,"  ## Warning: new mesh\n");

  mesh->np  = np;
  mesh->ne  = ne;
  mesh->nt  = nt;
  mesh->na  = na;
  mesh->npi = mesh->np;
  mesh->nei = mesh->ne;
  mesh->nti = mesh->nt;
  mesh->nai = mesh->na;

  if ( mesh->point )
    _MMG5_DEL_MEM(mesh,mesh->point,(mesh->npmax+1)*sizeof(MMG5_Point));
  if ( mesh->tetra )
    _MMG5_DEL_MEM(mesh,mesh->tetra,(mesh->nemax+1)*sizeof(MMG5_Tetra));
  if ( mesh->tria )
    _MMG5_DEL_MEM(mesh,mesh->tria,(mesh->nt+1)*sizeof(MMG5_Tria));
  if ( mesh->edge )
    _MMG5_DEL_MEM(mesh,mesh->edge,(mesh->na+1)*sizeof(MMG5_Edge));

  /*tester si -m definie : renvoie 0 si pas ok et met la taille min dans info.mem */
  if( mesh->info.mem > 0) {
    if((mesh->npmax < mesh->np || mesh->ntmax < mesh->nt || mesh->nemax < mesh->ne)) {
      _MMG3D_memOption(mesh);
      //     printf("pas de pbs ? %d %d %d %d %d %d -- %d\n",mesh->npmax,mesh->np,
      //     mesh->ntmax,mesh->nt,mesh->nemax,mesh->ne,mesh->info.mem);
      if((mesh->npmax < mesh->np || mesh->ntmax < mesh->nt
          || mesh->nemax < mesh->ne)) {
        fprintf(stdout,"not enough memory: np : %d %d nt : %d %d ne :%d %d\n"
                ,mesh->npmax,mesh->np,
                mesh->ntmax,mesh->nt,mesh->nemax,mesh->ne);
        return(0);
      }
    } else if(mesh->info.mem < 39) {
      printf("not enough memory  %d\n",mesh->info.mem);
      return(0);
    }
  } else {
    mesh->npmax = MG_MAX(1.5*mesh->np,_MMG5_NPMAX);
    mesh->nemax = MG_MAX(1.5*mesh->ne,_MMG5_NEMAX);
    mesh->ntmax = MG_MAX(1.5*mesh->nt,_MMG5_NTMAX);

  }
  _MMG5_ADD_MEM(mesh,(mesh->npmax+1)*sizeof(MMG5_Point),"initial vertices",
                printf("  Exit program.\n");
                exit(EXIT_FAILURE));
  _MMG5_SAFE_CALLOC(mesh->point,mesh->npmax+1,MMG5_Point);


  _MMG5_ADD_MEM(mesh,(mesh->nemax+1)*sizeof(MMG5_Tetra),"initial tetrahedra",
                printf("  Exit program.\n");
                exit(EXIT_FAILURE));
  _MMG5_SAFE_CALLOC(mesh->tetra,mesh->nemax+1,MMG5_Tetra);


  if ( mesh->nt ) {
    _MMG5_ADD_MEM(mesh,(mesh->nt+1)*sizeof(MMG5_Tria),"initial triangles",return(0));
    _MMG5_SAFE_CALLOC(mesh->tria,mesh->nt+1,MMG5_Tria);
  }


  mesh->namax = mesh->na;
  if ( mesh->na ) {
    _MMG5_ADD_MEM(mesh,(mesh->na+1)*sizeof(MMG5_Edge),"initial edges",return(0));
    _MMG5_SAFE_CALLOC(mesh->edge,(mesh->na+1),MMG5_Edge);
  }

  /* keep track of empty links */
  mesh->npnil = mesh->np + 1;
  mesh->nenil = mesh->ne + 1;
  for (k=mesh->npnil; k<mesh->npmax-1; k++) {
    mesh->point[k].tmp  = k+1;
  }
  for (k=mesh->nenil; k<mesh->nemax-1; k++) {
    mesh->tetra[k].v[3] = k+1;
  }

  /* stats */
  if ( abs(mesh->info.imprim) > 6 ) {
    fprintf(stdout,"     NUMBER OF VERTICES     %8d\n",mesh->np);
    if ( mesh->na ) {
      fprintf(stdout,"     NUMBER OF EDGES        %8d\n",mesh->na);
    }
    if ( mesh->nt )
      fprintf(stdout,"     NUMBER OF TRIANGLES    %8d\n",mesh->nt);
    fprintf(stdout,"     NUMBER OF ELEMENTS     %8d\n",mesh->ne);
  }
  return(1);
}


/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param typEntity pointer toward the type of entities to which solutions are applied.
 * \param np pointer toward the number of solutions.
 * \param typSol pointer toward the type of the solutions (scalar, vectorial...)
 * \return 1.
 *
 * Get the solution number, dimension and type.
 *
 */
int MMG3D_Get_solSize(MMG5_pMesh mesh, MMG5_pSol sol, int* typEntity, int* np, int* typSol) {

  *typEntity = MMG5_Vertex;
  if ( sol->size == 1 )
    *typSol    = MMG5_Scalar;
  else if ( sol->size == 3 )
    *typSol    = MMG5_Vector;
  else if ( sol->size == 6 )
    *typSol    = MMG5_Tensor;
  else
    *typSol    = MMG5_Notype;

  assert( (!sol->np) || (sol->np == mesh->np));

  *np = sol->np;
  sol->npi = 0;

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param np pointer toward the number of vertices.
 * \param ne pointer toward the number of elements (tetrahedra).
 * \param nt pointer toward the number of triangles.
 * \param na pointer toward the number of edges.
 * \return 1.
 *
 * Get the number of vertices, tetrahedra, triangles and edges of the mesh.
 *
 */
int MMG3D_Get_meshSize(MMG5_pMesh mesh, int* np, int* ne, int* nt, int* na) {

  if ( np != NULL )
    *np = mesh->np;
  if ( ne != NULL )
    *ne = mesh->ne;
  if ( nt != NULL )
    *nt = mesh->nt;
  if ( na != NULL )
    *na = mesh->na;

  mesh->npi = 0;
  mesh->nei = 0;
  mesh->nti = 0;
  mesh->nai = 0;

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param c0 coordinate of the point along the first dimension.
 * \param c1 coordinate of the point along the second dimension.
 * \param c2 coordinate of the point along the third dimension.
 * \param ref point reference.
 * \param pos position of the point in the mesh.
 * \return 1.
 *
 * Set vertex of coordinates \a c0, \a c1,\a c2 and reference \a ref
 * at position \a pos in mesh structure
 *
 */
int MMG3D_Set_vertex(MMG5_pMesh mesh, double c0, double c1, double c2, int ref, int pos) {

  if ( !mesh->np ) {
    fprintf(stdout,"  ## Error: you must set the number of points with the");
    fprintf(stdout," MMG3D_Set_meshSize function before setting vertices in mesh\n");
    return(0);
  }

  if ( pos > mesh->npmax ) {
    fprintf(stdout,"  ## Error: unable to allocate a new point.\n");
    fprintf(stdout,"    max number of points: %d\n",mesh->npmax);
    _MMG5_INCREASE_MEM_MESSAGE();
    return(0);
  }

  if ( pos > mesh->np ) {
    fprintf(stdout,"  ## Error: attempt to set new vertex at position %d.",pos);
    fprintf(stdout," Overflow of the given number of vertices: %d\n",mesh->np);
    fprintf(stdout,"  ## Check the mesh size, its compactness or the position");
    fprintf(stdout," of the vertex.\n");
    return(0);
  }

  mesh->point[pos].c[0] = c0;
  mesh->point[pos].c[1] = c1;
  mesh->point[pos].c[2] = c2;
  mesh->point[pos].ref  = ref;
  mesh->point[pos].tag  = MG_NUL;
  mesh->point[pos].flag = 0;
  mesh->point[pos].tmp = 0;

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param c0 pointer toward the coordinate of the point along the first dimension.
 * \param c1 pointer toward the coordinate of the point along the second dimension.
 * \param c2 pointer toward the coordinate of the point along the third dimension.
 * \param ref poiter to the point reference.
 * \param isCorner pointer toward the flag saying if point is corner.
 * \param isRequired pointer toward the flag saying if point is required.
 * \return 1.
 *
 * Get coordinates \a c0, \a c1,\a c2 and reference \a ref of next
 * vertex of mesh.
 *
 */
int MMG3D_Get_vertex(MMG5_pMesh mesh, double* c0, double* c1, double* c2, int* ref,
                    int* isCorner, int* isRequired) {

  mesh->npi++;

  if ( mesh->npi > mesh->np ) {
    fprintf(stdout,"  ## Error: unable to get point.\n");
    fprintf(stdout,"     The number of call of MMG3D_Get_vertex function");
    fprintf(stdout," can not exceed the number of points: %d\n ",mesh->np);
    return(0);
  }

  *c0  = mesh->point[mesh->npi].c[0];
  *c1  = mesh->point[mesh->npi].c[1];
  *c2  = mesh->point[mesh->npi].c[2];
  if ( ref != NULL )
    *ref = mesh->point[mesh->npi].ref;

  if ( isCorner != NULL ) {
    if ( mesh->point[mesh->npi].tag & MG_CRN )
      *isCorner = 1;
    else
      *isCorner = 0;
  }

  if ( isRequired != NULL ) {
    if ( mesh->point[mesh->npi].tag & MG_REQ )
      *isRequired = 1;
    else
      *isRequired = 0;
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 first vertex of tetrahedron.
 * \param v1 second vertex of tetrahedron.
 * \param v2 third vertex of tetrahedron.
 * \param v3 fourth vertex of tetrahedron.
 * \param ref tetrahedron reference (must be positive).
 * \param pos tetrahedron position in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set tetrahedra of vertices \a v0, \a v1,\a v2,\a v3 and reference
 * \a ref at position \a pos in mesh structure.
 *
 */
int MMG3D_Set_tetrahedron(MMG5_pMesh mesh, int v0, int v1, int v2, int v3, int ref, int pos) {
  MMG5_pTetra pt;
  MMG5_pPoint ppt;
  double aux, vol;
  int    j, ip;

  if ( !mesh->ne ) {
    fprintf(stdout,"  ## Error: You must set the number of elements with the");
    fprintf(stdout," MMG3D_Set_meshSize function before setting elements in mesh\n");
    return(0);
  }

  if ( pos > mesh->nemax ) {
    fprintf(stdout,"  ## Error: unable to allocate a new element.\n");
    fprintf(stdout,"    max number of element: %d\n",mesh->nemax);
    _MMG5_INCREASE_MEM_MESSAGE();
    return(0);
  }

  if ( pos > mesh->ne ) {
    fprintf(stdout,"  ## Error: attempt to set new tetrahedron at position %d.",pos);
    fprintf(stdout," Overflow of the given number of tetrahedron: %d\n",mesh->ne);
    fprintf(stdout,"  ## Check the mesh size, its compactness or the position");
    fprintf(stdout," of the tetrahedron.\n");
    return(0);
  }

  pt = &mesh->tetra[pos];
  pt->v[0] = v0;
  pt->v[1] = v1;
  pt->v[2] = v2;
  pt->v[3] = v3;
  pt->ref  = abs(ref);

  mesh->point[pt->v[0]].tag &= ~MG_NUL;
  mesh->point[pt->v[1]].tag &= ~MG_NUL;
  mesh->point[pt->v[2]].tag &= ~MG_NUL;
  mesh->point[pt->v[3]].tag &= ~MG_NUL;

  vol = _MMG5_orvol(mesh->point,pt->v);
  if ( vol == 0.0 ) {
    fprintf(stdout,"  ## Error: tetrahedron %d has volume null.\n",pos);
    for ( ip=0; ip<4; ip++ ) {
      ppt = &mesh->point[pt->v[ip]];
      for ( j=0; j<3; j++ ) {
        if ( fabs(ppt->c[j])>0. ) {
          fprintf(stdout," Check that you don't have a sliver tetrahedron.\n");
          return(0);
        }
      }
    }
    fprintf(stdout,"  All vertices have zero coordinates.");
    fprintf(stdout," Check that you have set the vertices before the tetrahedra.\n");
    return(0);
  }
  else if ( vol < 0.0 ) {
    /* Possibly switch 2 vertices number so that each tet is positively oriented */
    aux = pt->v[2];
    pt->v[2] = pt->v[3];
    pt->v[3] = aux;
    /* mesh->xt temporary used to count reoriented tetra */
    mesh->xt++;
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 pointer toward the first vertex of tetrahedron.
 * \param v1 pointer toward the second vertex of tetrahedron.
 * \param v2 pointer toward the third vertex of tetrahedron.
 * \param v3 pointer toward the fourth vertex of tetrahedron.
 * \param ref pointer toward the tetrahedron reference.
 * \param isRequired pointer toward the flag saying if tetrahedron is required.
 * \return 0 if failed, 1 otherwise.
 *
 * Get vertices \a v0, \a v1, \a v2, \a v3 and reference \a ref of
 * next tetra of mesh.
 *
 */
int MMG3D_Get_tetrahedron(MMG5_pMesh mesh, int* v0, int* v1, int* v2, int* v3,
                         int* ref, int* isRequired) {

  mesh->nei++;

  if ( mesh->nei > mesh->ne ) {
    fprintf(stdout,"  ## Error: unable to get tetra.\n");
    fprintf(stdout,"    The number of call of MMG3D_Get_tetrahedron function");
    fprintf(stdout," can not exceed the number of tetra: %d\n ",mesh->ne);
    return(0);
  }

  *v0  = mesh->tetra[mesh->nei].v[0];
  *v1  = mesh->tetra[mesh->nei].v[1];
  *v2  = mesh->tetra[mesh->nei].v[2];
  *v3  = mesh->tetra[mesh->nei].v[3];
  if ( ref != NULL ) {
    *ref = mesh->tetra[mesh->nei].ref;
  }

  if ( isRequired != NULL ) {
    if ( mesh->tetra[mesh->nei].tag & MG_REQ )
      *isRequired = 1;
    else
      *isRequired = 0;
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 first vertex of triangle.
 * \param v1 second vertex of triangle.
 * \param v2 third vertex of triangle.
 * \param ref triangle reference.
 * \param pos triangle position in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set triangle of vertices \a v0, \a v1, \a v2 and reference \a ref
 * at position \a pos in mesh structure.
 *
 */
int MMG3D_Set_triangle(MMG5_pMesh mesh, int v0, int v1, int v2, int ref,int pos) {

  if ( !mesh->nt ) {
    fprintf(stdout,"  ## Error: You must set the number of triangles with the");
    fprintf(stdout," MMG3D_Set_meshSize function before setting triangles in mesh\n");
    return(0);
  }

  if ( pos > mesh->ntmax ) {
    fprintf(stdout,"  ## Error: unable to allocate a new triangle.\n");
    fprintf(stdout,"    max number of triangle: %d\n",mesh->ntmax);
    _MMG5_INCREASE_MEM_MESSAGE();
    return(0);
  }

  if ( pos > mesh->nt ) {
    fprintf(stdout,"  ## Error: attempt to set new triangle at position %d.",pos);
    fprintf(stdout," Overflow of the given number of triangles: %d\n",mesh->nt);
    fprintf(stdout,"  ## Check the mesh size, its compactness or the position");
    fprintf(stdout," of the triangle.\n");
    return(0);
  }

  mesh->tria[pos].v[0] = v0;
  mesh->tria[pos].v[1] = v1;
  mesh->tria[pos].v[2] = v2;
  mesh->tria[pos].ref  = ref;

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 pointer toward the first vertex of triangle.
 * \param v1 pointer toward the second vertex of triangle.
 * \param v2 pointer toward the third vertex of triangle.
 * \param ref pointer toward the triangle reference.
 * \param isRequired pointer toward the flag saying if triangle is required.
 * \return 0 if failed, 1 otherwise.
 *
 * Get vertices \a v0,\a v1,\a v2 and reference \a ref of next
 * triangle of mesh.
 *
 */
int MMG3D_Get_triangle(MMG5_pMesh mesh, int* v0, int* v1, int* v2, int* ref
                      ,int* isRequired) {
  MMG5_pTria  ptt;

  mesh->nti++;

  if ( mesh->nti > mesh->nt ) {
    fprintf(stdout,"  ## Error: unable to get triangle.\n");
    fprintf(stdout,"    The number of call of MMG3D_Get_triangle function");
    fprintf(stdout," can not exceed the number of triangles: %d\n ",mesh->nt);
    return(0);
  }

  ptt = &mesh->tria[mesh->nti];
  *v0  = ptt->v[0];
  *v1  = ptt->v[1];
  *v2  = ptt->v[2];
  if ( ref != NULL )
    *ref = ptt->ref;

  if ( isRequired != NULL ) {
    if ( (ptt->tag[0] & MG_REQ) && (ptt->tag[1] & MG_REQ) &&
         (ptt->tag[2] & MG_REQ) )
      *isRequired = 1;
    else
      *isRequired = 0;
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 first extremity of the edge.
 * \param v1 second extremity of the edge.
 * \param ref edge reference.
 * \param pos edge position in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set edge of extremities \a v0, \a v1 and reference \a ref at
 * position \a pos in mesh structure
 *
 */
int MMG3D_Set_edge(MMG5_pMesh mesh, int v0, int v1, int ref, int pos) {

  if ( !mesh->na ) {
    fprintf(stdout,"  ## Error: You must set the number of edges with the");
    fprintf(stdout," MMG3D_Set_meshSize function before setting edges in mesh\n");
    return(0);
  }
  if ( pos > mesh->namax ) {
    fprintf(stdout,"  ## Error: unable to allocate a new edge.\n");
    fprintf(stdout,"    max number of edge: %d\n",mesh->namax);
    _MMG5_INCREASE_MEM_MESSAGE();
    return(0);
  }
  if ( pos > mesh->na ) {
    fprintf(stdout,"  ## Error: attempt to set new edge at position %d.",pos);
    fprintf(stdout," Overflow of the given number of edges: %d\n",mesh->na);
    fprintf(stdout,"  ## Check the mesh size, its compactness or the position");
    fprintf(stdout," of the edge.\n");
    return(0);
  }

  mesh->edge[pos].a = v0;
  mesh->edge[pos].b = v1;
  mesh->edge[pos].ref  = ref;
  mesh->edge[pos].tag |= MG_REF;

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param e0 pointer toward the first extremity of the edge.
 * \param e1 pointer toward the second  extremity of the edge.
 * \param ref pointer toward the edge reference.
 * \param isRidge pointer toward the flag saying if the edge is ridge.
 * \param isRequired pointer toward the flag saying if the edge is required.
 * \return 0 if failed, 1 otherwise.
 *
 * Get extremities \a e0, \a e1 and reference \a ref of next edge of mesh.
 *
 */
int MMG3D_Get_edge(MMG5_pMesh mesh, int* e0, int* e1, int* ref
                  ,int* isRidge, int* isRequired) {

  mesh->nai++;

  if ( mesh->nai > mesh->na ) {
    fprintf(stdout,"  ## Error: unable to get edge.\n");
    fprintf(stdout,"    The number of call of MMG3D_Get_edge function");
    fprintf(stdout," can not exceed the number of edges: %d\n ",mesh->na);
    return(0);
  }

  *e0  = mesh->edge[mesh->nai].a;
  *e1  = mesh->edge[mesh->nai].b;
  if ( ref!=NULL )
    *ref = mesh->edge[mesh->nai].ref;

  if ( isRidge != NULL ) {
    if ( mesh->edge[mesh->nai].tag & MG_GEO )
      *isRidge = 1;
    else
      *isRidge = 0;
  }

  if ( isRequired != NULL ) {
    if ( mesh->edge[mesh->nai].tag & MG_REQ )
      *isRequired = 1;
    else
      *isRequired = 0;
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k vertex index.
 * \return 1.
 *
 * Set corner at point \a k.
 *
 */
int MMG3D_Set_corner(MMG5_pMesh mesh, int k) {
  assert ( k <= mesh->np );
  mesh->point[k].tag |= MG_CRN;
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k vertex index.
 * \return 1.
 *
 * Set point \a k as required.
 *
 */
int MMG3D_Set_requiredVertex(MMG5_pMesh mesh, int k) {
  assert ( k <= mesh->np );
  mesh->point[k].tag |= MG_REQ;
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k element index.
 * \return 1.
 *
 * Set element \a k as required.
 *
 */
int MMG3D_Set_requiredTetrahedron(MMG5_pMesh mesh, int k) {
  assert ( k <= mesh->ne );
  mesh->tetra[k].tag |= MG_REQ;
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k triangle index.
 * \return 1.
 *
 * Set triangle \a k as required.
 *
 */
int MMG3D_Set_requiredTriangle(MMG5_pMesh mesh, int k) {
  assert ( k <= mesh->nt );
  mesh->tria[k].tag[0] |= MG_REQ;
  mesh->tria[k].tag[1] |= MG_REQ;
  mesh->tria[k].tag[2] |= MG_REQ;
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k edge index.
 * \return 1.
 *
 * Set ridge at edge \a k.
 *
 */
int MMG3D_Set_ridge(MMG5_pMesh mesh, int k) {
  assert ( k <= mesh->na );
  mesh->edge[k].tag |= MG_GEO;
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k edge index.
 * \return 1.
 *
 * Set edge \a k as required.
 *
 */
int MMG3D_Set_requiredEdge(MMG5_pMesh mesh, int k) {
  assert ( k <= mesh->na );
  mesh->edge[k].tag |= MG_REQ;
  return(1);
}

/**
 * \param met pointer toward the sol structure.
 * \param s solution scalar value.
 * \param pos position of the solution in the mesh (begin to 1).
 * \return 0 if failed, 1 otherwise.
 *
 * Set scalar value \a s at position \a pos in solution structure
 *
 */
int MMG3D_Set_scalarSol(MMG5_pSol met, double s, int pos) {

  if ( !met->np ) {
    fprintf(stdout,"  ## Error: You must set the number of solution with the");
    fprintf(stdout," MMG3D_Set_solSize function before setting values");
    fprintf(stdout," in solution structure \n");
    return(0);
  }
  if ( pos < 1 ) {
    fprintf(stdout,"  ## Error: unable to set a new solution.\n");
    fprintf(stdout,"    Minimal index of the solution position must be 1.\n");
    return(0);
  }
  if ( pos >= met->npmax ) {
    fprintf(stdout,"  ## Error: unable to set a new solution.\n");
    fprintf(stdout,"    max number of solutions: %d\n",met->npmax);
    return(0);
  }

  if ( pos > met->np ) {
    fprintf(stdout,"  ## Error: attempt to set new solution at position %d.",pos);
    fprintf(stdout," Overflow of the given number of solutions: %d\n",met->np);
    fprintf(stdout,"  ## Check the solution size, its compactness or the position");
    fprintf(stdout," of the solution.\n");
    return(0);
  }

  met->m[pos] = s;
  return(1);
}

/**
 * \param met pointer toward the sol structure.
 * \param s pointer toward the scalar solution value.
 * \return 0 if failed, 1 otherwise.
 *
 * Get solution \a s of next vertex of mesh.
 *
 */
int MMG3D_Get_scalarSol(MMG5_pSol met, double* s) {

  met->npi++;

  if ( met->npi > met->np ) {
    fprintf(stdout,"  ## Error: unable to get solution.\n");
    fprintf(stdout,"     The number of call of MMG3D_Get_scalarSol function");
    fprintf(stdout," can not exceed the number of points: %d\n ",met->np);
    return(0);
  }

  *s  = met->m[met->npi];

  return(1);
}

/**
 * \param met pointer toward the sol structure.
 * \param vx x value of the vectorial solution.
 * \param vy y value of the vectorial solution.
 * \param vz z value of the vectorial solution.
 * \param pos position of the solution in the mesh (begin to 1).
 * \return 0 if failed, 1 otherwise.
 *
 * Set vectorial value \f$(v_x,v_y,v_z)\f$ at position \a pos in solution
 * structure.
 *
 */
int MMG3D_Set_vectorSol(MMG5_pSol met, double vx,double vy, double vz, int pos) {

  if ( !met->np ) {
    fprintf(stdout,"  ## Error: You must set the number of solution with the");
    fprintf(stdout," MMG3D_Set_solSize function before setting values");
    fprintf(stdout," in solution structure \n");
    return(0);
  }
  if ( pos < 1 ) {
    fprintf(stdout,"  ## Error: unable to set a new solution.\n");
    fprintf(stdout,"    Minimal index of the solution position must be 1.\n");
    return(0);
  }
  if ( pos >= met->npmax ) {
    fprintf(stdout,"  ## Error: unable to set a new solution.\n");
    fprintf(stdout,"    max number of solutions: %d\n",met->npmax);
    return(0);
  }

  if ( pos > met->np ) {
    fprintf(stdout,"  ## Error: attempt to set new solution at position %d.",pos);
    fprintf(stdout," Overflow of the given number of solutions: %d\n",met->np);
    fprintf(stdout,"  ## Check the solution size, its compactness or the position");
    fprintf(stdout," of the solution.\n");
    return(0);
  }

  met->m[3*pos]   = vx;
  met->m[3*pos+1] = vy;
  met->m[3*pos+2] = vz;

  return(1);
}

/**
 * \param met pointer toward the sol structure.
 * \param vx x value of the vectorial solution.
 * \param vy y value of the vectorial solution.
 * \param vz z value of the vectorial solution.
 * \return 0 if failed, 1 otherwise.
 *
 * Get vectorial solution \f$(v_x,v_y,vz)\f$ of next vertex of mesh.
 *
 */
int MMG3D_Get_vectorSol(MMG5_pSol met, double* vx, double* vy, double* vz) {

  met->npi++;

  if ( met->npi > met->np ) {
    fprintf(stdout,"  ## Error: unable to get solution.\n");
    fprintf(stdout,"     The number of call of MMG3D_Get_scalarSol function");
    fprintf(stdout," can not exceed the number of points: %d\n ",met->np);
    return(0);
  }

  *vx  = met->m[3*met->npi];
  *vy  = met->m[3*met->npi+1];
  *vz  = met->m[3*met->npi+2];

  return(1);
}
/**
 * \param met pointer toward the sol structure.
 * \param m11 value of the tensorial solution at position (1,1) in the tensor.
 * \param m12 value of the tensorial solution at position (1,2) in the tensor.
 * \param m13 value of the tensorial solution at position (1,3) in the tensor.
 * \param m22 value of the tensorial solution at position (2,2) in the tensor.
 * \param m23 value of the tensorial solution at position (2,3) in the tensor.
 * \param m33 value of the tensorial solution at position (3,3) in the tensor.
 * \param pos position of the solution in the mesh (begin to 1).
 * \return 0 if failed, 1 otherwise.
 *
 * Set tensorial values at position \a pos in solution
 * structure.
 *
 */
int MMG3D_Set_tensorSol(MMG5_pSol met, double m11,double m12, double m13,
                       double m22,double m23, double m33, int pos) {

  if ( !met->np ) {
    fprintf(stdout,"  ## Error: You must set the number of solution with the");
    fprintf(stdout," MMG3D_Set_solSize function before setting values");
    fprintf(stdout," in solution structure \n");
    return(0);
  }
  if ( pos < 1 ) {
    fprintf(stdout,"  ## Error: unable to set a new solution.\n");
    fprintf(stdout,"    Minimal index of the solution position must be 1.\n");
    return(0);
  }
  if ( pos >= met->npmax ) {
    fprintf(stdout,"  ## Error: unable to set a new solution.\n");
    fprintf(stdout,"    max number of solutions: %d\n",met->npmax);
    return(0);
  }

  if ( pos > met->np ) {
    fprintf(stdout,"  ## Error: attempt to set new solution at position %d.",pos);
    fprintf(stdout," Overflow of the given number of solutions: %d\n",met->np);
    fprintf(stdout,"  ## Check the solution size, its compactness or the position");
    fprintf(stdout," of the solution.\n");
    return(0);
  }

  met->m[6*pos]   = m11;
  met->m[6*pos+1] = m12;
  met->m[6*pos+2] = m13;
  met->m[6*pos+3] = m22;
  met->m[6*pos+4] = m23;
  met->m[6*pos+5] = m33;

  return(1);
}

/**
 * \param met pointer toward the sol structure.
 * \param m11 pointer toward the position (1,1) in the solution tensor.
 * \param m12 pointer toward the position (1,2) in the solution tensor.
 * \param m13 pointer toward the position (1,3) in the solution tensor.
 * \param m22 pointer toward the position (2,2) in the solution tensor.
 * \param m23 pointer toward the position (2,3) in the solution tensor.
 * \param m33 pointer toward the position (3,3) in the solution tensor.
 * \return 0 if failed, 1 otherwise.
 *
 * Get tensorial solution of next vertex of mesh.
 *
 */
int MMG3D_Get_tensorSol(MMG5_pSol met, double *m11,double *m12, double *m13,
                       double *m22,double *m23, double *m33) {

  met->npi++;

  if ( met->npi > met->np ) {
    fprintf(stdout,"  ## Error: unable to get solution.\n");
    fprintf(stdout,"     The number of call of MMG3D_Get_scalarSol function");
    fprintf(stdout," can not exceed the number of points: %d\n ",met->np);
    return(0);
  }

  *m11 = met->m[6*met->npi];
  *m12 = met->m[6*met->npi+1];
  *m13 = met->m[6*met->npi+2];
  *m22 = met->m[6*met->npi+3];
  *m13 = met->m[6*met->npi+4];
  *m33 = met->m[6*met->npi+5];

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 *
 * To mark as ended a mesh given without using the API functions
 * (for example, mesh given by mesh->point[i] = 0 ...). Not recommanded.
 *
 */
void MMG3D_Set_handGivenMesh(MMG5_pMesh mesh) {
  int k, aux;

  /* Possibly switch 2 vertices number so that each tet is positively oriented */
  for (k=1; k<=mesh->ne; k++) {
    if ( _MMG5_orvol(mesh->point,mesh->tetra[k].v) < 0.0 ) {
      /* mesh->xt temporary used to count reoriented tetra */
      mesh->xt++;
      aux = mesh->tetra[k].v[2];
      mesh->tetra[k].v[2] = mesh->tetra[k].v[3];
      mesh->tetra[k].v[3] = aux;
    }
  }
  return;
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward the sol structure.
 * \return 0 if failed, 1 otherwise.
 *
 * Check if the number of given entities match with mesh and sol size
 * (not mandatory) and check mesh datas.
 *
 */
int MMG3D_Chk_meshData(MMG5_pMesh mesh,MMG5_pSol met) {

  if ( (mesh->npi != mesh->np) || (mesh->nei != mesh->ne) ) {
    fprintf(stdout,"  ## Error: if you don't use the MMG3D_loadMesh function,");
    fprintf(stdout," you must call the MMG3D_Set_meshSize function to have a");
    fprintf(stdout," valid mesh.\n");
    fprintf(stdout," Missing datas.\n");
    return(0);
  }

  if ( met->npi != met->np ) {
    fprintf(stdout,"  ## Error: if you don't use the MMG3D_loadSol function,");
    fprintf(stdout," you must call the MMG3D_Set_solSize function to have a");
    fprintf(stdout," valid solution.\n");
    fprintf(stdout," Missing datas.\n");
    return(0);
  }

  /*  Check mesh data */
  if ( mesh->info.ddebug ) {
    if ( (!mesh->np) || (!mesh->point) ||
         (!mesh->ne) || (!mesh->tetra) ) {
      fprintf(stdout,"  ** MISSING DATA.\n");
      fprintf(stdout," Check that your mesh contains points and tetrahedra.\n");
      fprintf(stdout," Exit program.\n");
      return(0);
    }
  }

  if ( mesh->dim != 3 ) {
    fprintf(stdout,"  ** 3 DIMENSIONAL MESH NEEDED. Exit program.\n");
    return(0);
  }
  if ( met->dim != 3 ) {
    fprintf(stdout,"  ** WRONG DIMENSION FOR METRIC. Exit program.\n");
    return(0);
  }
  if ( !mesh->ver )  mesh->ver = 2;
  if ( !met ->ver )  met ->ver = 2;

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \return 0 if failed, 1 otherwise.
 *
 * Skip the \a MG_ISO references in an input mesh.
 *
 */
static inline
int _MMG3D_skipIso(MMG5_pMesh mesh) {
  MMG5_pTria  ptt,ptt1;
  MMG5_pEdge  pa,pa1;
  int    k;

  if ( (mesh->info.imprim > 5) || mesh->info.ddebug )
    fprintf(stdout,"  ## Warning: skip of all entites with %d reference.\n",MG_ISO);

  /* Skip triangles with MG_ISO refs */
  k = 1;
  do {
    ptt = &mesh->tria[k];
    if ( abs(ptt->ref) != MG_ISO ) continue;
    /* here ptt is the first tri of mesh->tria that we want to delete */
    do {
      ptt1 = &mesh->tria[mesh->nti];
    }
    while( (abs(ptt1->ref) == MG_ISO) && (k <= --mesh->nti) );

    if ( abs(ptt1->ref) != MG_ISO )
      /* ptt1 is the last tri of mesh->tria that we want to keep */
      memcpy(ptt,ptt1,sizeof(MMG5_Tria));
  } while( ++k <= mesh->nti );

  if ( mesh->nti < mesh->nt ) {
    if( !mesh->nti )
      _MMG5_DEL_MEM(mesh,mesh->tria,(mesh->nt+1)*sizeof(MMG5_Tria));
    else {
      _MMG5_ADD_MEM(mesh,mesh->nti-mesh->nt,"triangles",return(0));
      _MMG5_SAFE_RECALLOC(mesh->tria,mesh->nt+1,(mesh->nti+1),MMG5_Tria,"triangles");
    }
    mesh->nt = mesh->nti;
  }

  /* Skip edges with MG_ISO refs */
  k = 1;
  do {
    pa = &mesh->edge[k];
    if ( abs(pa->ref) != MG_ISO ) {
      pa->ref = abs(pa->ref);
      continue;
    }
    /* here pa is the first edge of mesh->edge that we want to delete */
    do {
      pa1 = &mesh->edge[mesh->nai];
    }
    while( (abs(pa1->ref) == MG_ISO) && (k <= --mesh->nai) );

    if ( abs(pa1->ref) != MG_ISO ) {
      /* pa1 is the last edge of mesh->edge that we want to keep */
      memcpy(pa,pa1,sizeof(MMG5_Edge));
      pa1->ref = abs(pa1->ref);
    }
  } while( ++k <= mesh->nai );

  if ( mesh->nai < mesh->na ) {
    if( !mesh->nai )
      _MMG5_DEL_MEM(mesh,mesh->edge,(mesh->nai+1)*sizeof(MMG5_Edge));
    else {
      _MMG5_ADD_MEM(mesh,mesh->nai-mesh->na,"Edges",return(0));
      _MMG5_SAFE_RECALLOC(mesh->edge,mesh->na+1,(mesh->nai+1),MMG5_Edge,"edges");
    }
    mesh->na = mesh->nai;
  }

  /* delete tetrahedra references */
  for (k=1; k<=mesh->ne; k++) {
    mesh->tetra[k].ref = 0;
  }
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param iparam integer parameter to set (see \a MMG3D_Param structure).
 * \param val value for the parameter.
 * \return 0 if failed, 1 otherwise.
 *
 * Set integer parameter \a iparam at value \a val.
 *
 */
int MMG3D_Set_iparameter(MMG5_pMesh mesh, MMG5_pSol sol, int iparam, int val){
  int k;

  switch ( iparam ) {
    /* Integer parameters */
  case MMG3D_IPARAM_verbose :
    mesh->info.imprim   = val;
    break;
  case MMG3D_IPARAM_mem :
    if ( val <= 0 ) {
      fprintf(stdout,"  ## Warning: maximal memory authorized must be strictly positive.\n");
      fprintf(stdout,"  Reset to default value.\n");
    }
    else
      mesh->info.mem      = val;
    _MMG3D_memOption(mesh);
    if(mesh->np && (mesh->npmax < mesh->np || mesh->ntmax < mesh->nt || mesh->nemax < mesh->ne)) {
      return(0);
    } else if(mesh->info.mem < 39)
      return(0);
    break;
#ifndef PATTERN
  case MMG3D_IPARAM_bucket :
    mesh->info.bucket   = val;
    break;
#endif
  case MMG3D_IPARAM_debug :
    mesh->info.ddebug   = val;
    break;
  case MMG3D_IPARAM_angle :
    /* free table that may contains old ridges */
    if ( mesh->htab.geom )
      _MMG5_DEL_MEM(mesh,mesh->htab.geom,(mesh->htab.max+1)*sizeof(MMG5_hgeom));
    if ( mesh->xpoint )
      _MMG5_DEL_MEM(mesh,mesh->xpoint,(mesh->xpmax+1)*sizeof(MMG5_xPoint));
    if ( mesh->xtetra )
      _MMG5_DEL_MEM(mesh,mesh->xtetra,(mesh->xtmax+1)*sizeof(MMG5_xTetra));
    if ( !val )
      mesh->info.dhd    = -1.;
    else {
      if ( (mesh->info.imprim > 5) || mesh->info.ddebug )
        fprintf(stdout,"  ## Warning: angle detection parameter set to default value\n");
      mesh->info.dhd    = _MMG5_ANGEDG;
    }
    break;
  case MMG3D_IPARAM_iso :
    mesh->info.iso      = val;
    if ( mesh->info.iso )
      if ( mesh->nt && !_MMG3D_skipIso(mesh) )
        exit(EXIT_FAILURE);
    break;
  case MMG3D_IPARAM_lag :
#ifdef USE_SUSCELAS
    if ( val < 0 || val > 2 )
      exit(EXIT_FAILURE);
    mesh->info.lag = val;
#else
    fprintf(stdout,"  ## Error:"
            " \"lagrangian motion\" option unavailable (-lag):\n"
            " set the USE_SUSCELAS CMake's flag to ON when compiling the mmg3d"
            " library to enable this feature.\n");
    return(0);
#endif
    break;
  case MMG3D_IPARAM_optim :
    mesh->info.optim = val;
    break;
  case MMG3D_IPARAM_noinsert :
    mesh->info.noinsert = val;
    break;
  case MMG3D_IPARAM_noswap :
    mesh->info.noswap   = val;
    break;
  case MMG3D_IPARAM_nomove :
    mesh->info.nomove   = val;
    break;
  case MMG3D_IPARAM_nosurf :
    mesh->info.nosurf   = val;
    break;
  case MMG3D_IPARAM_numberOfLocalParam :
    if ( mesh->info.par ) {
      _MMG5_DEL_MEM(mesh,mesh->info.par,mesh->info.npar*sizeof(MMG5_Par));
      if ( (mesh->info.imprim > 5) || mesh->info.ddebug )
        fprintf(stdout,"  ## Warning: new local parameter values\n");
    }
    mesh->info.npar  = val;
    mesh->info.npari = 0;
    _MMG5_ADD_MEM(mesh,mesh->info.npar*sizeof(MMG5_Par),"parameters",
                  printf("  Exit program.\n");
                  exit(EXIT_FAILURE));
    _MMG5_SAFE_CALLOC(mesh->info.par,mesh->info.npar,MMG5_Par);

    for (k=0; k<mesh->info.npar; k++) {
      mesh->info.par[k].elt   = MMG5_Noentity;
      mesh->info.par[k].ref   = INT_MAX;
      mesh->info.par[k].hausd = mesh->info.hausd;
    }

    break;
#ifdef USE_SCOTCH
  case MMG3D_IPARAM_renum :
    mesh->info.renum    = val;
    break;
#endif
  default :
    fprintf(stdout,"  ## Error: unknown type of parameter\n");
    return(0);
  }
  /* other options */
  mesh->info.fem      = 0;
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param iparam integer parameter to set (see \a MMG3D_Param structure).
 * \return The value of integer parameter.
 *
 * Get the value of integer parameter \a iparam.
 *
 */
int MMG3D_Get_iparameter(MMG5_pMesh mesh, int iparam) {

  switch ( iparam ) {
    /* Integer parameters */
  case MMG3D_IPARAM_verbose :
    return ( mesh->info.imprim );
    break;
  case MMG3D_IPARAM_mem :
    return ( mesh->info.mem );
    break;
#ifndef PATTERN
  case MMG3D_IPARAM_bucket :
    return ( mesh->info.bucket );
    break;
#endif
  case MMG3D_IPARAM_debug :
    return ( mesh->info.ddebug );
    break;
  case MMG3D_IPARAM_angle :
    return ( mesh->info.dhd );
    break;
  case MMG3D_IPARAM_iso :
    return ( mesh->info.iso );
    break;
  case MMG3D_IPARAM_lag :
    return ( mesh->info.lag );
    break;
  case MMG3D_IPARAM_noinsert :
    return ( mesh->info.noinsert );
    break;
  case MMG3D_IPARAM_noswap :
    return ( mesh->info.noswap );
    break;
  case MMG3D_IPARAM_nomove :
    return ( mesh->info.nomove );
    break;
  case MMG3D_IPARAM_nosurf :
    return ( mesh->info.nosurf );
    break;
  case MMG3D_IPARAM_numberOfLocalParam :
    return ( mesh->info.npar );
    break;
#ifdef USE_SCOTCH
  case MMG3D_IPARAM_renum :
    return ( mesh->info.renum );
    break;
#endif
  default :
    fprintf(stdout,"  ## Error: unknown type of parameter\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param dparam double parameter to set (see \a MMG3D_Param structure).
 * \param val value of the parameter.
 * \return 0 if failed, 1 otherwise.
 *
 * Set double parameter \a dparam at value \a val.
 *
 */
int MMG3D_Set_dparameter(MMG5_pMesh mesh, MMG5_pSol sol, int dparam, double val){

  switch ( dparam ) {
    /* double parameters */
  case MMG3D_DPARAM_angleDetection :
    mesh->info.dhd = val;
    mesh->info.dhd = MG_MAX(0.0, MG_MIN(180.0,mesh->info.dhd));
    mesh->info.dhd = cos(mesh->info.dhd*M_PI/180.0);
    break;
  case MMG3D_DPARAM_hmin :
    mesh->info.hmin     = val;
    break;
  case MMG3D_DPARAM_hmax :
    mesh->info.hmax     = val;
    break;
  case MMG3D_DPARAM_hgrad :
    mesh->info.hgrad    = val;
    if ( mesh->info.hgrad < 0.0 )
      mesh->info.hgrad = -1.0;
    else
      mesh->info.hgrad = log(mesh->info.hgrad);
    break;
  case MMG3D_DPARAM_hausd :
    if ( val <=0 ) {
      fprintf(stdout,"  ## Error: hausdorff number must be strictly positive.\n");
      return(0);
    }
    else
      mesh->info.hausd    = val;
    break;
  case MMG3D_DPARAM_ls :
    mesh->info.ls       = val;
    break;
  default :
    fprintf(stdout,"  ## Error: unknown type of parameter\n");
    return(0);
  }
  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param typ type of entity (triangle, edge,...).
 * \param ref reference of the entity.
 * \param val value of the Hausdorff number.
 * \return 0 if failed, 1 otherwise.
 *
 * Set local parameters: set the hausdorff value at \a val for all
 * elements of type \a typ and reference \a ref.
 *
 */
int MMG3D_Set_localParameter(MMG5_pMesh mesh,MMG5_pSol sol, int typ, int ref, double val){
  int k;

  if ( !mesh->info.npar ) {
    fprintf(stdout,"  ## Error: You must set the number of local parameters");
    fprintf(stdout," with the MMG3D_Set_iparameters function before setting");
    fprintf(stdout," values in local parameters structure. \n");
    return(0);
  }
  if ( mesh->info.npari > mesh->info.npar ) {
    fprintf(stdout,"  ## Error: unable to set a new local parameter.\n");
    fprintf(stdout,"    max number of local parameters: %d\n",mesh->info.npar);
    return(0);
  }

  switch ( typ ) {
    /* double parameters */
  case MMG5_Triangle :
    for (k=0; k<mesh->info.npari; k++) {
      if ( mesh->info.par[k].ref == ref ) {
        mesh->info.par[k].hausd = val;
        if ( (mesh->info.imprim > 5) || mesh->info.ddebug ) {
          fprintf(stdout,"  ## Warning: new hausdorff value for triangles");
          fprintf(stdout," of ref %d\n",ref);
        }
        return(1);
      }
    }
    if ( mesh->info.npari == mesh->info.npar ) {
      fprintf(stdout,"  ## Error: unable to set a new local parameter.\n");
      fprintf(stdout,"    max number of local parameters: %d\n",mesh->info.npar);
      return(0);
    }
    mesh->info.par[mesh->info.npari].elt   = typ;
    mesh->info.par[mesh->info.npari].ref   = ref;
    mesh->info.par[mesh->info.npari].hausd = val;
    mesh->info.npari++;
    break;
  default :
    fprintf(stdout,"  ## Warning: you must apply local hausdorff number");
    fprintf(stdout," on triangles (MMG5_Triangle or %d).\n",MMG5_Triangle);
    fprintf(stdout,"  ## Ignored.\n");
    return(1);
  }

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward a sol structure (metric or solution).
 * \param disp pointer toward a sol structure (displacement).
 *
 * Structure deallocations before return.
 *
 */
void MMG3D_Free_structures(MMG5_pMesh mesh,MMG5_pSol met,MMG5_pSol disp
  ){

  MMG3D_Free_names(mesh,met,disp);

  /* mesh */
  if ( mesh->point )
    _MMG5_DEL_MEM(mesh,mesh->point,(mesh->npmax+1)*sizeof(MMG5_Point));

  if ( mesh->tetra )
    _MMG5_DEL_MEM(mesh,mesh->tetra,(mesh->nemax+1)*sizeof(MMG5_Tetra));

  if ( mesh->edge )
    _MMG5_DEL_MEM(mesh,mesh->edge,(mesh->na+1)*sizeof(MMG5_Edge));

  if ( mesh->adja )
    _MMG5_DEL_MEM(mesh,mesh->adja,(4*mesh->nemax+5)*sizeof(int));

  if ( mesh->xpoint )
    _MMG5_DEL_MEM(mesh,mesh->xpoint,(mesh->xpmax+1)*sizeof(MMG5_xPoint));

  if ( mesh->htab.geom )
    _MMG5_DEL_MEM(mesh,mesh->htab.geom,(mesh->htab.max+1)*sizeof(MMG5_hgeom));

  if ( mesh->tria )
    _MMG5_DEL_MEM(mesh,mesh->tria,(mesh->nt+1)*sizeof(MMG5_Tria));

  if ( mesh->xtetra )
    _MMG5_DEL_MEM(mesh,mesh->xtetra,(mesh->xtmax+1)*sizeof(MMG5_xTetra));

  /* met */
  if ( met && met->m )
    _MMG5_DEL_MEM(mesh,met->m,(met->size*(met->npmax+1))*sizeof(double));

  /* disp */
  if ( disp && disp->m )
    _MMG5_DEL_MEM(mesh,disp->m,(disp->size*(disp->npmax+1))*sizeof(double));

  /* mesh->info */
  if ( mesh->info.npar && mesh->info.par )
    _MMG5_DEL_MEM(mesh,mesh->info.par,mesh->info.npar*sizeof(MMG5_Par));

  if ( mesh->info.imprim>6 || mesh->info.ddebug )
    printf("  MEMORY USED AT END (bytes) %ld\n",_MMG5_safeLL2LCast(mesh->memCur));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward a sol structure (metric or solution).
 * \param disp pointer toward a sol structure (displacement).
 *
 * Structure deallocations before return.
 *
 */
void MMG3D_Free_names(MMG5_pMesh mesh,MMG5_pSol met,MMG5_pSol disp){

  /* mesh & met */
  MMG5_mmgFree_names(mesh,met);

  /* disp */
  if ( disp ) {
    if ( disp->namein ) {
      _MMG5_DEL_MEM(mesh,disp->namein,(strlen(disp->namein)+1)*sizeof(char));
    }

    if ( disp->nameout ) {
      _MMG5_DEL_MEM(mesh,disp->nameout,(strlen(disp->nameout)+1)*sizeof(char));
    }
  }
}

/** Old API °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°*/

/**
 * \param mesh adress of a pointer toward a pointer toward the mesh structure.
 * \param sol adress of a pointer toward a sol structure (metric or level-set).
 * \param disp adress of a pointer toward a sol structure (displacement for
 * the lagrangian mode).
 *
 * Allocate the mesh and solution structures and initialize it to
 * their default values.
 *
 */
void MMG5_Init_mesh(MMG5_pMesh *mesh, MMG5_pSol *sol, MMG5_pSol *disp ) {
  printf("  ## MMG5_Init_mesh: MMG5_ API is deprecated (replaced by the"
         " MMG3D_ one) and will be removed soon\n." );

  MMG3D_Init_mesh(mesh,sol,disp);

  return;
}

/**
 * \param mesh pointer toward the mesh structure.
 *
 * Initialization of the input parameters (stored in the Info structure).
 *
 */
void MMG5_Init_parameters(MMG5_pMesh mesh) {
  printf("  ##  MMG5_Init_parameters:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  MMG3D_Init_parameters(mesh);
  return;
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param typEntity type of solutions entities (vertices, triangles...).
 * \param np number of solutions.
 * \param typSol type of solution (scalar, vectorial...).
 * \return 0 if failed, 1 otherwise.
 *
 * Set the solution number, dimension and type.
 *
 */
int MMG5_Set_solSize(MMG5_pMesh mesh, MMG5_pSol sol, int typEntity, int np, int typSol) {
  printf("  ## MMG5_Set_solSize:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_solSize(mesh,sol,typEntity,np,typSol));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param np number of vertices.
 * \param ne number of elements (tetrahedra).
 * \param nt number of triangles.
 * \param na number of edges.
 * \return 0 if failed, 1 otherwise.
 *
 * Set the number of vertices, tetrahedra, triangles and edges of the
 * mesh and allocate the associated tables. If call twice, reset the
 * whole mesh to realloc it at the new size
 *
 */
int MMG5_Set_meshSize(MMG5_pMesh mesh, int np, int ne, int nt, int na) {
  printf("  ## MMG5_Set_meshSize:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_meshSize(mesh,np,ne,nt,na));
}


/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param typEntity pointer toward the type of entities to which solutions are applied.
 * \param np pointer toward the number of solutions.
 * \param typSol pointer toward the type of the solutions (scalar, vectorial...)
 * \return 1.
 *
 * Get the solution number, dimension and type.
 *
 */
int MMG5_Get_solSize(MMG5_pMesh mesh, MMG5_pSol sol, int* typEntity, int* np, int* typSol) {
  printf("  ## MMG5_Get_solSize:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_solSize(mesh,sol,typEntity,np,typSol));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param np pointer toward the number of vertices.
 * \param ne pointer toward the number of elements (tetrahedra).
 * \param nt pointer toward the number of triangles.
 * \param na pointer toward the number of edges.
 * \return 1.
 *
 * Get the number of vertices, tetrahedra, triangles and edges of the mesh.
 *
 */
int MMG5_Get_meshSize(MMG5_pMesh mesh, int* np, int* ne, int* nt, int* na) {
  printf("  ## MMG5_Get_meshSize:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_meshSize(mesh,np,ne,nt,na));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param c0 coordinate of the point along the first dimension.
 * \param c1 coordinate of the point along the second dimension.
 * \param c2 coordinate of the point along the third dimension.
 * \param ref point reference.
 * \param pos position of the point in the mesh.
 * \return 1.
 *
 * Set vertex of coordinates \a c0, \a c1,\a c2 and reference \a ref
 * at position \a pos in mesh structure
 *
 */
int MMG5_Set_vertex(MMG5_pMesh mesh, double c0, double c1, double c2, int ref, int pos) {
  printf("  ## MMG5_Set_vertex:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_vertex(mesh,c0,c1,c2,ref,pos));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param c0 pointer toward the coordinate of the point along the first dimension.
 * \param c1 pointer toward the coordinate of the point along the second dimension.
 * \param c2 pointer toward the coordinate of the point along the third dimension.
 * \param ref poiter to the point reference.
 * \param isCorner pointer toward the flag saying if point is corner.
 * \param isRequired pointer toward the flag saying if point is required.
 * \return 1.
 *
 * Get coordinates \a c0, \a c1,\a c2 and reference \a ref of next
 * vertex of mesh.
 *
 */
int MMG5_Get_vertex(MMG5_pMesh mesh, double* c0, double* c1, double* c2, int* ref,
                    int* isCorner, int* isRequired) {
  printf("  ## MMG5_Get_vertex:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_vertex(mesh,c0,c1,c2,ref,isCorner,isRequired));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 first vertex of tetrahedron.
 * \param v1 second vertex of tetrahedron.
 * \param v2 third vertex of tetrahedron.
 * \param v3 fourth vertex of tetrahedron.
 * \param ref tetrahedron reference (must be positive).
 * \param pos tetrahedron position in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set tetrahedra of vertices \a v0, \a v1,\a v2,\a v3 and reference
 * \a ref at position \a pos in mesh structure.
 *
 */
int MMG5_Set_tetrahedron(MMG5_pMesh mesh, int v0, int v1, int v2, int v3, int ref, int pos) {
  printf("  ## MMG5_Set_tetrahedron:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_tetrahedron(mesh,v0,v1,v2,v3,ref,pos));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 pointer toward the first vertex of tetrahedron.
 * \param v1 pointer toward the second vertex of tetrahedron.
 * \param v2 pointer toward the third vertex of tetrahedron.
 * \param v3 pointer toward the fourth vertex of tetrahedron.
 * \param ref pointer toward the tetrahedron reference.
 * \param isRequired pointer toward the flag saying if tetrahedron is required.
 * \return 0 if failed, 1 otherwise.
 *
 * Get vertices \a v0, \a v1, \a v2, \a v3 and reference \a ref of
 * next tetra of mesh.
 *
 */
int MMG5_Get_tetrahedron(MMG5_pMesh mesh, int* v0, int* v1, int* v2, int* v3,
                         int* ref, int* isRequired) {
  printf("  ## MMG5_Get_tetrahedron:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_tetrahedron(mesh,v0,v1,v2,v3,ref,isRequired));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 first vertex of triangle.
 * \param v1 second vertex of triangle.
 * \param v2 third vertex of triangle.
 * \param ref triangle reference.
 * \param pos triangle position in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set triangle of vertices \a v0, \a v1, \a v2 and reference \a ref
 * at position \a pos in mesh structure.
 *
 */
int MMG5_Set_triangle(MMG5_pMesh mesh, int v0, int v1, int v2, int ref,int pos) {
  printf("  ## MMG5_Set_triangle:"
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_triangle(mesh,v0,v1,v2,ref,pos));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 pointer toward the first vertex of triangle.
 * \param v1 pointer toward the second vertex of triangle.
 * \param v2 pointer toward the third vertex of triangle.
 * \param ref pointer toward the triangle reference.
 * \param isRequired pointer toward the flag saying if triangle is required.
 * \return 0 if failed, 1 otherwise.
 *
 * Get vertices \a v0,\a v1,\a v2 and reference \a ref of next
 * triangle of mesh.
 *
 */
int MMG5_Get_triangle(MMG5_pMesh mesh, int* v0, int* v1, int* v2, int* ref
                      ,int* isRequired) {
  printf("  ## MMG5_Get_triangle: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_triangle(mesh,v0,v1,v2,ref,isRequired));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param v0 first extremity of the edge.
 * \param v1 second extremity of the edge.
 * \param ref edge reference.
 * \param pos edge position in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set edge of extremities \a v0, \a v1 and reference \a ref at
 * position \a pos in mesh structure
 *
 */
int MMG5_Set_edge(MMG5_pMesh mesh, int v0, int v1, int ref, int pos) {
  printf("  ## MMG5_Set_edge:"
         " MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_edge(mesh,v0,v1,ref,pos));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param e0 pointer toward the first extremity of the edge.
 * \param e1 pointer toward the second  extremity of the edge.
 * \param ref pointer toward the edge reference.
 * \param isRidge pointer toward the flag saying if the edge is ridge.
 * \param isRequired pointer toward the flag saying if the edge is required.
 * \return 0 if failed, 1 otherwise.
 *
 * Get extremities \a e0, \a e1 and reference \a ref of next edge of mesh.
 *
 */
int MMG5_Get_edge(MMG5_pMesh mesh, int* e0, int* e1, int* ref
                  ,int* isRidge, int* isRequired) {
  printf("  ## MMG5_Get_edge:"
         " MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_edge(mesh,e0,e1,ref,isRidge,isRequired));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k vertex index.
 * \return 1.
 *
 * Set corner at point \a k.
 *
 */
int MMG5_Set_corner(MMG5_pMesh mesh, int k) {
  printf("  ## MMG5_Set_corner: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_corner(mesh,k));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k vertex index.
 * \return 1.
 *
 * Set point \a k as required.
 *
 */
int MMG5_Set_requiredVertex(MMG5_pMesh mesh, int k) {
  printf("  ## MMG5_Set_requiredVertex:"
         " MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_requiredVertex(mesh,k));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k element index.
 * \return 1.
 *
 * Set element \a k as required.
 *
 */
int MMG5_Set_requiredTetrahedron(MMG5_pMesh mesh, int k) {
  printf("  ## MMG5_Set_requiredTetrahedron: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_requiredTetrahedron(mesh,k));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k triangle index.
 * \return 1.
 *
 * Set triangle \a k as required.
 *
 */
int MMG5_Set_requiredTriangle(MMG5_pMesh mesh, int k) {
  printf("  ## MMG5_Set_requiredTriangle: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_requiredTriangle(mesh,k));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k edge index.
 * \return 1.
 *
 * Set ridge at edge \a k.
 *
 */
int MMG5_Set_ridge(MMG5_pMesh mesh, int k) {
  printf("  ## MMG5_Set_ridge: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_ridge(mesh,k));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param k edge index.
 * \return 1.
 *
 * Set edge \a k as required.
 *
 */
int MMG5_Set_requiredEdge(MMG5_pMesh mesh, int k) {
  printf("  ## MMG5_Set_requiredEdge: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_requiredEdge(mesh,k));
}

/**
 * \param met pointer toward the sol structure.
 * \param s solution scalar value.
 * \param pos position of the solution in the mesh (begin to 1).
 * \return 0 if failed, 1 otherwise.
 *
 * Set scalar value \a s at position \a pos in solution structure
 *
 */
int MMG5_Set_scalarSol(MMG5_pSol met, double s, int pos) {
  printf("  ## MMG5_Set_scalarSol: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_scalarSol(met,s,pos));
}

/**
 * \param met pointer toward the sol structure.
 * \param s pointer toward the scalar solution value.
 * \return 0 if failed, 1 otherwise.
 *
 * Get solution \a s of next vertex of mesh.
 *
 */
int MMG5_Get_scalarSol(MMG5_pSol met, double* s) {
  printf("  ## MMG5_Get_scalarSol: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_scalarSol(met,s));
}

/**
 * \param met pointer toward the sol structure.
 * \param vx x value of the vectorial solution.
 * \param vy y value of the vectorial solution.
 * \param vz z value of the vectorial solution.
 * \param pos position of the solution in the mesh (begin to 1).
 * \return 0 if failed, 1 otherwise.
 *
 * Set vectorial value \f$(v_x,v_y,v_z)\f$ at position \a pos in solution
 * structure.
 *
 */
int MMG5_Set_vectorSol(MMG5_pSol met, double vx,double vy, double vz, int pos) {
  printf("  ## MMG5_Set_vectorSol: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_vectorSol(met,vx,vy,vz,pos));
}

/**
 * \param met pointer toward the sol structure.
 * \param vx x value of the vectorial solution.
 * \param vy y value of the vectorial solution.
 * \param vz z value of the vectorial solution.
 * \return 0 if failed, 1 otherwise.
 *
 * Get vectorial solution \f$(v_x,v_y,vz)\f$ of next vertex of mesh.
 *
 */
int MMG5_Get_vectorSol(MMG5_pSol met, double* vx, double* vy, double* vz) {
  printf("  ## MMG5_Get_vectorSol: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_vectorSol(met,vx,vy,vz));
}
/**
 * \param met pointer toward the sol structure.
 * \param m11 value of the tensorial solution at position (1,1) in the tensor.
 * \param m12 value of the tensorial solution at position (1,2) in the tensor.
 * \param m13 value of the tensorial solution at position (1,3) in the tensor.
 * \param m22 value of the tensorial solution at position (2,2) in the tensor.
 * \param m23 value of the tensorial solution at position (2,3) in the tensor.
 * \param m33 value of the tensorial solution at position (3,3) in the tensor.
 * \param pos position of the solution in the mesh (begin to 1).
 * \return 0 if failed, 1 otherwise.
 *
 * Set tensorial values at position \a pos in solution
 * structure.
 *
 */
int MMG5_Set_tensorSol(MMG5_pSol met, double m11,double m12, double m13,
                       double m22,double m23, double m33, int pos) {
  printf("  ## MMG5_Set_tensorSol: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_tensorSol(met,m11,m12,m13,m22,m23,m33,pos));
}

/**
 * \param met pointer toward the sol structure.
 * \param m11 pointer toward the position (1,1) in the solution tensor.
 * \param m12 pointer toward the position (1,2) in the solution tensor.
 * \param m13 pointer toward the position (1,3) in the solution tensor.
 * \param m22 pointer toward the position (2,2) in the solution tensor.
 * \param m23 pointer toward the position (2,3) in the solution tensor.
 * \param m33 pointer toward the position (3,3) in the solution tensor.
 * \return 0 if failed, 1 otherwise.
 *
 * Get tensorial solution of next vertex of mesh.
 *
 */
int MMG5_Get_tensorSol(MMG5_pSol met, double *m11,double *m12, double *m13,
                       double *m22,double *m23, double *m33) {
  printf("  ## MMG5_Get_tensorSol: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_tensorSol(met,m11,m12,m13,m22,m23,m33));
}

/**
 * \param mesh pointer toward the mesh structure.
 *
 * To mark as ended a mesh given without using the API functions
 * (for example, mesh given by mesh->point[i] = 0 ...). Not recommanded.
 *
 */
void MMG5_Set_handGivenMesh(MMG5_pMesh mesh) {
  printf("  ## MMG5_Set_handGivenMesh: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  MMG3D_Set_handGivenMesh(mesh);
  return;
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward the sol structure.
 * \return 0 if failed, 1 otherwise.
 *
 * Check if the number of given entities match with mesh and sol size
 * (not mandatory) and check mesh datas.
 *
 */
int MMG5_Chk_meshData(MMG5_pMesh mesh,MMG5_pSol met) {
  printf("  ## MMG5_Chk_meshData: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Chk_meshData(mesh,met));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param iparam integer parameter to set (see \a MMG5_Param structure).
 * \param val value for the parameter.
 * \return 0 if failed, 1 otherwise.
 *
 * Set integer parameter \a iparam at value \a val.
 *
 */
int MMG5_Set_iparameter(MMG5_pMesh mesh, MMG5_pSol sol, int iparam, int val){
  printf("  ## MMG5_Set_iparameter: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_iparameter(mesh,sol,iparam,val));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param iparam integer parameter to set (see \a MMG5_Param structure).
 * \return The value of integer parameter.
 *
 * Get the value of integer parameter \a iparam.
 *
 */
int MMG5_Get_iparameter(MMG5_pMesh mesh, int iparam) {
  printf("  ## MMG5_Get_iparameter: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Get_iparameter(mesh,iparam));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param dparam double parameter to set (see \a MMG5_Param structure).
 * \param val value of the parameter.
 * \return 0 if failed, 1 otherwise.
 *
 * Set double parameter \a dparam at value \a val.
 *
 */
int MMG5_Set_dparameter(MMG5_pMesh mesh, MMG5_pSol sol, int dparam, double val){
  printf("  ## MMG5_Set_dparameter: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_dparameter(mesh,sol,dparam,val));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param typ type of entity (triangle, edge,...).
 * \param ref reference of the entity.
 * \param val value of the Hausdorff number.
 * \return 0 if failed, 1 otherwise.
 *
 * Set local parameters: set the hausdorff value at \a val for all
 * elements of type \a typ and reference \a ref.
 *
 */
int MMG5_Set_localParameter(MMG5_pMesh mesh,MMG5_pSol sol, int typ, int ref, double val){
  printf("  ##  MMG5_Set_localParameter "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  return(MMG3D_Set_localParameter(mesh,sol,typ,ref,val));
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward a sol structure (metric or solution).
 * \param disp pointer toward a sol structure (displacement).
 *
 * Structure deallocations before return.
 *
 */
void MMG5_Free_structures(MMG5_pMesh mesh,MMG5_pSol met,MMG5_pSol disp
  ){
  printf("  ## MMG5_Free_structures: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  MMG3D_Free_structures(mesh,met,disp);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward a sol structure (metric or solution).
 * \param disp pointer toward a sol structure (displacement).
 *
 * Structure deallocations before return.
 *
 */
void MMG5_Free_names(MMG5_pMesh mesh,MMG5_pSol met,MMG5_pSol disp){
  printf("  ## MMG5_Free_names: "
         "MMG5_ API is deprecated (replaced by the MMG3D_ one) and will"
        " be removed soon\n." );

  MMG3D_Free_names(mesh,met,disp);

}
