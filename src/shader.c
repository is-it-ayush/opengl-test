#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>

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
        stderr, "[ERROR] Could not compile %s: %.*s\n",
        shader_type_as_cstr(shader_type), message_size, message
    );
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
    fprintf(
        stderr, "[Error] Could not link program: %.*s\n", message_size, message
    );
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return program;
}

GLuint process_shaders(const GLchar* vert_source, const GLchar* frag_source) {
  GLuint vert_shader = 0, frag_shader = 0, program = 0;
  if(!compile_shader_source(vert_source, GL_VERTEX_SHADER, &vert_shader) ||
     !compile_shader_source(frag_source, GL_FRAGMENT_SHADER, &frag_shader) ||
     !link_program(vert_shader, frag_shader, &program)) {
    fprintf(stderr, "[ERROR] Could not compile/link shaders\n");
    exit(1);
  }
  glUseProgram(program);
  return program;
}
