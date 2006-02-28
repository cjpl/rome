// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  TNetFolder
//
//  $Id: TNetFolder.cpp 770 2005-12-13 11:53:31Z sawada $
//
//////////////////////////////////////////////////////////////////////////

#include "TObjString.h"
#include "TSystem.h"
#include "TNetFolder.h"
#include "Riostream.h"

ClassImp(TNetFolder)

TNetFolder::TNetFolder():TNamed()
{
   fSocket = 0;
   fFolder = 0;
   fReconnect = kTRUE;
}

TNetFolder::TNetFolder(const char *name, const char *title, TSocket *socket, Bool_t reconnect):TNamed(name, title)
{
   fSocket = socket;
   fFolder = GetPointer();
   fReconnect = reconnect;
   fHost = fSocket->GetUrl();
   fPort = fSocket->GetPort();
}

TNetFolder::~TNetFolder()
{
}

void TNetFolder::Reconnect()
{
   Warning("Reconnect", "can not make socket connection to %s on port %d.", fHost.Data(), fPort);
   Warning("Reconnect", "program sleeps for 5s and tries again.");
   gSystem->Sleep(5000);
   delete fSocket;
   fSocket = new TSocket (fHost.Data(), fPort);
}

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

size_t TNetFolder::GetPointer()
{
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

TObjArray *TNetFolder::GetListOfFolders()
{
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

TObject *TNetFolder::FindObject(const char *name) const
{
   TNetFolder* const localThis = const_cast<TNetFolder* const>(this);
   TString str = "FindObject ";
   str.Append(name);
   if (!localThis->Send(str.Data()))
      return localThis->FindObject(name);

   TMessage m(kMESS_ANY);
   m.Reset();
   m << fFolder;
   if (!localThis->Send(m))
      return localThis-> FindObject(name);

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

TObject *TNetFolder::FindObjectAny(const char *name) const
{
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

const char *TNetFolder::FindFullPathName(const char *name)
{
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

Int_t TNetFolder::Occurence(const TObject *obj)
{
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
