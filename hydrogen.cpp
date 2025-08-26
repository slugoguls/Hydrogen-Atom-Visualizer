#include <iostream>
#include <complex>
#include <cmath>
#include <iomanip>
using namespace std;

const double PI = 3.14159265358979323846;

class Hydrogen {
    int n;
    int m;
    int l;
    int s;
    const double a = 1/sqrt(2*PI);  //Bohr radius in atomic units
    const double a0 = 0.52917721067;  //Angstrom
    public:
    Hydrogen(int n, int m, int l, int s) : n(n), m(m), l(l), s(s) {}
    complex<double> getP(double phi) {
        return complex<double>(a*cos(phi), a*sin(phi));
    }
    double getTheta(double theta) {
        if (l == 0) return 1.0 / sqrt(2.0);
        if (l == 1) {
            if (m == 0) return sqrt(6.0)/2.0 * cos(theta);
            if (abs(m) == 1) return sqrt(3.0)/2.0 * sin(theta);
        }
        if (l == 2) {
            if (m == 0) return sqrt(10.0)/4.0 * (3 * pow(cos(theta), 2) - 1);
            if (abs(m) == 1) return sqrt(15.0)/2.0 * sin(theta) * cos(theta);
            if (abs(m) == 2) return sqrt(15.0)/4.0 * pow(sin(theta), 2);
        }
        return 0.0; 
    }
    double getR(double r) {
        double x = r / a0;
        if (n == 1 && l == 0) return 2 * exp(-x) / pow(a0, 1.5);
        if (n == 2 && l == 0) return (1.0 / (2 * sqrt(2) * pow(a0, 1.5))) * (2 - x) * exp(-x / 2);
        if (n == 2 && l == 1) return (1.0 / (2 * sqrt(6) * pow(a0, 1.5))) * x * exp(-x / 2);
        if (n == 3 && l == 0) return (2.0 / (27 * sqrt(3) * pow(a0, 1.5))) * (27 - 18 * x + 2 * pow(x, 2)) * exp(-x / 3);
        if (n == 3 && l == 1) return (4.0 / (81 * sqrt(6) * pow(a0, 1.5))) * (6 * x - pow(x, 2)) * exp(-x / 3);
        if (n == 3 && l == 2) return (4.0 / (81 * sqrt(30) * pow(a0, 1.5))) * pow(x, 2) * exp(-x / 3);
        return 0.0;
    }   
};
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    Hydrogen h(1,0,0,1);
    cout << fixed << setprecision(10) << h.getR(0.529) << "\n";
    return 0;
}