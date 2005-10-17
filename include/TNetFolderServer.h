/********************************************************************
  TNetFolderServer.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef TNetFolderServer_H
#define TNetFolderServer_H

#include <TNamed.h>

class TNetFolderServer : public TNamed
{
protected:
   int fPort;
public:
   void StartServer(TApplication *app,int port);
};

#endif   // TNetFolderServer_H
