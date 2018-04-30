#include "enkf.h"

int                 debug_mode;
int                 verbose_mode;
int                 nelem;
int                 nriver;
int                 tecplot;

int main(int argc, char *argv[])
{

    char            pihm_dir[MAXSTRING];
    char            output_dir[MAXSTRING];
    char            project[MAXSTRING];
    char            vartbl_fn[MAXSTRING];
    char            paramtbl_fn[MAXSTRING];
    char            obstbl_fn[MAXSTRING];
    int             obs_time;
    int             first_cycle;
    pihm_struct     pihm;
    vartbl_struct   vartbl[MAXVAR];
    paramtbl_struct paramtbl[MAXPARAM];
    obstbl_struct   obstbl[MAXOBS];
    double          inflt_weight;
    ens_struct      ens;

    if (argc != 3)
    {
        printf("Error: command line parameters are not correctly set.\n");
        exit(EXIT_FAILURE);
    }

    /*
     * Copy parameters from command line argument and environment variables
     */
     /* PIHM directory */
    strcpy(pihm_dir, getenv("PIHM_DIR"));

    /* Name of project */
    strcpy(project, getenv("PROJECT"));

    /* Output directory */
    strcpy(output_dir, getenv("OUTPUT_DIR"));

    /* Name of parameter table file */
    strcpy(paramtbl_fn, getenv("PARAM_TBL"));

    /* Name of variable table file */
    strcpy(vartbl_fn, getenv("VAR_TBL"));

    /* Name of observation table file */
    strcpy(obstbl_fn, getenv("OBS_TBL"));

    /* Number of ensemble member */
    ens.ne = atoi(getenv("NUM_MEMBER"));

    /* Covariance relaxation weight */
    inflt_weight = atof(getenv("INFLT_WEIGHT"));

    /* Time */
    obs_time = atoi(argv[1]);

    /* First assimilation cycle flag */
    first_cycle = atoi(argv[2]);

    /*
     * Read and initialize some PIHM structures for observation operators
     */
    pihm = (pihm_struct)malloc(sizeof(*pihm));

    BuildPIHM(pihm_dir, project, pihm);

    /*
     * Read variable, parameter, and observation tables
     */
    ReadVarTbl(vartbl_fn, nelem, nriver, vartbl);

    ReadParamTbl(paramtbl_fn, paramtbl);

    ReadObsTbl(obstbl_fn, obstbl);

    /*
     * Read variables and parameters
     */
    ReadEnsVar(pihm_dir, project, output_dir, obs_time, vartbl, &ens);

    ReadEnsParam(pihm_dir, project, paramtbl, &ens);

    /*
     * Create observation operators
     */
    ObsOper(obstbl, vartbl, pihm);

    /*
     * Data assimilation
     */
    Assim(pihm_dir, output_dir, paramtbl, vartbl, obstbl, inflt_weight,
        obs_time, first_cycle, &ens);

    /*
     * Adjust water balance
     */
    WaterBal(paramtbl, vartbl, pihm, &ens);

    /*
     * Write output files
     */
    WriteParamOutput(pihm_dir, output_dir, paramtbl, obs_time, ASSIM, &ens);

    WriteVarOutput(pihm_dir, output_dir, project, vartbl, obs_time, &ens);

    WriteInit(pihm_dir, project, vartbl, &ens);

    //WriteCalFile(pihm_dir, project, paramtbl, &ens);

    /*
     * Free memory
     */
    FreeObsOper(obstbl);
    FreeEnsMbr(vartbl, &ens);

    FreeRivtbl(&pihm->rivtbl);
    FreeShptbl(&pihm->shptbl);
    FreeMatltbl(&pihm->matltbl);
    FreeMeshtbl(&pihm->meshtbl);
    FreeAtttbl(&pihm->atttbl);
    FreeSoiltbl(&pihm->soiltbl);
    free(pihm->elem);
    free(pihm->river);
    free(pihm);

    return EXIT_SUCCESS;
}

void Assim(const char *pihm_dir, const char *output_dir,
    const paramtbl_struct *paramtbl, const vartbl_struct *vartbl,
    const obstbl_struct *obstbl, double inflt_weight, int t, int first_cycle,
    ens_struct *ens)
{
    double          xf[MAXNE + 1];
    int             i, j;
    int             ne;
    char            obs_out_fn[MAXSTRING];
    FILE           *obs_fp;
    double          obs;
    double          obs_error;
    ens_struct      prior;
    pihm_t_struct   pihm_time;

    pihm_time = PIHMTime(t);

    ne = ens->ne;

    /* Save prior parameters and states for inflation */
    SavePrior(ens, vartbl, &prior);

    printf("\n  Starting EnKF ... \n");

    /*
     * Assimilate observations using sequential processing
     */
    if (obstbl[0].name[0] != '\0')
    {
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

            printf("\n  # %s\n", obstbl[i].name);

            /* Read observations */
            ReadObs(t, obstbl[i].fname, &obs, &obs_error);
            printf("    observed value = %lf (error: %lf)\n", obs, obs_error);

            /* Read ensemble forecasts */
            Forecast(ens, vartbl, &obstbl[i], xf);
            printf("    prediction = ");
            for (j = 0; j < ne; j++)
            {
                printf("%f\t", xf[j]);
            }
            printf("mean: %f\n", xf[ne]);

            /* Write observations to files */
            fprintf(obs_fp, "\t%lf", obs);

            /* EnKF analysis */
            Anlys(paramtbl, vartbl, obs, obs_error, xf, ens);
        }

        fprintf(obs_fp, "\n");
        fflush (obs_fp);
        fclose (obs_fp);

        /* Covariance inflation */
        CovInflt(paramtbl, vartbl, inflt_weight, &prior, ens);
    }

    FreeEnsMbr(vartbl, &prior);
}

void SavePrior(const ens_struct *ens, const vartbl_struct *vartbl,
    ens_struct *prior)
{
    int             i;

    prior->ne = ens->ne;

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ens->ne; i++)
    {
        int             j, k;

        for (k = 0; k < MAXVAR; k++)
        {
            if (vartbl[k].dim > 0)
            {
                prior->member[i].var[k] =
                    (double *)malloc(vartbl[k].dim * sizeof(double));

                for (j = 0; j < vartbl[k].dim; j++)
                {
                    prior->member[i].var[k][j] = ens->member[i].var[k][j];
                }
            }
        }

        for (k = 0; k < MAXPARAM; k++)
        {
            prior->member[i].param[k] = ens->member[i].param[k];
        }
    }
}

void FreeEnsMbr(const vartbl_struct *vartbl, ens_struct *ens)
{
    int             i;

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ens->ne; i++)
    {
        int             k;

        for (k = 0; k < MAXVAR; k++)
        {
            if (vartbl[k].dim > 0)
            {
                free(ens->member[i].var[k]);
            }
        }
    }
}
