// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// TNetFolderServer
//
//
//  $Log$
//  Revision 1.1  2005/04/08 14:54:55  schneebeli_m
//  new
//
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#include <windows.h>
#endif

#if defined( R__UNIX )
#include <sys/socket.h>
#include <TThread.h>
#endif
#include <TMessage.h>
#include <TSocket.h>
#include <TServerSocket.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TFolder.h>
#include <TObjArray.h>
#include <TNetFolderServer.h>

#define PTYPE int

#if defined( R__UNIX )
#define THREADRETURN NULL
#define THREADTYPE void*
#endif
#if defined( R__VISUAL_CPLUSPLUS )
#define THREADRETURN 0
#define THREADTYPE DWORD WINAPI
#endif

TFolder *ReadFolderPointer(TSocket *fSocket);
int ResponseFunction(TSocket *fSocket);
THREADTYPE Server(void *arg);
THREADTYPE ServerLoop(void *arg);
extern void writeLineToProcess(const char* str);

TFolder *ReadFolderPointer(TSocket *fSocket)
{
   //read pointer to current folder
   TMessage *message = new TMessage(kMESS_OBJECT);
   fSocket->Recv(message);
   Int_t p;
   *message>>p;
   delete message;
   return (TFolder*)p;
}

int ResponseFunction(TSocket *fSocket) {
   char str[80];
   if (fSocket->Recv(str, sizeof(str)) <= 0) {
      fSocket->Close();
      delete fSocket;
      return 0;
   }
   TMessage *message = new TMessage(kMESS_OBJECT);
   if (strcmp(str, "GetListOfFolders") == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);
      if (folder==NULL) {
         message->Reset(kMESS_OBJECT);
         message->WriteObject(NULL);
         fSocket->Send(*message);
         delete message;
         return 1;
      }

      //get folder names
      TObject *obj;
      TObjArray *names = new TObjArray(100);

      TCollection *folders = folder->GetListOfFolders();
      TIterator *iterFolders = folders->MakeIterator();
      TString str;
      while (obj = iterFolders->Next()) {
         str = obj->GetName();
         names->Add(new TObjString(str.Data()));
      }

      //write folder names
      message->Reset(kMESS_OBJECT);
      message->WriteObject(names);
      fSocket->Send(*message);

      for (int i = 0; i < names->GetLast() + 1; i++)
         delete(TObjString *) names->At(i);

      delete names;

      delete message;
      return 1;
   }
   else if (strncmp(str, "FindObject", 10) == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);

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
         fSocket->Send("Error");
      } else {
         message->Reset(kMESS_OBJECT);
         message->WriteObject(obj);
         fSocket->Send(*message);
      }
      delete message;
      return 1;
   }

   else if (strncmp(str, "FindFullPathName", 16) == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);

      //find path
      const char* path = folder->FindFullPathName(str+17);

      //write path
      if (!path) {
         fSocket->Send("Error");
      } else {
         TObjString *obj = new TObjString(path);
         message->Reset(kMESS_OBJECT);
         message->WriteObject(obj);
         fSocket->Send(*message);
         delete obj;
      }
      delete message;
      return 1;
   }

   else if (strncmp(str, "Occurence", 9) == 0) {
      TFolder *folder = ReadFolderPointer(fSocket);

      //read object
      message->Reset(kMESS_OBJECT);
      fSocket->Recv(message);
      TObject *obj = ((TObject*) message->ReadObject(message->GetClass()));

      //get occurence
      Int_t retValue = folder->Occurence(obj);

      //write occurence
      message->Reset(kMESS_OBJECT);
      *message<<retValue;
      fSocket->Send(*message);

      delete message;
      return 1;
   }

   else if (strncmp(str, "GetPointer", 10) == 0) {
      //find object
      TObject *obj = gROOT->FindObjectAny(str+11);

      //write pointer
      message->Reset(kMESS_ANY);
      int p = (PTYPE)obj;
      *message<<p;
      fSocket->Send(*message);

      delete message;
      return 1;
   }
   else if (strncmp(str, "Command", 7) == 0) {
      char objName[100];
      char method[100];
      char type[100];
      char arg[100];
      fSocket->Recv(objName, sizeof(objName));
      fSocket->Recv(type, sizeof(type));
      fSocket->Recv(method, sizeof(method));
      fSocket->Recv(arg, sizeof(arg));

      TString str = "temporarySocketObject = gROOT->FindObjectAny(\"";
      str += objName;
      str += "\");";
      writeLineToProcess(str.Data());
      str = "((";
      str += type;
      str += ")temporarySocketObject)->";
      str += method;
      str += "(";
      str += arg;
      str += ");";
      writeLineToProcess(str.Data());

      delete message;
      return 1;
   }
   return 1;
}




THREADTYPE Server(void *arg)
{
   TSocket *fSocket = (TSocket *) arg;

   while (ResponseFunction(fSocket))
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
   writeLineToProcess("TObject* temporarySocketObject;");

   do {
      TSocket *sock = lsock->Accept();

#if defined( R__UNIX )
      TThread *thread = new TThread("Server", Server, sock);
      thread->Run();
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      LPDWORD lpThreadId=0;
      CloseHandle(CreateThread(NULL,1024,&Server,sock,0,lpThreadId));
#endif
   } while (1);
   return THREADRETURN;
}


void TNetFolderServer::StartServer(int port) 
{
   int pport;
   pport = port;
// start fSocket server loop
#if defined( R__UNIX )
   TThread *thread = new TThread("server_loop", ServerLoop, &pport);
   thread->Run();
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   LPDWORD lpThreadId=0;
   CloseHandle(CreateThread(NULL,1024,&ServerLoop,&pport,0,lpThreadId));
#endif
}
