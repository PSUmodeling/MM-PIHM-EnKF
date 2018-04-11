#include "enkf.h"

void WriteParamOutput(const char *timestr, const char *pihm_dir,
    const char *output_dir, const param_struct *paramtbl, ens_struct *ens)
{
    char            fn[MAXSTRING];
    FILE           *fid;
    int             i, j;

    for (i = 0; i < MAXPARAM; i++)
    {
        if (1 == paramtbl[i].perturb)
        {
            sprintf(fn, "%s/output/%s/%s.txt",
                pihm_dir, output_dir, paramtbl[i].name);
            fid = fopen(fn, "w");
            CheckFile(fid, fn);

            fprintf(fid, "\"%s\"", timestr);
            for (j = 0; j < ens->ne; j++)
            {
                fprintf (fid, "\t%lf", ens->member[j].param[i]);
            }
            fprintf (fid, "\n");
            fflush(fid);
            fclose(fid);
        }
    }
}

void WriteCalFile(const ens_struct *ens, const char *project,
    const char *pihm_dir)
{
    char            fn[MAXSTRING];
    FILE           *fid;
    int             i;

    for (i = 0; i < ens->ne; i++)
    {
        sprintf(fn, "%s/input/%s/%s.%3.3d.calib",
            pihm_dir, project, project, i + 1);

        fid = fopen(fn, "w");
        CheckFile(fid, fn);

        fprintf(fid, "%-16s%-.3lf\n",
            "KSATH", ens->member[i].param[KSATH]);
        fprintf(fid, "%-16s%-.3lf\n",
            "KSATV", ens->member[i].param[KSATV]);
        fprintf(fid, "%-16s%-.3lf\n",
            "KINF", ens->member[i].param[KINF]);
        fprintf(fid, "%-16s%-.3lf\n",
            "KMACSATH", ens->member[i].param[KMACSATH]);
        fprintf(fid, "%-16s%-.3lf\n",
            "KMACSATV", ens->member[i].param[KMACSATV]);
        fprintf(fid, "%-16s%-.3lf\n",
            "DINF", ens->member[i].param[DINF]);
        fprintf(fid, "%-16s%-.3lf\n",
            "DROOT", ens->member[i].param[DROOT]);
        fprintf(fid, "%-16s%-.3lf\n",
            "DMAC", ens->member[i].param[DMAC]);
        fprintf(fid, "%-16s%-.3lf\n",
            "POROSITY", ens->member[i].param[POROSITY]);
        fprintf(fid, "%-16s%-.3lf\n",
            "ALPHA", ens->member[i].param[ALPHA]);
        fprintf(fid, "%-16s%-.3lf\n",
            "BETA", ens->member[i].param[BETA]);
        fprintf(fid, "%-16s%-.3lf\n",
            "MACVF", ens->member[i].param[MACVF]);
        fprintf(fid, "%-16s%-.3lf\n",
            "MACHF", ens->member[i].param[MACHF]);
        fprintf(fid, "%-16s%-.3lf\n",
            "VEGFRAC", ens->member[i].param[VEGFRAC]);
        fprintf(fid, "%-16s%-.3lf\n",
            "ALBEDO", ens->member[i].param[ALBEDO]);
        fprintf(fid, "%-16s%-.3lf\n",
            "ROUGH", ens->member[i].param[ROUGH]);
        fprintf(fid, "%-16s%-.3lf\n",
            "EC", ens->member[i].param[EC]);
        fprintf(fid, "%-16s%-.3lf\n",
            "ETT", ens->member[i].param[ETT]);
        fprintf(fid, "%-16s%-.3lf\n",
            "EDIR", ens->member[i].param[EDIR]);
        fprintf(fid, "%-16s%-.3lf\n",
            "ROUGH_RIV", ens->member[i].param[ROUGH_RIV]);
        fprintf(fid, "%-16s%-.3lf\n",
            "KRIVH", ens->member[i].param[KRIVH]);
        fprintf(fid, "%-16s%-.3lf\n",
            "KRIVV", ens->member[i].param[KRIVV]);
        fprintf(fid, "%-16s%-.3lf\n",
            "BEDTHCK", ens->member[i].param[BEDTHCK]);
        fprintf(fid, "%-16s%-.3lf\n",
            "RIV_DPTH", ens->member[i].param[RIV_DPTH]);
        fprintf(fid, "%-16s%-.3lf\n",
            "RIV_WDTH", ens->member[i].param[RIV_WDTH]);

        fprintf(fid, "%-s", "\nLSM_CALIBRATION\n");
        fprintf(fid, "%-16s%-.3lf\n",
            "DRIP", ens->member[i].param[DRIP]);
        fprintf(fid, "%-16s%-.3lf\n",
            "CMCMAX", ens->member[i].param[CMCMAX]);
        fprintf(fid, "%-16s%-.3lf\n",
            "RS", ens->member[i].param[RS]);
        fprintf(fid, "%-16s%-.3lf\n",
            "CZIL", ens->member[i].param[CZIL]);
        fprintf(fid, "%-16s%-.3lf\n",
            "FXEXP", ens->member[i].param[FXEXP]);
        fprintf(fid, "%-16s%-.3lf\n",
            "CFACTR", ens->member[i].param[CFACTR]);
        fprintf(fid, "%-16s%-.3lf\n",
            "RGL", ens->member[i].param[RGL]);
        fprintf(fid, "%-16s%-.3lf\n",
            "HS", ens->member[i].param[HS]);
        fprintf(fid, "%-16s%-.3lf\n",
            "REFSMC", ens->member[i].param[REFSMC]);
        fprintf(fid, "%-16s%-.3lf\n",
            "WLTSMC", ens->member[i].param[WLTSMC]);

        fprintf(fid, "%-s", "\nSCENARIO\n");
        fprintf(fid, "%-16s%-.3lf\n",
            "PRCP", ens->member[i].param[PRCP]);
        fprintf(fid, "%-16s%-.3lf\n",
            "SFCTMP", ens->member[i].param[SFCTMP]);

        fflush (fid);
        fclose (fid);
    }
}
