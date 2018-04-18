#include "enkf.h"

void ReadObsTbl(const char *fname, obstbl_struct *obstbl)
{
    char            cmdstr[MAXSTRING];
    FILE           *fp;
    int             lno = 0;
    int             i;

    /* Initialize observation table */
    for (i = 0; i < MAXOBS; i++)
    {
        obstbl[i].name[0] = '\0';
    }

    /* Open observation table */
    fp = fopen(fname, "r");
    CheckFile(fp, fname);

    /* Header line */
    FindLine(fp, "OBS_TYPE", &lno, fname);

    for (i = 0; i < MAXOBS; i++)
    {
        NextLine(fp, cmdstr, &lno);
        if (strcasecmp(cmdstr, "EOF") == 0)
        {
            break;
        }
        else
        {
            if (sscanf(cmdstr, "%s %s %lf %lf %lf %lf",
                obstbl[i].name, obstbl[i].fname,
                &obstbl[i].x, &obstbl[i].y,
                &obstbl[i].radius, &obstbl[i].depth) != 6)
            {
                printf("Observation table format error.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose (fp);
}
