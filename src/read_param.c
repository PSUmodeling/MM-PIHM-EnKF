#include "enkf.h"

void ReadCalib(const char *filename, const paramtbl_struct *paramtbl,
    double *param)
{
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    double          val;
    int             ind;
    FILE           *calib_fp;
    int             lno = 0;
    int             i;

    calib_fp = fopen(filename, "r");
    CheckFile(calib_fp, filename);

    while (1)
    {
        NextLine(calib_fp, cmdstr, &lno);
        if (strcasecmp(cmdstr, "EOF") == 0)
        {
            break;
        }

        sscanf(cmdstr, "%s", optstr);

        if (strcasecmp(optstr, "LSM_CALIBRATION") == 0 ||
            strcasecmp(optstr, "SCENARIO") == 0)
        {
            continue;
        }
        else
        {
            sscanf(cmdstr, "%*s %lf", &val);

            ind = FindParam(optstr, paramtbl);
            if (ind < 0)
            {
                printf("Error: %s is not listed in parameter table.\n", optstr);
                exit(EXIT_FAILURE);
            }
            else
            {
                param[ind] = val;
            }
        }
    }

    fclose(calib_fp);
}

int FindParam(const char *param_name, const paramtbl_struct *paramtbl)
{
    int             i;

    for (i = 0; i < MAXPARAM; i++)
    {
        if (strcasecmp(param_name, paramtbl[i].name) == 0)
        {
            return i;
        }
    }

    return -1;
}
