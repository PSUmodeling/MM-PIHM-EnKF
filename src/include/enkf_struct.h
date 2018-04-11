#ifndef ENKF_STRUCT_HEADER
#define ENKF_STRUCT_HEADER

typedef struct param_struct
{
    int             perturb;
    int             update;
    double          perturb_min;
    double          perturb_max;
    double          init_std;
    double          min;
    double          max;
    int             type;
    char            name[MAXSTRING];
} param_struct;

typedef struct member_struct
{
    double          param[MAXPARAM];
    double          var[MAXVAR];
} member_struct;

typedef struct ens_struct
{
    int             ne;
    member_struct   member[MAXNE];
} ens_struct;

#endif
