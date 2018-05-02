#include "enkf.h"

int                 debug_mode;
int                 verbose_mode;

int main(int argc, char *argv[])
{
    char            paramtbl_fn[MAXSTRING];
    char            calib_fn[MAXSTRING];
    char            pihm_dir[MAXSTRING];
    char            output_dir[MAXSTRING];
    char            project[MAXSTRING];
    double          dflt_val[MAXPARAM];
    int             t;
    int             ind[MAXPARAM];
    paramtbl_struct paramtbl[MAXPARAM];
    int             perturb_mode;
    int             i;
    int             counter;
    double        **randnum;
    ens_struct      ens;

    if (argc != 2)
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

    /* Perturbation mode */
    perturb_mode = atoi(getenv("PERTURB_MODE"));

    /* Number of ensemble member */
    ens.ne = atoi(getenv("NUM_MEMBER"));

    /* Time */
    t = atoi(argv[1]);

    /*
     * Read parameter table
     */
    ReadParamTbl(paramtbl_fn, paramtbl);

    /*
     * Read default parameter values and assign to ensemble
     */
    sprintf(calib_fn, "%s/input/%s/%s.calib", pihm_dir, project, project);
    ReadParam(calib_fn, paramtbl, dflt_val);

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ens.ne; i++)
    {
        int             j;

        for (j = 0; j < MAXPARAM; j++)
        {
            ens.member[i].param[j] = dflt_val[j];
        }
    }

    /* Count number of parameters that need to be perturbed */
    counter = 0;
    for (i = 0; i < MAXPARAM; i++)
    {
        if (1 == paramtbl[i].perturb)
        {
            ind[counter] = i;
            counter++;
        }
    }

    /*
     * Generate perturbed parameters
     */
    randnum = (double **)malloc(counter * sizeof(double *));
    for (i = 0; i < counter; i++)
    {
        randnum[i] = (double *)malloc(ens.ne * sizeof(double));
    }

    /* Generate random numbers with means of 0.0 and standard deviations of 1.0.
     *
     * The initial standard deviation (sigma) of perturbed parameter is INIT_STD
     * of parameter range. The allowed range for generated random number is
     * therefore
     * [(-0.5 / INIT_STD - 0.5 * abs(perturb_mode) / sigma,
     * (0.5 / INIT_STD - 0.5 * abs(perturb_mode) / sigma] */
    GenRandNum(ens.ne, counter, randnum,
        0.5 / INIT_STD - 0.5 * fabs((double)perturb_mode));

    /* Assign random numbers to parameter values */
    printf("\n  Initial parameters\n");

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < counter; i++)
    {
        int             j;
        double          prior_std;
        double          prior;
        double         *x[MAXNE];

        for (j = 0; j < ens.ne; j++)
        {
            x[j] = &ens.member[j].param[ind[i]];
        }

        prior_std = INIT_STD * (paramtbl[ind[i]].max - paramtbl[ind[i]].min);
        prior = (paramtbl[ind[i]].min + paramtbl[ind[i]].max) / 2.0 +
            0.5 * (double)perturb_mode * prior_std;

        for (j = 0; j < ens.ne; j++)
        {
            /* Adjust means and standard deviations */
            *x[j] = prior + prior_std * randnum[i][j];

            *x[j] = (paramtbl[ind[i]].type == 1) ? pow(10.0, *x[j]) : *x[j];
        }

        prior = (paramtbl[ind[i]].type == 1) ? pow(10.0, prior) : prior;

        /* Output to screen */
        printf("    %s: mean %-.3lf, initial std %.3lf\n",
            paramtbl[ind[i]].name, prior, prior_std);
        fflush(stdout);
    }

    /*
     * Write parameter output files and calibration files
     */
    WriteParamOutput(pihm_dir, output_dir, paramtbl, t, PERTURB, &ens);

    WriteCalFile(pihm_dir, project, paramtbl, &ens);

    for (i = 0; i < counter; i++)
    {
        free(randnum[i]);
    }
    free(randnum);

    return EXIT_SUCCESS;
}

void Randn(int n, double range, double *x)
{
    /*
     * Random number generator with Gaussian distribution
     */
    int             i;
    double          mean = 0.0;
    double          std = 0.0;
    const double    TAIL = 0.05;

    for (i = 0; i < n; i++)
    {
        x[i] =
            TAIL + ((double)(i + 1) * (1.0 - 2.0 * TAIL)) / ((double)(n + 1));
        x[i] = ltqnorm(x[i]);
        mean += x[i];
    }

    mean /= (double)n;

    /* Calcualte standard deviation of generated numbers */
    for (i = 0; i < n; i++)
    {
        std += (x[i] - mean) * (x[i] - mean);
    }
    std = sqrt(std / ((double)n - 1.0));

    /* Adjust generated random numbers so that they have means of
     * zero and standard deviations of 1.0 */
    for (i = 0; i < n; i++)
    {
        x[i] = (x[i] - mean) / std * 1.0;

        if (x[i] <= -range || x[i] >= range)
        {
            printf ("Error: cannot generate normal distribution "
                "within given range.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void Shuffle(const double *seed, int n, double *x)
{
    double          t;
    int             i, j;

    if (n > 1)
    {
        for (i = 0; i < n; i++)
        {
            x[i] = seed[i];
        }

        for (i = 0; i < n - 1; i++)
        {
            j = i + rand() / (RAND_MAX / (n - i) + 1);
            t = x[j];
            x[j] = x[i];
            x[i] = t;
        }
    }
}

void GenRandNum(int ne, int nparam, double **randnum, double range)
{
    /*
     * Function GenRandNum will generate groups of random numbers within
     * (-range, range). The correlation between groups should be smaller that
     * specified value to make sure groups are perturbed independently
     */
    int             i, j, k;
    double          corr;
    double          norm[MAXNE];
    int             corr_flag;
    double          s1, s2;
    double          max = -999.0;

    /* Initialize correlation flag */
    corr_flag = (nparam > 1) ? 1 : 0;

    /* Random number seed */
    srand(time(NULL));

    /* Create normally distributed random numbers within specified range */
    Randn(ne, range, norm);

    do
    {
        max = -999.0;

        /* Shuffle generated random numbers */
        for (j = 0; j < nparam; j++)
        {
            Shuffle(norm, ne, randnum[j]);
        }

        /* Check correlations between groups of random numbers */
        for (i = 0; i < nparam; i++)
        {
            for (j = 0; j < nparam; j++)
            {
                if (i == j)
                {
                    continue;
                }

                corr = 0.0;
                s1 = 0.0;
                s2 = 0.0;

                for (k = 0; k < ne; k++)
                {
                    corr += randnum[i][k] * randnum[j][k];
                    s1 += randnum[i][k] * randnum[i][k];
                    s2 += randnum[j][k] * randnum[j][k];
                }

                corr /= sqrt(s1 * s2);

                max = (fabs(corr) > max) ? fabs(corr) : max;
            }
        }

        if (max < CORRMAX)
        {
            corr_flag = 0;
        }
    } while (corr_flag);

    printf("  Max correlation between paramters is %lf\n", max);
    fflush(stdout);
}
