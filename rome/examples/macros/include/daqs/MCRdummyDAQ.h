#ifndef MCRdummyDAQ_H
#define MCRdummyDAQ_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MCRdummyDAQ                                                                //
//                                                                            //
// dummy DAQ class for MCRAnalyzer.                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <TRandom.h>
#include "ROMEDAQSystem.h"

class MCRdummyDAQ : public ROMEDAQSystem
{
protected:
   TRandom *fRandom;

private:
   MCRdummyDAQ(const MCRdummyDAQ &c); // not implemented
   MCRdummyDAQ &operator=(const MCRdummyDAQ &c); // not implemented

public:
   MCRdummyDAQ();
   virtual ~MCRdummyDAQ() { delete fRandom; }
   Int_t    GetTimeStamp() { return 0; }
   const char* GetName() const { return "dummy"; }

   Bool_t   Init();
   Bool_t   BeginOfRun();
   Bool_t   Event(Long64_t event);
   Long64_t Seek(Long64_t event);
   Bool_t   EndOfRun();
   Bool_t   Terminate();
   ClassDef(MCRdummyDAQ, 0)
};

#endif   // MCRdummyDAQ_H
