//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MEGTADCPedestal                                                            //
//                                                                            //
// Calculates the ADC Pedestal.                                                 
//                                                                            //
//                                                                            //
// This file has been generated by the ROMEBuilder.                           //
//                                                                            //
// This task contains the following histgrams :
// ADC0Histo
// ADC1Histo
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

#if defined( _MSC_VER )
#include <conio.h>
#include <strstream>
#endif
#if defined ( __linux__ )
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#endif

#include <TCanvas.h>
#include <TStyle.h>
#include <TF1.h>
#include <TH1.h>
#include <include/tasks/MEGTADCPedestal.h>
#include <Riostream.h>

ClassImp(MEGTADCPedestal)

int dfprintf(char *str, char *filename)
{
   ofstream ofile;
   ofile.open(filename,ofstream::out | ofstream::app);
   ofile<<str;
   ofile.close();
   return 0;
}
static int dfclear(char *filename) 
{
   ofstream ofile;
   ofile.open(filename,ofstream::out);
   ofile.close();
   return 0;
}
void MEGTADCPedestal::Init()
{
}

void MEGTADCPedestal::BeginOfRun()
{
   int nPMT = gAnalyzer->GetGeneralSteeringParameters()->GetPMT()->GetNumbers()->GetNumberOfPMT();
   char name[80],title[80];
   for (int j=0;j<nPMT;j++) {
      sprintf(name,"adc0_%i%i%i",j/100,(j%100)/10,j%10);
      sprintf(title,"ADC0 of PMT %s",gAnalyzer->GetCMPMTInfoAt(j)->GetAddress().Data());
      GetADC0HistoHandleAt(j)->SetName(name);
      GetADC0HistoHandleAt(j)->SetTitle(title);

      sprintf(name,"adc1_%i%i%i",j/100,(j%100)/10,j%10);
      sprintf(title,"ADC1 of PMT %s",gAnalyzer->GetCMPMTInfoAt(j)->GetAddress().Data());
      GetADC1HistoHandleAt(j)->SetName(name);
      GetADC1HistoHandleAt(j)->SetTitle(title);
   }
}

void MEGTADCPedestal::Event()
{
   int nPMT = gAnalyzer->GetGeneralSteeringParameters()->GetPMT()->GetNumbers()->GetNumberOfPMT();

   for (int i=0;i<nPMT;i++) {
      MEGCMPMTData *pmtData = gAnalyzer->GetCMPMTDataAt(i);
      int ipmt = gAnalyzer->GetCMPMTInfoAt(i)->GetADCID();
      FillADC0HistoAt(ipmt,pmtData->GetADC0Data());
      FillADC1HistoAt(ipmt,pmtData->GetADC1Data());
   }
}

