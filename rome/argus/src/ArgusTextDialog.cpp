// $Id$
// Author: Ryu Sawada

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusTextDialog                                                      //
//                                                                      //
// Dialogo bject which asks string                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include "ArgusTextDialog.h"

ClassImp(ArgusTextDialog)

ArgusTextDialog::ArgusTextDialog(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, Char_t *label, Char_t *ret_str, UInt_t options):TGTransientFrame(p, main, w, h, options)
{
   // Create a dialog to enter a single line text entry
   fRetStr = ret_str;

   ChangeOptions((GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

   fF1 = new TGCompositeFrame(this, 60, 20, kVerticalFrame | kFixedWidth);
   fF2 = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);

   fOkButton = new TGTextButton(fF1, new TGHotString("&Ok"), 1);
   fCancelButton = new TGTextButton(fF1, new TGHotString("&Cancel"), 2);
   fF1->Resize(fOkButton->GetDefaultWidth() + 40, GetDefaultHeight());

   fOkButton->Associate(this);
   fCancelButton->Associate(this);

   fL1 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 3, 0);
   fL21 = new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 5, 10, 0);

   fF1->AddFrame(fOkButton, fL1);
   fF1->AddFrame(fCancelButton, fL1);
   AddFrame(fF1, fL21);

   fLabel = new TGLabel(fF2, new TGHotString(label));

   fBLabel = new TGTextBuffer(50);
   if (fRetStr)
      fBLabel->AddText(0, fRetStr);
   else
      fOkButton->SetState(kButtonDisabled);

   fText = new TGTextEntry(fF2, fBLabel);
   fText->Associate(this);
   fText->Resize(220, fText->GetDefaultHeight());
   fText->SelectAll();

   fL5 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 3, 5, 0, 0);
   fL6 = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 0, 2, 0, 0);

   fF2->AddFrame(fLabel, fL5);
   fF2->AddFrame(fText, fL5);
   AddFrame(fF2, fL1);

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Int_t ax, ay;
   if (main) {
      Window_t wdum;
      Int_t mx, my; // geometry of main
      UInt_t mw, mh; // geometry of main
      gVirtualX->GetWindowSize(main->GetId(), mx, my, mw, mh);
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(), mw >> 1, mh >> 1, ax, ay, wdum);
   }
   else {
      UInt_t root_w, root_h;
      gVirtualX->GetWindowSize(fClient->GetRoot()->GetId(), ax, ay, root_w, root_h);
      ax = (root_w - fWidth) >> 1;
      ay = (root_h - fHeight) >> 1;
   }

   Move(ax, ay);
   SetWMPosition(ax, ay);

   SetWindowName("Enter Text");

   SetMWMHints(kMWMDecorAll | kMWMDecorMaximize | kMWMDecorMenu, kMWMFuncAll | kMWMFuncMaximize | kMWMFuncResize, kMWMInputModeless);

   MapWindow();
   fClient->WaitFor(this);
}

ArgusTextDialog::~ArgusTextDialog()
{
   // Clean up text dialog   
   delete fCancelButton;
   delete fOkButton;
   delete fText;
   delete fLabel;
   delete fF1;
   delete fF2;
   delete fL1;
   delete fL5;
   delete fL6;
   delete fL21;
}

void ArgusTextDialog::CloseWindow()
{
   // Close the dialog. On close the dialog will be deleted and cannot be
   // re-used.  
   DeleteWindow();
}

Bool_t ArgusTextDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   const Char_t *string;
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (parm1) {
         case 1:
            if (fRetStr) {
               strcpy(fRetStr, fBLabel->GetString());
            }
            CloseWindow();
            break;
         case 2:
            fRetStr[0] = 0;
            CloseWindow();
            break;
         }
         break;

      default:
         break;
      }
      break;
   case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {
      case kTE_TEXTCHANGED:
         string = fBLabel->GetString();
         if (strlen(string) == 0)
            fOkButton->SetState(kButtonDisabled);
         else
            fOkButton->SetState(kButtonUp);
         break;
      case kTE_ENTER:
         if (fRetStr) {
            strcpy(fRetStr, fBLabel->GetString());
         }
         CloseWindow();
         break;
      default:
         break;
      }
      break;
   default:
      break;
   }
   return kTRUE;
}
