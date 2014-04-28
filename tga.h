/*
 * Nehe Lesson 33 Code (ported to Linux//GLX by Patrick Schubert 2003
 * with help from the lesson 1 basecode for Linux/GLX by Mihael Vrbanec)
 *
 * portet for openNi by Matthias Max
 */

#ifndef __TGA_H__
#define __TGA_H__

#include <XnCppWrapper.h>

#ifndef USE_GLES
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif
#else
	#include "opengles.h"
#endif

typedef struct
{
	GLubyte	* imageData;									/* Image Data (Up To 32 Bits) */
	GLuint	bpp;											/* Image Color Depth In Bits Per Pixel */
	GLuint	width;											/* Image Width */
	GLuint	height;											/* Image Height */
	GLuint	texID;											/* Texture ID Used To Select A Texture */
	GLuint	type;											/* Image Type (GL_RGB, GL_RGBA) */
} Texture;

typedef struct
{
	GLubyte Header[12];									/* TGA File Header */
} TGAHeader;

typedef struct
{
	GLubyte		header[6];								/* First 6 Useful Bytes From The Header */
	GLuint		bytesPerPixel;							/* Holds Number Of Bytes Per Pixel Used In The TGA File */
	GLuint		imageSize;								/* Used To Store The Image Size When Setting Aside Ram */
	GLuint		temp;									/* Temporary Variable */
	GLuint		type;
	GLuint		Height;									/* Height of Image */
	GLuint		Width;									/* Width ofImage */
	GLuint		Bpp;									/* Bits Per Pixel */
} TGA;




bool LoadUncompressedTGA(Texture *,const char *,FILE *);	/* Load an Uncompressed file */

bool LoadTGA(Texture * texture,const char * filename);		/* Check if File is compressed or not */

#endif





