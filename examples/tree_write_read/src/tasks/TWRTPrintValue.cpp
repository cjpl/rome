////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TWRTPrintValue                                                             //
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
// Followings are include files of folders. ROMEBuilder will update it with   //
// reading this source code when it is executed next time.                    //
#include "generated/TWRData1.h"
#include "generated/TWRData2.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/TWRAnalyzer.h"

#include "include/tasks/TWRTPrintValue.h"
#include "Riostream.h"

ClassImp(TWRTPrintValue)

void TWRTPrintValue::Init()
{
}

void TWRTPrintValue::BeginOfRun()
{
}

void TWRTPrintValue::Event()
{
  cout << gAnalyzer->GetData1()->GetValue1() << "\t"
       << gAnalyzer->GetData2()->GetValue2() << endl;
}

void TWRTPrintValue::EndOfRun()
{
}

void TWRTPrintValue::Terminate()
{
}

