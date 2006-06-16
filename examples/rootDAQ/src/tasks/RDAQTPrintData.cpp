////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// RDAQTPrintData                                                             //
//                                                                            //
//                                                                            //
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/RDAQAnalyzer.h"

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
   cout << "T2:" << endl;
   cout << "np = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetNp() << endl;
   cout << "px = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPx() << endl;
   cout << "py = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPy() << endl;
   cout << "pz = " << gAnalyzer->GetRootDAQ()->GetT2()->GetBranch1()->GetPz() << endl;
   cout << endl;
}

void RDAQTPrintData::EndOfRun()
{
}

void RDAQTPrintData::Terminate()
{
}

