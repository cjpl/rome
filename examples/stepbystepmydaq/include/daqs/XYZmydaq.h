#ifndef XYZmydaq_H
#define XYZmydaq_H

#include "ROMEDAQSystem.h"
#include "Riostream.h"

class XYZmydaq : public ROMEDAQSystem
{
protected:
   fstream fFile;
   Long64_t fNumberOfEvent;

public:
   XYZmydaq();
   Int_t  GetTimeStamp() { return 0; };
   const char* GetName() { return "mydaq"; };

   Bool_t Init();
   Bool_t BeginOfRun();
   Bool_t Event(Long64_t event);
   Bool_t EndOfRun();
   Bool_t Terminate();
};

#endif   // XYZmydaq_H
