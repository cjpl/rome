//// Author: Ryu Sawada
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGTMakeHistogram                                                          //
//                                                                            //
// Create and fill histograms at the begin of run                               
// 
// 
// This header has been generated by the ROMEBuilder.
// The event methods have been written by Ryu Sawada.
// 
// Please note: The following information is only correct after executing the
//              ROMEBuilder.
// 
// This task contains the following histgrams :
//    hAdc0
//
// The histograms are created and saved automaticaly by the task.
//
// The following method can be used to fill a histogram :
//
// Fill<Histogram Name>(double value,double weight)
//
// For histogram arrays use :
//
// Fill<Histogram Name>At(int index,double value,double weight)
//
// If more histogram functions are needed use the following function the get
// a handle to the histogram and use the root functions.
//
// Get<Histogram Name>()
//
// For histogram arrays use :
//
// Get<Histogram Name>At(int index)
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include <include/tasks/HGTMakeHistogram.h>
#include <Riostream.h>
#include <TRandom.h>

ClassImp(HGTMakeHistogram)

void HGTMakeHistogram::Init()
{
   TRandom *rndm = new TRandom();
   int i;
   for(i=0;i<10000;i++){
      FillhAdc0(rndm->Gaus(2000,200));
   }
}

void HGTMakeHistogram::BeginOfRun()
{
}

void HGTMakeHistogram::Event()
{
}

void HGTMakeHistogram::EndOfRun()
{
}

void HGTMakeHistogram::Terminate()
{
}

