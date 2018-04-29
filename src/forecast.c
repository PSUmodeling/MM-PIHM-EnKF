#include "enkf.h"

void Forecast(const ens_struct *ens, const vartbl_struct *vartbl,
    const obstbl_struct *obs, double *xf)
{
    int             i;
    int             ne;

    ne = ens->ne;

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ne + 1; i++)
    {
        xf[i] = 0.0;
    }

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ne; i++)
    {
        int             j, k;
        int             ind;

        for (j = 0; j < obs->nlyr; j++)
        {
            ind = obs->var_ind[j];

            for (k = 0; k < vartbl[ind].dim; k++)
            {
                xf[i] += obs->weight[k] *
                    (ens->member[i].var[ind][k] * obs->k[k][j] + obs->b[k][j]);
            }
        }
    }

    for (i = 0; i < ne; i++)
    {
        xf[ne] += xf[i];
    }

    xf[ne] /= (double)ne;
}
