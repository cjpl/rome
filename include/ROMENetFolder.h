/********************************************************************
  ROMENetFolder.h, M. Schneebeli PSI

  $Id$

********************************************************************/

#ifndef ROMENetFolder_H
#define ROMENetFolder_H

#include "TNetFolder.h"

class ROMENetFolder : public TNetFolder
{  
public:
   ROMENetFolder():TNetFolder() {};
   ROMENetFolder( const char *name, const char *title, TSocket *socket,Bool_t reconnect=true):TNetFolder(name,title,socket,reconnect) {};
   virtual ~ROMENetFolder(){}
  
   Long64_t GetCurrentRunNumber();
   Long64_t GetCurrentEventNumber();

   ClassDef(ROMENetFolder, 0) // Customized TNetFolder for ROME
};

#endif   // ROMENetFolder_H
