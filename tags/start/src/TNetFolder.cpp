#include "TNetFolder.h"

ClassImp(TNetFolder)

TNetFolder::TNetFolder() : TNamed()
{
  fSocket = 0;
  fFolder = 0;
}

TNetFolder::TNetFolder(const char *name, const char *title,TSocket *socket) : TNamed(name,title) 
{
  fSocket = socket;  
  fFolder = GetPointer();
}

TNetFolder::~TNetFolder()
{
}

Int_t TNetFolder::GetPointer()
{
  TMessage *m;
  TString str = "GetPointer ";
  str.Append(this->GetName());
  if( fSocket->Send(str.Data()) == -1 )return 0;
  if( fSocket->Recv(m) == -1 )return 0;
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
  fSocket->Send("GetListOfFolders");
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  fSocket->Send(*m);
  fSocket->Recv(m);
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
  fSocket->Send(str.Data());
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  fSocket->Send(*m);
  
  fSocket->Recv(m);
  
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
  fSocket->Send(str.Data());
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  fSocket->Send(*m);
  
  fSocket->Recv(m);
  
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
  fSocket->Send(str.Data());
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  fSocket->Send(*m);
  fSocket->Recv(m);
  
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
  fSocket->Send("Occurence");
  
  TMessage *m = new TMessage(kMESS_ANY);
  m->Reset();
  *m<<fFolder;
  fSocket->Send(*m);
  delete m;
  
  m = new TMessage(kMESS_OBJECT);
  m->Reset();
  m->WriteObject(obj);
  fSocket->Send(*m);
  fSocket->Recv(m);
  
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

// end of file
