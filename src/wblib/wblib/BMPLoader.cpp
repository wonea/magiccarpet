#include "BMPLoader.h"

void wbLib::BMPLoader::screenShot(int _x, int _y, int _width, int _height, const char * _filename) {

  BitmapFileHeader bf;
  BitmapInfoHeader bi;

  unsigned char * image = NULL;
  std::ofstream outputStream;

  //unsigned char * image = (unsigned char*)malloc(sizeof(unsigned char)*_width*_height*3);
  image = new unsigned char[_width*_height*3];

  //FILE *file = fopen(fname, "wb");
  outputStream.open(_filename, std::ios_base::trunc | std::ios_base::binary);

  if (outputStream.fail()) {  
    std::cout << __FILE__ << " " << __LINE__ << " Could not create " << _filename << std::endl;
    return; 
  }

  if (image != NULL) {
    
    glReadPixels(_x, _y, _width, _height, GL_BGR_EXT, GL_UNSIGNED_BYTE, image);

    //memset(&bf, 0, sizeof(bf));
    bf.bfType = 19778; //'MB';
    bf.bfSize = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + _width * _height * 3;
    bf.bfReserved1 = 0;
    bf.bfReserved1 = 0;
    bf.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);

    //memset(&bi, 0, sizeof(bi));
    bi.biSize = sizeof(BitmapInfoHeader);
    bi.biWidth = _width;
    bi.biHeight = _height;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = 0;
    bi.biSizeImage = _width * _height * 3;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    //fwrite(&bf, sizeof(bf), 1, file);
    outputStream.write((const char *)&bf, sizeof(BitmapFileHeader));
    //fwrite(&bi, sizeof(bi), 1, file);
    outputStream.write((const char *)&bi, sizeof(BitmapInfoHeader));
    //fwrite(image, sizeof(unsigned char), _width*_height*3, file);
    outputStream.write((const char *)image, _width * _height * 3);

    //fclose(file);
    outputStream.flush();
    outputStream.clear();
    outputStream.close();
  
    //free( image );
    delete [] image;
    image = NULL;
  }
  return;
}

int wbLib::BMPLoader::screenshot(int _x, int _y, int _width, int _height, const char *_filename)
{
    BitmapFileHeader bf;
    BitmapInfoHeader bi;

		std::cout << "sizeof(BitmapFileHeader): " << sizeof(BitmapFileHeader) << std::endl;
		std::cout << "sizeof(bf): " << sizeof(bf) << std::endl;
  	std::cout << "sizeof(BitmapInfoHeader): " << sizeof(BitmapInfoHeader) << std::endl;
		std::cout << "sizeof(bi): " << sizeof(bi) << std::endl;

    unsigned char *image = (unsigned char*)malloc(sizeof(unsigned char)*_width*_height*3);
    FILE *file = fopen(_filename, "wb");

    if (image != NULL)
    {
        if (file != NULL)
        {
            glReadPixels(_x, _y, _width, _height, GL_BGR_EXT, GL_UNSIGNED_BYTE, image );

            memset( &bf, 0, sizeof( bf ) );
            memset( &bi, 0, sizeof( bi ) );

            bf.bfType = 19778; // 2 Byte
            bf.bfSize = sizeof(bf) + sizeof(bi) + _width * _height * 3; // 4 Byte
						bf.bfReserved1 = 0; // 2 Byte
				    bf.bfReserved1 = 0; // 2 Byte
            bf.bfOffBits = sizeof(bf) + sizeof(bi); // 2 Byte

            bi.biSize = sizeof(bi);
            bi.biWidth = _width;
            bi.biHeight = _height;
            bi.biPlanes = 1;
            bi.biBitCount = 24;
            bi.biCompression = 0;
						bi.biSizeImage = _width * _height * 3;
						bi.biXPelsPerMeter = 0;
						bi.biYPelsPerMeter = 0;
						bi.biClrUsed = 0;
						bi.biClrImportant = 0;

            fwrite(&bf, sizeof(bf), 1, file);
            fwrite(&bi, sizeof(bi), 1, file);
            fwrite(image, sizeof(unsigned char), _height * _width * 3, file);

            fclose(file);
        }
        free(image);
    }
    return 0;
}

