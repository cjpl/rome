/********************************************************************
  TNetFolderServer.h, M. Schneebeli PSI

  $Log$
  Revision 1.2  2005/04/08 17:08:07  schneebeli_m
  TNetFolderServer changes

  Revision 1.1  2005/04/08 14:55:46  schneebeli_m
  Added TNetFolderServer class


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
