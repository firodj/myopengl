#include <stdlib.h>
#include <string.h>

const char *find_embedded_file(const char *name, size_t *size);

static const unsigned char v1[] = "#version 330 core\r\n\
\r\n\
// Input vertex data, different for all executions of this shader.\r\n\
layout(location = 0) in vec3 vertexPosition_modelspace;\r\n\
\r\n\
void main(){\r\n\
\r\n\
    gl_Position.xyz = vertexPosition_modelspace;\r\n\
    gl_Position.w = 1.0;\r\n\
\r\n\
}\r\n\
\r\n\
";
static const unsigned char v2[] = "#version 330 core\r\n\
\r\n\
// Ouput data\r\n\
out vec3 color;\r\n\
\r\n\
void main()\r\n\
{\r\n\
\r\n\
	// Output color = red \r\n\
	color = vec3(1,0,0);\r\n\
\r\n\
}";

const struct embedded_file {
  const char *name;
  const unsigned char *data;
  size_t size;
} embedded_files[] = {
  {"SimpleVertexShader.vertexshader", v1, sizeof(v1) - 1},
  {"SimpleFragmentShader.fragmentshader", v2, sizeof(v2) - 1},
  {NULL, NULL, 0}
};

const char *find_embedded_file(const char *name, size_t *size) {
  const struct embedded_file *p;
  for (p = embedded_files; p->name != NULL; p++) {
    if (!strcmp(p->name, name)) {
      if (size != NULL) { *size = p->size; }
      return (const char *) p->data;
    }
  }
  return NULL;
}
