/********************************************************************
  ROME.h, M. Schneebeli PSI

  $Log$
  Revision 1.7  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.6  2005/02/21 21:29:07  sawada

  Changed OS specifying macros
  Support for DEC,Ultrix,FreeBSD,Solaris

  Revision 1.5  2004/12/02 17:46:43  sawada
  Macintosh port

  Revision 1.4  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

  Revision 1.3  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROME_H
#define ROME_H
#include <Rtypes.h>

#if defined( R__VISUAL_CPLUSPLUS )
#define OS_WINNT
#endif

#if defined( OS_WINNT ) && defined( ROMEDLL_EXPORTS )
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

// General
#define gShowTime 1

#endif   // ROME_H
