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
   return TNetFolderServer::CheckCommand(socket,str);
#endif // ROOT_VERSION
   return 1;
}

THREADTYPE ROMENetFolderServer::Server(void *arg)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   TSocket *socket = (TSocket *) arg;

   while (ROMENetFolderServer::ResponseFunction(socket))
   {}
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
