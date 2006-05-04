//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// XMLToForm                                                                 //
//                                                                            //
// This class generates a dialog window dynamically out of a XML file.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////
// $Id$

#include "XMLToForm.h"
#include "XMLToFormWindow.h"

void XMLToForm::InitSubFrames(XMLToFormFrame *frame) {
   int i;
   ROMEString value;
   ROMEString currentPath;
   ROMEString titleValue;
   ROMEString titleValueT;
   PMXML_NODE frameNode;
   int nFrames = 0;
   int nTabs = 0;
   int nFrameTags = 0;
   int nTabTags = 0;
   bool foundFrame = false;

   frame->fNumberOfSubFrames = 0;
   // number of plain frames
   currentPath = frame->fFramePath->Data();
   currentPath += "/Frame";
   frame->fNumberOfSubFrames += fXML->NumberOfOccurrenceOfPath(currentPath.Data());
   // number of tab frames
   currentPath = frame->fFramePath->Data();
   currentPath += "/Tab";
   frame->fNumberOfSubFrames += fXML->NumberOfOccurrenceOfPath(currentPath.Data());
   // define frames
   frame->fSubFrames = new XMLToFormFrame*[frame->fNumberOfSubFrames];
   // read frames
   frameNode = fXML->GetPathNode(frame->fFramePath->Data());
   for (i=0;i<frameNode->n_children;i++) {
      foundFrame = false;
      // frame
      if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"Frame")) {
         nFrameTags++;
         frame->fSubFrames[nFrames] = new XMLToFormFrame();
         currentPath.SetFormatted("%s/Frame[%d]/FrameTitle",frame->fFramePath->Data(),nFrameTags);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            frame->fSubFrames[nFrames]->fVisible = false;
            nFrames++;
            continue;
         }
         foundFrame = true;
         currentPath = frame->fFramePath->Data();
         currentPath.AppendFormatted("/Frame[%d]",nFrames+1);
         frame->fSubFrames[nFrames]->fFramePath = new ROMEString(currentPath.Data());
         frame->fSubFrames[nFrames]->fIsTab = false;
         frame->fSubFrames[nFrames]->fTabIndex = -1;
         frame->fSubFrames[nFrames]->fVisible = true;

         // read type
         currentPath.AppendFormatted("/FrameType");
         fXML->GetPathValue(currentPath,value);
         if (value=="horizontal")
            frame->fSubFrames[nFrames]->fVerticalFrame = false;
         else
            frame->fSubFrames[nFrames]->fVerticalFrame = true;

         nFrames++;
      }
      // tab
      if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"Tab")) {
         nTabTags++;
         frame->fSubFrames[nFrames] = new XMLToFormFrame();
         currentPath.SetFormatted("%s/Tab[%d]/TabTitle",frame->fFramePath->Data(),nTabTags);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            frame->fSubFrames[nFrames]->fVisible = false;
            nFrames++;
            nTabs++;
            continue;
         }
         foundFrame = true;
         currentPath = frame->fFramePath->Data();
         currentPath.AppendFormatted("/Tab[%d]",nFrames+1);
         frame->fSubFrames[nFrames]->fFramePath = new ROMEString(currentPath.Data());
         frame->fSubFrames[nFrames]->fIsTab = true;
         frame->fSubFrames[nFrames]->fTabIndex = nTabs;
         nFrames++;
         nTabs++;
      }
      if (foundFrame) {
         if (titleValue.Length()>0 && frame->fSubFrames[nFrames-1]->fVisible) {
            frame->fSubFrames[nFrames-1]->fLTitleLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
            frame->fSubFrames[nFrames-1]->fTitleLabelHotString = new TGHotString(titleValue.Data());
         }
         else {
            frame->fSubFrames[nFrames-1]->fTitleLabel = NULL;
            frame->fSubFrames[nFrames-1]->fTitleLabelHotString = NULL;
            frame->fSubFrames[nFrames-1]->fLTitleLabel = NULL;
         }
      }
   }
}
void XMLToForm::XMLToClass(XMLToFormFrame *frame)
{
   int j,k,ind;
   ROMEString valueT;
   ROMEString value;
   ROMEString path;
   ROMEString currentPath;
   ROMEString savePath;

   // create path
   path = frame->fFramePath->Data();

   // read elements

   // number of editboxes
   currentPath = path+"/EditBox";
   frame->fNumberOfEditBoxes = fXML->NumberOfOccurrenceOfPath(currentPath.Data());
   // number of buttons
   currentPath = path+"/Button";
   frame->fNumberOfButtons = fXML->NumberOfOccurrenceOfPath(currentPath.Data());
   // number of comboboxes
   currentPath = path+"/ComboBox";
   frame->fNumberOfComboBoxes = fXML->NumberOfOccurrenceOfPath(currentPath.Data());
   // number of checkbuttons
   currentPath = path+"/CheckButton";
   frame->fNumberOfCheckButtons = fXML->NumberOfOccurrenceOfPath(currentPath.Data());
   // number of elements
   frame->fNumberOfHHFrames = 0;
   frame->fNumberOfVFrames = 0;
   frame->fNumberOfElements = frame->fNumberOfEditBoxes
                             +frame->fNumberOfButtons
                             +frame->fNumberOfComboBoxes
                             +frame->fNumberOfCheckButtons;
   if (frame->fNumberOfElements>0) {
      // define editboxes
      frame->fEditLabelHotString = new TGHotString*[frame->fNumberOfEditBoxes];
      frame->fEditBoxBuffer = new TGTextBuffer*[frame->fNumberOfEditBoxes];
      frame->fEditBoxPaths = new ROMEString*[frame->fNumberOfEditBoxes];
      frame->fEditBoxWidth = new int[frame->fNumberOfEditBoxes];
      frame->fEditBoxElementIndex = new int[frame->fNumberOfEditBoxes];
      for (j=0;j<frame->fNumberOfEditBoxes;j++) {
         // label
         currentPath = path;
         currentPath.AppendFormatted("/EditBox[%d]/Label",j+1);
         fXML->GetPathValue(currentPath,valueT);
         Substitute(valueT,value);
         frame->fEditLabelHotString[j] = new TGHotString(value.Data());
         // edit
         frame->fEditBoxBuffer[j] = new TGTextBuffer(50);
         currentPath = path;
         currentPath.AppendFormatted("/EditBox[%d]/Value",j+1);
         fXML->GetPathValue(currentPath,value);
         frame->fEditBoxBuffer[j]->AddText(0, value.Data());
         // path
         currentPath = path;
         currentPath.AppendFormatted("/EditBox[%d]",j+1);
         frame->fEditBoxPaths[j] = new ROMEString(currentPath.Data());
         // width
         currentPath = path;
         currentPath.AppendFormatted("/EditBox[%d]/Width",j+1);
         fXML->GetPathValue(currentPath,value);
         frame->fEditBoxWidth[j] = value.ToInteger();
      }
      // define buttons
      frame->fButtonID = new int[frame->fNumberOfButtons];
      frame->fButtonHotString = new TGHotString*[frame->fNumberOfButtons];
      frame->fButtonPaths = new ROMEString*[frame->fNumberOfButtons];
      frame->fButtonWidth = new int[frame->fNumberOfButtons];
      frame->fButtonElementIndex = new int[frame->fNumberOfButtons];
      for (j=0;j<frame->fNumberOfButtons;j++) {
         // button
         currentPath = path;
         currentPath.AppendFormatted("/Button[%d]/Text",j+1);
         fXML->GetPathValue(currentPath,valueT);
         Substitute(valueT,value);
         frame->fButtonHotString[j] = new TGHotString(value.Data());
         // ID
         currentPath = path;
         currentPath.AppendFormatted("/Button[%d]/ID",j+1);
         fXML->GetPathValue(currentPath,value);
         if (value.Length()>0)
            frame->fButtonID[j] = value.ToInteger();
         else
            frame->fButtonID[j] = -1;
         // path
         currentPath = path;
         currentPath.AppendFormatted("/Button[%d]",j+1);
         frame->fButtonPaths[j] = new ROMEString(currentPath.Data());
         // width
         currentPath = path;
         currentPath.AppendFormatted("/Button[%d]/Width",j+1);
         fXML->GetPathValue(currentPath,value);
         frame->fButtonWidth[j] = value.ToInteger();
      }
      // define comboboxes
      frame->fComboLabelHotString = new TGHotString*[frame->fNumberOfComboBoxes];
      frame->fComboBoxPaths = new ROMEString*[frame->fNumberOfComboBoxes];
      frame->fComboBoxEntries = new ROMEStrArray*[frame->fNumberOfComboBoxes];
      frame->fComboBoxSelectedEntry = new int[frame->fNumberOfComboBoxes];
      frame->fComboBoxWidth = new int[frame->fNumberOfComboBoxes];
      frame->fComboBoxElementIndex = new int[frame->fNumberOfComboBoxes];
      for (j=0;j<frame->fNumberOfComboBoxes;j++) {
         // label
         currentPath = path;
         currentPath.AppendFormatted("/ComboBox[%d]/Label",j+1);
         fXML->GetPathValue(currentPath,valueT);
         Substitute(valueT,value);
         frame->fComboLabelHotString[j] = new TGHotString(value.Data());
         // selected
         currentPath = path;
         currentPath.AppendFormatted("/ComboBox[%d]/Selected",j+1);
         fXML->GetPathValue(currentPath,value);
         frame->fComboBoxSelectedEntry[j] = value.ToInteger();
         // entries
         frame->fComboBoxEntries[j] = new ROMEStrArray(0);
         savePath = path;
         savePath.AppendFormatted("/ComboBox[%d]/Entry",j+1);
         for (k=0;k<fXML->NumberOfOccurrenceOfPath(savePath.Data());k++) {
            currentPath = path;
            currentPath.AppendFormatted("/ComboBox[%d]/Entry[%d]",j+1,k+1);
            fXML->GetPathValue(currentPath,valueT);
            Substitute(valueT,value);
            frame->fComboBoxEntries[j]->AddAtAndExpand(value.Data(),k);
         }
         currentPath = path;
         currentPath.AppendFormatted("/ComboBox[%d]/Entries",j+1);
         fXML->GetPathValue(currentPath,valueT);
         Substitute(valueT,value);
         if (value.Length()>0) {
            k = 0;
            while ((ind=value.Index(";"))!=-1) {
               valueT = value(0,ind);
               frame->fComboBoxEntries[j]->AddAtAndExpand(valueT.Data(),k);
               value = value(ind+1,value.Length()-ind-1);
               k++;
            }
            frame->fComboBoxEntries[j]->AddAtAndExpand(value.Data(),k);
         }
         // path
         currentPath = path;
         currentPath.AppendFormatted("/ComboBox[%d]",j+1);
         frame->fComboBoxPaths[j] = new ROMEString(currentPath.Data());
         // width
         currentPath = path;
         currentPath.AppendFormatted("/ComboBox[%d]/Width",j+1);
         fXML->GetPathValue(currentPath,value);
         frame->fComboBoxWidth[j] = value.ToInteger();
      }
      // define checkbuttons
      frame->fCheckButtonLabelHotString = new TGHotString*[frame->fNumberOfCheckButtons];
      frame->fCheckButtonChecked = new bool[frame->fNumberOfCheckButtons];
      frame->fCheckButtonPaths = new ROMEString*[frame->fNumberOfCheckButtons];
      frame->fCheckButtonWidth = new int[frame->fNumberOfCheckButtons];
      frame->fCheckButtonElementIndex = new int[frame->fNumberOfCheckButtons];
      frame->fCheckButton = NULL;
      for (j=0;j<frame->fNumberOfCheckButtons;j++) {
         // label
         currentPath = path;
         currentPath.AppendFormatted("/CheckButton[%d]/Label",j+1);
         fXML->GetPathValue(currentPath,valueT);
         Substitute(valueT,value);
         frame->fCheckButtonLabelHotString[j] = new TGHotString(value.Data());
         // button check
         currentPath = path;
         currentPath.AppendFormatted("/CheckButton[%d]/Checked",j+1);
         fXML->GetPathValue(currentPath,value);
         if (value=="true")
            frame->fCheckButtonChecked[j] = true;
         else
            frame->fCheckButtonChecked[j] = false;
         // path
         currentPath = path;
         currentPath.AppendFormatted("/CheckButton[%d]",j+1);
         frame->fCheckButtonPaths[j] = new ROMEString(currentPath.Data());
         // width
         currentPath = path;
         currentPath.AppendFormatted("/CheckButton[%d]/Width",j+1);
         fXML->GetPathValue(currentPath,value);
         frame->fCheckButtonWidth[j] = value.ToInteger();
      }
   
      // element index
      int nButton = 0;
      int nEditBox = 0;
      int nComboBox = 0;
      int nCheckButton = 0;
      int indElement = 0;
      PMXML_NODE node = fXML->GetPathNode(path);
      for (j=0;j<node->n_children;j++) {
         // editbox
         if (!strcmp(fXML->GetSubNode(node,j)->name,"EditBox")) {
            frame->fEditBoxElementIndex[nEditBox] = indElement;
            nEditBox++;
            indElement++;
         }
         // Button
         if (!strcmp(fXML->GetSubNode(node,j)->name,"Button")) {
            frame->fButtonElementIndex[nButton] = indElement;
            nButton++;
            indElement++;
         }
         // ComboBox
         if (!strcmp(fXML->GetSubNode(node,j)->name,"ComboBox")) {
            frame->fComboBoxElementIndex[nComboBox] = indElement;
            nComboBox++;
            indElement++;
         }
         // CheckButton
         if (!strcmp(fXML->GetSubNode(node,j)->name,"CheckButton")) {
            frame->fCheckButtonElementIndex[nCheckButton] = indElement;
            nCheckButton++;
            indElement++;
         }
      }
   }

   // subframes
   InitSubFrames(frame);
}

