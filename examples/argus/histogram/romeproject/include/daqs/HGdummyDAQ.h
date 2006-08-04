// Author: Ryu Sawada

#ifndef HGdummyDAQ_H
#define HGdummyDAQ_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGdummyDAQ                                                                 //
//                                                                            //
// dummy DAQ class for HGframework.                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "ROMEDAQSystem.h"

class HGdummyDAQ : public ROMEDAQSystem
{
protected:
public:
   HGdummyDAQ();
   virtual ~HGdummyDAQ() {}
   Int_t    GetTimeStamp() { return 0; }
   const char* GetName() const { return "dummy"; }

   Bool_t   Init();
   Bool_t   BeginOfRun();
   Bool_t   Event(Long64_t event);
   Long64_t Seek(Long64_t event) { return event; }
   Bool_t   EndOfRun();
   Bool_t   Terminate();
   ClassDef(HGdummyDAQ, 0)
};

#endif   // HGdummyDAQ_H
