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
#include <TError.h>
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
#include "ROMEXML.h"
#include "XMLToForm.h"
#include "XMLToFormFrame.h"
#include "XMLToFormWindow.h"
#include "XMLToFormElementSignal.h"
#include "ROMECompositeFrame.h"

ClassImp(XMLToFormWindow)

//______________________________________________________________________________
XMLToFormWindow::XMLToFormWindow()
:TGTransientFrame()
,fMainWindow(0)
,fExitID(0)
,fFirstEdit(kTRUE)
,fMaximalWindowWidth(1000)
,fListTreeWidth(300)
,fListTreeHeight(300)
,fMainFrame(0)
,fTreeListActiveFrame(0)
,fXML(0)
{
}

//______________________________________________________________________________
XMLToFormWindow::XMLToFormWindow(const TGWindow * p, const TGWindow * main, XMLToFormFrame *frame,
                                 int *exitButtonID, int windowWidth,ROMEXML *xml)
:TGTransientFrame(p, main)
,fMainWindow(main)
,fExitID(exitButtonID)
,fFirstEdit(kTRUE)
,fMaximalWindowWidth(windowWidth)
,fListTreeWidth(300)
,fListTreeHeight(300)
,fMainFrame(frame)
,fTreeListActiveFrame(0)
,fXML(xml)
{
   // build up main frame
   BuildForm(fMainFrame);

   CreateFrame(fMainFrame);

   PlaceWindow(main);
   SetWindowName("Please wait while the mask is building up ...");
   HideFrame(fMainFrame);
   SetWindowName("Please edit mask");
   fClient->WaitFor(this);
}

//______________________________________________________________________________
XMLToFormWindow::~XMLToFormWindow()
{
}

