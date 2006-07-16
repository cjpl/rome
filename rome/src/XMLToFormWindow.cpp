//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// XMLToFormWindow                                                            //
//                                                                            //
// Window class for the XMLToForm class.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////
// $Id$

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGButton.h>
#include <TGTab.h>
#include <TGLayout.h>
#include <TGTextEntry.h>
#include <ROMELabel.h>
#include <TGFrame.h>
#include <TGListTree.h>
#include <TGListView.h>
#include <TGComboBox.h>
#include <TGCanvas.h>
#include "ROMEiostream.h"
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "XMLToForm.h"
#include "XMLToFormFrame.h"
#include "XMLToFormWindow.h"
#include "XMLToFormElementSignal.h"
#include "ROMEHorizontalFrame.h"

ClassImp(XMLToFormWindow)

void XMLToFormWindow::BuildFrame(XMLToFormFrame *frame)
{
   int j,k;
   int index,additionalWidth;
   ROMELabel *tempLabel;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;

   frame->SetFrameCreated(true);

   // create title
   if (frame->GetFrameTitle().Length()>0) {
      if (frame->IsFrameTab()) {
         frame->fTitleString = new TGHotString(frame->GetFrameTitle().Data());
         frame->fParentFrame->fTab->GetTabTab(frame->GetFrameTabIndex())->SetText(frame->fTitleString);
      }
      else if (!frame->IsFrameListTree() && !frame->IsFrameListTreeItem()) {
         frame->fTitleLabel = new ROMELabel(frame->fFrame, frame->GetFrameTitle().Data());
         // tool tip
         if (frame->GetFrameToolTip().Length()>0)
            frame->fTitleLabel->SetToolTipText(frame->GetFrameToolTip().Data(),0);
#if defined( R__VISUAL_CPLUSPLUS )
//            frame->fTitleLabel->SetTextFont("arial bold", kTRUE);
#else
//            frame->fTitleLabel->SetTextFont("-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1", kTRUE);
         /* available fonts under UNIX by default
            TGFont: -adobe-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1, prop, ref cnt = 2
            TGFont: -adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1, prop, ref cnt = 3
            TGFont: -adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1, prop, ref cnt = 1
            TGFont: -adobe-courier-medium-r-*-*-12-*-*-*-*-*-iso8859-1, fixed, ref cnt = 1
          */
#endif
      }
   }

   // create elements

   if (frame->GetNumberOfElements()>0) {
      // calculate number of vframes
      frame->SetFrameMaxWidth(0);
      // editboxes
      for (j=0;j<frame->GetNumberOfElements();j++) {
         if (frame->GetElementAt(j)->GetType()=="Button" || frame->GetElementAt(j)->GetType()=="ComboBox")
            additionalWidth = 8;
         else
            additionalWidth = 0;
         tempLabel = new ROMELabel(0,frame->GetElementAt(j)->GetTitle().Data());
         if (frame->GetElementAt(j)->GetWidth()<(int)tempLabel->GetSize().fWidth+additionalWidth)
            frame->GetElementAt(j)->SetWidth(tempLabel->GetSize().fWidth+additionalWidth);
         if (frame->GetFrameMaxWidth()<frame->GetElementAt(j)->GetWidth())
            frame->SetFrameMaxWidth(frame->GetElementAt(j)->GetWidth()+2*frame->elementPad);
         delete tempLabel;
         for (k=0;k<frame->GetElementAt(j)->GetNumberOfEntries();k++) {
            tempLabel = new ROMELabel(0,frame->GetElementAt(j)->GetEntryAt(k).Data());
            if (frame->GetElementAt(j)->GetWidth()<(int)tempLabel->GetSize().fWidth)
               frame->GetElementAt(j)->SetWidth(tempLabel->GetSize().fWidth);
            if (frame->GetFrameMaxWidth()<frame->GetElementAt(j)->GetWidth())
               frame->SetFrameMaxWidth(frame->GetElementAt(j)->GetWidth()+2*frame->elementPad);
            delete tempLabel;
         }
      }

      // create hframes
      frame->fHFrame = new TGHorizontalFrame(frame->fFrame,0,0);
      // create hhframes
      frame->fHHFrames = new TGHorizontalFrame*[2*frame->GetNumberOfElements()];
      frame->fHHFrameIndex = new Int_t[2*frame->GetNumberOfElements()];
      // create vframes
      if (frame->IsFrameListTreeItem())
         frame->fNumberOfVFrames = (fMaximalWindowWidth-fListTreeWidth+2*frame->framePad)/(frame->GetFrameMaxWidth());
      else
         frame->fNumberOfVFrames = (fMaximalWindowWidth+2*frame->framePad)/(frame->GetFrameMaxWidth());
      if (frame->fNumberOfVFrames<1)
         frame->fNumberOfVFrames = 1;
      if (frame->fNumberOfVFrames>frame->GetNumberOfElements())
         frame->fNumberOfVFrames=frame->GetNumberOfElements();
      frame->fVFrames = new TGVerticalFrame*[frame->fNumberOfVFrames];
      for (j=0;j<frame->fNumberOfVFrames;j++) {
         frame->fVFrames[j] = new TGVerticalFrame(frame->fHFrame,0,0);
      }

      // create elements
      int nDiv = 1;
      int nPart = 1;
      frame->fNumberOfHHFrames = 0;
      for (j=0;j<frame->GetNumberOfElements();j++) {
         index = frame->fNumberOfHHFrames%frame->fNumberOfVFrames;
         // editbox
         if (frame->GetElementAt(j)->GetType()=="EditBox") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = index;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10);
            }
            frame->GetElementAt(j)->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            frame->GetElementAt(j)->fLEditLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            frame->GetElementAt(j)->fLEditBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            frame->GetElementAt(j)->fEditVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            frame->GetElementAt(j)->fEditLabel = new ROMELabel(frame->GetElementAt(j)->fEditVFrames, frame->GetElementAt(j)->GetTitle().Data());
            // edit
            frame->GetElementAt(j)->fEditBoxBuffer = new TGTextBuffer(50);
            frame->GetElementAt(j)->fEditBoxBuffer->AddText(0, frame->GetElementAt(j)->GetValue().Data());
            frame->GetElementAt(j)->fEditBox = new TGTextEntry(frame->GetElementAt(j)->fEditVFrames, frame->GetElementAt(j)->fEditBoxBuffer);
            frame->GetElementAt(j)->fEditBox->Associate(this);
            frame->GetElementAt(j)->fEditBox->Resize(frame->GetElementAt(j)->GetWidth(), 22);
            if (fFirstEdit) {
//               frame->fEditBox[nEditBox]->SelectAll();
               frame->GetElementAt(j)->fEditBox->SetFocus();
               fFirstEdit = false;
            }
            // tool tip
            if (frame->GetElementAt(j)->GetToolTip().Length()>0)
               frame->GetElementAt(j)->fEditLabel->SetToolTipText(frame->GetElementAt(j)->GetToolTip().Data(),0);
         }
         // buttons
         if (frame->GetElementAt(j)->GetType()=="Button") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
