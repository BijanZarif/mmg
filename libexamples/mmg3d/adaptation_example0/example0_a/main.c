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
 * Example of use of the mmg3d library (basic use of mesh adaptation)
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

/** Include the mmg3d library hader file */
// if the header file is in the "include" directory
// #include "libmmg3d.h"
// if the header file is in "include/mmg/mmg3d"
#include "mmg/mmg3d/libmmg3d.h"

int main(int argc,char *argv[]) {
  MMG5_pMesh      mmgMesh;
  MMG5_pSol       mmgSol;
  int             ier;
  char            *pwd,*filename;

  fprintf(stdout,"  -- TEST MMG3DLIB \n");

  /* Name and path of the mesh file */
  pwd = getenv("PWD");
  filename = (char *) calloc(strlen(pwd) + 58, sizeof(char));
  if ( filename == NULL ) {
    perror("  ## Memory problem: calloc");
    exit(EXIT_FAILURE);
  }
  sprintf(filename, "%s%s%s", pwd, "/../libexamples/mmg3d/adaptation_example0/example0_a/", "cube");

  /** ------------------------------ STEP   I -------------------------- */
  /** 1) Initialisation of mesh and sol structures */
  /* args of InitMesh:
   * MMG5_ARG_start: we start to give the args of a variadic func
   * MMG5_ARG_ppMesh: next arg will be a pointer over a MMG5_pMesh
   * &mmgMesh: pointer toward your MMG5_pMesh (that store your mesh)
   * MMG5_ARG_ppMet: next arg will be a pointer over a MMG5_pSol storing a metric
   * &mmgSol: pointer toward your MMG5_pSol (that store your metric) */

  mmgMesh = NULL;
  mmgSol  = NULL;

  MMG3D_Init_mesh(MMG5_ARG_start,
                  MMG5_ARG_ppMesh,&mmgMesh,MMG5_ARG_ppMet,&mmgSol,
                  MMG5_ARG_end);

  /** 2) Build mesh in MMG5 format */
  /** Two solutions: just use the MMG3D_loadMesh function that will read a .mesh(b)
      file formatted or manually set your mesh using the MMG3D_Set* functions */

  /** with MMG3D_loadMesh function */
  /** a) (not mandatory): give the mesh name
     (by default, the "mesh.mesh" file is oppened)*/
  if ( MMG3D_Set_inputMeshName(mmgMesh,filename) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMG3D_loadMesh(mmgMesh,filename) != 1 )  exit(EXIT_FAILURE);

  /** 3) Build sol in MMG5 format */
  /** Two solutions: just use the MMG3D_loadSol function that will read a .sol(b)
      file formatted or manually set your sol using the MMG3D_Set* functions */

  /** With MMG3D_loadSol function */
  /** a) (not mandatory): give the sol name
     (by default, the "mesh.sol" file is oppened)*/
  if ( MMG3D_Set_inputSolName(mmgMesh,mmgSol,filename) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMG3D_loadSol(mmgMesh,mmgSol,filename) != 1 )
    exit(EXIT_FAILURE);

  /** 4) (not mandatory): check if the number of given entities match with mesh size */
  if ( MMG3D_Chk_meshData(mmgMesh,mmgSol) != 1 ) exit(EXIT_FAILURE);

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
  // MMG3D_Set_outputMeshName(mmgMesh,"output.mesh");
  /** b) function calling */
  if ( MMG3D_saveMesh(mmgMesh,"cube.o.mesh") != 1 ) {
    fprintf(stdout,"UNABLE TO SAVE MESH\n");
    return(MMG5_STRONGFAILURE);
  }

  /** 2) Automatically save the solution */
  /** a)  (not mandatory): give the ouptut sol name using MMG3D_Set_outputSolName
     (by default, the mesh is saved in the "mesh.o.sol" file */
  // MMG3D_Set_outputSolName(mmgMesh,mmgSol,"output.sol");
  /** b) function calling */
  if ( MMG3D_saveSol(mmgMesh,mmgSol,"cube.o.sol") != 1 ) {
    fprintf(stdout,"UNABLE TO SAVE SOL\n");
    return(MMG5_LOWFAILURE);
  }

  /** 3) Free the MMG3D5 structures */
  MMG3D_Free_all(MMG5_ARG_start,
                 MMG5_ARG_ppMesh,&mmgMesh,MMG5_ARG_ppMet,&mmgSol,
                 MMG5_ARG_end);

  free(filename);
  filename = NULL;

  return(ier);
}
