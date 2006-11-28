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
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
#   include <TThread.h>
#endif // ROOT_VERSION
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "ROMEAnalyzer.h"
#include "ROMENetFolderServer.h"

ClassImp(ROMENetFolderServer)

int ROMENetFolderServer::ResponseFunction(TSocket *socket) {
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
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
#endif // ROOT_VERSION
   return 1;
}

int ROMENetFolderServer::CheckCommand(TSocket *socket,char *str) {
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   if (!socket->IsValid())
      return 1;

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
      if (oldRunNumber<runNumber || oldRunNumber==runNumber && oldEventNumber<eventNumber) {
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
   return TNetFolderServer::CheckCommand(socket,str);
#endif // ROOT_VERSION
   return 1;
}

THREADTYPE ROMENetFolderServer::Server(void *arg)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
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

#endif // ROOT_VERSION
   return THREADRETURN;
}


THREADTYPE ROMENetFolderServer::ServerLoop(void *arg)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
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
#endif // ROOT_VERSION
   return THREADRETURN;
}

void ROMENetFolderServer::StartServer(TApplication *app,Int_t port,const char* serverName)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
// start Socket server loop
   fApplication = app;
   fPort = port;
   fServerName = serverName;
   TThread *thread = new TThread("server_loop", ROMENetFolderServer::ServerLoop, &fPort);
   thread->Run();
#endif // ROOT_VERSION
}
