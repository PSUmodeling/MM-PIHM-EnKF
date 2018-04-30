#include "enkf.h"

void CovInflt(const paramtbl_struct *paramtbl, const vartbl_struct *vartbl,
    double weight, const ens_struct *prior, ens_struct *ens)
{
    int             k;
    int             ne;
    ne = ens->ne;

    printf("\n  # Parameters\n");

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (k = 0; k < MAXPARAM; k++)
    {
        int             i;
        double          thr_std;
        double          xf[MAXNE + 1], xa[MAXNE + 1];

        if (1 == paramtbl[k].update)
        {
            xa[ne] = 0.0;
            xf[ne] = 0.0;
            for (i = 0; i < ne; i++)
            {
                xa[i] = ens->member[i].param[k];
                xf[i] = prior->member[i].param[k];

                /* Take log if necessary */
                xa[i] = (1 == paramtbl[k].type) ? log10(xa[i]) : xa[i];
                xf[i] = (1 == paramtbl[k].type) ? log10(xf[i]) : xf[i];

                xa[ne] += xa[i];
                xf[ne] += xf[i];
            }

            xa[ne] /= (double)ne;
            xf[ne] /= (double)ne;

            thr_std = CCI_THR * INIT_STD * (paramtbl[k].max - paramtbl[k].min);

            if (xa[ne] < paramtbl[k].max - thr_std &&
                xa[ne] > paramtbl[k].min + thr_std)
            {
                /* Covariance relaxation */
                CovRelax(weight, ne, xf, xa);

                /* Conditional covariance inflation */
                CondCovInflt(thr_std, ne, xa);

                PlsblRngCstr(paramtbl[k].max, paramtbl[k].min, ne, xa);

                for (i = 0; i < ne; i++)
                {
                    if (1 == paramtbl[k].type)
                    {
                        xa[i] = pow(10.0, xa[i]);
                    }

                    ens->member[i].param[k] = xa[i];
                }

                if (1 == paramtbl[k].type)
                {
                    xa[ne] = pow(10.0, xa[ne]);
                }
                printf("    %s\tmean: %lf\n", paramtbl[k].name, xa[ne]);
            }
            else
            {
                printf("    %s\tEnKF update %lf is out of range "
                    "and is rejected.\n", paramtbl[k].name, xa[i]);
                for (i = 0; i < ne; i++)
                {
                    xa[i] = xf[i];
                    if (1 == paramtbl[k].type)
                    {
                        xa[i] = pow(10.0, xf[i]);
                    }

                    ens->member[i].param[k] = xa[i];
                }
            }
        }
    }

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (k = 0; k < MAXVAR; k++)
    {
        int             i, j;
        double          xf[MAXNE + 1], xa[MAXNE + 1];

        if (vartbl[k].dim > 0)
        {
            for (j = 0; j < vartbl[k].dim; j++)
            {
                xa[ne] = 0.0;
                xf[ne] = 0.0;
                for (i = 0; i < ne; i++)
                {
                    xa[i] = ens->member[i].var[k][j];
                    xf[i] = prior->member[i].var[k][j];
                    xa[ne] += xa[i];
                    xf[ne] += xf[i];
                }

                xa[ne] /= (double)ne;
                xf[ne] /= (double)ne;

                CovRelax(weight, ne, xf, xa);

                for (i = 0; i < ne; i++)
                {
                    if (roundi(vartbl[k].max) != BADVAL)
                    {
                        xa[i] = (xa[i] < vartbl[k].max) ? xa[i] : vartbl[k].max;
                    }

                    if (roundi(vartbl[k].min) != BADVAL)
                    {
                        xa[i] = (xa[i] > vartbl[k].min) ? xa[i] : vartbl[k].min;
                    }

                    ens->member[i].var[k][j] = xa[i];
                }
            }
        }
    }

    printf("\n  Covariance inflation done.\n");
}

void CovRelax(double weight, int ne, const double *xf, double *xa)
{
    /*
     * Covariance relaxation (Zhang et al. 2004 MWR)
     * After the state variables and model parameters are updated by EnKF, the
     * analysis error covariance is inflated using a weighted average between
     * the prior perturbation and the posterior perturbation.
     */
    int             i;
    double          xap, xfp;

    for (i = 0; i < ne; i++)
    {
        xfp = xf[i] - xf[ne];
        xap = xa[i] - xa[ne];

        xap = (1.0 - weight) * xap + weight * xfp;

        xa[i] = xa[ne] + xap;
    }
}

void CondCovInflt(double thr_std, int ne, double *xa)
{
    /*
     * Conditional covariance inflation (Aksoy et al. 2006 MWR)
     * The posterior standard deviation of model parameter is inflated back to a
     * predefined threshold when the standard deviation is smaller than the
     * threshold.
     */
    double          std = 0.0;
    int             i;

    for (i = 0; i < ne; i++)
    {
        std += (xa[i] - xa[ne]) * (xa[i] - xa[ne]);
    }
    std = sqrt(std / ((double)ne - 1.0));

    if (std < thr_std)
    {
        for (i = 0; i < ne; i++)
        {
            xa[i] = thr_std / std * (xa[i] - xa[ne]) + xa[ne];
        }
    }
}

void PlsblRngCstr(double max_range, double min_range, int ne, double *xa)
{
    /*
     * Find maximum and minimum parameter values and test if they
     * are bounded within the parameter ranges
     */
    double          param_min = 999.0;
    double          param_max = -999.0;
    double          c1, c2, c;
    int             i;

    for (i = 0; i < ne; i++)
    {
        param_min = (param_min < xa[i]) ? param_min: xa[i];
        param_max = (param_max > xa[i]) ? param_max: xa[i];
    }

    c1 = (max_range - EPSILON - xa[ne]) / (param_max - xa[ne]);
    c2 = (xa[ne] - min_range - EPSILON) / (xa[ne] - param_min);
    c = (c1 < c2) ? c1 : c2;
    c = (c < 1.0) ? c : 1.0;

    for (i = 0; i < ne; i++)
    {
        xa[i] = c * (xa[i] - xa[ne]) + xa[ne];
    }
}
