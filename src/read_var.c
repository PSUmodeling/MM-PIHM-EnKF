#include "enkf.h"

void ReadEnsVar(const char *pihm_dir, const char *project,
    const char *output_dir, int obs_time, const vartbl_struct *vartbl,
    ens_struct *ens)
{
    int             i;
    int             ne;

    ne = ens->ne;

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ne; i++)
    {
        int             j, k;
        char            fn[MAXSTRING];
        FILE           *fid;
        double          ctime;
        int             success;

        for (k = 0; k < MAXVAR; k++)
        {
            if (vartbl[k].dim > 0)
            {
                /* Allocation */
                ens->member[i].var[k] =
                    (double *)malloc(vartbl[k].dim * sizeof(double));

                /* Open output file */
                sprintf (fn, "%s/output/%s/%s.%3.3d.%s.dat",
                    pihm_dir, output_dir, project, i + 1, vartbl[k].name);
                fid = fopen (fn, "rb");
                CheckFile (fid, fn);

                /* Find predictions at observation time */
                success = 0;
                while (1 == fread(&ctime, sizeof(double), 1, fid))
                {
                    if (roundi(ctime) == obs_time)
                    {
                        fread(ens->member[i].var[k], sizeof(double),
                            vartbl[k].dim, fid);

                        if (strcasecmp(vartbl[k].name, "rivflx1") == 0)
                        {
                            for (j = 0; j < vartbl[k].dim; j++)
                            {
                                ens->member[i].var[k][j] *= 24.0 * 3600.0;
                                ens->member[i].var[k][j] =
                                    log(ens->member[i].var[k][j] + QMIN);
                            }
                        }

                        success = 1;
                        break;
                    }
                    else
                    {
                        fseek(fid, vartbl[k].dim * sizeof(double), SEEK_CUR);
                    }
                }

                fclose (fid);

                if (0 == success)
                {
                    printf(
                        "Error: No %s output available for member %d at %d.\n",
                        vartbl[k].name, i + 1, obs_time);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                break;
            }
        }
    }
}