int wbLib::BMPLoader::load(const char * _fileName) {
  char * buffer = NULL;
  unsigned char * bufferPtr = NULL;
  int size = 0;
  int imageDataSize = 0;
  unsigned int textureObject = 0;
  BitmapFileHeader * bf = NULL;
  BitmapInfoHeader * bi = NULL;
  unsigned char * imageData = NULL;

	std::ifstream file;
	file.open(_fileName, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		return -1;
	}
	size = file.tellg();
	file.seekg (0, std::ios::beg);

	buffer = new char[size];
	file.read(buffer, size);

	if (!file.good()) {
		delete[] buffer;
		buffer = NULL;
	}

	file.close();

  bf = (BitmapFileHeader *)buffer;
  std::cout << "BitmapFileHeader" << std::endl;
  std::cout << "Type: " << bf->bfType << std::endl;
  std::cout << "Size: " << bf->bfSize << std::endl;
  std::cout << "Reserved1: " << bf->bfReserved1 << std::endl;
  std::cout << "Reserved2: " << bf->bfReserved2 << std::endl;
  std::cout << "OffBits: " << bf->bfOffBits << std::endl; // offset auf die Daten in Byte vom Dateianfang aus
  std::cout << std::endl;

  bufferPtr = (unsigned char *)buffer;
  bufferPtr += sizeof(BitmapFileHeader);
  bi = (BitmapInfoHeader *)bufferPtr;
  std::cout << "BitmapInfoHeader" << std::endl;
  std::cout << "Size: " << bi->biSize << std::endl;
  std::cout << "Width: " << bi->biWidth << std::endl;
  std::cout << "Height: " << bi->biHeight << std::endl;
  std::cout << "Planes: " << bi->biPlanes << std::endl;
  std::cout << "BitCount: " << bi->biBitCount << std::endl;
  std::cout << "Compression: " << bi->biCompression << std::endl;
  std::cout << "SizeImage: " << bi->biSizeImage << std::endl;
  std::cout << "XPelsPerMeter: " << bi->biXPelsPerMeter << std::endl;
  std::cout << "YPelsPerMeter: " << bi->biYPelsPerMeter << std::endl;
  std::cout << "biClrUsed: " << bi->biClrUsed << std::endl;
  std::cout << "biClrImportant: " << bi->biClrImportant << std::endl;

  // Bail out in case of non covered bits per pixel
  switch (bi->biBitCount) {
    case 24:
      break;

    case 1:
    case 4:
    case 8:
    case 16:
    case 32:
    default:
      std::cout << "BMP Loader: uncovered bits per pixel" << std::endl;
      if (NULL != buffer) {
        delete[] buffer;
        buffer = NULL;
      }
      if (NULL != imageData) {
        delete[] imageData;
        imageData = NULL;
      }
      return 0;
  };

  // Bail out in case of non covered compression
  switch (bi->biCompression) {
    case 0:
      break;

    case 1:
    case 2:
    case 3:
    default:
      std::cout << "BMP Loader: uncovered compression" << std::endl;
      if (NULL != buffer) {
        delete[] buffer;
        buffer = NULL;
      }
      if (NULL != imageData) {
        delete[] imageData;
        imageData = NULL;
      }
      return 0;
  }

  // TODO: non power of two?
  // TODO: image bigger than maximal allowed texture?

  // Größe berechnen (24 Bit bzw. 3 Byte per Pixel).
  imageDataSize = bi->biWidth * bi->biHeight * 3;
  
  imageData = new unsigned char[imageDataSize];
  //bi->biWidth = 64;
  //bi->biHeight = 64;
  //unsigned char imageData[64][64][3];
  //imageDataSize = 64*64*3;

  // Set pointer onto image data
  bufferPtr = (unsigned char *)buffer;
  bufferPtr += bf->bfOffBits;

  // Swizzle colors (bgr -> rgb)
  for (int i = 0; i < imageDataSize; i += 3) { 
    //imageData[i  ] = (unsigned char)200;//bufferPtr[i+2];
    ////std::cout << (unsigned char)imageData[i  ] << std::endl;
    //imageData[i+1] = (unsigned char)1;//bufferPtr[i+1];
    //imageData[i+2] = (unsigned char)1;//bufferPtr[i  ];

    imageData[i  ] = bufferPtr[i+2];
    imageData[i+1] = bufferPtr[i+1];
    imageData[i+2] = bufferPtr[i  ];
  }

 // glEnable(GL_TEXTURE_2D);    
 // glGenTextures(1, &textureObject);
 // glBindTexture(GL_TEXTURE_2D, textureObject);
 // //glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
 // //::CheckErrors("");

 // // Generate The Texture
 // //  void glTexImage2D( GLenum target,
 // //  GLint level,
 // //  GLint internalformat,
 // //  GLsizei width,
 // //  GLsizei height,
 // //  GLint border,
 // //  GLenum format,
 // //  GLenum type,
 // //  const GLvoid *pixels )
 // //
 // // The last three values supply the data from which the texture is 
 // // created plus the information how this data is arranged and how it
 // // should be interpreted by OpenGL.
 // // format - how many parts make up one texel (e.g. GL_RGB, one
 // // texel is specified by a R, G and B information)
 // // type - which datatype is used to specify one information (e.g. when
 // // GL_RGB is used for format, which datatype is used to store the R, G and
 // // B components)
 // // buffer - the memory location that holds the data. The data is arranged
 // // as a successive stream of [format] information which is stored using
 // // [type] datatype
 // glTexImage2D(GL_TEXTURE_2D, 
 //   0,
 //   //png_colorCount, 
 //   //GL_RGB,
 //   3,
 //   bi->biWidth, 
 //   bi->biHeight, 
 //   0, // border
 //   //GL_LUMINANCE_ALPHA,
 //   GL_RGB,
 //   //GL_GREEN|GL_RED,

 //   //GL_SHORT,
 //   //GL_UNSIGNED_INT,
 //   GL_UNSIGNED_BYTE,
 //   //&buffer[0][0][0]);
 //   imageData);

 // //::CheckErrors("");

 // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
 // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
 // //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 // //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//
	//glDisable(GL_TEXTURE_2D);  

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &textureObject);
  glBindTexture(GL_TEXTURE_2D, textureObject);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                 GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                 GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bi->biWidth, 
              bi->biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 
              (void *)imageData);

  

  /*const int checkImageWidth = 64;
  const int checkImageHeight = 64;
  unsigned int c;
  unsigned char checkImage[checkImageHeight][checkImageWidth][4];
  for (int i = 0; i < checkImageHeight; i++) {
      for (int j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
  }
 
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &textureObject);
  glBindTexture(GL_TEXTURE_2D, textureObject);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
                 GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                 GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, 
              checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
              checkImage);*/

  if (NULL != buffer) {
	  delete[] buffer;
	  buffer = NULL;
	}

  if (NULL != imageData) {
    delete[] imageData;
    imageData = NULL;
  }

  if (glIsTexture(textureObject)) {
    std::cout << "Texture created from bitmap" << std::endl;
  } else {
    std::cout << "Texture creation failed" << std::endl;
  }
	
	return textureObject;
}
