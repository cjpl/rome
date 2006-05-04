//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ROMEConfigToForm                                                           //
//                                                                            //
// This class generates a dialog window dynamically out of a 
// ROME configuration file.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////
// $Id$

#include "ROMEConfigToForm.h"

ROMEConfigToForm::ROMEConfigToForm()
{
   fWindow = NULL;
   fXML = NULL;
   fSubstitutes = NULL;
   fPlaceHolders = NULL;
   fMainFrame = NULL;
}

ROMEConfigToForm::~ROMEConfigToForm()
{
}

int ROMEConfigToForm::Show(const TGWindow * p, const TGWindow * main)
{
   int exitButtonID = 1;
   if (!Init())
      return -1;
   fWindow = new XMLToFormWindow(p,main,fMainFrame,&exitButtonID,fMaximalWindowWidth);
   return exitButtonID;
}

bool ROMEConfigToForm::Init()
{
   // read xml
   if (!XMLToRootClass())
      return false;

   XMLToClass(fMainFrame);
   return true;
}

bool ROMEConfigToForm::XMLToRootClass()
{
   ROMEString value;
   ROMEString valueT;
   fMaximalWindowWidth = 1000;

   fMainFrame = new XMLToFormFrame();
   fMainFrame->fFramePath = new ROMEString("/Configuration/MainConfiguration");

   fMainFrame->fVerticalFrame = true;

   fMainFrame->fIsTab = false;
   fMainFrame->fVisible = true;

   fMainFrame->fTitleLabel = NULL;
   fMainFrame->fTitleLabelHotString = NULL;
   fMainFrame->fLTitleLabel = NULL;

   fMainFrame->fLTitleLabel = NULL;

   return true;
}

void ROMEConfigToForm::XMLToClass(XMLToFormFrame *frame)
{
   ROMEString currentPath;
   int nFrames;
   nFrames = 0;

   frame->fVisible = true;
   ROMEConfigToFormElements *elements = new ROMEConfigToFormElements();
   AddElements(frame,elements);
   delete elements;

   frame->fNumberOfSubFrames = 2;
   frame->fNumberOfElements = 0;
   frame->fSubFrames = new XMLToFormFrame*[frame->fNumberOfSubFrames];
   // Tab Frame
   frame->fSubFrames[nFrames] = new XMLToFormFrame();
   frame->fSubFrames[nFrames]->fFramePath = new ROMEString(frame->fFramePath->Data());
   frame->fSubFrames[nFrames]->fIsTab = false;
   frame->fSubFrames[nFrames]->fTabIndex = nFrames;
   frame->fSubFrames[nFrames]->fLTitleLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
   frame->fSubFrames[nFrames]->fTitleLabelHotString = NULL;
   AddTabs(frame->fSubFrames[nFrames]);
   nFrames++;
   // Button Frame
   frame->fSubFrames[nFrames] = new XMLToFormFrame();
   frame->fSubFrames[nFrames]->fFramePath = new ROMEString(frame->fFramePath->Data());
   frame->fSubFrames[nFrames]->fIsTab = false;
   frame->fSubFrames[nFrames]->fTabIndex = nFrames;
   frame->fSubFrames[nFrames]->fLTitleLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
   frame->fSubFrames[nFrames]->fTitleLabelHotString = NULL;
   FillButtonFrame(frame->fSubFrames[nFrames]);
   nFrames++;
}
void ROMEConfigToForm::FillButtonFrame(XMLToFormFrame *frame)
{
   int nElement = 0;
   ROMEConfigToFormElements *elements = new ROMEConfigToFormElements();
   // Ok
   elements->AddElement("Button","Ok","","",1);
   // Cancel
   elements->AddElement("Button","Cancel","","",0);
   // Terminate
   elements->AddElement("Button","Terminate","","",-1);

   frame->fNumberOfSubFrames = 0;
   AddElements(frame,elements);
   delete elements;
}