bool XMLToForm::XMLToRootClass()
{
   ROMEString value;
   ROMEString valueT;
   if (!fXML->OpenFileForPath(fXMLFileName.Data()))
      return false;
   if (fXML->NumberOfOccurrenceOfPath("/XMLToForm")<1)
      return false;
   fXML->GetPathValue("/XMLToForm/MaximalWindowWidth",value);
   fMaximalWindowWidth = value.ToInteger();

   fMainFrame = new XMLToFormFrame();
   fMainFrame->fFramePath = new ROMEString("/XMLToForm");

   fXML->GetPathValue("/XMLToForm/FrameType",value);
   if (value=="horizontal")
      fMainFrame->fVerticalFrame = false;
   else
      fMainFrame->fVerticalFrame = true;

   fMainFrame->fIsTab = false;
   fMainFrame->fVisible = true;

   fXML->GetPathValue("/XMLToForm/FrameTitle",valueT);
   Substitute(valueT,value);
   if (value.Length()>0) {
      fMainFrame->fLTitleLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
      fMainFrame->fTitleLabelHotString = new TGHotString(value.Data());
   }
   else {
      fMainFrame->fTitleLabel = NULL;
      fMainFrame->fTitleLabelHotString = NULL;
      fMainFrame->fLTitleLabel = NULL;
   }

   return true;
}

