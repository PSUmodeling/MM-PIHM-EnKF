#include "enkf.h"

void ReadVarTbl(const char *fname, int nelem, int nriver, vartbl_struct *vartbl)
{
    char            cmdstr[MAXSTRING];
    FILE           *fid;
    char            type[MAXSTRING];
    int             i;
    int             lno = 0;

    /* Initial vareter table */
    for (i = 0; i < MAXVAR; i++)
    {
        vartbl[i].dim = 0;
    }

    /* Open variable table */
    fid = fopen (fname, "r");
    CheckFile(fid, fname);

    /* Header line */
    FindLine(fid, "VARIABLE", &lno, fname);

    /* Start reading variable table */
    for (i = 0; i < MAXVAR; i++)
    {
        NextLine(fid, cmdstr, &lno);
        if (strcasecmp(cmdstr, "EOF") == 0)
        {
            break;
        }
        else
        {
            if (sscanf(cmdstr, "%s %s %lf %lf",
                vartbl[i].name, type, &vartbl[i].min, &vartbl[i].max) != 4)
            {
                printf("Variable table format error.\n");
                exit(EXIT_FAILURE);
            }

            /* Define variable dimensions*/
            if (strcasecmp(type, "ELEM") == 0)
            {
                vartbl[i].dim = nelem;
            }
            else if (strcasecmp(type, "RIVER") == 0)
            {
                vartbl[i].dim = nriver;
            }
            else
            {
                printf("Error: variable type %s is not recognizable.\n", type);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(fid);
}
