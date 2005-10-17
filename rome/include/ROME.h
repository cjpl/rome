/********************************************************************
  ROME.h, M. Schneebeli PSI

  $Id:$

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

#endif   // ROME_H