void XMLToForm::SaveForm()
{
   SaveFrame(fMainFrame);
   fXML->WritePathFile(fXMLFileName.Data());
}

void XMLToForm::SaveFrame(XMLToFormFrame *frame)
{
   int i;
   ROMEString path;
   ROMEString value;
   for (i=0;i<frame->fNumberOfEditBoxes;i++) {
      path = frame->fEditBoxPaths[i]->Append("/Value");
      fXML->ReplacePathValue(path.Data(),frame->fEditBoxBuffer[i]->GetString());
   }
   for (i=0;i<frame->fNumberOfComboBoxes;i++) {
      path = frame->fComboBoxPaths[i]->Append("/Selected");
      value.SetFormatted("%d",frame->fComboBox[i]->GetSelected());
      fXML->ReplacePathValue(path.Data(),value.Data());
   }
   for (i=0;i<frame->fNumberOfCheckButtons;i++) {
      path = frame->fCheckButtonPaths[i]->Append("/Checked");
      if (frame->fCheckButton[i]->GetState()==kButtonDown)
         fXML->ReplacePathValue(path.Data(),"true");
      else
         fXML->ReplacePathValue(path.Data(),"false");
   }
   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      if (frame->fSubFrames[i]->fVisible)
         SaveFrame(frame->fSubFrames[i]);
   }
}

