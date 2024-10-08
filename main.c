#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/cglm/cglm.h"
#include "include/cglm/affine-pre.h"
#include "include/cglm/mat4.h"
#include "include/cglm/types.h"

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include "./src/callback.h"
#include "./src/shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./include/stb_image.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 600

char* vertex_shader_source = "#version 330 core\n"
                             "layout (location = 0) in vec3 verPos;\n"
                             "layout (location = 1) in vec3 colorPos;\n"
                             "layout (location = 2) in vec2 texPos;\n"
                             "out vec3 color;\n"
                             "out vec2 texture_cords;\n"
                             "uniform mat4 transform;\n"
                             "void main() {\n"
                             "  color = colorPos;\n"
                             "  texture_cords = texPos;\n"
                             "  gl_Position = transform * vec4(verPos, 1.0);\n"
                             "}\n";

char* frag_shader_source =
    "#version 330 core\n"
    "out vec4 frag_color;\n"
    "in vec3 color;\n"
    "in vec2 texture_cords;\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "void main() {\n"
    "  \n"
    "  frag_color = mix(texture(texture1, texture_cords), texture(texture2, "
    "texture_cords), 0.5);\n"
    "}\n";

float vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
};

unsigned int indices[] = {
    0, 1, 3, // triangle
    1, 2, 3  // triangle
};

// GLuint ebo;
GLuint ebo;
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

  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0
  ); // 3 point vertex
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))
  ); // 3 color vertex
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(
      2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))
  ); // 2 texture vertex
  glEnableVertexAttribArray(2);

  // element buffer object
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW
  );
}

void process_texture(
    char const* path, int texture_index, GLuint channel, bool flip
) {
  int width, height, nr_channels;
  if(flip) {
    stbi_set_flip_vertically_on_load(true);
  }

  unsigned char* data = stbi_load(path, &width, &height, &nr_channels, 0);
  if(!data) {
    fprintf(stderr, "[Error] Could not load %s", path);
    exit(1);
  }

  unsigned int texture;
  glGenTextures(texture_index, &texture);
  glActiveTexture(GL_TEXTURE0 + texture_index);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, channel, GL_UNSIGNED_BYTE,
      data
  );
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
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

float x_deg = 0.0f;
void process_math(GLuint program_id, double time) {
  mat4 res;
  glm_mat4_identity(res); // load identity matrix
  glm_rotate(
      res, glm_rad(x_deg), (vec3){1.0f, 0.0f, 0.0f}
  ); // rotate 90deg along z-axis

  GLuint transform_loc = glGetUniformLocation(program_id, "transform");
  glUniformMatrix4fv(transform_loc, 1, GL_FALSE, *res);
}

int main() {
  // load glfw (mulit-platform windowing library)
  if(!glfwInit()) {
    fprintf(stderr, "[ERROR] Could not initialize GLFW\n");
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
    fprintf(stderr, "[ERROR] Could not create window\n");
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);

  // init glew (opengl loading library)
  if(GLEW_OK != glewInit()) {
    fprintf(stderr, "[ERROR] Could not initialize GLEW\n");
    exit(1);
  }

  printf("[Info] Using OpenGL %s\n", glGetString(GL_VERSION));

  // check for ogl supported features that are used
  if(glDrawArraysInstanced == NULL) {
    fprintf(stderr, "[Error] Support for EXT_draw_instanced is required!\n");
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
  process_texture("./assets/container.jpg", 0, GL_RGB, false);
  process_texture("./assets/pepe.png", 1, GL_RGBA, true);
  GLuint program = process_shaders(vertex_shader_source, frag_shader_source);
  glUniform1i(glGetUniformLocation(program, "texture1"), 0);
  glUniform1i(glGetUniformLocation(program, "texture2"), 1);
  process_buffers();

  // loop
  while(!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();

    process_mouse(window);

    // clear frame before rendering
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render
    process_math(program, time);

    // glDrawArrays(GL_TRIANGLES, 0, 3); // render with vertex buffer object
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // render with
    // element buffer object indices and vertex buffer object

    // poll for events, call the registered callbacks & finally swap buffers on
    // window
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
