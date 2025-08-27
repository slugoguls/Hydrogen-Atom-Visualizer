#include "hydrogen.h"
#include <complex>
#include <cmath>

const double PI = 3.14159265358979323846;

Hydrogen::Hydrogen(int n, int m, int l, int s) : n(n), m(m), l(l), s(s) {
    a = 1 / sqrt(2 * PI);  // Bohr radius in atomic units
    a0 = 0.52917721067; // Angstrom
}

std::complex<double> Hydrogen::getP(double phi) {
    // Correctly implement e^(i*m*phi) = cos(m*phi) + i*sin(m*phi)
    return std::complex<double>(cos(this->m * phi), sin(this->m * phi));
}

double Hydrogen::getTheta(double theta) {
    // Using real spherical harmonics' theta-dependence
    if (l == 0) { // s
        return sqrt(1.0/2.0);
    }
    if (l == 1) { // p
        if (m == 0) return sqrt(3.0/2.0) * cos(theta);
        if (abs(m) == 1) return sqrt(3.0/4.0) * sin(theta);
    }
    if (l == 2) { // d
        if (m == 0) return sqrt(5.0/8.0) * (3 * pow(cos(theta), 2) - 1);
        if (abs(m) == 1) return sqrt(15.0/4.0) * sin(theta) * cos(theta);
        if (abs(m) == 2) return sqrt(15.0/16.0) * pow(sin(theta), 2);
    }
    if (l == 3) { // f
        if (m == 0) return sqrt(7.0/8.0) * (5 * pow(cos(theta), 3) - 3 * cos(theta));
        if (abs(m) == 1) return sqrt(21.0/32.0) * sin(theta) * (5 * pow(cos(theta), 2) - 1);
        if (abs(m) == 2) return sqrt(105.0/16.0) * pow(sin(theta), 2) * cos(theta);
        if (abs(m) == 3) return sqrt(35.0/32.0) * pow(sin(theta), 3);
    }
    return 0.0;
}

double Hydrogen::getR(double r)
{
	//https://en.wikipedia.org/wiki/Hydrogen_atom#Solutions_of_the_Schr%C3%B6dinger_equation
	// a0 = 1
	switch (this->n)
	{
	case 1:
		switch (this->l)
		{
		case 0:
			// 1s
			return 2 * exp(-r);
		}
	case 2:
		switch (this->l)
		{
		case 0:
			// 2s
			return (1.0f / (2.0f * sqrt(2.0f))) * (2.0f - r) * exp(-r / 2.0f);
		case 1:
			// 2p
			return (1.0f / (2.0f * sqrt(6.0f))) * r * exp(-r / 2.0f);
		}
	case 3:
		switch (this->l)
		{
		case 0:
			// 3s
			return (1.0f / (9.0f * sqrt(3.0f))) * (6.0f - 6.0f * r + (r*r)) * exp(-r / 3.0f);
		case 1:
			// 3p
			return (1.0f / (9.0f * sqrt(6.0f))) * (4.0f - (2.0f/3.0f)*r) * r * exp(-r / 3.0f);
		case 2:
			// 3d
			return (1.0f / (9.0f * sqrt(30.0f))) * (r*r) * exp(-r / 3.0f);
		}
	case 4:
		switch (this->l)
		{
		case 0:
			// 4s
			return (1.0f - 0.75f * r + 0.125f * (r*r) - (1.0f / 192.0f) * (r*r*r)) * exp(-r / 4.0f);
		case 1:
			// 4p
			return (1.0f - 0.25f * r + 0.0125f * (r*r)) * r * exp(-r / 4.0f);
		case 2:
			// 4d
			return (1.0f - (1.0f / 12.0f) * r) * (r*r) * exp(-r / 4.0f);
		case 3:
			// 4f
			return (r*r*r) * exp(-r / 4.0f);
		}
	}


	return 0.0f;
}
