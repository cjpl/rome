// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGTTab                                                                     //
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
// Generated header file containing necessary includes                        //
#include "generated/HGTTabGeneratedIncludes.h"
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/HGAnalyzer.h"
#include "include/generated/HGWindow.h"
#include "include/tabs/HGTTab.h"

// uncomment if you want to include headers of all folders
//#include "HGAllFolders.h"


ClassImp(HGTTab)

void HGTTab::Init()
{
   // Create a vertical frame containing buttons and canvas 
   fVert = new TGVerticalFrame(this, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create an embedded canvas and add to the main frame, centered in x and y 
   fCanvas = new TRootEmbeddedCanvas("Sample Canvas", fVert, (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create "Update" button 
   fBUpdate = new TGTextButton(fVert, "Update", B_UPDATE);
   fBUpdate->Associate(this);

   fVert->AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   fVert->AddFrame(fBUpdate, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
}

void HGTTab::EndInit()
{
}

void HGTTab::Update()
{
   if (!gAnalyzer->GetNetFolder("mynetfolder"))
      return;

   fCanvas->GetCanvas()->cd();

   if (fHisto) {
      delete fHisto;
   }
   fHisto = (TH1F *) gAnalyzer->GetNetFolder("mynetfolder")->FindObjectAny("hAdc");
   if (!fHisto) {
      cout << "Histo hAdc not available." << endl;
   }
   else {
      fHisto->Draw();
   }

   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
   return;
}

bool HGTTab::ProcessMessage(Long_t msg, Long_t param1, Long_t /*param2*/)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1) {
         case B_UPDATE:
            Update();
            break;
         }
         break;
      }
      break;
   }
   return true;
}

void HGTTab::EventHandler()
{
}

void HGTTab::MenuClicked(TGPopupMenu * /*menu*/,Long_t /*param*/)
{
}

void HGTTab::TabSelected()
{
}
void HGTTab::TabUnSelected()
{
}