//               if (frame->fNumberOfButtons!=frame->fNumberOfElements)
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = index;
//               else
//                  frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,0);
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10);
            }
            frame->GetElementAt(j)->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            frame->GetElementAt(j)->fLButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsCenterX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // button
            if (frame->GetElementAt(j)->GetButtonID()!=-1)
               frame->GetElementAt(j)->fButton = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], frame->GetElementAt(j)->GetTitle().Data(),frame->GetElementAt(j)->GetButtonID());
            else
               frame->GetElementAt(j)->fButton = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], frame->GetElementAt(j)->GetTitle().Data());
            frame->GetElementAt(j)->fButton->Associate(this);
            frame->GetElementAt(j)->fButton->Resize(frame->GetElementAt(j)->GetWidth(), 22);
            // tool tip
            if (frame->GetElementAt(j)->GetToolTip().Length()>0)
               frame->GetElementAt(j)->fButton->SetToolTipText(frame->GetElementAt(j)->GetToolTip().Data(),0);
         }
         // combobox
         if (frame->GetElementAt(j)->GetType()=="ComboBox") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = index;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10);
            }
            frame->GetElementAt(j)->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            frame->GetElementAt(j)->fLComboLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            frame->GetElementAt(j)->fLComboBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            frame->GetElementAt(j)->fComboVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            frame->GetElementAt(j)->fComboLabel = new ROMELabel(frame->GetElementAt(j)->fComboVFrames, frame->GetElementAt(j)->GetTitle().Data());
            // edit
            frame->GetElementAt(j)->fComboBox = new TGComboBox(frame->GetElementAt(j)->fComboVFrames);
            frame->GetElementAt(j)->fComboBox->Associate(this);
            frame->GetElementAt(j)->fComboBox->Resize(0, 22);
            for (k=0;k<frame->GetElementAt(j)->GetNumberOfEntries();k++) {
               frame->GetElementAt(j)->fComboBox->AddEntry(frame->GetElementAt(j)->GetEntryAt(k).Data(),k);
            }
            frame->GetElementAt(j)->fComboBox->Select(frame->GetElementAt(j)->GetSelectedEntry());
            // tool tip
            if (frame->GetElementAt(j)->GetToolTip().Length()>0)
               frame->GetElementAt(j)->fComboLabel->SetToolTipText(frame->GetElementAt(j)->GetToolTip().Data(),0);
         }
         // checkbuttons
         if (frame->GetElementAt(j)->GetType()=="CheckButton") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight | kChildFrame);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = index;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(frame->GetElementAt(j)->GetWidth()+10);
            }
            frame->GetElementAt(j)->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            frame->GetElementAt(j)->fLCheckButtonLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            frame->GetElementAt(j)->fLCheckButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            frame->GetElementAt(j)->fCheckButtonVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            frame->GetElementAt(j)->fCheckButtonLabel = new ROMELabel(frame->GetElementAt(j)->fCheckButtonVFrames, frame->GetElementAt(j)->GetTitle().Data());
            // button
            if (frame->GetElementAt(j)->GetButtonID()!=-1)
               frame->GetElementAt(j)->fCheckButton = new TGCheckButton(frame->GetElementAt(j)->fCheckButtonVFrames,"",frame->GetElementAt(j)->GetButtonID());
            else
               frame->GetElementAt(j)->fCheckButton = new TGCheckButton(frame->GetElementAt(j)->fCheckButtonVFrames);
            if (frame->GetElementAt(j)->GetButtonChecked())
               frame->GetElementAt(j)->fCheckButton->SetState(kButtonDown);
            if (frame->GetElementAt(j)->GetSignal()!=NULL)
               frame->GetElementAt(j)->fCheckButton->Connect(frame->GetElementAt(j)->GetSignal()->GetSignal().Data(),"XMLToFormWindow",this,"SignalHandler()");
            frame->GetElementAt(j)->fCheckButton->Associate(this);
            // tool tip
            if (frame->GetElementAt(j)->GetToolTip().Length()>0)
               frame->GetElementAt(j)->fCheckButtonLabel->SetToolTipText(frame->GetElementAt(j)->GetToolTip().Data(),0);
         }
      }
   }
}

