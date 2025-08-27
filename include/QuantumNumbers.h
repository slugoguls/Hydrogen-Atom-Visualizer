#ifndef QUANTUM_NUMBERS_H
#define QUANTUM_NUMBERS_H

struct QuantumNumbers {
    int n;
    int l;
    int m;
    int s; // 1=Up, -1=Down, 0=Both

    QuantumNumbers(int n_val = 1, int l_val = 0, int m_val = 0, int s_val = 1)
        : n(n_val), l(l_val), m(m_val), s(s_val) {}
};

#endif // QUANTUM_NUMBERS_H
