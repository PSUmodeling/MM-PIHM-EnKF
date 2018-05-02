#ifndef ENKF_FUNC_HEADER
#define ENKF_FUNC_HEADER

double          ltqnorm(double);

void            Anlys(const paramtbl_struct *, const vartbl_struct *, double,
    double, const double *, ens_struct *);
void            Assim(const char *, const char *, const paramtbl_struct *,
    const vartbl_struct *, const obstbl_struct *, double, int, int,
    ens_struct *);
void            BuildPIHM(const char *, const char *, pihm_struct);
void            COSMOSOper(obstbl_struct *, const vartbl_struct *,
    const pihm_struct);
void            CondCovInflt(double, int, double *);
void            CovInflt(const paramtbl_struct *, const vartbl_struct *, double,
    const ens_struct *, ens_struct *);
void            CovRelax(double, int, const double *, double *);
void            DischOper(obstbl_struct *, const vartbl_struct *,
    const pihm_struct);
double          Dist(double, double, double, double);
void            EnKF(double *, double, double, const double *, int);
int             FindParam(const char *, const paramtbl_struct *);
int             FindVar(const char *, const vartbl_struct *);
void            Forecast(const ens_struct *, const vartbl_struct *,
    const obstbl_struct *, double *);
void            FreeEnsMbr(const vartbl_struct *, ens_struct *);
void            FreeObsOper(obstbl_struct *);
void            GenRandNum(int, int, double **, double);
void            LandSfcTmpOper(obstbl_struct *, const vartbl_struct *,
    const pihm_struct);
void            ObsOper(obstbl_struct *, const vartbl_struct *,
    const pihm_struct);
void            ObsOperAlloc(obstbl_struct *);
void            PlsblRngCstr(double, double, int, double *);
void            Randn(int, double, double *);
void            ReadEnsParam(const char *, const char *, const paramtbl_struct *,
    ens_struct *);
void            ReadEnsVar(const char *, const char *, const char *, int,
    const vartbl_struct *, ens_struct *);
void            ReadObs(int, const char *, double *, double *);
void            ReadObsTbl(const char *, obstbl_struct *);
void            ReadParam(const char *, const paramtbl_struct *, double *);
void            ReadParamTbl(const char *, paramtbl_struct *);
void            ReadVarTbl(const char *, int, int, vartbl_struct *);
int             roundi(double);
void            SavePrior(const ens_struct *, const vartbl_struct *,
    ens_struct *);
void            Shuffle(const double *, int, double *);
void            WaterBal(const paramtbl_struct *, const vartbl_struct *,
    pihm_struct, ens_struct *);
void            WeightByArea(const elem_struct *, obstbl_struct *);
void            WriteCalFile(const char *, const char *,
    const paramtbl_struct *, const ens_struct *);
void            WriteInit(const char *, const char *, const vartbl_struct *,
    const ens_struct *);
void            WriteParamOutput(const char *, const char *,
    const paramtbl_struct *, int, int, ens_struct *);
void            WriteVarOutput(const char *, const char *, const char *,
    const vartbl_struct *, int, const ens_struct *);

/* MM-PIHM functions */
void            AdjSmProf(const soil_struct *, const pstate_struct *,
    const double *, double, wflux_struct *, wstate_struct *);
int             FindWaterTable(const double *, int, double, double *);
void            FreeAtttbl(atttbl_struct *);
void            FreeMatltbl(matltbl_struct *);
void            FreeMeshTbl(meshtbl_struct *);
void            FreeRivtbl(rivtbl_struct *);
void            FreeShptbl(shptbl_struct *);
void            FreeSoiltbl(soiltbl_struct *);
void            InitLsm(elem_struct *, const ctrl_struct *,
    const noahtbl_struct *, const calib_struct *);
void            InitMesh(elem_struct *, const meshtbl_struct *);
void            InitRiver(river_struct *, elem_struct *, const rivtbl_struct *,
    const shptbl_struct *, const matltbl_struct *, const meshtbl_struct *,
    const calib_struct *);
void            InitSoil(elem_struct *, const soiltbl_struct *,
    const noahtbl_struct *, const calib_struct *);
void            InitTopo(elem_struct *, const meshtbl_struct *);
pihm_t_struct   PIHMTime(int);
void            ReadAtt(const char *, atttbl_struct *);
void            ReadCalib(const char *, calib_struct *);
void            ReadLsm(const char *, siteinfo_struct *, ctrl_struct *,
    noahtbl_struct *);
void            ReadMesh(const char *, meshtbl_struct *);
void            ReadRiver(const char *, rivtbl_struct *, shptbl_struct *,
    matltbl_struct *, forc_struct *);
void            ReadSoil(const char *, soiltbl_struct *);
int             StrTime(const char *);

#endif
