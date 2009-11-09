#ifndef SAFE_STDIO_HEADER
#define SAFE_STDIO_HEADER
#ifdef __CELLOS_LV2__
#include <stdio.h>
#include <string.h> 
#include <math.h>
//Yang Jianguo
//#define strncpy_s strncpy

#define _vsnprintf sprintf_s  // is this correct?  or should only be vsprintf, what is the n?
#define vsprintf_s sprintf_s
#define stricmp(a, b) strcasecmp((a), (b))
#define  _stricmp strcmp
#define  _strnicmp strncmp
#define sscanf_s sscanf
#define  _stricmp strcmp
#define _isnan isnan
#define strncat_s(a, b, c, d) strcat_s(a, b, c)
typedef int errno_t;

#define strlwr(x)
//typedef unsigned long long LARGE_INTEGER;

//inline int QueryPerformanceFrequency(unsigned long long *f)
//{return -1;}

inline int strncpy_s( char *strDest,size_t sizeInBytes,const char *strSource,size_t count)
{
	if (	strDest		== NULL ||
			strSource	== NULL ||
			sizeInBytes == 0	)
	{
		return -1;
	}

	if ( sizeInBytes < count )
	{
		strDest[0] = 0;
		return -1;
	}

	strncpy( strDest, strSource, count );
	return 0;
}

inline int sprintf_s( char * _DstBuf, size_t _DstSize, const char * _Format, ...)
{
	if ( _DstBuf == NULL || _Format == NULL )
	{
		return -1;
	}

	va_list arg;
	va_start( arg, _Format );
	int r = vsprintf( _DstBuf, _Format, arg );
	va_end(arg);

	return r;
}
inline void strcat_s(char* dest, size_t size, const char* src)
{
	strcat(dest, src);
}

inline int fopen_s( FILE ** _File, const char * _Filename, const char * _Mode)
{
	FILE* fp = fopen( _Filename, _Mode );
	if ( fp )
	{
		*_File = fp;
		return 0;
	}
	return -1;
}

inline void strcpy_s(char* dest, size_t size, const char* src)
{
	strncpy(dest, src, size);
}
template <size_t size>
inline int sprintf_s(
			  char (&buffer)[size],
			  const char *format,
			  ... 
			  ) // C++ only
{

	va_list arg;
	va_start( arg, format );
	int r = vsprintf( buffer, format, arg );
	va_end(arg);

	return r;
}
#define __CELLOS_LV2__INLINE inline __attribute__((always_inline))
	//! \brief platform-specific finiteness check (not INF or NAN)
	__CELLOS_LV2__INLINE bool isFinite(float a)
	{
		return (!(isnan(a)||isinf(a)));
	}

	//! \brief platform-specific finiteness check (not INF or NAN)
	__CELLOS_LV2__INLINE bool isFinite(double a)
	{
		return (!(isnan(a)||isinf(a)));
	}
#define _finite(x) isFinite(x)
#endif
#endif