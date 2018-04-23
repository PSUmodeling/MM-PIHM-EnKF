#ifndef ENKF_CONST_HEADER
#define ENKF_CONST_HEADER

#define MAXNE           50
#define MAXPARAM        100
#define MAXVAR          100
#define MAXOBS          20
#define CORRMAX         0.25    /* Maximum correlation between perturbed
                                 * parameters */
#define INIT_STD        0.2     /* initial standar deviation expressed as
                                 * fraction of parameter range */
#define CCI_THR         0.25    /* conditional covariance inflation threshold
                                 * expressed as fraction of initial standard
                                 * deviation */
#define EPSILON         1.0E-6
#define PERTURB         0
#define ASSIM           1
#define BADVAL          -999

//enum prmt_type {
//    KSATH,
//    KSATV,
//    KINF,
//    KMACSATH,
//    KMACSATV,
//    DINF,
//    DROOT,
//    DMAC,
//    POROSITY,
//    ALPHA,
//    BETA,
//    MACVF,
//    MACHF,
//    VEGFRAC,
//    ALBEDO,
//    ROUGH,
//    EC,
//    ETT,
//    EDIR,
//    ROUGH_RIV,
//    KRIVH,
//    KRIVV,
//    BEDTHCK,
//    RIV_DPTH,
//    RIV_WDTH,
//    PRCP,
//    SFCTMP,
//    DRIP,
//    CMCMAX,
//    RS,
//    CZIL,
//    FXEXP,
//    CFACTR,
//    RGL,
//    HS,
//    REFSMC,
//    WLTSMC
//};

//static char PARAM_NAMES[MAXPARAM][MAXSTRING] = {
//    "KSATH",
//    "KSATV",
//    "KINF",
//    "KMACSATH",
//    "KMACSATV",
//    "DINF",
//    "DROOT",
//    "DMAC",
//    "POROSITY",
//    "ALPHA",
//    "BETA",
//    "MACVF",
//    "MACHF",
//    "VEGFRAC",
//    "ALBEDO",
//    "ROUGH",
//    "EC",
//    "ETT",
//    "EDIR",
//    "ROUGH_RIV",
//    "KRIVH",
//    "KRIVV",
//    "BEDTHCK",
//    "RIV_DPTH",
//    "RIV_WDTH",
//    "PRCP",
//    "SFCTMP",
//    "DRIP",
//    "CMCMAX",
//    "RS",
//    "CZIL",
//    "FXEXP",
//    "CFACTR",
//    "RGL",
//    "HS",
//    "REFSMC",
//    "WLTSMC",
//};

#endif
