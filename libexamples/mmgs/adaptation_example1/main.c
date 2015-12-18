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
 * Example of use of the mmgs library (advanced use of mesh adaptation)
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
// #include "libmmgs.h"
// if the header file is in "include/mmg/mmgs"
#include "mmg/mmgs/libmmgs.h"

int main(int argc,char *argv[]) {
  MMG5_pMesh      mmgMesh;
  MMG5_pSol       mmgSol;
  int             k,ier;
  char            *pwd,*inname,*outname;

  fprintf(stdout,"  -- TEST MMGSLIB \n");

  /* Name and path of the mesh files */
  pwd = getenv("PWD");
  inname = (char *) calloc(strlen(pwd) + 51, sizeof(char));
  if ( inname == NULL ) {
    perror("  ## Memory problem: calloc");
    exit(EXIT_FAILURE);
  }
  outname = (char *) calloc(strlen(pwd) + 60, sizeof(char));
  if ( outname == NULL ) {
    perror("  ## Memory problem: calloc");
    exit(EXIT_FAILURE);
  }
  sprintf(inname, "%s%s%s", pwd, "/../libexamples/mmgs/adaptation_example1/", "2spheres");

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
  if ( MMGS_Set_inputMeshName(mmgMesh,inname) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMGS_loadMesh(mmgMesh) != 1 )  exit(EXIT_FAILURE);

  /** 3) Build sol in MMG5 format */
  /** Two solutions: just use the MMGS_loadSol function that will read a .sol(b)
      file formatted or manually set your sol using the MMGS_Set* functions */

  /** With MMGS_loadSol function */
  /** a) (not mandatory): give the sol name
     (by default, the "mesh.sol" file is oppened)*/
  if ( MMGS_Set_inputSolName(mmgMesh,mmgSol,inname) != 1 )
    exit(EXIT_FAILURE);
  /** b) function calling */
  if ( MMGS_loadSol(mmgMesh,mmgSol) != 1 )
    exit(EXIT_FAILURE);

  /** 4) (not mandatory): check if the number of given entities match with mesh size */
  if ( MMGS_Chk_meshData(mmgMesh,mmgSol) != 1 ) exit(EXIT_FAILURE);

  /** 5) (not mandatory): set your global parameters using the
      MMGS_Set_iparameter and MMGS_Set_dparameter function
      (resp. for integer parameters and double param)*/


  /**------------------- First wave of refinment---------------------*/

  /* debug mode ON (default value = OFF) */
  if ( MMGS_Set_iparameter(mmgMesh,mmgSol,MMGS_IPARAM_debug, 1) != 1 )
    exit(EXIT_FAILURE);

  /* maximal memory size (default value = 50/100*ram) */
  if ( MMGS_Set_iparameter(mmgMesh,mmgSol,MMGS_IPARAM_mem, 600) != 1 )
    exit(EXIT_FAILURE);

  /* Maximal mesh size (default FLT_MAX)*/
  if ( MMGS_Set_dparameter(mmgMesh,mmgSol,MMGS_DPARAM_hmax,40) != 1 )
    exit(EXIT_FAILURE);

  /* Minimal mesh size (default 0)*/
  if ( MMGS_Set_dparameter(mmgMesh,mmgSol,MMGS_DPARAM_hmin,0.001) != 1 )
    exit(EXIT_FAILURE);

  /* Global hausdorff value (default value = 0.01) applied on the whole boundary */
  if ( MMGS_Set_dparameter(mmgMesh,mmgSol,MMGS_DPARAM_hausd, 0.1) != 1 )
    exit(EXIT_FAILURE);

  /* Gradation control (default value 1.105) */
  if ( MMGS_Set_dparameter(mmgMesh,mmgSol,MMGS_DPARAM_hgrad, 2) != 1 )
    exit(EXIT_FAILURE);

  /** library call */
  ier = MMGS_mmgslib(mmgMesh,mmgSol);
  if ( ier == MMG5_STRONGFAILURE ) {
    fprintf(stdout,"BAD ENDING OF MMGSLIB: UNABLE TO SAVE MESH\n");
    return(ier);
  } else if ( ier == MMG5_LOWFAILURE )
    fprintf(stdout,"BAD ENDING OF MMGSLIB\n");

  /* (Not mandatory) Automatically save the mesh */
  sprintf(outname, "%s%s%s", pwd, "/../libexamples/mmgs/adaptation_example1/", "2spheres_1.o.mesh");
  if ( MMGS_Set_outputMeshName(mmgMesh,outname) != 1 )
    exit(EXIT_FAILURE);

  MMGS_saveMesh(mmgMesh);

  /* (Not mandatory) Automatically save the solution */
  if ( MMGS_Set_outputSolName(mmgMesh,mmgSol,outname) != 1 )
    exit(EXIT_FAILURE);

  if ( MMGS_saveSol(mmgMesh,mmgSol) != 1 )
    exit(EXIT_FAILURE);


  /**------------------- Second wave of refinment---------------------*/
  /* We add different local hausdorff numbers on boundary componants (this
     local values are used instead of the global hausdorff number) */

  /* verbosity (default value = 4)*/
  if ( MMGS_Set_iparameter(mmgMesh,mmgSol,MMGS_IPARAM_verbose, 4) != 1 )
    exit(EXIT_FAILURE);

  if ( MMGS_Set_iparameter(mmgMesh,mmgSol,MMGS_IPARAM_mem, 1000) != 1 )
    exit(EXIT_FAILURE);
  if ( MMGS_Set_iparameter(mmgMesh,mmgSol,MMGS_IPARAM_debug, 0) != 1 )
    exit(EXIT_FAILURE);


  /** 6) (not mandatory): set your local parameters */
  /* use a hmin value of 0.005 on ref 36 and 0.1 on ref 38 */
  /* use a hmax value of 0.05 on ref 36 and 1 on ref 38 */
  /* For now, the local hausdroff value is not take into account */
  if ( MMGS_Set_iparameter(mmgMesh,mmgSol,MMGS_IPARAM_numberOfLocalParam,2) != 1 )
    exit(EXIT_FAILURE);

  /** for each local parameter: give the type and reference of the element on which
      you will apply a particular hausdorff number and the hausdorff number
      to apply. The global hausdorff number is applied on all boundary triangles
      without local hausdorff number */

  /* Be careful if you change the hausdorff number (or gradation value)
     between 2 run: the information of the previous hausdorff number
     (resp. gradation) is contained in the metric computed during
     the previous run.
     Then, you can not grow up the hausdorff value (resp. gradation) without
     resetting this metric (but you can decrease this value). */

  if ( MMGS_Set_localParameter(mmgMesh,mmgSol,MMG5_Triangle,36,0.005,0.05,1) != 1 )
    exit(EXIT_FAILURE);
  if ( MMGS_Set_localParameter(mmgMesh,mmgSol,MMG5_Triangle,38,0.1,1,1) != 1 )
    exit(EXIT_FAILURE);

  /** library call */
  ier = MMGS_mmgslib(mmgMesh,mmgSol);
  if ( ier == MMG5_STRONGFAILURE ) {
    fprintf(stdout,"BAD ENDING OF MMGSLIB: UNABLE TO SAVE MESH\n");
    return(ier);
  } else if ( ier == MMG5_LOWFAILURE )
    fprintf(stdout,"BAD ENDING OF MMGSLIB\n");

  /* (Not mandatory) Automatically save the mesh */
  sprintf(outname, "%s%s%s", pwd, "/../libexamples/mmgs/adaptation_example1/", "2spheres_2.o.mesh");
  if ( MMGS_Set_outputMeshName(mmgMesh,outname) != 1 )
    exit(EXIT_FAILURE);

  if ( MMGS_saveMesh(mmgMesh) != 1 )
    exit(EXIT_FAILURE);

  /* (Not mandatory) Automatically save the solution */
  if ( MMGS_Set_outputSolName(mmgMesh,mmgSol,outname) != 1 )
    exit(EXIT_FAILURE);

  if ( MMGS_saveSol(mmgMesh,mmgSol) != 1 )
    exit(EXIT_FAILURE);

  /* 7) free the MMGS structures */
  MMGS_Free_all(mmgMesh,mmgSol,NULL);

  free(inname);
  inname = NULL;
  free(outname);
  outname = NULL;

  return(ier);
}
