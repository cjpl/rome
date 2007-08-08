/********************************************************************
  ROME.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROME_H
#define ROME_H
#include <Rtypes.h>
#include <Riostream.h>
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

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

#if defined(R__GNU) && !defined(__MAKECINT__)
#   define G_GNUC_PRINTF( format_idx, arg_idx )                 \
   __attribute__((format (printf, format_idx, arg_idx)))
#   define G_GNUC_SCANF( format_idx, arg_idx )          \
   __attribute__((format (scanf, format_idx, arg_idx)))
#   define G_GNUC_FORMAT( arg_idx )             \
   __attribute__((format_arg (arg_idx)))
#else
#   define G_GNUC_PRINTF( format_idx, arg_idx )
#   define G_GNUC_SCANF( format_idx, arg_idx )
#   define G_GNUC_FORMAT( arg_idx )
#endif

#define WarningSuppression(x) { if (x) { int warning_suppression; warning_suppression = 0; } }

// This macro can be used for debugging like,
//  void test(int i)
//  {
//     if(i != 3) dbgprintf("i=%d", i);
//     if(i != 3) dbgcout<<"i="<<i<<endl;
//  }
#if defined( R__VISUAL_CPLUSPLUS )
// The next line does not compile
//#   define dbgprintf(...) (printf("=== %s:%u in %s:",gSystem->BaseName(__FILE__),__LINE__,__FUNCTION__),printf(" "__VA_ARGS__))
#   define dbgcout cout<<"=== "<<gSystem->BaseName(__FILE__)<<":"<<__LINE__<<" in "<<__FUNCTION__<<": "
#else
#   define dbgprintf(...) (printf("=== %s:%u in %s:",gSystem->BaseName(__FILE__),__LINE__,__func__),printf(" "__VA_ARGS__))
#   define dbgcout cout<<"==== "<<gSystem->BaseName(__FILE__)<<":"<<__LINE__<<" in "<<__func__<<": "
#endif

#endif   // ROME_H

