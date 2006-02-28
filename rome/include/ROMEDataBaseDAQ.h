/********************************************************************
  ROMEDataBaseDAQ.h, M. Schneebeli PSI

  $Id: ROMEDataBaseDAQ.h 480 2005-10-17 12:12:54Z schneebeli_m $

********************************************************************/
#ifndef ROMEDataBaseDAQ_H
#define ROMEDataBaseDAQ_H

#include <ROMEDAQSystem.h>

class ROMEDataBaseDAQ : public ROMEDAQSystem {
protected:

   Int_t         fTimeStamp;                       //! Current time stamp

public:
   ROMEDataBaseDAQ();
   virtual ~ROMEDataBaseDAQ() {};

   Int_t       GetTimeStamp() { return fTimeStamp; };
   const char *GetName() { return "database"; };

   Bool_t      Init();
   Bool_t      BeginOfRun();
   Bool_t      Event(Long64_t event);
   Bool_t      EndOfRun();
   Bool_t      Terminate();
};

#endif   // ROMEDataBaseDAQ_H