void XMLToFormWindow::BuildSubFrames(XMLToFormFrame *frame)
{
   int i;
   XMLToFormFrame *currentSubFrame;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;

   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         if (frame->GetSubFrameAt(i)->IsFrameTab()) {
            frame->fTab = new TGTab(frame->fFrame);
            break;
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         // create subframe
         currentSubFrame = frame->GetSubFrameAt(i);
         currentSubFrame->fIndex = i+1;
         currentSubFrame->fLFrame = new TGLayoutHints(kLHintsExpandX | kFixedHeight, frame->framePad, frame->framePad, frame->framePad, frame->framePad);
         currentSubFrame->fLFrameExpand = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->framePad, frame->framePad, frame->framePad, frame->framePad);
         currentSubFrame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
         currentSubFrame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
         if (currentSubFrame->IsFrameListTree()) {
            currentSubFrame->fHFrame = new TGHorizontalFrame(frame->fFrame,0,0,kRaisedFrame);
            frame->fFrame->AddFrame(currentSubFrame->fHFrame,currentSubFrame->fLFrameExpand);
            currentSubFrame->fListTreeCanvas = new TGCanvas(currentSubFrame->fHFrame,fListTreeWidth,fListTreeHeight);
            currentSubFrame->fHFrame->AddFrame(currentSubFrame->fListTreeCanvas,new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
            currentSubFrame->fListTree = new TGListTree(currentSubFrame->fListTreeCanvas, kHorizontalFrame);
            currentSubFrame->fListTree->Associate(currentSubFrame->fHFrame);
            currentSubFrame->fListTreeItem = currentSubFrame->fListTree->AddItem(0,"Config");
            currentSubFrame->fListTree->OpenItem(currentSubFrame->fListTreeItem);
            currentSubFrame->fFrame = new ROMEHorizontalFrame(currentSubFrame->fHFrame,0,0,kRaisedFrame);
            currentSubFrame->fHFrame->AddFrame(currentSubFrame->fFrame,currentSubFrame->fLFrame);
            currentSubFrame->fListTree->Connect("Clicked(TGListTreeItem*,Int_t)", "XMLToFormWindow", this, "ListTreeClicked(TGListTreeItem*,Int_t)");
         }
         else if (currentSubFrame->IsFrameListTreeItem()) {
            currentSubFrame->fListTree = currentSubFrame->fParentFrame->fListTree;
            if (currentSubFrame->fParentFrame->fListTreeFrame==NULL) {
               currentSubFrame->fListTreeFrame = currentSubFrame->fParentFrame->fFrame;
               currentSubFrame->fListTreeHFrame = currentSubFrame->fParentFrame->fHFrame;
            }
            else {
               currentSubFrame->fListTreeFrame = currentSubFrame->fParentFrame->fListTreeFrame;
               currentSubFrame->fListTreeHFrame = currentSubFrame->fParentFrame->fListTreeHFrame;
            }
            currentSubFrame->fListTreeItem = currentSubFrame->fListTree->AddItem(currentSubFrame->fParentFrame->fListTreeItem,currentSubFrame->GetFrameTitle());
            currentSubFrame->fFrame = new ROMEHorizontalFrame(currentSubFrame->fListTreeFrame,0,0,kRaisedFrame);
            currentSubFrame->fListTreeFrame->AddFrame(currentSubFrame->fFrame,currentSubFrame->fLFrame);
         }
         else if (currentSubFrame->IsFrameTab()) {
            currentSubFrame->fFrame = frame->fTab->AddTab("");
         }
         else {
            if (currentSubFrame->IsFrameVertical())
               currentSubFrame->fFrame = new TGVerticalFrame(frame->fFrame,0,0,kRaisedFrame);
            else
               currentSubFrame->fFrame = new ROMEHorizontalFrame(frame->fFrame,0,0,kRaisedFrame);
         }
      }
   }
}
void XMLToFormWindow::AddFrame(XMLToFormFrame *frame)
{
   int j;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;

   // create title
   if (frame->GetFrameTitle().Length()>0) {
      if (!frame->IsFrameTab() && !frame->IsFrameListTree() && !frame->IsFrameListTreeItem()) {
         frame->fFrame->AddFrame(frame->fTitleLabel, frame->fLTitleLabel);
      }
   }

   // create elements

   if (frame->GetNumberOfElements()>0) {
      frame->fFrame->AddFrame(frame->fHFrame, frame->fLInnerFrame);
      for (j=0;j<frame->fNumberOfVFrames;j++) {
         frame->fHFrame->AddFrame(frame->fVFrames[j], frame->fLInnerFrame);
      }
      for (j=0;j<frame->fNumberOfHHFrames;j++) {
         frame->fVFrames[frame->fHHFrameIndex[j]]->AddFrame(frame->fHHFrames[j], frame->fLInnerFrame);
      }

      for (j=0;j<frame->GetNumberOfElements();j++) {
         // editbox
         if (frame->GetElementAt(j)->GetType()=="EditBox") {
            // vframe
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->AddFrame(frame->GetElementAt(j)->fEditVFrames, frame->fLInnerFrame);
            // label
            frame->GetElementAt(j)->fEditVFrames->AddFrame(frame->GetElementAt(j)->fEditLabel, frame->GetElementAt(j)->fLEditLabel);
            // edit
            frame->GetElementAt(j)->fEditVFrames->AddFrame(frame->GetElementAt(j)->fEditBox, frame->GetElementAt(j)->fLEditBox);
         }
         // buttons
         if (frame->GetElementAt(j)->GetType()=="Button") {
            // button
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->AddFrame(frame->GetElementAt(j)->fButton, frame->GetElementAt(j)->fLButton);
         }
         // combobox
         if (frame->GetElementAt(j)->GetType()=="ComboBox") {
            // vframe
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->AddFrame(frame->GetElementAt(j)->fComboVFrames, frame->fLInnerFrame);
            // label
            frame->GetElementAt(j)->fComboVFrames->AddFrame(frame->GetElementAt(j)->fComboLabel, frame->GetElementAt(j)->fLComboLabel);
            // edit
            frame->GetElementAt(j)->fComboVFrames->AddFrame(frame->GetElementAt(j)->fComboBox, frame->GetElementAt(j)->fLComboBox);
         }
         // checkbuttons
         if (frame->GetElementAt(j)->GetType()=="CheckButton") {
            // vframe
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->AddFrame(frame->GetElementAt(j)->fCheckButtonVFrames, frame->fLInnerFrame);
            // label
            frame->GetElementAt(j)->fCheckButtonVFrames->AddFrame(frame->GetElementAt(j)->fCheckButtonLabel, frame->GetElementAt(j)->fLCheckButtonLabel);
            // button
            frame->GetElementAt(j)->fCheckButtonVFrames->AddFrame(frame->GetElementAt(j)->fCheckButton, frame->GetElementAt(j)->fLCheckButton);
         }
      }
   }
}

