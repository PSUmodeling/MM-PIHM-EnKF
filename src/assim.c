#include "enkf.h"

int                 debug_mode;
int                 verbose_mode;
int                 nelem;
int                 nriver;

int main(int argc, char *argv[])
{

    char            pihm_dir[MAXSTRING];
    char            output_dir[MAXSTRING];
    char            project[MAXSTRING];
    char            vartbl_fn[MAXSTRING];
    char            paramtbl_fn[MAXSTRING];
    char            obstbl_fn[MAXSTRING];
    int             nelem, nriver;
    int             obs_time;
    pihm_struct     pihm;
    vartbl_struct   vartbl[MAXVAR];
    paramtbl_struct paramtbl[MAXPARAM];
    obstbl_struct   obstbl[MAXOBS];
    ens_struct      ens;

    /*
     * Copy parameters from command line argument
     */
     /* PIHM directory */
    strcpy(pihm_dir, argv[1]);

    /* Name of project */
    strcpy(project, argv[2]);

    /* Output directory */
    strcpy(output_dir, argv[3]);

    /* Name of parameter table file */
    strcpy(paramtbl_fn, argv[4]);

    /* Name of variable table file */
    strcpy(vartbl_fn, argv[5]);

    /* Name of observation table file */
    strcpy(obstbl_fn, argv[6]);

    /* Number of ensemble member */
    ens.ne = atoi(argv[7]);

    /* Time */
    obs_time = atoi(argv[8]);

    /* Number of elements*/
    nelem = atoi(argv[9]);

    /* Nubmer of river segments */
    nriver = atoi(argv[10]);

    /*
     * Read variable, parameter, and observation tables
     */
    ReadVarTbl(vartbl_fn, nelem, nriver, vartbl);

    ReadParamTbl(paramtbl_fn, paramtbl);

    ReadObsTbl(obstbl_fn, obstbl);

    /*
     * Read variables and parameters
     */
    ReadVar(pihm_dir, project, output_dir, obs_time, vartbl, &ens);

    ReadParam(pihm_dir, project, paramtbl, &ens);

    /*
     * Read and initialize some PIHM structures for observation operators
     */
    /* Allocate memory for model data structure */
    pihm = (pihm_struct)malloc(sizeof(*pihm));

    BuildPIHM(pihm_dir, project, pihm);

    /*
     * Create observation operators
     */
    ObsOper(obstbl, vartbl, pihm);

    return EXIT_SUCCESS;
}
