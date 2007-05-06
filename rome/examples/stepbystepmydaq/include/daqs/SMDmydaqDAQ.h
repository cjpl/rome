#ifndef SMDmydaqDAQ_H
#define SMDmydaqDAQ_H

#include "ROMEDAQSystem.h"
#include "Riostream.h"

class SMDmydaqDAQ : public ROMEDAQSystem
{
protected:
   fstream *fFile;
   Long64_t fNumberOfEvent;

private:
   SMDmydaqDAQ(const SMDmydaqDAQ &c); // not implemented
   SMDmydaqDAQ &operator=(const SMDmydaqDAQ &c); // not implemented

public:
   SMDmydaqDAQ();
   Int_t  GetTimeStamp() { return 0; };
   const char* GetName() const { return "mydaq"; };

   Bool_t Init();
   Bool_t BeginOfRun();
   Bool_t Event(Long64_t event);
   Bool_t EndOfRun();
   Bool_t Terminate();

   ClassDef(SMDmydaqDAQ, 0)
};

#endif   // SMDmydaqDAQ_H
