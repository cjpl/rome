////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MIDTPrintValues                                                            //
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
#include "generated/MIDODB.h"
#include "generated/MIDGlobalSteering.h"
#include "generated/MIDMidasDAQ.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "TSystem.h"
#include "generated/MIDAnalyzer.h"
#include "generated/MIDGlobalSteering.h"
#include "tasks/MIDTPrintValues.h"
#include "MIDODB.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "MIDAllFolders.h"


ClassImp(MIDTPrintValues)

void MIDTPrintValues::Init()
{
}

void MIDTPrintValues::BeginOfRun()
{
   cout << "Run Number : " << gAnalyzer->GetODB()->GetRunNumber() << endl;
   cout << "Start Time : " << gAnalyzer->GetODB()->GetStartTime().Data() << endl;
}

void MIDTPrintValues::Event()
{
   int i;
   cout << "\nEvent : " << gAnalyzer->GetCurrentEventNumber() << endl;
   cout << "TaskTestValue : " << gAnalyzer->GetPrintValuesTask()->GetSP()->GetTaskTestValue() << endl;
   cout << "TestValue : " << gAnalyzer->GetGSP()->GetTestValue() << endl;
   cout << "ADC0" << endl;
   for (i=0;i<gAnalyzer->GetMidasDAQ()->GetADC0BankEntries();i++) {
      cout << gAnalyzer->GetMidasDAQ()->GetADC0BankAt(i) << endl;
   }
   cout << "TDC0" << endl;
   for (i=0;i<gAnalyzer->GetMidasDAQ()->GetTDC0BankEntries();i++) {
      cout << gAnalyzer->GetMidasDAQ()->GetTDC0BankAt(i) << endl;
   }
   cout << endl;
   cout << "SCLR" << endl;
   for (i=0;i<gAnalyzer->GetMidasDAQ()->GetSCLRBankEntries();i++) {
      cout << gAnalyzer->GetMidasDAQ()->GetSCLRBankAt(i) << endl;
   }
   cout << endl;
   gSystem->Sleep(10);
}

void MIDTPrintValues::EndOfRun()
{
}

void MIDTPrintValues::Terminate()
{
}

