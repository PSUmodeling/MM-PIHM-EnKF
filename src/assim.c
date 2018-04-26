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
    int             nelem, nriver;
    int             obs_time;
    int             first_cycle;
    pihm_struct     pihm;
    vartbl_struct   vartbl[MAXVAR];
    paramtbl_struct paramtbl[MAXPARAM];
    obstbl_struct   obstbl[MAXOBS];
    double          inflt_weight;
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

    /* Covariance relaxation weight */
    inflt_weight = atof(argv[12]);

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
     * Read and initialize some PIHM structures for observation operators
     */
    pihm = (pihm_struct)malloc(sizeof(*pihm));

    pihm->cal = (calib_struct){
    .ksath = 1.0, .ksatv = 1.0, .kinfv = 1.0, .kmach = 1.0, .kmacv = 1.0,
    .dinf = 1.0, .rzd = 1.0, .dmac = 1.0,
    .porosity = 1.0, .alpha = 1.0, .beta = 1.0,
    .areafv = 1.0, .areafh = 1.0,
    .vegfrac = 1.0, .albedo = 1.0, .rough = 1.0,
    .ec = 1.0, .ett = 1.0, .edir = 1.0,
    .rivrough = 1.0, .rivksath = 1.0, .rivksatv = 1.0,
    .rivbedthick = 1.0, .rivdepth = 1.0, .rivshpcoeff = 1.0,
    .prcp = 1.0, .sfctmp = 0.0,
    .smcref = 1.0, .smcwlt = 1.0,
    .rsmin = 1.0,
    .drip = 1.0, .cmcmax = 1.0,
    .czil = 1.0,
    .fxexp = 1.0, .cfactr = 1.0,
    .rgl = 1.0, .hs = 1.0};

    BuildPIHM(pihm_dir, project, pihm);

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

    WriteCalFile(pihm_dir, project, paramtbl, &ens);

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

    SavePrior(ens, vartbl, &prior);

    printf("\nStarting EnKF ... \n");

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

            fprintf(obs_fp, "\t%lf", obs);

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
