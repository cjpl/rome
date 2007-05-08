////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TWR2TPrintValue                                                            //
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
#include "generated/TWR2Raw.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/TWR2Raw.h"
#include "RawData.h"
#include "generated/TWR2Analyzer.h"
#include "tasks/TWR2TPrintValue.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "TWR2AllFolders.h"


ClassImp(TWR2TPrintValue)

void TWR2TPrintValue::Init()
{
}

void TWR2TPrintValue::BeginOfRun()
{
}

void TWR2TPrintValue::Event()
{
   cout << gAnalyzer->GetRawAt(0)->GetData()->GetData() << endl;
}

void TWR2TPrintValue::EndOfRun()
{
}

void TWR2TPrintValue::Terminate()
{
}

