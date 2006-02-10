/********************************************************************
  TNetFolderServer.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef TNetFolderServer_H
#define TNetFolderServer_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TNamed.h>
#include <TApplication.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#define THREADRETURN NULL
#define THREADTYPE void*

class TNetFolderServer : public TNamed
{
protected:
   static TApplication *fApplication;
   Int_t                fPort;
public:
   void               StartServer(TApplication *app,Int_t port);
   static int         ResponseFunction(TSocket *socket);
protected:
   static int         CheckCommand(TSocket *socket,char *str);
   static TFolder    *ReadFolderPointer(TSocket *socket);
   static THREADTYPE  Server(void *arg);
   static THREADTYPE  ServerLoop(void *arg);

};

#endif   // TNetFolderServer_H
