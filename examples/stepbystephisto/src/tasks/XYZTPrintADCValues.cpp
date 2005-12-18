//// Author: 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// XYZTPrintADCValues                                                         //
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
// This task contains the following histgrams :
//    ADC
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

#include <include/generated/XYZTPrintADCValues.h>
#include <TRandom.h>
#include <Riostream.h>
#include "TF1.h" 

Double_t lognormal(Double_t *x, Double_t *par) { 
    return TMath::LogNormal(x[0]-par[3],par[0],par[1],par[2]); 
} 

ClassImp(XYZTPrintADCValues)

void XYZTPrintADCValues::Init()
{
}

void XYZTPrintADCValues::BeginOfRun()
{
}

void XYZTPrintADCValues::Event()
{
   gAnalyzer->GetPMTData()->SetADC((int)gRandom->Gaus(50,10));
   GetADC()->Fill(gAnalyzer->GetPMTData()->GetADC());

   if (gAnalyzer->GetGSP()->GetOutputOnOff()) {
      cout << gAnalyzer->GetPMTData()->GetADC() << endl;
      cout << endl;
   }
}

void XYZTPrintADCValues::EndOfRun()
{
   TF1 *fitFcn = new TF1("fitFcn",lognormal,0,1,4); 

   fitFcn->SetParameters(0.001, 0, 1, 0); 
   fitFcn->SetParName(0, "sigma"); 
   fitFcn->SetParName(1, "theta"); 
   fitFcn->SetParName(2, "m"); 
   fitFcn->SetParName(3, "x_offset"); 

   GetADC()->Fit(fitFcn,"r"); 
   Double_t par[4]; 
   fitFcn->GetParameters(par); 

}

void XYZTPrintADCValues::Terminate()
{
}

