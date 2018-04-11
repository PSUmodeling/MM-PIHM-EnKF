#ifndef ENKF_FUNC_HEADER
#define ENKF_FUNC_HEADER

void            CheckFile(const FILE *, const char *);
int             CountLine(FILE *, char *, int , ...);
void            FindLine(FILE *, const char *, int *, const char *);
void            GenRandNum(int, int, double **, double);
void            NextLine(FILE *, char *, int *);
double          Randn(void);
void            ReadCalib(const char *, double *);
void            ReadParamTbl(const char *, param_struct *);
int             Readable(const char *);
void            WriteCalFile(const ens_struct *, const char *, const char *);
void            WriteParamOutput(int, const char *, const char *,
    const param_struct *, ens_struct *);

#endif

