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


#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGButton.h>
#include <TGTab.h>
#include <TGLayout.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGComboBox.h>
#include "ROMEiostream.h"
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "ROMEXML.h"
#include "XMLToForm.h"
#include "XMLToFormElement.h"
#include "XMLToFormElementSignal.h"
#include "XMLToFormFrame.h"
#include "XMLToFormWindow.h"

ClassImp(XMLToForm)

void XMLToForm::InitSubFrames(XMLToFormFrame *frame) {
   Int_t i;
   ROMEString value;
   ROMEString currentPath;
   ROMEString pathValue;
   ROMEString titleValue;
   ROMEString titleValueT;
   PMXML_NODE frameNode;
   Int_t nFrames = 0;
   Int_t nTabs = 0;
   Int_t nListTrees = 0;
   Int_t nListTreeItems = 0;
   Bool_t visible=false;
   Bool_t vertical=false;
   Int_t tabIndex=0;

   // read frames
   frameNode = fXML->GetPathNode(frame->GetFramePath().Data());
   for (i=0;i<frameNode->n_children;i++) {
      // frame
      if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"Frame")) {
         nFrames++;
         currentPath.SetFormatted("%s/Frame[%d]/FrameTitle",frame->GetFramePath().Data(),nFrames);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            visible = false;
            frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kFrame,visible,tabIndex));
            continue;
         }
         currentPath = frame->GetFramePath().Data();
         currentPath.AppendFormatted("/Frame[%d]",nFrames);
         pathValue = currentPath;
         tabIndex = -1;
         visible = true;
         currentPath.AppendFormatted("/FrameType");
         fXML->GetPathValue(currentPath,value);
         if (value=="horizontal")
            vertical = false;
         else
            vertical = true;

         frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kFrame,visible,tabIndex));
      }
      // tab
      else if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"Tab")) {
         nTabs++;
         currentPath.SetFormatted("%s/Tab[%d]/TabTitle",frame->GetFramePath().Data(),nTabs);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            visible = false;
            frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kTab,visible,tabIndex));
            continue;
         }
         visible = true;
         currentPath = frame->GetFramePath().Data();
         currentPath.AppendFormatted("/Tab[%d]",nTabs);
         pathValue = currentPath;
         tabIndex = nTabs-1;
         frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kTab,visible,tabIndex));
      }
      // list tree
      else if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"ListTree")) {
         nListTrees++;
         currentPath.SetFormatted("%s/ListTree[%d]/ListTreeTitle",frame->GetFramePath().Data(),nListTrees);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            visible = false;
            frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kListTree,visible,tabIndex));
            continue;
         }
         visible = true;
         currentPath = frame->GetFramePath().Data();
         currentPath.AppendFormatted("/ListTree[%d]",nListTrees);
         pathValue = currentPath;
         frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kListTree,visible,tabIndex));
      }
      // list tree item
      else if (!strcmp(fXML->GetSubNode(frameNode,i)->name,"ListTreeItem")) {
         if (!frame->IsFrameListTree() && frame->IsFrameListTreeItem()) {
            continue;
         }
         nListTreeItems++;
         currentPath.SetFormatted("%s/ListTreeItem[%d]/ListTreeItemTitle",frame->GetFramePath().Data(),nListTreeItems);
         fXML->GetPathValue(currentPath,titleValueT);
         if (!Substitute(titleValueT,titleValue)) {
            visible = false;
            frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kListTreeItem,visible,tabIndex));
            continue;
         }
         visible = true;
         currentPath = frame->GetFramePath().Data();
         currentPath.AppendFormatted("/ListTreeItem[%d]",nListTreeItems);
         pathValue = currentPath;
         frame->AddSubFrame(new XMLToFormFrame(frame,titleValue,pathValue,vertical,XMLToFormFrame::kListTreeItem,visible,tabIndex));
      }
   }
   visible = false;
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible()) {
         visible = true;
         break;
      }
   }
   if (frame->GetNumberOfSubFrames()>0 && !visible)
      frame->SetFrameVisible(false);
}
void XMLToForm::XMLToClass(XMLToFormFrame *frame)
{
   Int_t j,k,ind;
   ROMEString temp;
   ROMEString value;
   ROMEString title;
   ROMEString path;
   ROMEString xmlPath;
   ROMEString currentPath;
   ROMEString savePath;
   ROMEString entry;
   Int_t width;
   Int_t buttonID;
   Int_t selectedEntry;
   ROMEStrArray* entries = new ROMEStrArray(0);

   // create path
   xmlPath = frame->GetFramePath().Data();

   Int_t nButton = 0;
   Int_t nEditBox = 0;
   Int_t nComboBox = 0;
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
            buttonID = temp.ToInteger();
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
         // selected
         currentPath = xmlPath;
         currentPath.AppendFormatted("/ComboBox[%d]/Selected",nComboBox+1);
         fXML->GetPathValue(currentPath,temp);
         selectedEntry = temp.ToInteger();
         if (selectedEntry>=entries->GetEntriesFast())
            selectedEntry = entries->GetEntriesFast()-1;
         // path
         path = xmlPath;
         path.AppendFormatted("/ComboBox[%d]",nComboBox+1);
         // width
         currentPath = xmlPath;
         currentPath.AppendFormatted("/ComboBox[%d]/Width",nComboBox+1);
         fXML->GetPathValue(currentPath,temp);
         width = temp.ToInteger();
         nComboBox++;
         frame->AddElement(new XMLToFormElement("ComboBox",title,entries->At(selectedEntry),path,width,entries));
      }
      if (!strcmp(node->child[j].name,"CheckButton")) {
         // label
         currentPath = xmlPath;
         currentPath.AppendFormatted("/CheckButton[%d]/Label",nButton+1);
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

Bool_t XMLToForm::XMLToRootClass()
{
   Bool_t vertical;
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

   fMainFrame = new XMLToFormFrame(NULL,value,"/XMLToForm",vertical,XMLToFormFrame::kFrame,true,0);

   return true;
}

void XMLToForm::SaveForm()
{
   SaveFrame(fMainFrame);
   fXML->WritePathFile(fXMLFileName.Data());
}

void XMLToForm::SaveFrame(XMLToFormFrame *frame)
{
   Int_t i;
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


Int_t XMLToForm::GetIntValue(const char* label)
{
   char *cstop;
   return strtol(GetValue(label),&cstop,10);
}
double XMLToForm::GetDoubleValue(const char* label)
{
   char *cstop;
   return strtod(GetValue(label),&cstop);
}
Int_t XMLToForm::GetSelectedIndex(const char* label)
{
   return GetIndex(label);
}
Bool_t XMLToForm::IsChecked(const char* label)
{
   if (!strcmp(GetValue(label),"true"))
      return true;
   return false;
}

const char* XMLToForm::GetValue(const char* label)
{
   Int_t index = 0;
   XMLToFormFrame *frame;
   if (!fWindow->SearchWidget(label,&frame,&index,fMainFrame))
      return "";
   return frame->GetElementAt(index)->GetValue().Data();
}
Int_t XMLToForm::GetIndex(const char* label)
{
   Int_t index = 0;
   XMLToFormFrame *frame;
   if (!fWindow->SearchWidget(label,&frame,&index,fMainFrame))
      return -1;
   return frame->GetElementAt(index)->GetSelectedEntry();
}
void XMLToForm::InitSubstitutes(ROMEStrArray* substitutes)
{
   Int_t i,ind;
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
Bool_t XMLToForm::Substitute(ROMEString& placeHolder,ROMEString& substitute) {
   Int_t i;
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
   Int_t i;

   // read xml
   XMLToClass(frame);

   // read subframes
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      if (frame->GetSubFrameAt(i)->IsFrameVisible())
         FillClass(frame->GetSubFrameAt(i));
   }
}


Bool_t XMLToForm::Init(const char* xmlFileName,ROMEStrArray* substitutes)
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

XMLToForm::XMLToForm(const TGWindow * p, const TGWindow * main,const char* xmlFileName, Int_t *exitButtonID,ROMEStrArray* substitutes)
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
   if (fMainFrame != NULL) {
      fWindow->DeleteFrame(fMainFrame);
   }
   SafeDelete(fSubstitutes);
   SafeDelete(fPlaceHolders);
   SafeDelete(fXML);
}



void XMLToForm::PrintFrame(XMLToFormFrame *frame,Int_t tab)
{
   Int_t i;
   ROMEString tabChar;
   for (i=0;i<tab;i++)
      tabChar += "   ";
   ROMEPrint::Print("%sFrame : %s\n", tabChar.Data(), frame->GetFrameTitle().Data());
   for (i=0;i<frame->GetNumberOfElements();i++) {
      ROMEPrint::Print("%s   %s : %s\n", tabChar.Data(), frame->GetElementAt(i)->GetType().Data(), frame->GetElementAt(i)->GetTitle().Data());
   }
   for (i=0;i<frame->GetNumberOfSubFrames();i++) {
      PrintFrame(frame->GetSubFrameAt(i),tab+1);
   }
}
