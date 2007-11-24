/********************************************************************

  ROMEBuildWriteCode.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <map>
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 4244)
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TTimeStamp.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include <Riostream.h>
#include "TArrayI.h"
#include "ROMEBuilder.h"
#include "ROMEXML.h"
#include "ROMEConfig.h"
#include "ROMEConfigParameter.h"

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteFillObjectStorageObject(ROMEString &buffer,const char *objectPointer,
                                                 const char *objectStoragePointer,const char *objectActivePointer,
                                                 bool bypass)
{
   buffer.AppendFormatted("   for (i = 0; i < kMaxSocketClients; i++) {\n");
   buffer.AppendFormatted("      if(!fNetFolderServer->GetAcceptedSockets(i) || (!%s && !fNetFolderServer->IsCopyAll())) continue;\n",
                          objectActivePointer);
   if (bypass) {
      buffer.AppendFormatted("      bypassOld = %s->CanBypassStreamer();\n",objectPointer);
      buffer.AppendFormatted("      bypassStorageOld = %s->CanBypassStreamer();\n",objectStoragePointer);
      buffer.AppendFormatted("      %s->BypassStreamer(kTRUE);\n",objectPointer);
      buffer.AppendFormatted("      %s->BypassStreamer(kTRUE);\n",objectStoragePointer);
   }
   buffer.AppendFormatted("      CopyTObjectWithStreamer(buffer,%s,%s);\n",objectPointer,objectStoragePointer);
   if (bypass) {
      buffer.AppendFormatted("      %s->BypassStreamer(bypassOld);\n",objectPointer);
      buffer.AppendFormatted("      %s->BypassStreamer(bypassStorageOld);\n",objectStoragePointer);
   }
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   }\n");
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::AddMenuItems(ROMEString &buffer, Int_t i, Int_t j, Int_t iHeredity, Int_t jHeredity, Int_t jOffset)
{
   Int_t k;
   for (k = 0; k < numOfMenuItem[i][j]; k++) {
      if (menuItemTitle[i][j][k] == LINE_TITLE) {
         buffer.AppendFormatted("   GetUserPopupMenuAt(%d)->AddSeparator();\n", jHeredity);
      } else if (menuItemChildMenuIndex[i][j][k]) {
         if (!AddMenuItems(buffer, i, menuItemChildMenuIndex[i][j][k],iHeredity,
                           jOffset+menuItemChildMenuIndex[i][j][k],jOffset+menuItemChildMenuIndex[i][j][k])) {
            return kFALSE;
         }
         buffer.AppendFormatted("   GetUserPopupMenuAt(%d)->AddPopup(\"%s\", GetUserPopupMenuAt(%d));\n",
                                jHeredity, menuTitle[i][menuItemChildMenuIndex[i][j][k]].Data(),
                                jOffset+menuItemChildMenuIndex[i][j][k]);
      } else {
         buffer.AppendFormatted("   GetUserPopupMenuAt(%d)->AddEntry(\"%s\", %sWindow::%s);\n",
                                jHeredity, menuItemTitle[i][j][k].Data(), shortCut.Data(), menuItemEnumName[i][j][k].Data());
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigToFormSubMethods(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,
                                                ROMEString tabPointer,ROMEString configPointer,int level,int tab)
{
   int i,j,histoDimension;
   Bool_t histosWritten,graphsWritten;
   ROMEConfigParameterGroup *subGroup;

   ROMEString sTab = "";
   for (i = 0; i < tab; i++) {
      sTab += "   ";
   }

   ROMEString newConfigPointer;
   ROMEString temp;
   ROMEString comment;
   ROMEConfigParameter *parameter;

   if (parGroup->GetGroupIdentifier() == "Histogram") {
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0, temp.Last('-'));
      histoDimension = 1;
      if (parGroup->GetInfo().Index("2") != -1)
         histoDimension = 2;
      if (parGroup->GetInfo().Index("3") != -1)
         histoDimension = 3;
      buffer.AppendFormatted("%sAddHisto(tempFrame[%d],%s,%d);\n",sTab.Data(),level,temp.Data(),histoDimension);
   } else if (parGroup->GetGroupIdentifier() == "Graph") {
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0, temp.Last('-'));
      buffer.AppendFormatted("%sAddGraph(tempFrame[%d],%s);\n",sTab.Data(),level,temp.Data());
   } else {
      for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
         parameter = parGroup->GetParameterAt(i);
         if (parameter->GetArraySize() == "1") {
            buffer.AppendFormatted("\n%s// %s%s\n",sTab.Data(),tabPointer.Data(),parameter->GetName());
            if (strlen(parameter->GetDeclaration())>0) {
               buffer.AppendFormatted("%s%s\n", sTab.Data(), parameter->GetDeclaration());
            }
            for (j = 0; j < parameter->GetNumberOfWriteLines(); j++) {
               buffer.AppendFormatted("%s%s\n", sTab.Data(), parameter->GetWriteLineAt(j));
            }
            if (parameter->GetNumberOfComboBoxEntries() > 0) {
               buffer.AppendFormatted("%sentries.RemoveAll();\n",sTab.Data());
            }
            for (j = 0; j < parameter->GetNumberOfComboBoxEntries(); j++) {
               buffer.AppendFormatted("%sentries.AddLast(\"%s\");\n", sTab.Data(), parameter->GetComboBoxEntryAt(j));
            }
            comment = ProcessCommentString(parameter->GetComment(),temp).Data();
            buffer.AppendFormatted("%scomment = \"\";\n", sTab.Data());
            if (comment.Length() > 0) {
               buffer.AppendFormatted("%sif (fCommentLevel >= %d) {\n", sTab.Data(), parameter->GetCommentLevel());
               buffer.AppendFormatted("%s   comment = \"%s\";\n", sTab.Data(),comment.Data());
               buffer.AppendFormatted("%s}\n", sTab.Data());
            }
            buffer.AppendFormatted("%stempFrame[%d]->AddElement(new XMLToFormElement(\"%s\",\"%s\",writeString.Data(),\"\", 0, &entries,comment.Data()));\n",
                                   sTab.Data(), level,parameter->GetWidgetType().Data(), parameter->GetName());
         }
      }
   }

   histosWritten = false;
   graphsWritten= false;
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      // Special case histogram
      if (subGroup->GetGroupIdentifier()=="Histogram") {
         if (!histosWritten) {
            histosWritten = true;
            buffer.AppendFormatted("\n%s// %sHistograms\n",sTab.Data(),tabPointer.Data());
            buffer.AppendFormatted("%sfor (i=0;i<%sTask->GetNumberOfHistos();i++) {\n",sTab.Data(),parGroup->GetGroupName().Data());
            comment = ProcessCommentString(subGroup->GetComment(),temp).Data();
            buffer.AppendFormatted("%s   comment = \"\";\n",sTab.Data());
            if (comment.Length() > 0) {
               buffer.AppendFormatted("%s   if (fCommentLevel >= %d) {\n",sTab.Data(),
                                      subGroup->GetCommentLevel());
               buffer.AppendFormatted("%s      comment = \"%s\";\n",sTab.Data(),comment.Data());
               buffer.AppendFormatted("%s   }\n",sTab.Data());
            }
            buffer.AppendFormatted("%s   tempFrame[%d]->AddSubFrame(new XMLToFormFrame(tempFrame[%d],%sTask->GetHistoNameAt(i)->Data(),\"\",true,XMLToFormFrame::kListTreeItem,true,%d+i,comment.Data()));\n",
                                   sTab.Data(),level,level,parGroup->GetGroupName().Data(),i);
            buffer.AppendFormatted("%s   AddHisto(tempFrame[%d]->GetSubFrameAt(%d+i),%sTask->GetHistoParameterAt(i),%sTask->GetHistoDimensionAt(i));\n",sTab.Data(),level,i,parGroup->GetGroupName().Data(),parGroup->GetGroupName().Data());
            buffer.AppendFormatted("%s}\n",sTab.Data());
         }
         continue;
      }
      // Special case graph
      if (subGroup->GetGroupIdentifier()=="Graph") {
         if (!graphsWritten) {
            graphsWritten = true;
            buffer.AppendFormatted("\n%s// %sGraphs\n",sTab.Data(),tabPointer.Data());
            buffer.AppendFormatted("%sfor (i=0;i<%sTask->GetNumberOfGraphs();i++) {\n",sTab.Data(),parGroup->GetGroupName().Data());
            comment = ProcessCommentString(subGroup->GetComment(),temp).Data();
            buffer.AppendFormatted("%s   comment = \"\";\n",sTab.Data());
            if (comment.Length() > 0) {
               buffer.AppendFormatted("%s   if (fCommentLevel >= %d) {\n",sTab.Data(),
                                      subGroup->GetCommentLevel());
               buffer.AppendFormatted("%s      comment = \"%s\";\n",sTab.Data(),comment.Data());
               buffer.AppendFormatted("%s   }\n", sTab.Data());
            }
            buffer.AppendFormatted("%s   tempFrame[%d]->AddSubFrame(new XMLToFormFrame(tempFrame[%d],%sTask->GetGraphNameAt(i)->Data(),\"\",true,XMLToFormFrame::kListTreeItem,true,%d+i,comment.Data()));\n",
                                   sTab.Data(),level,level,parGroup->GetGroupName().Data(),i);
            buffer.AppendFormatted("%s   AddGraph(tempFrame[%d]->GetSubFrameAt(%d+i),%sTask->GetGraphParameterAt(i));\n",sTab.Data(),level,i,parGroup->GetGroupName().Data());
            buffer.AppendFormatted("%s}\n",sTab.Data());
         }
         continue;
      }
      // normal case
      if (level == 0 || (level == 1 && parGroup->GetGroupName() == "Common") ||
          (level == 2 && parGroup->GetGroupName() == "Folders")) {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sConfigToForm::Add%sFrame(XMLToFormFrame *frame)\n",shortCut.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   int ind,dimension;\n");
         buffer.AppendFormatted("   ind = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   bool writeFlag;\n");
         buffer.AppendFormatted("   writeFlag = true;\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString comment = \"\";\n");
         buffer.AppendFormatted("   ROMEString str = \"\";\n");
         buffer.AppendFormatted("   ROMEString tmp;\n");
         buffer.AppendFormatted("   tmp = \"\";\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString subStr = \"\";\n");
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEString writeString;\n");
         buffer.AppendFormatted("   ROMEStrArray entries;\n");
         buffer.AppendFormatted("   XMLToFormFrame *tempFrame[100];\n");
         buffer.AppendFormatted("   int nTabs;\n");
         buffer.AppendFormatted("   nTabs = 0;\n");
         buffer.AppendFormatted("   int i,ii[100];\n");
         buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   dimension = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   tempFrame[%d] = frame;\n",level);
      } else {
         buffer.AppendFormatted("\n%s// %s%s\n",sTab.Data(),tabPointer.Data(),
                                subGroup->GetGroupName().Data());
      }
      // Special case task
      if (subGroup->GetGroupIdentifier()=="Task") {
         buffer.AppendFormatted("%s%sT%s_Base* %sTask;\n",sTab.Data(),shortCut.Data(),subGroup->GetTagName().Data(),subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s%sTask = gAnalyzer->Get%sTaskBase();\n",sTab.Data(),subGroup->GetGroupName().Data(),subGroup->GetGroupName().Data());
      }
      comment = ProcessCommentString(subGroup->GetComment(),temp).Data();
      buffer.AppendFormatted("%scomment = \"\";\n",sTab.Data());
      if (comment.Length() > 0) {
         buffer.AppendFormatted("%sif (fCommentLevel >= %d) {\n",sTab.Data(),
                                subGroup->GetCommentLevel());
         buffer.AppendFormatted("%s   comment = \"%s\";\n",sTab.Data(),comment.Data());
         buffer.AppendFormatted("%s}\n", sTab.Data());
      }
      if (subGroup->GetArraySize() == "1" || subGroup->GetArraySize() == "unknown")
         buffer.AppendFormatted("%stempFrame[%d]->AddSubFrame(new XMLToFormFrame(tempFrame[%d],\"%s\",\"\",true,XMLToFormFrame::kListTreeItem,true,%d,comment.Data()));\n",
                                sTab.Data(),level,level,subGroup->GetGroupName().Data(),i);
      newConfigPointer.SetFormatted("%sf%s->",configPointer.Data(),subGroup->GetGroupName().Data());
      if (!(level == 0 && subGroup->GetGroupName() == "Common") &&
          !(level == 1 && subGroup->GetGroupName() == "Folders")) {
         if (subGroup->GetArraySize() == "1") {
            buffer.AppendFormatted("%stempFrame[%d] = tempFrame[%d]->GetSubFrameAt(%d);\n",sTab.Data(),level + 1,level,
                                   i);
            WriteConfigToFormSubMethods(buffer,subGroup,
                                        tabPointer+subGroup->GetGroupName().Data()+"/",
                                        newConfigPointer.Data(),level + 1,tab);
         } else if (subGroup->GetArraySize() == "unknown") {
            buffer.AppendFormatted("%sfor (i = 0; i < static_cast<%sConfig*>(gAnalyzer->GetConfiguration())->fConfigData[0]->%sf%sArraySize; i++) {\n",
                                   sTab.Data(),shortCut.Data(),configPointer.Data(),
                                   subGroup->GetGroupName().Data());
            buffer.AppendFormatted("%s   str.SetFormatted(\"%s %%d\",i);\n",sTab.Data(),
                                   subGroup->GetGroupName().Data());
            comment = ProcessCommentString(subGroup->GetComment(),temp).Data();
            buffer.AppendFormatted("%s   comment = \"\";\n",sTab.Data());
            if (comment.Length() > 0) {
               buffer.AppendFormatted("%s   if (fCommentLevel >= %d) {\n",sTab.Data(),
                                      subGroup->GetCommentLevel());
               buffer.AppendFormatted("%s      comment = \"%s\";\n",sTab.Data(),comment.Data());
               buffer.AppendFormatted("%s   }\n", sTab.Data());
            }
            buffer.AppendFormatted("%s   tempFrame[%d]->GetSubFrameAt(%d)->AddSubFrame(new XMLToFormFrame(tempFrame[%d]->GetSubFrameAt(%d),str.Data(),\"\",true,XMLToFormFrame::kListTreeItem,true, 0, comment.Data()));\n",
                                   sTab.Data(),level,i,level,i);
            buffer.AppendFormatted("%s   tempFrame[%d] = tempFrame[%d]->GetSubFrameAt(%d)->GetSubFrameAt(i);\n",
                                   sTab.Data(),level + 1,level,i);
            WriteConfigToFormSubMethods(buffer,subGroup,
                                        tabPointer+subGroup->GetGroupName().Data()+"/",
                                        newConfigPointer.Data(),level + 1,tab + 1);
            buffer.AppendFormatted("%s}\n",sTab.Data());
         } else {
            buffer.AppendFormatted("%sfor (ii[%d] = 0; ii[%d] < %s; ii[%d]++) {\n",sTab.Data(),
                                   subGroup->GetHierarchyLevel(),
                                   subGroup->GetHierarchyLevel(),
                                   subGroup->GetArraySize().Data(),
                                   subGroup->GetHierarchyLevel());
            buffer.AppendFormatted("%s   str.SetFormatted(\"%s %%d\",ii[%d]);\n",sTab.Data(),
                                   subGroup->GetGroupName().Data(),
                                   subGroup->GetHierarchyLevel());
            comment = ProcessCommentString(subGroup->GetComment(),temp).Data();
            buffer.AppendFormatted("%s   comment = \"\";\n",sTab.Data());
            if (comment.Length() > 0) {
               buffer.AppendFormatted("%s   if (fCommentLevel >= %d) {\n",sTab.Data(),
                                      subGroup->GetCommentLevel());
               buffer.AppendFormatted("%s      comment = \"%s\";\n",sTab.Data(),comment.Data());
               buffer.AppendFormatted("%s   }\n", sTab.Data());
            }
            buffer.AppendFormatted("%s   tempFrame[%d]->AddSubFrame(new XMLToFormFrame(tempFrame[%d],str.Data(),\"\",true,XMLToFormFrame::kListTreeItem,true, 0, comment.Data()));\n",
                                   sTab.Data(),level,level);
            buffer.AppendFormatted("%s   tempFrame[%d] = tempFrame[%d]->GetSubFrameAt(ii[%d]);\n",sTab.Data(),
                                   level + 1, level,subGroup->GetHierarchyLevel());
            WriteConfigToFormSubMethods(buffer,subGroup,
                                        tabPointer+subGroup->GetGroupName().Data()+"/",
                                        newConfigPointer.Data(),level + 1,tab + 1);
            buffer.AppendFormatted("%s}\n",sTab.Data());
         }
      }
      if (level == 0 || (level == 1 && parGroup->GetGroupName() == "Common") ||
          (level == 2 && parGroup->GetGroupName() == "Folders")) {
         if ((level == 0 && subGroup->GetGroupName() == "Common") ||
             (level == 1 && subGroup->GetGroupName() == "Folders")) {
            for (j = 0; j < subGroup->GetNumberOfSubGroups(); j++)
               buffer.AppendFormatted("%sAdd%sFrame(tempFrame[%d]->GetSubFrameAt(%d));\n",sTab.Data(),
                                      subGroup->GetSubGroupAt(j)->GetGroupName().Data(),level,i);
         }
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         if ((level == 0 && subGroup->GetGroupName() == "Common") ||
             (level == 1 && subGroup->GetGroupName() == "Folders")) {
            WriteConfigToFormSubMethods(buffer,subGroup,
                                        tabPointer+subGroup->GetGroupName().Data()+"/",
                                        newConfigPointer.Data(),level + 1,tab);
         }
      }
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigToFormSave(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString pointer,
                                          ROMEString tabPointer,ROMEString configPointer,int level,int tab,
                                          ROMEString indexes)
{
   int i,j;
   ROMEString sTab = "";
   for (i = 0; i < tab; i++) {
      sTab += "   ";
   }

   ROMEString newPointer;
   ROMEString newConfigPointer;
   ROMEString newIndexes;
   ROMEString temp;
   ROMEString temp2;
   ROMEConfigParameter* parameter;
   ROMEConfigParameterGroup *subGroup;
   Int_t histoDimension; 

   if (parGroup->GetGroupIdentifier() == "Histogram") {
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0, temp.Last('-'));
      temp2 = "static_cast<"+shortCut+"Config*>(fConfiguration)->fConfigData[0]->"+configPointer;
      temp2 = temp2(0, temp2.Last('-'));
      histoDimension = 1;
      if (parGroup->GetInfo().Index("2") != -1)
         histoDimension = 2;
      if (parGroup->GetInfo().Index("3") != -1)
         histoDimension = 3;
      buffer.AppendFormatted("%s   SaveHisto(dialog,\"%s\",%s,%s,%d);\n",sTab.Data(),pointer.Data(),temp.Data(),
                             temp2.Data(),histoDimension);
   } else {
      for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
         parameter = parGroup->GetParameterAt(i);
         if (parameter->GetArraySize() == "1") {
            buffer.AppendFormatted("\n%s   // %s%s\n",sTab.Data(),tabPointer.Data(), parameter->GetName());
            if (strlen(parameter->GetDeclaration())>0) {
               buffer.AppendFormatted("%s   %s\n", sTab.Data(), parameter->GetDeclaration());
            }
            if (parGroup->GetArraySize() == "unknown") {
               buffer.AppendFormatted("%s   str.SetFormatted(\"%s %%d/%s\"%s,i);\n",sTab.Data(),pointer.Data(),
                                      parameter->GetName(),indexes.Data());
               buffer.AppendFormatted("%s   str = dialog->GetValue(str.Data());\n",sTab.Data());
            } else {
               if (indexes.Length() == 0) {
                  buffer.AppendFormatted("%s   str = dialog->GetValue(\"%s%s\");\n",sTab.Data(),pointer.Data(),
                                         parameter->GetName());
               } else {
                  buffer.AppendFormatted("%s   str.SetFormatted(\"%s%s\"%s);\n",sTab.Data(),pointer.Data(),
                                         parameter->GetName(),indexes.Data());
                  buffer.AppendFormatted("%s   str = dialog->GetValue(str.Data());\n",sTab.Data());
               }
            }
            if (!parameter->IsWriteLinesAlways()) {
               for (j = 0; j < parameter->GetNumberOfWriteLines(); j++) {
                  buffer.AppendFormatted("%s   %s\n",sTab.Data(),parameter->GetWriteLineAt(j));
               }
               buffer.AppendFormatted("%s   if (str != writeString) {\n",sTab.Data());
               buffer.AppendFormatted("%s      static_cast<%sConfig*>(fConfiguration)->fConfigData[0]->%sf%sModified = true;\n",
                                      sTab.Data(),shortCut.Data(),configPointer.Data(),
                                      parameter->GetName());
               buffer.AppendFormatted("%s      static_cast<%sConfig*>(fConfiguration)->fConfigData[0]->%sf%s = str;\n",
                                      sTab.Data(), shortCut.Data(),configPointer.Data(),
                                      parameter->GetName());
               for (j = 0; j < parameter->GetNumberOfSetLines(); j++) {
                  temp = parameter->GetSetLineAt(j);
                  temp.ReplaceAll("##","str");
                  temp.ReplaceAll("configData","static_cast<"+shortCut+"Config*>(fConfiguration)->fConfigData[0]");
                  buffer.AppendFormatted("%s      %s\n",sTab.Data(),temp.Data());
               }
               buffer.AppendFormatted("%s   }\n",sTab.Data());
            } else {
               for (j = 0; j < parameter->GetNumberOfSetLines(); j++) {
                  temp = parameter->GetSetLineAt(j);
                  temp.ReplaceAll("##","str");
                  temp.ReplaceAll("configData","static_cast<"+shortCut+"Config*>(fConfiguration)->fConfigData[0]");
                  buffer.AppendFormatted("%s   %s\n",sTab.Data(),temp.Data());
               }
            }
         }
      }
   }

   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      if (level == 0 || (level == 1 && parGroup->GetGroupName() == "Common") ||
          (level == 2 && parGroup->GetGroupName() == "Folders")) {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %sAnalyzer::Save%s(%sConfigToForm *dialog)\n",shortCut.Data(),
                                subGroup->GetGroupName().Data(),shortCut.Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   int i,ii[100];\n");
         buffer.AppendFormatted("   i = 0; ii[0] = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   bool writeFlag;\n");
         buffer.AppendFormatted("   writeFlag = true;\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString str;\n");
         buffer.AppendFormatted("   ROMEString tmp;\n");
         buffer.AppendFormatted("   tmp = \"\";\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString subStr = \"\";\n");
         buffer.AppendFormatted("   ROMEString writeString;\n");
         buffer.AppendFormatted("   ROMEString path = \"\";\n");
         buffer.AppendFormatted("   ROMEString subPath = \"\";\n");
         buffer.AppendFormatted("   int ind;\n");
         buffer.AppendFormatted("   ind = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   char *cstop;\n");
         buffer.AppendFormatted("   cstop = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   int inttmp;\n");
         buffer.AppendFormatted("   inttmp = 0;\n"); // to suppress unused warning
         tab = 0;
         sTab = "";
      }
      newIndexes = indexes;
      newConfigPointer.SetFormatted("%sf%s->",configPointer.Data(),subGroup->GetGroupName().Data());
      newPointer.SetFormatted("%s%s/",pointer.Data(),subGroup->GetGroupName().Data());
      if (!(level == 0 && subGroup->GetGroupName() == "Common") &&
          !(level == 1 && subGroup->GetGroupName() == "Folders")) {
         if (subGroup->GetArraySize() == "1") {
            newPointer.SetFormatted("%s%s/",pointer.Data(),subGroup->GetGroupName().Data());
            WriteConfigToFormSave(buffer,subGroup,newPointer.Data(),
                                  tabPointer+subGroup->GetGroupName().Data()+"/",
                                  newConfigPointer.Data(),level + 1,tab,newIndexes);
         } else if (subGroup->GetArraySize() == "unknown") {
            buffer.AppendFormatted("%s   for (i = 0; i < static_cast<%sConfig*>(gAnalyzer->GetConfiguration())->fConfigData[0]->%sf%sArraySize; i++) {\n",
                                   sTab.Data(),shortCut.Data(),configPointer.Data(),
                                   subGroup->GetGroupName().Data());
            newPointer.SetFormatted("%s%s/%s",pointer.Data(),subGroup->GetGroupName().Data(),
                                    subGroup->GetGroupName().Data());
            WriteConfigToFormSave(buffer,subGroup,newPointer.Data(),
                                  tabPointer+subGroup->GetGroupName().Data()+"/",
                                  newConfigPointer.Data(),level + 1,tab + 1,newIndexes);
            buffer.AppendFormatted("%s   }\n",sTab.Data());
         } else {
            buffer.AppendFormatted("%s   for (ii[%d] = 0; ii[%d] < %s; ii[%d]++) {\n",sTab.Data(),
                                   subGroup->GetHierarchyLevel(),
                                   subGroup->GetHierarchyLevel(),
                                   subGroup->GetArraySize().Data(),
                                   subGroup->GetHierarchyLevel());
            newPointer.SetFormatted("%s%s %%d/",pointer.Data(),subGroup->GetGroupName().Data());
            newIndexes.AppendFormatted(",ii[%d]",subGroup->GetHierarchyLevel());
            WriteConfigToFormSave(buffer,subGroup,newPointer.Data(),
                                  tabPointer+subGroup->GetGroupName().Data()+"/",
                                  newConfigPointer.Data(),level + 1,tab + 1,newIndexes);
            buffer.AppendFormatted("%s   }\n",sTab.Data());
         }
      }
      if (level == 0 || (level == 1 && parGroup->GetGroupName() == "Common") ||
          (level == 2 && parGroup->GetGroupName() == "Folders")) {
         if ((level == 0 && subGroup->GetGroupName() == "Common") ||
             (level == 1 && subGroup->GetGroupName() == "Folders")) {
            for (j = 0; j < subGroup->GetNumberOfSubGroups(); j++) {
               buffer.AppendFormatted("%s   if (!Save%s(dialog)) return false;\n",sTab.Data(),
                                      subGroup->GetSubGroupAt(j)->GetGroupName().Data());
            }
         }
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("   WarningSuppression(dialog);\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         if ((level == 0 && subGroup->GetGroupName() == "Common") ||
             (level == 1 && subGroup->GetGroupName() == "Folders")) {
            WriteConfigToFormSave(buffer,subGroup,newPointer.Data(),
                                  tabPointer+subGroup->GetGroupName().Data()+"/",
                                  newConfigPointer.Data(),level + 1,tab,newIndexes);
         }
      }
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::AddConfigParameters()
{
   int i,j;
   int nIndex = 1;
   int *indx = new int[nIndex];
   ROMEString str;
   ROMEString tmp;
   for (i = 0; i < nIndex; i++)
      indx[i] = -1;
   maxConfigParameterHierarchyLevel = 0;

   histoParameters = new ROMEStrArray(19);
   histoParameterTypes = new ROMEStrArray(19);
   histoParameterWidgetTypes = new ROMEStrArray(19);
   histoParameters->AddLast("Active");
   histoParameterTypes->AddLast("bool");
   histoParameterWidgetTypes->AddLast("CheckButton");
   histoParameters->AddLast("Write");
   histoParameterTypes->AddLast("bool");
   histoParameterWidgetTypes->AddLast("CheckButton");
   histoParameters->AddLast("Title");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("FolderTitle");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ArraySize");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ArrayStartIndex");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("XLabel");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("YLabel");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ZLabel");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("XNbins");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Xmin");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Xmax");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("YNbins");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Ymin");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Ymax");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ZNbins");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Zmin");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Zmax");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Accumulate");
   histoParameterTypes->AddLast("bool");
   histoParameterWidgetTypes->AddLast("CheckButton");

   graphParameters = new ROMEStrArray(9);
   graphParameterTypes = new ROMEStrArray(9);
   graphParameterWidgetTypes = new ROMEStrArray(9);
   graphParameters->AddLast("Active");
   graphParameterTypes->AddLast("bool");
   graphParameterWidgetTypes->AddLast("CheckButton");
   graphParameters->AddLast("Write");
   graphParameterTypes->AddLast("bool");
   graphParameterWidgetTypes->AddLast("CheckButton");
   graphParameters->AddLast("Title");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("FolderTitle");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("ArraySize");
   graphParameterTypes->AddLast("%d");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("ArrayStartIndex");
   graphParameterTypes->AddLast("%d");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("XLabel");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("YLabel");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("ZLabel");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Xmin");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Xmax");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Ymin");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Ymax");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Zmin");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Zmax");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");

   mainParGroup = new ROMEConfigParameterGroup("ConfigData");

   // Modes
   {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Modes");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Modes/AnalyzingMode
      subGroup->AddParameter(new ROMEConfigParameter("AnalyzingMode","1","ComboBox"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->isOnline() && !gAnalyzer->isOffline()) {");
      subGroup->GetLastParameter()->AddSetLine("   if (## == \"online\") {");
      subGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetOnline();");
      subGroup->GetLastParameter()->AddSetLine("   } else {");
      subGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetOffline();");
      subGroup->GetLastParameter()->AddSetLine("   }");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddSetLine("if (## == \"online\") {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetConfigOnline();");
      subGroup->GetLastParameter()->AddSetLine("} else {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetConfigOffline();");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->ConfigIsOnline()) {");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"online\";");
      subGroup->GetLastParameter()->AddWriteLine("} else {");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"offline\";");
      subGroup->GetLastParameter()->AddWriteLine("}");

      subGroup->GetLastParameter()->AddComboBoxEntry("offline");
      subGroup->GetLastParameter()->AddComboBoxEntry("online");

      // Modes/DAQSystem
      subGroup->AddParameter(new ROMEConfigParameter("DAQSystem","1","ComboBox"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (false) {}");
      for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
         str = daqNameArray->At(i);
         str.ToLower();
         subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"%s\",TString::kIgnoreCase)) {",str.Data());
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->Set%sDAQ(new %s%sDAQ());",daqNameArray->At(i).Data(),
                                                  daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(gAnalyzer->Get%sDAQ());",
                                                  daqNameArray->At(i).Data());
         subGroup->GetLastParameter()->AddSetLine("}");
      }
      subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"none\",TString::kIgnoreCase) || ##.Length() == 0) {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(new ROMENoDAQSystem());");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddComboBoxEntry("none");
      for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
         str = daqNameArray->At(i);
         str.ToLower();
         subGroup->GetLastParameter()->AddComboBoxEntry(str.Data());
      }
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isActiveDAQSet()) {");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetActiveDAQ()->GetName();");
      subGroup->GetLastParameter()->AddWriteLine("} else {");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"none\";");
      subGroup->GetLastParameter()->AddWriteLine("}");
#if 0
      // batch mode does not work fully with configuration file,
      // because we have to know before constructor of analyzer
      // comment out for the moment.
      // Modes/BatchMode
      subGroup->AddParameter(new ROMEConfigParameter("BatchMode","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetBatchMode(## == \"true\");");
      subGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isBatchMode()?1:0];");
#endif
   }

   // Offline
   {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Offline");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // RunNumbers
      subGroup->AddParameter(new ROMEConfigParameter("RunNumbers"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("configData->fLastRunNumberIndex = 0;");
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("gAnalyzer->DecodeNumbers(configData->fOffline->fRunNumbers,configData->fRunNumberArray);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetRunNumberStringOriginal();");
      // EventNumbers
      subGroup->AddParameter(new ROMEConfigParameter("EventNumbers"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetEventNumbers(##);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetEventNumberStringOriginal();");
      // InputFileNames
      subGroup->AddParameter(new ROMEConfigParameter("InputFileNames"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("configData->fLastInputFileNameIndex = 0;");
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("gAnalyzer->DecodeInputFileNames(configData->fOffline->fInputFileNames,configData->fInputFileNameArray);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetInputFileNamesStringOriginal();");
   }

   // Online
   {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Online");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Host
      subGroup->AddParameter(new ROMEConfigParameter("Host"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineHost(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineHost();");
      // Experiment
      subGroup->AddParameter(new ROMEConfigParameter("Experiment"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineExperiment(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineExperiment();");
      // AnalyzerName
      subGroup->AddParameter(new ROMEConfigParameter("AnalyzerName"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##.Length())");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetOnlineAnalyzerName(##.Data());");
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineAnalyzerName(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineAnalyzerName();");
      // Memory Buffer
      subGroup->AddParameter(new ROMEConfigParameter("MemoryBuffer"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##.Length())");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetOnlineMemoryBuffer(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineMemoryBuffer();");
   }

   // Paths
   {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Paths");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // InputFilePath
      subGroup->AddParameter(new ROMEConfigParameter("InputFilePath"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##[##.Length() - 1] != '/' && ##[##.Length() - 1] != '\\\\')");
      subGroup->GetLastParameter()->AddSetLine("   ##.Append(\"/\");");
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetInputDir(##);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetInputDir();");
      // OutputFilePath
      subGroup->AddParameter(new ROMEConfigParameter("OutputFilePath"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##[##.Length() - 1] != '/' && ##[##.Length() - 1] != '\\\\')");
      subGroup->GetLastParameter()->AddSetLine("   ##.Append(\"/\");");
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOutputDir(##);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOutputDir();");
      // OutputFileOption
      subGroup->AddParameter(new ROMEConfigParameter("OutputFileOption","1","ComboBox"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOutputFileOption(##);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOutputFileOption();");
      subGroup->GetLastParameter()->AddComboBoxEntry("RECREATE");
      subGroup->GetLastParameter()->AddComboBoxEntry("UPDATE");
      subGroup->GetLastParameter()->AddComboBoxEntry("NUMBERED");

      // MakeOutputDirectory
      subGroup->AddParameter(new ROMEConfigParameter("MakeOutputDirectory","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetMakeOutputDirectory(## == \"true\");");
      subGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isMakeOutputDirectory()?1:0];");
   }

   // Common
   {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Common");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Settings
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("Settings");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // VerboseLevel
         subSubGroup->AddParameter(new ROMEConfigParameter("VerboseLevel","1","ComboBox"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("inttmp = strtol(##,&cstop, 10);");
         subSubGroup->GetLastParameter()->AddSetLine("if (## == \"mute\" || ((!cstop || !strlen(cstop)) && inttmp == ROMEPrint::kMute)) {");
         subSubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kMute);");
         subSubGroup->GetLastParameter()->AddSetLine("} else if (## == \"error\" || ((!cstop || !strlen(cstop)) && inttmp == ROMEPrint::kError)) {");
         subSubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kError);");
         subSubGroup->GetLastParameter()->AddSetLine("} else if (## == \"warning\" || ((!cstop || !strlen(cstop)) && inttmp == ROMEPrint::kWarning)) {");
         subSubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kWarning);");
         subSubGroup->GetLastParameter()->AddSetLine("} else if (## == \"normal\" || ((!cstop || !strlen(cstop)) && inttmp == ROMEPrint::kNormal)) {");
         subSubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kNormal);");
         subSubGroup->GetLastParameter()->AddSetLine("} else if (## == \"verbose\" || ((!cstop || !strlen(cstop)) && inttmp == ROMEPrint::kVerbose)) {");
         subSubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kVerbose);");
         subSubGroup->GetLastParameter()->AddSetLine("} else if (## == \"debug\" || ((!cstop || !strlen(cstop)) && inttmp == ROMEPrint::kDebug)) {");
         subSubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kDebug);");
         subSubGroup->GetLastParameter()->AddSetLine("}");
         subSubGroup->GetLastParameter()->AddWriteLine("if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kMute) {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"mute\";");
         subSubGroup->GetLastParameter()->AddWriteLine("} else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kError) {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"error\";");
         subSubGroup->GetLastParameter()->AddWriteLine("} else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kWarning) {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"warning\";");
         subSubGroup->GetLastParameter()->AddWriteLine("} else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kNormal) {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"normal\";");
         subSubGroup->GetLastParameter()->AddWriteLine("} else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kVerbose) {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"verbose\";");
         subSubGroup->GetLastParameter()->AddWriteLine("} else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kDebug) {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"debug\";");
         subSubGroup->GetLastParameter()->AddWriteLine("} else {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"normal\";");
         subSubGroup->GetLastParameter()->AddWriteLine("}");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("mute");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("error");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("warning");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("normal");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("verbose");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("debug");
         // ReportMaxCount
         subSubGroup->AddParameter(new ROMEConfigParameter("ReportMaxCount"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("ROMEPrint::SetReportMaxCount(strtol(##,&cstop, 10));");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\", ROMEPrint::GetReportMaxCount());");
         // ConfigCommentLevel
         subSubGroup->AddParameter(new ROMEConfigParameter("ConfigCommentLevel","1","ComboBox"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetConfiguration()->SetCommentLevel(strtol(##,&cstop, 10));");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\", gAnalyzer->GetConfiguration()->GetCommentLevel());");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("0");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("1");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("2");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("3");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("4");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("5");
         // QuitMode
         subSubGroup->AddParameter(new ROMEConfigParameter("QuitMode","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetQuitMode(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isQuitMode()?1:0];");
         // ShowSplashScreen
         subSubGroup->AddParameter(new ROMEConfigParameter("ShowSplashScreen","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSplashScreen(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isSplashScreen()?1:0];");
         // GraphicalConfigEdit
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphicalConfigEdit","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetGraphicalConfigEdit(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isGraphicalConfigEdit()?1:0];");
         // PreserveConfig
         subSubGroup->AddParameter(new ROMEConfigParameter("PreserveConfig","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetPreserveConfig(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isPreserveConfig()?1:0];");
      }

      // Floating point exception trap
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("FloatingPointExceptionTrap");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // Invalid
         subSubGroup->AddParameter(new ROMEConfigParameter("Invalid","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->SetFPEInvalid(## == \"true\");");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->EnableFPETrap();");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetApplication()->GetFPEInvalid()?1:0];");
         // DivByZero
         subSubGroup->AddParameter(new ROMEConfigParameter("DivByZero","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->SetFPEDivByZero(## == \"true\");");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->EnableFPETrap();");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetApplication()->GetFPEDivByZero()?1:0];");
         // Overflow
         subSubGroup->AddParameter(new ROMEConfigParameter("Overflow","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->SetFPEOverflow(## == \"true\");");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->EnableFPETrap();");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetApplication()->GetFPEOverflow()?1:0];");
         // Underflow
         subSubGroup->AddParameter(new ROMEConfigParameter("Underflow","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->SetFPEUnderflow(## == \"true\");");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->EnableFPETrap();");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetApplication()->GetFPEUnderflow()?1:0];");
         // Inexact
         subSubGroup->AddParameter(new ROMEConfigParameter("Inexact","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->SetFPEInexact(## == \"true\");");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetApplication()->EnableFPETrap();");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetApplication()->GetFPEInexact()?1:0];");
      }

      // SocketServer
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("SocketServer");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // Active
         subSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketServerActive(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isSocketServerActive()?1:0];");
         // PortNumber
         subSubGroup->AddParameter(new ROMEConfigParameter("PortNumber"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketServerPortNumber(##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetSocketServerPortNumber());");
      }
      // DataBases
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("DataBase","unknown");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subSubGroup->AddReadGroupArrayInitLine("gAnalyzer->InitDataBases(configData->fCommon->fDataBaseArraySize);");
         subGroup->AddSubGroup(subSubGroup);
         // DataBase/Name
         subSubGroup->AddParameter(new ROMEConfigParameter("Name"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetDataBaseName(i,##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBaseName(i);");
         // DataBase/Connection
         subSubGroup->AddParameter(new ROMEConfigParameter("Connection"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetDataBaseConnection(i,##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBaseConnection(i);");
         // DataBase/Type
         subSubGroup->AddParameter(new ROMEConfigParameter("Type","1","ComboBox"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("if (## == \"sql\") {");
         if (sql) {
            subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMESQLDataBase());");
            subSubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",gAnalyzer->GetDataBaseConnection(i))) {");
            subSubGroup->GetLastParameter()->AddSetLine("      return false;");
            subSubGroup->GetLastParameter()->AddSetLine("   }");
         } else {
            subSubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::Error(\"%%s is not linked with sql support.\\n\", gAnalyzer->GetProgramName())\n;");
            subSubGroup->GetLastParameter()->AddSetLine("   return false;");
         }
         subSubGroup->GetLastParameter()->AddSetLine("}");
         subSubGroup->GetLastParameter()->AddSetLine("else if (## == \"none\" ||");
         subSubGroup->GetLastParameter()->AddSetLine("         ## == \"\") {");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMENoDataBase());");
         subSubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",\"\")) {");
         subSubGroup->GetLastParameter()->AddSetLine("      return false;");
         subSubGroup->GetLastParameter()->AddSetLine("   }");
         subSubGroup->GetLastParameter()->AddSetLine("}");
         subSubGroup->GetLastParameter()->AddSetLine("else if (## == \"xml\") {");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMEXMLDataBase());");
         subSubGroup->GetLastParameter()->AddSetLine("   str = gAnalyzer->GetDataBaseConnection(i);");
         subSubGroup->GetLastParameter()->AddSetLine("   if ((ind=str.Index(\";\", 1, 0, TString::kExact)) == -1) {");
         subSubGroup->GetLastParameter()->AddSetLine("      ROMEPrint::Error(\"Invalid database connection\\n\");");
         subSubGroup->GetLastParameter()->AddSetLine("      return false;");
         subSubGroup->GetLastParameter()->AddSetLine("   }");
         subSubGroup->GetLastParameter()->AddSetLine("   path = str(0, ind);");
         subSubGroup->GetLastParameter()->AddSetLine("   if (path[path.Length() - 1] != '/' && path[path.Length() - 1] != '\\\\') {");
         subSubGroup->GetLastParameter()->AddSetLine("      path += \"/\";");
         subSubGroup->GetLastParameter()->AddSetLine("   }");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBaseDir(i,path.Data());");
         subSubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),gAnalyzer->GetDataBaseDir(i),static_cast<TString>(str(ind + 1,str.Length()-ind - 1)).Data())) {");
         subSubGroup->GetLastParameter()->AddSetLine("      return false;");
         subSubGroup->GetLastParameter()->AddSetLine("   }");
         subSubGroup->GetLastParameter()->AddSetLine("}");
         subSubGroup->GetLastParameter()->AddSetLine("else if (## == \"text\") {");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMETextDataBase());");
         subSubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),gAnalyzer->GetDataBaseConnection(i),\"\")) {");
         subSubGroup->GetLastParameter()->AddSetLine("      return false;");
         subSubGroup->GetLastParameter()->AddSetLine("   }");
         subSubGroup->GetLastParameter()->AddSetLine("}");
         subSubGroup->GetLastParameter()->AddSetLine("if (## == \"odb\") {");
         subSubGroup->GetLastParameter()->AddSetLine("   if (gAnalyzer->isOffline()) {");
         subSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetDataBase(i,new ROMEODBOfflineDataBase());");
         subSubGroup->GetLastParameter()->AddSetLine("   } else {");
         subSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetDataBase(i,new ROMEODBOnlineDataBase());");
         subSubGroup->GetLastParameter()->AddSetLine("   }");
         subSubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",\"\")) {");
         subSubGroup->GetLastParameter()->AddSetLine("      return false;");
         subSubGroup->GetLastParameter()->AddSetLine("   }");
         subSubGroup->GetLastParameter()->AddSetLine("}");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("none");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("sql");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("xml");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("text");
         subSubGroup->GetLastParameter()->AddComboBoxEntry("odb");
         for (i = 0; i < numOfDB; i++) {
            subSubGroup->GetLastParameter()->AddSetLine("else if (## == \"%s\") {\n",dbName[i].Data());
            subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new %s%sDataBase());\n",shortCut.Data(),dbName[i].Data());
            subSubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",gAnalyzer->GetDataBaseConnection(i))) {");
            subSubGroup->GetLastParameter()->AddSetLine("      return false;");
            subSubGroup->GetLastParameter()->AddSetLine("   }");
            subSubGroup->GetLastParameter()->AddSetLine("}");
            subSubGroup->GetLastParameter()->AddComboBoxEntry(dbName[i].Data());
         }
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBase(i)->GetType();");
         // DataBase/EventBased
         subSubGroup->AddParameter(new ROMEConfigParameter("EventBased","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetEventBasedDataBase(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->IsEventBasedDataBase()?1:0];");
      }

      // Folders
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("Folders");
         subGroup->AddSubGroup(subSubGroup);
         for (i = 0; i < numOfFolder; i++) {
            if (!folderUsed[i]) {
               continue;
            }
            if (folderDataBase[i] && !folderSupport[i]) {
               ROMEConfigParameterGroup* subSubSubGroup = new ROMEConfigParameterGroup(folderName[i],"1","Folder");
               subSubSubGroup->SetComment(ROMEConfig::kCommentLevelObj, folderShortDescription[i]);
               subSubGroup->AddSubGroup(subSubSubGroup);
               for (j = 0; j < numOfValue[i]; j++) {
                  if (valueDimension[i][j]>1) {
                     continue;
                  }
                  ROMEConfigParameterGroup* subSubSubSubGroup = new ROMEConfigParameterGroup(valueName[i][j],"1","Field");
                  subSubSubSubGroup->SetComment(ROMEConfig::kCommentLevelObj, valueComment[i][j]);
                  subSubSubGroup->AddSubGroup(subSubSubSubGroup);
                  // DataBaseName
                  subSubSubSubGroup->AddParameter(new ROMEConfigParameter("DataBaseName"));
                  subSubSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Field");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetDBAccess()->SetDBNameAt(%d,%d,##.Data());",
                                                                    i,j);
                  subSubSubSubGroup->GetLastParameter()->DontWriteLinesAlways();
                  subSubSubSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDBAccess()->GetDBNameAt(%d,%d);",
                                                                      i,j);
                  // DataBasePath
                  subSubSubSubGroup->AddParameter(new ROMEConfigParameter("DataBasePath"));
                  subSubSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Field");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("path = ##.Data();");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("ind = path.Last('\"');");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("if (ind == -1 || ind == path.Length() - 1) {");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   subPath = path.ReplaceAll(\"\\\"\",\"\");");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetDBAccess()->SetDBPathAt(%d,%d,subPath.Data());",
                                                                    i,j);
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("} else {");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   subPath = path(1,ind - 1);");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetDBAccess()->SetDBPathAt(%d,%d,subPath.Data());",
                                                                    i,j);
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   path = path(ind+2,path.Length()-ind+2);");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   int num = path.CountChar(',') + 1;");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   for (i = 0; i < num; i++) {");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("      if (i < num - 1) {");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("         ind = path.First(',');");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("      } else {");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("         ind = path.Length();");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("      }");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("      subPath = path(0, ind);");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetDBAccess()->SetDBCodeAt(%d,%d,i,gAnalyzer->GetObjectInterpreterCode(subPath.Data()));",
                                                                    i,j);
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("      path = path(ind + 1,path.Length()-ind + 1);");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("   }");
                  subSubSubSubGroup->GetLastParameter()->AddSetLine("}");
                  subSubSubSubGroup->GetLastParameter()->DontWriteLinesAlways();
                  subSubSubSubGroup->GetLastParameter()->AddWriteLine("tmp = gAnalyzer->GetDBAccess()->GetDBPathOriginalAt(%d,%d);",
                                                                      i,j);
                  subSubSubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetDBPathWriteFlag(tmp.Data())) {");
                  subSubSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetDBAccess()->GetDBPathOriginalAt(%d,%d);",
                                                                      i,j);
                  subSubSubSubGroup->GetLastParameter()->AddWriteLine("} else {");
                  subSubSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"\";");
                  subSubSubSubGroup->GetLastParameter()->AddWriteLine("}");
               }
            }
         }
      }

      // Trees
      if (numOfTree > 0) {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("Trees");
         subGroup->AddSubGroup(subSubGroup);
         // Accumulate
         subSubGroup->AddParameter(new ROMEConfigParameter("Accumulate","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName(),"/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Trees']/xs:annotation/xs:documentation");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetTreeAccumulation(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->isTreeAccumulation()?1:0];");
         // Tree
         for (i = 0; i < numOfTree; i++) {
            ROMEConfigParameterGroup* subSubSubGroup = new ROMEConfigParameterGroup(treeName[i],"1","Tree");
            subSubSubGroup->SetComment(ROMEConfig::kCommentLevelObj, treeDescription[i]);
            subSubGroup->AddSubGroup(subSubSubGroup);
            // Read
            subSubSubGroup->AddParameter(new ROMEConfigParameter("Read","1","CheckButton"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetRead(## == \"true\");",
                                                           i);
            subSubSubGroup->GetLastParameter()->AddSetLine("}");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetTreeObjectAt(%d)->isRead()?1:0];",
                                                             i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("}");
            // Write
            subSubSubGroup->AddParameter(new ROMEConfigParameter("Write","1","CheckButton"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetWrite(## == \"true\");",
                                                           i);
            subSubSubGroup->GetLastParameter()->AddSetLine("}");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetTreeObjectAt(%d)->isWrite()?1:0];",
                                                             i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("}");
            // Fill
            subSubSubGroup->AddParameter(new ROMEConfigParameter("Fill","1","CheckButton"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetFill((## == \"true\") || gAnalyzer->GetTreeObjectAt(%d)->isWrite());",
                                                           i, i);
            subSubSubGroup->GetLastParameter()->AddSetLine("}");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetTreeObjectAt(%d)->isFill()?1:0];",
                                                             i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("}");
            // CompressionLevel
            subSubSubGroup->AddParameter(new ROMEConfigParameter("CompressionLevel"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetCompressionLevel(##.ToInteger());",
                                                           i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%d\",gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());",
                                                             i);
            // MaxNumberOfEntries
            subSubSubGroup->AddParameter(new ROMEConfigParameter("MaxNumberOfEntries"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetMaxEntries(static_cast<Long_t>(##.ToInteger()));",
                                                           i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%"R_LLD"\",gAnalyzer->GetTreeObjectAt(%d)->GetMaxEntries());",
                                                             i);
            // TreeInputFileName
            subSubSubGroup->AddParameter(new ROMEConfigParameter("TreeInputFileName"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetConfigInputFileName(##);",
                                                           i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetTreeObjectAt(%d)->GetConfigInputFileName().Data();",
                                                             i);

            // TreeOutputFileName
            subSubSubGroup->AddParameter(new ROMEConfigParameter("TreeOutputFileName"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetConfigOutputFileName(##);",
                                                           i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor())");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetTreeObjectAt(%d)->GetConfigOutputFileName().Data();",
                                                             i);

            // SaveConfiguration
            subSubSubGroup->AddParameter(new ROMEConfigParameter("SaveConfiguration", "1", "CheckButton"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetSaveConfig(## == \"true\");",
                                                           i);
            subSubSubGroup->GetLastParameter()->AddSetLine("}");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetTreeObjectAt(%d)->isSaveConfig()?1:0];",
                                                             i);
            subSubSubGroup->GetLastParameter()->AddWriteLine("}");

            // Branch
            for (j = 0; j < numOfBranch[i]; j++) {
               tmp = branchName[i][j];
               if (tmp.EndsWith(".")) {
                  tmp.Resize(tmp.Length() - 1);
               }
               ROMEConfigParameterGroup* subSubSubSubGroup = new ROMEConfigParameterGroup(tmp,"1","Branch");
               subSubSubSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Branch");
               subSubSubSubGroup->SetWriteEmptyLine(kFALSE);
               subSubSubGroup->AddSubGroup(subSubSubSubGroup);
               // Active
               subSubSubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
               subSubSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Branch");
               subSubSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
               subSubSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetBranchActiveAt(%d, ## != \"false\");",
                                                                 i, j);
               subSubSubSubGroup->GetLastParameter()->AddSetLine("}");
               subSubSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
               subSubSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetTreeObjectAt(%d)->GetBranchActiveAt(%d)?1:0];",
                                                                   i, j);
               subSubSubSubGroup->GetLastParameter()->AddWriteLine("}");
            }
         }
      }

      // Global Steering Parameters
      if (readGlobalSteeringParameters) {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("GlobalSteeringParameters");
         subGroup->AddSubGroup(subSubGroup);
         AddSteeringConfigParameters(subSubGroup, 0, numOfTask,"gAnalyzer->GetGSP()","gAnalyzer");
      }
   }

   // Analyzer
   {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Analyzer");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // HistogramRead
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("HistogramRead");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // Read
         subSubGroup->AddParameter(new ROMEConfigParameter("Read","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetHistosRead(## == \"true\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->IsHistosRead()?1:0];");
         // RunNumber
         subSubGroup->AddParameter(new ROMEConfigParameter("RunNumber"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetHistosRun(##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetHistosRun();");
         // Path
         subSubGroup->AddParameter(new ROMEConfigParameter("Path"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetHistosPath(##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetHistosPath();");
      }

      // Macros
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("Macros");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // Macros/BeginOfRun
         subSubGroup->AddParameter(new ROMEConfigParameter("BeginOfRun"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetBeginOfRunMacro(##.Data());",
                                                     shortCut.Data());
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetBeginOfRunMacro();",
                                                       shortCut.Data());
         // Macros/BeginOfEvent
         subSubGroup->AddParameter(new ROMEConfigParameter("BeginOfEvent"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetBeginOfEventMacro(##.Data());",
                                                     shortCut.Data());
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetBeginOfEventMacro();",
                                                       shortCut.Data());
         // Macros/EndOfEvent
         subSubGroup->AddParameter(new ROMEConfigParameter("EndOfEvent"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetEndOfEventMacro(##.Data());",
                                                     shortCut.Data());
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetEndOfEventMacro();",
                                                       shortCut.Data());
         // Macros/EndOfRun
         subSubGroup->AddParameter(new ROMEConfigParameter("EndOfRun"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetEndOfRunMacro(##.Data());",
                                                     shortCut.Data());
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetEndOfRunMacro();",
                                                       shortCut.Data());
      }

      // Tasks
      if (numOfTask > 0) {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("Tasks");
         subGroup->AddSubGroup(subSubGroup);
         AddTaskConfigParameters(subSubGroup, -1);
      }
   }

   // Monitor
   {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Monitor");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Display
      if (numOfTab > 0) {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("Display");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // WindowScale
         subSubGroup->AddParameter(new ROMEConfigParameter("WindowScale"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetWindowScale(##.ToFloat());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%2.1f\",gAnalyzer->GetWindow()->GetWindowScale());");
         // StatusBar
         subSubGroup->AddParameter(new ROMEConfigParameter("StatusBar","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetStatusBarSwitch(## != \"false\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetWindow()->GetStatusBarSwitch()?1:0];");
         // UpdatePeriod
         // For the moment "UpdateFrequency" is used as the configuration parameter, although its value is inverse of the frequency indeed.
         subSubGroup->AddParameter(new ROMEConfigParameter("UpdateFrequency"));
//         subSubGroup->AddParameter(new ROMEConfigParameter("UpdatePeriod"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetWindowUpdatePeriod(##.ToInteger());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetWindowUpdatePeriod());");
         // ListTreeView
         subSubGroup->AddParameter(new ROMEConfigParameter("ListTreeView","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetListTreeView(## != \"false\");");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetWindow()->IsListTreeView()?1:0];");
         // AnalyzerController
         {
            ROMEConfigParameterGroup* subSubSubGroup = new ROMEConfigParameterGroup("AnalyzerController");
            subSubSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
            subSubGroup->AddSubGroup(subSubSubGroup);
            // Active
            subSubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam,
                                                            subSubSubGroup->GetGroupName());
            subSubSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetControllerActive(## == \"true\");");
            subSubSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetWindow()->IsControllerActive()?1:0];");
            // NetFolderName
            subSubSubGroup->AddParameter(new ROMEConfigParameter("NetFolderName"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam,
                                                            subSubSubGroup->GetGroupName());
            subSubSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetControllerNetFolder(##.Data());");
            subSubSubGroup->GetLastParameter()->AddWriteLine("writeString = \"\";");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->GetControllerNetFolder() != 0)");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetWindow()->GetControllerNetFolder()->GetName();");
         }
      }

      // SocketClient
      {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("SocketClient");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // Host
         subSubGroup->AddParameter(new ROMEConfigParameter("Host"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketClientHost(##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetSocketClientHost();");
         // Port
         subSubGroup->AddParameter(new ROMEConfigParameter("Port"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketClientPort(##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetSocketClientPort());");
      }

      // NetFolder
      for (i = 0; i < numOfNetFolder; i++) {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup(netFolderName[i],"1","NetFolder");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"NetFolder");
         subGroup->AddSubGroup(subSubGroup);
         // Active
         subSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderActive(%d, ## == \"true\");",i);
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetNetFolderActive(%d)?1:0];",
                                                       i);
         // Reconnect
         subSubGroup->AddParameter(new ROMEConfigParameter("Reconnect","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderReconnect(%d,## == \"true\");",i);
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[gAnalyzer->GetNetFolderReconnect(%d)?1:0];",
                                                       i);
         // Root
         subSubGroup->AddParameter(new ROMEConfigParameter("Root"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderRoot(%d,const_cast<char*>(##.Data()));",i);
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetNetFolderRoot(%d);",i);
         // Host
         subSubGroup->AddParameter(new ROMEConfigParameter("Host"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderHost(%d,const_cast<char*>(##.Data()));",i);
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%s\",gAnalyzer->GetNetFolderHost(%d));",
                                                       i);
         // Port
         subSubGroup->AddParameter(new ROMEConfigParameter("Port"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderPort(%d,const_cast<char*>(##.Data()));",i);
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\", gAnalyzer->GetNetFolderPort(%d));",i);
      }
      // Tabs
      if (numOfTab > 0) {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup("Tabs");
         subGroup->AddSubGroup(subSubGroup);
         AddTabConfigParameters(subSubGroup, -1);
      }
   }

   // midas banks
   if (numOfEvent > 0) {
      ROMEConfigParameterGroup* subGroup = new ROMEConfigParameterGroup("Midas");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Midas","/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Midas']/xs:annotation/xs:documentation");
      mainParGroup->AddSubGroup(subGroup);

      // byte swap
      subGroup->AddParameter(new ROMEConfigParameter("MidasByteSwap","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName(),
                                                "/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Midas']/xs:complexType/xs:sequence/xs:element[@name=MidasByteSwap]/xs:annotation/xs:documentation");
      subGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetMidasDAQ()->SetByteSwap(## == \"true\");");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetMidasDAQ()->GetByteSwap()?1:0];");
      subGroup->GetLastParameter()->AddWriteLine("}");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = \"false\";");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("}");

      // online communication thread
      subGroup->AddParameter(new ROMEConfigParameter("MidasOnlineCommunicationThread","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName(),
                                                "/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Midas']/xs:complexType/xs:sequence/xs:element[@name=MidasOnlineCommunicationThread]/xs:annotation/xs:documentation");
      subGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetMidasDAQ()->SetOnlineThread(## == \"true\");");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetMidasDAQ()->IsOnlineThread()?1:0];");
      subGroup->GetLastParameter()->AddWriteLine("}");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = \"false\";");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("}");
      // online loop period
      subGroup->AddParameter(new ROMEConfigParameter("MidasOnlineLoopPeriod"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName(),
                                                "/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Midas']/xs:complexType/xs:sequence/xs:element[@name=MidasOnlineLoopPeriod]/xs:annotation/xs:documentation");
      subGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetMidasDAQ()->SetOnlineLoopPeriod(strtol(##,&cstop, 10));");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddWriteLine("  writeString.SetFormatted(\"%%d\", gAnalyzer->GetMidasDAQ()->GetOnlineLoopPeriod());");
      subGroup->GetLastParameter()->AddWriteLine("}");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = kMidasInitialOnlineLoopPeriod;");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("}");

      for (i = 0; i < numOfEvent; i++) {
         ROMEConfigParameterGroup* subSubGroup = new ROMEConfigParameterGroup(eventName[i],"1","Event");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Event");
         subSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
         subGroup->AddSubGroup(subSubGroup);
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Event");
         subSubGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetMidasDAQ()->Set%sEventActive(## == \"true\");",
                                                     eventName[i].Data());
         subSubGroup->GetLastParameter()->AddSetLine("}");
         subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetMidasDAQ()->is%sEventActive()?1:0];",
                                                       eventName[i].Data());
         subSubGroup->GetLastParameter()->AddWriteLine("}");
         subSubGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
         subSubGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
         subSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
         subSubGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
         subSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddAdditionalWriteLine("}");
         for (j = 0; j < numOfBank[i]; j++) {
            ROMEConfigParameterGroup* subSubSubGroup = new ROMEConfigParameterGroup(bankAlias[i][j],"1","Bank");
            subSubSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Bank");
            subSubSubGroup->SetWriteEmptyLine(kFALSE);
            subSubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
            subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Bank");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetMidasDAQ()->Set%sBankActive(## == \"true\");",
                                                           bankAlias[i][j].Data());
            subSubSubGroup->GetLastParameter()->AddSetLine("}");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[gAnalyzer->GetMidasDAQ()->is%sBankActive()?1:0];",
                                                             bankAlias[i][j].Data());
            subSubSubGroup->GetLastParameter()->AddWriteLine("}");
            subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
            subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
            subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
            subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
            subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = \"true\";");
            subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("}");
            subSubGroup->AddSubGroup(subSubSubGroup);
         }
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::AddTaskConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex)
{
   int i,j;
   ROMEString name;
   ROMEString steerPointerT;
   ROMEString taskPointerT;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      if (taskHierarchyParentIndex[i] != parentIndex)
         continue;
      name.SetFormatted("%s%s",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
      subGroup = new ROMEConfigParameterGroup(name.Data(),"1","Task","TaskName","",taskHierarchyName[i].Data(),
                                              taskHierarchyMultiplicity[i]);
      subGroup->SetComment(ROMEConfig::kCommentLevelObj, taskShortDescription[taskHierarchyClassIndex[i]]);
      // Active
      subGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Task");
      subGroup->GetLastParameter()->SetDeclaration("%sT%s_Base *taskObject%d = static_cast<%sT%s_Base*>(gAnalyzer->GetTaskObjectAt(%d));",
                                                   shortCut.Data(), taskName[taskHierarchyClassIndex[i]].Data(), taskHierarchyObjectIndex[i],
                                                   shortCut.Data(), taskName[taskHierarchyClassIndex[i]].Data(), taskHierarchyObjectIndex[i]);
      subGroup->GetLastParameter()->AddSetLine("taskObject%d->SetActive(## == \"true\");",
                                               taskHierarchyObjectIndex[i]);
      subGroup->GetLastParameter()->AddSetLine("if (## != \"true\") {");
      if (taskHierarchyParentIndex[i] == -1) {
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetMainHistoFolder()->Remove(gAnalyzer->GetHistoFolderAt(%d));",
                                                  i);
      } else {
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetHistoFolderAt(%d)->Remove(gAnalyzer->GetHistoFolderAt(%d));",
                                                  taskHierarchyParentIndex[i],i);
      }
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[taskObject%d->IsActive()?1:0];",
                                                 taskHierarchyObjectIndex[i]);
      parGroup->AddSubGroup(subGroup);
      for (j = 0; j < numOfHistos[taskHierarchyClassIndex[i]]; j++) {
         subSubGroup = new ROMEConfigParameterGroup(histoName[taskHierarchyClassIndex[i]][j],"1","Histogram",
                                                    "HistName","","",1,kTRUE,histoType[taskHierarchyClassIndex[i]][j]);
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Histogram");
         subSubGroup->AddParameter(new ROMEConfigParameter("HistActive"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetActive(## == \"true\");",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[taskObject%d->Get%sHisto()->IsActive()?1:0];",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistWrite"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetWrite(## == \"true\");",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[taskObject%d->Get%sHisto()->IsWrite()?1:0];",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetTitle(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sHisto()->GetTitle();",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistFolderTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetFolderTitle(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sHisto()->GetFolderTitle();",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistArraySize"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetArraySize(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",taskObject%d->Get%sHisto()->GetArraySize());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistArrayStartIndex"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetArrayStartIndex(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",taskObject%d->Get%sHisto()->GetArrayStartIndex());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetXLabel(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sHisto()->GetXLabel();",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetYLabel(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sHisto()->GetYLabel();",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetZLabel(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sHisto()->GetZLabel();",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXNbins"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetXNbins(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",taskObject%d->Get%sHisto()->GetXNbins());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetXmin(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sHisto()->GetXmin());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetXmax(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sHisto()->GetXmax());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYNbins"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetYNbins(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",taskObject%d->Get%sHisto()->GetYNbins());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetYmin(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sHisto()->GetYmin());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetYmax(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sHisto()->GetYmax());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZNbins"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetZNbins(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",taskObject%d->Get%sHisto()->GetZNbins());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetZmin(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sHisto()->GetZmin());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetZmax(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sHisto()->GetZmax());",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistAccumulate","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sHisto()->SetAccumulate(## != \"false\");",
                                                     taskHierarchyObjectIndex[i],
                                                     histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddSetLine(" ");
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[taskObject%d->Get%sHisto()->IsAccumulate()?1:0];",
                                                       taskHierarchyObjectIndex[i],
                                                       histoName[taskHierarchyClassIndex[i]][j].Data());
         subGroup->AddSubGroup(subSubGroup);
      }
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         subSubGroup = new ROMEConfigParameterGroup(graphName[taskHierarchyClassIndex[i]][j],"1","Graph","GraphName");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Graph");
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphActive"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetActive(## == \"true\");",
                                                     taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[taskObject%d->Get%sGraph()->IsActive()?1:0];",
                                                       taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphWrite"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetWrite(## == \"true\");",
                                                     taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[taskObject%d->Get%sGraph()->IsWrite()?1:0];",
                                                       taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetTitle(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sGraph()->GetTitle();",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphFolderTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetFolderTitle(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sGraph()->GetFolderTitle();",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphArraySize"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetArraySize(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",taskObject%d->Get%sGraph()->GetArraySize());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphArrayStartIndex"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetArrayStartIndex(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",taskObject%d->Get%sGraph()->GetArrayStartIndex());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphXLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetXLabel(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sGraph()->GetXLabel();",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphYLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetYLabel(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sGraph()->GetYLabel();",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphZLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetZLabel(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = taskObject%d->Get%sGraph()->GetZLabel();",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphXmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetXmin(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sGraph()->GetXmin());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphXmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetXmax(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sGraph()->GetXmax());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphYmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetYmin(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sGraph()->GetYmin());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphYmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetYmax(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sGraph()->GetYmax());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphZmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetZmin(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sGraph()->GetZmin());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphZmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("taskObject%d->Get%sGraph()->SetZmax(##.Data());",
                                                     taskHierarchyObjectIndex[i],
                                                     graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",taskObject%d->Get%sGraph()->GetZmax());",
                                                       taskHierarchyObjectIndex[i],
                                                       graphName[taskHierarchyClassIndex[i]][j].Data());
         subGroup->AddSubGroup(subSubGroup);
      }
      if (numOfSteering[taskHierarchyClassIndex[i]] > 0) {
         steerPointerT.SetFormatted("taskObject%d->GetSP()",taskHierarchyObjectIndex[i]);
         taskPointerT.SetFormatted("taskObject%d",taskHierarchyObjectIndex[i]);
         AddSteeringConfigParameters(subGroup, 0, taskHierarchyClassIndex[i],steerPointerT,taskPointerT);
      }
      AddTaskConfigParameters(subGroup,i);
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::AddTabConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex)
{
   int iTab, j, k;
   ROMEString switchString;
   ROMEString steerPointerT;
   ROMEString tabPointerT;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab]) {
         continue;
      }
      if (tabParentIndex[iTab] != parentIndex) {
         continue;
      }
      subGroup = new ROMEConfigParameterGroup(tabName[iTab],"1","Tab","TabName");
      subGroup->SetComment(ROMEConfig::kCommentLevelObj, tabShortDescription[iTab]);

      // Active
      subGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
      subGroup->GetLastParameter()->SetDeclaration("%sT%s_Base *tabObject%d = static_cast<%sT%s_Base*>(gAnalyzer->GetWindow()->GetTabObjectAt(%d));",
                                                   shortCut.Data(), tabName[iTab].Data(), tabUsedIndex[iTab],
                                                   shortCut.Data(), tabName[iTab].Data(), tabUsedIndex[iTab]);
      subGroup->GetLastParameter()->AddSetLine("tabObject%d->SetSwitch(## != \"false\");",
                                               tabUsedIndex[iTab]);
      subGroup->GetLastParameter()->AddSetLine("tabObject%d->SetTabActive(## != \"false\");",
                                               tabUsedIndex[iTab]);
      subGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsSwitch()?1:0];",
                                                 tabUsedIndex[iTab]);
      if (tabObjectDisplay[iTab]) {
         for (j = 0; j < numOfTabObjectDisplays[iTab]; j++) {
            subSubGroup = new ROMEConfigParameterGroup(tabObjectDisplayName[iTab][j],"1","ObjectDisplay",
                                                       "ObjectDisplayName");
            subSubGroup->ReadComment(ROMEConfig::kCommentLevelObj, "ObjectDisplay");
            // Draw Option
            subSubGroup->AddParameter(new ROMEConfigParameter("DrawOption"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "ObjectDisplay");
            subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetDrawOptionAt(%d,##.Data());",
                                                        tabUsedIndex[iTab], j);
            subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%s\",tabObject%d->GetDrawOptionAt(%d));",
                                                          tabUsedIndex[iTab], j);
            // Logarithmic scale X
            subSubGroup->AddParameter(new ROMEConfigParameter("LogScaleX","1","CheckButton"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
            subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetLogScaleXAt(%d, ## == \"true\");",
                                                        tabUsedIndex[iTab], j);
            subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsLogScaleXAt(%d)?1:0];",
                                                          tabUsedIndex[iTab], j);
            // Logarithmic scale Y
            subSubGroup->AddParameter(new ROMEConfigParameter("LogScaleY","1","CheckButton"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
            subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetLogScaleYAt(%d, ## == \"true\");",
                                                          tabUsedIndex[iTab], j);
            subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsLogScaleYAt(%d)?1:0];",
                                                          tabUsedIndex[iTab], j);
            // Logarithmic scale Z
            subSubGroup->AddParameter(new ROMEConfigParameter("LogScaleZ","1","CheckButton"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
            subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetLogScaleZAt(%d, ## == \"true\");",
                                                        tabUsedIndex[iTab], j);
            subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsLogScaleZAt(%d)?1:0];",
                                                          tabUsedIndex[iTab], j);
            subGroup->AddSubGroup(subSubGroup);
         }
         // Number Of Pads X
         subGroup->AddParameter(new ROMEConfigParameter("NumberOfPadsX"));
         subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
         subGroup->GetLastParameter()->AddSetLine("tabObject%d->SetNumberOfPadsX(##.ToInteger());",
                                                  tabUsedIndex[iTab]);
         subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",tabObject%d->GetNumberOfPadsX());",
                                                    tabUsedIndex[iTab]);
         // Number Of Pads Y
         subGroup->AddParameter(new ROMEConfigParameter("NumberOfPadsY"));
         subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
         subGroup->GetLastParameter()->AddSetLine("tabObject%d->SetNumberOfPadsY(##.ToInteger());",
                                                  tabUsedIndex[iTab]);
         subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",tabObject%d->GetNumberOfPadsY());",
                                                    tabUsedIndex[iTab]);
         // Pad Configuration
         subGroup->AddParameter(new ROMEConfigParameter("PadConfiguration","1","CheckButton"));
         subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
         subGroup->GetLastParameter()->AddSetLine("tabObject%d->SetPadConfigActive(## == \"true\");",
                                                  tabUsedIndex[iTab]);
         subGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsPadConfigActive()?1:0];",
                                                    tabUsedIndex[iTab]);
      }
      if (numOfTabSingleObjects[iTab] > 0) {
         int nskip = 0;
         int *skip = new int[numOfTabSingleObjects[iTab]];
         bool skipthis = false;
         int nmulti = 0;
         int *multi = new int[numOfTabSingleObjects[iTab]];
         for (j = 0; j < numOfTabSingleObjects[iTab]; j++) {
            skipthis = false;
            for (k = 0; k < nskip; k++) {
               if (skip[k] == j) {
                  skipthis = true;
                  break;
               }
            }
            if (skipthis)
               continue;
            multi[0] = j;
            nmulti = 1;
            for (k = j + 1; k < numOfTabSingleObjects[iTab]; k++) {
               if (tabSingleObjectName[iTab][j] == tabSingleObjectName[iTab][k]) {
                  multi[nmulti] = k;
                  nmulti++;
                  skip[nskip] = k;
                  nskip++;
               }
            }
            subSubGroup = new ROMEConfigParameterGroup(tabSingleObjectName[iTab][j],"1","SingleObject","SingleObjectName");
            subSubGroup->ReadComment(ROMEConfig::kCommentLevelObj, "SingleObject");
            // Draw Option
            subSubGroup->AddParameter(new ROMEConfigParameter("DrawOption"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "ObjectDisplay");
            for (k = 0; k < nmulti; k++) {
               subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetDrawOptionAt(%d,##.Data());",
                                                           tabUsedIndex[iTab], multi[k]);
               subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%s\",tabObject%d->GetDrawOptionAt(%d));",
                                                             tabUsedIndex[iTab], multi[k]);
            }
            // Logarithmic scale X
            subSubGroup->AddParameter(new ROMEConfigParameter("LogScaleX","1","CheckButton"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
            for (k = 0; k < nmulti; k++) {
               subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetLogScaleXAt(%d, ## == \"true\");",
                                                           tabUsedIndex[iTab], multi[k]);
               subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsLogScaleXAt(%d)?1:0];",
                                                             tabUsedIndex[iTab], multi[k]);
            }
            // Logarithmic scale Y
            subSubGroup->AddParameter(new ROMEConfigParameter("LogScaleY","1","CheckButton"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
            for (k = 0;k < nmulti; k++) {
               subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetLogScaleYAt(%d, ## == \"true\");",
                                                           tabUsedIndex[iTab], multi[k]);
               subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsLogScaleYAt(%d)?1:0];",
                                                             tabUsedIndex[iTab], multi[k]);
            }
            // Logarithmic scale Z
            subSubGroup->AddParameter(new ROMEConfigParameter("LogScaleZ","1","CheckButton"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
            for (k = 0;k < nmulti; k++) {
               subSubGroup->GetLastParameter()->AddSetLine("tabObject%d->SetLogScaleZAt(%d, ## == \"true\");",
                                                           tabUsedIndex[iTab], multi[k]);
               subSubGroup->GetLastParameter()->AddWriteLine("writeString = kFalseTrueString[tabObject%d->IsLogScaleZAt(%d)?1:0];",
                                                             tabUsedIndex[iTab], multi[k]);
            }
            subGroup->AddSubGroup(subSubGroup);
         }
      }
      parGroup->AddSubGroup(subGroup);
      if (numOfSteering[iTab+numOfTask + 1] > 0) {
         steerPointerT.SetFormatted("tabObject%d->GetSP()",
                                    tabUsedIndex[iTab]);
         tabPointerT.SetFormatted("tabObject%d",
                                  tabUsedIndex[iTab]);
         AddSteeringConfigParameters(subGroup, 0, iTab+numOfTask + 1,steerPointerT,tabPointerT);
      }
      AddTabConfigParameters(subGroup,iTab);
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::AddSteeringConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t numSteer,Int_t numTask,
                                                ROMEString steerPointer,ROMEString taskPointer)
{
   int i;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEConfigParameterGroup* subGroup, *subSubGroup;
   ROMEString tmp;
   for (i = 0; i < numOfSteerFields[numTask][numSteer]; i++) {
      if (!steerFieldUsed[numTask][numSteer][i])
         continue;
      subGroup = new ROMEConfigParameterGroup(steerFieldName[numTask][numSteer][i].Data(),"1","SteeringParameterField",
                                              "SPName");
      subGroup->SetWriteEmptyLine(kFALSE);
      subGroup->SetComment(ROMEConfig::kCommentLevelSP,
                           ProcessCommentString(steerFieldShortDescription[numTask][numSteer][i],tmp).Data());
      parGroup->AddSubGroup(subGroup);
      if (subGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
         maxConfigParameterHierarchyLevel = subGroup->GetHierarchyLevel();
      if (steerFieldArraySize[numTask][numSteer][i] != "1") {
         subSubGroup = new ROMEConfigParameterGroup("SPFieldArray",steerFieldArraySize[numTask][numSteer][i].Data(),"",
                                                    "","SPFieldArrayIndex");
         subGroup->AddSubGroup(subSubGroup);
         if (subSubGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
            maxConfigParameterHierarchyLevel = subSubGroup->GetHierarchyLevel();
         if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subSubGroup->AddParameter(new ROMEConfigParameter("SPFieldArrayValue","1","CheckButton"));
         } else {
            subSubGroup->AddParameter(new ROMEConfigParameter("SPFieldArrayValue","1"));
         }
         subSubGroup->GetLastParameter()->SetComment(ROMEConfig::kCommentLevelSP,
                                                     ProcessCommentString(steerFieldShortDescription[numTask][numSteer][i],tmp).Data());
         setValue(&decodedValue,"","##",steerFieldType[numTask][numSteer][i].Data(), 1);
         subSubGroup->GetLastParameter()->AddSetLine("if (%s)",taskPointer.Data());
         if (steerFieldType[numTask][numSteer][i] == "std::string") {
            subSubGroup->GetLastParameter()->AddSetLine("   %s->Set%sAt(ii[%d],##.Data());",steerPointer.Data(),
                                                        steerFieldName[numTask][numSteer][i].Data(),
                                                        subSubGroup->GetHierarchyLevel());
         } else {
            subSubGroup->GetLastParameter()->AddSetLine("   %s->Set%sAt(ii[%d],static_cast<%s>(%s));",
                                                        steerPointer.Data(),
                                                        steerFieldName[numTask][numSteer][i].Data(),
                                                        subSubGroup->GetHierarchyLevel(),steerFieldType[numTask][numSteer][i].Data(),decodedValue.Data());
         }
         subSubGroup->GetLastParameter()->AddWriteLine("if (%s) {",taskPointer.Data());
         if (steerFieldType[numTask][numSteer][i] == "TString" || steerFieldType[numTask][numSteer][i] == "ROMEString") {
            subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%s\",%s->Get%sAt(ii[%d]).Data());",
                                                          steerPointer.Data(),
                                                          steerFieldName[numTask][numSteer][i].Data(),
                                                          subSubGroup->GetHierarchyLevel());
         } else if (steerFieldType[numTask][numSteer][i] == "std::string") {
            subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%s\",%s->Get%sAt(ii[%d]).c_str());",
                                                          steerPointer.Data(),
                                                          steerFieldName[numTask][numSteer][i].Data(),
                                                          subSubGroup->GetHierarchyLevel());
         } else if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subSubGroup->GetLastParameter()->AddWriteLine("   writeString = kFalseTrueString[%s->Get%sAt(ii[%d])?1:0];",
                                                          steerPointer.Data(),
                                                          steerFieldName[numTask][numSteer][i].Data(),
                                                          subSubGroup->GetHierarchyLevel());
         } else {
            if (steerFieldFormat[numTask][numSteer][i].Length()) {
               subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%s\",%s->Get%sAt(ii[%d]));",
                                                             steerFieldFormat[numTask][numSteer][i].Data(),
                                                             steerPointer.Data(),
                                                             steerFieldName[numTask][numSteer][i].Data(),
                                                             subSubGroup->GetHierarchyLevel());
            } else {
               subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(ROMEUtilities::GetFormat(typeid(%s)),%s->Get%sAt(ii[%d]));",
                                                             steerFieldType[numTask][numSteer][i].Data(),
                                                             steerPointer.Data(),
                                                             steerFieldName[numTask][numSteer][i].Data(),
                                                             subSubGroup->GetHierarchyLevel());
            }
         }
         subSubGroup->GetLastParameter()->AddWriteLine("}");
         subSubGroup->GetLastParameter()->AddWriteLine("else {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString.Resize(0);");
         subSubGroup->GetLastParameter()->AddWriteLine("}");
      } else {
         if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subGroup->AddParameter(new ROMEConfigParameter("SPValue","1","CheckButton"));
         } else {
            subGroup->AddParameter(new ROMEConfigParameter("SPValue","1"));
         }
         subGroup->GetLastParameter()->SetComment(ROMEConfig::kCommentLevelNever,
                                                  ProcessCommentString(steerFieldShortDescription[numTask][numSteer][i],
                                                                       tmp).Data());
         setValue(&decodedValue,"","##",steerFieldType[numTask][numSteer][i].Data(), 1);
//         subGroup->GetLastParameter()->AddSetLine("if (%s)",taskPointer.Data());
         if (steerFieldType[numTask][numSteer][i] == "std::string") {
            subGroup->GetLastParameter()->AddSetLine("%s->Set%s(##.Data());",steerPointer.Data(),
                                                     steerFieldName[numTask][numSteer][i].Data());
         } else {
            subGroup->GetLastParameter()->AddSetLine("%s->Set%s(static_cast<%s>(%s));",steerPointer.Data(),
                                                     steerFieldName[numTask][numSteer][i].Data(),
                                                     steerFieldType[numTask][numSteer][i].Data(),decodedValue.Data());
         }
         ROMEString sTab;
         if (taskPointer != "gAnalyzer") {
            subGroup->GetLastParameter()->AddWriteLine("if (%s) {",taskPointer.Data());
            sTab = "   ";
         } else {
            sTab = "";
         }
         if (steerFieldType[numTask][numSteer][i] == "TString" ||
             steerFieldType[numTask][numSteer][i] == "ROMEString") {
            subGroup->GetLastParameter()->AddWriteLine("%swriteString.SetFormatted(\"%%s\",%s->Get%s().Data());",
                                                       sTab.Data(), steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         } else if (steerFieldType[numTask][numSteer][i] == "std::string") {
            subGroup->GetLastParameter()->AddWriteLine("%swriteString.SetFormatted(\"%%s\",%s->Get%s().c_str());",
                                                       sTab.Data(), steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         } else if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subGroup->GetLastParameter()->AddWriteLine("%swriteString = kFalseTrueString[%s->Get%s()?1:0];",
                                                       sTab.Data(), steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         } else {
            if (steerFieldFormat[numTask][numSteer][i].Length()) {
               subGroup->GetLastParameter()->AddWriteLine("%swriteString.SetFormatted(\"%s\",%s->Get%s());",
                                                          sTab.Data(),
                                                          steerFieldFormat[numTask][numSteer][i].Data(),
                                                          steerPointer.Data(),
                                                          steerFieldName[numTask][numSteer][i].Data());
            } else {
               subGroup->GetLastParameter()->AddWriteLine("%swriteString.SetFormatted(ROMEUtilities::GetFormat(typeid(%s)),%s->Get%s());",
                                                          sTab.Data(),
                                                          steerFieldType[numTask][numSteer][i].Data(),
                                                          steerPointer.Data(),
                                                          steerFieldName[numTask][numSteer][i].Data());
            }
         }
         if (taskPointer != "gAnalyzer") {
            subGroup->GetLastParameter()->AddWriteLine("} else {");
            subGroup->GetLastParameter()->AddWriteLine("   writeString.Resize(0);");
            subGroup->GetLastParameter()->AddWriteLine("}");
         }
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]]) {
         continue;
      }
      subGroup = new ROMEConfigParameterGroup(steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"1",
                                              "SteeringParameterGroup","SPGroupName");
      parGroup->AddSubGroup(subGroup);
      if (subGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel) {
         maxConfigParameterHierarchyLevel = subGroup->GetHierarchyLevel();
      }
      subSubGroup = subGroup;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] != "1") {
         subSubGroup = new ROMEConfigParameterGroup("SPGroupArray",
                                                    steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                                    "","","SPGroupArrayIndex");
         subGroup->AddSubGroup(subSubGroup);
         if (subSubGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel) {
            maxConfigParameterHierarchyLevel = subSubGroup->GetHierarchyLevel();
         }
         steerPointerT.SetFormatted("%s->Get%sAt(ii[%d])",steerPointer.Data(),
                                    steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                    subSubGroup->GetHierarchyLevel());
      } else {
         steerPointerT.SetFormatted("%s->Get%s()",steerPointer.Data(),
                                    steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      AddSteeringConfigParameters(subSubGroup,steerChildren[numTask][numSteer][i],numTask,steerPointerT,taskPointer);
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::CheckConfigParameters(ROMEConfigParameterGroup *parGroup)
{
   int i;
   ROMEConfigParameter *parameter;
   ROMEConfigParameterGroup *subGroup;
   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      if (parameter->IsWriteLinesAlways() && parameter->GetNumberOfWriteLines() > 0) {
         parGroup->SetWriteAlways();
         break;
      }
   }
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      CheckConfigParameters(subGroup);
      if (subGroup->IsWriteAlways())
         parGroup->SetWriteAlways();
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigClass(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab)
{
   int i;
   bool found = false;
   ROMEConfigParameter *parameter;
   ROMEConfigParameterGroup *subGroup;

   ROMEString sTab = "";
   for (i = 0; i < tab; i++)
      sTab += "   ";
   buffer.AppendFormatted("\n%s// %s;\n",sTab.Data(),parGroup->GetGroupName().Data());
   if (parGroup->GetGroupIdentifier() == "Histogram") {
      buffer.AppendFormatted("%sROMEConfigHisto* f%s;\n",sTab.Data(),parGroup->GetGroupName().Data());
      buffer.AppendFormatted("%sBool_t f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier() == "Graph") {
      buffer.AppendFormatted("%sROMEConfigGraph* f%s;\n",sTab.Data(),parGroup->GetGroupName().Data());
      buffer.AppendFormatted("%sBool_t f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
      return true;
   }
   buffer.AppendFormatted("%sclass %s {\n",sTab.Data(),parGroup->GetGroupName().Data());
   buffer.AppendFormatted("%spublic:\n",sTab.Data());
   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      if (parameter->GetArraySize() == "1") {
         buffer.AppendFormatted("%s   ROMEString   f%s;\n",sTab.Data(),parameter->GetName());
         buffer.AppendFormatted("%s   Bool_t       f%sModified;\n",sTab.Data(),parameter->GetName());
      } else {
         buffer.AppendFormatted("%s   ROMEString   f%s[%s];\n",sTab.Data(),parameter->GetName(),
                                parameter->GetArraySize().Data());
         buffer.AppendFormatted("%s   Bool_t       f%sModified[%s];\n",sTab.Data(),
                                parameter->GetName(),
                                parameter->GetArraySize().Data());
         buffer.AppendFormatted("%s   Bool_t       f%sArrayModified;\n",sTab.Data(),
                                parameter->GetName());
      }
   }
   if (parGroup->GetGroupName() == "ConfigData") {
      buffer.AppendFormatted("%s   Int_t        fLastRunNumberIndex;\n",sTab.Data());
      buffer.AppendFormatted("%s   TArrayL64    fRunNumberArray;\n",sTab.Data());
      buffer.AppendFormatted("%s   Int_t        fLastInputFileNameIndex;\n",sTab.Data());
      buffer.AppendFormatted("%s   ROMEStrArray fInputFileNameArray;\n",sTab.Data());
   }
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      WriteConfigClass(buffer,parGroup->GetSubGroupAt(i),tab + 1);
   }

   // Constructor
   buffer.AppendFormatted("%sprivate:\n",sTab.Data());
   buffer.AppendFormatted("%s   %s(const %s &c); // not implemented\n",sTab.Data(),
                          parGroup->GetGroupName().Data(),parGroup->GetGroupName().Data());
   buffer.AppendFormatted("%s   %s &operator=(const %s &c); // not implemented\n",sTab.Data(),
                          parGroup->GetGroupName().Data(),parGroup->GetGroupName().Data());
   buffer.AppendFormatted("%spublic:\n",sTab.Data());
   buffer.AppendFormatted("%s   %s()\n",sTab.Data(),parGroup->GetGroupName().Data());
   ROMEString separator = ":";
   if (parGroup->GetGroupName() == "ConfigData") {
      buffer.AppendFormatted("%s   %sfLastRunNumberIndex(-1)\n",sTab.Data(),separator.Data());
      buffer.AppendFormatted("%s   ,fRunNumberArray()\n",sTab.Data());
      buffer.AppendFormatted("%s   ,fLastInputFileNameIndex(-1)\n",sTab.Data());
      buffer.AppendFormatted("%s   ,fInputFileNameArray()\n",sTab.Data());
      separator = ",";
   }
   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      if (parameter->GetArraySize() == "1") {
         buffer.AppendFormatted("%s   %sf%s(\"\")\n",sTab.Data(),separator.Data(),
                                parameter->GetName());
         buffer.AppendFormatted("%s   ,f%sModified(kFALSE)\n",sTab.Data(),parameter->GetName());
         separator = ",";
      } else {
         buffer.AppendFormatted("%s   %sf%sArrayModified(kFALSE)\n",sTab.Data(),separator.Data(),
                                parameter->GetName());
         separator = ",";
      }
   }
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      if (subGroup->GetGroupIdentifier() == "Histogram") {
         buffer.AppendFormatted("%s   %sf%s(new ROMEConfigHisto())\n", sTab.Data(), separator.Data(), subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   ,f%sModified(kFALSE)\n",sTab.Data(), subGroup->GetGroupName().Data());
         separator = ",";
      } else if (subGroup->GetGroupIdentifier() == "Graph") {
         buffer.AppendFormatted("%s   %sf%s(new ROMEConfigGraph())\n", sTab.Data(), separator.Data(), subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   ,f%sModified(kFALSE)\n", sTab.Data(), subGroup->GetGroupName().Data());
         separator = ",";
      } else if (subGroup->GetArraySize() == "1") {
         buffer.AppendFormatted("%s   %sf%s(new %s())\n", sTab.Data(), separator.Data(), subGroup->GetGroupName().Data(), subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   ,f%sModified(kFALSE)\n", sTab.Data(), subGroup->GetGroupName().Data());
         separator = ",";
      } else if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%s   %sf%s(0)\n",sTab.Data(),separator.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   ,f%sModified(0)\n",sTab.Data(),subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   ,f%sArrayModified(kFALSE)\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   ,f%sArraySize(0)\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         separator = ",";
      } else {
         buffer.AppendFormatted("%s   %sf%s(new %s*[%s])\n",sTab.Data(),separator.Data(),
                                subGroup->GetGroupName().Data(),
                                subGroup->GetGroupName().Data(),
                                subGroup->GetArraySize().Data());
         buffer.AppendFormatted("%s   ,f%sModified(new Bool_t[%s])\n",sTab.Data(),
                                subGroup->GetGroupName().Data(),
                                subGroup->GetArraySize().Data());
         buffer.AppendFormatted("%s   ,f%sArrayModified(kFALSE)\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   ,f%sArraySize(%s)\n",sTab.Data(),
                                subGroup->GetGroupName().Data(),
                                subGroup->GetArraySize().Data());
         separator = ",";
      }
   }

   buffer.AppendFormatted("%s   {\n",sTab.Data());
   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      if (parameter->GetArraySize() != "1") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         found = true;
         break;
      }
   }

   for (i = 0; i < parGroup->GetNumberOfSubGroups() && !found; i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      if (subGroup->GetArraySize() != "1" &&
          subGroup->GetArraySize() != "unknown") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         break;
      }
   }
   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      if (parameter->GetArraySize() == "1") {
      } else {
         buffer.AppendFormatted("%s      for (i = 0; i < %s; i++)\n",sTab.Data(),
                                parameter->GetArraySize().Data());
         buffer.AppendFormatted("%s         f%sModified[i] = false;\n",sTab.Data(),
                                parameter->GetName());
      }
   }
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      if (subGroup->GetGroupIdentifier() == "Histogram") {
      } else if (subGroup->GetGroupIdentifier() == "Graph") {
      } else if (subGroup->GetArraySize() == "1") {
      } else if (subGroup->GetArraySize() == "unknown") {
      } else {
         buffer.AppendFormatted("%s      for (i = 0; i < %s; i++) {\n",sTab.Data(),
                                subGroup->GetArraySize().Data());
         buffer.AppendFormatted("%s         f%s[i] = new %s();\n",sTab.Data(),
                                subGroup->GetGroupName().Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s         f%sModified[i] = false;\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
      }
   }
   buffer.AppendFormatted("%s   }\n",sTab.Data());
   // Destructor
   buffer.AppendFormatted("%s   ~%s() {\n",sTab.Data(),parGroup->GetGroupName().Data());
   found = false;
   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      if (parameter->GetArraySize() != "1") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         found = true;
         break;
      }
   }
   for (i = 0; i < parGroup->GetNumberOfSubGroups() && !found; i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize() != "1") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         break;
      }
   }
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      if (subGroup->GetGroupIdentifier() == "Histogram" ||
          subGroup->GetGroupIdentifier() == "Graph" ||
          subGroup->GetArraySize() == "1") {
         buffer.AppendFormatted("%s      SafeDelete(f%s);\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
      } else if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%s      for (i = 0; i < f%sArraySize; i++) {\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s         SafeDelete(f%s[i]);\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%sModified);\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%s);\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
      } else {
         buffer.AppendFormatted("%s      for (i = 0; i < %s; i++) {\n",sTab.Data(),
                                subGroup->GetArraySize().Data());
         buffer.AppendFormatted("%s         SafeDelete(f%s[i]);\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%s);\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%sModified);\n",sTab.Data(),
                                subGroup->GetGroupName().Data());
      }
   }
   buffer.AppendFormatted("%s   }\n",sTab.Data());
   buffer.AppendFormatted("%s};\n",sTab.Data());
   if (parGroup->GetGroupName() == "ConfigData") {
      buffer.AppendFormatted("%s%s** f%s;\n",sTab.Data(),parGroup->GetGroupName().Data(),
                             parGroup->GetGroupName().Data());
   } else {
      if (parGroup->GetArraySize() == "1") {
         buffer.AppendFormatted("%s%s* f%s;\n",sTab.Data(),parGroup->GetGroupName().Data(),
                                parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sBool_t f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
      } else {
         buffer.AppendFormatted("%s%s** f%s;\n",sTab.Data(),parGroup->GetGroupName().Data(),
                                parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sBool_t *f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sBool_t f%sArrayModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sInt_t  f%sArraySize;\n",sTab.Data(),parGroup->GetGroupName().Data());
      }
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigRead(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,
                                    ROMEString className,ROMEString pointer,ROMEString indexes,Int_t *iSub)
{
   int i,j;
   ROMEString classNameT;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString indexesT;
   ROMEString temp;
   ROMEString sTabT = "";
   ROMEString sTab = "";
   ROMEConfigParameter *parameter;
   ROMEConfigParameterGroup *subGroup;

   for (i = 0; i < tab; i++) {
      sTab += "   ";
   }

   // Parameters
   if (parGroup->GetGroupIdentifier() == "Histogram") {
      pointerT = pointer(0, pointer.Length()-2);
      buffer.AppendFormatted("%sReadHistoConfiguration(xml,path+\"/%s\",%s);\n",sTab.Data(),groupName.Data(),
                             pointerT.Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier() == "Graph") {
      pointerT = pointer(0, pointer.Length()-2);
      buffer.AppendFormatted("%sReadGraphConfiguration(xml,path+\"/%s\",%s);\n",sTab.Data(),groupName.Data(),
                             pointerT.Data());
      return true;
   }
   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      buffer.AppendFormatted("\n%s// %s%s\n",sTab.Data(),groupName.Data(),parameter->GetName());
      if (indexes.Length() == 0) {
         buffer.AppendFormatted("%sxml->GetPathValue(path+\"/%s%s\",\n",
                                sTab.Data(), groupName.Data(), parameter->GetName());
         buffer.AppendFormatted("%s                  %sf%s, \"\");\n",
                                sTab.Data(), pointer.Data(), parameter->GetName());
      } else {
         buffer.AppendFormatted("%stempPath.SetFormatted(\"/%s%s\"%s);\n",sTab.Data(),groupName.Data(),
                                parameter->GetName(),indexes.Data());
         buffer.AppendFormatted("%sxml->GetPathValue(path+tempPath, %sf%s, \"\");\n",
                                sTab.Data(), pointer.Data(), parameter->GetName());
      }
      buffer.AppendFormatted("%s%sf%sModified = (%sf%s != \"\");\n",sTab.Data(),pointer.Data(),
                             parameter->GetName(),pointer.Data(),
                             parameter->GetName());
      if (parameter->GetNumberOfReadModifiedTrueLines() > 0) {
         buffer.AppendFormatted("%sif (%sf%s == \"\") {\n",sTab.Data(),pointer.Data(),
                                parameter->GetName());
         for (j = 0; j < parameter->GetNumberOfReadModifiedTrueLines(); j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),parameter->GetReadModifiedTrueLineAt(j));
         }
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }

   // Sub Groups
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%sif (indx == 0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   %sf%sArraySize = xml->NumberOfOccurrenceOfPath(path+\"/%s%ss/%s\");\n",
                                sTab.Data(),pointer.Data(),subGroup->GetGroupName().Data(),
                                groupName.Data(),subGroup->GetGroupName().Data(),
                                subGroup->GetGroupName().Data());
         for (j = 0; j < subGroup->GetNumberOfReadGroupArrayInitLines(); j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),subGroup->GetReadGroupArrayInitLineAt(j));
         }
         buffer.AppendFormatted("%s   %sf%s = new ConfigData::%s%s*[%sf%sArraySize];\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data(),className.Data(),
                                subGroup->GetGroupName().Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   %sf%sModified = new bool[%sf%sArraySize];\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s   for (i = 0; i < %sf%sArraySize; i++) {\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      %sf%s[i] = new ConfigData::%s%s();\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data(),className.Data(),
                                subGroup->GetGroupName().Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%sfor (ii[%d] = 0; ii[%d] < %s; ii[%d]++) {\n",sTab.Data(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetArraySize().Data(),
                                subGroup->GetHierarchyLevel());
      }
      indexesT = indexes;
      classNameT.SetFormatted("%s%s::",className.Data(),subGroup->GetGroupName().Data());
      sTabT = sTab.Data();
      if (subGroup->GetArraySize() == "unknown") {
         sTabT += "      ";
      } else if (subGroup->GetArraySize() != "1") {
         sTabT += "   ";
      }
      if (subGroup->GetArraySize() == "unknown") {
         groupNameT.SetFormatted("%s%ss[%d]/%s[%%d]/",groupName.Data(),subGroup->GetTagName().Data(),
                                 subGroup->GetMultiplicity(),
                                 subGroup->GetTagName().Data());
         indexesT.AppendFormatted(",i + 1");
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),subGroup->GetGroupName().Data());
         WriteConfigRead(buffer,subGroup,tab+2,groupNameT,classNameT,pointerT,indexesT,iSub);
      } else if (subGroup->GetArraySize() != "1") {
         if (subGroup->GetGroupIdentifier().Length() > 0) {
            groupNameT.SetFormatted("%s%s[%s='%s'][%s%%d]/",groupName.Data(),
                                    subGroup->GetGroupIdentifier().Data(),
                                    subGroup->GetNameIdentifier().Data(),
                                    subGroup->GetTagName().Data(),
                                    subGroup->GetArrayIdentifier().Data());
         } else {
            groupNameT.SetFormatted("%s%s[%s%%d]/",groupName.Data(),subGroup->GetTagName().Data(),
                                    subGroup->GetArrayIdentifier().Data());
         }
         indexesT.AppendFormatted(",ii[%d]",subGroup->GetHierarchyLevel());
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),subGroup->GetGroupName().Data(),
                               subGroup->GetHierarchyLevel());
         WriteConfigRead(buffer,subGroup,tab + 1,groupNameT,classNameT,pointerT,indexesT,iSub);
      } else {
         if (subGroup->GetGroupIdentifier().Length() > 0) {
            groupNameT.SetFormatted("%s%s[%s='%s'][%d]/",groupName.Data(),
                                    subGroup->GetGroupIdentifier().Data(),
                                    subGroup->GetNameIdentifier().Data(),
                                    subGroup->GetTagName().Data(),
                                    subGroup->GetMultiplicity());
         } else {
            groupNameT.SetFormatted("%s%s[%d]/",groupName.Data(),subGroup->GetTagName().Data(),
                                    subGroup->GetMultiplicity());
         }
         if (subGroup->GetGroupIdentifier() == "Histogram") {
            buffer.AppendFormatted("%sROMEConfigHisto* tmp%s%d = %sf%s;\n", sTab.Data(),
                                   subGroup->GetGroupName().Data(), *iSub,
                                   pointer.Data(), subGroup->GetGroupName().Data());
         } else if (subGroup->GetGroupIdentifier() == "Graph") {
            buffer.AppendFormatted("%sROMEConfigGraph* tmp%s%d = %sf%s;\n", sTab.Data(),
                                   subGroup->GetGroupName().Data(), *iSub,
                                   pointer.Data(),subGroup->GetGroupName().Data());
         } else {
            temp = classNameT(0, classNameT.Length() - 2);
            buffer.AppendFormatted("%sConfigData::%s* tmp%s%d;\n", sTab.Data(), // for suppresion compiler warning
                                   temp.Data(), subGroup->GetGroupName().Data(), *iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n", sTab.Data(),
                                   subGroup->GetGroupName().Data(), *iSub,
                                   pointer.Data(), subGroup->GetGroupName().Data());
         }
         pointerT.SetFormatted("tmp%s%d->",subGroup->GetGroupName().Data(),*iSub);
         (*iSub)++;
         WriteConfigRead(buffer,subGroup,tab,groupNameT,classNameT,pointerT,indexes,iSub);
      }
      if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s}\n",sTab.Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigCheckModified(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,
                                             ROMEString groupName,ROMEString className,ROMEString pointer,
                                             ROMEString indexes,Int_t* iSub)
{
   int i,j;
   ROMEString classNameT;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString indexesT;
   ROMEString temp;
   ROMEString sTabT = "";
   ROMEString sTab = "";
   ROMEConfigParameterGroup *subGroup, *subsubGroup;
   int iSubSave = 0;
   for (i = 0; i < tab; i++)
      sTab += "   ";

   // Sub Groups
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%sif (indx == 0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   for (i = 0; i < %sf%sArraySize; i++) {\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%sfor (ii[%d] = 0; ii[%d] < %s; ii[%d]++) {\n",sTab.Data(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetArraySize().Data(),
                                subGroup->GetHierarchyLevel());
      }
      indexesT = indexes;
      classNameT.SetFormatted("%s%s::",className.Data(),subGroup->GetGroupName().Data());
      sTabT = sTab.Data();
      if (subGroup->GetArraySize() == "unknown") {
         sTabT += "      ";
      } else if (subGroup->GetArraySize() != "1") {
         sTabT += "   ";
      }
      if (subGroup->GetArraySize() == "unknown") {
         groupNameT.SetFormatted("%s%ss[%d]/%s[%%d]/",groupName.Data(),subGroup->GetTagName().Data(),
                                 subGroup->GetMultiplicity(),
                                 subGroup->GetTagName().Data());
         indexesT.AppendFormatted(",i + 1");
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),subGroup->GetGroupName().Data());
         WriteConfigCheckModified(buffer,subGroup,tab+2,groupNameT,classNameT,pointerT,indexesT,iSub);
      } else if (subGroup->GetArraySize() != "1") {
         if (subGroup->GetGroupIdentifier().Length() > 0) {
            groupNameT.SetFormatted("%s%s[%s='%s'][%s%%d]/",groupName.Data(),
                                    subGroup->GetGroupIdentifier().Data(),
                                    subGroup->GetNameIdentifier().Data(),
                                    subGroup->GetTagName().Data(),
                                    subGroup->GetArrayIdentifier().Data());
         } else {
            groupNameT.SetFormatted("%s%s[%s%%d]/",groupName.Data(),subGroup->GetTagName().Data(),
                                    subGroup->GetArrayIdentifier().Data());
         }
         indexesT.AppendFormatted(",ii[%d]",subGroup->GetHierarchyLevel());
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),subGroup->GetGroupName().Data(),
                               subGroup->GetHierarchyLevel());
         WriteConfigCheckModified(buffer,subGroup,tab + 1,groupNameT,classNameT,
                                  pointerT,indexesT,iSub);
      } else {
         if (subGroup->GetGroupIdentifier().Length() > 0) {
            groupNameT.SetFormatted("%s%s[%s='%s'][%d]/",groupName.Data(),
                                    subGroup->GetGroupIdentifier().Data(),
                                    subGroup->GetNameIdentifier().Data(),
                                    subGroup->GetTagName().Data(),
                                    subGroup->GetMultiplicity());
         } else {
            groupNameT.SetFormatted("%s%s[%d]/",groupName.Data(),subGroup->GetTagName().Data(),
                                    subGroup->GetMultiplicity());
         }
         if (subGroup->GetGroupIdentifier() == "Histogram") {
            buffer.AppendFormatted("%sROMEConfigHisto* tmp%s%d;\n",sTab.Data(),
                                   subGroup->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),subGroup->GetGroupName().Data(),
                                   *iSub,pointer.Data(),subGroup->GetGroupName().Data());
         } else if (subGroup->GetGroupIdentifier() == "Graph") {
            buffer.AppendFormatted("%sROMEConfigGraph* tmp%s%d;\n",sTab.Data(),
                                   subGroup->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),subGroup->GetGroupName().Data(),
                                   *iSub,pointer.Data(),subGroup->GetGroupName().Data());
         } else {
            temp = classNameT(0, classNameT.Length()-2);
            buffer.AppendFormatted("%sConfigData::%s* tmp%s%d;\n",sTab.Data(),temp.Data(),
                                   subGroup->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),subGroup->GetGroupName().Data(),
                                   *iSub,pointer.Data(),subGroup->GetGroupName().Data());
         }
         pointerT.SetFormatted("tmp%s%d->",subGroup->GetGroupName().Data(),*iSub);
         iSubSave = *iSub;
         (*iSub)++;
         WriteConfigCheckModified(buffer,subGroup,tab,groupNameT,classNameT,pointerT,indexes,iSub);
      }
      if (subGroup->GetNumberOfParameters()+subGroup->GetNumberOfSubGroups() > 0) {
         buffer.AppendFormatted("%s// %s%s--\n",sTabT.Data(),groupName.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sif (",sTabT.Data());
         if (subGroup->GetGroupIdentifier() == "Histogram") {
            buffer.AppendFormatted("CheckHistoConfigurationModified(%sf%s)",pointer.Data(),
                                   subGroup->GetGroupName().Data());
         } else if (subGroup->GetGroupIdentifier() == "Graph") {
            buffer.AppendFormatted("CheckGraphConfigurationModified(%sf%s)",pointer.Data(),
                                   subGroup->GetGroupName().Data());
         } else {
            for (j = 0; j < subGroup->GetNumberOfParameters(); j++) {
               if (subGroup->GetArraySize() == "unknown") {
                  buffer.AppendFormatted("%sf%s[i]->f%sModified",pointer.Data(),
                                         subGroup->GetGroupName().Data(),
                                         subGroup->GetParameterAt(j)->GetName());
               } else if (subGroup->GetArraySize() != "1") {
                  buffer.AppendFormatted("%sf%s[ii[%d]]->f%sModified",pointer.Data(),
                                         subGroup->GetGroupName().Data(),
                                         subGroup->GetHierarchyLevel(),
                                         subGroup->GetParameterAt(j)->GetName());
               } else {
                  buffer.AppendFormatted("tmp%s%d->f%sModified",subGroup->GetGroupName().Data(),
                                         iSubSave,subGroup->GetParameterAt(j)->GetName());
               }
               if (j < subGroup->GetNumberOfParameters() - 1 ||
                   subGroup->GetNumberOfSubGroups() != 0)
                  buffer.AppendFormatted(" ||\n%s    ",sTabT.Data());
            }
            for (j = 0; j < subGroup->GetNumberOfSubGroups(); j++) {
               subsubGroup = subGroup->GetSubGroupAt(j);
               if (subGroup->GetArraySize() == "unknown") {
                  buffer.AppendFormatted("%sf%s[i]->f%sModified",pointer.Data(),
                                         subGroup->GetGroupName().Data(),
                                         subsubGroup->GetGroupName().Data());
               } else if (subGroup->GetArraySize() != "1") {
                  buffer.AppendFormatted("%sf%s[ii[%d]]->f%sModified",pointer.Data(),
                                         subGroup->GetGroupName().Data(),
                                         subGroup->GetHierarchyLevel(),
                                         subsubGroup->GetGroupName().Data());
               } else {
                  buffer.AppendFormatted("tmp%s%d->f%sModified",subGroup->GetGroupName().Data(),
                                         iSubSave,subsubGroup->GetGroupName().Data());
               }
               if (j < subGroup->GetNumberOfSubGroups() - 1)
                  buffer.AppendFormatted(" ||\n%s    ",sTabT.Data());
            }
         }
         buffer.AppendFormatted(") {\n");
         if (subGroup->GetArraySize() == "unknown") {
            buffer.AppendFormatted("%s   %sf%sModified[i] = true;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
            buffer.AppendFormatted("%s   %sf%sArrayModified = true;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         } else if (subGroup->GetArraySize() != "1") {
            buffer.AppendFormatted("%s   %sf%sModified[ii[%d]] = true;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data(),
                                   subGroup->GetHierarchyLevel());
            buffer.AppendFormatted("%s   %sf%sArrayModified = true;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         } else {
            buffer.AppendFormatted("%s   %sf%sModified = true;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         }
         buffer.AppendFormatted("%s}\n",sTabT.Data());
         buffer.AppendFormatted("%selse {\n",sTabT.Data());
         if (subGroup->GetArraySize() == "unknown") {
            buffer.AppendFormatted("%s   %sf%sModified[i] = false;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         } else if (subGroup->GetArraySize() != "1") {
            buffer.AppendFormatted("%s   %sf%sModified[ii[%d]] = false;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data(),
                                   subGroup->GetHierarchyLevel());
         } else {
            buffer.AppendFormatted("%s   %sf%sModified = false;\n",sTabT.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         }
         buffer.AppendFormatted("%s}\n",sTabT.Data());
      }
      if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s}\n",sTab.Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigSet(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,
                                   ROMEString pointer,Int_t* iSub)
{
   int i,j,tabT;
   ROMEString buf;
   ROMEString temp;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString sTab = "";
   ROMEConfigParameter *parameter;
   ROMEConfigParameterGroup *subGroup;

   for (i = 0; i < tab; i++) {
      sTab += "   ";
   }

   // Parameters
   if (parGroup->GetGroupIdentifier() == "Histogram") {
      pointerT = pointer(0, pointer.Length()-2);
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0, temp.Last('-'));
      buffer.AppendFormatted("%sSetHistoConfiguration(%s,configData->%s);\n",sTab.Data(),temp.Data(),pointerT.Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier() == "Graph") {
      pointerT = pointer(0, pointer.Length()-2);
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0, temp.Last('-'));
      buffer.AppendFormatted("%sSetGraphConfiguration(%s,configData->%s);\n",sTab.Data(),temp.Data(),pointerT.Data());
      return true;
   }
   for (i = 0; i < parGroup->GetNumberOfParameters() ; i++) {
      parameter = parGroup->GetParameterAt(i);
      if (parameter->GetNumberOfSetLines() > 0) {
         buffer.AppendFormatted("\n%s// %s%s\n",sTab.Data(),groupName.Data(),parameter->GetName());
         if (strlen(parameter->GetDeclaration())>0) {
            buffer.AppendFormatted("%s%s\n", sTab.Data(), parameter->GetDeclaration());
         }
         buffer.AppendFormatted("%sif (modConfigData->%sf%sModified) {\n",sTab.Data(),pointer.Data(),
                                parameter->GetName());
         for (j = 0; j < parameter->GetNumberOfSetLines(); j++) {
            buf = parameter->GetSetLineAt(j);
            temp.SetFormatted("configData->%sf%s",pointer.Data(),parameter->GetName());
            buf.ReplaceAll("##",temp);
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),buf.Data());
         }
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }

   // Sub Groups
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      tabT = tab;
      groupNameT.SetFormatted("%s%s/",groupName.Data(),subGroup->GetGroupName().Data());
      pointerT.SetFormatted("%sf%s->",pointer.Data(),subGroup->GetGroupName().Data());
      if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%sif (indx == 0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   if (modConfigData->%sf%sArrayModified) {\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      for (i = 0; i < configData->%sf%sArraySize; i++) {\n",sTab.Data(),
                                pointer.Data(),subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s         if (modConfigData->%sf%sModified[i]) {\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
         tabT += 4;
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),subGroup->GetGroupName().Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%sfor (ii[%d] = 0; ii[%d] < %s; ii[%d]++) {\n",sTab.Data(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetArraySize().Data(),
                                subGroup->GetHierarchyLevel());
         tabT += 1;
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),subGroup->GetGroupName().Data(),
                               subGroup->GetHierarchyLevel());
      }
      WriteConfigSet(buffer,subGroup,tabT,groupNameT,pointerT,iSub);
      if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%s         }\n",sTab.Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s}\n",sTab.Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigWrite(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,
                                     ROMEString pointer,Int_t* iSub)
{
   int i,j,tabT;
   ROMEString buf;
   ROMEString temp;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString sTabT;
   ROMEString sTab = "";
   ROMEConfigParameter *parameter;
   ROMEConfigParameterGroup *subGroup;
   for (i = 0; i < tab; i++) {
      sTab += "   ";
   }

   // Parameters
   if (parGroup->GetGroupIdentifier() == "Histogram") {
      pointerT = pointer(0, pointer.Length()-2);
      buffer.AppendFormatted("%sWriteHistoConfiguration(xml,configData->%s,fCommentLevel,histoComments);\n",sTab.Data(),
                             pointerT.Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier() == "Graph") {
      pointerT = pointer(0, pointer.Length()-2);
      buffer.AppendFormatted("%sWriteGraphConfiguration(xml,configData->%s,fCommentLevel,graphComments);\n",sTab.Data(),
                             pointerT.Data());
      return true;
   }

   for (i = 0; i < parGroup->GetNumberOfParameters(); i++) {
      parameter = parGroup->GetParameterAt(i);
      buffer.AppendFormatted("\n%s// %s%s\n",sTab.Data(), groupName.Data(), parameter->GetName());
      if (strlen(parameter->GetDeclaration())>0) {
         buffer.AppendFormatted("%s%s\n", sTab.Data(), parameter->GetDeclaration());
      }
      if (!parameter->IsWriteLinesAlways() ||
          parameter->GetNumberOfWriteLines() == 0) {
         if (parameter->GetComment().Length()) {
            buffer.AppendFormatted("%sif (fCommentLevel >= %d && configData->%sf%sModified) {\n",sTab.Data(),
                                   parameter->GetCommentLevel(),pointer.Data(),
                                   parameter->GetName());
            buffer.AppendFormatted("%s   xml->WriteComment(\"%s\");\n",sTab.Data(),
                                   ProcessCommentString(parameter->GetComment(),temp).Data());
            buffer.AppendFormatted("%s}\n",sTab.Data());
         }
         buffer.AppendFormatted("%s",sTab.Data());
      } else {
         if (parameter->GetComment().Length()) {
            buffer.AppendFormatted("%sif (fCommentLevel >= %d && (configData->%sf%sModified || indx == 0)) {\n",
                                   sTab.Data(),parameter->GetCommentLevel(),pointer.Data(),
                                   parameter->GetName());
            buffer.AppendFormatted("%s   xml->WriteComment(\"%s\");\n",sTab.Data(),
                                   ProcessCommentString(parameter->GetComment(),temp).Data());
            buffer.AppendFormatted("%s}\n",sTab.Data());
         }
         buffer.AppendFormatted("%sif (indx == 0) {\n",sTab.Data());
         for (j = 0; j < parameter->GetNumberOfWriteLines(); j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),parameter->GetWriteLineAt(j));
         }
         for (j = 0; j < parameter->GetNumberOfAdditionalWriteLines(); j++) {
            buf = parameter->GetAdditionalWriteLineAt(j);
            temp.SetFormatted("configData->%sf%s",pointer.Data(),parameter->GetName());
            buf.ReplaceAll("##",temp);
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),buf.Data());
         }
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",writeString.Data());\n",sTab.Data(),
                                parameter->GetName());
         buffer.AppendFormatted("%s} else ",sTab.Data());
      }
      buffer.AppendFormatted("if (configData->%sf%sModified) {\n",pointer.Data(),
                             parameter->GetName());
      buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",configData->%sf%s.Data());\n",sTab.Data(),
                             parameter->GetName(),pointer.Data(),
                             parameter->GetName());
      buffer.AppendFormatted("%s}\n",sTab.Data());
   }

   // Sub Groups
   for (i = 0; i < parGroup->GetNumberOfSubGroups(); i++) {
      subGroup = parGroup->GetSubGroupAt(i);
      sTabT = sTab;
      tabT = tab;
      groupNameT.SetFormatted("%s%s/",groupName.Data(),subGroup->GetGroupName().Data());
      pointerT.SetFormatted("%sf%s->",pointer.Data(),subGroup->GetGroupName().Data());
      buffer.AppendFormatted("\n%s// %s%s\n",sTab.Data(),groupName.Data(),
                             subGroup->GetGroupName().Data());
      if (subGroup->GetArraySize() == "unknown") {
         if (subGroup->IsWriteAlways()) {
            buffer.AppendFormatted("%sif (configData->%sf%sArrayModified || indx == 0) {\n",sTab.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         } else {
            buffer.AppendFormatted("%sif (configData->%sf%sArrayModified) {\n",sTab.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         }
         if (i != 0 && subGroup->GetWriteEmptyLine()) {
            buffer.AppendFormatted("%s   xml->WriteEmptyLine();\n",sTab.Data());
         }
         buffer.AppendFormatted("%s   xml->StartElement(\"%ss\");\n",sTab.Data(),
                                subGroup->GetTagName().Data());
         buffer.AppendFormatted("%s   for (i = 0; i < configData->%sf%sArraySize; i++) {\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
         buffer.AppendFormatted("%s      if (configData->%sf%sModified[i]) {\n",sTab.Data(),pointer.Data(),
                                subGroup->GetGroupName().Data());
         sTabT += "      ";
         tabT += 2;
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),subGroup->GetGroupName().Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%sfor (ii[%d] = 0; ii[%d] < %s; ii[%d]++) {\n",sTab.Data(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetHierarchyLevel(),
                                subGroup->GetArraySize().Data(),
                                subGroup->GetHierarchyLevel());
         if (subGroup->IsWriteAlways()) {
            buffer.AppendFormatted("%s   if (configData->%sf%sModified[ii[%d]] || indx == 0) {\n",sTab.Data(),
                                   pointer.Data(),subGroup->GetGroupName().Data(),
                                   subGroup->GetHierarchyLevel());
         } else {
            buffer.AppendFormatted("%s   if (configData->%sf%sModified[ii[%d]]) {\n",sTab.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data(),
                                   subGroup->GetHierarchyLevel());
         }
         tabT += 1;
         sTabT += "   ";
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),subGroup->GetGroupName().Data(),
                               subGroup->GetHierarchyLevel());
      } else {
         if (subGroup->IsWriteAlways()) {
            buffer.AppendFormatted("%sif (configData->%sf%sModified || indx == 0) {\n",sTab.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         } else {
            buffer.AppendFormatted("%sif (configData->%sf%sModified) {\n",sTab.Data(),pointer.Data(),
                                   subGroup->GetGroupName().Data());
         }
      }
      for (j = 0; j < subGroup->GetNumberOfWriteStartLines(); j++) {
         buffer.AppendFormatted("%s   %s\n",sTabT.Data(),subGroup->GetWriteStartLineAt(j));
      }
      if (i != 0 && subGroup->GetWriteEmptyLine()) {
         buffer.AppendFormatted("%s   xml->WriteEmptyLine();\n",sTabT.Data());
      }
      if (subGroup->GetGroupIdentifier().Length() > 0) {
         buffer.AppendFormatted("%s   xml->StartElement(\"%s\");\n",sTabT.Data(),
                                subGroup->GetGroupIdentifier().Data());
         if (subGroup->GetComment().Length()) {
            buffer.AppendFormatted("%s   if (fCommentLevel >= %d) {\n",sTabT.Data(),
                                   subGroup->GetCommentLevel());
            buffer.AppendFormatted("%s      xml->WriteComment(\"%s\");\n",sTabT.Data(),
                                   ProcessCommentString(subGroup->GetComment(),temp).Data());
            buffer.AppendFormatted("%s   }\n", sTabT.Data());
         }
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",\"%s\");\n",sTabT.Data(),
                                subGroup->GetNameIdentifier().Data(),
                                subGroup->GetTagName().Data());
      } else {
         buffer.AppendFormatted("%s   xml->StartElement(\"%s\");\n",sTabT.Data(),
                                subGroup->GetTagName().Data());
         if (subGroup->GetComment().Length()) {
            buffer.AppendFormatted("%s   if (fCommentLevel >= %d) {\n",sTabT.Data(),
                                   subGroup->GetCommentLevel());
            buffer.AppendFormatted("%s      xml->WriteComment(\"%s\");\n",sTabT.Data(),
                                   ProcessCommentString(subGroup->GetComment(),temp).Data());
            buffer.AppendFormatted("%s   }\n", sTabT.Data());
         }
      }
      if (subGroup->GetArraySize() != "1" &&
          subGroup->GetArrayIdentifier().Length()) {
         if (subGroup->GetArraySize() == "unknown") {
            buffer.AppendFormatted("%s   str.SetFormatted(\"%%d\",i);\n",sTabT.Data());
         } else {
            buffer.AppendFormatted("%s   str.SetFormatted(\"%%d\",ii[%d]);\n",sTabT.Data(),
                                   subGroup->GetHierarchyLevel());
         }
         if (subGroup->GetComment().Length()) {
            buffer.AppendFormatted("%s   if (fCommentLevel >= %d) {\n",sTabT.Data(),
                                   subGroup->GetCommentLevel());
            buffer.AppendFormatted("%s      xml->WriteComment(\"%s\");\n",sTabT.Data(),
                                   ProcessCommentString(subGroup->GetComment(),temp).Data());
            buffer.AppendFormatted("%s   }", sTabT.Data());
         }
         temp = subGroup->GetArrayIdentifier();
         if (temp.EndsWith("="))
            temp.Resize(temp.Length() - 1);
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",str.Data());\n",sTabT.Data(),temp.Data());
      }
      WriteConfigWrite(buffer,subGroup,tabT + 1,groupNameT,pointerT,iSub);
      buffer.AppendFormatted("%s   xml->EndElement();\n",sTabT.Data());
      for (j = 0; j < subGroup->GetNumberOfWriteEndLines(); j++) {
         buffer.AppendFormatted("%s   %s\n",sTabT.Data(),subGroup->GetWriteEndLineAt(j));
      }
      if (subGroup->GetArraySize() == "unknown") {
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s   xml->EndElement();\n",sTab.Data());
      } else if (subGroup->GetArraySize() != "1") {
         buffer.AppendFormatted("%s   }\n",sTab.Data());
      }
      buffer.AppendFormatted("%s}\n",sTab.Data());
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteUpdateObjectsObject(ROMEString &buffer,const char *objectPointer,
                                             const char *objectStoragePointer,bool bypass)
{
   if (bypass) {
      buffer.AppendFormatted("         bypassOrgOld = %s->CanBypassStreamer();\n",objectStoragePointer);
      buffer.AppendFormatted("         bypassOld = %s->CanBypassStreamer();\n",objectPointer);
      buffer.AppendFormatted("         %s->BypassStreamer(kTRUE);\n",objectStoragePointer);
      buffer.AppendFormatted("         %s->BypassStreamer(kTRUE);\n",objectPointer);
   }
   buffer.AppendFormatted("         gAnalyzer->CopyTObjectWithStreamer(buffer,%s,%s);\n",objectStoragePointer,
                          objectPointer);
   if (bypass) {
      buffer.AppendFormatted("         %s->BypassStreamer(bypassOrgOld);\n",objectStoragePointer);
      buffer.AppendFormatted("         %s->BypassStreamer(bypassOld);\n",objectPointer);
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteSteeringClass(ROMEString &buffer,Int_t numSteer,Int_t numTask,Int_t tab)
{
   ROMEString sc;
   ROMEString blank="";
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i = 0; i < numOfSteerFields[numTask][numSteer]; i++) {
      if (!steerFieldUsed[numTask][numSteer][i])
         continue;
      if (typeLen < static_cast<int>(steerFieldType[numTask][numSteer][i].Length())) {
         typeLen = steerFieldType[numTask][numSteer][i].Length();
      }
      if (nameLen < static_cast<int>(steerFieldName[numTask][numSteer][i].Length())) {
         nameLen = steerFieldName[numTask][numSteer][i].Length();
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (typeLen < static_cast<int>(steerName[numTask][steerChildren[numTask][numSteer][i]].Length() + 1)) {
         typeLen = steerName[numTask][steerChildren[numTask][numSteer][i]].Length() + 1;
      }
      if (nameLen < static_cast<int>(steerName[numTask][steerChildren[numTask][numSteer][i]].Length())) {
         nameLen = steerName[numTask][steerChildren[numTask][numSteer][i]].Length();
      }
   }
   if (tab == 0) {
      sc = shortCut;
   } else {
      sc = "";
   }
   for (i = 0 ; i < tab; i++)
      blank.Append("   ");

   buffer.AppendFormatted("\n%sclass %s%s\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("%s{\n",blank.Data());

   buffer.AppendFormatted("%spublic:\n",blank.Data());

   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      WriteSteeringClass(buffer,steerChildren[numTask][numSteer][i],numTask,tab + 1);
   }

   buffer.AppendFormatted("%sprotected:\n",blank.Data());

   ROMEString tmp;

   // Fields
   for (j = 0; j < numOfSteerFields[numTask][numSteer]; j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j] == "1") {
         buffer.AppendFormatted("%s   %-*s f%s;%*s %s\n",blank.Data(),
                                typeLen,steerFieldType[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data(),
                                nameLen+5-steerFieldName[numTask][numSteer][j].Length(), "",
                                ProcessCommentCPP(steerFieldComment[numTask][numSteer][j],tmp).Data());
      } else {
         buffer.AppendFormatted("%s   %-*s f%s[%s];%*s %s\n",blank.Data(),
                                typeLen,steerFieldType[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data(),
                                steerFieldArraySize[numTask][numSteer][j].Data(),
                                nameLen + 3 - steerFieldName[numTask][numSteer][j].Length() -
                                steerFieldArraySize[numTask][numSteer][j].Length(),"",
                                ProcessCommentCPP(steerFieldComment[numTask][numSteer][j],tmp).Data());
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] == "1") {
         buffer.AppendFormatted("%s   %-*s *f%s;%*s // Handle to %s Class\n",blank.Data(),
                                typeLen - 1, steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                nameLen + 1 - steerName[numTask][steerChildren[numTask][numSteer][i]].Length(),"",
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      } else {
         buffer.AppendFormatted("%s   %-*s **f%s;%*s // Handle to %s Class\n",blank.Data(),typeLen - 1,
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length(),"",
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("\n");
   // Constructor
   buffer.AppendFormatted("%sprivate:\n",blank.Data());
   buffer.AppendFormatted("%s   %s%s(const %s%s &c); // not implemented\n",blank.Data(),
                          sc.Data(),steerName[numTask][numSteer].Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("%s   %s%s &operator=(const %s%s &c); // not implemented\n",blank.Data(),
                          sc.Data(),steerName[numTask][numSteer].Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%spublic:\n",blank.Data());

   buffer.AppendFormatted("%s   %s%s()\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   ROMEString separator = ":";
   for (j = 0; j < numOfSteerFields[numTask][numSteer]; j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j] == "1") {
         if (steerFieldType[numTask][numSteer][j] == "std::string") {
            buffer.AppendFormatted("%s   %sf%s(%s)\n",blank.Data(),separator.Data(),
                                   steerFieldName[numTask][numSteer][j].Data(),
                                   steerFieldInit[numTask][numSteer][j].Data());
            separator = ",";
         } else {
            buffer.AppendFormatted("%s   %sf%s(static_cast<%s>(%s))\n",blank.Data(),separator.Data(),
                                   steerFieldName[numTask][numSteer][j].Data(),
                                   steerFieldType[numTask][numSteer][j].Data(),
                                   steerFieldInit[numTask][numSteer][j].Data());
            separator = ",";
         }
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]]) {
         continue;
      }
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] == "1") {
         buffer.AppendFormatted("%s   %sf%s(new %s())\n",blank.Data(),separator.Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
         separator = ",";
      } else {
         buffer.AppendFormatted("%s   %sf%s(new %s*[%s])\n",blank.Data(),separator.Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data());
         separator = ",";
      }
   }
   buffer.AppendFormatted("%s   {\n",blank.Data());
   for (j = 0; j < numOfSteerFields[numTask][numSteer]; j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j] != "1") {
         buffer.AppendFormatted("int j;\n");
         break;
      }
   }
   for (j = 0; j < numOfSteerFields[numTask][numSteer]; j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j] == "1") {
      } else {
         if (steerFieldType[numTask][numSteer][j] == "std::string") {
         } else {
            buffer.AppendFormatted("%s   for (j = 0; j < %s; j++) f%s[j] = static_cast<%s>(%s);\n",blank.Data(),
                                   steerFieldArraySize[numTask][numSteer][j].Data(),
                                   steerFieldName[numTask][numSteer][j].Data(),
                                   steerFieldType[numTask][numSteer][j].Data(),
                                   steerFieldInit[numTask][numSteer][j].Data());
         }
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] != "1") {
         buffer.AppendFormatted("int i;\n");
         break;
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] == "1") {
      } else {
         buffer.AppendFormatted("%s   for (i = 0; i < %s; i++) f%s[i] = new %s();\n",blank.Data(),
                                steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("%s   }\n",blank.Data());
   buffer.AppendFormatted("\n");

   // Destructor
   buffer.AppendFormatted("%s   ~%s%s()\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("%s   {\n",blank.Data());
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] != "1") {
         buffer.AppendFormatted("%s   int i; ",blank.Data());
         break;
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] == "1") {
         buffer.AppendFormatted("%s   delete f%s;\n",blank.Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      } else {
         buffer.AppendFormatted("%s    for (i = 0; i < %s; i++) delete f%s[i]; delete [] f%s;\n",blank.Data(),
                                steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("%s   };\n",blank.Data());

   // Getters
   for (j = 0; j < numOfSteerFields[numTask][numSteer]; j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j] == "1") {
         buffer.AppendFormatted("%s   %-*s Get%s() const%*s { return f%s; }\n",blank.Data(),typeLen,
                                steerFieldType[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data(),
                                nameLen + 7 - steerFieldName[numTask][numSteer][j].Length(),"",
                                steerFieldName[numTask][numSteer][j].Data());
      } else {
         buffer.AppendFormatted("%s   %-*s Get%sAt(Int_t i) const%*s{ return f%s[i]; }\n",blank.Data(),typeLen,
                                steerFieldType[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data(),
                                nameLen - steerFieldName[numTask][numSteer][j].Length(),"",
                                steerFieldName[numTask][numSteer][j].Data());
      }
   }
   for (i = 0; i < numOfSteerChildren[numTask][numSteer]; i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]] == "1") {
         buffer.AppendFormatted("%s   %-*s *Get%s()%*s const { return f%s; }\n",blank.Data(),
                                typeLen - 1,
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                nameLen + 7 - steerName[numTask][steerChildren[numTask][numSteer][i]].Length(),"",
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      } else {
         buffer.AppendFormatted("%s   %-*s *Get%sAt(Int_t i) const%*s { return f%s[i]; }\n",blank.Data(),
                                typeLen - 1, steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),
                                nameLen - steerName[numTask][steerChildren[numTask][numSteer][i]].Length(),"",
                                steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   // Setters
   buffer.AppendFormatted("\n");
   for (j = 0; j < numOfSteerFields[numTask][numSteer]; j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j] == "1") {
         buffer.AppendFormatted("%s   void Set%s(%s %s)%*s { f%s = %s; }\n",blank.Data(),steerFieldName[numTask][numSteer][j].Data(),
                                steerFieldType[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data(),
                                2 * nameLen + typeLen + 8 - 2 * steerFieldName[numTask][numSteer][j].Length() -
                                steerFieldType[numTask][numSteer][j].Length(),"",
                                steerFieldName[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data());
      } else {
         buffer.AppendFormatted("%s   void Set%sAt(Int_t i,%s %s)%*s { f%s[i] = %s; }\n",
                                blank.Data(),steerFieldName[numTask][numSteer][j].Data(),
                                steerFieldType[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data(),
                                2 * nameLen + typeLen - 2 * steerFieldName[numTask][numSteer][j].Length() -
                                steerFieldType[numTask][numSteer][j].Length(),"",
                                steerFieldName[numTask][numSteer][j].Data(),
                                steerFieldName[numTask][numSteer][j].Data());
      }
   }

   // Footer
   buffer.AppendFormatted("%s};\n\n",blank.Data());

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteSteeringReadParameters(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,
                                                ROMEString& steerPointer)
{
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   int k;
   // Fields
   for (k = 0; k < numOfSteerFields[numTask][numSteer]; k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldCLOption[numTask][numSteer][k].Length()) {
         if (steerFieldArraySize[numTask][numSteer][k] == "1") {
            buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("   if (option == \"-%s\") {\n",steerFieldCLOption[numTask][numSteer][k].Data());
            if (isBoolType(steerFieldType[numTask][numSteer][k].Data())) {
               buffer.AppendFormatted("      %s->Set%s(true);\n",steerPointer.Data(),
                                      steerFieldName[numTask][numSteer][k].Data());
            } else {
               buffer.AppendFormatted("      if (!strlen(value))\n");
               buffer.AppendFormatted("         ROMEPrint::Warning(\"warning: %s might not be specified properly.\\n\");\n",
                                      steerFieldName[numTask][numSteer][k].Data());
               setValue(&decodedValue,"","value",steerFieldType[numTask][numSteer][k].Data(), 1);
               buffer.AppendFormatted("      %s->Set%s(static_cast<%s>(%s));\n",steerPointer.Data(),
                                      steerFieldName[numTask][numSteer][k].Data(),
                                      steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
               buffer.AppendFormatted("      i++;\n");
            }
            buffer.AppendFormatted("      return true;\n");
            buffer.AppendFormatted("   }\n");
         } else {
            buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("   for (int i_%s = 0; i_%s < %s; i_%s++) {\n",
                                   steerFieldCLOption[numTask][numSteer][k].Data(),
                                   steerFieldCLOption[numTask][numSteer][k].Data(),
                                   steerFieldArraySize[numTask][numSteer][k].Data(),
                                   steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      tmp = \"-%s\";\n",steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      tmp += i_%s;\n",steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      if (option == tmp) {\n");
            if (isBoolType(steerFieldType[numTask][numSteer][k].Data())) {
               buffer.AppendFormatted("         %s->Set%sAt(i_%s,true);\n",steerPointer.Data(),
                                      steerFieldName[numTask][numSteer][k].Data(),
                                      steerFieldCLOption[numTask][numSteer][k].Data());
            } else {
               buffer.AppendFormatted("         if (!strlen(value))\n");
               buffer.AppendFormatted("            ROMEPrint::Warning(\"warning: %s might not be specified properly.\\n\");\n",
                                      steerFieldName[numTask][numSteer][k].Data());
               setValue(&decodedValue,"","value",steerFieldType[numTask][numSteer][k].Data(), 1);
               buffer.AppendFormatted("         %s->Set%sAt(i_%s,static_cast<%s>(%s));\n",steerPointer.Data(),
                                      steerFieldName[numTask][numSteer][k].Data(),
                                      steerFieldCLOption[numTask][numSteer][k].Data(),
                                      steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
               buffer.AppendFormatted("         i++;\n");
            }
            buffer.AppendFormatted("         return true;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   // Groups
   for (k = 0; k < numOfSteerChildren[numTask][numSteer]; k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringReadParameters(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteSteeringParameterUsage(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,
                                                ROMEString& steerPointer)
{
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   ROMEString arrayIndex;
   int nspace;
   char *cstop=0;
   int k;
   // Fields
   for (k = 0; k < numOfSteerFields[numTask][numSteer]; k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldCLOption[numTask][numSteer][k].Length()) {
         arrayIndex = "";
         if (steerFieldArraySize[numTask][numSteer][k] != "1")
            arrayIndex.SetFormatted("[0-%ld]",strtol(steerFieldArraySize[numTask][numSteer][k].Data(),&cstop, 10) - 1);
         nspace = 8-steerFieldCLOption[numTask][numSteer][k].Length()-arrayIndex.Length();
         if (nspace < 1)
            nspace = 1;
         buffer.AppendFormatted("   ROMEPrint::Print(\"  -%s%s%*s%s",steerFieldCLOption[numTask][numSteer][k].Data(),arrayIndex.Data(),nspace,"",
                                steerFieldCLDescription[numTask][numSteer][k].Data());
         if (isBoolType(steerFieldCLOption[numTask][numSteer][k].Data()))
            buffer.AppendFormatted(" (no Argument)");
         buffer.AppendFormatted("\\n\");\n");
      }
   }
   // Groups
   for (k = 0; k < numOfSteerChildren[numTask][numSteer]; k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringParameterUsage(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}

//______________________________________________________________________________
Int_t ROMEBuilder::WriteSteeringInterpreterCode(ROMEString &buffer,Int_t codeNumber,Int_t numSteer,Int_t numTask,
                                                ROMEString& path1,ROMEString& path2,Int_t tab)
{
   ROMEString path1T;
   ROMEString path2T;
   ROMEString blank = "";
   for (int i = 0; i < tab; i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k = 0; k < numOfSteerFields[numTask][numSteer]; k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldArraySize[numTask][numSteer][k] == "1") {
         buffer.AppendFormatted("%s   if (path == \"%s/%s\")\n",blank.Data(),path1.Data(),
                                steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return %d;\n",blank.Data(),codeNumber);
         buffer.AppendFormatted("%s   if (path.Index(\"%s/%s\") != -1)\n",blank.Data(),path1.Data(),
                                steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return -2;\n",blank.Data());
         buffer.AppendFormatted("%s   if (path == \"%s->Get%s()\")\n",blank.Data(),path2.Data(),
                                steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return %d;\n",blank.Data(),codeNumber);
         buffer.AppendFormatted("%s   if (path.Index(\"%s->Get%s()\") != -1)\n",blank.Data(),path2.Data(),
                                steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return -2;\n",blank.Data());
         codeNumber++;
      }
   }
   // Groups
   for (k = 0; k < numOfSteerChildren[numTask][numSteer]; k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]] == "1") {
         path1T = path1;
         path2T = path2;
         path1.AppendFormatted("/%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         path2.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber,steerChildren[numTask][numSteer][k],numTask,path1,
                                                   path2,tab);
         path1 = path1T;
         path2 = path2T;
      }
   }
   return codeNumber;
}

//______________________________________________________________________________
Int_t ROMEBuilder::WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,Int_t codeNumber,Int_t numSteer,
                                                 Int_t numTask,ROMEString& steerPointer,Int_t tab)
{
   ROMEString steerPointerT;
   ROMEString str;
   ROMEString blank = "";
   ROMEString stringBuffer = "buffer";
   for (int i = 0; i < tab; i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k = 0; k < numOfSteerFields[numTask][numSteer]; k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldArraySize[numTask][numSteer][k] == "1") {
         buffer.AppendFormatted("%s   case %d:\n",blank.Data(),codeNumber);
         str.SetFormatted("%s->Get%s()",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         stringBuffer = "buffer";
         buffer.AppendFormatted("%s      return %s;\n",blank.Data(),
                                convertType(str.Data(), steerFieldType[numTask][numSteer][k].Data(),
                                            type,stringBuffer).Data());
         codeNumber++;
      }
   }
   // Groups
   for (k = 0; k < numOfSteerChildren[numTask][numSteer]; k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]] == "1") {
         steerPointerT = steerPointer;
         steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         codeNumber = WriteSteeringInterpreterValue(buffer,type,codeNumber,steerChildren[numTask][numSteer][k],numTask,
                                                    steerPointer,tab);
         steerPointer = steerPointerT;
      }
   }
   return codeNumber;
}

//______________________________________________________________________________
void ROMEBuilder::WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName)
{
   int i,j;
   ROMEString tmp;
   ROMEString stringBuffer = "buffer";
   if (!strcmp(type,"ROMEString&")) {
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%s %sAnalyzer::GetObjectInterpreter%sValue(Int_t code,%s defaultValue,%s %s) const\n{\n",
                             type, shortCut.Data(),fctName,type,type,stringBuffer.Data());
   } else {
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%s %sAnalyzer::GetObjectInterpreter%sValue(Int_t code,%s defaultValue) const\n{\n",type,
                             shortCut.Data(),fctName,type);
      buffer.AppendFormatted("   ROMEString buffer;\n");
   }
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   cstop = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   switch (code) {\n");
   buffer.AppendFormatted("      case -1:\n");
   buffer.AppendFormatted("         return defaultValue;\n");
   ROMEString steerPointer = "gAnalyzer->GetGSP()";
   int codeNumber = WriteSteeringInterpreterValue(buffer,type, 0, 0, numOfTask,steerPointer, 1);
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderArray[i] == "1" && !folderSupport[i]) {
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] == 0 && !isFolder(valueType[i][j].Data()) && !valueIsTObject[i][j]) {
               buffer.AppendFormatted("      case %d:\n",codeNumber);
               tmp.SetFormatted("gAnalyzer->Get%s()->Get%s()",folderName[i].Data(),valueName[i][j].Data());
               stringBuffer = "buffer";
               buffer.AppendFormatted("         return %s;\n",convertType(tmp.Data(),valueType[i][j].Data(),type,
                                                                          stringBuffer).Data());
               codeNumber++;
            }
         }
      }
   }
   buffer.AppendFormatted("   }\n");
   if (!strcmp(type,"ROMEString&")) {
      buffer.AppendFormatted("   return %s;\n",stringBuffer.Data());
   } else {
      buffer.AppendFormatted("   return 0;\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
}

//______________________________________________________________________________
void ROMEBuilder::WriteReadDataBaseFolder(ROMEString &buffer,Int_t numFolder,Int_t type)
{
   // type == 1 : single folder
   // type == 2 : array folder
   int j,k;
   ROMEString valueString;
   ROMEString buf;
   ROMEString blank = "";
   ROMEString iValue = "0";
   ROMEString tempBuffer;
   if (type == 2) {
      blank = "   ";
      iValue= "i";
      buffer.AppendFormatted("   int i;\n");
      buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warnin
      buffer.AppendFormatted("   int nentry;\n");
      buffer.AppendFormatted("   nentry = 0;\n"); // to suppress unused warnin
   }
   buffer.AppendFormatted("   int j;\n");
   buffer.AppendFormatted("   j = 0;\n"); // to suppress unused warnin
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   str = \"\";\n"); // to suppress unused warnin
   buffer.AppendFormatted("   ROMEString path;\n");
   buffer.AppendFormatted("   path = \"\";\n"); // to suppress unused warnin
   buffer.AppendFormatted("   ROMEString name;\n");
   if (maxNumberOfPathObjectInterpreterCodes > 0) {
      buffer.AppendFormatted("   ROMEString buffer[%d];\n",maxNumberOfPathObjectInterpreterCodes);
      buffer.AppendFormatted("   buffer[0] = \"\";\n"); // to suppress unused warnin
   }
   buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1, 1);\n");
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   cstop = 0;\n"); // to suppress unused warning
   for (j = 0; j < numOfValue[numFolder]; j++) {
      if (valueDimension[numFolder][j] > 1 || valueArray[numFolder][j][0] == "variable" ||
          isFolder(valueType[numFolder][j].Data())) {
         continue;
      }

      if (valueIsTObject[numFolder][j]) {
         continue;
      }

#if 1 // forbid DB access for fields without DB path in definixion XML file
      if (!valueDBPath[numFolder][j].Length()) {
         continue;
      }
#endif

      if (valueDBIf[numFolder][j].Length()) {
         buffer.AppendFormatted("   if (%s) {\n", valueDBIf[numFolder][j].Data());
      } else {
         buffer.AppendFormatted("   {\n");
      }
      if (folderArray[numFolder] == "1" && type == 1 || folderArray[numFolder] != "1" && type == 2 &&
          !folderSupport[numFolder]) {
         buffer.AppendFormatted("      values->RemoveAll();\n");
         buffer.AppendFormatted("      if (fDBName[%d][%d].Length() == 0)\n",numFolder,j);
         if (valueDBName[numFolder][j].Length()) {
            buffer.AppendFormatted("         name.SetFormatted(\"%s\");\n",valueDBName[numFolder][j].Data());
         } else {
            buffer.AppendFormatted("         name.Resize(0);\n");
         }
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         name.SetFormatted(fDBName[%d][%d].Data());\n",numFolder,j);
         buffer.AppendFormatted("      if (fDBPath[%d][%d].Length() == 0)\n",numFolder,j);
#if 0 // allow DB access for fields without DB path in definixion XML file
         if (valueDBPath[numFolder][j].Length()) {
            buffer.AppendFormatted("         path.SetFormatted(%s);\n",valueDBPath[numFolder][j].Data());
         } else {
            buffer.AppendFormatted("         path.Resize(0);\n");
         }
#else
         buffer.AppendFormatted("         path.SetFormatted(%s);\n",valueDBPath[numFolder][j].Data());
#endif

         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         path.SetFormatted(fDBPath[%d][%d].Data()",numFolder,j);
         for (k = 0; k < maxNumberOfPathObjectInterpreterCodes; k++)
            buffer.AppendFormatted("\n            ,gAnalyzer->GetObjectInterpreterCharValue(fDBCode[%d][%d]->At(%d),buffer[%d],buffer[%d]).Data()",
                                   numFolder,j,k,k,k);
         buffer.AppendFormatted(");\n");
         buffer.AppendFormatted("      if (name.Length() && path.Length() && gAnalyzer->isDataBaseActive(name.Data())) {\n");
         buffer.AppendFormatted("         ROMEPrint::Debug(\"Reading database %%s(type=%%s, path=%%s)\\n\",name.Data(),gAnalyzer->GetDataBase(name.Data())->GetType(),path.Data());\n");
         buffer.AppendFormatted("         if (gAnalyzer->GetDataBase(name.Data())->Read(values,path,gAnalyzer->GetCurrentRunNumber(),gAnalyzer->GetCurrentEventNumber())) {\n");
         buffer.AppendFormatted("            ROMEPrint::Debug(\"%s/%s was filled.\\n\");\n",
                                folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("         } else {\n");
         buffer.AppendFormatted("            ROMEPrint::Error(\"Error in Folder '%s' Value '%s'.\\n\");\n",
                                folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("            return false;\n");
         buffer.AppendFormatted("         }\n");
         if (type == 2) {
            buffer.AppendFormatted("         nentry = gAnalyzer->Get%ss()->GetEntriesFast();\n",
                                   folderName[numFolder].Data());
            buffer.AppendFormatted("         for (i = 0; i < nentry; i++) {\n");
         }
         if (valueDimension[numFolder][j] == 0) {
            buffer.AppendFormatted("%s         if (values->At(%s, 0).Length() != 0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s, 0).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),valueType[numFolder][j].Data(), 1);
            if (type == 1) {
               buffer.AppendFormatted("%s            gAnalyzer->Get%s()->Set%s(static_cast<%s>(%s));\n",blank.Data(),
                                      folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("%s         else\n",blank.Data());
               buffer.AppendFormatted("%s            gAnalyzer->Get%s()->Set%s(%s);\n",blank.Data(),
                                      folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      valueInit[numFolder][j].Data());
            } else {
               buffer.AppendFormatted("               gAnalyzer->Get%sAt(i)->Set%s(static_cast<%s>(%s));\n",
                                      folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("            else\n");
               buffer.AppendFormatted("               gAnalyzer->Get%sAt(i)->Set%s(%s);\n",
                                      folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      valueInit[numFolder][j].Data());
            }
         } else if (valueDimension[numFolder][j] == 1) {
            buffer.AppendFormatted("%s         for (j = 0; j < %s; j++) {\n",blank.Data(),
                                   valueArray[numFolder][j][0].Data());
            buffer.AppendFormatted("%s            if (values->At(%s,j).Length() != 0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s,j).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),
                     TArray2StandardType(valueType[numFolder][j],tempBuffer), 1);
            if (type == 1) {
               buffer.AppendFormatted("%s               gAnalyzer->Get%s()->Set%sAt(j, static_cast<%s>(%s));\n",
                                      blank.Data(), folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      TArray2StandardType(valueType[numFolder][j],tempBuffer),buf.Data());
               buffer.AppendFormatted("%s            else\n",blank.Data());
               buffer.AppendFormatted("%s               gAnalyzer->Get%s()->Set%sAt(j,%s);\n",blank.Data(),
                                      folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      valueInit[numFolder][j].Data());
            } else {
               buffer.AppendFormatted("                  gAnalyzer->Get%sAt(i)->Set%sAt(j, static_cast<%s>(%s));\n",
                                      folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      TArray2StandardType(valueType[numFolder][j],tempBuffer),buf.Data());
               buffer.AppendFormatted("               else\n");
               buffer.AppendFormatted("                  gAnalyzer->Get%sAt(i)->Set%sAt(j,%s);\n",
                                      folderName[numFolder].Data(),valueName[numFolder][j].Data(),
                                      valueInit[numFolder][j].Data());
            }
            buffer.AppendFormatted("%s         }\n",blank.Data());
         } else {
            cout<<"Warning: multiple dimension field can not have database connection."<<endl;
            return;
         }
         if (type == 2)
            buffer.AppendFormatted("         }\n");
      }
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   values->RemoveAll();\n");
   buffer.AppendFormatted("   delete values;\n");
}

//______________________________________________________________________________
void ROMEBuilder::WriteFolderGetterInclude(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   if (FolderToBeGenerated(numFolder)) {
      if (folderArray[numFolder] == "1") {
         buffer.AppendFormatted("   %s%s* Get%s();\n",shortCut.Data(),folderName[numFolder].Data(),
                                folderName[numFolder].Data());
         buffer.AppendFormatted("   %s%s** Get%sAddress();\n",shortCut.Data(),folderName[numFolder].Data(),
                                folderName[numFolder].Data());
         buffer.AppendFormatted("   %s%s* Get%sFolderStorage() const { return f%sFolderStorage; } \n",shortCut.Data(),
                                folderName[numFolder].Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
      } else {
         buffer.AppendFormatted("   %s%s* Get%sAt(Int_t indx);\n",shortCut.Data(),folderName[numFolder].Data(),
                                folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray* Get%ss();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray** Get%sAddress();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray* Get%sFoldersStorage() const { return f%sFoldersStorage; }\n",
                                folderName[numFolder].Data(),folderName[numFolder].Data());
      }
      buffer.AppendFormatted("   void   Update%s();\n", folderName[numFolder].Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteFolderSetterInclude(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   if (FolderToBeGenerated(numFolder)) {
      if (folderArray[numFolder] == "1") {
         buffer.AppendFormatted("   void Set%s(%s%s* pointer);\n",folderName[numFolder].Data(),shortCut.Data(),
                                folderName[numFolder].Data());
      } else {
         buffer.AppendFormatted("   void Set%ss(TClonesArray* pointer);\n",folderName[numFolder].Data());
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteFolderGetterSource(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   if (FolderToBeGenerated(numFolder)) {
      if (folderArray[numFolder] == "1") {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%s()\n{\n",shortCut.Data(),folderName[numFolder].Data(),
                                shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolder = static_cast<%s%s*>(GetSocketClientNetFolder()->FindObjectAny(\"%s%s\"));\n",
                                folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data(),
                                shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return f%sFolder;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("%s%s** %sAnalyzer::Get%sAddress()\n{\n",shortCut.Data(),folderName[numFolder].Data(),
                                shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      Update%s();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   return &f%sFolder;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sAnalyzer::Update%s()\n{\n",shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolder = static_cast<%s%s*>(GetSocketClientNetFolder()->FindObjectAny(\"%s%s\"));\n",
                                folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data(),
                                shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      } else if (folderArray[numFolder] == "variable") {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%sAt(Int_t indx)\n{\n",shortCut.Data(),
                                folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      Update%s();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   if (f%sFolders->GetEntriesFast() <= indx)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("      FolderArrayOutOfBouds(indx,\"%s\",\"%s\");\n",folderName[numFolder].Data(),folderArray[numFolder].Data());
         buffer.AppendFormatted("   return static_cast<%s%s*>(f%sFolders->At(indx));\n",shortCut.Data(),
                                folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("TClonesArray* %sAnalyzer::Get%ss()\n{\n",shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      Update%s();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   return f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("TClonesArray** %sAnalyzer::Get%sAddress()\n{\n",shortCut.Data(),
                                folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      Update%s();\n", folderName[numFolder].Data());
         buffer.AppendFormatted("   return &f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sAnalyzer::Update%s()\n{\n",shortCut.Data(), folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = static_cast<TClonesArray*>(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",
                                folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      } else {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%sAt(Int_t indx)\n{\n",shortCut.Data(),
                                folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      Update%s();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   if (f%sFolders->GetEntriesFast() <= indx)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("      FolderArrayOutOfBouds(indx,\"%s\",\"%s\");\n",folderName[numFolder].Data(),folderArray[numFolder].Data());
         buffer.AppendFormatted("   return static_cast<%s%s*>(f%sFolders->At(indx));\n",shortCut.Data(),
                                folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("TClonesArray* %sAnalyzer::Get%ss()\n{\n",shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      Update%s();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   return f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("TClonesArray** %sAnalyzer::Get%sAddress()\n{\n",shortCut.Data(),
                                folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      Update%s();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   return &f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sAnalyzer::Update%s()\n{\n",shortCut.Data(), folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = static_cast<TClonesArray*>(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",
                                folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteFolderSetterSource(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   if (FolderToBeGenerated(numFolder)) {
      if (folderArray[numFolder] == "1") {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sAnalyzer::Set%s(%s%s* pointer)\n{\n",shortCut.Data(),
                                folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());

         buffer.AppendFormatted("   SafeDelete(f%sFolder);\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   f%sFolder = pointer;\n",folderName[numFolder].Data());

         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      } else {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sAnalyzer::Set%ss(TClonesArray* pointer)\n{\n",shortCut.Data(),
                                folderName[numFolder].Data());

         buffer.AppendFormatted("   SafeDelete(f%sFolders);\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   f%sFolders = pointer;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteHTMLSteering(ROMEString &buffer,Int_t numSteer,Int_t numTask,const char* group)
{
   int k;
   ROMEString groupName;
   ROMEString tmp;
   bool trodd = true;
   for (k = 0; k < numOfSteerFields[numTask][numSteer]; k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",
                             trodd ? "odd" : "even",
                             steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),
                             ProcessCommentHTML(steerFieldComment[numTask][numSteer][k],tmp).Data());
      trodd = !trodd;
   }
   // Groups
   for (k = 0; k < numOfSteerChildren[numTask][numSteer]; k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      groupName = group;
      if (groupName.Length())
         groupName.AppendFormatted("/");
      groupName.AppendFormatted("%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("<tr class=\"group\"><td colspan=\"3\">&nbsp;%s&nbsp;</td></tr>\n",groupName.Data());
      WriteHTMLSteering(buffer,steerChildren[numTask][numSteer][k],numTask,groupName.Data());
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteHTMLStyle(ROMEString& buffer)
{
   buffer.AppendFormatted("   body { color: #000000; background-color: #ffffff; }\n");
   buffer.AppendFormatted("   h1 { color: blue; }\n");
   buffer.AppendFormatted("   h2 { color: green; }\n");
   buffer.AppendFormatted("   table { border: 1; }\n");
   buffer.AppendFormatted("   a.object { text-decoration: underline; font-weight: bold; }\n");
   buffer.AppendFormatted("   tr.cont { color: #ffffff; background-color: #819cc8; font-weight: bold; }\n");
   buffer.AppendFormatted("   tr.group { background-color: #d0d0ee; }\n");
   buffer.AppendFormatted("   tr.even { background-color: #e0e0e0; }\n");
   buffer.AppendFormatted("   tr.odd { background-color: #f0f0f0; }\n");
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReplaceHeader(const char* filename,const char* header,const char* body,Int_t nspace,
                                  const char* str1, const char* str2,const char* condition, bool replaceWhenFound)
{
   ROMEStrArray arr1;
   ROMEStrArray arr2;
   ROMEStrArray cond;
   TArrayI rep(1);
   arr1.Add(str1);
   arr2.Add(str2);
   cond.Add(condition);
   rep.AddAt(replaceWhenFound ? 1 : 0, 0);
   return ReplaceHeader(filename,header,body,nspace,arr1,arr2,cond,rep);
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReplaceHeader(const char* filename,const char* header,const char* body,Int_t nspace,
                                  ROMEStrArray &arr1, ROMEStrArray &arr2, ROMEStrArray &condition,
                                  TArrayI &replaceWhenFound)
{
   int i;
   bool writeFile = false;
   fstream *fileStream;
   ROMEString fileBuffer;
   Ssiz_t pBuffer = -1;
   ROMEString buffer;
   if (header != 0)
      buffer = header;
   if (gSystem->AccessPathName(filename, kFileExists)) {
      writeFile = true;
      buffer += body;
   } else {
      if (fileBuffer.ReadFile(filename, kTRUE) < 0) {
         if (makeOutput)
            cout<<"\n\nError : Failed to open '"<<filename<<"' !!!"<<endl;
         return false;
      }
      if (header != 0) {
         pBuffer = fileBuffer.Index(kHeaderEndMark);
         if (pBuffer < 0) {
            if (makeOutput) {
               cout<<"\n\nWarning : File '"<<filename
                   <<"' does not have header end mark. Builder does not modify this file."<<endl;
            }
            return true;
         }
         // compare old and new header
         if (fileBuffer(0, pBuffer+80) != header) {
            writeFile = true;
         }
      }
      //check if the file has string to be replaced.
      for (i = 0; i < TMath::Min(TMath::Min(arr1.GetEntriesFast(),arr2.GetEntriesFast()),
                                 TMath::Min(condition.GetEntriesFast(),replaceWhenFound.GetSize())); i++) {
         if (arr1.At(i).Length() > 0  && fileBuffer.ContainsFast(arr1.At(i))) {
            if (condition.At(i).Length() > 0 && (fileBuffer.ContainsFast(condition.At(i)) && replaceWhenFound.At(i)) ||
                (!fileBuffer.ContainsFast(condition.At(i)) && !replaceWhenFound.At(i))) {
               writeFile = true;
               fileBuffer.ReplaceAll(arr1.At(i), arr2.At(i));
            }
         }
      }
#if 0
// this special treatment is neccesary only for several month from Jun.2006
      //check if the file has string to be replaced.
      ROMEString filenameStr = filename;
      ROMEString analyzerInclude[4];
      analyzerInclude[0].SetFormatted("#include <generated/%sAnalyzer.h>",shortCut.Data());
      analyzerInclude[1].SetFormatted("#include <include/generated/%sAnalyzer.h>",shortCut.Data());
      analyzerInclude[2].SetFormatted("#include \"generated/%sAnalyzer.h\"",shortCut.Data());
      analyzerInclude[3].SetFormatted("#include \"include/generated/%sAnalyzer.h\"",shortCut.Data());
      if (filenameStr.ContainsFast("src/tasks") || filenameStr.ContainsFast("src/tabs")) {
         if (!fileBuffer.ContainsFast(analyzerInclude[0])
             && !fileBuffer.ContainsFast(analyzerInclude[1])
             && !fileBuffer.ContainsFast(analyzerInclude[2])
             && !fileBuffer.ContainsFast(analyzerInclude[3])
             ) {
            writeFile = true;
            fileBuffer.Insert(strlen(header), analyzerInclude[2]);
            fileBuffer.Insert(strlen(header), "\n");
         }
      }
#endif
      if (header == 0)
         buffer = fileBuffer;
   }
   if (writeFile) {
      if (pBuffer >= 0)
         buffer += fileBuffer(pBuffer+80, fileBuffer.Length());
      if (!(fileStream = new fstream(filename,ios::out | ios::trunc))) {
         if (makeOutput) cout<<"\n\nError : Failed to open '"<<filename<<"' !!!"<<endl;
         return false;
      }
      if (makeOutput) cout<<setw(nspace)<<""<<filename<<endl;
      *fileStream<<buffer.Data();
      delete fileStream;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::BackUpFile(const char* filename)
{
   // return true when backup file is created.
   if (gSystem->AccessPathName(filename, kFileExists))
      return false;

   ROMEString newName;
   Int_t i;
   int errcode; // error code is int instead of Int_t
   for (i = 0; i < kMaxInt; i++) {
      if (i == 0) {
         newName.SetFormatted("%s.save", filename);
      } else {
         newName.SetFormatted("%s.save.%d", filename, i);
      }

      if (gSystem->AccessPathName(newName.Data(), kFileExists)) {
         errcode = gSystem->Rename(filename, newName.Data());
         if (errcode) {
            cerr<<"Error:"<<errcode<<"\t"<<gSystem->GetErrorStr()<<endl;
            return false;
         } else {
            if (makeOutput)
               cout<<"   renamed '"<<filename<<"' -> '"<<newName<<"'."<<endl;
            return true;
         }
      }
   }
   return true;
}

static map<string, int> fgAccessFolderCache;
//______________________________________________________________________________
Bool_t ROMEBuilder::accessFolder(const char* filename, Int_t numFolder,
                                 Bool_t includeUnused)
{
   if (folderSupport[numFolder]) {
      return false;
   }
   if (!includeUnused && !FolderToBeGenerated(numFolder)) {
      return false;
   }

   ROMEString key;
   ROMEString filenameMode = filename;
   if (filenameMode.BeginsWith(outDir)) {
      filenameMode.Remove(0, outDir.Length());
   }
   key.SetFormatted("%s:%d:%d", filenameMode.Data(), numFolder, includeUnused ? 1 : 0);

   if (fgAccessFolderCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessFolderCache[key.Data()] == -1) {
      return kFALSE;
   }

   static ROMEString fileNameOld = "";
   static ROMEString fileBuffer = "";
   if (fileNameOld != filename) {
      fileNameOld = filename;
      fileBuffer.ReadFile(filename, kTRUE);
   }

   Bool_t ret = accessFolderBuffer(fileBuffer, numFolder, filename, includeUnused);
//   fgAccessFolderCache[key.Data()] = ret ? 1 : -1; // must be filled in UpdateAccessCache

   return ret;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::accessFolderBuffer(ROMEString &fileBuffer, Int_t numFolder, const char* cachename,
                                       Bool_t includeUnused)
{
   if (folderSupport[numFolder]) {
      return false;
   }
   if (!includeUnused && !FolderToBeGenerated(numFolder)) {
      return false;
   }

   ROMEString key;
   ROMEString cachenameMod = cachename;
   if (cachenameMod.BeginsWith(outDir)) {
      cachenameMod.Remove(0, outDir.Length());
   }
   key.SetFormatted("%s:%d:%d", cachenameMod.Data(), numFolder, includeUnused ? 1 : 0);

   if (fgAccessFolderCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessFolderCache[key.Data()] == -1) {
      return kFALSE;
   }

   UpdateAccessCache(fileBuffer, cachename);

   if (fgAccessFolderCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessFolderCache[key.Data()] == -1) {
      return kFALSE;
   }

#if 0
   ROMEString str;

   // Get
   str = "Get";
   str += folderName[numFolder];
   if (folderArray[numFolder] != "1") {
      str += "At";
   }
   str += "(";
   if (fileBuffer.ContainsFast(str)) {
      return true;
   }
   str.ReplaceAll("(", " (");
   if (fileBuffer.ContainsFast(str)) {
      return true;
   }

   // Get s
   if (folderArray[numFolder] != "1") {
      str = "Get";
      str += folderName[numFolder];
      str += "s";
      str += "(";
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
      str.ReplaceAll("(", " (");
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
   }

   // Get Address
   str = "Get";
   str += folderName[numFolder];
   str += "Address";
   str += "(";
   if (fileBuffer.ContainsFast(str)) {
      return true;
   }
   str.ReplaceAll("(", " (");
   if (fileBuffer.ContainsFast(str)) {
      return true;
   }

   // Set size
   if (folderArray[numFolder] == "variable") {
      str = "Set";
      str += folderName[numFolder];
      str += "Size(";
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
      str.ReplaceAll("(", " (");
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
   }

   // Set Array
   if (folderArray[numFolder] == "variable") {
      str = "Set";
      str += folderName[numFolder];
      str += "s(";
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
      str.ReplaceAll("(", " (");
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
   }
#else
   cerr<<"Error of ROMEBuilder::UpdateAccessCache"
         ". Please report bug to ROME Elog (https://ladd00.triumf.ca/elog/Rome)"<<endl;
#endif

   return false;
}

static map<string, int> fgAccessHistoCache;
//______________________________________________________________________________
Bool_t ROMEBuilder::accessHisto(const char* filename, Int_t numTask, Int_t numHisto)
{
   ROMEString key;
   ROMEString filenameMode = filename;
   if (filenameMode.BeginsWith(outDir)) {
      filenameMode.Remove(0, outDir.Length());
   }
   key.SetFormatted("%s:%d:%d", filenameMode.Data(), numTask, numHisto);

   if (fgAccessHistoCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessHistoCache[key.Data()] == -1) {
      return kFALSE;
   }

   static ROMEString fileNameOld = "";
   static ROMEString fileBuffer = "";
   if (fileNameOld != filename) {
      fileNameOld = filename;
      fileBuffer.ReadFile(filename, kTRUE);
   }

   Bool_t ret = accessHistoBuffer(fileBuffer, numTask, numHisto, filename);
//   fgAccessHistoCache[key.Data()] = ret ? 1 : -1; // must be filled in UpdateAccessCache

   return ret;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::accessHistoBuffer(ROMEString &fileBuffer, Int_t numTask, Int_t numHisto, const char* cachename)
{
   ROMEString key;
   ROMEString cachenameMod = cachename;
   if (cachenameMod.BeginsWith(outDir)) {
      cachenameMod.Remove(0, outDir.Length());
   }
   key.SetFormatted("%s:%d:%d", cachenameMod.Data(), numTask, numHisto);

   if (fgAccessHistoCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessHistoCache[key.Data()] == -1) {
      return kFALSE;
   }

   UpdateAccessCache(fileBuffer, cachename);

   if (fgAccessHistoCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessHistoCache[key.Data()] == -1) {
      return kFALSE;
   }

#if 0
   ROMEString str;
   // Get
   str = "Get";
   str += histoName[numTask][numHisto];
   str += "(";
   if (fileBuffer.ContainsFast(str)) {
      return true;
   }

   // Get At
   if (histoArraySize[numTask][numHisto] != "1") {
      str = "Get";
      str += histoName[numTask][numHisto];
      str += "At";
      str += "(";
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
   }
#else
   cerr<<"Error of ROMEBuilder::UpdateAccessCache."
         " Please report bug to ROME Elog (https://ladd00.triumf.ca/elog/Rome)"<<endl;
#endif

   return false;
}

static map<string, int> fgAccessGraphCache;
//______________________________________________________________________________
Bool_t ROMEBuilder::accessGraph(const char* filename, Int_t numTask, Int_t numGraph)
{
   ROMEString key;
   ROMEString filenameMode = filename;
   if (filenameMode.BeginsWith(outDir)) {
      filenameMode.Remove(0, outDir.Length());
   }
   key.SetFormatted("%s:%d:%d", filenameMode.Data(), numTask, numGraph);

   if (fgAccessGraphCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessGraphCache[key.Data()] == -1) {
      return kFALSE;
   }

   static ROMEString fileNameOld = "";
   static ROMEString fileBuffer = "";
   if (fileNameOld != filename) {
      fileNameOld = filename;
      fileBuffer.ReadFile(filename, kTRUE);
   }

   Bool_t ret = accessGraphBuffer(fileBuffer, numTask, numGraph, filename);
//   fgAccessGraphCache[key.Data()] = ret ? 1 : -1; // must be filled in UpdateAccessCache

   return ret;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::accessGraphBuffer(ROMEString &fileBuffer, Int_t numTask, Int_t numGraph, const char* cachename)
{
   ROMEString key;
   ROMEString cachenameMod = cachename;
   if (cachenameMod.BeginsWith(outDir)) {
      cachenameMod.Remove(0, outDir.Length());
   }
   key.SetFormatted("%s:%d:%d", cachenameMod.Data(), numTask, numGraph);

   if (fgAccessGraphCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessGraphCache[key.Data()] == -1) {
      return kFALSE;
   }

   UpdateAccessCache(fileBuffer, cachename);

   if (fgAccessGraphCache[key.Data()] == 1) {
      return kTRUE;
   } else if (fgAccessGraphCache[key.Data()] == -1) {
      return kFALSE;
   }

#if 0
   ROMEString str;

   // Get
   str = "Get";
   str += graphName[numTask][numGraph];
   str += "(";
   if (fileBuffer.ContainsFast(str)) {
      return true;
   }

   // Get At
   if (graphArraySize[numTask][numGraph] != "1") {
      str = "Get";
      str += graphName[numTask][numGraph];
      str += "At";
      str += "(";
      if (fileBuffer.ContainsFast(str)) {
         return true;
      }
   }
#else
   cerr<<"Error of ROMEBuilder::UpdateAccessCache."
         " Please report bug to ROME Elog (https://ladd00.triumf.ca/elog/Rome)"<<endl;
#endif

   return false;
}

//______________________________________________________________________________
void ROMEBuilder::UpdateAccessCache(ROMEString &fileBuffer, const char* filename)
{
   
   Int_t i;
   const char *p = fileBuffer.Data();
   const Int_t l = fileBuffer.Length();

   // folders
   Int_t iFolder;
   Bool_t *accessingFolder = new Bool_t[numOfFolder];
   Int_t  *folderNameLen = new Int_t[numOfFolder];
   for (iFolder = 0; iFolder < numOfFolder; iFolder++) {
      folderNameLen[iFolder] = folderName[iFolder].Length();
      accessingFolder[iFolder] = kFALSE;
   }

   Int_t iTask;

   // histos
   Int_t iHisto;
   Bool_t **accessingHisto = new Bool_t*[numOfTask];
   Int_t  **histoNameLen = new Int_t*[numOfTask];
   for (iTask = 0; iTask < numOfTask; iTask++) {
      accessingHisto[iTask] = new Bool_t[numOfHistos[iTask]];
      histoNameLen[iTask] = new Int_t[numOfHistos[iTask]];
      for (iHisto = 0; iHisto < numOfHistos[iTask]; iHisto++) {
         histoNameLen[iTask][iHisto] = histoName[iTask][iHisto].Length();
         accessingHisto[iTask][iHisto] = kFALSE;
      }
   }

   // graphs
   Int_t iGraph;
   Bool_t **accessingGraph = new Bool_t*[numOfTask];
   Int_t  **graphNameLen = new Int_t*[numOfTask];
   for (iTask = 0; iTask < numOfTask; iTask++) {
      accessingGraph[iTask] = new Bool_t[numOfGraphs[iTask]];
      graphNameLen[iTask] = new Int_t[numOfGraphs[iTask]];
      for (iGraph = 0; iGraph < numOfGraphs[iTask]; iGraph++) {
         graphNameLen[iTask][iGraph] = graphName[iTask][iGraph].Length();
         accessingGraph[iTask][iGraph] = kFALSE;
      }
   }

   // check code
   for (i = 0; i < l; i++) {
      if (p[i] == 'G') {
         if (strncmp(p + i + 1, "et", 2) == 0) { // Get
            // folders
            for (iFolder = 0; iFolder < numOfFolder; iFolder++) {
               if (accessingFolder[iFolder] || !FolderToBeGenerated(iFolder)) {
                  continue;
               }
               if (strncmp(p + i + 3, folderName[iFolder].Data(), folderNameLen[iFolder]) == 0) {
                  // Get
                  if (folderArray[iFolder] != "1") {
                     if (strncmp(p + i + 3 + folderNameLen[iFolder], "At(", 3) == 0 ||
                         strncmp(p + i + 3 + folderNameLen[iFolder], "At (", 4) == 0) {
                        accessingFolder[iFolder] = kTRUE;
                        continue;
                     }
                  } else {
                     if (strncmp(p + i + 3 + folderNameLen[iFolder], "(", 1) == 0 ||
                         strncmp(p + i + 3 + folderNameLen[iFolder], " (", 2) == 0) {
                        accessingFolder[iFolder] = kTRUE;
                        continue;
                     }
                  }

                  // Get s
                  if (folderArray[iFolder] != "1") {
                     if (strncmp(p + i + 3 + folderNameLen[iFolder], "s(", 2) == 0 ||
                         strncmp(p + i + 3 + folderNameLen[iFolder], "s (", 3) == 0) {
                        accessingFolder[iFolder] = kTRUE;
                        continue;
                     }
                  }

                  // Get Address
                  if (strncmp(p + i + 3 + folderNameLen[iFolder], "Address(", 8) == 0 ||
                      strncmp(p + i + 3 + folderNameLen[iFolder], "Address (", 9) == 0) {
                     accessingFolder[iFolder] = kTRUE;
                     continue;
                  }
               }
            }

            // histos
            for (iTask = 0; iTask < numOfTask; iTask++) {
               for (iHisto = 0; iHisto < numOfHistos[iTask]; iHisto++) {
                  if (accessingHisto[iTask][iHisto]) {
                     continue;
                  }
                  if (strncmp(p + i + 3, histoName[iTask][iHisto].Data(), histoNameLen[iTask][iHisto]) == 0) {
                     // Get
                     if (strncmp(p + i + 3 + histoNameLen[iTask][iHisto], "(", 1) == 0 ||
                         strncmp(p + i + 3 + histoNameLen[iTask][iHisto], " (", 2) == 0) {
                        accessingHisto[iTask][iHisto] = kTRUE;
                        continue;
                     }
                     if (histoArraySize[iTask][iHisto] != "1") {
                        if (strncmp(p + i + 3 + histoNameLen[iTask][iHisto], "At(", 3) == 0 ||
                            strncmp(p + i + 3 + histoNameLen[iTask][iHisto], "At (", 4) == 0) {
                           accessingHisto[iTask][iHisto] = kTRUE;
                           continue;
                        }
                     }
                  }
               }
            }

            // graphs
            for (iTask = 0; iTask < numOfTask; iTask++) {
               for (iGraph = 0; iGraph < numOfGraphs[iTask]; iGraph++) {
                  if (accessingGraph[iTask][iGraph]) {
                     continue;
                  }
                  if (strncmp(p + i + 3, graphName[iTask][iGraph].Data(), graphNameLen[iTask][iGraph]) == 0) {
                     // Get
                     if (strncmp(p + i + 3 + graphNameLen[iTask][iGraph], "(", 1) == 0 ||
                         strncmp(p + i + 3 + graphNameLen[iTask][iGraph], " (", 2) == 0) {
                        accessingGraph[iTask][iGraph] = kTRUE;
                        continue;
                     }
                     if (graphArraySize[iTask][iGraph] != "1") {
                        if (strncmp(p + i + 3 + graphNameLen[iTask][iGraph], "At(", 3) == 0 ||
                            strncmp(p + i + 3 + graphNameLen[iTask][iGraph], "At (", 4) == 0) {
                           accessingGraph[iTask][iGraph] = kTRUE;
                           continue;
                        }
                     }
                  }
               }
            }
         }
      }

      if (p[i] == 'S') {
         if (strncmp(p + i + 1, "et", 2) == 0) {
            // Set
            for (iFolder = 0; iFolder < numOfFolder; iFolder++) {
               if (accessingFolder[iFolder] || !FolderToBeGenerated(iFolder)) {
                  continue;
               }
               if (strncmp(p + i + 3, folderName[iFolder].Data(), folderNameLen[iFolder]) == 0) {
                  // Set size
                  if (folderArray[iFolder] == "variable") {
                     if (strncmp(p + i + 3 + folderNameLen[iFolder], "Size(", 5) == 0 ||
                         strncmp(p + i + 3 + folderNameLen[iFolder], "Size (", 6) == 0) {
                        accessingFolder[iFolder] = kTRUE;
                        continue;
                     }
                  }

                  // Set s
                  if (folderArray[iFolder] == "variable") {
                     if (strncmp(p + i + 3 + folderNameLen[iFolder], "s(", 2) == 0 ||
                         strncmp(p + i + 3 + folderNameLen[iFolder], "s (", 3) == 0) {
                        accessingFolder[iFolder] = kTRUE;
                        continue;
                     }
                  }
               }
            }
         }
      }
   }

   // update cache
   ROMEString key;
   ROMEString filenameMode = filename;
   if (filenameMode.BeginsWith(outDir)) {
      filenameMode.Remove(0, outDir.Length());
   }

   // folders
   Int_t includeUnused;
   for (iFolder = 0; iFolder < numOfFolder; iFolder++) {
      if (folderSupport[iFolder]) {
         accessingFolder[iFolder] = kFALSE;
      }
      for (includeUnused = 0; includeUnused < 2; includeUnused++) {
         key.SetFormatted("%s:%d:%d", filenameMode.Data(), iFolder, includeUnused);
         if (accessingFolder[iFolder] && (includeUnused != 0 || folderUsed[iFolder])) {
            fgAccessFolderCache[key.Data()] = 1;
         } else {
            fgAccessFolderCache[key.Data()] = -1;
         } 
      }
   }
   delete [] accessingFolder;
   delete [] folderNameLen;

   // histos
   for (iTask = 0; iTask < numOfTask; iTask++) {
      for (iHisto = 0; iHisto < numOfHistos[iTask]; iHisto++) {
         key.SetFormatted("%s:%d:%d", filenameMode.Data(), iTask, iHisto);
         if (accessingHisto[iTask][iHisto]) {
            fgAccessHistoCache[key.Data()] = 1;
         } else {
            fgAccessHistoCache[key.Data()] = -1;
         }
      }
      delete [] accessingHisto[iTask];
      delete [] histoNameLen[iTask];
   }
   delete [] accessingHisto;
   delete [] histoNameLen;

   // graphs
   for (iTask = 0; iTask < numOfTask; iTask++) {
      for (iGraph = 0; iGraph < numOfGraphs[iTask]; iGraph++) {
         key.SetFormatted("%s:%d:%d", filenameMode.Data(), iTask, iGraph);
         if (accessingGraph[iTask][iGraph]) {
            fgAccessGraphCache[key.Data()] = 1;
         } else {
            fgAccessGraphCache[key.Data()] = -1;
         } 
      }
      delete [] accessingGraph[iTask];
      delete [] graphNameLen[iTask];
   }
   delete [] accessingGraph;
   delete [] graphNameLen;
}

//______________________________________________________________________________
void ROMEBuilder::WriteHeader(ROMEString& buffer, Int_t nAuthor, const ROMEString* author,
                              const ROMEString* email, Bool_t overwrite)
{
   Int_t i;
   ROMEString separator = " ";
   if (nAuthor > 0) {
      buffer.AppendFormatted("// Author:");
      for (i = 0; i < nAuthor; i++) {
         buffer.Append(separator);
         buffer.Append(author[i]);
         if (email[i].Length() > 1) {
            if (email[i].ContainsFast("://")) {
               buffer.AppendFormatted(" <%s>", email[i].Data());
            } else {
               buffer.AppendFormatted(" <mailto:%s>", email[i].Data());
            }
         }
         separator = ", ";
      }
      buffer.Append("\n\n");
   }

   if (overwrite) {
      buffer.AppendFormatted("/******************************************************************************\n");
      buffer.AppendFormatted(" *         ***  This file will be overwritten by the ROMEBuilder  ***         *\n");
      buffer.AppendFormatted(" *          ***      Don't make manual changes to this file      ***          *\n");
      buffer.AppendFormatted(" ******************************************************************************/\n\n");
   }
}

