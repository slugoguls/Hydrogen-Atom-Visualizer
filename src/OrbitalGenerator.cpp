#include "OrbitalGenerator.h"
#include "hydrogen.h"
#include <random>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h> // Added for OpenGL functions

// Constructor
OrbitalGenerator::OrbitalGenerator(unsigned int orbitalVAO, unsigned int orbitalPosVBO, unsigned int orbitalColorVBO)
    : orbitalVAO_(orbitalVAO), orbitalPosVBO_(orbitalPosVBO), orbitalColorVBO_(orbitalColorVBO),
      numOrbitalPoints_(0) {}

void OrbitalGenerator::generateOrbital(const QuantumNumbers& qn) {
    orbitalPoints_.clear();
    orbitalColors_.clear();
    Hydrogen h(qn.n, qn.m, qn.l, qn.s);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double max_r = qn.n * qn.n * 2.5;
    double max_prob = 0.0;
    for (int i = 0; i < 10000; ++i) {
        double r = dis(gen) * max_r;
        double theta = dis(gen) * 3.14159265;
        double prob = std::pow(h.getR(r), 2) * std::pow(h.getTheta(theta), 2);
        if (prob > max_prob) {
            max_prob = prob;
        }
    }
    if (max_prob == 0.0) max_prob = 1.0;

    glm::vec3 color_up(0.2f, 0.5f, 1.0f);
    glm::vec3 color_down(1.0f, 0.3f, 0.2f);

    for (int i = 0; i < 50000; ++i) {
        double r = dis(gen) * max_r;
        double theta = dis(gen) * 3.14159265;
        double phi = dis(gen) * 2 * 3.14159265;

        double prob = std::pow(h.getR(r), 2) * std::pow(h.getTheta(theta), 2);

        if (prob / max_prob > dis(gen)) {
            float x = (float)(r * sin(theta) * cos(phi));
            float y = (float)(r * sin(theta) * sin(phi));
            float z = (float)(r * cos(theta));
            orbitalPoints_.push_back(glm::vec3(x, y, z));

            if (qn.s == 1) {
                orbitalColors_.push_back(color_up);
            } else if (qn.s == -1) {
                orbitalColors_.push_back(color_down);
            } else { // qn.s == 0 (Both)
                orbitalColors_.push_back((dis(gen) > 0.5) ? color_up : color_down);
            }
        }
    }

    numOrbitalPoints_ = orbitalPoints_.size();

    glBindVertexArray(orbitalVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalPosVBO_);
    glBufferData(GL_ARRAY_BUFFER, orbitalPoints_.size() * sizeof(glm::vec3), orbitalPoints_.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalColorVBO_);
    glBufferData(GL_ARRAY_BUFFER, orbitalColors_.size() * sizeof(glm::vec3), orbitalColors_.data(), GL_STATIC_DRAW);
}
