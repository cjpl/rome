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
int ROMENetFolderServer::ResponseFunction(TSocket *socket) {
   if (!socket->IsValid())
      return 0;

   // Read Command
   char str[200];
   if (socket->Recv(str, sizeof(str)) <= 0) {
      socket->Close();
      delete socket;
      return 0;
   }
   return CheckCommand(socket,str);
}

//______________________________________________________________________________
int ROMENetFolderServer::CheckCommand(TSocket *socket,char *str) {
   if (!socket->IsValid())
      return 1;

   ROMENetFolderServer* localThis = static_cast<ROMENetFolderServer*>(gROME->GetNetFolderServer());

   // Check Command
   if (strncmp(str, "GetCurrentRunNumber", 19) == 0) {
      //get run number
      Long64_t runNumber = gROME->GetCurrentRunNumber();

      TMessage message(kMESS_OBJECT);
      message<<runNumber;
      socket->Send(message);
      return 1;
   }
   if (strncmp(str, "GetCurrentEventNumber", 21) == 0) {
      //get event number
      Long64_t eventNumber = gROME->GetCurrentEventNumber();

      TMessage message(kMESS_OBJECT);
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
      Long64_t oldRunNumber = runStr.ToLong(); // --> This should be ToLong64
      int blank = runStr.Index(" ");
      ROMEString eventStr = runStr(blank+1,runStr.Length()-blank-1);
      Long64_t oldEventNumber = eventStr.ToLong(); // --> This should be ToLong64

      ret = kFALSE;
      if ((oldRunNumber<runNumber || oldRunNumber==runNumber && oldEventNumber<eventNumber) && localThis->IsEventStorageAvailable()) {
         gROME->UpdateObjectStorage();
         while (!gROME->IsObjectStorageUpdated())
            gSystem->Sleep(20);
         ret = kTRUE;
      }

      TMessage message(kMESS_OBJECT);
      message<<ret;
      socket->Send(message);
      return 1;
   }
   if (strncmp(str, "RequestEvent", 12) == 0) {
      //return current event
      Bool_t ret;
      ret = kFALSE;
      if (localThis->IsAllDataAvailable())
         ret = kTRUE;
      else if (localThis->IsEventStorageAvailable()) {
         gROME->UpdateObjectStorage();
         while (!gROME->IsObjectStorageUpdated())
            gSystem->Sleep(20);
         ret = kTRUE;
      }

      TMessage message(kMESS_OBJECT);
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

      TMessage message(kMESS_OBJECT);
      message<<ret;
      socket->Send(message);
      return 1;
   }
   return TNetFolderServer::CheckCommand(socket,str);
}

//______________________________________________________________________________
THREADTYPE ROMENetFolderServer::Server(void *arg)
{
   TSocket *socket = (TSocket *) arg;
   ROMENetFolderServer* localThis = gROME->GetNetFolderServer();

   if (!socket->IsValid())
      return THREADRETURN;

   if(localThis->Register(socket) == -1)
      return THREADRETURN;

   localThis->ConstructObjects(socket);
   while (ROMENetFolderServer::ResponseFunction(socket))
   {}
   localThis->DestructObjects(socket);
   localThis->UnRegister(socket);
   return THREADRETURN;
}

//______________________________________________________________________________
THREADTYPE ROMENetFolderServer::ServerLoop(void *arg)
{
// Server loop listening for incoming network connections on port
// specified by command line option -s. Starts a searver_thread for
// each connection.
   int port = *(int*)arg;
   TServerSocket *lsock = new TServerSocket(port, kTRUE);
   if (!lsock->IsValid()) {
      switch(lsock->GetErrorCode()) {
         case -1:
            ROMEPrint::Error("Error: Low level socket() call failed. Failed to connect port %d\n", port);
            break;
         case -2:
            ROMEPrint::Error("Error: Low level bind() call failed. Failed to connect port %d\n", port);
            break;
         case -3:
            ROMEPrint::Error("Error: Low level listen() call failed. Failed to connect port %d\n", port);
            break;
         default:
            ROMEPrint::Error("Error: Failed to connect port %d\n", port);
            break;
      };
      return THREADRETURN;
   }

   do {
      TSocket *sock = lsock->Accept();

      TThread *thread = new TThread("Server", ROMENetFolderServer::Server, sock);
      thread->Run();

   } while (1);
   return THREADRETURN;
}

//______________________________________________________________________________
void ROMENetFolderServer::StartServer(TApplication *app,Int_t port,const char* serverName)
{
// start Socket server loop
   fApplication = app;
   fPort = port;
   fServerName = serverName;
   TThread *thread = new TThread("server_loop", ROMENetFolderServer::ServerLoop, &fPort);
   thread->Run();
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
