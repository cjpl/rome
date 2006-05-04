//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// XMLToFormWindow                                                            //
//                                                                            //
// Window class for the XMLToForm class.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////
// $Id: XMLToFormWindow.cpp 3402 2006-04-30 13:10:16Z Matthias Schneebeli $

#include "XMLToFormWindow.h"

ClassImp(XMLToFormWindow)

void XMLToFormWindow::BuildFrame(XMLToFormFrame *frame)
{
   int i,j,k;
   int maxWidth,index;
   TGLabel *tempLabel;
   XMLToFormFrame *currentSubFrame;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;

   // create title
   if (frame->fTitleLabelHotString) {
      if (frame->fIsTab) {
         frame->fParentFrame->fTab->GetTabTab(frame->fTabIndex)->SetText(frame->fTitleLabelHotString);
      }
      else {
         frame->fTitleLabel = new TGLabel(frame->fFrame, frame->fTitleLabelHotString);
         frame->fTitleLabel->SetTextFont("arial bold",true);
         frame->fFrame->AddFrame(frame->fTitleLabel, frame->fLTitleLabel);
      }
   }

   // create elements

   if (frame->fNumberOfElements>0) {
      // calculate number of vframes
      maxWidth = 0;
      // editboxes
      for (j=0;j<frame->fNumberOfEditBoxes;j++) {
         tempLabel = new TGLabel(0,frame->fEditLabelHotString[j]->GetString());
         if (frame->fEditBoxWidth[j]<(int)tempLabel->GetSize().fWidth)
            frame->fEditBoxWidth[j] = tempLabel->GetSize().fWidth;
         if (maxWidth<frame->fEditBoxWidth[j])
            maxWidth = frame->fEditBoxWidth[j]+2*elementPad;
         delete tempLabel;
      }
      // buttons
      for (j=0;j<frame->fNumberOfButtons;j++) {
         tempLabel = new TGLabel(0,frame->fButtonHotString[j]->GetString());
         if (frame->fButtonWidth[j]<(int)tempLabel->GetSize().fWidth+8)
            frame->fButtonWidth[j] = tempLabel->GetSize().fWidth+8;
         if (maxWidth<frame->fButtonWidth[j])
            maxWidth = frame->fButtonWidth[j]+2*elementPad;
         delete tempLabel;
      }
      // comboboxes
      for (j=0;j<frame->fNumberOfComboBoxes;j++) {
         tempLabel = new TGLabel(0,frame->fComboLabelHotString[j]->GetString());
         if (frame->fComboBoxWidth[j]<(int)tempLabel->GetSize().fWidth+8)
            frame->fComboBoxWidth[j] = tempLabel->GetSize().fWidth+8;
         if (maxWidth<frame->fComboBoxWidth[j])
            maxWidth = frame->fComboBoxWidth[j]+2*elementPad;
         delete tempLabel;
         for (k=0;k<frame->fComboBoxEntries[j]->GetEntriesFast();k++) {
            tempLabel = new TGLabel(0,frame->fComboBoxEntries[j]->At(k).Data());
            if (frame->fComboBoxWidth[j]<(int)tempLabel->GetSize().fWidth)
               frame->fComboBoxWidth[j] = tempLabel->GetSize().fWidth;
            if (maxWidth<frame->fComboBoxWidth[j])
               maxWidth = frame->fComboBoxWidth[j]+2*elementPad;
            delete tempLabel;
         }
      }
      // checkbuttons
      for (j=0;j<frame->fNumberOfCheckButtons;j++) {
         tempLabel = new TGLabel(0,frame->fCheckButtonLabelHotString[j]->GetString());
         if (frame->fCheckButtonWidth[j]<(int)tempLabel->GetSize().fWidth)
            frame->fCheckButtonWidth[j] = tempLabel->GetSize().fWidth;
         if (maxWidth<frame->fCheckButtonWidth[j])
            maxWidth = frame->fCheckButtonWidth[j]+2*elementPad;
         delete tempLabel;
      }


      // create hframes
      frame->fHFrame = new TGHorizontalFrame(frame->fFrame,0,0);
      frame->fFrame->AddFrame(frame->fHFrame, frame->fLInnerFrame);
      // create hhframes
      frame->fHHFrames = new TGHorizontalFrame*[2*frame->fNumberOfElements];
      // create vframes
      frame->fNumberOfVFrames = (fMaximalWindowWidth+2*framePad)/(maxWidth);
      if (frame->fNumberOfVFrames<1)
         frame->fNumberOfVFrames = 1;
      if (frame->fNumberOfVFrames>frame->fNumberOfElements)
         frame->fNumberOfVFrames=frame->fNumberOfElements;
      frame->fVFrames = new TGVerticalFrame*[frame->fNumberOfVFrames];
      for (j=0;j<frame->fNumberOfVFrames;j++) {
         frame->fVFrames[j] = new TGVerticalFrame(frame->fHFrame,0,0);
         frame->fHFrame->AddFrame(frame->fVFrames[j], frame->fLInnerFrame);
      }

      // define elements

      // editbox
      frame->fEditVFrames = new TGVerticalFrame*[frame->fNumberOfEditBoxes];
      frame->fEditLabel = new TGLabel*[frame->fNumberOfEditBoxes];
      frame->fLEditLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
      frame->fEditBox = new TGTextEntry*[frame->fNumberOfEditBoxes];
      frame->fLEditBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, elementPad, elementPad, elementPad, elementPad);
      // define buttons
      frame->fButton = new TGTextButton*[frame->fNumberOfButtons];
      frame->fLButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsCenterX, elementPad, elementPad, elementPad, elementPad);
      // define comboboxes
      frame->fComboVFrames = new TGVerticalFrame*[frame->fNumberOfComboBoxes];
      frame->fComboLabel = new TGLabel*[frame->fNumberOfComboBoxes];
      frame->fLComboLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
      frame->fComboBox = new TGComboBox*[frame->fNumberOfComboBoxes];
      frame->fLComboBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, elementPad, elementPad, elementPad, elementPad);
      // checkbuttons
      frame->fCheckButtonVFrames = new TGVerticalFrame*[frame->fNumberOfCheckButtons];
      frame->fCheckButtonLabel = new TGLabel*[frame->fNumberOfCheckButtons];
      frame->fLCheckButtonLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
      frame->fCheckButton = new TGCheckButton*[frame->fNumberOfCheckButtons];
      frame->fLCheckButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);

      // create elements
      int nButton = 0;
      int nEditBox = 0;
      int nComboBox = 0;
      int nCheckButton = 0;
      int nDiv = 1;
      int nPart = 1;
      frame->fNumberOfHHFrames = 0;
      for (j=0;j<frame->fNumberOfElements;j++) {
         index = frame->fNumberOfHHFrames%frame->fNumberOfVFrames;
         // editbox
         if (frame->fNumberOfEditBoxes>0) {
            if (frame->fEditBoxElementIndex[nEditBox]==j) {
               nDiv = TMath::Min(maxWidth/(frame->fEditBoxWidth[nEditBox]+10),nDiv);
               if (nDiv>nPart && frame->fNumberOfElements>frame->fNumberOfVFrames) {
                  nPart++;
               }
               else {
                  frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
                  frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerFrame);
                  frame->fNumberOfHHFrames++;
                  nPart = 1;
                  nDiv = maxWidth/(frame->fEditBoxWidth[nEditBox]+10);
               }
               // vframe
               frame->fEditVFrames[nEditBox] = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
               frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->fEditVFrames[nEditBox], frame->fLInnerFrame);
               // label
               frame->fEditLabel[nEditBox] = new TGLabel(frame->fEditVFrames[nEditBox], frame->fEditLabelHotString[nEditBox]);
               frame->fEditVFrames[nEditBox]->AddFrame(frame->fEditLabel[nEditBox], frame->fLEditLabel);
               // edit
               frame->fEditBox[nEditBox] = new TGTextEntry(frame->fEditVFrames[nEditBox], frame->fEditBoxBuffer[nEditBox]);
               frame->fEditBox[nEditBox]->Associate(this);
               frame->fEditBox[nEditBox]->Resize(frame->fEditBoxWidth[nEditBox], 22);
               frame->fEditVFrames[nEditBox]->AddFrame(frame->fEditBox[nEditBox], frame->fLEditBox);
               if (fFirstEdit) {
   //               frame->fEditBox[nEditBox]->SelectAll();
                  frame->fEditBox[nEditBox]->SetFocus();
                  fFirstEdit = false;
               }
               nEditBox++;
            }
         }
         // buttons
         if (frame->fNumberOfButtons>0) {
            if (frame->fButtonElementIndex[nButton]==j) {
               nDiv = TMath::Min(maxWidth/(frame->fButtonWidth[nButton]+10),nDiv);
               if (nDiv>nPart && frame->fNumberOfElements>frame->fNumberOfVFrames) {
                  nPart++;
               }
               else {
                  if (frame->fNumberOfButtons!=frame->fNumberOfElements)
                     frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
                  else
                     frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,0);
                  frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerFrame);
                  frame->fNumberOfHHFrames++;
                  nPart = 1;
                  nDiv = maxWidth/(frame->fButtonWidth[nButton]+10);
               }
               if (frame->fButtonID[nButton]!=-1)
                  frame->fButton[nButton] = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], frame->fButtonHotString[nButton],frame->fButtonID[nButton]);
               else
                  frame->fButton[nButton] = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], frame->fButtonHotString[nButton]);
               frame->fButton[nButton]->Associate(this);
               frame->fButton[nButton]->Resize(frame->fButtonWidth[nButton], 22);
               frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->fButton[nButton], frame->fLButton);
               nButton++;
            }
         }
         // combobox
         if (frame->fNumberOfComboBoxes>0) {
            if (frame->fComboBoxElementIndex[nComboBox]==j) {
               nDiv = TMath::Min(maxWidth/(frame->fComboBoxWidth[nComboBox]+10),nDiv);
               if (nDiv>nPart && frame->fNumberOfElements>frame->fNumberOfVFrames) {
                  nPart++;
               }
               else {
                  frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
                  frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerFrame);
                  frame->fNumberOfHHFrames++;
                  nPart = 1;
                  nDiv = maxWidth/(frame->fComboBoxWidth[nComboBox]+10);
               }
               // vframe
               frame->fComboVFrames[nComboBox] = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
               frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->fComboVFrames[nComboBox], frame->fLInnerFrame);
               // label
               frame->fComboLabel[nComboBox] = new TGLabel(frame->fComboVFrames[nComboBox], frame->fComboLabelHotString[nComboBox]);
               frame->fComboVFrames[nComboBox]->AddFrame(frame->fComboLabel[nComboBox], frame->fLComboLabel);
               // edit
               frame->fComboBox[nComboBox] = new TGComboBox(frame->fComboVFrames[nComboBox]);
               frame->fComboBox[nComboBox]->Associate(this);
               frame->fComboBox[nComboBox]->Resize(0, 22);
               for (k=0;k<frame->fComboBoxEntries[nComboBox]->GetEntriesFast();k++) {
                  frame->fComboBox[nComboBox]->AddEntry(frame->fComboBoxEntries[nComboBox]->At(k).Data(),k);
               }
               frame->fComboBox[nComboBox]->Select(frame->fComboBoxSelectedEntry[nComboBox]);
               frame->fComboVFrames[nComboBox]->AddFrame(frame->fComboBox[nComboBox], frame->fLEditBox);
               nComboBox++;
            }
         }
         // checkbuttons
         if (frame->fNumberOfCheckButtons>0) {
            if (frame->fCheckButtonElementIndex[nCheckButton]==j) {
               nDiv = TMath::Min(maxWidth/(frame->fCheckButtonWidth[nCheckButton]+10),nDiv);
               if (nDiv>nPart && frame->fNumberOfElements>frame->fNumberOfVFrames) {
                  nPart++;
               }
               else {
                  frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight | kChildFrame);
                  frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerCheckButtonFrame);
                  frame->fNumberOfHHFrames++;
                  nPart = 1;
                  nDiv = maxWidth/(frame->fCheckButtonWidth[nCheckButton]+10);
               }
               // vframe
               frame->fCheckButtonVFrames[nCheckButton] = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
               frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->fCheckButtonVFrames[nCheckButton], frame->fLInnerFrame);
               // label
               frame->fCheckButtonLabel[nCheckButton] = new TGLabel(frame->fCheckButtonVFrames[nCheckButton], frame->fCheckButtonLabelHotString[nCheckButton]);
               frame->fCheckButtonVFrames[nCheckButton]->AddFrame(frame->fCheckButtonLabel[nCheckButton], frame->fLCheckButtonLabel);
               // button
               frame->fCheckButton[nCheckButton] = new TGCheckButton(frame->fCheckButtonVFrames[nCheckButton]);
               if (frame->fCheckButtonChecked[nCheckButton])
                  frame->fCheckButton[nCheckButton]->SetState(kButtonDown);
               frame->fCheckButton[nCheckButton]->Associate(this);
               frame->fCheckButtonVFrames[nCheckButton]->AddFrame(frame->fCheckButton[nCheckButton], frame->fLCheckButton);
               nCheckButton++;
            }
         }
      }
