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
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TFolder.h>
#include <TObjArray.h>
#include <TApplication.h>
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
#include <TThread.h>
#endif // ROOT_VERSION
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include <TNetFolderServer.h>
#include <Riostream.h>

#define PTYPE int

#define THREADRETURN NULL
#define THREADTYPE void*

TApplication *fApplication;

TFolder *ReadFolderPointer(TSocket *socket);
int ResponseFunction(TSocket *socket);
THREADTYPE Server(void *arg);
THREADTYPE ServerLoop(void *arg);

TFolder *ReadFolderPointer(TSocket *socket)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   //read pointer to current folder
   TMessage *message = 0;
   socket->Recv(message);
   Int_t p;
   *message>>p;
   delete message;
   return (TFolder*)p;
#else
   return NULL;
#endif // ROOT_VERSION
}

int ResponseFunction(TSocket *socket) {
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   // Command handling
   char str[80];
   if (socket->Recv(str, sizeof(str)) <= 0) {
      socket->Close();
      delete socket;
      return 0;
   }
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
      TString str;
      while ((obj = iterFolders->Next())) {
         str = obj->GetName();
         names->Add(new TObjString(str.Data()));
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

   else if (strncmp(str, "GetPointer", 10) == 0) {
      //find object
      TMessage message(kMESS_OBJECT);
      TObject *obj = gROOT->FindObjectAny(str+11);

      //write pointer
      message.Reset(kMESS_ANY);
      int p = (PTYPE)obj;
      message<<p;
      socket->Send(message);

      return 1;
   }
   else if (strncmp(str, "ExecuteMethod", 13) == 0) {
      char objName[100];
      char method[100];
      char type[100];
      char arg[100];
      socket->Recv(objName, sizeof(objName));
      socket->Recv(type, sizeof(type));
      socket->Recv(method, sizeof(method));
      socket->Recv(arg, sizeof(arg));

      TString str = "temporarySocketObject = gROOT->FindObjectAny(\"";
      str += objName;
      str += "\");";
      fApplication->ProcessLine(str.Data());
      str = "((";
      str += type;
      str += ")temporarySocketObject)->";
      str += method;
      str += "(";
      str += arg;
      str += ");";
      fApplication->ProcessLine(str.Data());
      return 1;
   }
   else if (strncmp(str, "Execute", 7) == 0) {
      char string[200];
      socket->Recv(string, sizeof(string));

      TString str = string;
      fApplication->ProcessLine(str.Data());
      return 1;
   }
#endif // ROOT_VERSION
   return 1;
}




THREADTYPE Server(void *arg)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   TSocket *socket = (TSocket *) arg;

   while (ResponseFunction(socket))
   {}
#endif // ROOT_VERSION
   return THREADRETURN;
}


THREADTYPE ServerLoop(void *arg)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
// Server loop listening for incoming network connections on port
// specified by command line option -s. Starts a searver_thread for
// each connection.
   int port = *(int*)arg;
   TServerSocket *lsock = new TServerSocket(port, kTRUE);
   fApplication->ProcessLine("TObject* temporarySocketObject;");

   do {
      TSocket *sock = lsock->Accept();

      TThread *thread = new TThread("Server", Server, sock);
      thread->Run();

   } while (1);
#endif // ROOT_VERSION
   return THREADRETURN;
}


void TNetFolderServer::StartServer(TApplication *app,int port)
{
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
// start Socket server loop
   fApplication = app;
   fPort = port;
   TThread *thread = new TThread("server_loop", ServerLoop, &fPort);
   thread->Run();
#endif // ROOT_VERSION
}
