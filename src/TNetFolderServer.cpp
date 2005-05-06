// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// TNetFolderServer
//
//
//  $Log$
//  Revision 1.4  2005/05/06 08:39:16  schneebeli_m
//  ported windows threads to TThread
//
//  Revision 1.3  2005/04/23 15:32:18  sawada
//  small modification.
//
//  Revision 1.2  2005/04/08 17:08:09  schneebeli_m
//  TNetFolderServer changes
//
//  Revision 1.1  2005/04/08 14:54:55  schneebeli_m
//  new
//
//
//////////////////////////////////////////////////////////////////////////

#include <TThread.h>
#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TFolder.h>
#include <TObjArray.h>
#include <TApplication.h>
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
   //read pointer to current folder
   TMessage *message = new TMessage(kMESS_OBJECT);
   socket->Recv(message);
   Int_t p;
   *message>>p;
   delete message;
   return (TFolder*)p;
}

int ResponseFunction(TSocket *socket) {
   // Command handling
   char str[80];
   if (socket->Recv(str, sizeof(str)) <= 0) {
      socket->Close();
      delete socket;
      return 0;
   }
   if (strcmp(str, "GetListOfFolders") == 0) {
      TMessage *message = new TMessage(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);
      if (folder==NULL) {
         message->Reset(kMESS_OBJECT);
         message->WriteObject(NULL);
         socket->Send(*message);
         delete message;
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
      message->Reset(kMESS_OBJECT);
      message->WriteObject(names);
      socket->Send(*message);

      for (int i = 0; i < names->GetLast() + 1; i++)
         delete(TObjString *) names->At(i);

      delete names;

      delete message;
      return 1;
   }
   else if (strncmp(str, "FindObject", 10) == 0) {
      TMessage *message = new TMessage(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);

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
         message->Reset(kMESS_OBJECT);
         message->WriteObject(obj);
         socket->Send(*message);
      }
      delete message;
      return 1;
   }

   else if (strncmp(str, "FindFullPathName", 16) == 0) {
      TMessage *message = new TMessage(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);

      //find path
      const char* path = folder->FindFullPathName(str+17);

      //write path
      if (!path) {
         socket->Send("Error");
      } else {
         TObjString *obj = new TObjString(path);
         message->Reset(kMESS_OBJECT);
         message->WriteObject(obj);
         socket->Send(*message);
         delete obj;
      }
      delete message;
      return 1;
   }

   else if (strncmp(str, "Occurence", 9) == 0) {
      TMessage *message = new TMessage(kMESS_OBJECT);
      TFolder *folder = ReadFolderPointer(socket);

      //read object
      message->Reset(kMESS_OBJECT);
      socket->Recv(message);
      TObject *obj = ((TObject*) message->ReadObject(message->GetClass()));

      //get occurence
      Int_t retValue = folder->Occurence(obj);

      //write occurence
      message->Reset(kMESS_OBJECT);
      *message<<retValue;
      socket->Send(*message);

      delete message;
      return 1;
   }

   else if (strncmp(str, "GetPointer", 10) == 0) {
      //find object
      TMessage *message = new TMessage(kMESS_OBJECT);
      TObject *obj = gROOT->FindObjectAny(str+11);

      //write pointer
      message->Reset(kMESS_ANY);
      int p = (PTYPE)obj;
      *message<<p;
      socket->Send(*message);

      delete message;
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
   return 1;
}




THREADTYPE Server(void *arg)
{
   TSocket *socket = (TSocket *) arg;

   while (ResponseFunction(socket))
   {}
   return THREADRETURN;
}


THREADTYPE ServerLoop(void *arg)
{
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
   return THREADRETURN;
}


void TNetFolderServer::StartServer(TApplication *app,int port) 
{
// start Socket server loop
   fApplication = app;
   fPort = port;
   TThread *thread = new TThread("server_loop", ServerLoop, &fPort);
   thread->Run();
}