void XMLToFormWindow::AddSubFrames(XMLToFormFrame *frame)
{
   int i;
   XMLToFormFrame *currentSubFrame;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;

   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         if (frame->GetSubFrameAt(i)->IsFrameTab()) {
            frame->fFrame->AddFrame(frame->fTab,frame->fLFrame);
            break;
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         // create subframe
         currentSubFrame = frame->GetSubFrameAt(i);
         if (!currentSubFrame->IsFrameTab() && !currentSubFrame->IsFrameListTree() && !currentSubFrame->IsFrameListTreeItem()) {
            frame->fFrame->AddFrame(currentSubFrame->fFrame,currentSubFrame->fLFrame);
         }
      }
   }
}

void XMLToFormWindow::RemoveFrame(XMLToFormFrame *frame)
{
   int j;

   // create title
   if (frame->GetFrameTitle().Length()>0) {
      if (!frame->IsFrameTab() && !frame->IsFrameListTree() && !frame->IsFrameListTreeItem()) {
         frame->fFrame->RemoveFrame(frame->fTitleLabel);
      }
   }

   // create elements

   if (frame->GetNumberOfElements()>0) {
      for (j=0;j<frame->GetNumberOfElements();j++) {
         // editbox
         if (frame->GetElementAt(j)->GetType()=="EditBox") {
            // vframe
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->RemoveFrame(frame->GetElementAt(j)->fEditVFrames);
            // label
            frame->GetElementAt(j)->fEditVFrames->RemoveFrame(frame->GetElementAt(j)->fEditLabel);
            // edit
            frame->GetElementAt(j)->fEditVFrames->RemoveFrame(frame->GetElementAt(j)->fEditBox);
         }
         // buttons
         if (frame->GetElementAt(j)->GetType()=="Button") {
            // button
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->RemoveFrame(frame->GetElementAt(j)->fButton);
         }
         // combobox
         if (frame->GetElementAt(j)->GetType()=="ComboBox") {
            // vframe
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->RemoveFrame(frame->GetElementAt(j)->fComboVFrames);
            // label
            frame->GetElementAt(j)->fComboVFrames->RemoveFrame(frame->GetElementAt(j)->fComboLabel);
            // edit
            frame->GetElementAt(j)->fComboVFrames->RemoveFrame(frame->GetElementAt(j)->fComboBox);
         }
         // checkbuttons
         if (frame->GetElementAt(j)->GetType()=="CheckButton") {
            // vframe
            frame->fHHFrames[frame->GetElementAt(j)->GetParentFrameIndex()]->RemoveFrame(frame->GetElementAt(j)->fCheckButtonVFrames);
            // label
            frame->GetElementAt(j)->fCheckButtonVFrames->RemoveFrame(frame->GetElementAt(j)->fCheckButtonLabel);
            // button
            frame->GetElementAt(j)->fCheckButtonVFrames->RemoveFrame(frame->GetElementAt(j)->fCheckButton);
         }
      }
      for (j=0;j<frame->fNumberOfHHFrames;j++) {
         frame->fVFrames[frame->fHHFrameIndex[j]]->RemoveFrame(frame->fHHFrames[j]);
      }
      for (j=0;j<frame->fNumberOfVFrames;j++) {
         frame->fHFrame->RemoveFrame(frame->fVFrames[j]);
      }
      frame->fFrame->RemoveFrame(frame->fHFrame);
   }
}


