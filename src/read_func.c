#include "enkf.h"

int Readable(const char *cmdstr)
{
    int             readable;
    int             i;
    char            ch;

    for (i = 0; i < (int)(strlen(cmdstr)); i++)
    {
        if (cmdstr[i] == 32 || cmdstr[i] == '\t' || cmdstr[i] == ' ')
        {
            continue;
        }
        else
        {
            break;
        }
    }

    if (i >= (int)(strlen(cmdstr)))
    {
        readable = 0;
    }
    else
    {
        ch = cmdstr[i];

        if (ch != '#' && ch != '\n' && ch != '\0' && ch != '\r')
        {
            readable = 1;
        }
        else
        {
            readable = 0;
        }
    }

    return readable;
}

void FindLine(FILE *fid, const char *token, int *lno, const char *filename)
{
    int             success = 0;
    char            cmdstr[MAXSTRING];
    char            optstr[MAXSTRING];

    if (strcasecmp("BOF", token) == 0)
    {
        rewind(fid);
        *lno = 0;
        success = 1;
    }
    else
    {
        /* Initialize cmdstr */
        strcpy(cmdstr, "\0");

        while (!feof(fid))
        {
            if (Readable(cmdstr))
            {
                sscanf(cmdstr, "%s", optstr);
                if (strcasecmp(token, optstr) == 0)
                {
                    success = 1;
                    break;
                }
            }

            fgets(cmdstr, MAXSTRING, fid);
            (*lno)++;
        }
    }

    if (!success)
    {
        printf("Cannot find required keyword %s.\n", token);
        printf("Error reading %s.\n", filename);
        exit(EXIT_FAILURE);
    }
}

void NextLine(FILE *fid, char *cmdstr, int *lno)
{
    /*
     * Read a non-blank line into cmdstr
     */
    strcpy(cmdstr, "\0");

    while (!Readable(cmdstr))
    {
        if (fgets(cmdstr, MAXSTRING, fid) == NULL)
        {
            strcpy(cmdstr, "EOF");
            break;
        }
        else
        {
            (*lno)++;
        }
    }
}

int CountLine(FILE *fid, char *cmdstr, int num_arg, ...)
{
    /*
     * Count number of non-blank lines between current location to where
     * token occurs
     */
    va_list         valist;
    char            optstr[MAXSTRING];
    char            token[MAXSTRING];
    int             count;
    int             success = 0;
    int             i;

    /* access all the arguments assigned to valist */
    /* Initialize cmdstr */
    strcpy(cmdstr, "\0");
    count = 0;

    while (!feof(fid))
    {
        if (Readable(cmdstr))
        {
            sscanf(cmdstr, "%s", optstr);

            /* initialize valist for num number of arguments */
            va_start(valist, num_arg);
            for (i = 0; i < num_arg; i++)
            {
                strcpy(token, va_arg(valist, char *));
                if (strcasecmp(token, optstr) == 0)
                {
                    success = 1;
                }
            }
            /* clean memory reserved for valist */
            va_end(valist);

            if (success)
            {
                break;
            }
            else
            {
                count++;
            }
        }

        fgets(cmdstr, MAXSTRING, fid);
    }

    return count;
}

void CheckFile(const FILE *fid, const char *fn)
{
    if (NULL == fid)
    {
        printf("Error opening %s.\n", fn);
        exit(EXIT_FAILURE);
    }
}
