#ifndef TEXTURE_HPP
#define TEXTURE_HPP

GLuint loadEmbeddedTexture(const char *data, size_t size);
GLuint direct_load_DDS_from_memory(
	const char * buffer,
	size_t buffer_length);

#endif