// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// TNetFolderServer
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
#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TFolder.h>
#include <TObjArray.h>
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
#   include <TThread.h>
#endif // ROOT_VERSION
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include <TNetFolderServer.h>
#include <Riostream.h>

TApplication *TNetFolderServer::fApplication;
TString TNetFolderServer::fServerName;

ClassImp(TNetFolderServer)

TFolder *TNetFolderServer::ReadFolderPointer(TSocket *socket)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   //read pointer to current folder
   TMessage *message = 0;
   socket->Recv(message);
   size_t p;
   *message>>p;
   delete message;
   return (TFolder*)p;
#else
   return NULL;
#endif // ROOT_VERSION
}

int TNetFolderServer::ResponseFunction(TSocket *socket) {
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
int TNetFolderServer::CheckCommand(TSocket *socket,char *str) {
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   // Check Command
   if (strcmp(str, "GetListOfFolders") == 0) {
      TMessage message(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);
      if (folder==NULL) {
         message.Reset(kMESS_OBJECT);
         message.WriteObject(NULL);
         socket->Send(message);
         return 1;
      }

      //get folder names
      TObject *obj;
      TObjArray *names = new TObjArray(100);

      TCollection *folders = folder->GetListOfFolders();
      TIterator *iterFolders = folders->MakeIterator();
      TString name;
      while ((obj = iterFolders->Next())) {
         name = obj->GetName();
         names->Add(new TObjString(name.Data()));
      }

      //write folder names
      message.Reset(kMESS_OBJECT);
      message.WriteObject(names);
      socket->Send(message);

      for (int i = 0; i < names->GetLast() + 1; i++)
         delete(TObjString *) names->At(i);

      delete names;
      return 1;
   }
   else if (strncmp(str, "FindObject", 10) == 0) {
      TMessage message(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);
      if (folder==NULL) {
         message.Reset(kMESS_OBJECT);
         message.WriteObject(NULL);
         socket->Send(message);
         return 1;
      }

      //get object
      TObject *obj;
      if (strncmp(str+10, "Any", 3) == 0) {
         obj = folder->FindObjectAny(str+14);
      }
      else {
         obj = folder->FindObject(str+11);
      }

      //write object
      if (!obj) {
         socket->Send("Error");
      } else {
         message.Reset(kMESS_OBJECT);
         message.WriteObject(obj);
         socket->Send(message);
      }
      return 1;
   }

   else if (strncmp(str, "FindFullPathName", 16) == 0) {
      TMessage message(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);
      if (folder==NULL) {
         message.Reset(kMESS_OBJECT);
         message.WriteObject(NULL);
         socket->Send(message);
         return 1;
      }

      //find path
      const char* path = folder->FindFullPathName(str+17);

      //write path
      if (!path) {
         socket->Send("Error");
      } else {
         TObjString *obj = new TObjString(path);
         message.Reset(kMESS_OBJECT);
         message.WriteObject(obj);
         socket->Send(message);
         delete obj;
      }
      return 1;
   }

   else if (strncmp(str, "Occurence", 9) == 0) {
      TMessage message(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);
      if (folder==NULL) {
         message.Reset(kMESS_OBJECT);
         message.WriteObject(NULL);
         socket->Send(message);
         return 1;
      }

      //read object
      message.Reset(kMESS_OBJECT);
      TMessage *answer;
      socket->Recv(answer);
      TObject *obj = ((TObject*) answer->ReadObject(answer->GetClass()));
      delete answer;

      //get occurence
      Int_t retValue = folder->Occurence(obj);

      //write occurence
      message.Reset(kMESS_OBJECT);
      message<<retValue;
      socket->Send(message);

      return 1;
   }

   else if (strncmp(str, "GetServerName", 13) == 0) {
      //write name
      TMessage message(kMESS_STRING);
      message.WriteString(fServerName.Data());
      cout << fServerName.Data() << endl;
      socket->Send(message);

      return 1;
   }
   else if (strncmp(str, "GetPointer", 10) == 0) {
      //find object
      TMessage message(kMESS_OBJECT);
      TObject *obj = gROOT->FindObjectAny(str+11);

      //write pointer
      message.Reset(kMESS_ANY);
      size_t p = (size_t)obj;
      message<<p;
      socket->Send(message);

      return 1;
   }
   else if (strncmp(str, "Execute", 7) == 0) {
      char string[200];
      socket->Recv(string, sizeof(string));

      TString command = string;
      fApplication->ProcessLine(command.Data());
      return 1;
   }
#endif // ROOT_VERSION
   return 1;
}


THREADTYPE TNetFolderServer::Server(void *arg)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   TSocket *socket = (TSocket *) arg;

   while (TNetFolderServer::ResponseFunction(socket))
   {}
#endif // ROOT_VERSION
   return THREADRETURN;
}


THREADTYPE TNetFolderServer::ServerLoop(void *arg)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
// Server loop listening for incoming network connections on port
// specified by command line option -s. Starts a searver_thread for
// each connection.
   int port = *(int*)arg;
   TServerSocket *lsock = new TServerSocket(port, kTRUE);
   do {
      TSocket *sock = lsock->Accept();

      TThread *thread = new TThread("Server", TNetFolderServer::Server, sock);
      thread->Run();

   } while (1);
#endif // ROOT_VERSION
   return THREADRETURN;
}


void TNetFolderServer::StartServer(TApplication *app,Int_t port,const char* serverName)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
// start Socket server loop
   fApplication = app;
   fPort = port;
   fServerName = serverName;
   TThread *thread = new TThread("server_loop", TNetFolderServer::ServerLoop, &fPort);
   thread->Run();
#endif // ROOT_VERSION
}
