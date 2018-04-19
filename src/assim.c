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
    int             first_cycle;
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

    /* First assimilation cycle flag */
    first_cycle = atoi(argv[11]);

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
    pihm = (pihm_struct)malloc(sizeof(*pihm));

    BuildPIHM(pihm_dir, project, pihm);

    /*
     * Create observation operators
     */
    ObsOper(obstbl, vartbl, pihm);

    /*
     * Data assimilation
     */
    Assim(pihm_dir, output_dir, paramtbl, vartbl, obstbl, obs_time, first_cycle,
        &ens);

    return EXIT_SUCCESS;
}

void Assim(const char *pihm_dir, const char *output_dir,
    const paramtbl_struct *paramtbl, const vartbl_struct *vartbl,
    const obstbl_struct *obstbl, int t, int first_cycle, ens_struct *ens)
{
    double         *xf;
    int             i, j, k;
    int             ne;
    char            obs_out_fn[MAXSTRING];
    char            obs_in_fn[MAXSTRING];
    FILE           *obs_fp;
    double          obs;
    double          obs_error;
    pihm_t_struct   pihm_time;

    pihm_time = PIHMTime(t);
//
//    enkf_struct     ens0;
//
    ne = ens->ne;
//
//    ens0 = (enkf_struct)malloc (sizeof *ens0);
//    ens0->ne = ne;
//    ens0->member = (ens_mbr_struct *)malloc (ne * sizeof(ens_mbr_struct));
//
//    for (i = 0; i < ne; i++)
//    {
//        for (j = 0; j < MAXVAR; j++)
//        {
//            if (ens->var[j].dim > 0)
//            {
//                ens0->member[i].var[j] =
//                    (double *)malloc(ens->var[j].dim * sizeof(double));
//            }
//        }
//    }
//
    xf = (double *)malloc(sizeof(double) * (ne + 1));

    printf("\nStarting EnKF ... \n");
//
//    /* Copy prior from ens to En0 */
//
//    for (i = 0; i < ne; i++)
//    {
//        for (j = 0; j < MAXPARAM; j++)
//        {
//            ens0->member[i].param[j] = ens->member[i].param[j];
//        }
//        for (j = 0; j < MAXVAR; j++)
//        {
//            if (ens->var[j].dim > 0)
//            {
//                for(k = 0; k < ens->var[j].dim; k++)
//                {
//                    ens0->member[i].var[j][k] = ens->member[i].var[j][k];
//                }
//            }
//        }
//    }
//
//
//    if (ens->nobs > 0)
//    {
    sprintf(obs_out_fn, "%s/output/%s/obs.dat", pihm_dir, output_dir);
    if (first_cycle)
    {
        obs_fp = fopen(obs_out_fn, "w");
    }
    else
    {
        obs_fp = fopen(obs_out_fn, "a");
    }
    fprintf (obs_fp, "\"%s\"", pihm_time.str);

    for (i = 0; i < MAXOBS; i++)
    {
        if ('\0' == obstbl[i].name[0])
        {
            break;
        }

        printf("\n*****%s******\n", obstbl[i].name);

        /* Read observations */
        ReadObs(t, obstbl[i].fname, &obs, &obs_error);

        printf("observed value = %lf (error: %lf)\n", obs, obs_error);

        /* Read ensemble forecasts */
        Forecast(ens, vartbl, &obstbl[i], xf);

        /* Prepare forecast vectors */
        printf("prediction = ");
        for (j = 0; j < ne; j++)
        {
            printf("%f\t", xf[j]);
        }
        printf("mean: %f\n", xf[ne]);

        /* Write observations to files */

        fprintf (obs_fp, "\t%lf", obs);

//      UpdAnlys (ens, obs, obs_error, xf);
    }

        fprintf(obs_fp, "\n");
        fflush (obs_fp);
        fclose (obs_fp);
//
//        /* Covariance inflation */
//        CovInflt(ens, ens0);
//    }
//
//    WriteEnKFOut (project, ens, outputdir, obs_time);
//
//    for (i = 0; i < ne; i++)
//    {
//        for (j = 0; j < MAXVAR; j++)
//        {
//            if (ens->var[j].dim > 0)
//            {
//                free (ens0->member[i].var[j]);
//            }
//        }
//    }
//
//    free (ens0->member);
//    free (ens0);
//
//    free(xf);
}
