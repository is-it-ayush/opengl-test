#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include "./src/callback.h"
#include "./src/shader.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

char* vertex_shader_source = "#version 330 core\n"
                             "layout (location = 0) in vec3 verPos;\n"
                             "layout (location = 1) in vec3 colorPos;\n"
                             "out vec3 color;\n"
                             "void main() {\n"
                             "  color = colorPos;\n"
                             "  gl_Position = vec4(verPos, 1.0);\n"
                             "}\n";

char* frag_shader_source = "#version 330 core\n"
                           "out vec4 frag_color;\n"
                           "in vec3 color;\n"
                           "void main() {\n"
                           "  frag_color = vec4(color, 1.0);\n"
                           "}\n";
float vertices[] = {
    // x     y     z   // colors
    0.0f, 0.5f, 0.0f, 1.0f, 0.0f,  0.0f, -0.5f, -0.5f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f,  0.0f,  1.0f,
};
// unsigned int indices[] = {
//     0,
//     1,
//     2, // triangle 1
//     1,
//     2,
//     3, // triangle 2
// };

// GLuint ebo;
GLuint vbo;
GLuint vao;
void process_buffers() {
  // vertex buffer object
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  // vertex array object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))
  );
  glEnableVertexAttribArray(1);

  // element buffer object
  // glGenBuffers(1, &ebo);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  // GL_DYNAMIC_DRAW);
}

// input handling on screen
void process_mouse(GLFWwindow* window) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  xpos = xpos - width * 0.5f;
  ypos = (height - ypos) - height * 0.5f;
}

int main() {
  // load glfw (mulit-platform windowing library)
  if(!glfwInit()) {
    fprintf(stderr, "ERROR: could not initialize GLFW\n");
    exit(1);
  }
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // create window
  GLFWwindow* const window = glfwCreateWindow(
      DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "uwu", NULL, NULL
  );
  if(!window) {
    fprintf(stderr, "ERROR: could not create window\n");
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);

  // init glew (opengl loading library)
  if(GLEW_OK != glewInit()) {
    fprintf(stderr, "ERROR: could not initialize GLEW\n");
    exit(1);
  }

  printf("[Info] Using OpenGL %s\n", glGetString(GL_VERSION));

  // check for ogl supported features that are used
  if(glDrawArraysInstanced == NULL) {
    fprintf(stderr, "Support for EXT_draw_instanced is required!\n");
    exit(1);
  }

  // features
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // wireframe mode

  // callbacks
  glfwSetFramebufferSizeCallback(window, window_size_callback);
  glfwSetKeyCallback(window, key_callback);

  // inits
  GLuint program = process_shaders(vertex_shader_source, frag_shader_source);
  process_buffers();

  // loop
  double time = glfwGetTime();
  while(!glfwWindowShouldClose(window)) {
    process_mouse(window);

    // clear frame before rendering
    glClearColor(0.2f, 0.1f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render

    glDrawArrays(GL_TRIANGLES, 0, 3); // redner with vertex buffer object
    // glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // render with
    // element buffer object indices and vertex buffer object

    // poll for events, call the registered callbacks & finally swap buffers on
    // window
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
