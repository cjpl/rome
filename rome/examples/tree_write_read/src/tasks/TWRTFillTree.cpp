//// Author: 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TWRTFillTree                                                               //
//                                                                            //
// 
// 
// This header has been generated by the ROMEBuilder.
// The event methods have been written by .
// 
// Please note: The following information is only correct after executing the
//              ROMEBuilder.
// 
// This task accesses the following folders :
//    Data
// 
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "include/generated/TWRTFillTree.h"
#include "Riostream.h"

ClassImp(TWRTFillTree)

void TWRTFillTree::Init()
{
}

void TWRTFillTree::BeginOfRun()
{
}

void TWRTFillTree::Event()
{
   gAnalyzer->GetData()->SetValue(gAnalyzer->GetCurrentEventNumber());
}

void TWRTFillTree::EndOfRun()
{
}

void TWRTFillTree::Terminate()
{
}

