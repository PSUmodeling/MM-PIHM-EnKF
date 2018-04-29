#include "enkf.h"

void ObsOper(obstbl_struct *obstbl, const vartbl_struct *vartbl,
    const pihm_struct pihm)
{
    int             i;

    for (i = 0; i < MAXOBS; i++)
    {
        if (obstbl[i].name[0] != '\0')
        {
            if (strcasecmp(obstbl[i].name, "discharge") == 0)
            {
                DischOper(&obstbl[i], vartbl, pihm);
            }
            else if (strcasecmp(obstbl[i].name, "skin_temperature") == 0)
            {
                LandSfcTmpOper(&obstbl[i], vartbl, pihm);
            }
            else if (strcasecmp(obstbl[i].name, "COSMOS") == 0)
            {
                COSMOSOper(&obstbl[i], vartbl, pihm);
            }
            else
            {
                printf("Observation operator for %s is not supported.\n",
                    obstbl[i].name);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            break;
        }
    }
}

void DischOper(obstbl_struct *obs, const vartbl_struct *vartbl,
    const pihm_struct pihm)
{
    double          dist;
    double          dist_min = 999999999.9;
    int             ind_min;
    int             i;

    obs->var_ind = (int *)malloc(sizeof(int));
    obs->var_ind[0] = FindVar("rivflx1", vartbl);

    obs->dim = nriver;
    obs->nlyr = 1;

    ObsOperAlloc(obs);

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < nriver; i++)
    {
        obs->weight[i] = 0.0;
        obs->k[i][0] = 0.0;
        obs->b[i][0] = 0.0;
    }

    for (i = 0; i < nriver; i++)
    {
        dist = Dist(pihm->river[i].topo.x, pihm->river[i].topo.y,
            obs->x, obs->y);
        if (dist < dist_min)
        {
            ind_min = i;
            dist_min = dist;
        }
    }

    /* Use weight to convert unit */
    obs->weight[ind_min] = 1.0;
    obs->k[ind_min][0] = 1.0;
}

void LandSfcTmpOper(obstbl_struct *obs, const vartbl_struct *vartbl,
    const pihm_struct pihm)
{
    obs->var_ind = (int *)malloc(sizeof(int));
    obs->var_ind[0] = FindVar("t1", vartbl);

    obs->dim = nelem;
    obs->nlyr = 1;

    ObsOperAlloc(obs);

    WeightByArea(pihm->elem, obs);
}

void COSMOSOper(obstbl_struct *obs, const vartbl_struct *vartbl,
    const pihm_struct pihm)
{
    obs->var_ind = (int *)malloc(sizeof(int));
    obs->var_ind[0] = FindVar("smc0", vartbl);

    obs->dim = nelem;
    obs->nlyr = 1;

    ObsOperAlloc(obs);

    WeightByArea(pihm->elem, obs);
}

int FindVar(const char *var_name, const vartbl_struct *vartbl)
{
    int             i;

    for (i = 0; i < MAXVAR; i++)
    {
        if (strcasecmp(var_name, vartbl[i].name) == 0)
        {
            return i;
        }
    }

    return -1;
}

double Dist(double x1, double y1, double x2, double y2)
{
    return (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1- y2)));
}

void ObsOperAlloc(obstbl_struct *obs)
{
    int             i;

    obs->weight = (double *)malloc(obs->dim * sizeof(double));

    obs->k = (double **)malloc(obs->dim * sizeof(double));
    obs->b = (double **)malloc(obs->dim * sizeof(double));
#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < obs->dim; i++)
    {
        obs->k[i] = (double *)malloc(obs->nlyr * sizeof(double));
        obs->b[i] = (double *)malloc(obs->nlyr * sizeof(double));
    }
}

void WeightByArea(const elem_struct *elem, obstbl_struct *obs)
{
    double          dist;
    double          total_area = 0.0;
    int             i;

    for (i = 0; i < nelem; i++)
    {
        dist = Dist(elem[i].topo.x, elem[i].topo.y, obs->x, obs->y);

        if (dist < obs->radius)
        {
            obs->weight[i] = elem[i].topo.area;
            obs->k[i][0] = 1.0;
            obs->b[i][0] = 0.0;

            total_area += elem[i].topo.area;
        }
        else
        {
            obs->weight[i] = 0.0;
            obs->k[i][0] = 0.0;
            obs->b[i][0] = 0.0;
        }
    }

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < nelem; i++)
    {
        obs->weight[i] /= total_area;
    }
}

void FreeObsOper(obstbl_struct *obstbl)
{
    int             i;

    for (i = 0; i < MAXOBS; i++)
    {
        int             k;

        if (obstbl[i].name[0] != '\0')
        {
            free(obstbl[i].var_ind);
            free(obstbl[i].weight);
            for (k = 0; k < obstbl[i].dim; k++)
            {
                free(obstbl[i].k[k]);
                free(obstbl[i].b[k]);
            }
            free(obstbl[i].k);
            free(obstbl[i].b);
        }
    }
}