/*      index = frame->fNumberOfHHFrames%frame->fNumberOfVFrames;
      while (index>0) {
         frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
         frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerFrame);
         frame->fNumberOfHHFrames++;
         index = frame->fNumberOfHHFrames%frame->fNumberOfVFrames;
      }*/
   }

   // subframes

   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      if (frame->fSubFrames[i]->fVisible) {
         if (frame->fSubFrames[i]->fIsTab) {
            frame->fTab = new TGTab(frame->fFrame);
            frame->fFrame->AddFrame(frame->fTab,frame->fLFrame);
            break;
         }
      }
   }
   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      if (frame->fSubFrames[i]->fVisible) {
         // create subframe
         currentSubFrame = frame->fSubFrames[i];
         currentSubFrame->fIndex = i+1;
         currentSubFrame->fLFrame = new TGLayoutHints(kLHintsExpandX | kFixedHeight, framePad, framePad, framePad, framePad);
         currentSubFrame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, innerFramePad, innerFramePad, innerFramePad, innerFramePad);
         currentSubFrame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, innerFramePad, innerFramePad, innerFramePad, innerFramePad);
         if (currentSubFrame->fIsTab) {
            currentSubFrame->fFrame = frame->fTab->AddTab("");
         }
         else {
            if (currentSubFrame->fVerticalFrame)
               currentSubFrame->fFrame = new TGVerticalFrame(frame->fFrame,0,0,kRaisedFrame);
            else
               currentSubFrame->fFrame = new TGHorizontalFrame(frame->fFrame,0,0,kRaisedFrame);
            frame->fFrame->AddFrame(currentSubFrame->fFrame,currentSubFrame->fLFrame);
         }
         currentSubFrame->fParentFrame = frame;
      }
   }
}

