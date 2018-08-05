#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include GLAD
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stbi_DDS.h>

GLuint loadEmbeddedTexture(const char * data, size_t size)
{
	// Load the texture using any two methods
	//GLuint Texture = loadBMP_custom("uvtemplate.bmp");
	//GLuint Texture = loadDDS("uvtemplate.DDS");
	int width, height, channel;
	unsigned char *raw = nullptr;

	if (stbi_dds_test_memory((const stbi_uc* )data, size))
		raw = stbi_dds_load_from_memory((const stbi_uc *)data, size, &width, &height, &channel, 0);
	else
		raw	= stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, &channel, 0);
	
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

GLuint direct_load_DDS_from_memory(
		const char * buffer,
		size_t buffer_length)
{
	/* forced arguments */
	unsigned int reuse_texture_ID = 0;
	int is_repeats = 0;
	int loading_as_cubemap = 0;
	/*	variables	*/
	DDS_header header;
	unsigned int buffer_index = 0;
	GLuint tex_ID = 0;
	/*	file reading variables	*/
	unsigned int S3TC_type = 0;
	unsigned char *DDS_data;
	unsigned int DDS_main_size;
	unsigned int DDS_full_size;
	unsigned int width, height;
	int mipmaps, cubemap, uncompressed, block_size = 16;
	unsigned int flag;
	unsigned int cf_target, ogl_target_start, ogl_target_end;
	unsigned int opengl_texture_type;
	int i;
	/*	1st off, does the filename even exist?	*/
	if( NULL == buffer )
	{
		/*	we can't do it!	*/
		fprintf(stderr, "NULL buffer.\n");
		return 0;
	}
	if( buffer_length < sizeof( DDS_header ) )
	{
		/*	we can't do it!	*/
		fprintf(stderr, "DDS file was too small to contain the DDS header.\n");
		return 0;
	}
	/*	try reading in the header	*/
	memcpy ( (void*)(&header), (const void *)buffer, sizeof( DDS_header ) );
	buffer_index = sizeof( DDS_header );
	/*	guilty until proven innocent	*/
	fprintf(stderr, "MAYBE Failed to read a known DDS header.\n");
	/*	validate the header (warning, "goto"'s ahead, shield your eyes!!)	*/
	flag = ('D'<<0)|('D'<<8)|('S'<<16)|(' '<<24);
	if( header.dwMagic != flag ) {goto quick_exit;}
	if( header.dwSize != 124 ) {goto quick_exit;}
	/*	I need all of these	*/
	flag = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	if( (header.dwFlags & flag) != flag ) {goto quick_exit;}
	/*	According to the MSDN spec, the dwFlags should contain
		DDSD_LINEARSIZE if it's compressed, or DDSD_PITCH if
		uncompressed.  Some DDS writers do not conform to the
		spec, so I need to make my reader more tolerant	*/
	/*	I need one of these	*/
	flag = DDPF_FOURCC | DDPF_RGB;
	if( (header.sPixelFormat.dwFlags & flag) == 0 ) {goto quick_exit;}
	if( header.sPixelFormat.dwSize != 32 ) {goto quick_exit;}
	if( (header.sCaps.dwCaps1 & DDSCAPS_TEXTURE) == 0 ) {goto quick_exit;}
	/*	make sure it is a type we can upload	*/
	if( (header.sPixelFormat.dwFlags & DDPF_FOURCC) &&
		!(
		(header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('1'<<24))) ||
		(header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('3'<<24))) ||
		(header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('5'<<24)))
		) )
	{
		goto quick_exit;
	}
	/*	OK, validated the header, let's load the image data	*/
	fprintf(stderr, "DDS header loaded and validated.\n");
	width = header.dwWidth;
	height = header.dwHeight;
	uncompressed = 1 - (header.sPixelFormat.dwFlags & DDPF_FOURCC) / DDPF_FOURCC;
	cubemap = (header.sCaps.dwCaps2 & DDSCAPS2_CUBEMAP) / DDSCAPS2_CUBEMAP;
	if( uncompressed )
	{
		S3TC_type = GL_RGB;
		block_size = 3;
		if( header.sPixelFormat.dwFlags & DDPF_ALPHAPIXELS )
		{
			S3TC_type = GL_RGBA;
			block_size = 4;
		}
		DDS_main_size = width * height * block_size;
	} else
	{
		/*	can we even handle direct uploading to OpenGL DXT compressed images?	*/
		if(! GL_EXT_texture_compression_s3tc )
		{
			/*	we can't do it!	*/
			fprintf(stderr, "Direct upload of S3TC images not supported by the OpenGL driver.\n");
			return 0;
		}
		/*	well, we know it is DXT1/3/5, because we checked above	*/
		switch( (header.sPixelFormat.dwFourCC >> 24) - '0' )
		{
		// #define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
		case 1:
			S3TC_type = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			block_size = 8;
			break;
		// #define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
		case 3:
			S3TC_type = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			block_size = 16;
			break;
		// #define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
		case 5:
			S3TC_type = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			block_size = 16;
			break;
		}
		DDS_main_size = ((width+3)>>2)*((height+3)>>2)*block_size;
	}
	if( cubemap )
	{
		/* does the user want a cubemap?	*/
		if( !loading_as_cubemap )
		{
			/*	we can't do it!	*/
			fprintf(stderr, "DDS image was a cubemap.\n");
			return 0;
		}
		/*	can we even handle cubemaps with the OpenGL driver?	*/
		if(GL_EXT_texture_cube_map)
		{
			/*	we can't do it!	*/
			fprintf(stderr, "Direct upload of cubemap images not supported by the OpenGL driver.\n");
			return 0;
		}
		ogl_target_start = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		ogl_target_end =   GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		opengl_texture_type = GL_TEXTURE_CUBE_MAP;
	} else
	{
		/* does the user want a non-cubemap?	*/
		if( loading_as_cubemap )
		{
			/*	we can't do it!	*/
			fprintf(stderr, "DDS image was not a cubemap.\n");
			return 0;
		}
		ogl_target_start = GL_TEXTURE_2D;
		ogl_target_end =   GL_TEXTURE_2D;
		opengl_texture_type = GL_TEXTURE_2D;
	}
	if( (header.sCaps.dwCaps1 & DDSCAPS_MIPMAP) && (header.dwMipMapCount > 1) )
	{
		int shift_offset;
		mipmaps = header.dwMipMapCount - 1;
		DDS_full_size = DDS_main_size;
		if( uncompressed )
		{
			/*	uncompressed DDS, simple MIPmap size calculation	*/
			shift_offset = 0;
		} else
		{
			/*	compressed DDS, MIPmap size calculation is block based	*/
			shift_offset = 2;
		}
		for( i = 1; i <= mipmaps; ++ i )
		{
			int w, h;
			w = width >> (shift_offset + i);
			h = height >> (shift_offset + i);
			if( w < 1 )
			{
				w = 1;
			}
			if( h < 1 )
			{
				h = 1;
			}
			DDS_full_size += w*h*block_size;
		}
	} else
	{
		mipmaps = 0;
		DDS_full_size = DDS_main_size;
	}
	DDS_data = (unsigned char*)malloc( DDS_full_size );
	/*	got the image data RAM, create or use an existing OpenGL texture handle	*/
	tex_ID = reuse_texture_ID;
	if( tex_ID == 0 )
	{
		glGenTextures( 1, &tex_ID );
	}
	/*  bind an OpenGL texture ID	*/
	glBindTexture( opengl_texture_type, tex_ID );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );	
	/*	do this for each face of the cubemap!	*/
	for( cf_target = ogl_target_start; cf_target <= ogl_target_end; ++cf_target )
	{
		if( buffer_index + DDS_full_size <= buffer_length )
		{
			unsigned int byte_offset = DDS_main_size;
			memcpy( (void*)DDS_data, (const void*)(&buffer[buffer_index]), DDS_full_size );
			buffer_index += DDS_full_size;
			/*	upload the main chunk	*/
			if( uncompressed )
			{
				/*	and remember, DXT uncompressed uses BGR(A),
					so swap to RGB(A) for ALL MIPmap levels	*/
				for( i = 0; i < DDS_full_size; i += block_size )
				{
					unsigned char temp = DDS_data[i];
					DDS_data[i] = DDS_data[i+2];
					DDS_data[i+2] = temp;
				}
				glTexImage2D(
					cf_target, 0,
					S3TC_type, width, height, 0,
					S3TC_type, GL_UNSIGNED_BYTE, DDS_data );
			} else
			{
				glCompressedTexImage2D(
					cf_target, 0,
					S3TC_type, width, height, 0,
					DDS_main_size, DDS_data );
			}
			/*	upload the mipmaps, if we have them	*/
			for( i = 1; i <= mipmaps; ++i )
			{
				int w, h, mip_size;
				w = width >> i;
				h = height >> i;
				if( w < 1 )
				{
					w = 1;
				}
				if( h < 1 )
				{
					h = 1;
				}
				/*	upload this mipmap	*/
				if( uncompressed )
				{
					mip_size = w*h*block_size;
					glTexImage2D(
						cf_target, i,
						S3TC_type, w, h, 0,
						S3TC_type, GL_UNSIGNED_BYTE, &DDS_data[byte_offset] );
				} else
				{
					mip_size = ((w+3)/4)*((h+3)/4)*block_size;
					glCompressedTexImage2D(
						cf_target, i,
						S3TC_type, w, h, 0,
						mip_size, &DDS_data[byte_offset] );
				}
				/*	and move to the next mipmap	*/
				byte_offset += mip_size;
			}
			/*	it worked!	*/
			fprintf(stderr, "DDS file loaded.\n");
		} else
		{
			glDeleteTextures( 1, & tex_ID );
			tex_ID = 0;
			cf_target = ogl_target_end + 1;
			fprintf(stderr, "DDS file was too small for expected image data.\n");
		}
	}/* end reading each face */
	free( DDS_data );
	if( tex_ID )
	{
		/*	did I have MIPmaps?	*/
		if( mipmaps > 0 )
		{
			/*	instruct OpenGL to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		} else
		{
			/*	instruct OpenGL _NOT_ to use the MIPmaps	*/
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		}
		/*	does the user want clamping, or wrapping?	*/
		if( is_repeats )
		{
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_R, GL_REPEAT );
		} else
		{
			unsigned int clamp_mode = GL_CLAMP_TO_EDGE;
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_S, clamp_mode );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_T, clamp_mode );
			glTexParameteri( opengl_texture_type, GL_TEXTURE_WRAP_R, clamp_mode );
		}
	}

quick_exit:
	/*	report success or failure	*/
	return tex_ID;
}