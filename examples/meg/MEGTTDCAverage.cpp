//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MEGTTDCAverage                                                             //
//                                                                            //
// Calculates the TDC Average.                                                  
//                                                                            //
//                                                                            //
// This file has been generated by the ROMEBuilder.                           //
//                                                                            //
// This task contains the following histgrams :
// TDCAverageHisto
//
// The histograms are created and saved automaticaly by the task.
//
// The following methods can be used to fill the histogram and to set the
// right name,title and binwidth of the histogram :
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
// Get<Histogram Name>Handle()
//
// For histogram arrays use :
//
// Get<Histogram Name>HandleAt(int index)
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include <TCanvas.h>
#include "MEG.h"
#include "MEGTTDCAverage.h"
#include "Riostream.h"

ClassImp(MEGTTDCAverage)

void MEGTTDCAverage::Init()
{
}

void MEGTTDCAverage::BeginOfRun()
{
}

void MEGTTDCAverage::Event()
{
   int nPMT = fAnalyzer->GetGeneralSteeringParameters()->GetPMT()->GetNumbers()->GetNumberOfPMT();
   float invalid = fAnalyzer->GetGeneralSteeringParameters()->GetInvalidValue();
//   cout <<fAnalyzer->GetTriggerObject()->GetID()<<endl;
   if (fAnalyzer->GetTriggerObject()->GetID()!=1) return;

   float sum = 0;
   int nsum = 0;
   Bool_t corrupt = false;
   for (int j=0;j<nPMT&&!corrupt;j++) {
      MEGCMPMTData *pmtData = fAnalyzer->GetCMPMTDataAt(j);
      MEGCMPMTInfo *pmtInfo = fAnalyzer->GetCMPMTInfoAt(j);
      if (pmtInfo->GetFace()==FRONT) {
         if (pmtData->GetTDCData()!=invalid) {
            sum = sum + pmtData->GetTDCData();
            nsum++;
         }
         else corrupt = true;
      }
   }
   if (!corrupt) FillTDCAverageHisto(sum/nsum);
}

void MEGTTDCAverage::EndOfRun()
{
   if (fAnalyzer->isBatchMode()) return;
   TCanvas *c1 = new TCanvas("c1","Average of Frontface TDCs",600,400);

   this->DrawTDCAverageHisto();
}

void MEGTTDCAverage::Terminate()
{
}