XMLToFormFrame* XMLToForm::SearchFrame(XMLToFormFrame *frame,const char* title,const char* label)
{
   XMLToFormFrame *returnFrame;
   int i;
   if (title!=NULL && frame->fTitleLabelHotString!=NULL) {
      if (!strcmp(frame->fTitleLabelHotString->GetString(),title))
         return frame;
   }
   if (label!=NULL) {
      for (i=0;i<frame->fNumberOfEditBoxes;i++) {
         if (frame->fEditLabelHotString[i]!=NULL) {
            if (!strcmp(frame->fEditLabelHotString[i]->GetString(),label))
               return frame;
         }
      }
      for (i=0;i<frame->fNumberOfComboBoxes;i++) {
         if (frame->fComboLabelHotString[i]!=NULL) {
            if (!strcmp(frame->fComboLabelHotString[i]->GetString(),label))
               return frame;
         }
      }
      for (i=0;i<frame->fNumberOfCheckButtons;i++) {
         if (frame->fCheckButtonLabelHotString[i]!=NULL) {
            if (!strcmp(frame->fCheckButtonLabelHotString[i]->GetString(),label))
               return frame;
         }
      }
   }
   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      returnFrame = SearchFrame(frame->fSubFrames[i],title,label);
      if (returnFrame!=NULL)
         return returnFrame;
   }
   return NULL;
}

