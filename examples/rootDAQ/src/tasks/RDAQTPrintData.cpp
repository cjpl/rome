////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// RDAQTPrintData                                                             //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/RDAQTPrintDataGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/RDAQAnalyzer.h"
#include "generated/RDAQRootDAQ.h"

#include "tasks/RDAQTPrintData.h"
#include <Riostream.h>

// uncomment if you want to include headers of all folders
//#include "RDAQAllFolders.h"


ClassImp(RDAQTPrintData)

void RDAQTPrintData::Init()
{
}

void RDAQTPrintData::BeginOfRun()
{
}

void RDAQTPrintData::Event()
{
   cout << "T1:" << endl;
   cout << "np = " << gAnalyzer->GetRootDAQ()->GetT1()->GetNp() << endl;
   cout << "px = " << gAnalyzer->GetRootDAQ()->GetT1()->GetPx() << endl;
   cout << "py = " << gAnalyzer->GetRootDAQ()->GetT1()->GetPy() << endl;
   cout << "pz = " << gAnalyzer->GetRootDAQ()->GetT1()->GetPz() << endl;
   cout << "pp[0] = " << gAnalyzer->GetRootDAQ()->GetT1()->GetPpAt(0) << endl;
   cout << "pp[1] = " << gAnalyzer->GetRootDAQ()->GetT1()->GetPpAt(1) << endl;
   cout << "pp[2] = " << gAnalyzer->GetRootDAQ()->GetT1()->GetPpAt(2) << endl;
   cout << "T2:" << endl;
   cout << "np = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetNp() << endl;
   cout << "px = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPx() << endl;
   cout << "py = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPy() << endl;
   cout << "pz = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPz() << endl;
   cout << "pp[0] = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPpAt(0) << endl;
   cout << "pp[1] = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPpAt(1) << endl;
   cout << "pp[2] = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPpAt(2) << endl;
   cout << endl;
}

void RDAQTPrintData::EndOfRun()
{
}

void RDAQTPrintData::Terminate()
{
}

