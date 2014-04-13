
////-----------------------------------------------------------------------------------------------------------------------------------------------------------------
////der grosse versuch tgas selbst zu laden
//typedef struct
//{
//	GLubyte	* imageData;									/* Image Data (Up To 32 Bits) */
//	GLuint	bpp;											/* Image Color Depth In Bits Per Pixel */
//	GLuint	width;											/* Image Width */
//	GLuint	height;											/* Image Height */
//	GLuint	texID;											/* Texture ID Used To Select A Texture */
//	GLuint	type;											/* Image Type (GL_RGB, GL_RGBA) */
//} Texture;
//
//typedef struct
//{
//	GLubyte Header[12];									/* TGA File Header */
//} TGAHeader;
//
//typedef struct
//{
//	GLubyte		header[6];								/* First 6 Useful Bytes From The Header */
//	GLuint		bytesPerPixel;							/* Holds Number Of Bytes Per Pixel Used In The TGA File */
//	GLuint		imageSize;								/* Used To Store The Image Size When Setting Aside Ram */
//	GLuint		temp;									/* Temporary Variable */
//	GLuint		type;
//	GLuint		Height;									/* Height of Image */
//	GLuint		Width;									/* Width ofImage */
//	GLuint		Bpp;									/* Bits Per Pixel */
//} TGA;
//
//
//TGAHeader tgaheader;									/* TGA header */
//TGA tga;												/* TGA image data */
//
//
//GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	/* Uncompressed TGA Header */
//GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	/* Compressed TGA Header */
//
//
//bool LoadUncompressedTGA(Texture * texture, const char * filename, FILE * fTGA)	/* Load an uncompressed TGA (note, much of this code is based on NeHe's */
//{
//	GLuint cswap;															/* TGA Loading code nehe.gamedev.net) */
//	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)				/* Read TGA header */
//	{
//		printf("Error could not read info header");							/* Display error */
//		if(fTGA != NULL)													/* if file is still open */
//		{
//			fclose(fTGA);													/* Close it */
//		}
//		return FALSE;														/* Return failure */
//	}
//
//	texture->width  = tga.header[1] * 256 + tga.header[0];					/* Determine The TGA Width	(highbyte*256+lowbyte) */
//	texture->height = tga.header[3] * 256 + tga.header[2];					/* Determine The TGA Height	(highbyte*256+lowbyte) */
//	texture->bpp	= tga.header[4];										/* Determine the bits per pixel */
//	printf(" %d", texture->bpp);
//	tga.Width		= texture->width;										/* Copy width into local structure */
//	tga.Height		= texture->height;										/* Copy height into local structure */
//	tga.Bpp			= texture->bpp;											/* Copy BPP into local structure */
//
//	if((texture->width <= 0) || (texture->height <= 0) || ((texture->bpp != 24) && (texture->bpp !=32)))	/* Make sure all information is valid */
//	{
//		printf("Error invalid texture information");						/* Display Error */
//		if(fTGA != NULL)													/* Check if file is still open */
//		{
//			fclose(fTGA);													/* If so, close it */
//		}
//		return FALSE;														/* Return failed */
//	}
//
//	if(texture->bpp == 24)													/* If the BPP of the image is 24... */
//	{
//		texture->type	= GL_RGB;											/* Set Image type to GL_RGB */
//	}
//	else																	/* Else if its 32 BPP */
//	{
//		texture->type	= GL_RGBA;											/* Set image type to GL_RGBA */
//	}
//
//	tga.bytesPerPixel	= (tga.Bpp / 8);									/* Compute the number of BYTES per pixel */
//	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		/* Compute the total amount of memory needed to store data */
//	texture->imageData	= (GLubyte	*) malloc(tga.imageSize);							/* Allocate that much memory */
//
//	if(texture->imageData == NULL)											/* If no space was allocated */
//	{
//		printf("Error could not allocate memory for image");				/* Display Error */
//		fclose(fTGA);														/* Close the file */
//		return FALSE;														/* Return failed */
//	}
//
//	if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	/* Attempt to read image data */
//	{
//		printf("Error could not read image data");							/* Display Error */
//		if(texture->imageData != NULL)										/* If imagedata has data in it */
//		{
//			free(texture->imageData);										/* Delete data from memory */
//		}
//		fclose(fTGA);														/* Close file */
//		return FALSE;														/* Return failed */
//	}
//
//	/* Byte Swapping Optimized By Steve Thomas */
//	for(cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
//	{
//		texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
//		texture->imageData[cswap] ^= texture->imageData[cswap+2];
//	}
//
//	fclose(fTGA);															/* Close file */
//	return TRUE;															/* Return success */
//}
//
//bool LoadTGA(Texture * texture,const char * filename)				/* Load a TGA file */
//{
//	FILE * fTGA;													/* File pointer to texture file */
//	fTGA = fopen(filename, "rb");									/* Open file for reading */
//
//	if(fTGA == NULL)												/* If it didn't open.... */
//	{
//		printf("Error could not open texture file");				/* Display an error message */
//		return FALSE;												/* Exit function */
//	}
//
//	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)			/* Attempt to read 12 byte header from file */
//	{
//		printf("Error could not read file header");					/* If it fails, display an error message */
//		if(fTGA != NULL)												/* Check to see if file is still open */
//		{
//			fclose(fTGA);												/* If it is, close it */
//		}
//		return FALSE;													/* Exit function */
//	}
//
//	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)	/* See if header matches the predefined header of */
//	{																/* an Uncompressed TGA image */
//		LoadUncompressedTGA(texture, filename, fTGA);					/* If so, jump to Uncompressed TGA loading code */
//	}
//	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)	/* See if header matches the predefined header of */
//	{																	/* an RLE compressed TGA image */
//		printf("The TGA File is compressed, please decompress manually");	/* If so, you need Compressed TGA loading code */
//	}
//	else																/* If header matches neither type */
//	{
//		printf("Error TGA file be type 2 or type 10 ");				/* Display an error */
//		fclose(fTGA);
//		return FALSE;												/* Exit function */
//	}
//	return TRUE;														/* All went well, continue on */
//}