const char* XMLToForm::GetEditBoxValue(const char* editBoxLabel)
{
   return GetValue(editBoxLabel,EDIT_BOX);
}
int XMLToForm::GetEditBoxIntValue(const char* editBoxLabel)
{
   char *cstop;
   return strtol(GetValue(editBoxLabel,EDIT_BOX),&cstop,10);
}
double XMLToForm::GetEditBoxDoubleValue(const char* editBoxLabel)
{
   char *cstop;
   return strtod(GetValue(editBoxLabel,EDIT_BOX),&cstop);
}
const char* XMLToForm::GetComboBoxValue(const char* comboBoxLabel)
{
   return GetValue(comboBoxLabel,COMBO_BOX);
}
int XMLToForm::GetComboBoxIntValue(const char* comboBoxLabel)
{
   char *cstop;
   return strtol(GetValue(comboBoxLabel,COMBO_BOX),&cstop,10);
}
double XMLToForm::GetComboBoxDoubleValue(const char* comboBoxLabel)
{
   char *cstop;
   return strtod(GetValue(comboBoxLabel,COMBO_BOX),&cstop);
}
int XMLToForm::GetComboBoxSelectedIndex(const char* comboBoxLabel)
{
   return GetIndex(comboBoxLabel,COMBO_BOX);
}
const char* XMLToForm::GetCheckButtonValue(const char* checkButtonLabel)
{
   return GetValue(checkButtonLabel,CHECK_BUTTON);
}
bool XMLToForm::IsCheckButtonChecked(const char* checkButtonLabel)
{
   if (!strcmp(GetValue(checkButtonLabel,CHECK_BUTTON),"true"))
      return true;
   return false;
}

