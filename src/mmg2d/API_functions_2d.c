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
 * \file mmg2d/API_functions_2d.c
 * \brief C API functions definitions for MMG2D library.
 * \author Algiane Froehly (Bx INP/Inria/UBordeaux)
 * \version 5
 * \date 01 2014
 * \copyright GNU Lesser General Public License.
 *
 * \note This file contains some internal functions for the API, see
 * the \ref mmg2d/libmmg2d.h header file for the documentation of all
 * the usefull user's API functions.
 *
 * C API for MMG2D library.
 *
 */


#include "mmg2d.h"

/**
 * \param starter dummy argument used to initialize the variadic argument list
 * \param ... variadic arguments. For now, you need to call the \a
 * MMG2D_Init_mesh function with the following arguments :
 * MMG2D_Init_mesh(MMG5_ARG_start,MMG5_ARG_ppMesh, your_mesh,
 * MMG5_ARG_ppMet, your_metric,MMG5_ARG_end). Here, \a your_mesh is a pointer
 * toward \a MMG5_pMesh and \a your_metric a pointer toward \a MMG5_pSol.
 *
 * MMG structures allocation and initialization.
 *
 */
void MMG2D_Init_mesh(enum MMG5_arg starter,...) {
  va_list argptr;

  va_start(argptr, starter);

  _MMG2D_Init_mesh_var(argptr);

  va_end(argptr);

  return;
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 *
 * Initialize file names to their default values.
 *
 */
void MMG2D_Init_fileNames(MMG5_pMesh mesh,MMG5_pSol sol
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
int MMG2D_Set_inputMeshName(MMG5_pMesh mesh, char* meshin) {

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
int MMG2D_Set_inputSolName(MMG5_pMesh mesh,MMG5_pSol sol, char* solin) {
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
int MMG2D_Set_outputMeshName(MMG5_pMesh mesh, char* meshout) {

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
int MMG2D_Set_outputSolName(MMG5_pMesh mesh,MMG5_pSol sol, char* solout) {
  return(MMG5_Set_outputSolName(mesh,sol,solout));
}
/**
 * \param mesh pointer toward the mesh structure.
 *
 * Initialization of the input parameters (stored in the Info structure).
 *
 */
void MMG2D_Init_parameters(MMG5_pMesh mesh) {

  /* Init common parameters for mmg2d, mmgs and mmg2d. */
  _MMG5_Init_parameters(mesh);

 /* default values for integers */
  /** MMG2D_IPARAM_iso = 0 */
  mesh->info.iso      =  0;  /* [0/1]    ,Turn on/off levelset meshing */
  /** MMG2D_IPARAM_lag = -1 */
  mesh->info.lag      = -1;
  /** MMG2D_IPARAM_optim = 0 */
  mesh->info.optim    =  0;
  /** MMG2D_IPARAM_nosurf = 0 */
  mesh->info.nosurf   =  0;  /* [0/1]    ,avoid/allow surface modifications */

  mesh->info.renum    = 0;   /* [0]    , Turn on/off the renumbering using SCOTCH; */
  mesh->info.nreg     = 0;
  /* default values for doubles */
  mesh->info.ls       = 0.0;      /* level set value */
  mesh->info.hgrad    = 1.3;      /* control gradation; */

  mesh->info.dhd  = 135.;

  //mesh->info.imprim = -7;

  /** MMG2D_IPARAM_bucket = 64 */
  mesh->info.bucket = 64;
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param sol pointer toward the sol structure.
 * \param iparam integer parameter to set (see \a MMG2D_Param structure).
 * \param val value for the parameter.
 * \return 0 if failed, 1 otherwise.
 *
 * Set integer parameter \a iparam at value \a val.
 *
 */
int MMG2D_Set_iparameter(MMG5_pMesh mesh, MMG5_pSol sol, int iparam, int val){

  switch ( iparam ) {
    /* Integer parameters */
  case MMG2D_IPARAM_verbose :
    mesh->info.imprim   = val;
    break;
  case MMG2D_IPARAM_mem :
    if ( val <= 0 ) {
      fprintf(stdout,"  ## Warning: maximal memory authorized must be strictly positive.\n");
      fprintf(stdout,"  Reset to default value.\n");
    }
    else
      mesh->info.mem      = val;
    _MMG2D_memOption(mesh);
    if(mesh->np && (mesh->npmax < mesh->np || mesh->ntmax < mesh->nt )) {
      return(0);
    } else if(mesh->info.mem < 39)
      return(0);
    break;
  case MMG2D_IPARAM_bucket :
    mesh->info.bucket   = val;
    break;
  case MMG2D_IPARAM_debug :
    mesh->info.ddebug   = val;
    break;
  case MMG2D_IPARAM_angle :
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
  case MMG2D_IPARAM_iso :
    mesh->info.iso      = val;
    break;
  case MMG2D_IPARAM_lag :
    if ( val < 0 || val > 2 )
      exit(EXIT_FAILURE);
    mesh->info.lag = val;
    break;
  case MMG2D_IPARAM_msh :
    mesh->info.nreg = val;
    break;
  case MMG2D_IPARAM_numsubdomain :
    mesh->info.renum = val;
    break;
  case MMG2D_IPARAM_noinsert :
    mesh->info.noinsert = val;
    break;
  case MMG2D_IPARAM_noswap :
    mesh->info.noswap   = val;
    break;
  case MMG2D_IPARAM_nomove :
    mesh->info.nomove   = val;
    break;
  case MMG2D_IPARAM_nosurf :
    mesh->info.nosurf   = val;
    break;
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
 * \param sol pointer toward the sol structure.
 * \param dparam double parameter to set (see \a MMG2D_Param structure).
 * \param val value of the parameter.
 * \return 0 if failed, 1 otherwise.
 *
 * Set double parameter \a dparam at value \a val.
 *
 */
int MMG2D_Set_dparameter(MMG5_pMesh mesh, MMG5_pSol sol, int dparam, double val){

  switch ( dparam ) {
    /* double parameters */
  case MMG2D_DPARAM_angleDetection :
    mesh->info.dhd = val;
    mesh->info.dhd = MG_MAX(0.0, MG_MIN(180.0,mesh->info.dhd));
    mesh->info.dhd = 180. - mesh->info.dhd;
    break;
  case MMG2D_DPARAM_hmin :
    mesh->info.hmin     = val;
    break;
  case MMG2D_DPARAM_hmax :
    mesh->info.hmax     = val;
    break;
  case MMG2D_DPARAM_hgrad :
    mesh->info.hgrad    = val;
    if ( mesh->info.hgrad < 0.0 )
      mesh->info.hgrad = -1.0;
    break;
  case MMG2D_DPARAM_hausd :
    if ( val <=0 ) {
      fprintf(stdout,"  ## Error: hausdorff number must be strictly positive.\n");
      return(0);
    }
    else
      mesh->info.hausd    = val;
    break;
  case MMG2D_DPARAM_ls :
    mesh->info.ls       = val;
    fprintf(stdout,"  ## Warning: unstable feature.\n");
    break;
  default :
    fprintf(stdout,"  ## Error: unknown type of parameter\n");
    return(0);
  }
  return(1);
}



/**
 * \param mesh pointer toward the mesh structure.
 * \param np number of vertices.
 * \param nt number of triangles.
 * \param na number of edges.
 * \return 0 if failed, 1 otherwise.
 *
 * Set the number of vertices, tetrahedra, triangles and edges of the
 * mesh and allocate the associated tables. If call twice, reset the
 * whole mesh to realloc it at the new size
 *
 */
int MMG2D_Set_meshSize(MMG5_pMesh mesh, int np, int nt, int na) {
  int k;

  if ( ( (mesh->info.imprim > 5) || mesh->info.ddebug ) &&
       ( mesh->point || mesh->tria || mesh->edge) )
    fprintf(stdout,"  ## Warning: new mesh\n");

  mesh->np  = np;
  mesh->nt  = nt;
  mesh->na  = na;
  mesh->npi = mesh->np;
  mesh->nti = mesh->nt;
  mesh->nai = mesh->na;

  if ( mesh->point )
    _MMG5_DEL_MEM(mesh,mesh->point,(mesh->npmax+1)*sizeof(MMG5_Point));
  if ( mesh->tria )
    _MMG5_DEL_MEM(mesh,mesh->tria,(mesh->ntmax+1)*sizeof(MMG5_Tria));
  if ( mesh->edge )
    _MMG5_DEL_MEM(mesh,mesh->edge,(mesh->namax+1)*sizeof(MMG5_Edge));

  /*tester si -m definie : renvoie 0 si pas ok et met la taille min dans info.mem */
  if( mesh->info.mem > 0) {
    if((mesh->npmax < mesh->np || mesh->ntmax < mesh->nt || mesh->namax < mesh->na) ) {
      _MMG2D_memOption(mesh);
      //     printf("pas de pbs ? %d %d %d %d %d %d -- %d\n",mesh->npmax,mesh->np,
      //     mesh->ntmax,mesh->nt,mesh->nemax,mesh->ne,mesh->info.mem);
      if((mesh->npmax < mesh->np || mesh->ntmax < mesh->nt)) {
        fprintf(stdout,"mem insuffisante np : %d %d nt : %d %d \n"
                ,mesh->npmax,mesh->np,
                mesh->ntmax,mesh->nt);
        return(0);
      }
      else
        return(1);
    } else if(mesh->info.mem < 39) {
      printf("mem insuffisante %d\n",mesh->info.mem);
      return(0);
    }
  } else {
    mesh->npmax = MG_MAX(1.5*mesh->np,_MMG2D_NPMAX);
    mesh->ntmax = MG_MAX(1.5*mesh->nt,_MMG2D_NEMAX);

  }
  _MMG5_ADD_MEM(mesh,(mesh->npmax+1)*sizeof(MMG5_Point),"initial vertices",
                printf("  Exit program.\n");
                exit(EXIT_FAILURE));
  _MMG5_SAFE_CALLOC(mesh->point,mesh->npmax+1,MMG5_Point);

  _MMG5_ADD_MEM(mesh,(mesh->ntmax+1)*sizeof(MMG5_Tria),"initial triangles",return(0));
  _MMG5_SAFE_CALLOC(mesh->tria,mesh->ntmax+1,MMG5_Tria);

  mesh->namax =  MG_MAX(mesh->na,_MMG2D_NEDMAX);
  _MMG5_ADD_MEM(mesh,(mesh->namax+1)*sizeof(MMG5_Edge),"initial edges",return(0));
  _MMG5_SAFE_CALLOC(mesh->edge,(mesh->namax+1),MMG5_Edge);

  /* keep track of empty links */
  mesh->npnil = mesh->np + 1;
  mesh->nenil = mesh->nt + 1;
  mesh->nanil = mesh->na + 1;

  for (k=mesh->npnil; k<mesh->npmax-1; k++) {
    mesh->point[k].tmp  = k+1;
  }
  for (k=mesh->nenil; k<mesh->ntmax-1; k++) {
    mesh->tria[k].v[2] = k+1;
  }
  for (k=mesh->nanil; k<mesh->namax-1; k++) {
    mesh->edge[k].b = k+1;
  }

  if ( !mesh->nt ) {
    fprintf(stdout,"  **WARNING NO GIVEN TRIANGLE\n");
  }

  /* stats */
  if ( abs(mesh->info.imprim) > 6 ) {
    fprintf(stdout,"     NUMBER OF VERTICES     %8d\n",mesh->np);
    if ( mesh->na ) {
      fprintf(stdout,"     NUMBER OF EDGES        %8d\n",mesh->na);
    }
    if ( mesh->nt )
      fprintf(stdout,"     NUMBER OF TRIANGLES    %8d\n",mesh->nt);
  }
  return(1);
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
int MMG2D_Set_solSize(MMG5_pMesh mesh, MMG5_pSol sol, int typEntity, int np, int typSol) {

  if ( ( (mesh->info.imprim > 5) || mesh->info.ddebug ) && sol->m )
    fprintf(stdout,"  ## Warning: new solution\n");

  if ( typEntity != MMG5_Vertex ) {
    fprintf(stdout,"  ## Error: MMG2D need a solution imposed on vertices\n");
    return(0);
  }
  if ( typSol == MMG5_Scalar ) {
    sol->size = 1;
  }
  else if ( typSol == MMG5_Tensor ) {
    sol->size = 6;
  }
  else {
    fprintf(stdout,"  ## Error: type of solution not yet implemented\n");
    return(0);
  }

  sol->dim = 2;
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
 * \param sol pointer toward the sol structure.
 * \param typEntity pointer toward the type of entities to which solutions are applied.
 * \param np pointer toward the number of solutions.
 * \param typSol pointer toward the type of the solutions (scalar, vectorial...)
 * \return 1.
 *
 * Get the solution number, dimension and type.
 *
 */
int MMG2D_Get_solSize(MMG5_pMesh mesh, MMG5_pSol sol, int* typEntity, int* np,
                      int* typSol) {

  *typEntity = MMG5_Vertex;
  if ( sol->size == 1 )
    *typSol    = MMG5_Scalar;
  else if ( sol->size == 2 )
    *typSol    = MMG5_Vector;
  else if ( sol->size == 3 )
    *typSol    = MMG5_Tensor;
  else
    *typSol    = MMG5_Notype;

  assert( (!sol->np) || (sol->np == mesh->np));

  *np = sol->np;

  return(1);
}


/**
 * \param mesh pointer toward the mesh structure.
 * \param np pointer toward the number of vertices.
 * \param nt pointer toward the number of triangles.
 * \param na pointer toward the number of edges.
 * \return 1.
 *
 * Get the number of vertices, triangles and edges of the mesh.
 *
 * \warning special treatment for edges because they are not packed.
 */
int MMG2D_Get_meshSize(MMG5_pMesh mesh, int* np, int* nt, int* na) {
  int k;

  if ( np != NULL )
    *np = mesh->np;
  if ( nt != NULL )
    *nt = mesh->nt;

  if ( na != NULL ) {
    // Edges are not packed, thus we must count it.
    *na = 0;
    if ( mesh->na ) {
      for (k=1; k<=mesh->na; k++) {
        if ( mesh->edge[k].a ) ++(*na);
      }
    }
  }

  return(1);
}
/**
 * \param mesh pointer toward the mesh structure.
 * \param c0 coordinate of the point along the first dimension.
 * \param c1 coordinate of the point along the second dimension.
 * \param ref point reference.
 * \param pos position of the point in the mesh.
 * \return 1.
 *
 * Set vertex of coordinates \a c0, \a c1 and reference \a ref
 * at position \a pos in mesh structure
 *
 */
int MMG2D_Set_vertex(MMG5_pMesh mesh, double c0, double c1, int ref, int pos) {

  if ( !mesh->np ) {
    fprintf(stdout,"  ## Error: You must set the number of points with the");
    fprintf(stdout," MMG2D_Set_meshSize function before setting vertices in mesh\n");
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
  mesh->point[pos].ref  = ref;
  if ( mesh->nt )
    mesh->point[pos].tag  = MG_NUL;
  else
    mesh->point[pos].tag  &= ~MG_NUL;

  mesh->point[pos].flag = 0;
  mesh->point[pos].tmp = 0;

  return(1);
}
/* /\** */
/*  * \param mesh pointer toward the mesh structure. */
/*  * \param k vertex index. */
/*  * \return 1. */
/*  * */
/*  * Set corner at point \a k. */
/*  * */
/*  *\/ */
/* int MMG2D_Set_corner(MMG5_pMesh mesh, int k) { */
/*   assert ( k <= mesh->np ); */
/*   mesh->point[k].tag |= M_CORNER; */
/*   return(1); */
/* } */
/* /\** */
/*  * \param mesh pointer toward the mesh structure. */
/*  * \param k vertex index. */
/*  * \return 1. */
/*  * */
/*  * Set point \a k as required. */
/*  * */
/*  *\/ */
/* int MMG2D_Set_requiredVertex(MMG5_pMesh mesh, int k) { */
/*   assert ( k <= mesh->np ); */
/*   mesh->point[k].tag |= M_REQUIRED; */
/*   return(1); */
/* } */

/**
 * \param mesh pointer toward the mesh structure.
 * \param c0 pointer toward the coordinate of the point along the first dimension.
 * \param c1 pointer toward the coordinate of the point along the second dimension.
 * \param ref poiter to the point reference.
 * \param isCorner pointer toward the flag saying if point is corner.
 * \param isRequired pointer toward the flag saying if point is required.
 * \return 1.
 *
 * Get coordinates \a c0, \a c1 and reference \a ref of
 * vertex num of mesh.
 *
 */
int MMG2D_Get_vertex(MMG5_pMesh mesh, double* c0, double* c1, int* ref,
                    int* isCorner, int* isRequired) {

 if ( mesh->npi == mesh->np ) {
   mesh->npi = 0;
   if ( mesh->info.ddebug ) {
    fprintf(stdout,"  ## Warning: reset the internal counter of points.\n");
    fprintf(stdout,"     You must pass here exactly one time (the first time ");
    fprintf(stdout,"you call the MMG2D_Get_vertex function).\n");
    fprintf(stdout,"     If not, the number of call of this function");
    fprintf(stdout," exceed the number of points: %d\n ",mesh->np);
   }
 }

  mesh->npi++;

  if ( mesh->npi > mesh->np ) {
    fprintf(stdout,"  ## Error: unable to get point.\n");
    fprintf(stdout,"     The number of call of MMG2D_Get_vertex function");
    fprintf(stdout," exceed the number of points: %d\n ",mesh->np);
    return(0);
  }

  *c0  = mesh->point[mesh->npi].c[0];
  *c1  = mesh->point[mesh->npi].c[1];
  if ( ref != NULL )
    *ref = mesh->point[mesh->npi].ref;

  if ( isCorner != NULL ) {
    if ( mesh->point[mesh->npi].tag & M_CORNER )
      *isCorner = 1;
    else
      *isCorner = 0;
  }

  if ( isRequired != NULL ) {
    if ( mesh->point[mesh->npi].tag & M_REQUIRED )
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
int MMG2D_Set_triangle(MMG5_pMesh mesh, int v0, int v1, int v2, int ref, int pos) {
  MMG5_pTria pt;
  double  vol;
  int    i,tmp;

  if ( !mesh->nt ) {
    fprintf(stdout,"  ## Error: You must set the number of elements with the");
    fprintf(stdout," MMG2D_Set_meshSize function before setting elements in mesh\n");
    return(0);
  }

  if ( pos > mesh->ntmax ) {
    fprintf(stdout,"  ## Error: unable to allocate a new element.\n");
    fprintf(stdout,"    max number of element: %d\n",mesh->ntmax);
    _MMG5_INCREASE_MEM_MESSAGE();
    return(0);
  }

  if ( pos > mesh->nt ) {
    fprintf(stdout,"  ## Error: attempt to set new triangle at position %d.",pos);
    fprintf(stdout," Overflow of the given number of triangle: %d\n",mesh->nt);
    fprintf(stdout,"  ## Check the mesh size, its compactness or the position");
    fprintf(stdout," of the triangle.\n");
    return(0);
  }

  pt = &mesh->tria[pos];
  pt->v[0] = v0;
  pt->v[1] = v1;
  pt->v[2] = v2;
  pt->ref  = ref;

  mesh->point[pt->v[0]].tag &= ~MG_NUL;
  mesh->point[pt->v[1]].tag &= ~MG_NUL;
  mesh->point[pt->v[2]].tag &= ~MG_NUL;

  for(i=0 ; i<3 ; i++)
    pt->edg[i] = 0;

  vol = MMG2_quickarea(mesh->point[pt->v[0]].c,mesh->point[pt->v[1]].c,
                           mesh->point[pt->v[2]].c);
  if(vol < 0) {
    printf("Tr %d bad oriented\n",pos);
    tmp = pt->v[2];
    pt->v[2] = pt->v[1];
    pt->v[1] = tmp;
    /* mesh->xt temporary used to count reoriented tetra */
    mesh->xt++;
  }
  if ( mesh->info.ddebug && (mesh->nt == pos) && mesh->xt > 0 ) {
    fprintf(stdout,"  ## %d triangles reoriented\n",mesh->xt);
    mesh->xt = 0;
  }

  return(1);
}
/* /\** */
/*  * \param mesh pointer toward the mesh structure. */
/*  * \param k triangle index. */
/*  * \return 1. */
/*  * */
/*  * Set triangle \a k as required. */
/*  * */
/*  *\/ */
/* int MMG2D_Set_requiredTriangle(MMG5_pMesh mesh, int k) { */
/*   assert ( k <= mesh->nt ); */
/*   mesh->tria[k].tag[0] |= M_REQUIRED; */
/*   mesh->tria[k].tag[1] |= M_REQUIRED; */
/*   mesh->tria[k].tag[2] |= M_REQUIRED; */
/*   return(1); */
/* } */

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
int MMG2D_Get_triangle(MMG5_pMesh mesh, int* v0, int* v1, int* v2, int* ref
                       ,int* isRequired) {
  MMG5_pTria  ptt;

  if ( mesh->nti == mesh->nt ) {
    mesh->nti = 0;
    if ( mesh->info.ddebug ) {
      fprintf(stdout,"  ## Warning: reset the internal counter of triangles.\n");
      fprintf(stdout,"     You must pass here exactly one time (the first time ");
      fprintf(stdout,"you call the MMG2D_Get_triangle function).\n");
      fprintf(stdout,"     If not, the number of call of this function");
      fprintf(stdout," exceed the number of triangles: %d\n ",mesh->nt);
    }
  }

  mesh->nti++;

  if ( mesh->nti > mesh->nt ) {
    fprintf(stdout,"  ## Error: unable to get triangle.\n");
    fprintf(stdout,"    The number of call of MMG2D_Get_triangle function");
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
 * \param v0 first vertex of edge.
 * \param v1 second vertex of edge.
 * \param ref edge reference.
 * \param pos edge position in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set edge of vertices \a v0, \a v1 and reference \a ref
 * at position \a pos in mesh structure.
 *
 */
int MMG2D_Set_edge(MMG5_pMesh mesh, int v0, int v1, int ref, int pos) {
  MMG5_pEdge pt;

  if ( !mesh->na ) {
    fprintf(stdout,"  ## Error: You must set the number of elements with the");
    fprintf(stdout," MMG2D_Set_meshSize function before setting elements in mesh\n");
    return(0);
  }

  if ( pos > mesh->namax ) {
    fprintf(stdout,"  ## Error: unable to allocate a new element.\n");
    fprintf(stdout,"    max number of element: %d\n",mesh->namax);
    _MMG5_INCREASE_MEM_MESSAGE();
    return(0);
  }

  if ( pos > mesh->na ) {
    fprintf(stdout,"  ## Error: attempt to set new edge at position %d.",pos);
    fprintf(stdout," Overflow of the given number of edge: %d\n",mesh->na);
    fprintf(stdout,"  ## Check the mesh size, its compactness or the position");
    fprintf(stdout," of the edge.\n");
    return(0);
  }

  pt = &mesh->edge[pos];
  pt->a = v0;
  pt->b = v1;
  pt->ref  = ref;

  mesh->point[pt->a].tag &= ~MG_NUL;
  mesh->point[pt->b].tag &= ~MG_NUL;

  return(1);
}
/* /\** */
/*  * \param mesh pointer toward the mesh structure. */
/*  * \param k edge index. */
/*  * \return 1. */
/*  * */
/*  * Set edge \a k as required. */
/*  * */
/*  *\/ */
/* int MMG2D_Set_requiredEdge(MMG5_pMesh mesh, int k) { */
/*   assert ( k <= mesh->na ); */
/*   mesh->edge[k].tag |= M_REQUIRED; */
/*   return(1); */
/* } */

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
 * \warning edges are not packed.
 */
int MMG2D_Get_edge(MMG5_pMesh mesh, int* e0, int* e1, int* ref
                   ,int* isRidge, int* isRequired) {
  MMG5_pEdge        ped;

  if ( mesh->nai == mesh->na ) {
    mesh->nai = 0;
    if ( mesh->info.ddebug ) {
      fprintf(stdout,"  ## Warning: reset the internal counter of edges.\n");
      fprintf(stdout,"     You must pass here exactly one time (the first time ");
      fprintf(stdout,"you call the MMG2D_Get_edge function).\n");
      fprintf(stdout,"     If not, the number of call of this function");
      fprintf(stdout," exceed the number of edges.\n ");
      fprintf(stdout,"     Please, call the MMG2D_Get_meshSize function to get"
              " this number.\n ");
    }
  }

  mesh->nai++;

  if ( mesh->nai > mesh->na ) {
    fprintf(stdout,"  ## Error: unable to get edge.\n");
    fprintf(stdout,"    The number of call of MMG2D_Get_edge function");
    fprintf(stdout," can not exceed the number of edges: %d\n ",mesh->na);
    return(0);
  }

  ped = &mesh->edge[mesh->nai];

  while ( !ped->a && ++mesh->nai <= mesh->na ) {
    ped = &mesh->edge[mesh->nai];
  }


  *e0  = ped->a;
  *e1  = ped->b;

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
 * \param met pointer toward the sol structure.
 * \param s solution scalar value.
 * \param pos position of the solution in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set scalar value \a s at position \a pos in solution structure
 *
 */
int MMG2D_Set_scalarSol(MMG5_pSol met, double s, int pos) {

  if ( !met->np ) {
    fprintf(stdout,"  ## Error: You must set the number of solution with the");
    fprintf(stdout," MMG2D_Set_solSize function before setting values");
    fprintf(stdout," in solution structure \n");
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
int  MMG2D_Get_scalarSol(MMG5_pSol met, double* s)
{
  int ddebug = 0;

  if ( met->npi == met->np ) {
    met->npi = 0;
    if ( ddebug ) {
      fprintf(stdout,"  ## Warning: reset the internal counter of points.\n");
      fprintf(stdout,"     You must pass here exactly one time (the first time ");
      fprintf(stdout,"you call the MMG2D_Get_scalarSol function).\n");
      fprintf(stdout,"     If not, the number of call of this function");
      fprintf(stdout," exceed the number of points: %d\n ",met->np);
    }
  }

  met->npi++;

  if ( met->npi > met->np ) {
    fprintf(stdout,"  ## Error: unable to get solution.\n");
    fprintf(stdout,"     The number of call of MMG2D_Get_scalarSol function");
    fprintf(stdout," can not exceed the number of points: %d\n ",met->np);
    return(0);
  }

  *s  = met->m[met->npi];

  return(1);
}

/**
 * \param met pointer toward the sol structure.
 * \param m11 value at position (1,1) in the solution tensor.
 * \param m12 value at position (1,2) in the solution tensor.
 * \param m22 value at position (2,2) in the solution tensor.
 * \param pos position of the solution in the mesh.
 * \return 0 if failed, 1 otherwise.
 *
 * Set tensor value \a s at position \a pos in solution structure
 *
 */
int MMG2D_Set_tensorSol(MMG5_pSol met, double m11, double m12, double m22,
                        int pos) {
  int isol;

  if ( !met->np ) {
    fprintf(stdout,"  ## Error: You must set the number of solution with the");
    fprintf(stdout," MMG2D_Set_solSize function before setting values");
    fprintf(stdout," in solution structure \n");
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
  isol = (pos-1) * met->size + 1;
  met->m[isol + 0] = m11;
  met->m[isol + 1] = m12;
  met->m[isol + 2] = m22;
  return(1);
}
/**
 * \param met pointer toward the sol structure.
 * \param m11 pointer toward the position (1,1) in the solution tensor.
 * \param m12 pointer toward the position (1,2) in the solution tensor.
 * \param m22 pointer toward the position (2,2) in the solution tensor.
 * \return 0 if failed, 1 otherwise.
 *
 * Get tensorial solution of next vertex of mesh.
 *
 */
int MMG2D_Get_tensorSol(MMG5_pSol met, double *m11,double *m12,double *m22)
{
  int ddebug = 0;

  if ( met->npi == met->np ) {
    met->npi = 0;
    if ( ddebug ) {
      fprintf(stdout,"  ## Warning: reset the internal counter of points.\n");
      fprintf(stdout,"     You must pass here exactly one time (the first time ");
      fprintf(stdout,"you call the MMG2D_Get_tensorSol function).\n");
      fprintf(stdout,"     If not, the number of call of this function");
      fprintf(stdout," exceed the number of points: %d\n ",met->np);
    }
  }

  met->npi++;

  if ( met->npi > met->np ) {
    fprintf(stdout,"  ## Error: unable to get solution.\n");
    fprintf(stdout,"     The number of call of MMG2D_Get_tensorSol function");
    fprintf(stdout," can not exceed the number of points: %d\n ",met->np);
    return(0);
  }

  *m11 = met->m[6*(met->npi-1)+1];
  *m12 = met->m[6*(met->npi-1)+2];
  *m22 = met->m[6*(met->npi-1)+3];

  return(1);
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
int MMG2D_Chk_meshData(MMG5_pMesh mesh,MMG5_pSol met) {

  if ( (mesh->npi != mesh->np) || (mesh->nti != mesh->nt) ) {
    fprintf(stdout,"  ## Error: if you don't use the MMG2D_loadMesh function,");
    fprintf(stdout," you must call the MMG2D_Set_meshSize function to have a");
    fprintf(stdout," valid mesh.\n");
    fprintf(stdout," Missing datas.\n");
    return(0);
  }

  if ( met->npi != met->np ) {
    fprintf(stdout,"  ## Error: if you don't use the MMG2D_loadMet function,");
    fprintf(stdout," you must call the MMG2D_Set_solSize function to have a");
    fprintf(stdout," valid solution.\n");
    fprintf(stdout," Missing datas.\n");
    return(0);
  }

  /*  Check mesh data */
  if ( mesh->info.ddebug ) {
    if ( (!mesh->np) || (!mesh->point) ||
         (!mesh->nt)  ) {
      fprintf(stdout,"  ** MISSING DATA.\n");
      fprintf(stdout," Check that your mesh contains points.\n");
      fprintf(stdout," Exit program.\n");
      return(0);
    }
  }

  if ( mesh->dim != 2 ) {
    fprintf(stdout,"  ** 2 DIMENSIONAL MESH NEEDED. Exit program.\n");
    return(0);
  }
  if ( met->dim != 2 ) {
    fprintf(stdout,"  ** WRONG DIMENSION FOR METRIC. Exit program.\n");
    return(0);
  }
  if ( !mesh->ver )  mesh->ver = 2;
  if ( !met ->ver )  met ->ver = 2;

  return(1);
}

/**
 * \param starter dummy argument used to initialize the variadic argument list.
 * \param ... variadic arguments. For now, you need to call the \a
 * MMG2D_Free_all function with the following arguments :
 * MMG2D_Free_all(MMG5_ARG_start,MMG5_ARG_ppMesh, your_mesh,
 * MMG5_ARG_ppMet, your_metric,MMG5_ARG_end). Here, \a your_mesh is a pointer
 * toward \a MMG5_pMesh and \a your_metric a pointer toward \a MMG5_pSol.
 *
 * Deallocations before return.
 *
 * \remark we pass the structures by reference in order to have argument
 * compatibility between the library call from a Fortran code and a C code.
 *
 */
void MMG2D_Free_all(enum MMG5_arg starter,...)
{

  va_list argptr;

  va_start(argptr, starter);

  _MMG2D_Free_all_var(argptr);

  va_end(argptr);

  return;
}

/**
 * \param starter dummy argument used to initialize the variadic argument list.
 * \param ... variadic arguments. For now, you need to call the \a
 * MMG2D_Free_structures function with the following arguments :
 * MMG2D_Free_structures(MMG5_ARG_start,MMG5_ARG_ppMesh, your_mesh,
 * MMG5_ARG_ppMet, your_metric,MMG5_ARG_end). Here, \a your_mesh is a pointer
 * toward \a MMG5_pMesh and \a your_metric a pointer toward \a MMG5_pSol.
 *
 * Structure deallocations before return.
 *
 * \remark we pass the structures by reference in order to have argument
 * compatibility between the library call from a Fortran code and a C code.
 *
 */
void MMG2D_Free_structures(enum MMG5_arg starter,...)
{

  va_list argptr;

  va_start(argptr, starter);

  _MMG2D_Free_structures_var(argptr);

  va_end(argptr);

  return;
}

/**
 * \param starter dummy argument used to initialize the variadic argument list.
 * \param ... variadic arguments. For now, you need to call the \a
 * MMG2D_Free_names function with the following arguments :
 * MMG2D_Free_names(MMG5_ARG_start,MMG5_ARG_ppMesh, your_mesh,
 * MMG5_ARG_ppMet, your_metric,MMG5_ARG_end). Here, \a your_mesh is a pointer
 * toward \a MMG5_pMesh and \a your_metric a pointer toward \a MMG5_pSol.
 *
 * Structure deallocations before return.
 *
 * \remark we pass the structures by reference in order to have argument
 * compatibility between the library call from a Fortran code and a C code.
 *
 */
void MMG2D_Free_names(enum MMG5_arg starter,...)
{

  va_list argptr;

  va_start(argptr, starter);

  _MMG2D_Free_names_var(argptr);

  va_end(argptr);

  return;
}