void XMLToFormWindow::CreateFrame(XMLToFormFrame *frame)
{
   int i;

   // build up frame
   BuildFrame(frame);
   AddFrame(frame);
   if (frame->IsFrameListTreeItem()) {
      frame->fListTreeFrame->HideFrame(frame->fFrame);
   }
   BuildSubFrames(frame);
   AddSubFrames(frame);

   // create subframes
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         if (frame->GetSubFrameAt(i)->IsFrameListTree()) {
            CreateFrame(frame->GetSubFrameAt(i));
         }
         else if (frame->GetSubFrameAt(i)->IsFrameListTreeItem()) {
            CreateFrame(frame->GetSubFrameAt(i));
         }
         else {
            CreateFrame(frame->GetSubFrameAt(i));
         }
      }
   }
}

void XMLToFormWindow::HideFrame(XMLToFormFrame *frame)
{
   int i;

   if (frame->IsFrameListTreeItem()) {
      frame->fListTreeFrame->HideFrame(frame->fFrame);
   }
   if (frame->IsFrameListTree()) {
      frame->fHFrame->HideFrame(frame->fFrame);
   }

   // hide subframes
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         HideFrame(frame->GetSubFrameAt(i));
      }
   }
   if (frame->IsFrameListTree()) {
      frame->fHFrame->ShowFrame(frame->fFrame);
   }
}

