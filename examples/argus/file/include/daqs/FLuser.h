// Author: Ryu Sawada

#ifndef FLuser_H
#define FLuser_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FLuser                                                                     //
//                                                                            //
// user DAQ class for FLmonitor.                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "generated/FLRomeDAQ.h"

class FLuser : public FLRomeDAQ
{
protected:
public:
   FLuser();
   Int_t  GetTimeStamp() { return 0; };
   const char* GetName() { return "user"; };

   Bool_t Init();
   Bool_t BeginOfRun();
   Bool_t Event(Long64_t event);
   Bool_t EndOfRun();
   Bool_t Terminate();
   ClassDef(FLuser, 0)
};

#endif   // FLuser_H
