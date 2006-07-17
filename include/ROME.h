/********************************************************************
  ROME.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROME_H
#define ROME_H
#include <Rtypes.h>

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
#endif   // ROME_H
