#include <GL/glew.h>

#ifndef SHADER_FUNCTIONS
#define SHADER_FUNCTIONS

/**
 * Compile a shader resource.
 */
bool compile_shader_source(
    const GLchar* source, GLenum shader_type, GLuint* shader
);
/**
 * Create and link the program
 */
bool link_program(GLuint vert_shader, GLuint frag_shader, GLuint* program);

#endif