void XMLToFormWindow::BuildForm(XMLToFormFrame *frame)
{
   // Frame
   if (frame->IsFrameVertical())
      frame->fFrame = new TGVerticalFrame(this,0,0,kRaisedFrame);
   else
      frame->fFrame = new ROMEHorizontalFrame(this,0,0,kRaisedFrame);
   frame->fLFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->framePad, frame->framePad, frame->framePad, frame->framePad);
   frame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
   frame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
   AddFrame(frame->fFrame,frame->fLFrame);
   frame->fIndex = 1;
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
   fListTreeWidth = 200;
   fListTreeHeight = 200;
   fMainFrame = frame;
   fMainWindow = main;
   fTreeListActiveFrame = NULL;

   CreateForm(fMainFrame);

   PlaceWindow(main);
   SetWindowName("Please wait while the mask is building up ...");
   HideFrame(fMainFrame);
   SetWindowName("Please edit mask");
   fClient->WaitFor(this);
}

XMLToFormWindow::~XMLToFormWindow()
{
}

void XMLToFormWindow::PlaceWindow(const TGWindow * main)
{
   MapSubwindows();
   Resize(fMaximalWindowWidth,GetDefaultSize().fHeight);

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

Bool_t XMLToFormWindow::ListTreeClicked(TGListTreeItem* item,Int_t /*btn*/) {
   XMLToFormFrame *frame = SearchFrame(fMainFrame,item->GetText(),"");
   if (frame==NULL)
      return true;
   if (!frame->IsFrameListTreeItem())
      return true;

   if (fTreeListActiveFrame!=NULL) {
      fTreeListActiveFrame->fListTreeFrame->HideFrame(fTreeListActiveFrame->fFrame);
   }
   frame->fListTreeFrame->ShowFrame(frame->fFrame);
   frame->fListTreeHFrame->ShowFrame(frame->fListTreeFrame);

   fTreeListActiveFrame = frame;
   return true;
}

Bool_t XMLToFormWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t /*parm2*/)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         *fExitID = parm1;
         SaveCurrentValues(fMainFrame);
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
         SaveCurrentValues(fMainFrame);
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

