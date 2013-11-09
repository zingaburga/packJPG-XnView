/*
 * Plugin example for XnView 1.65 or more
 *
 * When Xuser is built, put it in PluginsEx folder
 */

#include <stdio.h>
#include <windows.h>


/*** library includes ***/

#ifndef __cplusplus
typedef int bool;
#endif
#include "../packJPG/packjpglib.h"

#include "libyuv.h"


#ifdef __cplusplus
extern "C" {
#endif



/*** XnView stuff ***/
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) 
{
	switch (ul_reason_for_call) 
	{
	case DLL_PROCESS_ATTACH :
	case DLL_PROCESS_DETACH :
	case DLL_THREAD_ATTACH  :
	case DLL_THREAD_DETACH  :
		break;
  }
	return TRUE;
}

#define API __stdcall
#define DLLEXPORT __declspec(dllexport)

#define GFP_RGB	0
#define GFP_BGR	1

#define GFP_READ	0x0001
#define GFP_WRITE 0x0002

DLLEXPORT BOOL API gfpGetPluginInfo( DWORD version, LPSTR label, INT label_max_size, LPSTR extension, INT extension_max_size, INT * support )
{
	if ( version != 0x0002 )
		return FALSE; 

	strncpy( label, "packJPG Files", label_max_size ); 
	strcpy( extension, "pjg" ); 

	*support = GFP_READ;
	
	return TRUE; 
}


typedef struct {
		unsigned char * rgbData;
		unsigned int dataLen;
		int decodeStatus; // 0 = not started, 1 = started, 2 = finished
		
		unsigned int stride;
		//unsigned int width, height;
		//unsigned int bpp;
		struct pjglib_pjg_info info;
		unsigned char * imageStream;
		
		void* pjg;
	} MYDATA;

DLLEXPORT void * API gfpLoadPictureInit( LPCSTR filename )
{
	MYDATA * data = (MYDATA*)calloc( 1, sizeof(MYDATA) ); 
	
	#define ERROR_EXIT { free(data); return NULL; }
	if(!(data->pjg = pjglib_get_instance()))
		ERROR_EXIT
	pjglib_init_streams(data->pjg, (void*)filename, 0, 0, &data->imageStream, 1);
	
	if(!pjglib_pjg_get_info(data->pjg, &data->info))
		ERROR_EXIT
	
	// TODO: align stride??
	data->stride = data->info.width*3;
	data->dataLen = data->stride * data->info.height;
	
	return data; 
}


DLLEXPORT BOOL API gfpLoadPictureGetInfo( void * ptr, INT * pictype, INT * width, INT * height, INT * dpi, INT * bits_per_pixel, INT * bytes_per_line, BOOL * has_colormap, LPSTR label, INT label_max_size )
{
	MYDATA * data = (MYDATA *)ptr; 
	
	// fill out info
	*pictype = GFP_BGR; 
	*width = data->info.width;
	*height = data->info.height;
	*dpi = 72; // 68
	*bits_per_pixel = data->info.channels*8; // TODO: change
	*bytes_per_line = 3 * *width; 
	*has_colormap = FALSE; 
	strncpy( label, "packJPG 2.5 file", label_max_size ); 
	
	return TRUE; 
}

DLLEXPORT BOOL API gfpLoadPictureGetLine( void * ptr, INT line, unsigned char * buffer )
{
	MYDATA * data = (MYDATA *)ptr; 
	
	if(!data->decodeStatus)
	{
		unsigned int imageLen;
		data->decodeStatus = 1;
		// start decoding
		if(!pjglib_pjg_get_argb(data->pjg, &data->imageStream, &imageLen, NULL))
			return FALSE;
		
		if(!imageLen)
			return FALSE;
		
		// TODO: we could probably do the following line by line
		data->rgbData = (unsigned char*)malloc(data->dataLen);
		ARGBToRGB24(data->imageStream, data->info.width*4, data->rgbData, data->stride, data->info.width, data->info.height);
		// TODO: when is imageStream ever cleared?
	}
	memcpy(buffer, data->rgbData + data->stride*line, data->stride);
	if(line+1 == data->info.height)
		data->decodeStatus = 2;
	return TRUE; 
}

DLLEXPORT BOOL API gfpLoadPictureGetColormap( void * ptr, void * cmap )
{
	return FALSE; 
}

DLLEXPORT void API gfpLoadPictureExit( void * ptr )
{
	MYDATA * data = (MYDATA *)ptr; 
	pjglib_del_instance(data->pjg);
	if(data->decodeStatus != 2) {} // something wrong...
	free(data->rgbData); 
	free(data); 
}


#define FUNC_NOT_SUPPORTED { return FALSE; }
DLLEXPORT BOOL API gfpSavePictureIsSupported( INT width, INT height, INT bits_per_pixel, BOOL has_colormap ) FUNC_NOT_SUPPORTED
DLLEXPORT void * API gfpSavePictureInit( LPCSTR filename, INT width, INT height, INT bits_per_pixel, INT dpi, INT * picture_type, LPSTR label, INT label_max_size ) { return NULL; }
DLLEXPORT BOOL API gfpSavePicturePutLine( void * ptr, INT line, const unsigned char * buffer ) FUNC_NOT_SUPPORTED
DLLEXPORT void API gfpSavePictureExit( void * ptr ) {}

#ifdef __cplusplus
}
#endif

