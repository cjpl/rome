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
   virtual Bool_t    UpdateObjects() = 0;
   virtual void      ConstructObjects(TSocket* socket) = 0;
   virtual void      DestructObjects(TSocket* socket) = 0;
   virtual Int_t     Register(TSocket* socket);
   virtual void      UnRegister(TSocket* socket);
   virtual Int_t     FindId(TSocket* socket) const;
   TSocket          *GetAcceptedSockets(Int_t i) const { return fAcceptedSockets[i]; }

private:
   ROMENetFolderServer(const ROMENetFolderServer &server); // not implemented
   ROMENetFolderServer &operator=(const ROMENetFolderServer &rhs); // not implemented

public:
   ROMENetFolderServer();
   virtual ~ROMENetFolderServer(){}

   Bool_t   IsSocketClientRead(Int_t clientID) const { return fSocketClientRead[clientID]; }
   Bool_t   IsCopyAll() const { return fCopyAll; }
   void     SetCopyAll(bool copyAll);

   Bool_t   IsEventStorageAvailable() const { return fEventStorageAvailable; }
   void     SetEventStorageAvailable(bool eventStorageAvailable) { fEventStorageAvailable = eventStorageAvailable; }

   Bool_t   IsAllDataAvailable() const { return fAllDataAvailable; }
   void     SetAllDataAvailable(bool allDataAvailable) { fAllDataAvailable = allDataAvailable; }


protected:
   static Int_t      CheckCommand(TSocket *socket,char *str);

   ClassDef(ROMENetFolderServer, 0) // Customized TNetFolder for ROME
};

#endif   // ROMENetFolderServer_H
