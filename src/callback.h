#include <GLFW/glfw3.h>

#ifndef CALLBACK_FUNCTIONS
#define CALLBACK_FUNCTIONS

/**
 * Called when there is an error, info or warning from OpenGL.
 */
void message_callback(
    GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam
);

/**
 * Called when the window is resized.
 */
void window_size_callback(GLFWwindow* window, int width, int height);

/**
 * Called when a key is pressed.
 */
void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mods
);

#endif
