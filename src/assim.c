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
    int             nelem, nriver;
    int             obs_time;
    pihm_struct     pihm;
    vartbl_struct   vartbl[MAXVAR];
    paramtbl_struct paramtbl[MAXPARAM];
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

    /* Number of ensemble member */
    ens.ne = atoi(argv[6]);

    /* Time */
    obs_time = atoi(argv[7]);

    /* Number of elements*/
    nelem = atoi(argv[8]);

    /* Nubmer of river segments */
    nriver = atoi(argv[9]);

    /*
     * Read variable and parameter tables
     */
    ReadVarTbl(vartbl_fn, nelem, nriver, vartbl);

    ReadParamTbl(paramtbl_fn, paramtbl);

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

    /* Generate PIHM input file names */
    sprintf(pihm->filename.riv,  "%s/input/%s/%s.riv",
        pihm_dir, project, project);
    sprintf(pihm->filename.mesh, "%s/input/%s/%s.mesh",
        pihm_dir, project, project);
    sprintf(pihm->filename.att,  "%s/input/%s/%s.att",
        pihm_dir, project, project);
    sprintf(pihm->filename.soil, "%s/input/%s/%s.soil",
        pihm_dir, project, project);

    /* Read river input file */
    ReadRiver(pihm->filename.riv, &pihm->rivtbl, &pihm->shptbl, &pihm->matltbl,
        &pihm->forc);

    /* Read mesh structure input file */
    ReadMesh(pihm->filename.mesh, &pihm->meshtbl);

    /* Read attribute table input file */
    ReadAtt(pihm->filename.att, &pihm->atttbl);

    /* Read soil input file */
    ReadSoil(pihm->filename.soil, &pihm->soiltbl);

    return EXIT_SUCCESS;
}
