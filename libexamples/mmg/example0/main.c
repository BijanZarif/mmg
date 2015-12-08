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
 * Example of use of the mmg library that gathers the mmg2d, mmgs and mmg3d
 * libraries (basic use of mesh adaptation).
 *
 * \author Charles Dapogny (LJLL, UPMC)
 * \author Cécile Dobrzynski (Inria / IMB, Université de Bordeaux)
 * \author Pascal Frey (LJLL, UPMC)
 * \author Algiane Froehly (Inria / IMB, Université de Bordeaux)
 * \version 5
 * \copyright GNU Lesser General Public License.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

/** Include the mmg library hader file */
// if the "include/mmg" dir is in your include path
//#include "libmmg.h"
// if your include path do not contain the "mmg" subdirectories
#include "mmg/libmmg.h"

int main(int argc,char *argv[]) {
  MMG5_pMesh      mmgMesh;
  MMG5_pSol       mmgSol;
  int             ier,k;
  char            *pwd,*filename,*filename_os,*filename_o3d;

  fprintf(stdout,"  -- TEST MMGLIB \n");

  /** ================== 2d remeshing using the mmg2d library ========== */

  /** ------------------------------ STEP   I -------------------------- */
  /** 1) Initialisation of mesh and sol structures */
  /* args of InitMesh: mesh=&mmgMesh, sol=&mmgSol, input mesh name, input sol name,
     output mesh name */
  mmgMesh = NULL;
  mmgSol  = NULL;
  MMG2D_Init_mesh(&mmgMesh,&mmgSol);

  /** 2) Build mesh in MMG5 format */
  /** Two solutions: just use the MMG3D_loadMesh function that will read a .mesh(b)
      file formatted or manually set your mesh using the MMG2D_Set* functions */

  /** with MMG2D_loadMesh function */
  if ( MMG2D_loadMesh(mmgMesh,"init.mesh") != 1 )  exit(EXIT_FAILURE);
  /** 3) Build sol in MMG5 format */
  /** Two solutions: just use the MMG2D_loadSol function that will read a .sol(b)
      file formatted or manually set your sol using the MMG2D_Set* functions */
  if ( MMG2D_loadSol(mmgMesh,mmgSol,"init",0) != 1 )  exit(EXIT_FAILURE);

  /** ------------------------------ STEP  II -------------------------- */
  /** library call */
  ier = MMG2D_mmg2dlib(mmgMesh,mmgSol,NULL);

  if ( ier == MMG5_STRONGFAILURE ) {
    fprintf(stdout,"BAD ENDING OF MMG2DLIB: UNABLE TO SAVE MESH\n");
    return(ier);
  } else if ( ier == MMG5_LOWFAILURE )
    fprintf(stdout,"BAD ENDING OF MMG2DLIB\n");

  /** ------------------------------ STEP III -------------------------- */
  /** get results */
  /** Two solutions: just use the MMG2D_saveMesh/MMG2D_saveSol functions
      that will write .mesh(b)/.sol formatted files or manually get your mesh/sol
      using the MMG2D_getMesh/MMG2D_getSol functions */

  /** 1) Automatically save the mesh */
  /*save result*/
  if ( MMG2D_saveMesh(mmgMesh,"result.mesh") != 1 )  exit(EXIT_FAILURE);
  /*save metric*/
  if ( MMG2D_saveSol(mmgMesh,mmgSol,"result") != 1 )  exit(EXIT_FAILURE);

  /** 2) Free the MMG2D structures */
  MMG2D_Free_all(mmgMesh,mmgSol);

  /** ================ surface remeshing using the mmgs library ======== */

  /* Name and path of the mesh file */
  pwd = getenv("PWD");
  filename = (char *) calloc(strlen(pwd) + 47, sizeof(char));
  if ( filename == NULL ) {
    perror("  ## Memory problem: calloc");
    exit(EXIT_FAILURE);
  }
  filename_os = (char *) calloc(strlen(pwd) + 47, sizeof(char));
  if ( filename_os == NULL ) {
    perror("  ## Memory problem: calloc");
    exit(EXIT_FAILURE);
  }
  sprintf(filename, "%s%s%s", pwd, "/../libexamples/mmg/example0/", "cube");
  sprintf(filename_os, "%s%s%s", pwd, "/../libexamples/mmg/example0/", "cube.s");

  /** ------------------------------ STEP   I -------------------------- */
  /** 1) Initialisation of mesh and sol structures */
  /* args of InitMesh: mesh=&mmgMesh, sol=&mmgSol, input mesh name, input sol name,
     output mesh name */

  mmgMesh = NULL;
  mmgSol  = NULL;
  MMGS_Init_mesh(&mmgMesh,&mmgSol,NULL);

  /** 2) Build mesh in MMG5 format */
  /** Two solutions: just use the MMGS_loadMesh function that will read a .mesh(b)
      file formatted or manually set your mesh using the MMGS_Set* functions */

  /** with MMGS_loadMesh function */
  /** a) (not mandatory): give the mesh name
      (by default, the "mesh.mesh" file is oppened)*/
  if ( MMGS_Set_inputMeshName(mmgMesh,filename) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMGS_loadMesh(mmgMesh) != 1 )  exit(EXIT_FAILURE);
  /** 3) Build sol in MMG5 format */
  /** Two solutions: just use the MMGS_loadSol function that will read a .sol(b)
      file formatted or manually set your sol using the MMGS_Set* functions */

  /** With MMGS_loadSol function */
  /** a) (not mandatory): give the sol name (by default, the name with the same
      name as the mesh but the .sol extension file is oppened)*/
  if ( MMGS_Set_inputSolName(mmgMesh,mmgSol,filename) != 1 )
    exit(EXIT_FAILURE);

  /** b) function calling */
  if ( MMGS_loadSol(mmgMesh,mmgSol) != 1 )  exit(EXIT_FAILURE);

  /** ------------------------------ STEP  II -------------------------- */
  /** library call */
  ier = MMGS_mmgslib(mmgMesh,mmgSol);

  if ( ier == MMG5_STRONGFAILURE ) {
    fprintf(stdout,"BAD ENDING OF MMGSLIB: UNABLE TO SAVE MESH\n");
    return(ier);
  } else if ( ier == MMG5_LOWFAILURE )
    fprintf(stdout,"BAD ENDING OF MMGSLIB\n");

  /** ------------------------------ STEP III -------------------------- */
  /** get results */
  /** Two solutions: just use the MMGS_saveMesh/MMGS_saveSol functions
      that will write .mesh(b)/.sol formatted files or manually get your mesh/sol
      using the MMGS_getMesh/MMGS_getSol functions */

  /** 1) Automatically save the mesh */
  /** a)  (not mandatory): give the ouptut mesh name using MMGS_Set_outputMeshName
      (by default, the mesh is saved in the "mesh.o.mesh" file */
  if ( MMGS_Set_outputMeshName(mmgMesh,filename_os) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  /*save result*/
  if ( MMGS_saveMesh(mmgMesh) != 1 )  exit(EXIT_FAILURE);

  /** 2) Automatically save the solution */
  /** a)  (not mandatory): give the ouptut sol name using MMGS_Set_outputSolName
      (by default, the mesh is saved in the "mesh.o.sol" file */
  if ( MMGS_Set_outputSolName(mmgMesh,mmgSol,filename_os) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  /*save metric*/
  if ( MMGS_saveSol(mmgMesh,mmgSol) != 1 )  exit(EXIT_FAILURE);

  /** 3) Free the MMGS structures */
  MMGS_Free_all(mmgMesh,mmgSol,NULL);

  /** ================== 3d remeshing using the mmg3d library ========== */
  filename_o3d = (char *) calloc(strlen(pwd) + 47, sizeof(char));
  if ( filename_o3d == NULL ) {
    perror("  ## Memory problem: calloc");
    exit(EXIT_FAILURE);
  }
  sprintf(filename_o3d, "%s%s%s", pwd, "/../libexamples/mmg/example0/", "cube.3d");

  /** ------------------------------ STEP   I -------------------------- */
  /** 1) Initialisation of mesh and sol structures */
  /* args of InitMesh: mesh=&mmgMesh, sol=&mmgSol */
  mmgMesh = NULL;
  mmgSol  = NULL;
  MMG3D_Init_mesh(&mmgMesh,&mmgSol,NULL);

  /** 2) Build mesh in MMG5 format */
  /** Two solutions: just use the MMG3D_loadMesh function that will read a .mesh(b)
      file formatted or manually set your mesh using the MMG3D_Set* functions */

  /** with MMG3D_loadMesh function */
  /** a) (not mandatory): give the mesh name
      (by default, the "mesh.mesh" file is oppened)*/
  if ( MMG3D_Set_inputMeshName(mmgMesh,filename) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMG3D_loadMesh(mmgMesh) != 1 )  exit(EXIT_FAILURE);

  /** 3) Build sol in MMG5 format */
  /** Two solutions: just use the MMG3D_loadSol function that will read a .sol(b)
      file formatted or manually set your sol using the MMG3D_Set* functions */

  /** With MMG3D_loadSol function */
  /** a) (not mandatory): give the sol name
      (by default, the "mesh.sol" file is oppened)*/
  if ( MMG3D_Set_inputSolName(mmgMesh,mmgSol,filename) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMG3D_loadSol(mmgMesh,mmgSol) != 1 )  exit(EXIT_FAILURE);

  /** ------------------------------ STEP  II -------------------------- */
  /** library call */
  ier = MMG3D_mmg3dlib(mmgMesh,mmgSol);

  if ( ier == MMG5_STRONGFAILURE ) {
    fprintf(stdout,"BAD ENDING OF MMG3DLIB: UNABLE TO SAVE MESH\n");
    return(ier);
  } else if ( ier == MMG5_LOWFAILURE )
    fprintf(stdout,"BAD ENDING OF MMG3DLIB\n");

  /** ------------------------------ STEP III -------------------------- */
  /** get results */
  /** Two solutions: just use the MMG3D_saveMesh/MMG3D_saveSol functions
      that will write .mesh(b)/.sol formatted files or manually get your mesh/sol
      using the MMG3D_getMesh/MMG3D_getSol functions */

  /** 1) Automatically save the mesh */
  /** a)  (not mandatory): give the ouptut mesh name using MMG3D_Set_outputMeshName
      (by default, the mesh is saved in the "mesh.o.mesh" file */
  if ( MMG3D_Set_outputMeshName(mmgMesh,filename_o3d) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMG3D_saveMesh(mmgMesh) != 1 )  exit(EXIT_FAILURE);

  /** 2) Automatically save the solution */
  /** a)  (not mandatory): give the ouptut sol name using MMG3D_Set_outputSolName
      (by default, the mesh is saved in the "mesh.o.sol" file */
  if ( MMG3D_Set_outputSolName(mmgMesh,mmgSol,filename_o3d) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMG3D_saveSol(mmgMesh,mmgSol) != 1 )  exit(EXIT_FAILURE);

  /** 3) Free the MMG3D structures */
  MMG3D_Free_all(mmgMesh,mmgSol,NULL);

  free(filename);
  free(filename_os);
  free(filename_o3d);

  return(0);
}
