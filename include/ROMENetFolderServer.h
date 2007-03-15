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
   Bool_t   fCopyAll;
   Bool_t   fEventStorageAvailable;
   Bool_t   fAllDataAvailable;

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
   ROMENetFolderServer();
   virtual ~ROMENetFolderServer(){}

   Bool_t   IsSocketClientRead(Int_t clientID) { return fSocketClientRead[clientID]; };
   Bool_t   IsCopyAll() { return fCopyAll; };
   void     SetCopyAll(bool copyAll);

   Bool_t   IsEventStorageAvailable() { return fEventStorageAvailable; };
   void     SetEventStorageAvailable(bool eventStorageAvailable) { fEventStorageAvailable = eventStorageAvailable; };

   Bool_t   IsAllDataAvailable() { return fAllDataAvailable; };
   void     SetAllDataAvailable(bool allDataAvailable) { fAllDataAvailable = allDataAvailable; };


protected:
   static Int_t      CheckCommand(TSocket *socket,char *str);
   static THREADTYPE Server(void *arg);
   static THREADTYPE ServerLoop(void *arg);

   ClassDef(ROMENetFolderServer, 0) // Customized TNetFolder for ROME
};

#endif   // ROMENetFolderServer_H
