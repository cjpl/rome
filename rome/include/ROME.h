/********************************************************************
  ROME.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROME_H
#define ROME_H
#include <Rtypes.h>

#define THREADRETURN NULL
#define THREADTYPE void*

#if defined( R__VISUAL_CPLUSPLUS )
#   define OS_WINNT
#endif

#if defined( OS_WINNT ) && defined( ROMEDLL_EXPORTS )
#   define DLLEXPORT __declspec(dllexport)
#else
#   define DLLEXPORT
#endif

#ifdef R__GNU
#   define SafeDeleteArray(p) { if (p) { delete [] p; p = 0; } }
#else
#   define SafeDeleteArray(p) { delete [] p; p = 0; }
#endif

#define WarningSuppression(x) { if (x) { int warning_suppression; warning_suppression = 0; } }

// This macro can be used for debugging like,
//  void test(int i)
//  {
//     if(i != 3) dbgprintf("i=%d", i);
//  }
#if defined( R__VISUAL_CPLUSPLUS )
#   define dbgprintf(...) (printf("%s:%u in %s:",__FILE__,__LINE__,__FUNCTION__),printf(" "__VA_ARGS__))
#else
#   define dbgprintf(...) (printf("%s:%u in %s:",__FILE__,__LINE__,__func__),printf(" "__VA_ARGS__))
#endif

#endif   // ROME_H

