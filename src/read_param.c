#include "enkf.h"

void ReadParamTbl(const char *fname, param_struct *paramtbl)
{
    char            cmdstr[MAXSTRING];
    FILE           *fid;
    int             i;
    int             lno = 0;

    /* Initial parameter table */
    for (i = 0; i < MAXPARAM; i++)
    {
        paramtbl[i].perturb = 0;
        paramtbl[i].update = 0;
        paramtbl[i].type = 0;
    }

    /* Open parameter table */
    fid = fopen (fname, "r");
    CheckFile(fid, fname);

    /* Header line */
    FindLine(fid, "PARAMETER", &lno, fname);

    /* Start reading parameter table */
    for (i = 0; i < MAXPARAM; i++)
    {
        NextLine(fid, cmdstr, &lno);
        if (strcasecmp(cmdstr, "EOF") == 0)
        {
            break;
        }
        else
        {
            if (sscanf(cmdstr, "%s %d %d %lf %lf %lf %lf %d",
                paramtbl[i].name, &paramtbl[i].perturb, &paramtbl[i].update,
                &paramtbl[i].perturb_min, &paramtbl[i].perturb_max,
                &paramtbl[i].min, &paramtbl[i].max, &paramtbl[i].type) != 8)
            {
                printf("Parameter table format error.\n");
                exit(EXIT_FAILURE);
            }

            /* Convert parameters that should be perturbed in log space */
            if (paramtbl[i].type == 1)
            {
                paramtbl[i].min = log10(paramtbl[i].min);
                paramtbl[i].max = log10(paramtbl[i].max);
                paramtbl[i].perturb_min = log10(paramtbl[i].perturb_min);
                paramtbl[i].perturb_max = log10(paramtbl[i].perturb_max);
            }
        }
    }

    fclose(fid);
}
