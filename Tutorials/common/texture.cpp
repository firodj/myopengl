#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include GLAD
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint loadEmbeddedTexture(const char * data, size_t size)
{
	// Load the texture using any two methods
	//GLuint Texture = loadBMP_custom("uvtemplate.bmp");
	//GLuint Texture = loadDDS("uvtemplate.DDS");
	int width, height, channel;
	unsigned char *raw = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, &channel, 0);
	
	// Create one OpenGL texture
	GLuint Texture;
	glGenTextures(1, &Texture);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, Texture);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, raw);
	
	stbi_image_free(raw);

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return Texture;
}