void XMLToFormWindow::CreateFrame(XMLToFormFrame *frame)
{
   int i;

   // build up frame
   BuildFrame(frame);

   // create subframes
   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      if (frame->fSubFrames[i]->fVisible)
         CreateFrame(frame->fSubFrames[i]);
   }
}

void XMLToFormWindow::BuildForm(XMLToFormFrame *frame)
{
   // Frame
   if (frame->fVerticalFrame)
      frame->fFrame = new TGVerticalFrame(this,0,0,kRaisedFrame);
   else
      frame->fFrame = new TGHorizontalFrame(this,0,0,kRaisedFrame);
   frame->fLFrame = new TGLayoutHints(kLHintsExpandX | kFixedHeight, framePad, framePad, framePad, framePad);
   frame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, innerFramePad, innerFramePad, innerFramePad, innerFramePad);
   frame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, innerFramePad, innerFramePad, innerFramePad, innerFramePad);
   AddFrame(frame->fFrame,frame->fLFrame);
   frame->fIndex = 1;
   frame->fParentFrame = NULL;
   frame->fIsTab = false;
   frame->fTabIndex = -1;
}

bool XMLToFormWindow::CreateForm(XMLToFormFrame *frame)
{
   // build up main frame
   BuildForm(frame);

   CreateFrame(frame);

   return true;
}
XMLToFormWindow::XMLToFormWindow(const TGWindow * p, const TGWindow * main, XMLToFormFrame *frame,int *exitButtonID, int windowWidth):TGTransientFrame(p,main)
{
   fFirstEdit = true;
   fExitID = exitButtonID;
   fMaximalWindowWidth = windowWidth;

   CreateForm(frame);

   PlaceWindow(main);
   fClient->WaitFor(this);
}
XMLToFormWindow::~XMLToFormWindow()
{

}

