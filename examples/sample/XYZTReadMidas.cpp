//// Author: 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// XYZTReadMidas                                                              //
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
//    PMTData
// 
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include <include/tasks/XYZTReadMidas.h>
#include <Riostream.h>

ClassImp(XYZTReadMidas)

void XYZTReadMidas::Init()
{
}

void XYZTReadMidas::BeginOfRun()
{
}

void XYZTReadMidas::Event()
{
   for (int i=0;i<228;i++) {
	   Float_t adcValue = gAnalyzer->GetADC0BankAt(i);
      gAnalyzer->GetPMTDataAt(i)->SetADC(adcValue);
   }
}

void XYZTReadMidas::EndOfRun()
{
}

void XYZTReadMidas::Terminate()
{
}

