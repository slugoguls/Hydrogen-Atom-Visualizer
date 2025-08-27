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

#define USE_GPU_ENGINE 0
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void generateOrbital(int n, int l, int m);


// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 640 / 2.0f;
float lastY = 480 / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Quantum numbers
int n = 1, l = 0, m = 0;
bool orbitalNeedsUpdate = true;

// Orbital data
unsigned int orbitalVAO, orbitalVBO;
std::vector<glm::vec3> orbitalPoints;
int numOrbitalPoints = 0;

// Sphere generation
void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectors, int stacks);

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

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Sphere
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSphere(vertices, indices, 0.1f, 36, 18);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Orbital
    glGenVertexArrays(1, &orbitalVAO);
    glGenBuffers(1, &orbitalVBO);
    glBindVertexArray(orbitalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        if (orbitalNeedsUpdate) {
            generateOrbital(n, l, m);
            orbitalNeedsUpdate = false;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI Window
        ImGui::Begin("Controls");
        ImGui::Text("Quantum Numbers");
        ImGui::Text("n = %d, l = %d, m = %d", n, l, m);
        ImGui::Text("Use arrow keys to change state.");
        ImGui::Text("Up/Down: n");
        ImGui::Text("Left/Right: l");
        ImGui::Text("'[' and ']': m");
        ImGui::End();


        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // Draw nucleus
        nucleusShader.bind();
        glUniformMatrix4fv(glGetUniformLocation(nucleusShader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(nucleusShader.id, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(nucleusShader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

        // Draw orbital
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

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &orbitalVAO);
    glDeleteBuffers(1, &orbitalVBO);

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
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        bool stateChanged = false;
        if (key == GLFW_KEY_UP) {
            n++;
            if (n > 7) n = 7; // Cap n at a reasonable value
            l = 0; m = 0;
            stateChanged = true;
        }
        if (key == GLFW_KEY_DOWN) {
            n--;
            if (n < 1) n = 1;
            l = 0; m = 0;
            stateChanged = true;
        }
        if (key == GLFW_KEY_RIGHT) {
            if (n > 1) {
                l++;
                if (l >= n) l = n - 1;
                m = 0;
                stateChanged = true;
                printf("l changed to: %d\n", l);
            }
        }
        if (key == GLFW_KEY_LEFT) {
            if (n > 1) {
                l--;
                if (l < 0) l = 0;
                m = 0;
                stateChanged = true;
                printf("l changed to: %d\n", l);
            }
        }
        if (key == GLFW_KEY_RIGHT_BRACKET) {
            m++;
            if (m > l) m = l;
            stateChanged = true;
            printf("m changed to: %d\n", m);
        }
        if (key == GLFW_KEY_LEFT_BRACKET) {
            m--;
            if (m < -l) m = -l;
            stateChanged = true;
            printf("m changed to: %d\n", m);
        }

        if (stateChanged) {
            orbitalNeedsUpdate = true;
        }
    }

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

void generateOrbital(int n, int l, int m) {
    orbitalPoints.clear();
    Hydrogen h(n, m, l, 0);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Determine a suitable maximum radius for sampling
    // This is a heuristic, might need adjustment
    double max_r = n * n * 2.5;

    double max_prob = 0.0;
    // Pre-sample to find a rough maximum probability to normalize against
    for (int i = 0; i < 10000; ++i) {
        double r = dis(gen) * max_r;
        double theta = dis(gen) * 3.14159265;
        double prob = std::pow(h.getR(r), 2) * std::pow(h.getTheta(theta), 2);
        if (prob > max_prob) {
            max_prob = prob;
        }
    }
    if (max_prob == 0.0) max_prob = 1.0;


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
        }
    }

    numOrbitalPoints = orbitalPoints.size();

    glBindVertexArray(orbitalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitalVBO);
    glBufferData(GL_ARRAY_BUFFER, orbitalPoints.size() * sizeof(glm::vec3), orbitalPoints.data(), GL_STATIC_DRAW);
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