void XMLToFormWindow::PlaceWindow(const TGWindow * main)
{
   MapSubwindows();
   Resize(fMaximalWindowWidth,GetDefaultSize().fHeight);

   SetWindowName("Please Edit Form");

   // position relative to the parent's window
   Int_t ax, ay;
   if (main) {
      Window_t wdum;
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                                      (Int_t) (((TGFrame *) main)->GetWidth() -
                                               fWidth) >> 1,
                                      (Int_t) (((TGFrame *) main)->GetHeight() -
                                               fHeight) >> 1, ax, ay, wdum);
   } else {
      UInt_t root_w, root_h;
      gVirtualX->GetWindowSize(fClient->GetRoot()->GetId(), ax, ay, root_w, root_h);
      ax = (root_w - fWidth) >> 1;
      ay = (root_h - fHeight) >> 1;
   }

   Move(ax, ay);
   SetWMPosition(ax, ay);

   SetMWMHints(kMWMDecorAll | kMWMDecorMaximize | kMWMDecorMenu,
               kMWMFuncAll | kMWMFuncMaximize | kMWMFuncResize, kMWMInputModeless);

   MapWindow();
}

Bool_t XMLToFormWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         *fExitID = parm1;
         CloseWindow();
         break;
      default:
         break;
      }
      break;

   case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {
      case kTE_TEXTCHANGED:
         break;
      case kTE_ENTER:
         CloseWindow();
         break;
      case kTE_TAB:
         gVirtualX->SetInputFocus(gVirtualX->GetInputFocus()+1);
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