//______________________________________________________________________________
void XMLToFormWindow::BuildFrame(XMLToFormFrame *frame)
{
   int j,k;
   int indx,additionalWidth;
   ROMELabel *tempLabel;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;
   XMLToFormElement *element;

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
         if (frame->GetFrameToolTip().Length()>0) {
            frame->fTitleLabel->SetToolTipText(frame->GetFrameToolTip().Data(),0);
         }
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
      // calculate width
      for (j=0;j<frame->GetNumberOfElements();j++) {
         element = frame->GetElementAt(j);
         if (element->GetType()=="Button" || element->GetType()=="ComboBox") {
            additionalWidth = 8;
         } else {
            additionalWidth = 0;
         }
         tempLabel = new ROMELabel(0, element->GetTitle());
         if (element->GetWidth() < static_cast<int>(tempLabel->GetSize().fWidth) + additionalWidth) {
            element->SetWidth(tempLabel->GetSize().fWidth+additionalWidth);
         }
         if (frame->GetFrameMaxWidth()<element->GetWidth()) {
            frame->SetFrameMaxWidth(element->GetWidth()+2*frame->elementPad);
         }
         delete tempLabel;
         for (k=0;k<element->GetNumberOfEntries();k++) {
            tempLabel = new ROMELabel(0,element->GetEntryAt(k).Data());
            if (element->GetWidth() < static_cast<int>(tempLabel->GetSize().fWidth)) {
               element->SetWidth(tempLabel->GetSize().fWidth);
            }
            if (frame->GetFrameMaxWidth()<element->GetWidth()) {
               frame->SetFrameMaxWidth(element->GetWidth()+2*frame->elementPad);
            }
            delete tempLabel;
         }
      }

      // create hframes
      frame->fHFrame = new TGHorizontalFrame(frame->fFrame,0,0);
      // create hhframes
      frame->fHHFrames = new TGHorizontalFrame*[2*frame->GetNumberOfElements()];
      frame->fHHFrameIndex = new Int_t[2*frame->GetNumberOfElements()];
      // create vframes
      if (frame->IsFrameListTreeItem()) {
         frame->fNumberOfVFrames = (fMaximalWindowWidth-fListTreeWidth+2*frame->framePad)/(frame->GetFrameMaxWidth());
      } else {
         frame->fNumberOfVFrames = (fMaximalWindowWidth+2*frame->framePad)/(frame->GetFrameMaxWidth());
      }
      if (frame->fNumberOfVFrames<1) {
         frame->fNumberOfVFrames = 1;
      }
      if (frame->fNumberOfVFrames>frame->GetNumberOfElements()) {
         frame->fNumberOfVFrames = frame->GetNumberOfElements();
      }
      frame->fVFrames = new TGVerticalFrame*[frame->fNumberOfVFrames];
      for (j=0;j<frame->fNumberOfVFrames;j++) {
         frame->fVFrames[j] = new TGVerticalFrame(frame->fHFrame,0,0);
      }

      // create elements
      int nDiv = 1;
      int nPart = 1;
      frame->fNumberOfHHFrames = 0;
      for (j=0;j<frame->GetNumberOfElements();j++) {
         element = frame->GetElementAt(j);
         indx = frame->fNumberOfHHFrames%frame->fNumberOfVFrames;
         // editbox
         if (element->GetType()=="EditBox") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(element->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[indx],0,58,kFixedHeight);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = indx;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(element->GetWidth()+10);
            }
            element->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            element->fLEditLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            element->fLEditBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            element->fEditVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            element->fEditLabel = new ROMELabel(element->fEditVFrames, element->GetTitle());
            // edit
            element->fEditBoxBuffer = new TGTextBuffer(50);
            element->fEditBoxBuffer->AddText(0, element->GetValue().Data());
            element->fEditBox = new TGTextEntry(element->fEditVFrames, element->fEditBoxBuffer);
            element->fEditBox->Associate(this);
            element->fEditBox->Resize(element->GetWidth(), 22);
            if (fFirstEdit) {
//               frame->fEditBox[nEditBox]->SelectAll();
               element->fEditBox->SetFocus();
               fFirstEdit = false;
            }
            // tool tip
            if (element->GetToolTip().Length()>0) {
               element->fEditLabel->SetToolTipText(element->GetToolTip().Data(),0);
            }
         }
         // buttons
         if (element->GetType()=="Button") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(element->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
//               if (frame->fNumberOfButtons!=frame->fNumberOfElements)
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[indx],0,58,kFixedHeight);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = indx;
//               else
//                  frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[indx],0,0);
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(element->GetWidth()+10);
            }
            element->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            element->fLButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsCenterX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // button
            if (element->GetButtonID()!=-1) {
               element->fButton = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], element->GetTitle(),element->GetButtonID());
            } else {
               element->fButton = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], element->GetTitle());
            }
            element->fButton->Associate(this);
            element->fButton->Resize(element->GetWidth(), 22);
            // tool tip
            if (element->GetToolTip().Length()>0) {
               element->fButton->SetToolTipText(element->GetToolTip().Data(),0);
            }
         }
         // combobox
         if (element->GetType()=="ComboBox") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(element->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[indx],0,58,kFixedHeight);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = indx;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(element->GetWidth()+10);
            }
            element->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            element->fLComboLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            element->fLComboBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            element->fComboVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            element->fComboLabel = new ROMELabel(element->fComboVFrames, element->GetTitle());
            // edit
            element->fComboBox = new TGComboBox(element->fComboVFrames);
            element->fComboBox->Associate(this);
            element->fComboBox->Resize(0, 22);
            for (k=0;k<element->GetNumberOfEntries();k++) {
               element->fComboBox->AddEntry(element->GetEntryAt(k).Data(),k);
            }
            element->fComboBox->Select(element->GetSelectedEntry());
            // tool tip
            if (element->GetToolTip().Length()>0) {
               element->fComboLabel->SetToolTipText(element->GetToolTip().Data(),0);
            }
         }
         // checkbuttons
         if (element->GetType()=="CheckButton") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(element->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[indx],0,58,kFixedHeight | kChildFrame);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = indx;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(element->GetWidth()+10);
            }
            element->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            element->fLCheckButtonLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            element->fLCheckButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            element->fCheckButtonVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            element->fCheckButtonLabel = new ROMELabel(element->fCheckButtonVFrames, element->GetTitle());
            // button
            if (element->GetButtonID()!=-1)
               element->fCheckButton = new TGCheckButton(element->fCheckButtonVFrames,"",element->GetButtonID());
            else
               element->fCheckButton = new TGCheckButton(element->fCheckButtonVFrames);
            if (element->GetButtonChecked())
               element->fCheckButton->SetState(kButtonDown);
            if (element->GetSignal()!=NULL)
               element->fCheckButton->Connect(element->GetSignal()->GetSignal().Data(),"XMLToFormWindow",this,"SignalHandler()");
            element->fCheckButton->Associate(this);
            // tool tip
            if (element->GetToolTip().Length()>0)
               element->fCheckButtonLabel->SetToolTipText(element->GetToolTip().Data(),0);
         }
         // radiobuttons
         if (element->GetType()=="RadioButton") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(element->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[indx],0,58,kFixedHeight | kChildFrame);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = indx;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(element->GetWidth()+10);
            }
            element->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            element->fLRadioButtonLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            element->fLRadioButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            element->fRadioButtonVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            element->fRadioButtonLabel = new ROMELabel(element->fRadioButtonVFrames, element->GetTitle());
            // button
            if (element->GetButtonID()!=-1) {
               element->fRadioButton = new TGRadioButton(element->fRadioButtonVFrames,"",element->GetButtonID());
            } else {
               element->fRadioButton = new TGRadioButton(element->fRadioButtonVFrames);
            }
            if (element->GetButtonChecked()) {
               element->fRadioButton->SetState(kButtonDown);
            }
            if (element->GetSignal()!=NULL) {
               element->fRadioButton->Connect(element->GetSignal()->GetSignal().Data(),"XMLToFormWindow",this,"SignalHandler()");
            }
            element->fRadioButton->Associate(this);
            // tool tip
            if (element->GetToolTip().Length()>0)
               element->fRadioButtonLabel->SetToolTipText(element->GetToolTip().Data(),0);
         }
         // fileselector
         if (element->GetType()=="FileSelector") {
            nDiv = TMath::Min(frame->GetFrameMaxWidth()/(element->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[indx],0,58,kFixedHeight);
               frame->fHHFrameIndex[frame->fNumberOfHHFrames] = indx;
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = frame->GetFrameMaxWidth()/(element->GetWidth()+10);
            }
            element->SetParentFrameIndex(frame->fNumberOfHHFrames-1);
            // hints
            element->fLComboLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            element->fLComboBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            element->fComboVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            // label
            element->fComboLabel = new ROMELabel(element->fComboVFrames, element->GetTitle());
            // edit
            element->fComboBox = new TGComboBox(element->fComboVFrames,-2);
            element->fComboBox->Associate(this);
            element->fComboBox->Resize(0, 22);
            for (k=0;k<element->GetNumberOfEntries();k++) {
               element->fComboBox->AddEntry(element->GetEntryAt(k).Data(),k);
            }
            element->fComboBox->Select(element->GetSelectedEntry());
            // tool tip
            if (element->GetToolTip().Length()>0) {
               element->fComboLabel->SetToolTipText(element->GetToolTip().Data(),0);
            }
         }
      }
   }
}