#define ALIGN_DESC // align description 80 chars.
//______________________________________________________________________________
void ROMEBuilder::WriteDescription(ROMEString& buffer, const char* className, const char* description, Bool_t endmark,
                                   const char* header)
{
   const Int_t nc = 80;
   ROMEString desc = description;
   ROMEStrArray descs;
   Ssiz_t p, pLast;
#if defined ( ALIGN_DESC )
   Ssiz_t pSpace;
#endif
   Int_t i;
   char *tmp = new char[desc.Length() + 1];

   // analyze description
   if (description && strlen(description)) {
      pLast = 0;
#if defined ( ALIGN_DESC )
      pSpace = 0;
#endif
      for (p = 0; p < desc.Length(); p++) {
         if (p == desc.Length() - 1) {
            strcpy(tmp, desc(pLast, p - pLast).Data());
            tmp[p - pLast + 1] = '\0';
            if (tmp[p - pLast] == '\n')
               tmp[p - pLast] = '\0';
            descs.Add(tmp);
            break;
         } else if (desc[p] == '\n') {
            strcpy(tmp, desc(pLast, p - 1 - pLast).Data());
            tmp[ p - 1 - pLast + 1] = '\0';
            descs.Add(tmp);
            pLast = p + 1;
            continue;
         }
#if defined ( ALIGN_DESC )
         else if (isspace(desc[p])) {
            pSpace = p;
         } else if (p >= pLast + nc - static_cast<Ssiz_t>(strlen("//  //")) - 1) {
            if (pLast > pSpace) {
               strcpy(tmp, desc(pLast, p - pLast).Data());
               tmp[p - pLast + 1] = '\0';
               descs.Add(tmp);
               pLast = p;
            } else {
               strcpy(tmp, desc(pLast, pSpace - pLast).Data());
               tmp[pSpace - pLast + 1] = '\0';
               descs.Add(tmp);
               pLast = pSpace + 1;
            }
            continue;
         }
#endif
      }
   }
   delete [] tmp;
   Bool_t inHTML = kFALSE;

   if (className && strlen(className)) {
      // cast to avoid inconsistency between format and arguments.
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// %s%*s //\n", className, static_cast<int>(nc - strlen("//  //") - strlen(className)), "");
      buffer.AppendFormatted("//                                                                            //\n");
      for (i = 0; i < descs.GetEntriesFast(); i++) {
         if (inHTML && descs[i] == "End_Html") {
            inHTML = kFALSE;
            buffer.Append("<!--*/\n");
            buffer.Append("// --> End_Html\n");
         } else if (!inHTML && descs[i] == "Begin_Html") {
            inHTML = kTRUE;
            buffer.Append("// Begin_Html <!--\n");
            buffer.Append("/*-->\n");
         } else {
#if defined ( ALIGN_DESC )
            if (descs[i].BeginsWith("#include") || descs[i].BeginsWith("#pragma") || inHTML) {
               buffer.AppendFormatted("%s\n", descs[i].Data());
            } else {
               buffer.AppendFormatted("// %s%*s //\n", descs[i].Data(), static_cast<int>(nc - strlen("//  //") - descs[i].Length()), "");
            }
#else
            if (descs[i].BeginsWith("#include") || descs[i].BeginsWith("#pragma") || inHTML) {
               buffer.AppendFormatted("%s\n", descs[i].Data());
            } else {
               buffer.AppendFormatted("// %s\n", descs[i].Data());
            }
#endif
         }
      }
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.Append("\n");
      if (header) {
         buffer.AppendFormatted("/* Generated header file containing necessary includes                        */\n");
         buffer.Append(header);
         buffer.Append("\n");
      }
      if (endmark) {
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("/*  This header was generated by ROMEBuilder. Manual changes above the        *\n");
         buffer.AppendFormatted(" * following line will be lost next time ROMEBuilder is executed.             */\n");
         buffer.AppendFormatted(kHeaderEndMark);
      }
   }
}

