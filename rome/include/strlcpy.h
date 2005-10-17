/********************************************************************\

   Name:         strlcpy.h
   Created by:   Stefan Ritt

   Contents:     Header file for strlcpy.c

   $Log$
   Revision 1.1  2005/05/11 12:50:02  schneebeli_m
   added strlcpy

   Revision 1.1  2005/05/09 09:12:06  ritt
   Initial revision

\********************************************************************/

#ifndef EXPRT
#if defined(EXPORT_DLL)
#define EXPRT __declspec(dllexport)
#else
#define EXPRT
#endif
#endif

size_t EXPRT strlcpy(char *dst, const char *src, size_t size);
size_t EXPRT strlcat(char *dst, const char *src, size_t size);
