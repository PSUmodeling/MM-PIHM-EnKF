#include "enkf.h"

void ReadCalib(const char *filename, double *dflt_val)
{
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];
    double          val;
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
            for (i = 0; i < MAXPARAM; i++)
            {
                if (strcasecmp(optstr, PARAM_NAMES[i]) == 0)
                {
                    dflt_val[i] = val;
                    break;
                }
            }
        }
    }

    fclose(calib_fp);
}

