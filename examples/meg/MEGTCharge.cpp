//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MEGTCharge                                                                 //
//                                                                            //
// Calculates the Charge.                                                       
// 
// 
// This header has been generated by the ROMEBuilder.
// The event methods have been written by Matthias Schneebeli.
// 
// Please note: The following information is only correct after executing the
//              ROMEBuilder.
// 
// This task accesses the following folders :
//    CMPMTData
//    CMPMTInfo
//    CMHit
//    Trigger
// 
// This task contains the following histgrams :
//    FrontMapHisto
//    BackMapHisto
//    TopMapHisto
//    BottomMapHisto
//    RightMapHisto
//    LeftMapHisto
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
// Get<Histogram Name>Handle()
//
// For histogram arrays use :
//
// Get<Histogram Name>HandleAt(int index)
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include <TCanvas.h>
#include <ROME.h>
#include "MEG.h"
#include <include/tasks/MEGTCharge.h>

ClassImp(MEGTCharge)

void MEGTCharge::Init()
{
}

void MEGTCharge::BeginOfRun()
{
   int nMapHistos = GetSP()->GetNumberOfMapHistos();
   char name[80],title[80];
   for (int j=0;j<nMapHistos;j++) {
      sprintf(name,"frontmap%i%i",j/10,j%10);
      sprintf(title,"Front side Map for event %d",j+1);
      GetFrontMapHistoHandleAt(j)->SetName(name);
      GetFrontMapHistoHandleAt(j)->SetTitle(title);

      sprintf(name,"backmap%i%i",j/10,j%10);
      sprintf(title,"Back side Map for event %d",j+1);
      GetBackMapHistoHandleAt(j)->SetName(name);
      GetBackMapHistoHandleAt(j)->SetTitle(title);

      sprintf(name,"topmap%i%i",j/10,j%10);
      sprintf(title,"Top side Map for event %d",j+1);
      GetTopMapHistoHandleAt(j)->SetName(name);
      GetTopMapHistoHandleAt(j)->SetTitle(title);

      sprintf(name,"bottommap%i%i",j/10,j%10);
      sprintf(title,"Bottom side Map for event %d",j+1);
      GetBottomMapHistoHandleAt(j)->SetName(name);
      GetBottomMapHistoHandleAt(j)->SetTitle(title);

      sprintf(name,"rightmap%i%i",j/10,j%10);
      sprintf(title,"Right side Map for event %d",j+1);
      GetRightMapHistoHandleAt(j)->SetName(name);
      GetRightMapHistoHandleAt(j)->SetTitle(title);

      sprintf(name,"leftmap%i%i",j/10,j%10);
      sprintf(title,"Left side Map for event %d",j+1);
      GetLeftMapHistoHandleAt(j)->SetName(name);
      GetLeftMapHistoHandleAt(j)->SetTitle(title);
   }
}

