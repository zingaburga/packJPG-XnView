// packJPGlib.h - function declarations for the packJPG library
#ifdef __cplusplus
	#define EXTERNC extern "C"
#else
	#define EXTERNC
#endif
#if defined BUILD_DLL
	#define EXPORT EXTERNC __declspec( dllexport )
#else
	#define EXPORT EXTERNC
#endif

struct pjglib_pjg_info {
	int channels;
	int width, height;
};

/* -----------------------------------------------
	function declarations: library only functions
	----------------------------------------------- */

EXPORT void* pjglib_get_instance( void );
EXPORT void pjglib_del_instance( void* p );
EXPORT bool pjglib_convert_stream2stream( void* p, char* msg );
EXPORT bool pjglib_convert_file2file( void* p, char* in, char* out, char* msg );
EXPORT bool pjglib_convert_stream2mem( void* p, unsigned char** out_file, unsigned int* out_size, char* msg );
EXPORT bool pjglib_decode_stream2mem( void* p, unsigned char** out_file, unsigned int* out_size, char* msg );
EXPORT void pjglib_init_streams( void* p, void* in_src, int in_type, int in_size, void* out_dest, int out_type );
EXPORT const char* pjglib_version_info( void );
EXPORT const char* pjglib_short_name( void );

EXPORT bool pjglib_pjg_get_info( void* p, struct pjglib_pjg_info* info );
EXPORT bool pjglib_pjg_get_argb( void* p, unsigned char** out_file, unsigned int* out_size, char* msg );

/* a short reminder about input/output stream types
   for the pjglib_init_streams() function
	
	if input is file
	----------------
	in_scr -> name of input file
	in_type -> 0
	in_size -> ignore
	
	if input is memory
	------------------
	in_scr -> array containg data
	in_type -> 1
	in_size -> size of data array
	
	if input is *FILE (f.e. stdin)
	------------------------------
	in_src -> stream pointer
	in_type -> 2
	in_size -> ignore
	
	vice versa for output streams! */
