#ifndef SHADER_HPP
#define SHADER_HPP

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint LoadEmbeddedShaders(const char * VertexShaderCode, const char * FragmentShaderCode);

#endif