void ROMEConfigToForm::AddElements(XMLToFormFrame *frame,ROMEConfigToFormElements *elements) 
{
   int j,k;
   int nEditBox=0;
   int nButton=0;
   int nComboBox=0;
   int nCheckButton=0;
   ROMEString value;
   frame->fNumberOfElements = elements->GetNumberOfElements();
   frame->fNumberOfEditBoxes = 0;
   frame->fNumberOfButtons = 0;
   frame->fNumberOfComboBoxes = 0;
   frame->fNumberOfCheckButtons = 0;
   for (j=0;j<frame->fNumberOfElements;j++) {
      if (elements->GetTypeAt(j)=="EditBox")
         frame->fNumberOfEditBoxes++;
      if (elements->GetTypeAt(j)=="Button")
         frame->fNumberOfButtons++;
      if (elements->GetTypeAt(j)=="ComboBox")
         frame->fNumberOfComboBoxes++;
      if (elements->GetTypeAt(j)=="CheckButton")
         frame->fNumberOfCheckButtons++;
   }
   // define editboxes
   frame->fEditLabelHotString = new TGHotString*[frame->fNumberOfEditBoxes];
   frame->fEditBoxBuffer = new TGTextBuffer*[frame->fNumberOfEditBoxes];
   frame->fEditBoxPaths = new ROMEString*[frame->fNumberOfEditBoxes];
   frame->fEditBoxWidth = new int[frame->fNumberOfEditBoxes];
   frame->fEditBoxElementIndex = new int[frame->fNumberOfEditBoxes];
   // define buttons
   frame->fButtonID = new int[frame->fNumberOfButtons];
   frame->fButtonHotString = new TGHotString*[frame->fNumberOfButtons];
   frame->fButtonPaths = new ROMEString*[frame->fNumberOfButtons];
   frame->fButtonWidth = new int[frame->fNumberOfButtons];
   frame->fButtonElementIndex = new int[frame->fNumberOfButtons];
   // define comboboxes
   frame->fComboLabelHotString = new TGHotString*[frame->fNumberOfComboBoxes];
   frame->fComboBoxPaths = new ROMEString*[frame->fNumberOfComboBoxes];
   frame->fComboBoxEntries = new ROMEStrArray*[frame->fNumberOfComboBoxes];
   frame->fComboBoxSelectedEntry = new int[frame->fNumberOfComboBoxes];
   frame->fComboBoxWidth = new int[frame->fNumberOfComboBoxes];
   frame->fComboBoxElementIndex = new int[frame->fNumberOfComboBoxes];
   // define checkbuttons
   frame->fCheckButtonLabelHotString = new TGHotString*[frame->fNumberOfCheckButtons];
   frame->fCheckButtonChecked = new bool[frame->fNumberOfCheckButtons];
   frame->fCheckButtonPaths = new ROMEString*[frame->fNumberOfCheckButtons];
   frame->fCheckButtonWidth = new int[frame->fNumberOfCheckButtons];
   frame->fCheckButtonElementIndex = new int[frame->fNumberOfCheckButtons];
   frame->fCheckButton = NULL;

   for (j=0;j<frame->fNumberOfElements;j++) {
      if (elements->GetTypeAt(j)=="EditBox") {
         // editboxes
         frame->fEditLabelHotString[nEditBox] = new TGHotString(elements->GetTitleAt(j).Data());
         frame->fEditBoxBuffer[nEditBox] = new TGTextBuffer(50);
         frame->fEditBoxBuffer[nEditBox]->AddText(0, elements->GetValueAt(j).Data());
         frame->fEditBoxPaths[nEditBox] = new ROMEString(elements->GetPathAt(j).Data());
         frame->fEditBoxWidth[nEditBox] = 0;
         frame->fEditBoxElementIndex[nEditBox] = j;
         nEditBox++;
      }
      if (elements->GetTypeAt(j)=="Button") {
         // button
         frame->fButtonHotString[nButton] = new TGHotString(elements->GetTitleAt(j).Data());
         // ID
         frame->fButtonID[nButton] = elements->GetButtonIDAt(j);
         // path
         frame->fButtonPaths[nButton] = new ROMEString(elements->GetPathAt(j).Data());
         frame->fButtonWidth[nButton] = 0;
         frame->fButtonElementIndex[nButton] = j;
         nButton++;
      }
      if (elements->GetTypeAt(j)=="ComboBox") {
         // combobox
         frame->fComboLabelHotString[nComboBox] = new TGHotString(elements->GetTitleAt(j).Data());
         frame->fComboBoxEntries[nComboBox] = new ROMEStrArray(0);
         frame->fComboBoxSelectedEntry[nComboBox] = 0;
         for (k=0;k<elements->GetNumberOfEntriesAt(j);k++) {
            frame->fComboBoxEntries[nComboBox]->AddAtAndExpand(elements->GetEntryAt(j,k).Data(),k);
            if (elements->GetValueAt(j)==elements->GetEntryAt(j,k))
               frame->fComboBoxSelectedEntry[nComboBox] = k;
         }
         frame->fComboBoxPaths[nComboBox] = new ROMEString(elements->GetPathAt(j).Data());
         frame->fComboBoxWidth[nComboBox] = 0;
         frame->fComboBoxElementIndex[nComboBox] = j;
         nComboBox++;
      }
      if (elements->GetTypeAt(j)=="CheckButton") {
         // checkbutton
         frame->fCheckButtonLabelHotString[nCheckButton] = new TGHotString(elements->GetTitleAt(j).Data());
         if (elements->GetValueAt(j)=="true")
            frame->fCheckButtonChecked[nCheckButton] = true;
         else
            frame->fCheckButtonChecked[nCheckButton] = false;
         frame->fCheckButtonPaths[nCheckButton] = new ROMEString(elements->GetPathAt(j).Data());
         frame->fCheckButtonWidth[nCheckButton] = 0;
         frame->fCheckButtonElementIndex[nCheckButton] = j;
         nCheckButton++;
      }
   }
}
