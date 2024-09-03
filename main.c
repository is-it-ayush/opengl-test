#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

const char* vertex_shader_source = "#version 330 core\n"
                                   "\n"
                                   "void main() {\n"
                                   "\n"
                                   "}\n";

const char* frag_shader_source = "#version 330 core\n"
                                 "\n"
                                 "void main() {\n"
                                 "\n"
                                 "}\n";

typedef struct {
    GLuint vao;
    GLuint vbo;
} Renderer;

const char* shader_type_as_cstr(GLuint shader) {
    switch(shader) {
    case GL_VERTEX_SHADER:
        return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
        return "GL_FRAGMENT_SHADER";
    default:
        return "(Unknown)";
    }
}

bool compile_shader_source(
    const GLchar* source, GLenum shader_type, GLuint* shader
) {
    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

    if(!compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(
            stderr,
            "ERROR: could not compile %s\n",
            shader_type_as_cstr(shader_type)
        );
        fprintf(stderr, "%.*s\n", message_size, message);
        return false;
    }

    return true;
}

bool link_program(GLuint vert_shader, GLuint frag_shader, GLuint* program) {
    *program = glCreateProgram();

    glAttachShader(*program, vert_shader);
    glAttachShader(*program, frag_shader);
    glLinkProgram(*program);

    GLint linked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if(!linked) {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(*program, sizeof(message), &message_size, message);
        fprintf(stderr, "Program Linking: %.*s\n", message_size, message);
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return program;
}

void MessageCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam
) {
    (void)source;
    (void)id;
    (void)length;
    (void)userParam;
    fprintf(
        stderr,
        "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type,
        severity,
        message
    );
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

void init_shaders() {
    GLuint vert_shader, frag_shader, program;
    if(!compile_shader_source(
           vertex_shader_source, GL_VERTEX_SHADER, &vert_shader
       ) ||
       !compile_shader_source(
           frag_shader_source, GL_FRAGMENT_SHADER, &frag_shader
       ) ||
       !link_program(vert_shader, frag_shader, &program)) {
        fprintf(stderr, "ERROR: could not compile/link shaders\n");
        exit(1);
    }
    glUseProgram(program);
}

int main() {
    if(!glfwInit()) {
        fprintf(stderr, "ERROR: could not initialize GLFW\n");
        exit(1);
    }
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* const window = glfwCreateWindow(
        DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Otto", NULL, NULL
    );
    if(!window) {
        fprintf(stderr, "ERROR: could not create window\n");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);

    if(GLEW_OK != glewInit()) {
        fprintf(stderr, "ERROR: could not initialize GLEW\n");
        glfwTerminate();
        exit(1);
    }

    if(glDrawArraysInstanced == NULL) {
        fprintf(stderr, "Support for EXT_draw_instanced is required!\n");
        exit(1);
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(window, window_size_callback);

    init_shaders();

    double time = glfwGetTime();
    double prev_time = 0.0;
    while(!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos = xpos - width * 0.5f;
        ypos = (height - ypos) - height * 0.5f;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
