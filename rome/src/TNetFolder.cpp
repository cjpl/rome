// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  TNetFolder
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TObjString.h>
#include <TSystem.h>
#include <TSocket.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "Riostream.h"
#include "TNetFolder.h"

ClassImp(TNetFolder)

//______________________________________________________________________________
TNetFolder::TNetFolder()
:TNamed()
,fSocket(0)
,fFolder(0)
,fReconnect(kTRUE)
,fHost("localhost")
,fPort(9090)
{
}

//______________________________________________________________________________
TNetFolder::TNetFolder(const char *name, const char *title, TSocket *socket, Bool_t reconnect)
:TNamed(name, title)
,fSocket(socket)
,fFolder(GetPointer())
,fReconnect(reconnect)
,fHost(socket->GetUrl())
,fPort(socket->GetPort())
{
}

//______________________________________________________________________________
TNetFolder::~TNetFolder()
{
}

//______________________________________________________________________________
void TNetFolder::Reconnect()
{
   Warning("Reconnect", "can not make socket connection to %s on port %d.\nprogram sleeps for 5s and tries again.", fHost.Data(), fPort);
   gSystem->Sleep(5000);
   delete fSocket;
   fSocket = new TSocket (fHost.Data(), fPort);
}

//______________________________________________________________________________
Bool_t TNetFolder::Send(const TMessage &mess)
{
   if (fSocket->Send(mess) == -1) {
      while (!fSocket->IsValid()) {
         Reconnect();
      }
      fFolder = GetPointer();
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TNetFolder::Send(const char *mess, Int_t kind)
{
   if (fSocket->Send(mess, kind) == -1) {
      do {
         Reconnect();
      } while (!fSocket->IsValid());
      fFolder = GetPointer();
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TNetFolder::Recv(TMessage *&mess)
{
   if (fSocket->Recv(mess) == -1) {
      while (!fSocket->IsValid()) {
         Reconnect();
      }
      fFolder = GetPointer();
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
size_t TNetFolder::GetPointer()
{
// Get Pointer to the Servers Folder
   TMessage *m;
   TString str = "GetPointer ";
   str.Append(this->GetName());
   if (!Send(str.Data()))
      return GetPointer();
   if (!Recv(m))
      return GetPointer();

   if (m == NULL) {
      delete m;
      return 0;
   }
   size_t p;
   *m >> p;
   delete m;
   return p;
}

//______________________________________________________________________________
void TNetFolder::GetServerName(char* serverName,int serverNameSize)
{
// Get the Name of the Server
   TMessage *m;
   TString str = "GetServerName";
   if (!Send(str.Data())) {
      GetServerName(serverName,serverNameSize);
      return;
   }
   if (!Recv(m)) {
      GetServerName(serverName,serverNameSize);
      return;
   }

   if (m == NULL) {
      delete m;
      return;
   }
   m->ReadString(serverName,serverNameSize);
   delete m;
}

//______________________________________________________________________________
TObjArray *TNetFolder::GetListOfFolders()
{
// Get List of Folders
   if (!Send("GetListOfFolders"))
      return GetListOfFolders();

   TMessage m(kMESS_ANY);
   m.Reset();
   m << fFolder;
   if (!Send(m))
      return GetListOfFolders();
   TMessage *mr = 0;
   if (!Recv(mr))
      return GetListOfFolders();
   if (mr) {
      TObjArray *list = (TObjArray *) mr->ReadObject(mr->GetClass());
      delete mr;
      return list;
   }
   return 0;
}

//______________________________________________________________________________
TObject *TNetFolder::FindObject(const char *name) const
{
// search object identified by name in the tree of folders inside
// this folder.
// name may be of the forms:
//   A, specify a full pathname starting at the top ROOT folder
//     //root/xxx/yyy/name
//
//   B, specify a pathname starting with a single slash. //root is assumed
//     /xxx/yyy/name
//
//   C, Specify a pathname relative to this folder
//     xxx/yyy/name
//     name

   TNetFolder* const localThis = const_cast<TNetFolder* const>(this);
   TString str = "FindObject ";
   str.Append(name);
   if (!localThis->Send(str.Data()))
      return localThis->FindObject(name);

   TMessage m(kMESS_ANY);
   m.Reset();
   m << fFolder;
   if (!localThis->Send(m))
      return localThis->FindObject(name);

   TMessage *mr = 0;
   if (!localThis->Recv(mr))
      return localThis->FindObject(name);

   if (mr == NULL) {
      delete mr;
      return NULL;
   }
   TObject *obj = (TObject *) mr->ReadObject(mr->GetClass());

   delete mr;
   return obj;
}

//______________________________________________________________________________
TObject *TNetFolder::FindObjectAny(const char *name) const
{
// return a pointer to the first object with name starting at this folder

   TNetFolder* const localThis = const_cast<TNetFolder* const>(this);
   TString str = "FindObjectAny ";
   str.Append(name);
   if (!localThis->Send(str.Data()))
      return localThis->FindObjectAny(name);

   TMessage m(kMESS_ANY);
   m.Reset();
   m << fFolder;
   if (!localThis->Send(m))
      return localThis->FindObjectAny(name);

   TMessage *mr = 0;
   if (!localThis->Recv(mr))
      return localThis->FindObjectAny(name);

   if (mr == NULL) {
      delete mr;
      return NULL;
   }
   TObject *obj = (TObject *) mr->ReadObject(mr->GetClass());

   delete mr;
   return obj;
}

//______________________________________________________________________________
const char *TNetFolder::FindFullPathName(const char *name)
{
// return the full pathname corresponding to subpath name
// The returned path will be re-used by the next call to GetPath().

   TString str = "FindFullPathName ";
   str.Append(name);
   if (!Send(str.Data()))
      return FindFullPathName(name);

   TMessage m(kMESS_ANY);
   m.Reset();
   m << fFolder;
   if (!Send(m))
      return FindFullPathName(name);
   TMessage *mr = 0;
   if (!Recv(mr))
      return FindFullPathName(name);

   if (mr == NULL) {
      delete mr;
      return NULL;
   }

   const char *path = ((TObjString *) mr->ReadObject(mr->GetClass()))->String().Data();

   delete mr;
   return path;
}

//______________________________________________________________________________
Int_t TNetFolder::Occurence(const TObject *obj)
{
// Return occurence number of object in the list of objects of this folder.
// The function returns the number of objects with the same name as object
// found in the list of objects in this folder before object itself.
// If only one object is found, return 0;
   if (!Send("Occurence"))
      return Occurence(obj);

   TMessage m(kMESS_ANY);
   m.Reset();
   m << fFolder;
   if (!Send(m))
      return Occurence(obj);

   m.Reset();
   m.WriteObject(obj);
   if (!Send(m))
      return Occurence(obj);
   TMessage *mr = 0;
   if (!Recv(mr))
      return Occurence(obj);

   if (mr == NULL) {
      delete mr;
      return -1;
   }

   Int_t retValue;
   *mr >> retValue;

   delete mr;
   return retValue;
}

//______________________________________________________________________________
void TNetFolder::ExecuteCommand(const char *line)
{
   // The line is executed by the CINT of the server
   if (!Send("Execute")) {
      ExecuteCommand(line);
      return;
   }
   if (!Send(line)) {
      ExecuteCommand(line);
      return;
   }
}
