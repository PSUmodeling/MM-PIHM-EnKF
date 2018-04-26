#include "enkf.h"

void WaterBal(const paramtbl_struct *paramtbl, const vartbl_struct *vartbl,
    pihm_struct pihm, ens_struct *ens)
{
    int             n, kk;
    int             ind;
    char            varn[MAXSTRING];
    int             smc_ind[MAXLYR];
    int             sh2o_ind[MAXLYR];

    ind = FindParam("POROSITY", paramtbl);
    for (kk = 0; kk < MAXLYR; kk++)
    {
        sprintf(varn, "smc%d", kk);
        smc_ind[kk] = FindVar(varn, vartbl);

        sprintf(varn, "swc%d", kk);
        sh2o_ind[kk] = FindVar(varn, vartbl);
    }

    for (n = 0; n < ens->ne; n++)
    {
        elem_struct    *elem;
        int             i;

        elem = &pihm->elem[0];

        for (i = 0; i < nelem; i++)
        {
            double          pihm_soilm;
            double          noah_soilm;
            double          sice[MAXLYR];
            int             k;
            soil_struct     soil;

            elem[i].ws.unsat = ens->member[n].var[FindVar("unsat", vartbl)][i];
            elem[i].ws.gw = ens->member[n].var[FindVar("gw", vartbl)][i];

            for (k = 0; k < MAXLYR; k++)
            {
                elem[i].ws.smc[k] = ens->member[n].var[smc_ind[k]][i];
                elem[i].ws.sh2o[k] = ens->member[n].var[sh2o_ind[k]][i];
            }

            soil.smcmax = elem[i].soil.smcmax;
            soil.smcmin = elem[i].soil.smcmin;
            soil.porosity = elem[i].soil.porosity;

            if (1 == paramtbl[ind].perturb)
            {
                soil.smcmax *= ens->member[n].param[ind] / pihm->cal.porosity;
                soil.smcmin *= ens->member[n].param[ind] / pihm->cal.porosity;
                soil.porosity = soil.smcmax - soil.smcmin;
            }

            pihm_soilm =
                ((elem[i].ws.unsat + elem[i].ws.gw > elem[i].soil.depth) ?
                elem[i].soil.depth : elem[i].ws.unsat + elem[i].ws.gw) *
                soil.porosity + elem[i].soil.depth * soil.smcmin;
            noah_soilm = elem[i].ws.smc[0] * elem[i].ps.sldpth[0];
            for (k = 1; k < elem[i].ps.nsoil; k++)
            {
                noah_soilm += elem[i].ws.smc[k] * elem[i].ps.sldpth[k];
            }

            for (k = 0; k < elem[i].ps.nsoil; k++)
            {
                elem[i].ws.smc[k] *= pihm_soilm / noah_soilm;

                elem[i].ws.sh2o[k] = (elem[i].ws.sh2o[k] < elem[i].ws.smc[k]) ?
                    elem[i].ws.sh2o[k] : elem[i].ws.smc[k];

                sice[k] = elem[i].ws.smc[k] - elem[i].ws.sh2o[k];
            }

            elem[i].ps.nwtbl = FindWaterTable(elem[i].ps.sldpth,
                elem[i].ps.nsoil, elem[i].ws.gw, elem[i].ps.satdpth);

            AdjSmProf(&soil, &elem[i].ps, sice, 0.0, &elem[i].wf, &elem[i].ws);

            ens->member[n].var[FindVar("unsat", vartbl)][i] = elem[i].ws.unsat;
            ens->member[n].var[FindVar("gw", vartbl)][i] = elem[i].ws.gw;

            for (k = 0; k < MAXLYR; k++)
            {
                ens->member[n].var[smc_ind[k]][i] = elem[i].ws.smc[k];
                ens->member[n].var[sh2o_ind[k]][i] = elem[i].ws.sh2o[k];
            }
        }
    }
}
