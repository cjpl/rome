/********************************************************************
  ROMENetFolderServer.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMENetFolderServer_H
#define ROMENetFolderServer_H

#include <TNetFolderServer.h>

class ROMENetFolderServer : public TNetFolderServer
{
public:
   void StartServer(TApplication *app,Int_t port,const char* serverName);
   static int ResponseFunction(TSocket *socket);
protected:
   static int CheckCommand(TSocket *socket,char *str);
   static THREADTYPE Server(void *arg);
   static THREADTYPE ServerLoop(void *arg);

   ClassDef(ROMENetFolderServer, 0) // Customized TNetFolder for ROME
};

#endif   // ROMENetFolderServer_H
