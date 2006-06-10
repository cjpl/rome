/********************************************************************
  TNetFolder.h, M. Schneebeli PSI

  $Id$

********************************************************************/

#ifndef TNetFolder_H
#define TNetFolder_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TNamed.h>
#include <TMessage.h>
#include <TFolder.h>
#include <TObjArray.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

class TSocket;

class TNetFolder : public TNamed
{
protected:
  TSocket  *fSocket;     // connection to TObjServer server
  size_t    fFolder;     // pointer to TFolder object
  Bool_t    fReconnect;  // try to reconnect when connection is broken
  TString   fHost;       // host of socket connection
  Int_t     fPort;       // port of socket connection
  
public:
  TNetFolder();
  TNetFolder( char const *, char const *, TSocket * ,Bool_t reconnect=true);  
  virtual ~TNetFolder();
  
  virtual void        Add( TObject * ) { Error("Add","Not available for TNetFolders"); }
  TFolder            *AddFolder( char const *, char const *, TCollection * =0 ) { Error("AddFolder","Not available for TNetFolders"); return NULL; }
  virtual void        Browse( TBrowser * ) { Error("Browse","Not available for TNetFolders"); }
  virtual void        Clear( Option_t * ="" ) { Error("Clear","Not available for TNetFolders"); }
  virtual void        Copy( TObject & ) const { MayNotUse("Copy(TObject &)"); }
  virtual char const *FindFullPathName( char const * );
  virtual TObject    *FindObject( char const * ) const;
  virtual TObject    *FindObject( const TObject* obj ) const { MayNotUse("FindObject"); return NULL; };  
  virtual TObject    *FindObjectAny( char const * ) const;  
  TObjArray          *GetListOfFolders();
  Bool_t              IsFolder() const { return kTRUE; }
  virtual Int_t       Occurence( TObject const * );
  virtual void        RecursiveRemove( TObject * ) { Error("RecursiveRemove","Not available for TNetFolders"); }
  virtual void        Remove( TObject * ) { Error("Remove","Not available for TNetFolders"); }    
  void                ExecuteCommand(const char *line);
  size_t              GetPointer();
  void                GetServerName(char* serverName,Int_t serverNameSize);

protected:

  void                Reconnect();
  Bool_t              Send(const TMessage& mess);
  Bool_t              Send(const Char_t* mess, Int_t kind = kMESS_STRING);
  Bool_t              Recv(TMessage*& mess);

  ClassDef( TNetFolder, 0 ) // Client for socket connection
};

#endif   // TNetFolder_H
