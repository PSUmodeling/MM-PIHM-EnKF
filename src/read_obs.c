#include "enkf.h"

void ReadObs(int t, const char *fname, double *obs, double *obs_error)
{
    FILE           *fp;
    char            cmdstr[MAXSTRING];
    char            str1[MAXSTRING], str2[MAXSTRING];
    char            timestr[MAXSTRING];
    int             lno = 0;

    fp = fopen(fname, "r");
    CheckFile (fp, fname);

    FindLine (fp, "BOF", &lno, fname);

    while (1)
    {
        NextLine (fp, cmdstr, &lno);

        if (strcasecmp (cmdstr, "EOF") == 0)
        {
            printf("\nError: No observation available at %d in %s.\n",
                t, fname);
            exit(EXIT_FAILURE);
        }
        else if (sscanf(cmdstr, "%s %s %lf %lf", str1, str2, obs, obs_error) !=
            4)
        {
            printf("Error: observation file format error.");
            exit(EXIT_FAILURE);
        }

        sprintf(timestr, "%s %s", str1, str2);

        if (t == StrTime(timestr))
        {
            break;
        }
    }

    fclose (fp);
}
