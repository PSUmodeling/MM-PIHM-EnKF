#include "enkf.h"

int roundi(double x)
{
    int             y;

    y = (int)(x < 0.0 ? (x - 0.5) : (x + 0.5));

    return y;
}
