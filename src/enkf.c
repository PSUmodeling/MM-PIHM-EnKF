#include "enkf.h"

void Anlys(const paramtbl_struct *paramtbl, const vartbl_struct *vartbl,
    double obs, double obs_error, const double *xf, ens_struct *ens)
{
    int             k;
    int             ne;

    ne = ens->ne;

    /*
     * Optimize parameters using EnKF
     */
#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (k = 0; k < MAXPARAM; k++)
    {
        int             i;
        double          xa[MAXNE + 1];

        if (1 == paramtbl[k].update)
        {
            xa[ne] = 0.0;
            for (i = 0; i < ne; i++)
            {
                xa[i] = ens->member[i].param[k];
                xa[i] = (1 == paramtbl[k].type) ? log10(xa[i]) : xa[i];
                xa[ne] += xa[i];
            }
            xa[ne] /= (double)ne;

            EnKF(xa, obs, obs_error, xf, ne);

            for (i = 0; i < ne; i++)
            {
                xa[i] = (1 == paramtbl[k].type) ? pow(10.0, xa[i]) : xa[i];
                ens->member[i].param[k] = xa[i];
            }
        }
    }

    /*
     * Optimize model variables
     */
#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (k = 0; k < MAXVAR; k++)
    {
        int             j;

        if (vartbl[k].dim > 0)
        {
            for (j = 0; j < vartbl[k].dim; j++)
            {
                int             i;
                double          xa[MAXNE + 1];

                xa[ne] = 0.0;
                for (i = 0; i < ne; i++)
                {
                    xa[i] = ens->member[i].var[k][j];
                    xa[ne] += xa[i];
                }
                xa[ne] /= (double)ne;

                EnKF(xa, obs, obs_error, xf, ne);

                for (i = 0; i < ne; i++)
                {
                    ens->member[i].var[k][j] = xa[i];
                }
            }
        }
    }
}

void EnKF(double *xa, double obs, double obs_err, const double *xf, int ne)
{
    int             i;
    double          y_hxm;
    double          km;
    double          hxa[MAXNE];
    double          xp[MAXNE];
    double          var;
    double          fac;
    double          s;
    double          beta;
    double          fac2;

    for (i = 0; i < ne; i++)
    {
        xa[i] -= xa[ne];
    }

    y_hxm = obs - xf[ne];

    var = 0.0;
    for (i = 0; i < ne; i++)
    {
        hxa[i] = xf[i] - xf[ne];
        var += hxa[i] * hxa[i];
    }

    fac = 1.0 / ((double)ne - 1.0);
    s = fac * var + obs_err * obs_err;
    beta = 1.0 / (1.0 + sqrt((obs_err * obs_err) / s));
    fac2 = fac / s;

    km = 0.0;
    for (i = 0; i < ne; i++)
    {
        xp[i] = xa[i];
        km += xp[i] * hxa[i];
    }

    km *= fac2;

    xa[ne] += km * y_hxm;

    km *= beta;

    for (i = 0; i < ne; i++)
    {
        xa[i] += km * (0.0 - hxa[i]);
        xa[i] += xa[ne];
    }
}
