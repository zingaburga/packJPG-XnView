/*
 * Plugin example for XnView 1.65 or more
 *
 * When Xuser is built, put it in PluginsEx folder
 */

#include <stdio.h>
#include <windows.h>
#include <setjmp.h>


/*** library includes ***/

// needed for libjpeg
#define XMD_H
#define HAVE_BOOLEAN
#define INT16 short
#define INT32 long
#define boolean int
#include "../libjpeg-turbo/jpeglib.h"

#ifndef __cplusplus
typedef int bool;
#endif
#include "../packJPG/packjpglib.h"



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


/*** libjpeg stuff ***/

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

typedef struct {
		unsigned char * jpegFile;
		unsigned int jpegLen;
		struct jpeg_decompress_struct cinfo;
		struct my_error_mgr jerr;
		JSAMPARRAY jbuffer;
		int decodeStatus; // 0 = not started, 1 = started, 2 = finished
	} MYDATA; 

METHODDEF(void) libjpeg_error_exit (j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	char msg[512], msgTitle[256];
	
	if(cinfo->err->msg_code != 55) // don't display 'not a JPEG' error
	{
		vsprintf(msg, cinfo->err->jpeg_message_table[cinfo->err->msg_code], cinfo->err->msg_parm.s);
		sprintf(msgTitle, "JPEG Error (%d)", cinfo->err->msg_code);
		MessageBox(NULL, msg, msgTitle, 0); // TODO: get active window hWnd?
	}

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}
DLLEXPORT void API gfpLoadPictureExit( void * ptr );
static BOOL inline libjpeg_error(MYDATA* data)
{
	if (setjmp(((my_error_ptr)(data->cinfo.err))->setjmp_buffer)) {
		jpeg_abort((j_common_ptr)&(data->cinfo));
		// TODO: prevent crash?
		//jpeg_destroy_decompress(cinfo);
		//gfpLoadPictureExit(data); // - XnView will call this for us?
		return FALSE;
	}
	return TRUE;
}



DLLEXPORT void * API gfpLoadPictureInit( LPCSTR filename )
{
	MYDATA * data; 
	void* pjg;
	data = (MYDATA*)calloc( 1, sizeof(MYDATA) ); 

	#define ERROR_EXIT { free(data); return NULL; }
	if(!(pjg = pjglib_get_instance()))
		ERROR_EXIT
	pjglib_init_streams(pjg, (void*)filename, 0, 0, &(data->jpegFile), 1);
	if(!pjglib_convert_stream2mem(pjg, &(data->jpegFile), &data->jpegLen, NULL))
		ERROR_EXIT
	pjglib_del_instance(pjg);
	
	if(!data->jpegLen)
		ERROR_EXIT
	
	// packJPG process done, now do JPEG decode
	{
		// init JPEG
		struct jpeg_decompress_struct * cinfo = &(data->cinfo);
		struct my_error_mgr * jerr = &(data->jerr);
		cinfo->err = jpeg_std_error(&(jerr->pub));
		jerr->pub.error_exit = libjpeg_error_exit;
		/* Establish the setjmp return context for my_error_exit to use. */
		if(!libjpeg_error(data)) {
			// need to specifically clean up here
			jpeg_destroy_decompress(cinfo);
			free(data->jpegFile); 
			free(data); 
			return NULL;
		}
		jpeg_create_decompress(cinfo);
		jpeg_mem_src(cinfo, data->jpegFile, data->jpegLen);
	}
	return data; 
}


DLLEXPORT BOOL API gfpLoadPictureGetInfo( void * ptr, INT * pictype, INT * width, INT * height, INT * dpi, INT * bits_per_pixel, INT * bytes_per_line, BOOL * has_colormap, LPSTR label, INT label_max_size )
{
	MYDATA * data = (MYDATA *)ptr; 
	struct jpeg_decompress_struct * cinfo = &(data->cinfo);
	if(!libjpeg_error(data)) return FALSE;
	jpeg_read_header(cinfo, TRUE);
	
	// set decompression options
	cinfo->out_color_space = JCS_RGB; // TODO: check if correct
	jpeg_calc_output_dimensions(cinfo);
	
	
	// fill out info
	*pictype = GFP_RGB; 
	*width = cinfo->output_width;
	*height = cinfo->output_height;
	*dpi = 72; // 68
	*bits_per_pixel = cinfo->output_components*8;
	*bytes_per_line = *bits_per_pixel/8 * *width; 
	*has_colormap = FALSE; 
	strncpy( label, "packJPG 2.5 file", label_max_size ); 
	
	return TRUE; 
}

DLLEXPORT BOOL API gfpLoadPictureGetLine( void * ptr, INT line, unsigned char * buffer )
{
	MYDATA * data = (MYDATA *)ptr; 
	struct jpeg_decompress_struct * cinfo = &(data->cinfo);
	
	if(!data->decodeStatus)
	{
		if(!libjpeg_error(data)) return FALSE;
		// set decompression options
		cinfo->out_color_space = JCS_RGB;
		data->decodeStatus = 1;
		
		jpeg_start_decompress(cinfo);
		data->jbuffer = (*cinfo->mem->alloc_sarray)
				((j_common_ptr) cinfo, JPOOL_IMAGE, cinfo->output_components * cinfo->output_width, 1);
	}
	
	if(cinfo->output_scanline != (unsigned int)line) return FALSE;
	if(cinfo->output_scanline >= cinfo->output_height)
		return FALSE;
	//// assume only one buffer
	data->jbuffer[0] = buffer;
	jpeg_read_scanlines(cinfo, data->jbuffer, 1);
	
	if(cinfo->output_scanline == cinfo->output_height)
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
	struct jpeg_decompress_struct * cinfo = &(data->cinfo);
	if(libjpeg_error(data)) {
		if(cinfo) {
			if(data->decodeStatus) {
				if(data->decodeStatus == 1)
					jpeg_abort_decompress(cinfo);
				else
					jpeg_finish_decompress(cinfo);
			}
			jpeg_destroy_decompress(cinfo);
			cinfo = NULL;
		}
	}
	free(data->jpegFile); 
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

