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

ClassImp(XMLToForm)

void XMLToForm::InitSubFrames(XMLToFormFrame *frame) {
   int i;
   ROMEString value;
   ROMEString currentPath;
   ROMEString pathValue;
   ROMEString titleValue;
   ROMEString titleValueT;
   PMXML_NODE frameNode;
   int nFrames = 0;
   int nTabs = 0;
   int nFrameTags = 0;
   int nTabTags = 0;
   bool visible=false;
   bool isTab=false;
   bool vertical=false;
   int tabIndex=0;

   // read frames
   frameNode = fXML->GetPathNode(frame->GetFramePath().Data());
   for (i=0;i<frameNode->n_children;i++) {
      // frame
      if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"Frame")) {
         nFrameTags++;
         currentPath.SetFormatted("%s/Frame[%d]/FrameTitle",frame->GetFramePath().Data(),nFrameTags);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            visible = false;
            nFrames++;
            continue;
         }
         currentPath = frame->GetFramePath().Data();
         currentPath.AppendFormatted("/Frame[%d]",nFrames+1);
         pathValue = currentPath;
         isTab = false;
         tabIndex = -1;
         visible = true;
         currentPath.AppendFormatted("/FrameType");
         fXML->GetPathValue(currentPath,value);
         if (value=="horizontal")
            vertical = false;
         else
            vertical = true;

         nFrames++;
      }
      // tab
      if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"Tab")) {
         nTabTags++;
         currentPath.SetFormatted("%s/Tab[%d]/TabTitle",frame->GetFramePath().Data(),nTabTags);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            visible = false;
            nFrames++;
            nTabs++;
            continue;
         }
         currentPath = frame->GetFramePath().Data();
         currentPath.AppendFormatted("/Tab[%d]",nFrames+1);
         pathValue = currentPath;
         isTab = true;
         tabIndex = nTabs;
         nFrames++;
         nTabs++;
      }
      frame->AddSubFrame(new XMLToFormFrame(titleValue,pathValue,vertical,isTab,visible,tabIndex));
   }
}
void XMLToForm::XMLToClass(XMLToFormFrame *frame)
{
   int j,k,ind;
   ROMEString temp;
   ROMEString value;
   ROMEString title;
   ROMEString path;
   ROMEString xmlPath;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString entry;
   int width;
   int buttonID;
   int selectedEntry;
   ROMEStrArray* entries = new ROMEStrArray(0);

   // create path
   xmlPath = frame->GetFramePath().Data();

   int nButton = 0;
   int nEditBox = 0;
   int nComboBox = 0;
   int nCheckButton = 0;
   PMXML_NODE node = fXML->GetPathNode(xmlPath);
   for (j=0;j<node->n_children;j++) {
      if (!strcmp(node->child[j].name,"EditBox")) {
         // label
         currentPath = xmlPath;
         currentPath.AppendFormatted("/EditBox[%d]/Label",nEditBox+1);
         fXML->GetPathValue(currentPath,temp);
         Substitute(temp,title);
         // edit
         currentPath = xmlPath;
         currentPath.AppendFormatted("/EditBox[%d]/Value",nEditBox+1);
         fXML->GetPathValue(currentPath,value);
         // path
         path = xmlPath;
         path.AppendFormatted("/EditBox[%d]",nEditBox+1);
         // width
         currentPath = xmlPath;
         currentPath.AppendFormatted("/EditBox[%d]/Width",nEditBox+1);
         fXML->GetPathValue(currentPath,temp);
         width = temp.ToInteger();
         nEditBox++;
         frame->AddElement(new XMLToFormElement("EditBox",title,value,path,width));
      }
      if (!strcmp(node->child[j].name,"Button")) {
         // label
         currentPath = xmlPath;
         currentPath.AppendFormatted("/Button[%d]/Text",nButton+1);
         fXML->GetPathValue(currentPath,temp);
         Substitute(temp,title);
         // ID
         currentPath = xmlPath;
         currentPath.AppendFormatted("/Button[%d]/ID",nButton+1);
         fXML->GetPathValue(currentPath,temp);
         if (temp.Length()>0)
            buttonID = value.ToInteger();
         else
            buttonID = -1;
         // path
         path = xmlPath;
         path.AppendFormatted("/Button[%d]",nButton+1);
         // width
         currentPath = xmlPath;
         currentPath.AppendFormatted("/Button[%d]/Width",nButton+1);
         fXML->GetPathValue(currentPath,temp);
         width = temp.ToInteger();
         nButton++;
         frame->AddElement(new XMLToFormElement("Button",title,value,path,width,buttonID));
      }
      if (!strcmp(node->child[j].name,"ComboBox")) {
         // label
         currentPath = xmlPath;
         currentPath.AppendFormatted("/ComboBox[%d]/Label",nComboBox+1);
         fXML->GetPathValue(currentPath,temp);
         Substitute(temp,title);
         // selected
         currentPath = xmlPath;
         currentPath.AppendFormatted("/ComboBox[%d]/Selected",nComboBox+1);
         fXML->GetPathValue(currentPath,temp);
         selectedEntry = temp.ToInteger();
         // entries
         savePath = xmlPath;
         savePath.AppendFormatted("/ComboBox[%d]/Entry",nComboBox+1);
         for (k=0;k<fXML->NumberOfOccurrenceOfPath(savePath.Data());k++) {
            currentPath = xmlPath;
            currentPath.AppendFormatted("/ComboBox[%d]/Entry[%d]",nComboBox+1,k+1);
            fXML->GetPathValue(currentPath,temp);
            Substitute(temp,entry);
            entries->AddAtAndExpand(entry.Data(),k);
         }
         currentPath = xmlPath;
         currentPath.AppendFormatted("/ComboBox[%d]/Entries",nComboBox+1);
         fXML->GetPathValue(currentPath,temp);
         Substitute(temp,entry);
         if (entry.Length()>0) {
            k = 0;
            while ((ind=entry.Index(";"))!=-1) {
               temp = entry(0,ind);
               entries->AddAtAndExpand(temp.Data(),k);
               entry = entry(ind+1,entry.Length()-ind-1);
               k++;
            }
            entries->AddAtAndExpand(entry.Data(),k);
         }
         // path
         path = xmlPath;
         path.AppendFormatted("/ComboBox[%d]",nComboBox+1);
         // width
         currentPath = xmlPath;
         currentPath.AppendFormatted("/ComboBox[%d]/Width",nComboBox+1);
         fXML->GetPathValue(currentPath,temp);
         width = temp.ToInteger();
         nComboBox++;
         frame->AddElement(new XMLToFormElement("ComboBox",title,value,path,width,entries));
      }
      if (!strcmp(node->child[j].name,"CheckButton")) {
         // label
         currentPath = xmlPath;
         currentPath.AppendFormatted("/CheckButton[%d]/Text",nButton+1);
         fXML->GetPathValue(currentPath,temp);
         Substitute(temp,title);
         // value
         currentPath = xmlPath;
         currentPath.AppendFormatted("/CheckButton[%d]/Checked",nButton+1);
         fXML->GetPathValue(currentPath,value);
         // ID
         currentPath = xmlPath;
         currentPath.AppendFormatted("/CheckButton[%d]/ID",nButton+1);
         fXML->GetPathValue(currentPath,temp);
         if (temp.Length()>0)
            buttonID = value.ToInteger();
         else
            buttonID = -1;
         // path
         path = xmlPath;
         path.AppendFormatted("/CheckButton[%d]",nButton+1);
         // width
         currentPath = xmlPath;
         currentPath.AppendFormatted("/CheckButton[%d]/Width",nButton+1);
         fXML->GetPathValue(currentPath,temp);
         width = temp.ToInteger();
         nButton++;
         frame->AddElement(new XMLToFormElement("CheckButton",title,value,path,width,buttonID));
      }
   }
   delete entries;

   // subframes
   InitSubFrames(frame);
}