//______________________________________________________________________________
void XMLToFormWindow::BuildSubFrames(XMLToFormFrame *frame)
{
   int i;
   XMLToFormFrame *currentSubFrame;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;

   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      currentSubFrame = frame->GetSubFrameAt(i);
      if (currentSubFrame->IsFrameVisible()) {
         if (currentSubFrame->IsFrameTab()) {
            frame->fTab = new TGTab(frame->fFrame);
            break;
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      currentSubFrame = frame->GetSubFrameAt(i);
      if (currentSubFrame->IsFrameVisible()) {
         // create subframe
         currentSubFrame->fIndex = i+1;
         currentSubFrame->fLFrame = new TGLayoutHints(kLHintsExpandX | kFixedHeight, frame->framePad, frame->framePad, frame->framePad, frame->framePad);
         currentSubFrame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
         currentSubFrame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
         currentSubFrame->fLInnerRadioButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
         if (currentSubFrame->IsFrameListTree()) {
            currentSubFrame->fHFrame = new TGHorizontalFrame(frame->fFrame,0,0,kRaisedFrame);
            frame->fFrame->AddFrame(currentSubFrame->fHFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->framePad, frame->framePad, frame->framePad, frame->framePad));
            currentSubFrame->fListTreeCanvas = new TGCanvas(currentSubFrame->fHFrame,fListTreeWidth,fListTreeHeight);
            currentSubFrame->fHFrame->AddFrame(currentSubFrame->fListTreeCanvas,new TGLayoutHints(kLHintsLeft | kLHintsExpandY));
            currentSubFrame->fListTree = new TGListTree(currentSubFrame->fListTreeCanvas, kHorizontalFrame);
            currentSubFrame->fListTree->Associate(currentSubFrame->fHFrame);
            currentSubFrame->fListTreeItem = currentSubFrame->fListTree->AddItem(0,"Config");
            currentSubFrame->fListTree->OpenItem(currentSubFrame->fListTreeItem);
            currentSubFrame->fFrame = new ROMECompositeFrame(currentSubFrame->fHFrame,0,0,kRaisedFrame | kHorizontalFrame);
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
            currentSubFrame->fFrame = new ROMECompositeFrame(currentSubFrame->fListTreeFrame,0,0,kRaisedFrame | kHorizontalFrame);
            currentSubFrame->fListTreeFrame->AddFrame(currentSubFrame->fFrame,currentSubFrame->fLFrame);
         }
         else if (currentSubFrame->IsFrameTab()) {
            currentSubFrame->fFrame = frame->fTab->AddTab("");
         }
         else {
            if (currentSubFrame->IsFrameVertical()) {
               currentSubFrame->fFrame = new TGVerticalFrame(frame->fFrame,0,0,kRaisedFrame);
            } else {
               currentSubFrame->fFrame = new ROMECompositeFrame(frame->fFrame,0,0,kRaisedFrame | kHorizontalFrame);
            }
         }
      }
   }
}

