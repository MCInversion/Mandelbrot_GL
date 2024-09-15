#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// Vertex data
const float vertices[] = {
    // positions
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
};

// Function to load shader code from file
std::string loadShaderSource(const std::string& filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to compile shader
unsigned int compileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

// Function to create shader program
unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

class MandelbrotExplorer {
public:
    double x_min, x_max, y_min, y_max;
    unsigned int max_iterations;
    GLuint vao, vbo, shader;

    MandelbrotExplorer(unsigned int width, unsigned int height, unsigned int max_iterations)
        : x_min(-2.5), x_max(1.5), y_min(-2.0), y_max(2.0), max_iterations(max_iterations) {

        // Load and compile shaders
        std::string vertexShader = loadShaderSource("src/shader.vert");
        std::string fragmentShader = loadShaderSource("src/shader.frag");
        shader = createShader(vertexShader, fragmentShader);

        // Create and bind vertex array and buffer
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    ~MandelbrotExplorer() {
        glDeleteProgram(shader);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    void render() {
        glUseProgram(shader);
        glUniform2f(glGetUniformLocation(shader, "u_min"), x_min, y_min);
        glUniform2f(glGetUniformLocation(shader, "u_max"), x_max, y_max);
        glUniform1i(glGetUniformLocation(shader, "u_max_iterations"), max_iterations);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void zoom(double zoom_factor, double x_center, double y_center) {
        double x_range = x_max - x_min;
        double y_range = y_max - y_min;

        x_min = x_center - (x_center - x_min) * zoom_factor;
        x_max = x_center + (x_max - x_center) * zoom_factor;
        y_min = y_center - (y_center - y_min) * zoom_factor;
        y_max = y_center + (y_max - y_center) * zoom_factor;

        max_iterations = static_cast<unsigned int>(max_iterations * (1.0 / zoom_factor));
    }

    void reset() {
        x_min = -2.5; x_max = 1.5;
        y_min = -2.0; y_max = 2.0;
        max_iterations = 50;
    }
};

MandelbrotExplorer* explorer;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    double zoom_factor = (yoffset > 0) ? 0.9 : 1.1;
    double x_center = (explorer->x_min + explorer->x_max) / 2;
    double y_center = (explorer->y_min + explorer->y_max) / 2;
    explorer->zoom(zoom_factor, x_center, y_center);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        explorer->reset();
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 800, "Mandelbrot Explorer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    explorer = new MandelbrotExplorer(800, 800, 50);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window)) {
        explorer->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete explorer;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