//______________________________________________________________________________
Bool_t ROMEBuilder::RemoveFile(const char* filename, const char* str)
{
   // Remove file if exists.
   // If str != 0, remove file only when it contains 'str';
   // return true when the file is removed.
   if (!filename)
      return false;

   if (!gSystem->AccessPathName(filename,kFileExists)) {
      bool removeFile = false;
      ROMEString fileBuffer;
      if (!str) {
         removeFile = true;
      } else {
         if (fileBuffer.ReadFile(filename) < 0) {
            if (makeOutput) cout<<"\n\nError : Failed to open '"<<filename<<"' !!!"<<endl;
            return false;
         }
         if (fileBuffer.ContainsFast(str))
            removeFile = true;
      }
      if (removeFile) {
         gSystem->Unlink(filename);
         if (makeOutput)
            cout<<"   removed '"<<filename<<"'."<<endl;
         return true;
      }
   }

   return false;
}

//______________________________________________________________________________
void ROMEBuilder::RemoveDepFiles(const char* str)
{
   // Remove .d files in 'obj' directory
   // If str != 0, remove files only when it contains 'str';
   const char *direntry;
   TString afile;
   TString dirname = outDir;
   dirname += "obj";
   void *dirp = gSystem->OpenDirectory(dirname);
   if (!dirp)
      return;

   //loop on all entries of this directory
   while ((direntry = gSystem->GetDirEntry(dirp))) {
      afile = Form("%s/%s", dirname.Data(), direntry);
      if (afile.EndsWith(".d"))
         RemoveFile(afile.Data(), str);
   }

   return;
}

