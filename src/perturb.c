#include "enkf.h"

int main(int argc, char *argv[])
{
    char            paramtbl_fn[MAXSTRING];
    char            calib_fn[MAXSTRING];
    char            pihm_dir[MAXSTRING];
    char            output_dir[MAXSTRING];
    char            project[MAXSTRING];
    char            timestr[MAXSTRING];
    double          dflt_val[MAXPARAM];
    int             ind[MAXPARAM];
    param_struct    paramtbl[MAXPARAM];
    int             perturb_mode;
    int             i;
    int             counter;
    double        **randnum;
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

    /* Perturbation mode */
    perturb_mode = atoi(argv[5]);

    /* Number of ensemble member */
    ens.ne = atoi(argv[6]);

    /* Time */
    strcpy(timestr, argv[7]);

    /*
     * Read default parameter values and assign to ensemble
     */
    sprintf(calib_fn, "%s/input/%s/%s.calib", pihm_dir, project, project);
    ReadCalib(calib_fn, dflt_val);

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

    /*
     * Read parameter table
     */
    ReadParamTbl(paramtbl_fn, paramtbl);

    /* Count number of parameters that need to be perturbed */
    counter = 0;
    for (i = 0; i < MAXPARAM; i++)
    {
        if (paramtbl[i].perturb == 1)
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
     * The initial standard deviation (sigma) of perturbed parameter is SIGMA of
     * parameter range. The allowed range for generated random number is
     * therefore
     * [(-0.5 / SIGMA - abs(perturb_mode) / sigma,
     * (0.5 / SIGMA - abs(perturb_mode) / sigma] */
    GenRandNum(ens.ne, counter, randnum,
        0.5 / SIGMA - fabs((double)perturb_mode));

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

        prior_std = SIGMA * (paramtbl[ind[i]].max - paramtbl[ind[i]].min);
        prior = (paramtbl[ind[i]].min + paramtbl[ind[i]].max) / 2.0 +
            (double)perturb_mode * prior_std;

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
    WriteParamOutput(timestr, pihm_dir, output_dir, paramtbl, &ens);

    WriteCalFile(&ens, project, pihm_dir);

    for (i = 0; i < counter; i++)
    {
        free(randnum[i]);
    }
    free(randnum);

    return EXIT_SUCCESS;
}

double Randn(void)
{
    double          temp1, temp2;
    double          x;

    temp1 = (double)rand() / RAND_MAX;
    temp2 = (double)rand() / RAND_MAX;

    x = sqrt(-2.0 * log(temp1)) * cos(2.0 * PI * temp2);

    return x;
}

/*
 * Function GenRandNum will generate groups of random numbers within
 * (-range, range). The correlation between groups should be smaller that
 * specified value to make sure groups are perturbed independently
 */
void GenRandNum(int ne, int nparam, double **randnum, double range)
{
    int             i, j, k;
    double          corr;
    double          mean;
    double          std;
    int             corr_flag;
    int             range_flag = 1;
    double          s1, s2;
    double          max = -999.0;

    /* Initialize correlation flag */
    corr_flag = (nparam > 1) ? 1 : 0;

    /* Random number seed */
    srand(time(NULL));

    do
    {
        max = -999.0;

        for (j = 0; j < nparam; j++)
        {
            range_flag = 1;

            while (range_flag)
            {
                mean = 0.0;
                std = 0.0;

                /* Generate random numbers and calculate means */
                for (i = 0; i < ne; i++)
                {
                    randnum[j][i] = Randn();
                    mean += randnum[j][i];
                }

                mean /= (double)ne;

                /* Calcualte standard deviation of generated numbers */
                for (i = 0; i < ne; i++)
                {
                    std += (randnum[j][i] - mean) * (randnum[j][i] - mean);
                }
                std = sqrt(std / ((double)ne - 1.0));

                range_flag = 0;

                /* Adjust generated random numbers so that they have means of
                 * zero and standard deviations of 1.0 */
                for (i = 0; i < ne; i++)
                {
                    randnum[j][i] = (randnum[j][i] - mean) / std * 1.0;

                    /* If any random number is out of range, re-generate */
                    if (randnum[j][i] <= -range || randnum[j][i] >= range)
                    {
                        range_flag = 1;
                        break;
                    }
                }
            }
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
