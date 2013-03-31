// packJPGdll.h - function import declarations for the packJPG DLL
#define IMPORT __declspec( dllimport )

/* -----------------------------------------------
	function declarations: library only functions
	----------------------------------------------- */
	
IMPORT void* pjglib_get_instance( void );
IMPORT void pjglib_del_instance( void* p );
IMPORT bool pjglib_convert_stream2stream( void* p, char* msg );
IMPORT bool pjglib_convert_file2file( void* p, char* in, char* out, char* msg );
IMPORT bool pjglib_convert_stream2mem( void* p, unsigned char** out_file, unsigned int* out_size, char* msg );
IMPORT void pjglib_init_streams( void* p, void* in_src, int in_type, int in_size, void* out_dest, int out_type );
IMPORT const char* pjglib_version_info( void );
IMPORT const char* pjglib_short_name( void );

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