//______________________________________________________________________________
void XMLToFormWindow::AddFrame(XMLToFormFrame *frame)
{
   int j;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;
   XMLToFormElement *element;

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
         element = frame->GetElementAt(j);
         // editbox
         if (element->GetType()=="EditBox") {
            // vframe
            frame->fHHFrames[element->GetParentFrameIndex()]->AddFrame(element->fEditVFrames, frame->fLInnerFrame);
            // label
            element->fEditVFrames->AddFrame(element->fEditLabel, element->fLEditLabel);
            // edit
            element->fEditVFrames->AddFrame(element->fEditBox, element->fLEditBox);
         }
         // buttons
         if (element->GetType()=="Button") {
            // button
            frame->fHHFrames[element->GetParentFrameIndex()]->AddFrame(element->fButton, element->fLButton);
         }
         // combobox
         if (element->GetType()=="ComboBox") {
            // vframe
            frame->fHHFrames[element->GetParentFrameIndex()]->AddFrame(element->fComboVFrames, frame->fLInnerFrame);
            // label
            element->fComboVFrames->AddFrame(element->fComboLabel, element->fLComboLabel);
            // edit
            element->fComboVFrames->AddFrame(element->fComboBox, element->fLComboBox);
         }
         // checkbuttons
         if (element->GetType()=="CheckButton") {
            // vframe
            frame->fHHFrames[element->GetParentFrameIndex()]->AddFrame(element->fCheckButtonVFrames, frame->fLInnerFrame);
            // label
            element->fCheckButtonVFrames->AddFrame(element->fCheckButtonLabel, element->fLCheckButtonLabel);
            // button
            element->fCheckButtonVFrames->AddFrame(element->fCheckButton, element->fLCheckButton);
         }
         // radiobuttons
         if (element->GetType()=="RadioButton") {
            // vframe
            frame->fHHFrames[element->GetParentFrameIndex()]->AddFrame(element->fRadioButtonVFrames, frame->fLInnerFrame);
            // label
            element->fRadioButtonVFrames->AddFrame(element->fRadioButtonLabel, element->fLRadioButtonLabel);
            // button
            element->fRadioButtonVFrames->AddFrame(element->fRadioButton, element->fLRadioButton);
         }
         // fileselector
         if (element->GetType()=="FileSelector") {
            // vframe
            frame->fHHFrames[element->GetParentFrameIndex()]->AddFrame(element->fComboVFrames, frame->fLInnerFrame);
            // label
            element->fComboVFrames->AddFrame(element->fComboLabel, element->fLComboLabel);
            // edit
            element->fComboVFrames->AddFrame(element->fComboBox, element->fLComboBox);
         }
      }
   }
}

//______________________________________________________________________________
void XMLToFormWindow::AddSubFrames(XMLToFormFrame *frame)
{
   int i;
   XMLToFormFrame *currentSubFrame;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;

   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      currentSubFrame = frame->GetSubFrameAt(i);
      if (currentSubFrame->IsFrameVisible()) {
         if (currentSubFrame->IsFrameTab()) {
            frame->fFrame->AddFrame(frame->fTab,frame->fLFrame);
            break;
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      currentSubFrame = frame->GetSubFrameAt(i);
      if (currentSubFrame->IsFrameVisible()) {
         // create subframe
         if (!currentSubFrame->IsFrameTab() && !currentSubFrame->IsFrameListTree() && !currentSubFrame->IsFrameListTreeItem()) {
            frame->fFrame->AddFrame(currentSubFrame->fFrame,currentSubFrame->fLFrame);
         }
      }
   }
}

//______________________________________________________________________________
void XMLToFormWindow::CreateFrame(XMLToFormFrame *frame)
{
   int i;
   XMLToFormFrame *subframe;

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
      subframe = frame->GetSubFrameAt(i);
      if (subframe->IsFrameVisible()) {
         CreateFrame(subframe);
      }
   }
}

//______________________________________________________________________________
void XMLToFormWindow::HideFrame(XMLToFormFrame *frame)
{
   int i;
   XMLToFormFrame *subframe;

   if (frame->IsFrameListTreeItem()) {
      frame->fListTreeFrame->HideFrame(frame->fFrame);
   }
   if (frame->IsFrameListTree()) {
      frame->fHFrame->HideFrame(frame->fFrame);
   }

   // hide subframes
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      subframe = frame->GetSubFrameAt(i);
      if (subframe->IsFrameVisible()) {
         HideFrame(subframe);
      }
   }
   if (frame->IsFrameListTree()) {
      frame->fHFrame->ShowFrame(frame->fFrame);
   }
}

//______________________________________________________________________________
void XMLToFormWindow::BuildForm(XMLToFormFrame *frame)
{
   // Frame
   if (frame->IsFrameVertical()) {
      frame->fFrame = new TGVerticalFrame(this,0,0,kRaisedFrame);
   } else {
      frame->fFrame = new ROMECompositeFrame(this,0,0,kRaisedFrame | kHorizontalFrame);
   }
   frame->fLFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->framePad, frame->framePad, frame->framePad, frame->framePad);
   frame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
   frame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
   frame->fLInnerRadioButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
   AddFrame(frame->fFrame,frame->fLFrame);
   frame->fIndex = 1;
}

