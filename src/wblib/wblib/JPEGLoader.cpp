#include "JPEGLoader.h"

/*GLuint JPEGFile::GenerateOpenGLJPEGTexture(char * _fileName) {
  return 0;
}*/

struct wbLib::gl_texture_t *
  wbLib::JPEGLoader::ReadJPEGFromFile (const char *filename)
{
  struct gl_texture_t *texinfo = NULL;
  FILE *fp = NULL;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW j;
  int i;

  /* Open image file */
  fp = fopen(filename, "rb");
  if (!fp) {
    //fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
    std::cout << __FILE__ << " " << __LINE__ << " error: couldn't open: "
      << filename << std::endl;
    return NULL;
  }

  /* Create and configure decompressor */
  jpeg_create_decompress (&cinfo);
  cinfo.err = jpeg_std_error (&jerr);
  jpeg_stdio_src (&cinfo, fp);

  /*
   * NOTE: this is the simplest "readJpegFile" function. There
   * is no advanced error handling.  It would be a good idea to
   * setup an error manager with a setjmp/longjmp mechanism.
   * In this function, if an error occurs during reading the JPEG
   * file, the libjpeg abords the program.
   * See jpeg_mem.c (or RTFM) for an advanced error handling which
   * prevent this kind of behavior (http://tfc.duke.free.fr)
   */

  /* Read header and prepare for decompression */
  jpeg_read_header (&cinfo, TRUE);
  jpeg_start_decompress (&cinfo);

  /* Initialize image's member variables */
  texinfo = (struct gl_texture_t *)
    malloc (sizeof (struct gl_texture_t));
  texinfo->width = cinfo.image_width;
  texinfo->height = cinfo.image_height;
  texinfo->internalFormat = cinfo.num_components;

  if (cinfo.num_components == 1)
    texinfo->format = GL_LUMINANCE;
  else
    texinfo->format = GL_RGB;

  texinfo->texels = (GLubyte *)malloc (sizeof (GLubyte) * texinfo->width
			       * texinfo->height * texinfo->internalFormat);

  /* Extract each scanline of the image */
  for (i = 0; i < texinfo->height; ++i)
  {
    j = (texinfo->texels +
      ((texinfo->height - (i + 1)) * texinfo->width * texinfo->internalFormat));
    jpeg_read_scanlines (&cinfo, &j, 1);
  }

  /* Finish decompression and release memory */
  jpeg_finish_decompress (&cinfo);
  jpeg_destroy_decompress (&cinfo);

  fclose (fp);
  return texinfo;
}

/**
 * Loads file, creates OpenGL Texture, frees memory, returns OpenGL Texture ID
 * Returns 0 in case of error
 */
GLuint wbLib::JPEGLoader::LoadJPEGTexture(const char *filename)
{
  struct gl_texture_t *jpeg_tex = NULL;
  GLuint tex_id = 0;

  jpeg_tex = ReadJPEGFromFile(filename);

  if (jpeg_tex && jpeg_tex->texels)
  {
    /* Generate texture */
    glGenTextures (1, &jpeg_tex->id);
    glBindTexture (GL_TEXTURE_2D, jpeg_tex->id);

    /* Setup some parameters for texture filters and mipmapping */
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if 0
    glTexImage2D (GL_TEXTURE_2D, 0, jpeg_tex->internalFormat,
	    jpeg_tex->width, jpeg_tex->height, 0, jpeg_tex->format,
	    GL_UNSIGNED_BYTE, jpeg_tex->texels);
#else
    gluBuild2DMipmaps (GL_TEXTURE_2D, jpeg_tex->internalFormat,
		 jpeg_tex->width, jpeg_tex->height,
		 jpeg_tex->format, GL_UNSIGNED_BYTE, jpeg_tex->texels);
#endif

    tex_id = jpeg_tex->id;

    /* OpenGL has its own copy of texture data */
    free (jpeg_tex->texels);
    free (jpeg_tex);
  }

  return tex_id;
}

