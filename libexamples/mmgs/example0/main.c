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
 * Example of use of the mmgs library (basic use of mesh adaptation)
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

/** Include the mmgs library hader file */
// if the header file is in the "include" directory
#include "libmmgs.h"
// if the header file is in "include/mmg/mmgs"
// #include "mmg/mmgs/libmmgs.h"

int main(int argc,char *argv[]) {
  MMG5_pMesh      mmgMesh;
  MMG5_pSol       mmgSol;
  int             ier;
  char            *pwd,*filename;

  fprintf(stdout,"  -- TEST MMGSLIB \n");

  /* Name and path of the mesh file */
  pwd = getenv("PWD");
  filename = (char *) calloc(strlen(pwd) + 47, sizeof(char));
  if ( filename == NULL ) {
    perror("  ## Memory problem: calloc");
    exit(EXIT_FAILURE);
  }
  sprintf(filename, "%s%s%s", pwd, "/../libexamples/mmgs/example0/", "cube");

  /** ------------------------------ STEP   I -------------------------- */
  /** 1) Initialisation of mesh and sol structures */
  /* args of InitMesh: mesh=&mmgMesh, sol=&mmgSol */
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
  /** a) (not mandatory): give the sol name
     (by default, the "mesh.sol" file is oppened)*/
  if ( MMGS_Set_inputSolName(mmgMesh,mmgSol,filename) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMGS_loadSol(mmgMesh,mmgSol) != 1 )
    exit(EXIT_FAILURE);

  /** 4) (not mandatory): check if the number of given entities match with mesh size */
  if ( MMGS_Chk_meshData(mmgMesh,mmgSol) != 1 ) exit(EXIT_FAILURE);

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
  // MMGS_Set_outputMeshName(mmgMesh,"output.mesh");
  /** b) function calling */
  if ( MMGS_saveMesh(mmgMesh) != 1 )
    exit(EXIT_FAILURE);

  /** 2) Automatically save the solution */
  /** a)  (not mandatory): give the ouptut sol name using MMGS_Set_outputSolName
     (by default, the mesh is saved in the "mesh.o.sol" file */
  // MMGS_Set_outputSolName(mmgMesh,mmgSol,"output.sol");
  /** b) function calling */
  if ( MMGS_saveSol(mmgMesh,mmgSol) != 1 )
    exit(EXIT_FAILURE);

  /** 3) Free the MMGS structures */
  MMGS_Free_all(mmgMesh,mmgSol,NULL);

  free(filename);
  filename = NULL;

  return(ier);
}
