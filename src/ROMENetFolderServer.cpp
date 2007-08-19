// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMENetFolderServer
//
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSocket.h>
#include <TServerSocket.h>
#include <TMessage.h>
#include <TThread.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "ROMEAnalyzer.h"
#include "ROMENetFolderServer.h"

ClassImp(ROMENetFolderServer)

//______________________________________________________________________________
ROMENetFolderServer::ROMENetFolderServer()
:TNetFolderServer()
,fCopyAll(kFALSE)
,fEventStorageAvailable(kFALSE)
,fAllDataAvailable(kFALSE)
{
   Int_t i;
   for (i = 0; i < kMaxSocketClients; i++) {
      fAcceptedSockets[i] = 0;
      fSocketClientRead[i] = false;
   }
}

//______________________________________________________________________________
int ROMENetFolderServer::CheckCommand(TSocket *socket,char *str)
{
   if (!socket->IsValid() || !gROME || !gNetFolderServerRunning)
      return 1;

   ROMENetFolderServer* localThis = static_cast<ROMENetFolderServer*>(gROME->GetNetFolderServer());

   // Check Command
   if (strncmp(str, "GetCurrentRunNumber", 19) == 0) {
      //get run number
      Long64_t runNumber = gROME->GetCurrentRunNumber();

      TMessage message(kMESS_ANY);
      message<<runNumber;
      socket->Send(message);
      return 1;
   }
   if (strncmp(str, "GetCurrentEventNumber", 21) == 0) {
      //get event number
      Long64_t eventNumber = gROME->GetCurrentEventNumber();

      TMessage message(kMESS_ANY);
      message<<eventNumber;
      socket->Send(message);
      return 1;
   }
   if (strncmp(str, "RequestNewEvent ", 16) == 0) {
      //check for new event
      Bool_t ret;
      ROMEString string = str;
      Long64_t runNumber = gROME->GetCurrentRunNumber();
      Long64_t eventNumber = gROME->GetCurrentEventNumber();

      ROMEString runStr = string(16,string.Length()-16);
      Long64_t oldRunNumber = runStr.ToLong64();
      int blank = runStr.Index(" ");
      ROMEString eventStr = runStr(blank+1,runStr.Length()-blank-1);
      Long64_t oldEventNumber = eventStr.ToLong64();

      ret = kFALSE;
      if ((oldRunNumber < runNumber || (oldRunNumber==runNumber && oldEventNumber < eventNumber)) &&
          localThis->IsEventStorageAvailable()) {
         gROME->UpdateObjectStorage();
         while (gROME && gNetFolderServerRunning && !gROME->IsObjectStorageUpdated())
            gSystem->Sleep(20);
         if (!gROME || !gNetFolderServerRunning) {
            ret = kFALSE;
         } else {
            ret = kTRUE;
         }
      }

      TMessage message(kMESS_ANY);
      message<<ret;
      socket->Send(message);
      return 1;
   }
   if (strncmp(str, "RequestEvent", 12) == 0) {
      //return current event
      Bool_t ret;
      ret = kFALSE;
      if (gROME && gNetFolderServerRunning && localThis->IsAllDataAvailable())
         ret = kTRUE;
      else if (localThis->IsEventStorageAvailable()) {
         gROME->UpdateObjectStorage();
         while (gROME && gNetFolderServerRunning && !gROME->IsObjectStorageUpdated())
            gSystem->Sleep(20);
         if (!gROME || !gNetFolderServerRunning) {
            ret = kFALSE;
         } else {
            ret = kTRUE;
         }
      }

      TMessage message(kMESS_ANY);
      message<<ret;
      socket->Send(message);
      return 1;
   }
   if (strncmp(str, "IsProgramTerminated", 19) == 0) {
      //return program termination flag
      Bool_t ret;
      ret = kFALSE;
      if (gROME->IsProgramTerminated())
         ret = kTRUE;

      TMessage message(kMESS_ANY);
      message<<ret;
      socket->Send(message);
      return 1;
   }
   return TNetFolderServer::CheckCommand(socket,str);
}

//______________________________________________________________________________
void ROMENetFolderServer::SetCopyAll(bool copyAll)
{ 
   Int_t i;
   fCopyAll = copyAll;
   for (i = 0; i < kMaxSocketClients; i++) {
      fSocketClientRead[i] = kFALSE;
   }
}

//______________________________________________________________________________
Int_t ROMENetFolderServer::Register(TSocket* socket)
{
   Int_t i;
   i = 0; // to suppress unused warning
   Int_t id = -1;
   for (i = 0; i < kMaxSocketClients; i++) {
      if(fAcceptedSockets[i] == 0) {
         fAcceptedSockets[i] = socket;
         id = i;
         break;
      }
   }
   if(id == -1)
      Error("Register", "Number of netfolder clients exceeded the maximum (%d)", kMaxSocketClients);
   return id;
}

//______________________________________________________________________________
void ROMENetFolderServer::UnRegister(TSocket* socket)
{
   Int_t id = FindId(socket);
   if(id != -1) {
      fAcceptedSockets[id] = 0;
      fSocketClientRead[id] = kFALSE;
   }
}

//______________________________________________________________________________
Int_t ROMENetFolderServer::FindId(TSocket* socket) const
{
   Int_t i;
   i = 0; // to suppress unused warning
   for (i = 0; i < kMaxSocketClients; i++) {
      if(fAcceptedSockets[i] == socket) {
         return i;
      }
   }
   return -1;
}
