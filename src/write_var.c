#include "enkf.h"

void WriteVarOutput(const char *pihm_dir, const char *output_dir,
    const char *project, const vartbl_struct *vartbl, int t,
    const ens_struct *ens)
{
    double          outtime;
    int             i;

    outtime = (double)t;

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ens->ne; i++)
    {
        /*
         * Write variable/flux output
         */
        int             j, k;
        FILE           *fp;
        char            fname[MAXSTRING];

        for (k = 0; k < MAXVAR; k++)
        {
            if (vartbl[k].dim > 0)
            {
                sprintf(fname, "%s/output/%s/%s.%3.3d.%s.dat",
                    pihm_dir, output_dir, project, i + 1, vartbl[k].name);
                fp = fopen(fname, "ab");
                CheckFile(fp, fname);

                fwrite(&outtime, sizeof(double), 1, fp);
                for (j = 0; j < vartbl[k].dim; j++)
                {
                    if (strcasecmp(vartbl[k].name, "rivflx1") == 0)
                    {
                        ens->member[i].var[k][j] =
                            exp(ens->member[i].var[k][j]) - QMIN;
                        ens->member[i].var[k][j] /= 24.0 * 3600.0;
                    }
                    fwrite(&ens->member[i].var[k][j], sizeof(double), 1, fp);
                }
                fflush(fp);
                fclose(fp);
            }
        }
    }
}
