#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>
#include <array>
#include <string_view>

static void GLFWErrorHandler(int const errorCode, char const* const description) noexcept(false)
{
    throw std::runtime_error(fmt::format("GLFW Error ({}): {}", errorCode, description));
}

static void CompileShader(GLuint const shaderID, std::string_view const shaderCode)
{
    char const* shaderCodePtr = shaderCode.data();
    glShaderSource(shaderID, 1, &shaderCodePtr, nullptr);
    glCompileShader(shaderID);

    GLint result = GL_FALSE;
    int logLength = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength != 0)
    {
        auto errorMessage = std::string(logLength, '\0');
        glGetShaderInfoLog(shaderID, logLength, nullptr, errorMessage.data());
        throw std::runtime_error(fmt::format("Shader Compilation Failed ({}): {}", result, errorMessage));
    }
}

template <size_t N>
static void LinkShaders(GLuint const programID, std::array<GLuint, N> shaderIDs)
{
    for (GLuint const shaderID : shaderIDs)
        glAttachShader(programID, shaderID);

    glLinkProgram(programID);

    GLint result = GL_FALSE;
    int logLength = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength != 0)
    {
        auto errorMessage = std::string(logLength, '\0');
        glGetProgramInfoLog(programID, logLength, nullptr, errorMessage.data());
        throw std::runtime_error(fmt::format("Program Linking Failed ({}): {}", result, errorMessage));
    }

    for (GLuint const shaderID : shaderIDs) {
        glDetachShader(programID, shaderID);
        glDeleteShader(shaderID);
    }
}

static constexpr std::string_view vertexShaderCode = R"""(
#version 330 core

layout(location = 0) in vec3 modelSpace;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(modelSpace, 1);
}
)""";

static constexpr std::string_view fragmentShaderCode = R"""(
#version 330 core

out vec3 color;

void main() {
    color = vec3(1,0,0);
}
)""";

int main()
{
    try
    {
        glewExperimental = GL_TRUE;
        glfwSetErrorCallback(GLFWErrorHandler);
        glfwInit();

        glfwWindowHint(GLFW_SAMPLES, 1);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        uint16_t const windowWidth = 1920;
        uint16_t const windowHeight = 1080;
        GLFWwindow* const window = glfwCreateWindow(windowWidth, windowHeight, "OpenGLTest", nullptr, nullptr);
        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        if (GLenum const res = glewInit(); res != GLEW_OK)
            throw std::runtime_error(reinterpret_cast<char const*>(glewGetErrorString(res)));

        GLuint vertexArrayID;
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        GLuint const programID = glCreateProgram();
        GLuint const vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint const fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        CompileShader(vertexShaderID, vertexShaderCode);
        CompileShader(fragmentShaderID, fragmentShaderCode);
        LinkShaders(programID, std::array{ vertexShaderID, fragmentShaderID });

        GLint const mvpID = glGetUniformLocation(programID, "mvp");

        glm::mat4 const model = glm::mat4(1.f);
        glm::mat4 const view = glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 const projection = glm::perspective(glm::radians(45.f), static_cast<float>(windowWidth) / windowHeight, 0.1f, 100.f);
        glm::mat4 const mvp = projection * view * model;

        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

        constexpr std::array vertexBufferData = {
            -1.f, -1.f, 0.f,
            1.f, -1.f, 0.f,
            0.f, 1.f, 0.0f,

            -1.f, -1.f, 0.f,
            0.f, 1.f, 0.0f,
            0.f, -1.f, -1.41f,

            1.f, -1.f, 0.f,
            0.f, 1.f, 0.0f,
            0.f, -1.f, -1.41f,
        };

        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData.data(), GL_STATIC_DRAW);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(programID);
        glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, vertexBufferData.size() / 3);
        glDisableVertexAttribArray(0);
        glfwSwapBuffers(window);

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        while (glfwWindowShouldClose(window) == 0 && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
        {
            glfwPollEvents();
        }
    }
    catch (std::exception const& e)
    {
        fmt::print(stderr, "Fatal Death: {}\n", e.what());
    }

    glfwTerminate();
}