bool XMLToForm::SearchWidget(const char* path,int type,XMLToFormFrame** frame,int *index)
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
   *frame = fMainFrame;
   for (i=0;i<numberOfFrames;i++) {
      *frame = SearchFrame(*frame,frameNames[i]->Data(),NULL);
      if (*frame==NULL)
         return false;
   }
   delete [] frameNames;
   // search value
   *frame = SearchFrame(*frame,NULL,editName.Data());
   if (*frame!=NULL) {
      switch (type) {
         case EDIT_BOX :
            for (i=0;i<(*frame)->fNumberOfEditBoxes;i++) {
               if (!strcmp((*frame)->fEditLabelHotString[i]->GetString(),editName.Data())) {
                  *index = i;
                  return true;
               }
            }
            break;
         case COMBO_BOX :
            for (i=0;i<(*frame)->fNumberOfComboBoxes;i++) {
               if (!strcmp((*frame)->fComboLabelHotString[i]->GetString(),editName.Data())) {
                  *index = i;
                  return true;
               }
            }
            break;
         case CHECK_BUTTON :
            for (i=0;i<(*frame)->fNumberOfCheckButtons;i++) {
               if (!strcmp((*frame)->fCheckButtonLabelHotString[i]->GetString(),editName.Data())) {
                  *index = i;
                  return true;
               }
            }
            break;
         default :
            return false;
      }
   }
   return true;
}
const char* XMLToForm::GetValue(const char* label,int type)
{
   int index = 0;
   XMLToFormFrame *frame;
   if (!SearchWidget(label,type,&frame,&index))
      return "";
   switch (type) {
      case EDIT_BOX :
         return frame->fEditBoxBuffer[index]->GetString();
         break;
      case COMBO_BOX :
         return frame->fComboBoxEntries[index]->At(frame->fComboBoxSelectedEntry[index]).Data();
         break;
      case CHECK_BUTTON :
         if (frame->fCheckButton!=NULL) {
            if (frame->fCheckButton[index]->GetState()==kButtonDown)
               return "true";
            else
               return "false";
         }
         else {
            if (frame->fCheckButtonChecked[index])
               return "true";
            else
               return "false";
         }
         break;
   }
   return "";
}
int XMLToForm::GetIndex(const char* label,int type)
{
   int index = 0;
   XMLToFormFrame *frame;
   if (!SearchWidget(label,type,&frame,&index))
      return -1;
   switch (type) {
      case COMBO_BOX :
         return frame->fComboBox[index]->GetSelected();
         break;
   }
   return -1;
}
void XMLToForm::InitSubstitutes(ROMEStrArray* substitutes) 
{
   int i,ind;
   ROMEString str;
   if (substitutes==NULL) {
      fSubstitutes = NULL;
      fPlaceHolders = NULL;
      return;
   }
   fSubstitutes = new ROMEStrArray(substitutes->GetEntriesFast());
   fPlaceHolders = new ROMEStrArray(substitutes->GetEntriesFast());
   for (i=0;i<substitutes->GetEntriesFast();i++) {
      ind = substitutes->At(i).Index("=");
      if (ind==-1) {
         str.SetFormatted("%d",i);
         fPlaceHolders->Add(str);
         fSubstitutes->Add(substitutes->At(i));
      }
      else {
         str = substitutes->At(i)(0,ind);
         fPlaceHolders->Add(str);
         str = substitutes->At(i)(ind+1,substitutes->At(i).Length()-ind-1);
         fSubstitutes->Add(str);
      }
   }
}
bool XMLToForm::Substitute(ROMEString& placeHolder,ROMEString& substitute) {
   int i;
   ROMEString tempValue;
   if (placeHolder.Length()>0) {
      if (placeHolder[0]=='#') {
         tempValue = placeHolder(1,placeHolder.Length()-1);
         if (fPlaceHolders != NULL) {
            for (i=0;i<fPlaceHolders->GetEntriesFast();i++) {
               if (tempValue==fPlaceHolders->At(i)) {
                  substitute = fSubstitutes->At(i);
                  return true;
               }
            }
         }
         substitute="";
         return false;
      }
   }
   substitute = placeHolder;
   return true;
}
void XMLToForm::FillClass(XMLToFormFrame *frame)
{
   int i;

   // read xml
   XMLToClass(frame);

   // read subframes
   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      if (frame->fSubFrames[i]->fVisible)
         FillClass(frame->fSubFrames[i]);
   }
}


bool XMLToForm::Init(const char* xmlFileName,ROMEStrArray* substitutes)
{
   fXMLFileName = xmlFileName;
   fXML = new ROMEXML();
   InitSubstitutes(substitutes);

   // read xml
   if (!XMLToRootClass())
      return false;

   FillClass(fMainFrame);
   return true;
}
XMLToForm::XMLToForm(const char* xmlFileName,ROMEStrArray* substitutes)
{
   fWindow = NULL;
   fXML = NULL;
   fSubstitutes = NULL;
   fPlaceHolders = NULL;
   fMainFrame = NULL;
   Init(xmlFileName,substitutes);
}
XMLToForm::XMLToForm(const TGWindow * p, const TGWindow * main,const char* xmlFileName, int *exitButtonID,ROMEStrArray* substitutes)
{
   fWindow = NULL;
   fXML = NULL;
   fSubstitutes = NULL;
   fPlaceHolders = NULL;
   fMainFrame = NULL;
   if (!Init(xmlFileName,substitutes))
      return;
//   PrintFrame(fMainFrame);
   fWindow = new XMLToFormWindow(p,main,fMainFrame,exitButtonID,fMaximalWindowWidth);
}

XMLToForm::~XMLToForm()
{
   if (fMainFrame != NULL)
      DeleteFrame(fMainFrame);
#if defined( R__VISUAL_CPLUSPLUS )
   delete fWindow;
#endif
   delete fSubstitutes;
   delete fPlaceHolders;
   delete fXML;
}