bool XMLToForm::XMLToRootClass()
{
   bool vertical;
   ROMEString value;
   ROMEString valueT;
   if (!fXML->OpenFileForPath(fXMLFileName.Data()))
      return false;
   if (fXML->NumberOfOccurrenceOfPath("/XMLToForm")<1)
      return false;
   fXML->GetPathValue("/XMLToForm/MaximalWindowWidth",value);
   fMaximalWindowWidth = value.ToInteger();

   fXML->GetPathValue("/XMLToForm/FrameTitle",valueT);
   Substitute(valueT,value);

   fXML->GetPathValue("/XMLToForm/FrameType",value);
   if (value=="horizontal")
      vertical = false;
   else
      vertical = true;

   fMainFrame = new XMLToFormFrame(value,"/XMLToForm",vertical,false,true,0);

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
   for (i=0;i<frame->GetNumberOfElements();i++) {
      if (frame->GetElementAt(i)->GetType()=="EditBox") {
         path = frame->GetElementAt(i)->GetPath();
         path.Append("/Value");
         fXML->ReplacePathValue(path.Data(),frame->GetElementAt(i)->GetValue().Data());
      }
      if (frame->GetElementAt(i)->GetType()=="ComboBox") {
         path = frame->GetElementAt(i)->GetPath();
         path.Append("/Selected");
         value.SetFormatted("%d",frame->GetElementAt(i)->GetSelectedEntry());
         fXML->ReplacePathValue(path.Data(),value.Data());
      }
      if (frame->GetElementAt(i)->GetType()=="CheckButton") {
         path = frame->GetElementAt(i)->GetPath();
         path.Append("/Checked");
         fXML->ReplacePathValue(path.Data(),frame->GetElementAt(i)->GetValue().Data());
      }
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible())
         SaveFrame(frame->GetSubFrameAt(i));
   }
}


