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
#include "Riostream.h"

class TNetFolder : public TNamed
{
public:
  TSocket  *fSocket;     //connection to TObjServer server
  Int_t     fFolder;     //pointer to TFolder object
  
  TNetFolder();
  
  TNetFolder( char const *, char const *, TSocket * );
  
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
  
  ClassDef( TNetFolder, 0 )
};

#endif   // TNetFolder_H