void XMLToForm::DeleteFrame(XMLToFormFrame *frame)
{
   int i;
   // Title
   delete frame->fTitleLabel;
   // EditBox
   for (i=0;i<frame->fNumberOfEditBoxes;i++) {
      delete frame->fEditLabel[i];
      delete frame->fEditBox[i];
      delete frame->fEditBoxPaths[i];
      delete frame->fEditVFrames[i];
   }
   if (frame->fNumberOfEditBoxes>0) {
      delete [] frame->fEditLabel;
      delete [] frame->fEditBox;
      delete [] frame->fEditBoxPaths;
      delete [] frame->fEditVFrames;
      delete frame->fEditBoxWidth;
      delete frame->fEditBoxElementIndex;
   }
   // Button
   for (i=0;i<frame->fNumberOfButtons;i++) {
      delete frame->fButton[i];
      delete frame->fButtonPaths[i];
   }
   if (frame->fNumberOfButtons>0) {
      delete [] frame->fButton;
      delete [] frame->fButtonPaths;
      delete frame->fButtonID;
      delete frame->fButtonWidth;
      delete frame->fButtonElementIndex;      
   }
   // ComboBox
   for (i=0;i<frame->fNumberOfComboBoxes;i++) {
      delete frame->fComboLabel[i];
      delete frame->fComboBox[i];
      delete frame->fComboBoxPaths[i];
      delete frame->fComboVFrames[i];
      delete frame->fComboBoxEntries[i];
   }
   if (frame->fNumberOfComboBoxes>0) {
      delete [] frame->fComboLabel;
      delete [] frame->fComboBox;
      delete [] frame->fComboBoxPaths;
      delete [] frame->fComboVFrames;
      delete [] frame->fComboBoxEntries;
      delete frame->fComboBoxSelectedEntry;
      delete frame->fComboBoxWidth;
      delete frame->fComboBoxElementIndex;      
   }
   // CheckButton
   for (i=0;i<frame->fNumberOfCheckButtons;i++) {
      delete frame->fCheckButtonLabel[i];
      delete frame->fCheckButton[i];
      delete frame->fCheckButtonPaths[i];
      delete frame->fCheckButtonVFrames[i];
   }
   if (frame->fNumberOfCheckButtons>0) {
      delete [] frame->fCheckButtonLabel;
      delete [] frame->fCheckButton;
      delete [] frame->fCheckButtonPaths;
      delete [] frame->fCheckButtonVFrames;
      delete frame->fCheckButtonWidth;
      delete frame->fCheckButtonElementIndex;
      delete frame->fCheckButtonChecked;      
   }
   // Frames
   for (i=0;i<frame->fNumberOfHHFrames;i++) {
      delete frame->fHHFrames[i];
   }
   for (i=0;i<frame->fNumberOfVFrames;i++) {
      delete frame->fVFrames[i];
   }
   if (frame->fNumberOfElements>0) {
      delete [] frame->fHHFrames;
      delete [] frame->fVFrames;
      delete frame->fHFrame;
   }
   // SubFrames
   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      DeleteFrame(frame->fSubFrames[i]);
   }
   delete [] frame->fSubFrames;
   // Frame
   delete frame->fFrame;
   delete frame->fFramePath;
}



void XMLToForm::PrintFrame(XMLToFormFrame *frame,int tab)
{
   int i;
   ROMEString tabChar;
   for (i=0;i<tab;i++)
      tabChar += "   ";
   if (frame->fTitleLabelHotString)
      cout << tabChar.Data() << "Frame : " << frame->fTitleLabelHotString->Data() << endl;
   else
      cout << tabChar.Data() << "Frame : " << endl;
   for (i=0;i<frame->fNumberOfEditBoxes;i++) {
      cout << tabChar.Data() << "   EditBox : " << frame->fEditLabelHotString[i]->Data() << endl;
   }
   for (i=0;i<frame->fNumberOfComboBoxes;i++) {
      cout << tabChar.Data() << "   EditBox : " << frame->fComboLabelHotString[i]->Data() << endl;
   }
   for (i=0;i<frame->fNumberOfCheckButtons;i++) {
      cout << tabChar.Data() << "   EditBox : " << frame->fCheckButtonLabelHotString[i]->Data() << endl;
   }
   for (i=0;i<frame->fNumberOfButtons;i++) {
      cout << tabChar.Data() << "   EditBox : " << frame->fButtonHotString[i]->Data() << endl;
   }
   for (i=0;i<frame->fNumberOfSubFrames;i++) {
      PrintFrame(frame->fSubFrames[i],tab+1);
   }
}
