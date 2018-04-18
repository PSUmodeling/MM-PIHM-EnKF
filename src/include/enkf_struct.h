#ifndef ENKF_STRUCT_HEADER
#define ENKF_STRUCT_HEADER

typedef struct paramtbl_struct
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
} paramtbl_struct;

typedef struct vartbl_struct
{
    char            name[MAXSTRING];
    int             dim;
    double          min;
    double          max;
} vartbl_struct;

typedef struct obstbl_struct
{
    char            name[MAXSTRING];
    char            fname[MAXSTRING];
    double          x;
    double          y;
    double          radius;
    double          depth;
    int             dim;
    int             nlyr;
    int            *var_ind;
    /* The linear observation operator vector has a general form of
     *
     * xf = SUM_j { SUM_i  [w_i * (k_ij x_ij + b_ij)]}
     *
     * w_i, k_j, and b_j are initialized at the beginning and stored here for
     * easy access so they don't need to be calculated at every EnKF step.
     * w_i could be the area of each model grid
     * k_j is ususally 1.0, or could be the thickness of each soil layer
     * b_j could be the soil column depth if xf is water table depth. */
    double         *weight;
    double        **k;
    double        **b;
} obstbl_struct;

typedef struct member_struct
{
    double          param[MAXPARAM];
    double         *var[MAXVAR];
} member_struct;

typedef struct ens_struct
{
    int             ne;
    member_struct   member[MAXNE];
} ens_struct;

#endif