void MEGTCharge::Event()
{
   int nMapHistos = GetSP()->GetNumberOfMapHistos();
   int nPMT = gAnalyzer->GetGSP()->GetPMT()->GetNumbers()->GetNumberOfPMT();

   int eventNumber,face;
   int   j;
   float x_coor, y_coor, xmean, ymean, xsqr, ysqr;
   float qtot_back, qtot_left, qtot_front;
   float qtot_right, qtot_top, qtot_bottom;
   float pmt_signal;

   // fill front face map histos
   xmean = 0.;
   ymean = 0.;
   xsqr  = 0.;
   ysqr  = 0.;
   qtot_front = 0.;
   qtot_back  = 0.;
   qtot_left  = 0.;
   qtot_right = 0.;
   qtot_top   = 0.;
   qtot_bottom= 0.;
   for (j=0; j<nPMT; j++) {
      MEGCMPMTData *pmtData = gAnalyzer->GetCMPMTDataAt(j);
      MEGCMPMTInfo *pmtInfo = gAnalyzer->GetCMPMTInfoAt(j);
      pmt_signal = TMath::Max(pmtData->GetADC0Data(),0.f);
      eventNumber = gAnalyzer->GetTrigger()->GetEventnumber();
      x_coor= pmtInfo->GetX();
      y_coor= pmtInfo->GetY();
      face = pmtInfo->GetFace();
      if (face==FRONT) {
         xmean+=pmt_signal*x_coor;
         ymean+=pmt_signal*y_coor;
         xsqr +=pmt_signal*x_coor*x_coor;
         ysqr +=pmt_signal*y_coor*y_coor;
         qtot_front+=pmt_signal;
         if (eventNumber<nMapHistos) {
            FillFrontMapHistoAt(eventNumber,x_coor,y_coor,pmt_signal);
         }
      }
      if (face==BACK) {
         if (eventNumber<nMapHistos) {
            FillBackMapHistoAt(eventNumber,x_coor,y_coor,pmt_signal);
         }
         qtot_back += pmt_signal;
      }
      if (face==TOP) {
         if (eventNumber<nMapHistos) {
            FillTopMapHistoAt(eventNumber,x_coor,y_coor,pmt_signal);
         }
         qtot_top += pmt_signal;
      }
      if (face==BOTTOM) {
         if (eventNumber<nMapHistos) {
            FillBottomMapHistoAt(eventNumber,x_coor,y_coor,pmt_signal);
         }
         qtot_bottom += pmt_signal;
      }
      if (face==RIGHT) {
         if (eventNumber<nMapHistos) {
            FillRightMapHistoAt(eventNumber,x_coor,y_coor,pmt_signal);
         }
         qtot_right += pmt_signal;
      }
      if (face==LEFT) {
         if (eventNumber<nMapHistos) {
            FillLeftMapHistoAt(eventNumber,x_coor,y_coor,pmt_signal);
         }
         qtot_left += pmt_signal;
      }
   }

   xmean = ((qtot_front==0) ? 0. : xmean/qtot_front);
   ymean = ((qtot_front==0) ? 0. : ymean/qtot_front);
   xsqr =  ((qtot_front==0) ? 0. : xsqr/qtot_front );
   ysqr =  ((qtot_front==0) ? 0. : ysqr/qtot_front );

//   hist->sigma     = (xsqr - xmean*xmean + ysqr - ymean*ymean)/2;

   Float_t qsum = 0.f;
   Float_t nhit = 0;
   Float_t qraw = qtot_left + qtot_right + qtot_top + qtot_bottom + qtot_front + qtot_back;
/*   for (i=0 ; i<N_XEPMT ; i++) {
      if (cadc[i] > adccalib_param.pedestal_sigma[i]*3 &&
         cadc[i] > 3.f) {
         qsum += cadc[i];
         nhit++;
      }
   }
*/
   gAnalyzer->GetCMHit()->SetAll(0,0,0,0,0,xmean,ymean,qtot_front,qtot_back,qtot_top,
      qtot_bottom,qtot_right,qtot_left,qraw,qsum,nhit);

/*   gHistoHists1[1]->Fill((float)hist->qsum);
  
  / fill 2D histos for noise correlation, SR 4.Feb.02
  gHistoHists2[0]->Fill(cadc[123],cadc[24]);
  gHistoHists2[1]->Fill(cadc[123],cadc[117]);
  gHistoHists2[2]->Fill(cadc[123],cadc[119]);
  gHistoHists2[3]->Fill(cadc[123],cadc[120]);
*/
}

void MEGTCharge::EndOfRun()
{
   int nMapHistos = GetSP()->GetNumberOfMapHistos();
   if (gAnalyzer->isBatchMode()) return;
   TCanvas *c1 = new TCanvas("c1","Map Histos",600,400);

   for (int i=0;i<nMapHistos;i++) {
      DrawFrontMapHistoAt(i);
      c1->Update();
      DrawBackMapHistoAt(i);
      c1->Update();
      DrawTopMapHistoAt(i);
      c1->Update();
      DrawBottomMapHistoAt(i);
      c1->Update();
      DrawRightMapHistoAt(i);
      c1->Update();
      DrawLeftMapHistoAt(i);
      c1->Update();
   }
}

void MEGTCharge::Terminate()
{
}

