#ifndef ENKF_CONST_HEADER
#define ENKF_CONST_HEADER

#define PI              3.14159265

#define MAXSTRING       1024
#define MAXNE           50
#define MAXPARAM        100
#define MAXVAR          100
#define CORRMAX         0.25
#define SIGMA           0.2     /* 0.2 of parameter range */

#define BADVAL          -999

enum prmt_type {
    KSATH,
    KSATV,
    KINF,
    KMACSATH,
    KMACSATV,
    DINF,
    DROOT,
    DMAC,
    POROSITY,
    ALPHA,
    BETA,
    MACVF,
    MACHF,
    VEGFRAC,
    ALBEDO,
    ROUGH,
    EC,
    ETT,
    EDIR,
    ROUGH_RIV,
    KRIVH,
    KRIVV,
    BEDTHCK,
    RIV_DPTH,
    RIV_WDTH,
    PRCP,
    SFCTMP,
    DRIP,
    CMCMAX,
    RS,
    CZIL,
    FXEXP,
    CFACTR,
    RGL,
    HS,
    REFSMC,
    WLTSMC
};

static char PARAM_NAMES[MAXPARAM][MAXSTRING] = {
    "KSATH",
    "KSATV",
    "KINF",
    "KMACSATH",
    "KMACSATV",
    "DINF",
    "DROOT",
    "DMAC",
    "POROSITY",
    "ALPHA",
    "BETA",
    "MACVF",
    "MACHF",
    "VEGFRAC",
    "ALBEDO",
    "ROUGH",
    "EC",
    "ETT",
    "EDIR",
    "ROUGH_RIV",
    "KRIVH",
    "KRIVV",
    "BEDTHCK",
    "RIV_DPTH",
    "RIV_WDTH",
    "PRCP",
    "SFCTMP",
    "DRIP",
    "CMCMAX",
    "RS",
    "CZIL",
    "FXEXP",
    "CFACTR",
    "RGL",
    "HS",
    "REFSMC",
    "WLTSMC",
};

#endif
