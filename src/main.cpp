#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <openglDebug.h>
#include <demoShaderLoader.h>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include <random>
#include <algorithm>
#include "hydrogen.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Hint to use dedicated GPU
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void generateOrbital(int n, int l, int m, int s);
void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectors, int stacks);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 640 / 2.0f;
float lastY = 480 / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Quantum numbers
int n = 1, l = 0, m = 0, s = 1; // s: 1=Up, -1=Down, 0=Both
bool orbitalNeedsUpdate = true;

// Orbital data
unsigned int orbitalVAO, orbitalPosVBO, orbitalColorVBO;
std::vector<glm::vec3> orbitalPoints;
std::vector<glm::vec3> orbitalColors;
int numOrbitalPoints = 0;

int main(void)
{
    if (!glfwInit())
        return -1;

    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hydrogen Atom Visualizer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Shader lightingShader;
    lightingShader.loadShaderProgramFromFile(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "fragment.frag");

    Shader nucleusShader;
    nucleusShader.loadShaderProgramFromFile(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "nucleus.frag");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSphere(vertices, indices, 0.1f, 36, 18);

    unsigned int nucleusVBO, nucleusVAO, nucleusEBO;
    glGenVertexArrays(1, &nucleusVAO);
    glGenBuffers(1, &nucleusVBO);
    glGenBuffers(1, &nucleusEBO);
    glBindVertexArray(nucleusVAO);
    glBindBuffer(GL_ARRAY_BUFFER, nucleusVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nucleusEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &orbitalVAO);
    glGenBuffers(1, &orbitalPosVBO);
    glGenBuffers(1, &orbitalColorVBO);

    glBindVertexArray(orbitalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalPosVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalColorVBO);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        if (orbitalNeedsUpdate) {
            generateOrbital(n, l, m, s);
            orbitalNeedsUpdate = false;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Controls");
        ImGui::Text("Quantum Numbers");
        if (ImGui::SliderInt("n", &n, 1, 4)) orbitalNeedsUpdate = true;
        if (l >= n) l = n - 1;
        if (ImGui::SliderInt("l", &l, 0, n > 0 ? n - 1 : 0)) orbitalNeedsUpdate = true;
        if (m > l) m = l;
        if (m < -l) m = -l;
        if (ImGui::SliderInt("m", &m, -l, l)) orbitalNeedsUpdate = true;
        ImGui::Separator();
        ImGui::Text("Spin (s)");
        if (ImGui::RadioButton("Up", &s, 1)) orbitalNeedsUpdate = true;
        ImGui::SameLine();
        if (ImGui::RadioButton("Down", &s, -1)) orbitalNeedsUpdate = true;
        ImGui::SameLine();
        if (ImGui::RadioButton("Both", &s, 0)) orbitalNeedsUpdate = true;
        ImGui::End();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        nucleusShader.bind();
        glUniformMatrix4fv(glGetUniformLocation(nucleusShader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(nucleusShader.id, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(nucleusShader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(nucleusVAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

        lightingShader.bind();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.id, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(orbitalVAO);
        glPointSize(2.0f);
        glDrawArrays(GL_POINTS, 0, numOrbitalPoints);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &nucleusVAO);
    glDeleteBuffers(1, &nucleusVBO);
    glDeleteBuffers(1, &nucleusEBO);
    glDeleteVertexArrays(1, &orbitalVAO);
    glDeleteBuffers(1, &orbitalPosVBO);
    glDeleteBuffers(1, &orbitalColorVBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard("FORWARD", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard("BACKWARD", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard("LEFT", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard("RIGHT", deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        camera.processMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll((float)yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void generateOrbital(int n, int l, int m, int s) {
    orbitalPoints.clear();
    orbitalColors.clear();
    Hydrogen h(n, m, l, s);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double max_r = n * n * 2.5;
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
            orbitalPoints.push_back(glm::vec3(x, y, z));

            if (s == 1) {
                orbitalColors.push_back(color_up);
            } else if (s == -1) {
                orbitalColors.push_back(color_down);
            } else { // s == 0 (Both)
                orbitalColors.push_back((dis(gen) > 0.5) ? color_up : color_down);
            }
        }
    }

    numOrbitalPoints = orbitalPoints.size();

    glBindVertexArray(orbitalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalPosVBO);
    glBufferData(GL_ARRAY_BUFFER, orbitalPoints.size() * sizeof(glm::vec3), orbitalPoints.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalColorVBO);
    glBufferData(GL_ARRAY_BUFFER, orbitalColors.size() * sizeof(glm::vec3), orbitalColors.data(), GL_STATIC_DRAW);
}

void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectors, int stacks)
{
    vertices.clear();
    indices.clear();

    const float PI = 3.14159265359f;
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;

    float sectorStep = 2 * PI / sectors;
    float stackStep = PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < sectors; ++j)
        {
            int k1 = i * (sectors + 1) + j;
            int k2 = k1 + sectors + 1;

            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }
}