//______________________________________________________________________________
void XMLToFormWindow::PlaceWindow(const TGWindow * main)
{
   MapSubwindows();

   // work around to disable warning from TString::AssertElement
   Int_t ignoreLevelOrg = gErrorIgnoreLevel;
   gErrorIgnoreLevel = kBreak;

   Resize(fMaximalWindowWidth,GetDefaultSize().fHeight);

   // work around to disable warning from TString::AssertElement
   gErrorIgnoreLevel = ignoreLevelOrg;

   // position relative to the parent's window
   Int_t ax, ay;
   if (main) {
      Window_t wdum;
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                                      static_cast<Int_t>(static_cast<const TGFrame*>(main)->GetWidth() -
                                                         fWidth) >> 1,
                                      static_cast<Int_t>(static_cast<const TGFrame*>(main)->GetHeight() -
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

//______________________________________________________________________________
Bool_t XMLToFormWindow::ListTreeClicked(TGListTreeItem* item,Int_t /*btn*/)
{
   ROMEString path;
   TGListTreeItem *currentItem = item;
   while (currentItem!=NULL) {
      path.InsertFormatted(0,"%s/",currentItem->GetText());
      currentItem = currentItem->GetParent();
   }
   path.Strip(TString::kTrailing,'/');
   XMLToFormFrame *frame = SearchFrame(fMainFrame,path.Data(),"");
   if (frame==NULL) {
      return true;
   }
   if (!frame->IsFrameListTreeItem()) {
      return true;
   }

   if (fTreeListActiveFrame!=NULL) {
      fTreeListActiveFrame->fListTreeFrame->HideFrame(fTreeListActiveFrame->fFrame);
   }
   frame->fListTreeFrame->ShowFrame(frame->fFrame);
   frame->fListTreeHFrame->ShowFrame(frame->fListTreeFrame);

   fTreeListActiveFrame = frame;
   return true;
}

//______________________________________________________________________________
Bool_t XMLToFormWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   XMLToFormFrame   *frame;
   XMLToFormElement *element;
   Int_t indx,i,ne;
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         *fExitID = parm1;
         SaveCurrentValues(fMainFrame);
         CloseWindow();
         break;
      case kCM_RADIOBUTTON:
         frame = fMainFrame;
         SearchWidget(parm1,&frame,&indx);
         ne = frame->GetNumberOfElements();
         for (i=0;i<ne;i++) {
            element = frame->GetElementAt(i);
            element->GetType();
            if (element->GetType()=="RadioButton" && i!=indx) {
               element->fRadioButton->SetState(kButtonUp);
            }
         }
         break;
      case kCM_COMBOBOX:
         if (parm1==-2 && fXML!=NULL) {
            frame = fMainFrame;
            SearchWidget(-2,&frame,&indx);
            element = frame->GetElementAt(indx);
            if (parm2==element->GetNumberOfEntries()-1) {
               Int_t exitButtonID;
               Int_t frameNameIndex;
               ROMEString temp;
               ROMEString newFile;
               Int_t num;
               ROMEStrArray entries;
               XMLToFormFrame *frameNameFrame = fMainFrame;
               XMLToFormFrame *fileNameEditFrame = new XMLToFormFrame(NULL,"","",false,XMLToFormFrame::kFrame,true,0);
               fileNameEditFrame->AddElement(new XMLToFormElement("EditBox","File Name","","",200,&entries));
               fileNameEditFrame->AddElement(new XMLToFormElement("Button","Ok","","",20,&entries));
               XMLToFormWindow *fFileNameWindow = new XMLToFormWindow(gClient->GetRoot(),this,fileNameEditFrame,&exitButtonID,200);
               fFileNameWindow->SearchWidget("File Name",&frameNameFrame,&frameNameIndex,fileNameEditFrame);

               newFile = frameNameFrame->GetElementAt(frameNameIndex)->fEditBoxBuffer->GetString();
               num =  element->GetNumberOfEntries();
               temp = element->GetEntryAt(num-1);
               element->AddEntryAt(newFile.Data(),num-1);
               element->AddEntryAt(temp.Data(),num);
               for (i=0;i<num-1;i++) {
                  UpdateFileSelector(element->GetEntryAt(i).Data());
               }
               SaveCurrentValues(fMainFrame);
               SaveFrame(fMainFrame,fXML);
               fXML->WritePathFile(newFile.Data());
               frame->GetElementAt(indx)->fComboBox->RemoveEntry(num-1);
               frame->GetElementAt(indx)->fComboBox->AddEntry(newFile.Data(),num-1);
               frame->GetElementAt(indx)->fComboBox->AddEntry(temp.Data(),num);
               frame->GetElementAt(indx)->fComboBox->Select(num-1);
            }
            else {
               ReloadValues(element->fComboBox->GetSelectedEntry()->GetTitle());
//               MapSubwindows();
//               MapWindow();
            }
         }
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

//______________________________________________________________________________
void XMLToFormWindow::SaveCurrentValues(XMLToFormFrame *frame)
{
   int i;
   XMLToFormElement *element;
   XMLToFormFrame   *subframe;
   for (i=0;i<frame->GetNumberOfElements();i++) {
      element = frame->GetElementAt(i);
      if (element->GetType()=="EditBox") {
         element->SetValue(element->fEditBoxBuffer->GetString());
      }
      if (element->GetType()=="ComboBox") {
         element->SetValue(element->fComboBox->GetSelectedEntry()->GetTitle());
         element->SetSelectedEntry(element->fComboBox->GetSelected());
      }
      if (element->GetType()=="CheckButton") {
         if (element->fCheckButton->GetState()==kButtonDown) {
            element->SetValue("true");
            element->SetButtonChecked(true);
         }
         else {
            element->SetValue("false");
            element->SetButtonChecked(false);
         }
      }
      if (element->GetType()=="RadioButton") {
         if (element->fRadioButton->GetState()==kButtonDown) {
            element->SetValue("true");
            element->SetButtonChecked(true);
         }
         else {
            element->SetValue("false");
            element->SetButtonChecked(false);
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      subframe = frame->GetSubFrameAt(i);
      if (subframe->IsFrameVisible())
         SaveCurrentValues(subframe);
   }
}

//______________________________________________________________________________
void XMLToFormWindow::SaveFrame(XMLToFormFrame *frame,ROMEXML *xml)
{
   Int_t i,j,num;
   XMLToFormElement *element;
   ROMEString path;
   ROMEString tempPath;
   ROMEString value;
   for (i=0;i<frame->GetNumberOfElements();i++) {
      element = frame->GetElementAt(i);
      if (element->GetType()=="EditBox") {
         path = element->GetPath();
         path.Append("/Value");
         xml->ReplacePathValue(path.Data(),element->GetValue().Data());
      }
      if (element->GetType()=="ComboBox") {
         path = element->GetPath();
         path.Append("/Selected");
         value.SetFormatted("%d",element->GetSelectedEntry());
         xml->ReplacePathValue(path.Data(),value.Data());
      }
      if (element->GetType()=="CheckButton") {
         path = element->GetPath();
         path.Append("/Checked");
         xml->ReplacePathValue(path.Data(),element->GetValue().Data());
      }
      if (element->GetType()=="RadioButton") {
         path = element->GetPath();
         path.Append("/Checked");
         xml->ReplacePathValue(path.Data(),element->GetValue().Data());
      }
      if (element->GetType()=="FileSelector") {
         path = element->GetPath();
         for (j=0;j<element->GetNumberOfEntries();j++) {
            tempPath = path;
            num = xml->NumberOfOccurrenceOfPath(path+"/Entry");
            if (num>j) {
               tempPath.AppendFormatted("/Entry[%d]",j+1);
               xml->ReplacePathValue(tempPath.Data(),element->GetEntryAt(j).Data());
            }
            else {
               if (num==0) {
                  xml->NewPathChildElement(path.Data(),"Entry",element->GetEntryAt(j).Data());
               }
               else {
                  tempPath.AppendFormatted("/Entry[%d]",num);
                  xml->NewPathLastElement(tempPath.Data(),"Entry",element->GetEntryAt(j).Data());
               }
            }
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible())
         SaveFrame(frame->GetSubFrameAt(i),xml);
   }
}

//______________________________________________________________________________
void XMLToFormWindow::UpdateFileSelector(const char *xmlFileName)
{
   Int_t indx;
   XMLToFormFrame *frame = fMainFrame;
   XMLToFormElement *element;
   ROMEString path;
   ROMEString tempPath;
   Int_t num,j;
   ROMEXML *xml = new ROMEXML();
   if (!xml->OpenFileForPath(xmlFileName))
      return;
   SearchWidget(-2,&frame,&indx);
   element = frame->GetElementAt(indx);
   path = element->GetPath();
   for (j=0;j<element->GetNumberOfEntries();j++) {
      tempPath = path;
      num = xml->NumberOfOccurrenceOfPath(path+"/Entry");
      if (num>j) {
         tempPath.AppendFormatted("/Entry[%d]",j+1);
         xml->ReplacePathValue(tempPath.Data(),element->GetEntryAt(j).Data());
      }
      else {
         if (num==0) {
            xml->NewPathChildElement(path.Data(),"Entry",element->GetEntryAt(j).Data());
         }
         else {
            tempPath.AppendFormatted("/Entry[%d]",num);
            xml->NewPathLastElement(tempPath.Data(),"Entry",element->GetEntryAt(j).Data());
         }
      }
   }
   xml->WritePathFile(xmlFileName);
   delete xml;
}

//______________________________________________________________________________
XMLToFormFrame* XMLToFormWindow::SearchFrame(XMLToFormFrame *frame,const char* title,const char* label)
{
   ROMEString titleStr;
   if (title!=NULL) {
      titleStr = title;
   }
   ROMEString labelStr;
   if (label!=NULL) {
      labelStr = label;
   }
   if (titleStr.Length()==0 && labelStr.Length()==0) {
      return frame;
   }

   ROMEString first;
   ROMEString rest;
   XMLToFormFrame *returnFrame;
   int i;
   if (titleStr.Length()>0) {
      GetFirstPathItem(titleStr.Data(),first,rest);
      if (frame->GetFrameTitle()==first) {
         returnFrame = SearchFrame(frame,rest,"");
         if (returnFrame!=NULL) {
            return returnFrame;
         }
      }
   }
   else if (labelStr.Length()>0) {
      GetFirstPathItem(labelStr.Data(),first,rest);
      for (i=0;i<frame->GetNumberOfElements();i++) {
         if (frame->GetElementAt(i)->GetTitle()==first) {
            returnFrame = SearchFrame(frame,"",rest);
            if (returnFrame!=NULL) {
               return returnFrame;
            }
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      returnFrame = SearchFrame(frame->GetSubFrameAt(i),titleStr.Data(),labelStr.Data());
      if (returnFrame!=NULL) {
         return returnFrame;
      }
   }
   return NULL;
}

//______________________________________________________________________________
void XMLToFormWindow::GetFirstPathItem(const char* path,ROMEString& firstItem,ROMEString& rest) const
{
   ROMEString pathString = path;
   ROMEString restPath;
   int ind = pathString.First('/');
   if (ind==0) {
      pathString = pathString(1,pathString.Length()-1);
      ind = pathString.First('/');
   }
   if (ind>-1) {
      restPath = pathString(ind+1,pathString.Length()-ind-1);
      pathString = pathString(0,ind);
   }
   rest = restPath.Data();
   firstItem = pathString.Data();
}

//______________________________________________________________________________
bool XMLToFormWindow::SearchWidget(Int_t id,XMLToFormFrame** frame,int *indx)
{
   int i;
   XMLToFormFrame *tempFrame;
   XMLToFormElement *element;
   for (i=0;i<(*frame)->GetNumberOfElements();i++) {
      element = (*frame)->GetElementAt(i);
      if (element->GetType()=="EditBox") {
         if (element->fEditBox->WidgetId()==id) {
            *indx = i;
            return true;
         }
      }
      if (element->GetType()=="ComboBox") {
         if (element->fComboBox->WidgetId()==id) {
            *indx = i;
            return true;
         }
      }
      if (element->GetType()=="CheckButton") {
         if (element->fCheckButton->WidgetId()==id) {
            *indx = i;
            return true;
         }
      }
      if (element->GetType()=="RadioButton") {
         if (element->fRadioButton->WidgetId()==id) {
            *indx = i;
            return true;
         }
      }
      if (element->GetType()=="Button") {
         if (element->fButton->WidgetId()==id) {
            *indx = i;
            return true;
         }
      }
      if (element->GetType()=="FileSelector") {
         if (element->fComboBox->WidgetId()==id) {
            *indx = i;
            return true;
         }
      }
   }
   for (i=0;i<(*frame)->GetNumberOfSubFrames();i++) {
      if ((*frame)->GetSubFrameAt(i)->IsFrameVisible()) {
         tempFrame = (*frame)->GetSubFrameAt(i);
         if (SearchWidget(id,&tempFrame,indx)) {
            *frame = tempFrame;
            return true;
         }
      }
   }
   return false;
}

//______________________________________________________________________________
bool XMLToFormWindow::SearchWidget(const char* path,XMLToFormFrame** frame,int *indx,XMLToFormFrame* mainFrame)
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
      if (*frame==NULL) {
         return false;
      }
   }
   delete [] frameNames;
   // search value
   *frame = SearchFrame(*frame,NULL,editName.Data());
   if (*frame!=NULL) {
      for (i=0;i<(*frame)->GetNumberOfElements();i++) {
         if (!strcmp((*frame)->GetElementAt(i)->GetTitle(),editName.Data())) {
            *indx = i;
            return true;
         }
      }
   }
   return true;
}

//______________________________________________________________________________
void XMLToFormWindow::SignalHandler()
{
   CheckSignals(fMainFrame);
}

//______________________________________________________________________________
void XMLToFormWindow::CheckSignals(XMLToFormFrame *frame)
{
   bool senderState = false;
   XMLToFormFrame   *recvFrame;
   XMLToFormElement *element;
   int i,j;
   for (i=0;i<frame->GetNumberOfElements();i++) {
      element = frame->GetElementAt(i);
      if (element->GetSignal()!=NULL) {
         if (element->GetType()=="CheckButton") {
            if (element->fCheckButton->GetState()==kButtonDown) {
               senderState = true;
            } else {
               senderState = false;
            }
         }
         if (element->GetType()=="RadioButton") {
            if (element->fRadioButton->GetState()==kButtonDown) {
               senderState = true;
            } else {
               senderState = false;
            }
         }
         for (j=0;j<element->GetSignal()->GetNumberOfReceivers();j++) {
            recvFrame = SearchFrame(fMainFrame,element->GetSignal()->GetReceiverPathAt(j).Data(),NULL);
            if (recvFrame==NULL) {
               return;
            }
            if (element->GetSignal()->GetReceiverTypeAt(j)=="Enable") {
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

//______________________________________________________________________________
void XMLToFormWindow::DeleteFrame(XMLToFormFrame *frame)
{
   Int_t i;
   XMLToFormElement *element;
   // Title
   if (frame->GetFrameTitle().Length()>0) {
      if (!frame->IsFrameTab() && !frame->IsFrameListTree() && !frame->IsFrameListTreeItem()) {
         delete frame->fTitleLabel;
      }
   }
   // Widget
   for (i=0;i<frame->GetNumberOfElements();i++) {
      element = frame->GetElementAt(i);
      if (element->GetType()=="EditBox") {
//         delete element->fEditBox;
         delete element->fEditVFrames;
         delete element->fEditLabel;
      }
      if (element->GetType()=="Button") {
         delete element->fButton;
      }
      if (element->GetType()=="ComboBox") {
         delete element->fComboLabel;
         delete element->fComboBox;
         delete element->fComboVFrames;
      }
      if (element->GetType()=="CheckButton") {
         delete element->fCheckButtonLabel;
         delete element->fCheckButton;
         delete element->fCheckButtonVFrames;
      }
      if (element->GetType()=="RadioButton") {
         delete element->fRadioButtonLabel;
         delete element->fRadioButton;
         delete element->fRadioButtonVFrames;
      }
      if (element->GetType()=="FileSelector") {
         delete element->fComboLabel;
         delete element->fComboBox;
         delete element->fComboVFrames;
      }
      if (element->GetSignal()!=NULL) {
         delete element->GetSignal();
      }
      delete element;
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
      delete [] frame->fHHFrameIndex;
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

//______________________________________________________________________________
Bool_t XMLToFormWindow::ReloadValues(const char* xmlFileName)
{
   if (fXML==NULL) {
      return false;
   }
   if (!fXML->OpenFileForPath(xmlFileName)) {
      return false;
   }
   return ReloadValues(fMainFrame);
}

//______________________________________________________________________________
Bool_t XMLToFormWindow::ReloadValues(XMLToFormFrame *frame)
{
   int i;
   XMLToFormElement *element;
   ROMEString value;

   for (i=0;i<frame->GetNumberOfElements();i++) {
      element = frame->GetElementAt(i);
      if (element->GetType()=="EditBox") {
         fXML->GetPathValue(element->GetPath()+"/Value",value);
         element->SetValue(value.Data());
         element->fEditBoxBuffer->RemoveText(0,element->fEditBoxBuffer->GetBufferLength());
         element->fEditBoxBuffer->AddText(0,value.Data());
         element->fEditBox->RequestFocus();
      }
      if (element->GetType()=="ComboBox") {
         fXML->GetPathValue(element->GetPath()+"/Selected",value);
         element->SetValue(value.Data());
         element->SetSelectedEntry(value.ToInteger());
         element->fComboBox->Select(value.ToInteger());
      }
      if (element->GetType()=="CheckButton") {
         fXML->GetPathValue(element->GetPath()+"/Checked",value);
         if (value=="true") {
            element->SetValue(value.Data());
            element->SetButtonChecked(true);
            element->fCheckButton->SetState(kButtonDown);
         }
         else {
            element->SetValue(value.Data());
            element->SetButtonChecked(false);
            element->fCheckButton->SetState(kButtonUp);
         }
      }
      if (element->GetType()=="RadioButton") {
         fXML->GetPathValue(element->GetPath()+"/Checked",value);
         if (value=="true") {
            element->SetValue(value.Data());
            element->SetButtonChecked(true);
            element->fRadioButton->SetState(kButtonDown);
         }
         else {
            element->SetValue(value.Data());
            element->SetButtonChecked(false);
            element->fRadioButton->SetState(kButtonUp);
         }
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         ReloadValues(frame->GetSubFrameAt(i));
      }
   }

   return true;
}
