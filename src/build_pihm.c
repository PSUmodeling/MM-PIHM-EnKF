#include "enkf.h"

void BuildPIHM(const char *pihm_dir, const char *project, pihm_struct pihm)
{
    int             i;

    /* Generate PIHM input file names */
    sprintf(pihm->filename.riv,  "%s/input/%s/%s.riv",
        pihm_dir, project, project);
    sprintf(pihm->filename.mesh, "%s/input/%s/%s.mesh",
        pihm_dir, project, project);
    sprintf(pihm->filename.att,  "%s/input/%s/%s.att",
        pihm_dir, project, project);
    sprintf(pihm->filename.soil, "%s/input/%s/%s.soil",
        pihm_dir, project, project);
    sprintf(pihm->filename.lsm,  "%s/input/%s/%s.lsm",
        pihm_dir, project, project);

    /* Read river input file */
    ReadRiver(pihm->filename.riv, &pihm->rivtbl, &pihm->shptbl, &pihm->matltbl,
        &pihm->forc);

    /* Read mesh structure input file */
    ReadMesh(pihm->filename.mesh, &pihm->meshtbl);

    /* Read attribute table input file */
    ReadAtt(pihm->filename.att, &pihm->atttbl);

    /* Read soil input file */
    ReadSoil(pihm->filename.soil, &pihm->soiltbl);

    /* Read LSM input file */
    ReadLsm(pihm->filename.lsm, &pihm->siteinfo, &pihm->ctrl, &pihm->noahtbl);

    pihm->elem = (elem_struct *)malloc(nelem * sizeof(elem_struct));
    pihm->river = (river_struct *)malloc(nriver * sizeof(river_struct));

    for (i = 0; i < nelem; i++)
    {
        pihm->elem[i].attrib.soil_type = pihm->atttbl.soil[i];
    }

    InitMesh(pihm->elem, &pihm->meshtbl);

    InitTopo(pihm->elem, &pihm->meshtbl);

    InitSoil(pihm->elem, &pihm->soiltbl, &pihm->cal);

    InitRiver(pihm->river, pihm->elem, &pihm->rivtbl, &pihm->shptbl,
        &pihm->matltbl, &pihm->meshtbl, &pihm->cal);
}