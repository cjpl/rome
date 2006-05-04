#ifndef SMDmydaq_H
#define SMDmydaq_H

#include "ROMEDAQSystem.h"
#include "Riostream.h"

class SMDmydaq : public ROMEDAQSystem
{
protected:
   fstream fFile;
   Long64_t fNumberOfEvent;

public:
   SMDmydaq();
   Int_t  GetTimeStamp() { return 0; };
   const char* GetName() { return "mydaq"; };

   Bool_t Init();
   Bool_t BeginOfRun();
   Bool_t Event(Long64_t event);
   Bool_t EndOfRun();
   Bool_t Terminate();
};

#endif   // SMDmydaq_H
