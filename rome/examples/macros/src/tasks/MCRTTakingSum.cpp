////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MCRTTakingSum                                                              //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//     PMTData                                                                //
//     Sum                                                                    //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/MCRTTakingSumGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/MCRAnalyzer.h"
#include "tasks/MCRTTakingSum.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "MCRAllFolders.h"


ClassImp(MCRTTakingSum)

void MCRTTakingSum::Init()
{
}

void MCRTTakingSum::BeginOfRun()
{
}

void MCRTTakingSum::Event()
{
   Int_t iPmt;
   Int_t ADCSum = 0;

   for(iPmt = 0; iPmt < 10; iPmt++)
      ADCSum += gAnalyzer->GetPMTDataAt(iPmt)->GetADC();

   gAnalyzer->GetSum()->SetADCSum(ADCSum);
}

void MCRTTakingSum::EndOfRun()
{
}

void MCRTTakingSum::Terminate()
{
}

