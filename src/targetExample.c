#include <math.h>

double foo(double x) {
    // This function calculate the motivation example:
    // The example is from gsl_sf_airy_Ai_e function in Line 680 of gsl-2.5/specfunc/airy.c
    // you can copy this exampe to /atom/src and run make && bin/gslSolver.out example 0 under /atom
    // x range is [-infinty,1]
    double b = 0.0088094517676206868934;
    double a = -0.019999248799348758043;
    if(x<=1){
        double v1 = 0.25 + b;
        double v2 = x*v1;
        double v3 = a-v2;
        double v4 = 0.375+v3;
        return v4;

    }else{
        return 0.0;
    }
    double val = (1-cos(x))/(x*x);
    return val;
}

double foo2(double x) {
    // This function calculate the motivation example:
    // The example is from gsl_sf_airy_Ai_e function in Line 680 of gsl-2.5/specfunc/airy.c
    // Note that the results fitness may only approximate to value in the paper due to input x found by FPCC may not the same with the input found by ATOM in example
    // x range is [-infinty,1]
    double b = 0.0088094517676206868934;
    double a = -0.019999248799348758043;
    if(x<=1){
        double v1 = 0.25 + b;
        double v2 = x*v1;
        double v3 = a-v2;
        double v4 = 0.375*v3;
        return v4;

    }else{
        return 0.0;
    }
    double val = (1-cos(x))/(x*x);
    return val;
}


double foo3(double x) {
    // This function calculate the motivation example in atomu:
    // y = (1-cos(x))/(x^2)
    // See details at: https://www.wolframalpha.com/input/?i=(1-cos(x))%2F(x%5E2)

    // It triggers significant error when x->0, where y should be ->0.5.
    double val = (1-cos(x))/(x*x);
    return val;
}