void XMLToFormWindow::SaveCurrentValues(XMLToFormFrame *frame)
{
   int i;
   for (i=0;i<frame->GetNumberOfElements();i++) {
      if (frame->GetElementAt(i)->GetType()=="EditBox") {
         frame->GetElementAt(i)->SetValue(frame->GetElementAt(i)->fEditBoxBuffer->GetString());
      }
      if (frame->GetElementAt(i)->GetType()=="ComboBox") {
         frame->GetElementAt(i)->SetValue(frame->GetElementAt(i)->fComboBox->GetSelectedEntry()->GetTitle());
         frame->GetElementAt(i)->SetSelectedEntry(frame->GetElementAt(i)->fComboBox->GetSelected());
      }
      if (frame->GetElementAt(i)->GetType()=="CheckButton") {
         if (frame->GetElementAt(i)->fCheckButton->GetState()==kButtonDown) {
            frame->GetElementAt(i)->SetValue("true");
            frame->GetElementAt(i)->SetButtonChecked(true);
         }
         else {
            frame->GetElementAt(i)->SetValue("false");
            frame->GetElementAt(i)->SetButtonChecked(false);
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible())
         SaveCurrentValues(frame->GetSubFrameAt(i));
   }
}

XMLToFormFrame* XMLToFormWindow::SearchFrame(XMLToFormFrame *frame,const char* title,const char* label)
{
   XMLToFormFrame *returnFrame;
   int i;
   if (title!=NULL) {
      if (!strcmp(frame->GetFrameTitle().Data(),title))
         return frame;
   }
   if (label!=NULL) {
      for (i=0;i<frame->GetNumberOfElements();i++) {
         if (!strcmp(frame->GetElementAt(i)->GetTitle().Data(),label))
            return frame;
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      returnFrame = SearchFrame(frame->GetSubFrameAt(i),title,label);
      if (returnFrame!=NULL)
         return returnFrame;
   }
   return NULL;
}

bool XMLToFormWindow::SearchWidget(const char* path,XMLToFormFrame** frame,int *index,XMLToFormFrame* mainFrame)
{
   int i,ind;
   ROMEString editName = path;
   ROMEString tempString = path;
   int numberOfFrames = editName.NumberOfOccurrence("/");
   ROMEString **frameNames = new ROMEString*[numberOfFrames];
   // decode
   for (i=0;i<numberOfFrames;i++) {
      ind = editName.Index("/");
      tempString = editName(0,ind);
      frameNames[i] = new ROMEString(tempString.Data());
      editName = editName(ind+1,editName.Length()-ind-1);
   }
   // search frame
   *frame = mainFrame;
   for (i=0;i<numberOfFrames;i++) {
      *frame = SearchFrame(*frame,frameNames[i]->Data(),NULL);
      if (*frame==NULL)
         return false;
   }
   delete [] frameNames;
   // search value
   *frame = SearchFrame(*frame,NULL,editName.Data());
   if (*frame!=NULL) {
      for (i=0;i<(*frame)->GetNumberOfElements();i++) {
         if (!strcmp((*frame)->GetElementAt(i)->GetTitle().Data(),editName.Data())) {
            *index = i;
            return true;
         }
      }
   }
   return true;
}
void XMLToFormWindow::SignalHandler()
{
   CheckSignals(fMainFrame);
}
void XMLToFormWindow::CheckSignals(XMLToFormFrame *frame)
{
   bool senderState = false;
   XMLToFormFrame *recvFrame;
   int i,j;
   for (i=0;i<frame->GetNumberOfElements();i++) {
      if (frame->GetElementAt(i)->GetSignal()!=NULL) {
         if (frame->GetElementAt(i)->GetType()=="CheckButton") {
            if (frame->GetElementAt(i)->fCheckButton->GetState()==kButtonDown)
               senderState = true;
            else
               senderState = false;
         }
         for (j=0;j<frame->GetElementAt(i)->GetSignal()->GetNumberOfReceivers();j++) {
            recvFrame = SearchFrame(fMainFrame,frame->GetElementAt(i)->GetSignal()->GetReceiverPathAt(j).Data(),NULL);
            if (recvFrame==NULL)
               return;
            if (frame->GetElementAt(i)->GetSignal()->GetReceiverTypeAt(j)=="Enable") {
               if (recvFrame->IsFrameTab()) {
                  if (senderState != recvFrame->fParentFrame->fTab->GetTabTab(recvFrame->GetFrameTabIndex())->IsEnabled()) {
                     recvFrame->fParentFrame->fTab->GetTabTab(recvFrame->GetFrameTabIndex())->SetEnabled(senderState);
                     recvFrame->fParentFrame->fTab->GetTabTab(recvFrame->GetFrameTabIndex())->SetEnabled(senderState);
                     recvFrame->fParentFrame->fTab->HideFrame(recvFrame->fParentFrame->fTab->GetTabTab(recvFrame->GetFrameTabIndex()));
                     recvFrame->fParentFrame->fTab->ShowFrame(recvFrame->fParentFrame->fTab->GetTabTab(recvFrame->GetFrameTabIndex()));
                  }
               }
            }
            else {
            }
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      CheckSignals(frame->GetSubFrameAt(i));
   }
}
void XMLToFormWindow::DeleteFrame(XMLToFormFrame *frame)
{
   Int_t i;
   // Title
   if (frame->GetFrameTitle().Length()>0) {
      if (!frame->IsFrameTab() && !frame->IsFrameListTree() && !frame->IsFrameListTreeItem()) {
         delete frame->fTitleLabel;
      }
   }
   // EditBox
   for (i=0;i<frame->GetNumberOfElements();i++) {
      if (frame->GetElementAt(i)->GetType()=="EditBox") {
//         delete frame->GetElementAt(i)->fEditBox;
         delete frame->GetElementAt(i)->fEditVFrames;
         delete frame->GetElementAt(i)->fEditLabel;
      }
      if (frame->GetElementAt(i)->GetType()=="Button") {
         delete frame->GetElementAt(i)->fButton;
      }
      if (frame->GetElementAt(i)->GetType()=="ComboBox") {
         delete frame->GetElementAt(i)->fComboLabel;
         delete frame->GetElementAt(i)->fComboBox;
         delete frame->GetElementAt(i)->fComboVFrames;
      }
      if (frame->GetElementAt(i)->GetType()=="CheckButton") {
         delete frame->GetElementAt(i)->fCheckButtonLabel;
         delete frame->GetElementAt(i)->fCheckButton;
         delete frame->GetElementAt(i)->fCheckButtonVFrames;
      }
      if (frame->GetElementAt(i)->GetSignal()!=NULL) {
         delete frame->GetElementAt(i)->GetSignal();
      }
      delete frame->GetElementAt(i);
   }
   // Frames
   for (i=0;i<frame->fNumberOfHHFrames;i++) {
      delete frame->fHHFrames[i];
   }
   for (i=0;i<frame->fNumberOfVFrames;i++) {
      delete frame->fVFrames[i];
   }
   if (frame->GetNumberOfElements()>0) {
      delete [] frame->fHHFrames;
      delete frame->fHHFrameIndex;
      delete [] frame->fVFrames;
      delete frame->fHFrame;
   }
   // SubFrames
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      DeleteFrame(frame->GetSubFrameAt(i));
   }
   // Frame
   delete frame;
}