int XMLToForm::GetIntValue(const char* label)
{
   char *cstop;
   return strtol(GetValue(label),&cstop,10);
}
double XMLToForm::GetDoubleValue(const char* label)
{
   char *cstop;
   return strtod(GetValue(label),&cstop);
}
int XMLToForm::GetSelectedIndex(const char* label)
{
   return GetIndex(label);
}
bool XMLToForm::IsChecked(const char* label)
{
   if (!strcmp(GetValue(label),"true"))
      return true;
   return false;
}

const char* XMLToForm::GetValue(const char* label)
{
   int index = 0;
   XMLToFormFrame *frame;
   if (!fWindow->SearchWidget(label,&frame,&index))
      return "";
   return frame->GetElementAt(index)->GetValue().Data();
}
int XMLToForm::GetIndex(const char* label)
{
   int index = 0;
   XMLToFormFrame *frame;
   if (!fWindow->SearchWidget(label,&frame,&index))
      return -1;
   return frame->GetElementAt(index)->GetSelectedEntry();
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
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible())
         FillClass(frame->GetSubFrameAt(i));
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
   delete fSubstitutes;
   delete fPlaceHolders;
   delete fXML;
}

void XMLToForm::DeleteFrame(XMLToFormFrame *frame)
{
   int i;
   // Title
   if (frame->GetFrameTitle().Length()>0 && !frame->IsFrameTab())
      delete frame->fTitleLabel;
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
      delete [] frame->fVFrames;
      delete frame->fHFrame;
   }
   // SubFrames
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      DeleteFrame(frame->GetSubFrameAt(i));
   }
   // Frame
   delete frame->fFrame;
}



void XMLToForm::PrintFrame(XMLToFormFrame *frame,int tab)
{
   int i;
   ROMEString tabChar;
   for (i=0;i<tab;i++)
      tabChar += "   ";
   cout << tabChar.Data() << "Frame : " << frame->GetFrameTitle().Data() << endl;
   for (i=0;i<frame->GetNumberOfElements();i++) {
      cout << tabChar.Data() << "   " << frame->GetElementAt(i)->GetType().Data() << " : " << frame->GetElementAt(i)->GetTitle().Data() << endl;
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      PrintFrame(frame->GetSubFrameAt(i),tab+1);
   }
}
