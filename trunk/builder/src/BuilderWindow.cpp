/********************************************************************

  BuilderWindow.cpp, Ryu Sawada

  $Log$
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

bool ArgusBuilder::WriteWindowCpp() {
   int i,j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString menu_title;
   char fileBuffer[bufferLength];
   ROMEString buf;
   int nb,lenTot,ll;
   char *pos;
   int fileHandle;
   ROMEString classDescription;
   classDescription.SetFormatted("Main window class for the %s%s. This class creates main window and manages Tabs.",shortCut.Data(),mainProgName.Data());
   ROMEString tmp;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
      if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
   }
   // File name
   cppFile.SetFormatted("%s/src/monitor/%sWindow.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Monitor");
   format.Resize(0);
   buffer.AppendFormatted("//                                                                            //\n");
   pos = (char*)classDescription.Data();
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
   buffer.AppendFormatted("#include \"include/monitor/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/monitor/%sMonitor.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ClassImp(%sWindow);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sWindow *gWindow;\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("bool %sWindow::ConnectServer()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   char buffer[80] = \"\";\n");
   buffer.AppendFormatted("   if(strlen(gMonitor->GetSocketInterfaceHost()) == 0 ) {\n");
   buffer.AppendFormatted("      new ArgusTextDialog(gClient->GetRoot(), this, 100, 100, \"&Host name:\", (char*)buffer);\n");
   buffer.AppendFormatted("      gMonitor->SetSocketInterfaceHost((char*)buffer);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Connect to Server\n");
   buffer.AppendFormatted("   if(strlen(gMonitor->GetSocketInterfaceHost()) && !gMonitor->ConnectServer()) {\n");
   buffer.AppendFormatted("      sprintf(buffer, \"Cannot connect to server on host %%s, port %%d\", gMonitor->GetSocketInterfaceHost(), gMonitor->GetSocketInterfacePortNumber());\n");
   buffer.AppendFormatted("      new TGMsgBox(gClient->GetRoot(), this, \"Error\", buffer, kMBIconExclamation, 0, NULL);    \n");
   buffer.AppendFormatted("      SetWindowName(gMonitor->GetProgramName());\n");
   buffer.AppendFormatted("      return true;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sWindow::%sWindow(const TGWindow* p, char* title)\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fStatusBarSwitch = true;\n");
   for (i=0;i<numOfTab;i++) {
      int index = tabParentIndex[i];
      ROMEString switchString = tabName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   fTabSwitches.%s = true;\n", switchString.Data());
      for (j=0;j<numOfMenu[i];j++) {
         buffer.AppendFormatted("   f%sMenu[%d] = NULL;\n",tabName[i].Data(),j);
      }
   }
   for (i=0;i<numOfTab;i++) {
      format.SetFormatted("   f%%s%%03dTab = new %%sT%%s();\n",tabName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),tabName[i].Data(),i,shortCut.Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("}\n\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("bool %sWindow::Start()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   // Create status bar\n");
   buffer.AppendFormatted("   Int_t parts[] = {5};\n");
   buffer.AppendFormatted("   fStatusBar = new TGStatusBar(this,50,10,kHorizontalFrame);\n");
   buffer.AppendFormatted("   fStatusBar->SetParts(parts,sizeof(parts)/sizeof(Int_t));\n");
   buffer.AppendFormatted("   if(fStatusBarSwitch)\n");
   buffer.AppendFormatted("      this->AddFrame(fStatusBar,new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX,0,0,2,0));\n");
   buffer.AppendFormatted("   // Create menu\n");
   buffer.AppendFormatted("   fMenuFile = new TGPopupMenu(fClient->GetRoot());\n");
   buffer.AppendFormatted("   fMenuFile->AddEntry(\"&Connect to ...\", M_FILE_CONNECT);\n");
   buffer.AppendFormatted("   fMenuFile->AddEntry(\"E&xit\", M_FILE_EXIT);\n");
   buffer.AppendFormatted("   fMenuFile->Associate(this);\n");
   buffer.AppendFormatted("   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);\n");
   buffer.AppendFormatted("   fMenuBar->AddPopup(\"&File\", fMenuFile,\n");
   buffer.AppendFormatted("                      new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
   buffer.AppendFormatted("   AddFrame(fMenuBar,\n");
   buffer.AppendFormatted("            new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Create tab widget\n");
   buffer.AppendFormatted("   Int_t tabID = 0;\n",tabName[i].Data());      
   buffer.AppendFormatted("   fTab = new TGTab(this, (UInt_t)(600*gMonitor->GetWindowScale()), (UInt_t)(400*gMonitor->GetWindowScale()));\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTab;i++) {
      recursiveTabDepth=0;
      if(!AddTab(buffer,i))
         return false;
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   SetWindowName(gMonitor->GetProgramName());\n");
   buffer.AppendFormatted("   MapSubwindows();\n");
   buffer.AppendFormatted("   Resize(GetDefaultSize());\n");
   buffer.AppendFormatted("   MapWindow();\n");
   buffer.AppendFormatted("   fCurrentTabID = 1;\n");
   buffer.AppendFormatted("   ProcessMessage(MK_MSG(kC_COMMAND,kCM_TAB),0,0);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("void %sWindow::CloseWindow()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   // Got close message for this MainFrame. Terminate the application\n");
   buffer.AppendFormatted("   // or returns from the TApplication event loop (depending on the\n");
   buffer.AppendFormatted("   // argument specified in TApplication::Run()).\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gMonitor->GetApplication()->Terminate(0);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("void %sWindow::ClearStatusBar()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t parts[] = {5};\n");
   buffer.AppendFormatted("   fStatusBar->SetParts(parts,sizeof(parts)/sizeof(Int_t));\n");
   buffer.AppendFormatted("   fStatusBar->SetText(\"\",0);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("bool %sWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)\n",shortCut.Data());
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
   buffer.AppendFormatted("         case M_FILE_CONNECT:\n");
   buffer.AppendFormatted("            gMonitor->SetSocketInterfaceHost(\"\");\n");
   buffer.AppendFormatted("            gWindow->ConnectServer();\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         break;\n");      
   buffer.AppendFormatted("      case kCM_BUTTON:\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      case kCM_LISTBOX:\n");
   buffer.AppendFormatted("         break;      \n");
   buffer.AppendFormatted("      case kCM_TAB:\n");
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("         // %s\n",tabName[i].Data());
      buffer.AppendFormatted("         if(fCurrentTabID == f%sTabID && param1 != f%sTabID)\n",tabName[i].Data(),tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->TabUnSelected();\n",tabName[i].Data(),i);
   }
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("         // %s\n",tabName[i].Data());
      buffer.AppendFormatted("         if(fCurrentTabID != f%sTabID && param1 == f%sTabID)\n",tabName[i].Data(),tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->TabSelected();\n",tabName[i].Data(),i);
      buffer.AppendFormatted("         if (");
      int index = i;
      do  {
         buffer.AppendFormatted(" param1 == f%sTabID ||",tabName[index].Data());
         index = tabParentIndex[index];
      } while(index!=-1);
      buffer.Remove(buffer.Length()-2); // remove the last "||"
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("            f%s%03dTab->SetActive(true);\n",tabName[i].Data(),i);
      for (j=0;j<numOfMenu[i];j++) {
	 buffer.AppendFormatted("            f%sMenu[%d] = new TGPopupMenu(fClient->GetRoot());\n",tabName[i].Data(),j);
	 buffer.AppendFormatted("            f%sMenu[%d]->Associate(this);\n",tabName[i].Data(),j);
      }
      for (j=0;j<numOfMenu[i];j++) {
	 if(menuDepth[i][j] == 1){
	    if(!AddMenuItems(buffer,i,j))
	       return false;
	    buffer.AppendFormatted("            fMenuBar->AddPopup(\"%s\", f%sMenu[%d],\n",menuTitle[i][j].Data(),tabName[i].Data(),j);
	    buffer.AppendFormatted("                               new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
	 }
      }
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         else {\n");
      buffer.AppendFormatted("            f%s%03dTab->SetActive(false);\n",tabName[i].Data(),i);
      for (j=0;j<numOfMenu[i];j++) {
	 menu_title = menuTitle[i][j];
	 menu_title.ReplaceAll("&","");
	 buffer.AppendFormatted("            delete fMenuBar->RemovePopup(\"%s\");\n",menu_title.Data());
      }
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         MapSubwindows();\n");
   buffer.AppendFormatted("         Resize(fWidth-1,fHeight-1);\n");
   buffer.AppendFormatted("         Resize(fWidth+1,fHeight+1);\n");
   buffer.AppendFormatted("         MapWindow();\n");
   buffer.AppendFormatted("         fCurrentTabID = param1;\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      break;\n");    
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("TGPopupMenu* %sWindow::GetMenuHandle(const char* menuName)\n",shortCut.Data());
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
   buffer.AppendFormatted("%sWindow::~%sWindow()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("}\n");
   // Write File
   fileHandle = open(cppFile.Data(),O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb&&i<(int)buffer.Length();i++) {
         if (buffer[i] != fileBuffer[i]) {
            identical = false;
            break;
         }
      }
   }
   else
      identical = false;
   if (!identical) {
      fileHandle = open(cppFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(cppFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << cppFile.Data() << endl;
      nb = write(fileHandle,buffer.Data(), buffer.Length());
      close(fileHandle);
   }
   return true;
}

bool ArgusBuilder::WriteWindowH() {
   int i,j,k;
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   ROMEString buf;
   int nb,lenTot,ll;
   char *pos;
   int fileHandle;
   ROMEString classDescription;
   classDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tabs.",shortCut.Data(),mainProgName.Data());
   ROMEString tmp;
   ROMEString format;
   int nameLen = -1;
   int typeLen = 17;
   // max tab name length
   int tabLen=0;
   int scl = shortCut.Length();
   for (i=0;i<numOfTab;i++) {
      if (tabLen<(int)tabName[i].Length()) tabLen = tabName[i].Length();
      if (typeLen<(int)tabName[i].Length()+scl) typeLen = tabName[i].Length()+scl;
   }
   // max tab switch name length
   int switchLen = -1;
   ROMEString switchString;
   for (i=0;i<numOfTab;i++) {
      int index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (switchLen<(int)switchString.Length()) switchLen = switchString.Length();
   }
   // File name
   hFile.SetFormatted("%s/include/monitor/%sWindow.h",outDir.Data(),shortCut.Data());
   // Description
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Monitor");
   format.Resize(0);
   buffer.AppendFormatted("//                                                                            //\n");
   pos = (char*)classDescription.Data();
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
   buffer.AppendFormatted("#include <TGMenu.h>\n");
   buffer.AppendFormatted("#include <TGTab.h>\n");
   buffer.AppendFormatted("#include <TGStatusBar.h>\n");
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("#include \"include/tabs/%sT%s.h\"\n",shortCut.Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("\n");
   // Tab Switches Structure
   buffer.AppendFormatted("// Tab Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");
   for (i=0;i<numOfTab;i++) {
      int index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabName[index].Data());
         index = tabParentIndex[index];
      }
      format.SetFormatted("   bool %%s;%%%ds  //! %%s Tab\n",switchLen-switchString.Length());
      buffer.AppendFormatted((char*)format.Data(),switchString.Data(),"",switchString.Data());
//      buffer.AppendFormatted("   int %s;   //! %s Tab\n",switchString.Data(),switchString.Data());
   }
   buffer.AppendFormatted("} TabSwitches;\n");
   buffer.AppendFormatted("\n");
   // Class
   buffer.AppendFormatted("class %sWindow:public TGMainFrame {  \n",shortCut.Data());
   // Enumeration
   buffer.AppendFormatted("public:\n");
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
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   TGStatusBar         *fStatusBar;\n");
   buffer.AppendFormatted("   Bool_t              fStatusBarSwitch;\n");
   buffer.AppendFormatted("   TGMenuBar           *fMenuBar;\n");
   buffer.AppendFormatted("   TGPopupMenu         *fMenuFile;\n");
   buffer.AppendFormatted("   TGTab               *fTab;\n");
   buffer.AppendFormatted("   Int_t               fCurrentTabID;\n");
   for (i=0;i<numOfTab;i++) {
      if(tabNumOfChildren[i])
         buffer.AppendFormatted("   TGTab               *f%s%03dTabSubTab;\n",tabName[i].Data(),i);
   }
   for (i=0;i<numOfTab;i++) {
      if(numOfMenu[i]>0)
	 buffer.AppendFormatted("   TGPopupMenu         *f%sMenu[%d];\n",tabName[i].Data(),numOfMenu[i]);
      buffer.AppendFormatted("   Int_t               f%sTabID;\n",tabName[i].Data());
   }
   buffer.AppendFormatted("   enum CommandIdentifiers {\n");
   buffer.AppendFormatted("      M_FILE_EXIT,\n");
   buffer.AppendFormatted("      M_FILE_CONNECT\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");
   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches fTabSwitches;               //! Tab Switches\n");
   buffer.AppendFormatted("\n");
   // Tab Fields
   buffer.AppendFormatted("   // Tab Fields\n");
   for (i=0;i<numOfTab;i++) {
      format.SetFormatted("   %%sT%%s*%%%ds f%%s%%03dTab;%%%ds  // Handle to %%s Tab\n",typeLen-tabName[i].Length()-shortCut.Length(),tabLen-tabName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),tabName[i].Data(),"",tabName[i].Data(),i,"",tabName[i].Data());
      format.SetFormatted("   TGCompositeFrame*%%%ds t%%sT%%s;%%%ds  // Container of %%s Tab\n",typeLen-15,1+tabLen-tabName[i].Length()-shortCut.Length());
      buffer.AppendFormatted((char*)format.Data(),"",shortCut.Data(),tabName[i].Data(),"",tabName[i].Data());
   }
   buffer.AppendFormatted("\n");
   // Method
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sWindow(const TGWindow * p, char *title);\n",shortCut.Data());
   buffer.AppendFormatted("   ~%sWindow();\n",shortCut.Data());
   buffer.AppendFormatted("   bool Start();\n");
   buffer.AppendFormatted("\n");
   // Status bar
   buffer.AppendFormatted("   void SetStatusBarSwitch(Bool_t sw) { fStatusBarSwitch = sw; };\n");
   buffer.AppendFormatted("   Bool_t GetStatusBarSwitch() { return fStatusBarSwitch; };\n");
   buffer.AppendFormatted("   TGStatusBar* GetStatusBar() { return fStatusBar; };\n");
   // Menu
   buffer.AppendFormatted("   // Menu\n");
   buffer.AppendFormatted("   TGPopupMenu* GetMenuHandle(const char* menuName);\n");
   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches* GetTabSwitches() { return &fTabSwitches; };\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Tabs\n");
   for (i=0;i<numOfTab;i++) {
      format.SetFormatted("   %%sT%%s*%%%ds Get%%s%%03dTab()%%%ds { return f%%s%%03dTab;%%%ds };\n",typeLen-tabName[i].Length()-shortCut.Length(),tabLen-tabName[i].Length(),tabLen-tabName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),tabName[i].Data(),"",tabName[i].Data(),i,"",tabName[i].Data(),i,"");
   }
   buffer.AppendFormatted("   void CloseWindow();\n");
   buffer.AppendFormatted("   void ClearStatusBar();\n");
   buffer.AppendFormatted("   Bool_t ConnectServer();\n");
   buffer.AppendFormatted("   Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sWindow,1)\n",shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("extern %sWindow *gWindow;\n",shortCut.Data());
   buffer.AppendFormatted("#endif\n");
   // Write File
   fileHandle = open(hFile.Data(),O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb&&i<(int)buffer.Length();i++) {
         if (buffer[i] != fileBuffer[i]) {
            identical = false;
            break;
         }
      }
   }
   else
      identical = false;
   if (!identical) {
      fileHandle = open(hFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(hFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << hFile.Data() << endl;
      nb = write(fileHandle,buffer.Data(), buffer.Length());
      close(fileHandle);
   }
   return true;
}

bool ArgusBuilder::AddTab(ROMEString& buffer,int& i) {
   int j;
   ROMEString parentt;
   ROMEString format;
   int index = tabParentIndex[i];
   ROMEString switchString = tabName[i].Data();
   int depth;
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
         return false;
   }
   recursiveTabDepth--;
   if(tabNumOfChildren[j]){
      for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
      buffer.AppendFormatted("      t%sT%s->AddFrame(f%s%03dTabSubTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n",shortCut.Data(),tabName[j].Data(),tabName[j].Data(),j);
   }
   for(depth=0;depth<recursiveTabDepth;depth++) buffer += "   ";
   buffer.AppendFormatted("   }\n");

   return true;
 }

bool ArgusBuilder::AddMenuItems(ROMEString& buffer,int i,int j) {
   int k;
   for (k=0;k<numOfMenuItem[i][j];k++) {
      if(menuItemTitle[i][j][k] == LINE_TITLE){
         buffer.AppendFormatted("            f%sMenu[%d]->AddSeparator();\n",tabName[i].Data(),j);
      }
      else if(menuItemChildMenuIndex[i][j][k]) {
         if(!AddMenuItems(buffer,i,menuItemChildMenuIndex[i][j][k]))
            return false;
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
   return true;
}
