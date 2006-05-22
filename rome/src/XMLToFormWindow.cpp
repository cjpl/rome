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

#include "XMLToFormWindow.h"

ClassImp(XMLToFormWindow)

void XMLToFormWindow::BuildFrame(XMLToFormFrame *frame)
{
   int i,j,k;
   int maxWidth,index,additionalWidth;
   TGLabel *tempLabel;
   XMLToFormFrame *currentSubFrame;
   ROMEString value;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString str;

   // create title
   if (frame->GetFrameTitle().Length()>0) {
      if (frame->IsFrameTab()) {
         frame->fTitleString = new TGHotString(frame->GetFrameTitle().Data());
         frame->fParentFrame->fTab->GetTabTab(frame->GetFrameTabIndex())->SetText(frame->fTitleString);
      }
      else {
         frame->fTitleLabel = new TGLabel(frame->fFrame, frame->GetFrameTitle().Data());
#if defined( R__VISUAL_CPLUSPLUS )
         frame->fTitleLabel->SetTextFont("arial bold", kTRUE);
#else
         frame->fTitleLabel->SetTextFont("-adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1", kTRUE);
         /* available fonts under UNIX by default
            TGFont: -adobe-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1, prop, ref cnt = 2
            TGFont: -adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1, prop, ref cnt = 3
            TGFont: -adobe-helvetica-bold-r-*-*-12-*-*-*-*-*-iso8859-1, prop, ref cnt = 1
            TGFont: -adobe-courier-medium-r-*-*-12-*-*-*-*-*-iso8859-1, fixed, ref cnt = 1
          */
#endif
         frame->fFrame->AddFrame(frame->fTitleLabel, frame->fLTitleLabel);
      }
   }

   // create elements

   if (frame->GetNumberOfElements()>0) {
      // calculate number of vframes
      maxWidth = 0;
      // editboxes
      for (j=0;j<frame->GetNumberOfElements();j++) {
         if (frame->GetElementAt(j)->GetType()=="Button" || frame->GetElementAt(j)->GetType()=="ComboBox")
            additionalWidth = 8;
         else
            additionalWidth = 0;
         tempLabel = new TGLabel(0,frame->GetElementAt(j)->GetTitle().Data());
         if (frame->GetElementAt(j)->GetWidth()<(int)tempLabel->GetSize().fWidth+additionalWidth)
            frame->GetElementAt(j)->SetWidth(tempLabel->GetSize().fWidth+additionalWidth);
         if (maxWidth<frame->GetElementAt(j)->GetWidth())
            maxWidth = frame->GetElementAt(j)->GetWidth()+2*frame->elementPad;
         delete tempLabel;
         for (k=0;k<frame->GetElementAt(j)->GetNumberOfEntries();k++) {
            tempLabel = new TGLabel(0,frame->GetElementAt(j)->GetEntryAt(k).Data());
            if (frame->GetElementAt(j)->GetWidth()<(int)tempLabel->GetSize().fWidth)
               frame->GetElementAt(j)->SetWidth(tempLabel->GetSize().fWidth);
            if (maxWidth<frame->GetElementAt(j)->GetWidth())
               maxWidth = frame->GetElementAt(j)->GetWidth()+2*frame->elementPad;
            delete tempLabel;
         }
      }

      // create hframes
      frame->fHFrame = new TGHorizontalFrame(frame->fFrame,0,0);
      frame->fFrame->AddFrame(frame->fHFrame, frame->fLInnerFrame);
      // create hhframes
      frame->fHHFrames = new TGHorizontalFrame*[2*frame->GetNumberOfElements()];
      // create vframes
      frame->fNumberOfVFrames = (fMaximalWindowWidth+2*frame->framePad)/(maxWidth);
      if (frame->fNumberOfVFrames<1)
         frame->fNumberOfVFrames = 1;
      if (frame->fNumberOfVFrames>frame->GetNumberOfElements())
         frame->fNumberOfVFrames=frame->GetNumberOfElements();
      frame->fVFrames = new TGVerticalFrame*[frame->fNumberOfVFrames];
      for (j=0;j<frame->fNumberOfVFrames;j++) {
         frame->fVFrames[j] = new TGVerticalFrame(frame->fHFrame,0,0);
         frame->fHFrame->AddFrame(frame->fVFrames[j], frame->fLInnerFrame);
      }

      // create elements
      int nDiv = 1;
      int nPart = 1;
      frame->fNumberOfHHFrames = 0;
      for (j=0;j<frame->GetNumberOfElements();j++) {
         index = frame->fNumberOfHHFrames%frame->fNumberOfVFrames;
         // editbox
         if (frame->GetElementAt(j)->GetType()=="EditBox") {
            nDiv = TMath::Min(maxWidth/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
               frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerFrame);
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = maxWidth/(frame->GetElementAt(j)->GetWidth()+10);
            }
            // hints
            frame->GetElementAt(j)->fLEditLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            frame->GetElementAt(j)->fLEditBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            frame->GetElementAt(j)->fEditVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->GetElementAt(j)->fEditVFrames, frame->fLInnerFrame);
            // label
            frame->GetElementAt(j)->fEditLabel = new TGLabel(frame->GetElementAt(j)->fEditVFrames, frame->GetElementAt(j)->GetTitle().Data());
            frame->GetElementAt(j)->fEditVFrames->AddFrame(frame->GetElementAt(j)->fEditLabel, frame->GetElementAt(j)->fLEditLabel);
            // edit
            frame->GetElementAt(j)->fEditBoxBuffer = new TGTextBuffer(50);
            frame->GetElementAt(j)->fEditBoxBuffer->AddText(0, frame->GetElementAt(j)->GetValue().Data());
            frame->GetElementAt(j)->fEditBox = new TGTextEntry(frame->GetElementAt(j)->fEditVFrames, frame->GetElementAt(j)->fEditBoxBuffer);
            frame->GetElementAt(j)->fEditBox->Associate(this);
            frame->GetElementAt(j)->fEditBox->Resize(frame->GetElementAt(j)->GetWidth(), 22);
            frame->GetElementAt(j)->fEditVFrames->AddFrame(frame->GetElementAt(j)->fEditBox, frame->GetElementAt(j)->fLEditBox);
            if (fFirstEdit) {
//               frame->fEditBox[nEditBox]->SelectAll();
               frame->GetElementAt(j)->fEditBox->SetFocus();
               fFirstEdit = false;
            }
         }
         // buttons
         if (frame->GetElementAt(j)->GetType()=="Button") {
            nDiv = TMath::Min(maxWidth/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
//               if (frame->fNumberOfButtons!=frame->fNumberOfElements)
                  frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
//               else
//                  frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,0);
               frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerFrame);
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = maxWidth/(frame->GetElementAt(j)->GetWidth()+10);
            }
            // hints
            frame->GetElementAt(j)->fLButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsCenterX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // button
            if (frame->GetElementAt(j)->GetButtonID()!=-1)
               frame->GetElementAt(j)->fButton = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], frame->GetElementAt(j)->GetTitle().Data(),frame->GetElementAt(j)->GetButtonID());
            else
               frame->GetElementAt(j)->fButton = new TGTextButton(frame->fHHFrames[frame->fNumberOfHHFrames-1], frame->GetElementAt(j)->GetTitle().Data());
            frame->GetElementAt(j)->fButton->Associate(this);
            frame->GetElementAt(j)->fButton->Resize(frame->GetElementAt(j)->GetWidth(), 22);
            frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->GetElementAt(j)->fButton, frame->GetElementAt(j)->fLButton);
         }
         // combobox
         if (frame->GetElementAt(j)->GetType()=="ComboBox") {
            nDiv = TMath::Min(maxWidth/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight);
               frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerFrame);
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = maxWidth/(frame->GetElementAt(j)->GetWidth()+10);
            }
            // hints
            frame->GetElementAt(j)->fLComboLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            frame->GetElementAt(j)->fLComboBox = new TGLayoutHints(kLHintsTop | kLHintsExpandX, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            frame->GetElementAt(j)->fComboVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->GetElementAt(j)->fComboVFrames, frame->fLInnerFrame);
            // label
            frame->GetElementAt(j)->fComboLabel = new TGLabel(frame->GetElementAt(j)->fComboVFrames, frame->GetElementAt(j)->GetTitle().Data());
            frame->GetElementAt(j)->fComboVFrames->AddFrame(frame->GetElementAt(j)->fComboLabel, frame->GetElementAt(j)->fLComboLabel);
            // edit
            frame->GetElementAt(j)->fComboBox = new TGComboBox(frame->GetElementAt(j)->fComboVFrames);
            frame->GetElementAt(j)->fComboBox->Associate(this);
            frame->GetElementAt(j)->fComboBox->Resize(0, 22);
            for (k=0;k<frame->GetElementAt(j)->GetNumberOfEntries();k++) {
               frame->GetElementAt(j)->fComboBox->AddEntry(frame->GetElementAt(j)->GetEntryAt(k).Data(),k);
            }
            frame->GetElementAt(j)->fComboBox->Select(frame->GetElementAt(j)->GetSelectedEntry());
            frame->GetElementAt(j)->fComboVFrames->AddFrame(frame->GetElementAt(j)->fComboBox, frame->GetElementAt(j)->fLComboBox);
         }
         // checkbuttons
         if (frame->GetElementAt(j)->GetType()=="CheckButton") {
            nDiv = TMath::Min(maxWidth/(frame->GetElementAt(j)->GetWidth()+10),nDiv);
            if (nDiv>nPart && frame->GetNumberOfElements()>frame->fNumberOfVFrames) {
               nPart++;
            }
            else {
               frame->fHHFrames[frame->fNumberOfHHFrames] = new TGHorizontalFrame(frame->fVFrames[index],0,58,kFixedHeight | kChildFrame);
               frame->fVFrames[index]->AddFrame(frame->fHHFrames[frame->fNumberOfHHFrames], frame->fLInnerCheckButtonFrame);
               frame->fNumberOfHHFrames++;
               nPart = 1;
               nDiv = maxWidth/(frame->GetElementAt(j)->GetWidth()+10);
            }
            // hints
            frame->GetElementAt(j)->fLCheckButtonLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            frame->GetElementAt(j)->fLCheckButton = new TGLayoutHints(kFixedWidth | kFixedHeight | kLHintsLeft, frame->elementPad, frame->elementPad, frame->elementPad, frame->elementPad);
            // vframe
            frame->GetElementAt(j)->fCheckButtonVFrames = new TGVerticalFrame(frame->fHHFrames[frame->fNumberOfHHFrames-1],0,0);
            frame->fHHFrames[frame->fNumberOfHHFrames-1]->AddFrame(frame->GetElementAt(j)->fCheckButtonVFrames, frame->fLInnerFrame);
            // label
            frame->GetElementAt(j)->fCheckButtonLabel = new TGLabel(frame->GetElementAt(j)->fCheckButtonVFrames, frame->GetElementAt(j)->GetTitle().Data());
            frame->GetElementAt(j)->fCheckButtonVFrames->AddFrame(frame->GetElementAt(j)->fCheckButtonLabel, frame->GetElementAt(j)->fLCheckButtonLabel);
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
            frame->GetElementAt(j)->fCheckButtonVFrames->AddFrame(frame->GetElementAt(j)->fCheckButton, frame->GetElementAt(j)->fLCheckButton);
         }
      }
   }

   // subframes

   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         if (frame->GetSubFrameAt(i)->IsFrameTab()) {
            frame->fTab = new TGTab(frame->fFrame);
            frame->fFrame->AddFrame(frame->fTab,frame->fLFrame);
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
         currentSubFrame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
         currentSubFrame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
         if (currentSubFrame->IsFrameTab()) {
            currentSubFrame->fFrame = frame->fTab->AddTab("");
         }
         else {
            if (currentSubFrame->IsFrameVertical())
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
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible())
         CreateFrame(frame->GetSubFrameAt(i));
   }
}

void XMLToFormWindow::BuildForm(XMLToFormFrame *frame)
{
   // Frame
   if (frame->IsFrameVertical())
      frame->fFrame = new TGVerticalFrame(this,0,0,kRaisedFrame);
   else
      frame->fFrame = new TGHorizontalFrame(this,0,0,kRaisedFrame);
   frame->fLFrame = new TGLayoutHints(kLHintsExpandX | kFixedHeight, frame->framePad, frame->framePad, frame->framePad, frame->framePad);
   frame->fLInnerFrame = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
   frame->fLInnerCheckButtonFrame = new TGLayoutHints(kLHintsExpandX, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad, frame->innerFramePad);
   AddFrame(frame->fFrame,frame->fLFrame);
   frame->fIndex = 1;
   frame->fParentFrame = NULL;
/*   frame->fIsTab = false;
   frame->fTabIndex = -1;
   todo*/
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
   fMainFrame = frame;

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

Bool_t XMLToFormWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
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
   bool senderState;
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
