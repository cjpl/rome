/********************************************************************
  TNetFolder.h, M. Schneebeli PSI

  $Log$
  Revision 1.4  2005/08/02 12:39:04  sawada
  Removed netfolder title config.
  Added netfolder reconnect config.
  Check if the root folder is found when netfolder connects server.

  Revision 1.3  2005/07/01 12:43:37  schneebeli_m
  Update of TNetFolder : reconnect

  Revision 1.2  2005/04/25 14:40:31  sawada
  new TNerFolder

  Revision 1.3  2005/04/11 07:05:12  schneebeli
  inserted header



********************************************************************/

#ifndef TNetFolder_H
#define TNetFolder_H

#include "TNamed.h"
#include "TObjArray.h"
#include "TUrl.h"
#include "TSocket.h"
#include "TCollection.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TFolder.h"
#include "TMessage.h"
#include "TSystem.h"
#include "Riostream.h"

class TNetFolder : public TNamed
{
protected:
  TSocket  *fSocket;     // connection to TObjServer server
  Int_t     fFolder;     // pointer to TFolder object
  Bool_t    fReconnect;  // try to reconnect when connection is broken
  TString   fHost;       // host of socket connection
  Int_t     fPort;       // port of socket connection
  
public:
  TNetFolder();
  
  TNetFolder( char const *, char const *, TSocket * ,bool reconnect=true);
  
  virtual ~TNetFolder();
  
  virtual void Add( TObject * )
    { Error("Add","Not available for TNetFolders"); }
  
  TFolder *AddFolder( char const *, char const *, TCollection * =0 )
    { Error("AddFolder","Not available for TNetFolders"); return NULL; }
  
  virtual void Browse( TBrowser * )
    { Error("Browse","Not available for TNetFolders"); }
  
  virtual void Clear( Option_t * ="" )
    { Error("Clear","Not available for TNetFolders"); }
  
  virtual void Copy( TObject & ) const
    { MayNotUse("Copy(TObject &)"); }

  virtual char const *FindFullPathName( char const * );
  
  virtual TObject *FindObject( char const * );
  
  virtual TObject *FindObjectAny( char const * );
  
  TObjArray *GetListOfFolders();
  
  Bool_t IsFolder() const
    { return kTRUE; }
  
  virtual Int_t Occurence( TObject const * );
  
  virtual void RecursiveRemove( TObject * )
    { Error("RecursiveRemove","Not available for TNetFolders"); }
  
  virtual void Remove( TObject * )
    { Error("Remove","Not available for TNetFolders"); }
    
  Int_t GetPointer();
  void Execute(const char *line);
  void ExecuteMethod(const char *objectName,const char *objectType,const char *methodName,const char *methodArguments);

protected:

  void Reconnect();
  Bool_t Send(const TMessage& mess);
  Bool_t Send(const char* mess, Int_t kind = kMESS_STRING);
  Bool_t Recv(TMessage*& mess);


  ClassDef( TNetFolder, 0 )
};

#endif   // TNetFolder_H
