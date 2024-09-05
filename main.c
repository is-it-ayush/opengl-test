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

const char* vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 verPos;\n"
    "void main() {\n"
    "  gl_Position = vec4(verPos.x, verPos.y, verPos.z, 1.0);\n"
    "}\n";

const char* frag_shader_source = "#version 330 core\n"
                                 "out vec4 frag_color;\n"
                                 "void main() {\n"
                                 "  frag_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
                                 "}\n";
float vertices[] = {
    // x     y     z
    0.5f,  0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
   -0.5f,  0.5f, 0.0f, // top left
   -0.5f, -0.5f, 0.0f, // bottom left
};
unsigned int indices[] = {
  0, 1, 2, // triangle 1
  1, 2, 3, // triangle 2
};

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // element buffer object
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW
  );
}

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

// compiler a shader resource
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
        "[ERROR] Could not compile %s: %.*s\n",
        shader_type_as_cstr(shader_type),
        message_size,
        message
    );
    return false;
  }

  return true;
}

// link a program with a vertex and fragment shader.
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
    fprintf(
        stderr, "[Error] Could not link program: %.*s\n", message_size, message
    );
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return program;
}

// load the vertex and fragment shader
void process_shaders() {
  GLuint vert_shader = 0, frag_shader = 0, program = 0;
  if(!compile_shader_source(
         vertex_shader_source, GL_VERTEX_SHADER, &vert_shader
     ) ||
     !compile_shader_source(
         frag_shader_source, GL_FRAGMENT_SHADER, &frag_shader
     ) ||
     !link_program(vert_shader, frag_shader, &program)) {
    fprintf(stderr, "[ERROR] Could not compile/link shaders\n");
    exit(1);
  }
  glUseProgram(program);
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
void process_keyboard(GLFWwindow* window) {
  // ESC should close the window
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

// error/debug callback for opengl
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

// window resize callback for glfw
void window_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
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
  glDebugMessageCallback(MessageCallback, 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // callbacks
  glfwSetFramebufferSizeCallback(window, window_size_callback);

  // inits
  process_shaders();
  process_buffers();

  // loop
  double time = glfwGetTime();
  double prev_time = 0.0;
  while(!glfwWindowShouldClose(window)) {
    process_mouse(window);
    process_keyboard(window);

    // clear frame before rendering
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // render
    // glDrawArrays(GL_TRIANGLES, 0, 3); // redner with vertex buffer object
    glDrawElements(
        GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0
    ); // render with element buffer object indices and vertex buffer object
       // data

    // poll for events, call the registered callbacks & finally swap buffers on
    // window
    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}
