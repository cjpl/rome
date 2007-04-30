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
#include <ROMELabel.h>
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

//______________________________________________________________________________
XMLToForm::XMLToForm(const char* xmlFileName, ROMEStrArray* substitutes)
:TObject()
,fWindow(0)
,fMaximalWindowWidth(1000)
,fXML(0)
,fXMLFileName("")
,fMainFrame(0)
,fSubstitutes(0)
,fPlaceHolders(0)
{
   Init(xmlFileName,substitutes);
}

//______________________________________________________________________________
XMLToForm::XMLToForm()
:TObject()
,fWindow(0)
,fMaximalWindowWidth(1000)
,fXML(0)
,fXMLFileName("")
,fMainFrame(0)
,fSubstitutes(0)
,fPlaceHolders(0)
{
}

//______________________________________________________________________________
XMLToForm::XMLToForm(const TGWindow * p, const TGWindow * main,const char* xmlFileName,
                     Int_t *exitButtonID,ROMEStrArray* substitutes)
:TObject()
,fWindow(0)
,fMaximalWindowWidth(1000)
,fXML(0)
,fXMLFileName("")
,fMainFrame(0)
,fSubstitutes(0)
,fPlaceHolders(0)
{
   if (!Init(xmlFileName,substitutes))
      return;
//   PrintFrame(fMainFrame);
   fWindow = new XMLToFormWindow(p,main,fMainFrame,exitButtonID,fMaximalWindowWidth,fXML);
}

//______________________________________________________________________________
XMLToForm::~XMLToForm()
{
   if (fMainFrame != NULL) {
      fWindow->DeleteFrame(fMainFrame);
   }
   SafeDelete(fSubstitutes);
   SafeDelete(fPlaceHolders);
   SafeDelete(fXML);
}

//______________________________________________________________________________
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

//______________________________________________________________________________
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
   ROMEString buttonName;
   Int_t width;
   Int_t buttonID;
   Int_t selectedEntry;
   ROMEStrArray* entries = new ROMEStrArray(0);

   // create path
   xmlPath = frame->GetFramePath().Data();

   Int_t nButton = 0;
   Int_t nEditBox = 0;
   Int_t nComboBox = 0;
   Int_t nFileSelector = 0;
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
      if (!strcmp(node->child[j].name,"CheckButton") || !strcmp(node->child[j].name,"RadioButton")) {
         if (!strcmp(node->child[j].name,"CheckButton"))
            buttonName = "CheckButton";
         if (!strcmp(node->child[j].name,"RadioButton"))
            buttonName = "RadioButton";
         // label
         currentPath = xmlPath;
         currentPath.AppendFormatted("/%s[%d]/Label",buttonName.Data(),nButton+1);
         fXML->GetPathValue(currentPath,temp);
         Substitute(temp,title);
         // value
         currentPath = xmlPath;
         currentPath.AppendFormatted("/%s[%d]/Checked",buttonName.Data(),nButton+1);
         fXML->GetPathValue(currentPath,value);
         // ID
         currentPath = xmlPath;
         currentPath.AppendFormatted("/%s[%d]/ID",buttonName.Data(),nButton+1);
         fXML->GetPathValue(currentPath,temp);
         if (temp.Length()>0)
            buttonID = value.ToInteger();
         else
            buttonID = -1;
         // path
         path = xmlPath;
         path.AppendFormatted("/%s[%d]",buttonName.Data(),nButton+1);
         // width
         currentPath = xmlPath;
         currentPath.AppendFormatted("/%s[%d]/Width",buttonName.Data(),nButton+1);
         fXML->GetPathValue(currentPath,temp);
         width = temp.ToInteger();
         nButton++;
         frame->AddElement(new XMLToFormElement(buttonName.Data(),title,value,path,width,buttonID));
      }
      if (!strcmp(node->child[j].name,"FileSelector") && nFileSelector==0) {
         // label
         currentPath = xmlPath;
         currentPath.AppendFormatted("/FileSelector[%d]/Label",nFileSelector+1);
         fXML->GetPathValue(currentPath,temp);
         Substitute(temp,title);
         // entries
         savePath = xmlPath;
         savePath.AppendFormatted("/FileSelector[%d]/Entry",nFileSelector+1);
         for (k=0;k<fXML->NumberOfOccurrenceOfPath(savePath.Data());k++) {
            currentPath = xmlPath;
            currentPath.AppendFormatted("/FileSelector[%d]/Entry[%d]",nFileSelector+1,k+1);
            fXML->GetPathValue(currentPath,temp);
            Substitute(temp,entry);
            entries->AddAtAndExpand(entry.Data(),k);
         }
         currentPath = xmlPath;
         currentPath.AppendFormatted("/FileSelector[%d]/Entries",nFileSelector+1);
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
         selectedEntry = -1;
         for (k=0;k<entries->GetEntriesFast();k++) {
            if (fXMLFileName==entries->At(k)) {
               selectedEntry = k;
               break;
            }
         }
         if (selectedEntry==-1) {
            selectedEntry = entries->GetEntriesFast();
            entries->AddAtAndExpand(fXMLFileName.Data(),entries->GetEntriesFast());
         }
         ind = -1;
         for (k=0;k<entries->GetEntriesFast();k++) {
            if ("Save Current As ..."==entries->At(k)) {
               ind = k;
               break;
            }
         }
         if (ind==-1) {
            entries->AddAtAndExpand("Save Current As ...",entries->GetEntriesFast());
         }
         // path
         path = xmlPath;
         path.AppendFormatted("/FileSelector[%d]",nFileSelector+1);
         // width
         currentPath = xmlPath;
         currentPath.AppendFormatted("/FileSelector[%d]/Width",nFileSelector+1);
         fXML->GetPathValue(currentPath,temp);
         width = temp.ToInteger();
         nFileSelector++;
         frame->AddElement(new XMLToFormElement("FileSelector",title,entries->At(selectedEntry),path,width,entries));
      }
   }
   delete entries;

   // subframes
   InitSubFrames(frame);
}

