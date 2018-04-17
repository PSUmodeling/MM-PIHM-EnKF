#ifndef ENKF_FUNC_HEADER
#define ENKF_FUNC_HEADER

void            CheckFile(const FILE *, const char *);
int             CountLine(FILE *, char *, int , ...);
void            FindLine(FILE *, const char *, int *, const char *);
int             FindParam(const char *, const paramtbl_struct *);
void            GenRandNum(int, int, double **, double);
void            NextLine(FILE *, char *, int *);
double          Randn(void);
void            ReadCalib(const char *, const paramtbl_struct *, double *);
void            ReadParam(const char *, const char *, const paramtbl_struct *,
    ens_struct *);
void            ReadParamTbl(const char *, paramtbl_struct *);
void            ReadVar(const char *, const char *, const char *, int,
    const vartbl_struct *, ens_struct *);
void            ReadVarTbl(const char *, int, int, vartbl_struct *);
int             Readable(const char *);
int             roundi(double);
void            WriteCalFile(const ens_struct *, const char *, const char *,
    const paramtbl_struct *paramtbl);
void            WriteParamOutput(const char *, const char *, const char *,
    const paramtbl_struct *, ens_struct *);

/* MM-PIHM functions */
void            ReadAtt(const char *, atttbl_struct *);
void            ReadMesh(const char *, meshtbl_struct *);
void            ReadRiver(const char *, rivtbl_struct *, shptbl_struct *,
    matltbl_struct *, forc_struct *);
void            ReadSoil(const char *, soiltbl_struct *);

#endif
