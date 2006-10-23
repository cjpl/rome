// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FOTFillHisto                                                               //
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
// The event methods have been written by Ryu Sawada.                         //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
//                                                                            //
// This task contains the following histgrams :                               //
//    MyHisto                                                                 //
//                                                                            //
// The histograms are created and saved automaticaly by the task.             //
//                                                                            //
// The following method can be used to get a handle of the histogram :        //
//                                                                            //
// Get<Histogram Name>()                                                      //
//                                                                            //
// For histogram arrays use :                                                 //
//                                                                            //
// Get<Histogram Name>At(Int_t index)                                         //
#include "generated/FOMidasDAQ.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/FOAnalyzer.h"
#include "tasks/FOTFillHisto.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "FOAllFolders.h"


ClassImp(FOTFillHisto)

void FOTFillHisto::Init()
{
}

void FOTFillHisto::BeginOfRun()
{
}

void FOTFillHisto::Event()
{
   FillMyHisto(gAnalyzer->GetMidasDAQ()->GetADC0BankAt(0));
}

void FOTFillHisto::EndOfRun()
{
}

void FOTFillHisto::Terminate()
{
}