//______________________________________________________________________________
ROMEString& ROMEBuilder::ProcessCommentCPP(ROMEString& org, ROMEString& result)
{
   result = org;
   result.ReplaceAll("\n", "\n// ");
   if (! result.BeginsWith("//"))
      result.Insert(0, "// ");
   return result;
}

//______________________________________________________________________________
ROMEString& ROMEBuilder::ProcessCommentHTML(ROMEString& org, ROMEString& result)
{
   result = org;
   result.ReplaceAll("\n", "<br>\n");
   return result;
}

//______________________________________________________________________________
ROMEString& ROMEBuilder::ProcessCommentString(ROMEString& org, ROMEString& result)
{
   result = org;
   result.ReplaceAll("\"", "\\\"");
   result.ReplaceAll("\n", "\\n\"\n\"");
   return result;
}

//______________________________________________________________________________
ROMEString& ROMEBuilder::GetSteerPath(ROMEString& steerPath,int iTask,int iSteer,int iField,const char* seperator)
{
   steerPath.SetFormatted("%s",steerFieldName[iTask][iSteer][iField].Data());
   while (steerParent[iTask][iSteer] != -1) {
      iSteer = steerParent[iTask][iSteer];
      steerPath.InsertFormatted(0, "%s%s",steerName[iTask][iSteer].Data(),seperator);
   }
   return steerPath;
}

