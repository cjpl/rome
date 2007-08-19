// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// TNetFolderServer
//
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
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
#include <TApplication.h>
#include <TThread.h>
#include <TList.h>
#include <TCollection.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "Riostream.h"
#include "TNetFolderServer.h"

TApplication* TNetFolderServer::fApplication      = 0;
Int_t         TNetFolderServer::fPort             = 9090;
TString       TNetFolderServer::fServerName       = "localhost";
TThread*      TNetFolderServer::fServerLoopThread = 0;
TList*        TNetFolderServer::fServerThreadList = 0;
Bool_t        gNetFolderServerRunning             = kFALSE;

ClassImp(TNetFolderServer)

//______________________________________________________________________________
TNetFolderServer::TNetFolderServer()
:TNamed()
{
   if (!fServerThreadList) {
      fServerThreadList = new TList();
   }
}

//______________________________________________________________________________
TNetFolderServer::~TNetFolderServer()
{
   StopServer();
   SafeDelete(fServerThreadList);
}

//______________________________________________________________________________
void TNetFolderServer::StopServer()
{
   gNetFolderServerRunning = kFALSE;
   if (fServerLoopThread) {
      fServerLoopThread->Join();
   }
   SafeDelete(fServerLoopThread);
}

//______________________________________________________________________________
TFolder *TNetFolderServer::ReadFolderPointer(TSocket *socket)
{
   if (!socket || !socket->IsValid()) {
      return NULL;
   }

   //read pointer to current folder
   TMessage *message = 0;
   if (socket->Recv(message) <= 0) {
      socket->Close();
      delete message;
      return NULL;
   }
   Long64_t p;
   *message>>p;
   delete message;
   return reinterpret_cast<TFolder*>(p);
}

//______________________________________________________________________________
int TNetFolderServer::ResponseFunction(TSocket *socket) {
   if (!socket->IsValid()) {
      return 0;
   }

   // Read Command
   char str[200];
   Int_t status = socket->Select(TSocket::kRead, kNetFolderServerTimeOut);
   if (status == 0) { // time out
      return 1;
   }
   if (status == -1 || socket->Recv(str, sizeof(str)) <= 0) { // error
      return 0;
   }
   return CheckCommand(socket,str);
}

//______________________________________________________________________________
int TNetFolderServer::CheckCommand(TSocket *socket,char *str) {
   if (!socket->IsValid()) {
      return 1;
   }

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

      for (int i = 0; i < names->GetLast() + 1; i++) {
         delete static_cast<TObjString*>(names->At(i));
      }

      delete names;
      return 1;
   } else if (strncmp(str, "FindObject", 10) == 0) {
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
      } else {
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
   } else if (strncmp(str, "FindFullPathName", 16) == 0) {
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
   } else if (strncmp(str, "Occurence", 9) == 0) {
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
      if (socket->Recv(answer) <= 0) {
         socket->Close();
         delete answer;
         return 1;
      }
      TObject *obj = static_cast<TObject*>(answer->ReadObject(answer->GetClass()));
      delete answer;

      //get occurence
      Int_t retValue = folder->Occurence(obj);

      //write occurence
      message.Reset(kMESS_ANY);
      message<<retValue;
      socket->Send(message);

      return 1;
   } else if (strncmp(str, "GetServerName", 13) == 0) {
      //write name
      TMessage message(kMESS_STRING);
      message.WriteString(fServerName.Data());
      socket->Send(message);

      return 1;
   } else if (strncmp(str, "GetPointer", 10) == 0) {
      //find object
      TMessage message(kMESS_OBJECT);
      TObject *obj = gROOT->FindObjectAny(str+11);

      //write pointer
      message.Reset(kMESS_ANY);
      Long64_t p = reinterpret_cast<Long64_t>(obj);
      message<<p;
      socket->Send(message);

      return 1;
   } else if (strncmp(str, "Execute", 7) == 0) {
      char tmpStr[200];
      socket->Recv(tmpStr, sizeof(tmpStr));

      TString command = tmpStr;
      fApplication->ProcessLine(command.Data());
      return 1;
   } else if (strncmp(str, "Macro", 5) == 0) {
      TString    aclicMode;
      TString    arguments;
      TString    io;
      char       tmpStr[256];
      TString    orgNameStr;
      TString    newNameStr;
      TString    orgFileStr;
      TString    newFileStr;

      // recieve file name
      socket->Recv(tmpStr, sizeof(tmpStr));
      orgNameStr = tmpStr;
      newFileStr = orgFileStr = gSystem->SplitAclicMode(orgNameStr.Data(), aclicMode, arguments, io);
#if defined( R__UNIX )
      fclose(gSystem->TempFileName(newFileStr));
      gSystem->Unlink(newFileStr.Data());
#else
      GetTempFileName("c:\\", newFileStr.Data(), 0, tmpStr);
      newFileStr = tmpStr;
#endif
      newNameStr = orgNameStr;
      newNameStr.ReplaceAll(orgFileStr.Data(), newFileStr.Data());

      // recieve number of charactors
      socket->Recv(tmpStr, sizeof(tmpStr));
      char *cstop = 0;
      Int_t length = strtol(tmpStr, &cstop, 10);

      // recieve macro content
      char *macro = new char[length];
      socket->Recv(macro, length);

      // create temporary macrofile and execute
      ofstream ofile(newFileStr.Data(), ios::out | ios::trunc);
      if (!ofile.good()) {
         cerr<<"Error in <TNetFolderServer::CheckCommand>: failed to open "<<newFileStr<<endl;
      }
      ofile<<macro<<endl;
      delete [] macro;
      Int_t err;
      fApplication->ProcessFile(newNameStr.Data(), &err);
      gSystem->Unlink(newFileStr.Data());
      return 1;
   }
   return 1;
}

