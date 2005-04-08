/********************************************************************
  TNetFolderServer.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2005/04/08 14:55:46  schneebeli_m
  Added TNetFolderServer class


********************************************************************/
#ifndef TNetFolderServer_H
#define TNetFolderServer_H

#include <TNamed.h>

class TNetFolderServer : public TNamed
{
protected:

public:
   static void StartServer(int port);
};

#endif   // TNetFolderServer_H
