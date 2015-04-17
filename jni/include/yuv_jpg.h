#ifndef _YUV_JPG_H
#define	_YUV_JPG_H

/*****************************************************************************
Copyright (C), 2014-2014, Jovision Tech. co., Ltd.
File Name:      yuv_jpg.h 
Author:         wxy(walker)				
Description:
2014-08-19
Compress a yv12 image into a JPEG image.					
*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef void * hJPG; // jpg instance handle

/*****************************************************************************
 *yuv_jpg_init:
 *Create a jpg compressor instance.
 *In:
 *return: return a handle to the newly-created instance, or NULL if an error 	  
*****************************************************************************/ 
hJPG yuv_jpg_init();

/*****************************************************************************
 *yuv_jpg_destroy:
 *Destroy a jpg compressor instance.
 *In:     hJPG h
 *Return:   	  
*****************************************************************************/
void yuv_jpg_destroy(hJPG h);

/*****************************************************************************
 *JAE_EncodeOneFrame:
 *Compress a yv12 image into a JPEG image.
 *In:     JAENC_HANDLE    h
 *In:     int jpegQual           // the image quality of the generated JPEG 
 *                               // image (1 = worst, 100 = best)
 *In:     unsigned char * pY
 *In:     unsigned char * pU
 *In:     unsigned char * pV
 *In:     int	iYPitch
 *In:     int	iUPitch
 *In:     int	iVPitch
 *In:     int iPixelFormat       // not used
 *In/Out: unsigned char ** ppJpg // address of a pointer to an image buffer
 *                               // that will receive the JPEG image
 *In/Out: unsigned long *piSize  // pointer to an unsigned long variable that
 *                               // holds the size of the JPEG image buffer
 *Return: 1 if successful, or -1 if an error occurred 
*****************************************************************************/
int yuv_jpg_do(hJPG h, int jpegQual,
               unsigned char * pY, unsigned char * pU, unsigned char * pV,
               int iYPitch, int iUPitch, int iVPitch,
               int iWidth, int iHeight, int iPixelFormat, 
               unsigned char ** ppJpg, unsigned long *piSize);

/*****************************************************************************
 *save_jpg:
 *1. Compress a yv12 image into a JPEG image.
 *2. write a jpg data to file
 *In:     char * pJPGFile        // jpg file name
 *In:     int jpegQual           // the image quality of the generated JPEG 
 *                               // image (1 = worst, 100 = best)
 *In:     unsigned char * pY
 *In:     unsigned char * pU
 *In:     unsigned char * pV
 *In:     int	iYPitch
 *In:     int	iUPitch
 *In:     int	iVPitch
 *In:     int iPixelFormat       // not used
 *Return: 1 if successful, or <= 0 if an error occurred 
*****************************************************************************/
int save_jpg(char * pJPGFile, int iQuality, 
             unsigned char * pY, unsigned char * pU, unsigned char * pV,
             int iYPitch, int iUPitch, int iVPitch,
             int iWidth, int iHeight, int iPixelFormat);


#ifdef __cplusplus
}
#endif

#endif // _YUV_JPG_H