//______________________________________________________________________________
THREADTYPE TNetFolderServer::Server(void *arg)
{
   TSocket *socket = static_cast<TSocket*>(arg);
   if (!socket->IsValid()) {
      delete socket;
      return THREADRETURN;
   }

   while (gNetFolderServerRunning && TNetFolderServer::ResponseFunction(socket))
   {}

   delete socket;
   return THREADRETURN;
}

//______________________________________________________________________________
THREADTYPE TNetFolderServer::ServerLoop(void *arg)
{
// Server loop listening for incoming network connections on port
// specified by command line option -s. Starts a searver_thread for
// each connection.
   Int_t port;
   memcpy(&port, arg, sizeof(Int_t));
   TServerSocket *lsock = new TServerSocket(port, kTRUE);
   if (!lsock->IsValid()) {
      switch(lsock->GetErrorCode()) {
         case -1:
            cerr<<"Error: Low level socket() call failed. Failed to connect port "<<port<<endl;
            break;
         case -2:
            cerr<<"Error: Low level bind() call failed. Failed to connect port "<<port<<endl;
            break;
         case -3:
            cerr<<"Error: Low level listen() call failed. Failed to connect port "<<port<<endl;
            break;
         default:
            cerr<<"Error: Failed to connect port "<<port<<endl;;
            break;
      };
      delete lsock;
      return THREADRETURN;
   }

   TSocket *sock;
   Int_t status;
   TThread *thread;
   while (gNetFolderServerRunning) {
      status = lsock->Select(TSocket::kRead, kNetFolderServerTimeOut);
      if (status == -1) { // error
         break;
      } else if (status == 0) { // time out
         continue;
      } else {
         if ((sock = lsock->Accept()) > 0) {
            thread = new TThread("Server", TNetFolderServer::Server, sock);
            fServerThreadList->Add(thread);
            thread->Run();
         }
      }
      TIter join(fServerThreadList);
      while ((thread = static_cast<TThread*>(join()))) {
         if (thread->GetState() == TThread::kTerminatedState) {
            thread->Join();
            fServerThreadList->Remove(thread);
         }
      }
   }
   TIter next(fServerThreadList);
   while ((thread = static_cast<TThread*>(next()))) {
      thread->Join();
   }
   delete lsock;
   return THREADRETURN;
}

//______________________________________________________________________________
void TNetFolderServer::StartServer(TApplication *app,Int_t port,const char* serverName)
{
// start Socket server loop
   if (gNetFolderServerRunning || fServerLoopThread) {
      Warning("StartServer", "server is already running.");
      return;
   }
   fApplication = app;
   fPort = port;
   fServerName = serverName;
   gNetFolderServerRunning = kTRUE;
   fServerLoopThread = new TThread("server_loop", TNetFolderServer::ServerLoop, &fPort);
   fServerLoopThread->Run();
}
