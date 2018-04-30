#include "enkf.h"

void WriteParamOutput(const char *pihm_dir, const char *output_dir,
    const paramtbl_struct *paramtbl, int t, int flag, ens_struct *ens)
{
    int             i;
    pihm_t_struct   pihm_time;

    pihm_time = PIHMTime(t);

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < MAXPARAM; i++)
    {
        char            fn[MAXSTRING];
        FILE           *fp;
        int             j;

        if ((1 == paramtbl[i].perturb && PERTURB == flag) ||
            (1 == paramtbl[i].update && ASSIM == flag))
        {
            sprintf(fn, "%s/output/%s/%s.txt",
                pihm_dir, output_dir, paramtbl[i].name);
            if (PERTURB == flag)
            {
                fp = fopen(fn, "w");
            }
            else
            {
                fp = fopen(fn, "a");
            }
            CheckFile(fp, fn);

            fprintf(fp, "\"%s\"", pihm_time.str);
            for (j = 0; j < ens->ne; j++)
            {
                fprintf (fp, "\t%lf", ens->member[j].param[i]);
            }
            fprintf (fp, "\n");
            fflush(fp);
            fclose(fp);
        }
    }
}

void WriteCalFile(const char *pihm_dir, const char *project,
    const paramtbl_struct *paramtbl, const ens_struct *ens)
{
    int             i;
    const int       NUM_HYDROL_PARAM = 25;
    static char     HYDROL_PARAM[][MAXSTRING] = {
            "KSATH",
            "KSATV",
            "KINF",
            "KMACSATH",
            "KMACSATV",
            "DINF",
            "DROOT",
            "DMAC",
            "POROSITY",
            "ALPHA",
            "BETA",
            "MACVF",
            "MACHF",
            "VEGFRAC",
            "ALBEDO",
            "ROUGH",
            "EC",
            "ETT",
            "EDIR",
            "ROUGH_RIV",
            "KRIVH",
            "KRIVV",
            "BEDTHCK",
            "RIV_DPTH",
            "RIV_WDTH"
    };
    const int       NUM_LSM_PARAM = 10;
    static char     LSM_PARAM[][MAXSTRING] = {
            "DRIP",
            "CMCMAX",
            "RS",
            "CZIL",
            "FXEXP",
            "CFACTR",
            "RGL",
            "HS",
            "REFSMC",
            "WLTSMC"
    };
    const int       NUM_SCEN_PARAM = 2;
    static char     SCEN_PARAM[][MAXSTRING] = {
            "PRCP",
            "SFCTMP"
    };

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ens->ne; i++)
    {
        int             j;
        int             ind;
        char            fn[MAXSTRING];
        FILE           *fp;

        sprintf(fn, "%s/input/%s/%s.%3.3d.calib",
            pihm_dir, project, project, i + 1);

        fp = fopen(fn, "w");
        CheckFile(fp, fn);

        for (j = 0; j < NUM_HYDROL_PARAM; j++)
        {
            ind = FindParam(HYDROL_PARAM[j], paramtbl);
            if (ind < 0)
            {
                printf("Error: parameter %s is listed in parameter table.\n",
                    HYDROL_PARAM[j]);
            }
            else
            {
                fprintf(fp, "%-16s%-.3lf\n",
                    HYDROL_PARAM[j], ens->member[i].param[ind]);
            }
        }

        fprintf(fp, "%-s", "\nLSM_CALIBRATION\n");
        for (j = 0; j < NUM_LSM_PARAM; j++)
        {
            ind = FindParam(LSM_PARAM[j], paramtbl);
            if (ind < 0)
            {
                printf("Error: parameter %s is listed in parameter table.\n",
                    LSM_PARAM[j]);
            }
            else
            {
                fprintf(fp, "%-16s%-.3lf\n",
                    LSM_PARAM[j], ens->member[i].param[ind]);
            }
        }

        fprintf(fp, "%-s", "\nSCENARIO\n");
        for (j = 0; j < NUM_SCEN_PARAM; j++)
        {
            ind = FindParam(SCEN_PARAM[j], paramtbl);
            if (ind < 0)
            {
                printf("Error: parameter %s is listed in parameter table.\n",
                    SCEN_PARAM[j]);
            }
            else
            {
                fprintf(fp, "%-16s%-.3lf\n",
                    SCEN_PARAM[j], ens->member[i].param[ind]);
            }
        }

        fflush (fp);
        fclose (fp);
    }
}
