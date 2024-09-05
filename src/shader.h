#include <GL/glew.h>

#ifndef SHADER_FUNCTIONS
#define SHADER_FUNCTIONS

/**
 * Create, Link and Return a Program from vertex shader and fragment shader
 * source
 */
GLuint process_shaders(const GLchar* vert_source, const GLchar* frag_source);

#endif