//______________________________________________________________________________
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

//______________________________________________________________________________
void XMLToForm::SaveForm()
{
   fWindow->SaveFrame(fMainFrame,fXML);
   fXML->WritePathFile(fXML->GetFullFileName());
}

//______________________________________________________________________________
Bool_t XMLToForm::GetBoolValue(const char* label)
{
   if (!strcmp(GetValue(label),"true"))
      return true;
   return false;
}

//______________________________________________________________________________
Int_t XMLToForm::GetIntValue(const char* label)
{
   char *cstop;
   return strtol(GetValue(label),&cstop,10);
}

//______________________________________________________________________________
double XMLToForm::GetDoubleValue(const char* label)
{
   char *cstop;
   return strtod(GetValue(label),&cstop);
}

//______________________________________________________________________________
Int_t XMLToForm::GetSelectedIndex(const char* label)
{
   return GetIndex(label);
}

//______________________________________________________________________________
Bool_t XMLToForm::IsChecked(const char* label)
{
   if (!strcmp(GetValue(label),"true"))
      return true;
   return false;
}

//______________________________________________________________________________
const char* XMLToForm::GetValue(const char* label)
{
   Int_t indx = 0;
   XMLToFormFrame *frame;
   if (!fWindow->SearchWidget(label,&frame,&indx,fMainFrame))
      return "";
   return frame->GetElementAt(indx)->GetValue().Data();
}

//______________________________________________________________________________
Int_t XMLToForm::GetIndex(const char* label)
{
   Int_t indx = 0;
   XMLToFormFrame *frame;
   if (!fWindow->SearchWidget(label,&frame,&indx,fMainFrame))
      return -1;
   return frame->GetElementAt(indx)->GetSelectedEntry();
}

//______________________________________________________________________________
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

//______________________________________________________________________________
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

//______________________________________________________________________________
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

//______________________________________________________________________________
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

//______________________________________________________________________________
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
