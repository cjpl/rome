/********************************************************************

  BuilderWindow.cpp, Ryu Sawada

  $Log$
  Revision 1.32  2005/10/05 19:13:24  sawada
  better color of documents.
  steering parameters in documents.
  CSS suport.
  ROMETextDataBase support.
  userclean in Makefile.
  removed unused variables.

  Revision 1.31  2005/07/03 18:42:01  sawada
  get enumerations back into window class.

  Revision 1.30  2005/07/03 17:31:34  sawada
  Support folder.
  Multiple dimension fields in folders.

  Revision 1.29  2005/07/01 12:43:37  schneebeli_m
  Update of TNetFolder : reconnect

  Revision 1.28  2005/06/08 14:05:36  schneebeli_m
  delete menu

  Revision 1.27  2005/06/08 13:03:20  schneebeli_m
  menus with same name

  Revision 1.26  2005/06/01 14:41:48  schneebeli_m
  menu changes

  Revision 1.25  2005/05/26 14:26:55  sawada
  Lots of changes.
  Made ArgusBuilder an inheriting class of ROMEBuilder.
  Remove ROMEFolder and added NetFolers.
  Added ArgusWindow class.
  and so on.

  Revision 1.24  2005/05/16 16:46:57  sawada
  with gcc4, enumeration in class can not be constant in switch.

  Revision 1.23  2005/05/14 21:42:23  sawada
  Separated file writing function in builder.

  Revision 1.22  2005/05/08 00:28:54  sawada
  fixed mismathes of [Set,Append]Formatted in builder.
  added readme of examples.

  Revision 1.21  2005/05/05 20:08:05  sawada
  code clean up.

  Revision 1.20  2005/05/02 08:07:36  schneebeli_m
  small bug

  Revision 1.19  2005/04/28 16:26:26  sawada
  PostgreSQL support.
  Error message from GetMenuHandle.

  Revision 1.18  2005/04/25 17:06:16  schneebeli_m
  GetMenuHandle() implemented

  Revision 1.17  2005/04/22 15:29:09  schneebeli_m
  added menu id enumeration

  Revision 1.16  2005/04/22 12:58:24  schneebeli_m
  removed windows errors

  Revision 1.15  2005/04/01 12:31:19  sawada
  sub menu

  Revision 1.14  2005/03/28 10:54:37  sawada
  removed tab hierarchy.
  made ReadXMLMenu.

  Revision 1.13  2005/03/18 15:24:49  sawada
  added TabSelected,TabUnSelected.
  added gWindow->ClearStatusBar();

  Revision 1.12  2005/03/18 12:10:09  sawada
  added status bar.

  Revision 1.11  2005/03/12 22:26:10  sawada
  small change.

  Revision 1.10  2005/03/12 22:22:33  sawada
  Better output from builder.
  Menu of nested tab.

  Revision 1.9  2005/03/12 01:21:00  sawada
  Nested tab.

  Revision 1.8  2005/02/27 23:53:43  sawada
  Create placeholder of ROMEFolder at start.
  Environment variable in ROMEProjectPath.
  Bug fix of arrayed ROMEFolder.
  Bug fix of SetActive of tabs from monitor.
  Create menues of the first tab at start.

  Revision 1.7  2005/02/24 15:04:03  sawada
  Reduced number of configuration to 1.
  Replaced ss_getchar to getchar().
  Removed LineToProcess.
  Removed bk_is32,bk_find.
  Improved help.
  Handling of midas host and experiment.

  Revision 1.6  2005/02/22 10:43:31  sawada
  User defined menus.

  Revision 1.5  2005/02/04 22:58:46  sawada
  ROMEFolder

  Revision 1.4  2005/02/02 18:58:02  sawada
  small change.

  Revision 1.3  2005/02/01 14:44:33  sawada
  Resize frames in window.

  Revision 1.2  2005/01/30 20:39:39  sawada
  * Makefile of builder
  * Tab enable/disable
  * Bug fix.(fNetFolder, ConnectServer)

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

Bool_t ArgusBuilder::WriteWindowCpp() {
   Int_t i,j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString bufferTemp;
   ROMEString format;
   ROMEString menu_title;
   ROMEString buf;
   Int_t lenTot,ll;
   char *pos;

   ROMEString classDescription;
   classDescription.SetFormatted("Main window class for the %s%s. This class creates main window and manages Tabs.",shortCut.Data(),mainProgName.Data());
   ROMEString tmp;

   // File name
   cppFile.SetFormatted("%s/src/framework/%sWindow.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((Char_t*)format.Data(),shortCut.Data(),"Monitor");
   format.Resize(0);
   buffer.AppendFormatted("//                                                                            //\n");
   pos = (Char_t*)classDescription.Data();
   lenTot = classDescription.Length();
   while (pos-classDescription.Data() < lenTot) {
      if (lenTot+(classDescription.Data()-pos)<74)
         i=lenTot+(classDescription.Data()-pos);
      else for (i=74;pos[i]!=32&&i>0;i--) {}
      if (i<=0)
         i=TMath::Min(75,lenTot);
      pos[i] = 0;
      buffer.AppendFormatted("// %-74.74s   \n",pos);
      pos = pos+i+1;
   }
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ArgusBuilder.                          //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#include <TGMsgBox.h>\n");
   buffer.AppendFormatted("#include <ArgusTextDialog.h>\n");
   buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sWindow);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sWindow *gWindow;\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("%sWindow::%sWindow(const TGWindow* p, Char_t* title)\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fStatusBarSwitch = kTRUE;\n");

   for (i=0;i<numOfTab;i++) {
      Int_t index = tabParentIndex[i];
      ROMEString switchString = tabName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   fTabSwitches.%s = kTRUE;\n", switchString.Data());
      for (j=0;j<numOfMenu[i];j++) {
         buffer.AppendFormatted("   f%sMenu[%d] = NULL;\n",tabName[i].Data(),j);
      }
   }

   for (i=0;i<numOfTab;i++) {
      format.SetFormatted("   f%%s%%03dTab = new %%sT%%s();\n");
      buffer.AppendFormatted((Char_t*)format.Data(),tabName[i].Data(),i,shortCut.Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("}\n\n");
   buffer.AppendFormatted("\n");

   // CreateTabs
   buffer.AppendFormatted("Bool_t %sWindow::CreateTabs()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t tabID = 0;\n");
   for (i=0;i<numOfTab;i++) {
      recursiveTabDepth=0;
      if(!AddTab(buffer,i))
         return kFALSE;
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ProcessMessage
   buffer.AppendFormatted("Bool_t %sWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   // Process messages coming from widgets associated with the dialog.  \n");
   buffer.AppendFormatted("   switch (GET_MSG(msg)) {\n");
   buffer.AppendFormatted("   case kC_COMMAND:    \n");
   buffer.AppendFormatted("      switch (GET_SUBMSG(msg)) {\n");
   buffer.AppendFormatted("      case kCM_MENU:\n");
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("         if(fCurrentTabID == f%sTabID) {\n",tabName[i].Data());
      for (j=0;j<numOfMenu[i];j++) {
         buffer.AppendFormatted("            if (f%sMenu[%d]->GetEntry(param1)!=0)\n",tabName[i].Data(),j);
         buffer.AppendFormatted("               f%s%03dTab->MenuClicked(f%sMenu[%d],param1);\n"
			        ,tabName[i].Data(),i,tabName[i].Data(),j);
      }
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         switch (param1) {\n");
   buffer.AppendFormatted("         case M_FILE_EXIT:\n");
   buffer.AppendFormatted("            CloseWindow();\n");
   buffer.AppendFormatted("            break;\n");
   for (i=0;i<numOfNetFolder;i++){
      buffer.AppendFormatted("         case M_FILE_CONNECT_%s:\n",netFolderName[i].Data());
      buffer.AppendFormatted("            gMonitor->ConnectNetFolder(\"%s\");\n",netFolderName[i].Data());
      buffer.AppendFormatted("            break;\n");
   }
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      case kCM_BUTTON:\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      case kCM_LISTBOX:\n");
   buffer.AppendFormatted("         break;      \n");
   buffer.AppendFormatted("      case kCM_TAB:\n");


   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("         if(fCurrentTabID == f%sTabID && param1 != f%sTabID) {\n",tabName[i].Data(),tabName[i].Data());
      for (j=0;j<numOfMenu[i];j++) {
	      menu_title = menuTitle[i][j];
	      menu_title.ReplaceAll("&","");
	      buffer.AppendFormatted("            delete fMenuBar->RemovePopup(\"%s\");\n",menu_title.Data());
      }
      buffer.AppendFormatted("         }\n");
   }
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("         // %s\n",tabName[i].Data());
      buffer.AppendFormatted("         if (");
      Int_t index = i;
      do  {
         buffer.AppendFormatted(" param1 == f%sTabID ||",tabName[index].Data());
         index = tabParentIndex[index];
      } while(index!=-1);
      buffer.Remove(buffer.Length()-2); // remove the last "||"
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("            f%s%03dTab->SetActive(kTRUE);\n",tabName[i].Data(),i);
      for (j=0;j<numOfMenu[i];j++) {
         buffer.AppendFormatted("            f%sMenu[%d] = new TGPopupMenu(fClient->GetRoot());\n",tabName[i].Data(),j);
         buffer.AppendFormatted("            f%sMenu[%d]->Associate(this);\n",tabName[i].Data(),j);
      }
      for (j=0;j<numOfMenu[i];j++) {
	 if(menuDepth[i][j] == 1){
	    if(!AddMenuItems(buffer,i,j))
	       return kFALSE;
	    buffer.AppendFormatted("            fMenuBar->AddPopup(\"%s\", f%sMenu[%d],\n",menuTitle[i][j].Data(),tabName[i].Data(),j);
	    buffer.AppendFormatted("                               new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
	 }
      }
      buffer.AppendFormatted("         }\n");
   }
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("         // %s\n",tabName[i].Data());
      buffer.AppendFormatted("         if(fCurrentTabID == f%sTabID && param1 != f%sTabID) {\n",tabName[i].Data(),tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->TabUnSelected();\n",tabName[i].Data(),i);
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         if(fCurrentTabID != f%sTabID && param1 == f%sTabID) {\n",tabName[i].Data(),tabName[i].Data());
      buffer.AppendFormatted("            fCurrentTabID = param1;\n");
      buffer.AppendFormatted("            f%s%03dTab->TabSelected();\n",tabName[i].Data(),i);
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         fCurrentTabID = param1;\n");


   buffer.AppendFormatted("         MapSubwindows();\n");
   buffer.AppendFormatted("         Resize(fWidth-1,fHeight-1);\n");
   buffer.AppendFormatted("         Resize(fWidth+1,fHeight+1);\n");
   buffer.AppendFormatted("         MapWindow();\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // GetMenuHandle
   buffer.AppendFormatted("TGPopupMenu* %sWindow::GetMenuHandle(const Char_t* menuName)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("   if(fCurrentTabID == f%sTabID) {\n",tabName[i].Data());
      for (j=0;j<numOfMenu[i];j++) {
	      buffer.AppendFormatted("      if (!strcmp(menuName,\"%s\"))\n",menuTitle[i][j].Data());
	      buffer.AppendFormatted("         return f%sMenu[%d];\n",tabName[i].Data(),j);
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   cout<<\"Error: A menu (\"<<menuName<<\") was not found.\"<<endl;\n");
   buffer.AppendFormatted("   return NULL;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddMenuNetFolder
   buffer.AppendFormatted("Bool_t %sWindow::AddMenuNetFolder(TGPopupMenu* menu)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   if(gMonitor->GetNumberOfNetFolders() <= 0 )\n",i);
   buffer.AppendFormatted("      return kFALSE;\n",i);
   buffer.AppendFormatted("   Bool_t active = kFALSE;\n",i);
   for (i=0;i<numOfNetFolder;i++){
      buffer.AppendFormatted("   if(gMonitor->GetNetFolderActive(%d)){\n",i);
      buffer.AppendFormatted("      menu->AddEntry(\"%s\", M_FILE_CONNECT_%s);\n",netFolderName[i].Data(),netFolderName[i].Data());
   buffer.AppendFormatted("      active = kTRUE;\n");
   buffer.AppendFormatted("   }\n",i);
   }
   buffer.AppendFormatted("   return active;\n",i);
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);
   return kTRUE;
}


Bool_t ArgusBuilder::WriteWindowH() {
   Int_t i,j,k;
   ROMEString hFile;
   ROMEString buffer;
   ROMEString buf;
   Int_t lenTot,ll;
   char *pos;
   ROMEString classDescription;
   classDescription.SetFormatted("Main window class for the %s%s.",shortCut.Data(),mainProgName.Data());
   ROMEString tmp;
   ROMEString format;

   Int_t nameLen = -1;
   Int_t typeLen = -1;
   Int_t scl = shortCut.Length();
   for (i=0;i<numOfTab;i++) {
      nameLen = TMath::Max(nameLen,(Int_t)(tabName[i].Length()+strlen("f000Tab")));
      nameLen = TMath::Max(nameLen,(Int_t)(tabName[i].Length()+scl+strlen("tT")));
      nameLen = TMath::Max(nameLen,(Int_t)(tabName[i].Length()+strlen("Get000Tab()")));
      typeLen = TMath::Max(typeLen,(Int_t)(tabName[i].Length()+scl+strlen("T*")));
   }
   typeLen = TMath::Max(typeLen,(Int_t)strlen("TGCompositeFrame*"));

   // max tab switch name length
   Int_t switchLen = -1;
   ROMEString switchString;
   for (i=0;i<numOfTab;i++) {
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (switchLen<(Int_t)switchString.Length()) switchLen = switchString.Length();
   }

   // File name
   hFile.SetFormatted("%s/include/framework/%sWindow.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((Char_t*)format.Data(),shortCut.Data(),"Monitor");
   format.Resize(0);
   buffer.AppendFormatted("//                                                                            //\n");
   pos = (Char_t*)classDescription.Data();
   lenTot = classDescription.Length();
   while (pos-classDescription.Data() < lenTot) {
      if (lenTot+(classDescription.Data()-pos)<74)
         i=lenTot+(classDescription.Data()-pos);
      else for (i=74;pos[i]!=32&&i>0;i--) {}
      if (i<=0)
         i=TMath::Min(75,lenTot);
      pos[i] = 0;
      buffer.AppendFormatted("// %-74.74s   \n",pos);
      pos = pos+i+1;
   }
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ArgusBuilder.                          //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sWINDOW_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sWINDOW_H\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include \"ArgusWindow.h\"\n");
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("#include \"include/tabs/%sT%s.h\"\n",shortCut.Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Tab Switches Structure
   buffer.AppendFormatted("// Tab Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");

   for (i=0;i<numOfTab;i++) {
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabName[index].Data());
         index = tabParentIndex[index];
      }
      format.SetFormatted("   Bool_t %%s;%%%ds  //! %%s Tab\n",switchLen-switchString.Length());
      buffer.AppendFormatted((Char_t*)format.Data(),switchString.Data(),"",switchString.Data());
//      buffer.AppendFormatted("   Int_t %s;   //! %s Tab\n",switchString.Data(),switchString.Data());
   }
   buffer.AppendFormatted("} TabSwitches;\n");
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("class %sWindow : public ArgusWindow {  \n",shortCut.Data());
   // Enumeration
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   enum CommandIdentifiers{\n");
   buffer.AppendFormatted("      M_FILE_CONNECT_ROOT = 10,\n");
   for (i=0;i<numOfNetFolder;i++)
      buffer.AppendFormatted("      M_FILE_CONNECT_%s,\n",netFolderName[i].Data());
   buffer.Remove(buffer.Length()-2,1); // remove the last ','
   buffer.AppendFormatted("   };\n");

   buffer.AppendFormatted("   enum MenuEnumeration {\n");
   buffer.AppendFormatted("      M_ROOT = 1000,\n");
   for (i=0;i<numOfTab;i++) {
      for (j=0;j<numOfMenu[i];j++) {
         for (k=0;k<numOfMenuItem[i][j];k++) {
            if (menuItemEnumName[i][j][k].Length()>0)
               buffer.AppendFormatted("      %s,\n",menuItemEnumName[i][j][k].Data());
         }
      }
   }
   buffer.Remove(buffer.Length()-2,1); // remove the last ','
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("private:\n");
   for (i=0;i<numOfTab;i++) {
      if(tabNumOfChildren[i])
         buffer.AppendFormatted("   TGTab               *f%s%03dTabSubTab;\n",tabName[i].Data(),i);
   }
   for (i=0;i<numOfTab;i++) {
      if(numOfMenu[i]>0)
	 buffer.AppendFormatted("   TGPopupMenu         *f%sMenu[%d];\n",tabName[i].Data(),numOfMenu[i]);
      buffer.AppendFormatted("   Int_t               f%sTabID;\n",tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches fTabSwitches;               //! Tab Switches\n");
   buffer.AppendFormatted("\n");
   // Tab Fields
   buffer.AppendFormatted("   // Tab Fields\n");
   for (i=0;i<numOfTab;i++) {
      format.SetFormatted("   %%sT%%s*%%%ds f%%s%%03dTab;%%%ds  // Handle to %%s Tab\n",typeLen-tabName[i].Length()-shortCut.Length()-strlen("T*"),nameLen-tabName[i].Length()-strlen("f000Tab"));
      buffer.AppendFormatted((Char_t*)format.Data(),shortCut.Data(),tabName[i].Data(),"",tabName[i].Data(),i,"",tabName[i].Data());
      format.SetFormatted("   TGCompositeFrame*%%%ds t%%sT%%s;%%%ds  // Container of %%s Tab\n",typeLen-strlen("TGCompositeFrame*"),nameLen-tabName[i].Length()-scl-strlen("tT"));
      buffer.AppendFormatted((Char_t*)format.Data(),"",shortCut.Data(),tabName[i].Data(),"",tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Method
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sWindow(const TGWindow * p, char *title);\n",shortCut.Data());
   buffer.AppendFormatted("   ~%sWindow(){}\n",shortCut.Data());
   buffer.AppendFormatted("   Bool_t CreateTabs();\n");
   buffer.AppendFormatted("   Bool_t AddMenuNetFolder(TGPopupMenu* menu);\n");
   buffer.AppendFormatted("\n");

   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches* GetTabSwitches() { return &fTabSwitches; }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Tabs\n");
   for (i=0;i<numOfTab;i++) {
      format.SetFormatted("   %%sT%%s*%%%ds Get%%s%%03dTab()%%%ds { return f%%s%%03dTab;%%%ds }\n",typeLen-tabName[i].Length()-shortCut.Length()-strlen("T*"),nameLen-tabName[i].Length()-strlen("Get000Tab()"),nameLen-tabName[i].Length()-strlen("f000Tab"));
      buffer.AppendFormatted((Char_t*)format.Data(),shortCut.Data(),tabName[i].Data(),"",tabName[i].Data(),i,"",tabName[i].Data(),i,"");
   }
   buffer.AppendFormatted("   Bool_t       ProcessMessage(Long_t msg, Long_t param1, Long_t param2);\n");
   buffer.AppendFormatted("   TGPopupMenu* GetMenuHandle(const Char_t* menuName);\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   ClassDef(%sWindow,1)\n",shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("extern %sWindow *gWindow;\n",shortCut.Data());
   buffer.AppendFormatted("#endif\n");

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);
   return kTRUE;
}


Bool_t ArgusBuilder::AddTab(ROMEString& buffer,Int_t& i) {
   Int_t j;
   ROMEString parentt;
   ROMEString format;
   Int_t index = tabParentIndex[i];

   ROMEString switchString = tabName[i].Data();
   Int_t depth;
   while (index!=-1) {
      switchString.Insert(0,"_");
      switchString.Insert(0,tabName[index].Data());
      index = tabParentIndex[index];
   }
   if (tabParentIndex[i] == -1)
      parentt = "fTab";
   else
      parentt.SetFormatted("f%s%03dTabSubTab",tabName[tabParentIndex[i]].Data(),tabParentIndex[i]);

   for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
   buffer.AppendFormatted("   if (fTabSwitches.%s){\n",switchString.Data());

   for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
   buffer.AppendFormatted("      t%sT%s = %s->AddTab(\"%s\");\n",shortCut.Data(),tabName[i].Data(),parentt.Data(),tabTitle[i].Data());

   for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
   buffer.AppendFormatted("      f%s%03dTab->ReparentWindow(t%sT%s, 60, 20);\n",tabName[i].Data(),i,shortCut.Data(),tabName[i].Data());

   for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
   buffer.AppendFormatted("      f%s%03dTab->Init();\n",tabName[i].Data(),i);

   if(!tabNumOfChildren[i]){
      for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
      buffer.AppendFormatted("      t%sT%s->AddFrame(f%s%03dTab,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));\n",shortCut.Data(),tabName[i].Data(),tabName[i].Data(),i);
   }

   for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";

   buffer.AppendFormatted("      f%sTabID = tabID++;\n",tabName[i].Data());

   if(tabNumOfChildren[i]){
      for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
      buffer.AppendFormatted("      f%s%03dTabSubTab = new TGTab(t%sT%s, (UInt_t)(600*gMonitor->GetWindowScale()), (UInt_t)(400*gMonitor->GetWindowScale()));\n",tabName[i].Data(),i,shortCut.Data(),tabName[i].Data());
   }
   recursiveTabDepth++;
   j=i;

   while(i<j+tabNumOfChildren[j]){
      i++;
      if(!AddTab(buffer,i))
         return kFALSE;
   }

   recursiveTabDepth--;

   if(tabNumOfChildren[j]){
      for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
      buffer.AppendFormatted("      t%sT%s->AddFrame(f%s%03dTabSubTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n",shortCut.Data(),tabName[j].Data(),tabName[j].Data(),j);
   }

   for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
   buffer.AppendFormatted("   }\n");

   return kTRUE;
}


Bool_t ArgusBuilder::AddMenuItems(ROMEString& buffer,Int_t i,Int_t j) {
   Int_t k;

   for (k=0;k<numOfMenuItem[i][j];k++) {
      if(menuItemTitle[i][j][k] == LINE_TITLE){
         buffer.AppendFormatted("            f%sMenu[%d]->AddSeparator();\n",tabName[i].Data(),j);
      }
      else if(menuItemChildMenuIndex[i][j][k]) {
         if(!AddMenuItems(buffer,i,menuItemChildMenuIndex[i][j][k]))
            return kFALSE;
         buffer.AppendFormatted("            f%sMenu[%d]->AddPopup(\"%s\", f%sMenu[%d]);\n"
            ,tabName[i].Data(),j,menuTitle[i][menuItemChildMenuIndex[i][j][k]].Data()
            ,tabName[i].Data(),menuItemChildMenuIndex[i][j][k]);
      }
      else {
         buffer.AppendFormatted("            f%sMenu[%d]->AddEntry(\"%s\", %s);\n"
         ,tabName[i].Data(),j,menuItemTitle[i][j][k].Data()
         ,menuItemEnumName[i][j][k].Data());
      }
   }

   return kTRUE;
}
