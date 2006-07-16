// Author: Ryu Sawada

#ifndef FLuserDAQ_H
#define FLuserDAQ_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FLuserDAQ                                                                     //
//                                                                            //
// user DAQ class for FLmonitor.                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "generated/FLRomeDAQ.h"

class FLuserDAQ : public FLRomeDAQ
{
protected:
public:
   FLuserDAQ();
   Int_t  GetTimeStamp() { return 0; };
   const char* GetName() const { return "user"; };

   Bool_t Init();
   Bool_t BeginOfRun();
   Bool_t Event(Long64_t event);
   Bool_t EndOfRun();
   Bool_t Terminate();
   ClassDef(FLuserDAQ, 0)
};

#endif   // FLuserDAQ_H
