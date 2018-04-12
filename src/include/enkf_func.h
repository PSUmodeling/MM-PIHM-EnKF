#ifndef ENKF_FUNC_HEADER
#define ENKF_FUNC_HEADER

/* Perturb */
void            CheckFile(const FILE *, const char *);
int             CountLine(FILE *, char *, int , ...);
void            FindLine(FILE *, const char *, int *, const char *);
int             FindParam(const char *, const paramtbl_struct *);
void            GenRandNum(int, int, double **, double);
void            NextLine(FILE *, char *, int *);
double          Randn(void);
void            ReadCalib(const char *, const paramtbl_struct *, double *);
void            ReadParamTbl(const char *, paramtbl_struct *);
void            ReadVar(const char *, const char *, const char *, int,
    const vartbl_struct *, ens_struct *);
int             Readable(const char *);
int             roundi(double);
void            WriteCalFile(const ens_struct *, const char *, const char *,
    const paramtbl_struct *paramtbl);
void            WriteParamOutput(const char *, const char *, const char *,
    const paramtbl_struct *, ens_struct *);
#endif