//______________________________________________________________________________
ROMEString& ROMEBuilder::ParseDependences(ROMEString& org, ROMEString &result)
{
   int i;
   ROMEString str1, str2;
   result = org;
   TString ch;
   Int_t indx;
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      str1.SetFormatted("Task(%s)", taskHierarchyName[i].Data());
      str2.SetFormatted("((f%sConfigParametersFolder->Get%s%sTaskActive() && analyzer)",
                        mainProgName.Data(), taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data());
      indx = taskHierarchyParentIndex[i];
      while (indx != -1) {
         str2.AppendFormatted(" &&\n          (f%sConfigParametersFolder->Get%s%sTaskActive() && analyzer)",
                              mainProgName.Data(), taskHierarchyName[indx].Data(), taskHierarchySuffix[indx].Data());
         indx = taskHierarchyParentIndex[indx];
      }
      str2.Append(")");
      result.ReplaceAll(str1, str2);
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;

      str1.SetFormatted("Tab(%s)", tabName[i].Data());
      str2.SetFormatted("((GetWindow()->GetTabObjectAt(%d)->IsSwitch() && monitor)",
                        tabUsedIndex[i]);
      indx = tabParentIndex[i];
      while (indx != -1) {
         str2.AppendFormatted(" &&\n          (GetWindow()->GetTabObjectAt(%d)->IsSwitch() && monitor)",
                              tabUsedIndex[indx]);
         indx = tabParentIndex[indx];
      }
      str2.Append(")");
      result.ReplaceAll(str1, str2);
   }
   return result;
}
