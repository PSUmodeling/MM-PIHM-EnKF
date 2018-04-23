#include "enkf.h"

void WriteInit(const char *pihm_dir, const char *project,
    const vartbl_struct *vartbl, const ens_struct *ens)
{
    const char      elem_ic_var[MAXVAR][MAXSTRING] = {
        "is", "snow", "surf", "unsat", "gw",
        "t1", "snowh",
        "stc0", "stc1", "stc2", "stc3", "stc4", "stc5", "stc6", "stc7", "stc8",
        "stc9", "stc10",
        "smc0", "smc1", "smc2", "smc3", "smc4", "smc5", "smc6", "smc7", "smc8",
        "smc9", "smc10",
        "swc0", "swc1", "swc2", "swc3", "swc4", "swc5", "swc6", "swc7", "swc8",
        "swc9", "swc10",
        "\0"
    };
    const char      river_ic_var[MAXVAR][MAXSTRING] = {
        "stage", "gw",
        "\0"
    };
    int             i;

#if defined(_OPENMP)
# pragma omp parallel for
#endif
    for (i = 0; i < ens->ne; i++)
    {
        char            fn[MAXSTRING];
        FILE           *fp;
        int             j, k;
        int             ind;
        /*
         * Write PIHM initial condition
         */
        sprintf(fn, "%s/input/%s/%s.%3.3d.ic",
            pihm_dir, project, project, i + 1);
        fp = fopen(fn, "wb");
        CheckFile(fp, fn);

        for (j = 0; j < nelem; j++)
        {
            k = 0;
            while (elem_ic_var[k][0] != '\0')
            {
                ind = FindVar(elem_ic_var[k], vartbl);
                fwrite(&ens->member[i].var[ind][j], sizeof(double), 1, fp);
                k++;
            }
        }

        for (j = 0; j < nriver; j++)
        {
            k = 0;
            while (river_ic_var[k][0] != '\0')
            {
                ind = FindVar(river_ic_var[k], vartbl);
                fwrite(&ens->member[i].var[ind][j], sizeof(double), 1, fp);
                k++;
            }
        }

        fflush(fp);
        fclose(fp);
    }
}
