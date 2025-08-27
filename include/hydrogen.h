#ifndef HYDROGEN_H
#define HYDROGEN_H

#include <complex>

class Hydrogen {
public:
    Hydrogen(int n, int m, int l, int s);
    std::complex<double> getP(double phi);
    double getTheta(double theta);
    double getR(double r);

private:
    int n;
    int m;
    int l;
    int s;
    double a;
    double a0;
};

#endif // HYDROGEN_H
