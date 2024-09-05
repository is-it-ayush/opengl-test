#include <stdbool.h>
#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

void message_callback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam
) {
  (void)source;
  (void)id;
  (void)length;
  (void)userParam;
  fprintf(
      stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
      (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
      message
  );
}

// window resize callback for glfw
void window_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// key callback
bool is_wireframe = false;
void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mods
) {
  // ESC should close the window
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  // toggle wireframe
  if(key == GLFW_KEY_P && action == GLFW_PRESS) {
    if(!is_wireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    is_wireframe = !is_wireframe;
  }
}
