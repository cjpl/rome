////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// RUNHTTask1                                                                 //
//                                                                            //
// Begin_Html                                                                 //
//                                                                            //
// Description:                                                               //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Usage:                                                                     //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Status:                                                                    //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// To Do:                                                                     //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Known Problems:                                                            //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// End_Html                                                                   //
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//                                                                            //
//                                                                            //
// Generated header file containing necessary includes                        //
#include "generated/RUNHTTask1GeneratedIncludes.h"
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/RUNHAnalyzer.h"
#include "generated/RUNHRomeDAQ.h"

#include "include/tasks/RUNHTTask1.h"
#include "Riostream.h"
#include "TRandom.h"

ClassImp(RUNHTTask1)

void RUNHTTask1::Init()
{
}

void RUNHTTask1::BeginOfRun()
{
   Int_t i;
   if (!strcmp(gAnalyzer->GetActiveDAQ()->GetName(), "none")) {
      gAnalyzer->GetRunInfo()->SetTriggerType(gAnalyzer->GetGSP()->GetTriggerType());
      cout<<"Trigger type is "<<gAnalyzer->GetRunInfo()->GetTriggerType()<<"."<<endl;

      cout<<"Calibration"<<endl;
      for (i = 0; i < 10; i++) {
         gAnalyzer->GetCalibrationAt(i)->SetPedestal(static_cast<Int_t>(gRandom->Gaus(100,1)));
         cout<<i<<" "<<gAnalyzer->GetCalibrationAt(i)->GetPedestal()<<endl;
      }
   }

   // read run header
   if (!strcmp(gAnalyzer->GetActiveDAQ()->GetName(), "rome")) {
      cout<<"Trigger type is "<<gAnalyzer->GetRunInfo()->GetTriggerType()<<"."<<endl;

      cout<<"Calibration"<<endl;
      for (i = 0; i < 10; i++) {
         cout<<i<<" "<<gAnalyzer->GetCalibrationAt(i)->GetPedestal()<<endl;
      }
   }
}

void RUNHTTask1::Event()
{
}

void RUNHTTask1::EndOfRun()
{
}

void RUNHTTask1::Terminate()
{
}

