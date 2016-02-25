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
 * \file mmg3d/mmg3d.c
 * \brief Main file for MMG3D executable: perform 3d mesh adaptation.
 * \author Charles Dapogny (UPMC)
 * \author Cécile Dobrzynski (Bx INP/Inria/UBordeaux)
 * \author Pascal Frey (UPMC)
 * \author Algiane Froehly (Inria/UBordeaux)
 * \version 5
 * \copyright GNU Lesser General Public License.
 */

#include "mmg3d.h"

mytime         MMG5_ctim[TIMEMAX];


/**
 * Print elapsed time at end of process.
 */
static void _MMG5_endcod() {
  char   stim[32];

  chrono(OFF,&MMG5_ctim[0]);
  printim(MMG5_ctim[0].gdif,stim);
  fprintf(stdout,"\n   ELAPSED TIME  %s\n",stim);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \return 1 if success, 0 otherwise.
 *
 * Write a DEFAULT.mmg3d file containing the default values of parameters that
 * can be locally defined.
 *
 */
static inline
int _MMG3D_writeLocalParam( MMG5_pMesh mesh ) {
  char       data[]="DEFAULT.mmg3d";
  FILE       *out;

  /** Save the local parameters file */
  if ( !(out = fopen(data,"wb")) ) {
    fprintf(stderr,"\n  ** UNABLE TO OPEN %s.\n",data);
    return(0);
  }

  fprintf(stdout,"\n  %%%% %s OPENED\n",data);

  if (! _MMG5_writeLocalParam(mesh, out) ) return 0;

  fclose(out);
  fprintf(stdout,"  -- WRITING COMPLETED\n");

  return(1);
}

/**
 * \param mesh pointer toward the mesh structure.
 * \param met pointer toward a sol structure (metric).
 * \return \ref MMG5_SUCCESS if success, \ref MMG5_LOWFAILURE if failed
 * but a conform mesh is saved and \ref MMG5_STRONGFAILURE if failed and we
 * can't save the mesh.
 *
 * Program to save the local default parameter file: read the mesh and metric
 * (needed to compite the hmax/hmin parameters), scale the mesh and compute the
 * hmax/hmin param, unscale the mesh and write the default parameter file.
 *
 */
static inline
int _MMG3D_defaultOption(MMG5_pMesh mesh,MMG5_pSol met) {
  mytime    ctim[TIMEMAX];
  char      stim[32];

  _MMG3D_Set_commonFunc();

  signal(SIGABRT,_MMG5_excfun);
  signal(SIGFPE,_MMG5_excfun);
  signal(SIGILL,_MMG5_excfun);
  signal(SIGSEGV,_MMG5_excfun);
  signal(SIGTERM,_MMG5_excfun);
  signal(SIGINT,_MMG5_excfun);

  tminit(ctim,TIMEMAX);
  chrono(ON,&(ctim[0]));

  if ( mesh->info.npar ) {
    fprintf(stdout,"\n  ## Error: "
            "unable to save of a local parameter file with"
            " the default parameters values because local parameters"
            " are provided.\n");
    _LIBMMG5_RETURN(mesh,met,MMG5_LOWFAILURE);
  }


  if ( mesh->info.imprim ) fprintf(stdout,"\n  -- INPUT DATA\n");
  /* load data */
  chrono(ON,&(ctim[1]));

  if ( met->np && (met->np != mesh->np) ) {
    fprintf(stdout,"  ## WARNING: WRONG SOLUTION NUMBER. IGNORED\n");
    _MMG5_DEL_MEM(mesh,met->m,(met->size*(met->npmax+1))*sizeof(double));
    met->np = 0;
  }

  chrono(OFF,&(ctim[1]));
  printim(ctim[1].gdif,stim);
  if ( mesh->info.imprim )
    fprintf(stdout,"  --  INPUT DATA COMPLETED.     %s\n",stim);

  /* analysis */
  chrono(ON,&(ctim[2]));
  MMG3D_setfunc(mesh,met);

  if ( mesh->info.imprim ) {
    fprintf(stdout,"\n  %s\n   MODULE MMG3D: IMB-LJLL : %s (%s)\n  %s\n",MG_STR,MG_VER,MG_REL,MG_STR);
    fprintf(stdout,"\n  -- DEFAULT PARAMETERS COMPUTATION\n");
  }

  /* scaling mesh and hmin/hmax computation*/
  if ( !_MMG5_scaleMesh(mesh,met) ) _LIBMMG5_RETURN(mesh,met,MMG5_STRONGFAILURE);

  /* unscaling mesh */
  if ( !_MMG5_unscaleMesh(mesh,met) ) _LIBMMG5_RETURN(mesh,met,MMG5_STRONGFAILURE);

  /* Save the local parameters file */
  mesh->mark = 0;
  if ( !_MMG3D_writeLocalParam(mesh) ) {
    fprintf(stderr,"  ## Error: Unable to save the local parameters file.\n"
            "            Exit program.\n");
     _LIBMMG5_RETURN(mesh,met,MMG5_LOWFAILURE);
  }

  _LIBMMG5_RETURN(mesh,met,MMG5_SUCCESS);
}


/**
 * \param argc number of command line arguments.
 * \param argv command line arguments.
 * \return \ref MMG5_SUCCESS if success.
 * \return \ref MMG5_LOWFAILURE if failed but a conform mesh is saved.
 * \return \ref MMG5_STRONGFAILURE if failed and we can't save the mesh.
 *
 * Main program for MMG3D executable: perform mesh adaptation.
 *
 */
int main(int argc,char *argv[]) {

  MMG5_pMesh      mesh;
  MMG5_pSol       met,disp;
  int             ier;
  char            stim[32];

  fprintf(stdout,"  -- MMG3d, Release %s (%s) \n",MG_VER,MG_REL);
  fprintf(stdout,"     %s\n",MG_CPY);
  fprintf(stdout,"     %s %s\n",__DATE__,__TIME__);

  _MMG3D_Set_commonFunc();

  atexit(_MMG5_endcod);

  tminit(MMG5_ctim,TIMEMAX);
  chrono(ON,&MMG5_ctim[0]);


  /* assign default values */
  mesh = NULL;
  met  = NULL;
  disp = NULL;

  MMG3D_Init_mesh(MMG5_ARG_start,
                  MMG5_ARG_ppMesh,&mesh,MMG5_ARG_ppMet,&met,
                  MMG5_ARG_ppDisp,&disp,
                  MMG5_ARG_end);
  /* reset default values for file names */
  MMG3D_Free_names(MMG5_ARG_start,
                   MMG5_ARG_ppMesh,&mesh,MMG5_ARG_ppMet,&met,
                   MMG5_ARG_ppDisp,&disp,
                   MMG5_ARG_end);


  /* Set default metric size */
  if ( !MMG3D_Set_solSize(mesh,met,MMG5_Vertex,0,MMG5_Scalar) )
    _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);

  /* command line */
  if ( !MMG3D_parsar(argc,argv,mesh,met) )  return(MMG5_STRONGFAILURE);

  /* load data */
  fprintf(stdout,"\n  -- INPUT DATA\n");
  chrono(ON,&MMG5_ctim[1]);
  /* read mesh file */
  if ( MMG3D_loadMesh(mesh,mesh->namein)<1 )
    _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);

  /* read displacement if any */
  if ( mesh->info.lag > -1 ) {
    if ( !MMG3D_Set_solSize(mesh,disp,MMG5_Vertex,0,MMG5_Vector) )
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);

    if ( !MMG3D_Set_inputSolName(mesh,disp,met->namein) )
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);

    ier = MMG3D_loadSol(mesh,disp,disp->namein);
    if ( ier == 0 ) {
      fprintf(stdout,"  ## ERROR: NO DISPLACEMENT FOUND.\n");
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);
    }
    else if ( ier == -1 ) {
      fprintf(stdout,"  ## ERROR: WRONG DATA TYPE OR WRONG SOLUTION NUMBER.\n");
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);
    }
  }
  /* read metric if any */
  else {
    ier = MMG3D_loadSol(mesh,met,met->namein);
    if ( ier == -1 ) {
      fprintf(stdout,"  ## ERROR: WRONG DATA TYPE OR WRONG SOLUTION NUMBER.\n");
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);
    }
    if ( mesh->info.iso && !ier ) {
      fprintf(stdout,"  ## ERROR: NO ISOVALUE DATA.\n");
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);
    }
    if ( !MMG3D_parsop(mesh,met) )
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_LOWFAILURE);
  }

  chrono(OFF,&MMG5_ctim[1]);
  printim(MMG5_ctim[1].gdif,stim);
  fprintf(stdout,"  -- DATA READING COMPLETED.     %s\n",stim);

  if ( mesh->mark ) {
    /* Save a local parameters file containing the default parameters */
    ier = _MMG3D_defaultOption(mesh,met);
    _MMG5_RETURN_AND_FREE(mesh,met,disp,ier);
  }
  else if ( mesh->info.lag > -1 ) {
    ier = MMG3D_mmg3dmov(mesh,met,disp);
  }
  else if ( mesh->info.iso ) {
    ier = MMG3D_mmg3dls(mesh,met);
  }
  else {
    ier = MMG3D_mmg3dlib(mesh,met);
  }

  if ( ier != MMG5_STRONGFAILURE ) {
    chrono(ON,&MMG5_ctim[1]);
    if ( mesh->info.imprim )
      fprintf(stdout,"\n  -- WRITING DATA FILE %s\n",mesh->nameout);
    if ( !MMG3D_saveMesh(mesh,mesh->nameout) )
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);

    if ( !MMG3D_saveSol(mesh,met,met->nameout) )
      _MMG3D_RETURN_AND_FREE(mesh,met,disp,MMG5_STRONGFAILURE);

    chrono(OFF,&MMG5_ctim[1]);
    if ( mesh->info.imprim )
      fprintf(stdout,"  -- WRITING COMPLETED\n");
  }

  /* free mem */
  _MMG3D_RETURN_AND_FREE(mesh,met,disp,ier);
}
