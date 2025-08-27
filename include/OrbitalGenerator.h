#ifndef ORBITAL_GENERATOR_H
#define ORBITAL_GENERATOR_H

#include <glm/glm.hpp>
#include <vector>
#include "QuantumNumbers.h"

class OrbitalGenerator {
public:
    OrbitalGenerator(unsigned int orbitalVAO, unsigned int orbitalPosVBO, unsigned int orbitalColorVBO);
    void generateOrbital(const QuantumNumbers& qn);
    int getNumOrbitalPoints() const { return numOrbitalPoints_; }

private:
    unsigned int orbitalVAO_;
    unsigned int orbitalPosVBO_;
    unsigned int orbitalColorVBO_;
    std::vector<glm::vec3> orbitalPoints_;
    std::vector<glm::vec3> orbitalColors_;
    int numOrbitalPoints_;
};

#endif // ORBITAL_GENERATOR_H
