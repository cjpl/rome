// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  TNetFolder
//
//  
//
//  $Log$
//  Revision 1.5  2005/08/02 12:39:04  sawada
//  Removed netfolder title config.
//  Added netfolder reconnect config.
//  Check if the root folder is found when netfolder connects server.
//
//  Revision 1.4  2005/07/01 12:43:37  schneebeli_m
//  Update of TNetFolder : reconnect
//
//  Revision 1.3  2005/04/25 14:40:31  sawada
//  new TNerFolder
//
//  Revision 1.4  2005/04/11 07:05:12  schneebeli
//  inserted header
//
//
//
//////////////////////////////////////////////////////////////////////////

#include "TNetFolder.h"

ClassImp(TNetFolder)

TNetFolder::TNetFolder() : TNamed()
{
  fSocket = 0;
  fFolder = 0;
  fReconnect = true;
}

TNetFolder::TNetFolder(const char *name, const char *title,TSocket *socket,bool reconnect) : TNamed(name,title)
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
void TNetFolder::Reconnect() {
   Warning("Reconnect","can not make socket connection to %s on port %d.",fHost.Data(),fPort);
   Warning("Reconnect","program sleeps for 5s and tries again.");
   gSystem->Sleep(5000);
   delete fSocket;
   fSocket = new TSocket(fHost.Data(),fPort);
}

Bool_t TNetFolder::Send(const TMessage& mess) {
   if (fSocket->Send(mess)==-1) {
      while (!fSocket->IsValid()) {
         Reconnect();
      }
      fFolder = GetPointer();
      return false;
   }
   return true;
}
Bool_t TNetFolder::Send(const char* mess, Int_t kind) {
   if (fSocket->Send(mess,kind)==-1) {
      do {
         Reconnect();
      } while (!fSocket->IsValid());
      fFolder = GetPointer();
      return false;
   }
   return true;
}
Bool_t TNetFolder::Recv(TMessage*& mess) {
   if (fSocket->Recv(mess)==-1) {
      while (!fSocket->IsValid()) {
         Reconnect();
      }
      fFolder = GetPointer();
      return false;
   }
   return true;
}

Int_t TNetFolder::GetPointer()
{
  TMessage *m;
  TString str = "GetPointer ";
  str.Append(this->GetName());
  if (!Send(str.Data()))
     return GetPointer();
  if (!Recv(m))
     return GetPointer();

  if( m == NULL )
  {
    delete m;
    return 0;
  }
  int p;
  *m>>p;
  delete m;
  return p;
}

TObjArray* TNetFolder::GetListOfFolders()
{
  if (!Send("GetListOfFolders"))
     return GetListOfFolders();
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  if (!Send(*m))
     return GetListOfFolders();
  if (!Recv(m))
     return GetListOfFolders();
  if (m) {
    TObjArray *list = (TObjArray*) m->ReadObject(m->GetClass());
    delete m;
    return list;
  }
  return 0;
}

TObject* TNetFolder::FindObject(const char *name)
{
  TString str = "FindObject ";
  str.Append(name);
  if (!Send(str.Data()))
     return FindObject(name);
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  if (!Send(*m))
     return FindObject(name);
  
  if (!Recv(m))
     return FindObject(name);
  
  if( m == NULL )
  {
    delete m;
    return NULL;
  }
  TObject *obj = (TObject*) m->ReadObject(m->GetClass());
  
  delete m;
  return obj;
}

TObject* TNetFolder::FindObjectAny(const char *name)
{
  TString str = "FindObjectAny ";
  str.Append(name);
  if (!Send(str.Data()))
     return FindObjectAny(name);
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  if (!Send(*m))
     return FindObjectAny(name);

  if (!Recv(m))
     return FindObjectAny(name);

  if( m == NULL )
  {
    delete m;
    return NULL;
  }
  TObject *obj = (TObject*) m->ReadObject(m->GetClass());
  
  delete m;
  return obj;
}

const char *TNetFolder::FindFullPathName(const char *name)
{
  TString str = "FindFullPathName ";
  str.Append(name);
  if (!Send(str.Data()))
     return FindFullPathName(name);
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  if (!Send(*m))
     return FindFullPathName(name);
  if (!Recv(m))
     return FindFullPathName(name);
  
  if( m == NULL )
  {
    delete m;
    return NULL;
  }
  
  const char *path = ((TObjString*) m->ReadObject(m->GetClass()))->String().Data();
  
  delete m;
  return path;
}

Int_t TNetFolder::Occurence(const TObject *obj)
{
  if (!Send("Occurence"))
     return Occurence(obj);
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  if (!Send(*m))
     return Occurence(obj);
  delete m;
  
  m = new TMessage(kMESS_OBJECT);
  m->Reset();
  m->WriteObject(obj);
  if (!Send(*m))
     return Occurence(obj);
  if (!Recv(m))
     return Occurence(obj);
  
  if( m == NULL )
  {
    delete m;
    return -1;
  }
  
  Int_t retValue;
  *m>>retValue;
  
  delete m;
  return retValue;
}

void TNetFolder::Execute(const char *line)
{
  // The line is executed by the CINT of the server
  if (!Send("Execute"))
     return Execute(line);
  if (!Send(line))
     return Execute(line);
}

void TNetFolder::ExecuteMethod(const char *objectName,const char *objectType,const char *methodName,const char *methodArguments)
{
  // A method of an object is executed by the CINT of the server. 
  // The statment on the server side is the following :
  //   ((objectType)objectName)->methodName(methodArguments);
  if (!Send("ExecuteMethod"))
     return ExecuteMethod(objectName,objectType,methodName,methodArguments);
  if (!Send(objectName))
     return ExecuteMethod(objectName,objectType,methodName,methodArguments);
  if (!Send(objectType))
     return ExecuteMethod(objectName,objectType,methodName,methodArguments);
  if (!Send(methodName))
     return ExecuteMethod(objectName,objectType,methodName,methodArguments);
  if (!Send(methodArguments))
     return ExecuteMethod(objectName,objectType,methodName,methodArguments);
}

