#ifndef ENKF_FUNC_HEADER
#define ENKF_FUNC_HEADER

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
void            Forecast(const ens_struct *, const vartbl_struct *vartbl,
    const obstbl_struct *, double *);
void            GenRandNum(int, int, double **, double);
void            LandSfcTmpOper(obstbl_struct *, const vartbl_struct *,
    const pihm_struct);
void            ObsOper(obstbl_struct *, const vartbl_struct *,
    const pihm_struct);
void            ObsOperAlloc(obstbl_struct *);
void            PlsblRngCstr(double, double, int, double *);
double          Randn(void);
void            ReadCalib(const char *, const paramtbl_struct *, double *);
void            ReadObs(int, const char *, double *, double *);
void            ReadObsTbl(const char *, obstbl_struct *);
void            ReadParam(const char *, const char *, const paramtbl_struct *,
    ens_struct *);
void            ReadParamTbl(const char *, paramtbl_struct *);
void            ReadVar(const char *, const char *, const char *, int,
    const vartbl_struct *, ens_struct *);
void            ReadVarTbl(const char *, int, int, vartbl_struct *);
int             roundi(double);
void            SavePrior(const ens_struct *, const vartbl_struct *,
    ens_struct *);
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
void            InitMesh(elem_struct *, const meshtbl_struct *);
void            InitRiver(river_struct *, elem_struct *, const rivtbl_struct *,
    const shptbl_struct *, const matltbl_struct *, const meshtbl_struct *,
    const calib_struct *);
void            InitSoil(elem_struct *, const soiltbl_struct *,
    const calib_struct *);
void            InitTopo(elem_struct *, const meshtbl_struct *);
pihm_t_struct   PIHMTime(int);
void            ReadAtt(const char *, atttbl_struct *);
void            ReadLsm(const char *, siteinfo_struct *, ctrl_struct *,
    noahtbl_struct *);
void            ReadMesh(const char *, meshtbl_struct *);
void            ReadRiver(const char *, rivtbl_struct *, shptbl_struct *,
    matltbl_struct *, forc_struct *);
void            ReadSoil(const char *, soiltbl_struct *);
int             StrTime(const char *);

#endif
