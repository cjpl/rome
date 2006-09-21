/********************************************************************
  ROMENetFolderServer.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMENetFolderServer_H
#define ROMENetFolderServer_H

#include "ROMEAnalyzer.h"
#include "TNetFolderServer.h"

class ROMENetFolderServer : public TNetFolderServer
{
protected:
   Bool_t   fLocks[kMaxSocketClients];
   TSocket *fAcceptedSockets[kMaxSocketClients];

public:
   void              StartServer(TApplication *app,Int_t port,const char* serverName);
   static Int_t      ResponseFunction(TSocket *socket);
   virtual void      UpdateFolders() = 0;
   virtual void      ConstructFolders(TSocket* socket) = 0;
   virtual void      DestructFolders(TSocket* socket) = 0;
   virtual Int_t     Register(TSocket* socket) = 0;
   virtual void      UnRegister(TSocket* socket) = 0;

public:
   ROMENetFolderServer()
   {
      Int_t i;
      for (i = 0; i < kMaxSocketClients; i++) {
         fLocks[i] = kFALSE;
         fAcceptedSockets[i] = 0;
      }
   }
   virtual ~ROMENetFolderServer(){}

protected:
   static Int_t      CheckCommand(TSocket *socket,char *str);
   static THREADTYPE Server(void *arg);
   static THREADTYPE ServerLoop(void *arg);

   ClassDef(ROMENetFolderServer, 0) // Customized TNetFolder for ROME
};

#endif   // ROMENetFolderServer_H
