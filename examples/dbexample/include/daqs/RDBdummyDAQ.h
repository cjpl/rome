// Author: Ryu Sawada

#ifndef RDBdummyDAQ_H
#define RDBdummyDAQ_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// RDBdummyDAQ                                                                //
//                                                                            //
// dummy DAQ class for RDBframework.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "ROMEDAQSystem.h"

class RDBdummyDAQ : public ROMEDAQSystem
{
protected:
public:
   RDBdummyDAQ();
   virtual ~RDBdummyDAQ() {}
   Int_t    GetTimeStamp() { return 0; }
   const char* GetName() const { return "dummy"; }

   Bool_t   Init();
   Bool_t   BeginOfRun();
   Bool_t   Event(Long64_t event);
   Long64_t Seek(Long64_t event) { return event; }
   Bool_t   EndOfRun();
   Bool_t   Terminate();
   ClassDef(RDBdummyDAQ, 0)
};

#endif   // RDBdummyDAQ_H
