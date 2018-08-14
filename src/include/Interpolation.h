#ifndef NORENGINE_INTERPOLATION_H
#define NORENGINE_INTERPOLATION_H


#include <cmath>

struct Interpolation {
    static double linear(double val) {
        return val;
    }

    static double pow2in(double val) {
        return pow(val, 2);
    }
};


#endif //NORENGINE_INTERPOLATION_H
