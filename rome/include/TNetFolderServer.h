/********************************************************************
  TNetFolderServer.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef TNetFolderServer_H
#define TNetFolderServer_H

#include <TNamed.h>

class TNetFolderServer : public TNamed
{
protected:
   Int_t fPort;
public:
   void StartServer(TApplication *app,Int_t port);
};

#endif   // TNetFolderServer_H
