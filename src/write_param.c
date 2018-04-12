#include "enkf.h"

void WriteParamOutput(const char *timestr, const char *pihm_dir,
    const char *output_dir, const paramtbl_struct *paramtbl, ens_struct *ens)
{
    char            fn[MAXSTRING];
    FILE           *fp;
    int             i, j;

    for (i = 0; i < MAXPARAM; i++)
    {
        if (1 == paramtbl[i].perturb)
        {
            sprintf(fn, "%s/output/%s/%s.txt",
                pihm_dir, output_dir, paramtbl[i].name);
            fp = fopen(fn, "w");
            CheckFile(fp, fn);

            fprintf(fp, "\"%s\"", timestr);
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

void WriteCalFile(const ens_struct *ens, const char *project,
    const char *pihm_dir, const paramtbl_struct *paramtbl)
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
