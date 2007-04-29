// $Id$
// Author: Ryu Sawada

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusTextDialog                                                      //
//                                                                      //
// Dialogo bject which asks string                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ArgusTextDialog.h"

ClassImp(ArgusTextDialog)

//______________________________________________________________________________
ArgusTextDialog::ArgusTextDialog(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
                                 Char_t *label, Char_t *ret_str, UInt_t options)
:TGTransientFrame(p, main, w, h, options)
,fF1(0)
,fF2(0)
,fOkButton(0)
,fCancelButton(0)
,fL1(0)
,fL5(0)
,fL6(0)
,fL21(0)
,fText(0)
,fBLabel(0)
,fLabel(0)
,fRetStr(ret_str)
{
   // Create a dialog to enter a single line text entry
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

//______________________________________________________________________________
ArgusTextDialog::~ArgusTextDialog()
{
   // Clean up text dialog
#if 0 // deleting GUI objects may cause error
   SafeDelete(fCancelButton);
   SafeDelete(fOkButton);
   SafeDelete(fText);
   SafeDelete(fLabel);
   SafeDelete(fF1);
   SafeDelete(fF2);
   SafeDelete(fL1);
   SafeDelete(fL5);
   SafeDelete(fL6);
   SafeDelete(fL21);
#endif
}

//______________________________________________________________________________
void ArgusTextDialog::CloseWindow()
{
   // Close the dialog. On close the dialog will be deleted and cannot be
   // re-used.
   DeleteWindow();
}

//______________________________________________________________________________
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