void MEGTADCPedestal::EndOfRun()
{
   int nPMT = gAnalyzer->GetGeneralSteeringParameters()->GetPMT()->GetNumbers()->GetNumberOfPMT();
   int nLXePMT = gAnalyzer->GetGeneralSteeringParameters()->GetPMT()->GetNumbers()->GetNumberOfLXePMT();
   int ipmt,i,j;
   double *pedestal = new double[nPMT*2];    //pedestal
   double *spedestal = new double[nPMT*2];   //sigma of pedestal
   double *espedestal = new double[nPMT*2];  //err of sigma of pedestal
   char dfstring[1024];
   ostrstream dfstream(dfstring,sizeof(dfstring));

   char runNumber[6];
   char logfilename[gFileNameLength];
   sprintf(logfilename,"%spiedi_%s.log",gAnalyzer->GetOutputDir(),runNumber);
   dfclear(logfilename);
   char outfilename[gFileNameLength];
   sprintf(outfilename,"%sped%s.dat",gAnalyzer->GetOutputDir(),runNumber);

   TCanvas *c1 = new TCanvas(false);

   TStyle *mystyle = new TStyle("mystyle","");
   mystyle->cd();
   mystyle->SetOptFit(1);
   if (!gAnalyzer->isBatchMode()) {
      c1 = new TCanvas("c1");
      c1->cd();
   }
   TF1 *g1=new TF1("g1","gaus");//fit function
   TF1 *g2=new TF1("g2","pol1");//fit function
   g1->SetLineWidth(1);
   g2->SetLineWidth(1);
   g1->SetLineColor(2);
   g2->SetLineColor(2);

   TH1F *hist = new TH1F("","",0,0,0);;
   TH1F *pspread=new TH1F("pspread","Spread in Sigma",100,0,20);
   pspread->SetXTitle("pedestal spread in sigma [ch]");          
    
   //calculate pedestal
   for(ipmt=0 ; ipmt<nPMT ; ipmt++){
   	for(i=0;i<2;i++){
         if(i==0) {
            hist = GetADC0HistoHandleAt(ipmt);
            hist->SetXTitle("raw ADC 0 Sum[ch]");
         }
         else {
            hist = GetADC1HistoHandleAt(ipmt);
            hist->SetXTitle("raw ADC 1 Sum[ch]");
         }
    	   float xmin = hist->GetMean() - 50.;
	      float xmax = hist->GetMean() + 50.;	  
	      hist->Fit("g1","Q,0","R",xmin,xmax);
	      hist->GetXaxis()->SetRange(hist->GetXaxis()->FindBin(xmin),
		         hist->GetXaxis()->FindBin(xmax));
         if (!gAnalyzer->isBatchMode()) {
	         hist->Draw();
//	         g1->Draw("same");
//            c1->Update();
         }

         pedestal[ipmt+i*nPMT]  = g1->GetParameter(1);
	      spedestal[ipmt+i*nPMT] = g1->GetParameter(2);
	      espedestal[ipmt+i*nPMT]= g1->GetParError(2);

         if(i==0 && pedestal[ipmt]>GetSP()->GetPedestalThreshold()){
	         dfstream.seekp(0);
	         dfstream<<"ADC \033[31m"<< setw(3) << ipmt<<"\t  :"
		         <<setprecision(1)<<setiosflags(ios::fixed)<<setfill(' ')<<setw(5)
		         <<pedestal[ipmt]<<"\033[m\tch"<<endl<<ends;
            dfprintf(dfstring,logfilename);
   	   }
   	}
   }

    
   //Draw pedestal spread distribution
   for(j=0;j<nLXePMT;j++){
      pspread->Fill(spedestal[j]);
   }
   if (!gAnalyzer->isBatchMode()) {
      pspread->Draw();
      c1->SetLogy(1);
      c1->Update();
   }

   //out put pedestal file
   ofstream ofile(outfilename);
   for(i=0;i<2;i++){
      for(ipmt=0;ipmt<nPMT;ipmt++){
	      ofile<<setprecision(9)<<setiosflags(ios::fixed)
	         <<" "<<pedestal[ipmt+i*nPMT]<<" "<<spedestal[ipmt+i*nPMT]
	         <<" "<<espedestal[ipmt+i*nPMT]<<endl;
      }
   }
   ofile.close();

   dfstream.seekp(0);
   dfstream<<"------------------------------"<<endl
	    << "PMTs which have broaden (sigma>" 
	    << GetSP()->GetPedestalSpreadThreshold() << ") pedestal"<<endl<<ends;    
   dfprintf(dfstring,logfilename);

   for(ipmt=0;ipmt<nPMT;ipmt++){
      if (spedestal[ipmt]>GetSP()->GetPedestalSpreadThreshold()){
	      dfstream.seekp(0);
	      dfstream<<"ADC "<< setw(3)<<ipmt <<":\t"
		      << setprecision(1)<<setiosflags(ios::fixed)<<setfill(' ')<<setw(4)
		      <<spedestal[ipmt]<<" ch"<<endl<<ends;
	      dfprintf(dfstring,logfilename);
      }
      if (spedestal[ipmt]==0){ 
	      dfstream.seekp(0);
	      dfstream<< "ADC "<< setw(3)<< ipmt <<":\tNO SIGNAL"<<endl<<ends;
	      dfprintf(dfstring,logfilename);
      }
   }
//	      hist->GetXaxis()->SetRange(hist->GetXaxis()->FindBin(0.),
//		       hist->GetXaxis()->FindBin(4096.));
   delete pedestal;
   delete spedestal;
   delete espedestal;
   delete pspread;
}

void MEGTADCPedestal::Terminate()
{
}

