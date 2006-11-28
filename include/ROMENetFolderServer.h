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
   TSocket *fAcceptedSockets[kMaxSocketClients];
   Bool_t   fSocketClientRead[kMaxSocketClients];

public:
   void              StartServer(TApplication *app,Int_t port,const char* serverName);
   static Int_t      ResponseFunction(TSocket *socket);
   virtual Bool_t    UpdateObjects() = 0;
   virtual void      ConstructObjects(TSocket* socket) = 0;
   virtual void      DestructObjects(TSocket* socket) = 0;
   virtual Int_t     Register(TSocket* socket) = 0;
   virtual void      UnRegister(TSocket* socket) = 0;

   TSocket          *GetAcceptedSockets(Int_t i) { return fAcceptedSockets[i]; };

public:
   ROMENetFolderServer()
   {
      Int_t i;
      for (i = 0; i < kMaxSocketClients; i++) {
         fAcceptedSockets[i] = 0;
         fSocketClientRead[i] = false;
      }
   }
   virtual ~ROMENetFolderServer(){}

   Bool_t   IsSocketClientRead(Int_t clientID) { return fSocketClientRead[clientID]; };


protected:
   static Int_t      CheckCommand(TSocket *socket,char *str);
   static THREADTYPE Server(void *arg);
   static THREADTYPE ServerLoop(void *arg);

   ClassDef(ROMENetFolderServer, 0) // Customized TNetFolder for ROME
};

#endif   // ROMENetFolderServer_H
