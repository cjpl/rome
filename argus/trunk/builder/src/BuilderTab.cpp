/********************************************************************

  BuilderTab.cpp, Ryu Sawada

  $Log$
  Revision 1.19  2005/03/12 22:22:33  sawada
  Better output from builder.
  Menu of nested tab.

  Revision 1.18  2005/03/12 01:21:00  sawada
  Nested tab.

  Revision 1.17  2005/02/27 23:53:43  sawada
  Create placeholder of ROMEFolder at start.
  Environment variable in ROMEProjectPath.
  Bug fix of arrayed ROMEFolder.
  Bug fix of SetActive of tabs from monitor.
  Create menues of the first tab at start.

  Revision 1.16  2005/02/24 23:25:01  sawada
  Removed thread flag in builder.
  Added ProcessMessageThread.
  Changed arguments of constructor of tabs.

  Revision 1.15  2005/02/24 17:02:11  sawada
  start of thread function.

  Revision 1.14  2005/02/22 10:43:31  sawada
  User defined menus.

  Revision 1.13  2005/02/21 19:06:56  sawada
  changed platform specifying macros

  Revision 1.12  2005/02/06 02:15:18  sawada
  reduced number of configuration file to 1.

  Revision 1.11  2005/02/06 00:39:35  sawada
   Changed TRint to TApplication
   Stop method of thread function
   Name of method for start/stop

  Revision 1.10  2005/02/05 23:16:23  sawada
  Removed TROOT,VoidFuncPtr_t.
  small bug fix.

  Revision 1.9  2005/02/03 09:11:07  sawada
  function type of thread function on Windows.

  Revision 1.8  2005/02/02 18:19:25  sawada
  small bug fix.

  Revision 1.7  2005/02/02 17:39:30  sawada
  Thread functions for Windows.

  Revision 1.6  2005/02/02 11:29:59  sawada
  tthread example (for the moment, linux&Macintosh)

  Revision 1.5  2005/02/02 07:33:19  sawada
  small bug fix

  Revision 1.4  2005/02/01 21:53:24  sawada
  implemented thread functions of tabs.

  Revision 1.3  2005/01/31 10:21:29  sawada
   * make clean.
   * changed template of tabs.

  Revision 1.2  2005/01/30 20:39:39  sawada
  * Makefile of builder
  * Tab enable/disable
  * Bug fix.(fNetFolder, ConnectServer)

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

bool ArgusBuilder::ReadXMLTab() {
   // read the tab definitions out of the xml file
   ROMEString tmp;
   char* name;
   char* cstop;
   int type,i,j;
   ROMEString currentTabName = "";
   int currentNumberOfTabs;
   // count tabs
   numOfTab++;
   currentNumberOfTabs = numOfTab;
   if (currentNumberOfTabs>=maxNumberOfTabs) {
      cout << "Maximal number of tabs reached : " << maxNumberOfTabs << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   tabName[currentNumberOfTabs] = "";
   tabTitle[currentNumberOfTabs] = "";
   tabAuthor[currentNumberOfTabs] = mainAuthor;
   tabVersion[currentNumberOfTabs] = "1";
   tabDescription[currentNumberOfTabs] = "";
   numOfSteering[currentNumberOfTabs] = -1;
   numOfThreadFunctions[currentNumberOfTabs] = -1;
   numOfTabMenu[currentNumberOfTabs] = -1;
   tabHierarchyNumOfChildren[currentNumberOfTabs] = 0;
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subtab
      if (type == 1 && !strcmp((const char*)name,"Tab")) {
         // set tab as parent for subsequent tabs
         recursiveTabDepth++;
         parent[recursiveTabDepth] = tabName[currentNumberOfTabs];
         tabParentIndex[numOfTab+1] = currentNumberOfTabs;
         tabHierarchyNumOfChildren[currentNumberOfTabs]++;
         // read subtab
         if (!ReadXMLTab()) 
            return false;
         continue;
      }
      // end tab
      if (type == 15 && !strcmp((const char*)name,"Tab")) {
         // check input
         if (currentTabName=="") {
            cout << "The " << (currentNumberOfTabs+1) << ". Tab has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         recursiveTabDepth--;
         return true;
      }
      // tab name
      if (type == 1 && !strcmp((const char*)name,"TabName")) {
         xml->GetValue(tabName[currentNumberOfTabs],tabName[currentNumberOfTabs]);
         currentTabName = tabName[currentNumberOfTabs];
         // output
         if (makeOutput) for (i=0;i<recursiveTabDepth;i++) cout << "   ";
         if (makeOutput) tabName[currentNumberOfTabs].WriteLine();
      }
      // tab title
      if (type == 1 && !strcmp((const char*)name,"TabTitle"))
         xml->GetValue(tabTitle[currentNumberOfTabs],tabTitle[currentNumberOfTabs]);
      // tab author
      if (type == 1 && !strcmp((const char*)name,"Author"))
         xml->GetValue(tabAuthor[currentNumberOfTabs],tabAuthor[currentNumberOfTabs]);
      // tab version
      if (type == 1 && !strcmp((const char*)name,"TabVersion"))
         xml->GetValue(tabVersion[currentNumberOfTabs],tabVersion[currentNumberOfTabs]);
      // tab description
      if (type == 1 && !strcmp((const char*)name,"TabDescription"))
         xml->GetValue(tabDescription[currentNumberOfTabs],tabDescription[currentNumberOfTabs]);
      // tab steering parameters
      if (type == 1 && !strcmp((const char*)name,"SteeringParameters")) {
         // read steering parameter
         steerName[currentNumberOfTabs][0] = "Steering";
         steerParent[currentNumberOfTabs][0] = -1;
         actualSteerIndex = 0;
         recursiveSteerDepth = 0;
         if (!ReadXMLSteering(currentNumberOfTabs)) 
            return false;
         numOfSteering[currentNumberOfTabs]++;
      }
      // tab threadFunctions
      if (type == 1 && !strcmp((const char*)name,"ThreadFunctions")) {
         if (makeOutput) for (i=0;i<recursiveTabDepth+1;i++) cout << "   ";
         if (makeOutput) cout << "ThreadFunctions:"<<endl;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // end
            if (type == 15 && !strcmp((const char*)name,"ThreadFunctions"))
               break;
            // thread function
            if (type == 1 && !strcmp((const char*)name,"ThreadFunction")) {
               // count thread functions
               numOfThreadFunctions[currentNumberOfTabs]++;
               if (numOfThreadFunctions[currentNumberOfTabs]>=maxNumberOfThreadFunctions) {
                  cout << "Maximal number of thread functions reached : " << maxNumberOfThreadFunctions << " !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] = "";               
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // end
                  if (type == 15 && !strcmp((const char*)name,"ThreadFunction")){
                     // output
                     if (makeOutput) for (i=0;i<recursiveTabDepth+2;i++) cout << "   ";
                     if (makeOutput) threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]].WriteLine();
                     break;
                  }
                  if (type == 1 && !strcmp((const char*)name,"FunctionName"))
                     xml->GetValue(threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]],threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]);
               }
               // check input
               if (threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]=="") {
                  cout << "A thread function of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Name !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               for(j=0;j<numOfThreadFunctions[currentNumberOfTabs];j++){
                  if (threadFunctionName[currentNumberOfTabs][j]==threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]) {
                     cout << "Two thread functions of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Name !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
            }
         }
      }
      // tab menu
      if (type == 1 && !strcmp((const char*)name,"Menus")) {
         if (makeOutput) for (i=0;i<recursiveTabDepth+1;i++) cout << "   ";
         if (makeOutput) cout << "Menus:"<<endl;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // end
            if (type == 15 && !strcmp((const char*)name,"Menus")){
	       numOfTabMenu[currentNumberOfTabs]++;
               break;
	    }
            // menu
            if (type == 1 && !strcmp((const char*)name,"Menu")) {
               // count menus
               numOfTabMenu[currentNumberOfTabs]++;
	       numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]] = -1;
               if (numOfTabMenu[currentNumberOfTabs]>=maxNumberOfTabMenus) {
                  cout << "Maximal number of menus reached : " << maxNumberOfTabMenus << " !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               tabHierarchyMenuTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // end
                  if (type == 15 && !strcmp((const char*)name,"Menu")){
                     if (makeOutput) for (i=0;i<recursiveTabDepth+2;i++) cout << "   ";
                     if (makeOutput) tabHierarchyMenuTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]].WriteLine();
		     numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]++;
                     for(j=0;j<numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]];j++){
                        if (makeOutput) for (i=0;i<recursiveTabDepth+3;i++) cout << "   ";
                        if (makeOutput) tabHierarchyMenuItemTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][j].WriteLine();}
                     break;
		  }
		  if (type == 1 && !strcmp((const char*)name,"MenuTitle"))
		     xml->GetValue(tabHierarchyMenuTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]],tabHierarchyMenuTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]);
		  // tab menu items
		  if (type == 1 && !strcmp((const char*)name,"MenuItems")) {
		     while (xml->NextLine()) {
			type = xml->GetType();
			name = xml->GetName();
			// end
			if (type == 15 && !strcmp((const char*)name,"MenuItems"))
			   break;
			if (type == 1 && !strcmp((const char*)name,"MenuItem")){
			   // end
			   // count menu items
			   numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]++;
			   if (numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]>=maxNumberOfTabMenuItems) {
			      cout << "Maximal number of menu items reached : " << maxNumberOfTabMenuItems << " !" << endl;
			      cout << "Terminating program." << endl;
			      return false;
			   }
			   tabHierarchyMenuItemTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]] = "";
			   while (xml->NextLine()) {
			      type = xml->GetType();
			      name = xml->GetName();
			      // end
			      if (type == 15 && !strcmp((const char*)name,"MenuItem"))
				 break;
			      if (type == 1 && !strcmp((const char*)name,"MenuItemTitle"))
				 xml->GetValue(tabHierarchyMenuItemTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]],tabHierarchyMenuItemTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]]);
			      if (type == 1 && !strcmp((const char*)name,"MenuItemID")){
				 xml->GetValue(tmp,tmp);
				 tabHierarchyMenuItemID[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]] = strtol(tmp.Data(),&cstop,10);
			      }
			   }
			   // check input
			   if (tabHierarchyMenuItemTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]]=="") {
			      cout << "A menu item of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
			      cout << "Terminating program." << endl;
			      return false;
			   }
			   for(j=0;j<numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]];j++){
			      if (tabHierarchyMenuItemTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][j]==tabHierarchyMenuItemTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]]){
				 cout << "Two menu items of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
				 cout << "Terminating program." << endl;
				 return false;
			      }
			   }
			   if (tabHierarchyMenuItemID[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]]<0){
			      cout << "MenuItemID must not be negative vale !" << endl;
			      cout << "Terminating program." << endl;
			      return false;
			   }
			   if (tabHierarchyMenuItemID[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]][numOfTabMenuItem[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]]>maxNumberOfTabMenus*maxNumberOfTabMenuItems){
			      cout << "MenuItemID must be less than "<<maxNumberOfTabMenus*maxNumberOfTabMenuItems<<" !" << endl;
			      cout << "Terminating program." << endl;
			      return false;
			   }
			}
		     }
		  }
	       }
	       // check input
	       if (tabHierarchyMenuTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]=="") {
                  cout << "A menu of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               for(j=0;j<numOfTabMenu[currentNumberOfTabs];j++){
                  if (tabHierarchyMenuTitle[currentNumberOfTabs][j]==tabHierarchyMenuTitle[currentNumberOfTabs][numOfTabMenu[currentNumberOfTabs]]) {
                     cout << "Two thread functions of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
            }
         }
      }
   }
   return true;
}

bool ArgusBuilder::WriteTabCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   int lenTot,ll,i,j;
   char *pBuffer=NULL;
   int bufferLen=0;
   char *pos;
   int fileHandle;
   ROMEString format;
   ROMEString discript;
   ROMEString str;
   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iTab=0;iTab<numOfTab;iTab++) {
      // File name
      cppFile.SetFormatted("%s/src/tabs/%sT%s.cpp",outDir.Data(),shortCut.Data(),tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("//// Author: %s\n",tabAuthor[iTab].Data());
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      ll = 73-shortCut.Length();
      format.SetFormatted("// %%sT%%-%d.%ds //\n",ll,ll);
      buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("//                                                                            //\n");
      discript = tabDescription[iTab].Data();
      pos = (char*)discript.Data();
      lenTot = discript.Length();
      while (pos-discript.Data() < lenTot) {
         if (lenTot+(discript.Data()-pos)<74) 
            i=lenTot+(discript.Data()-pos);
         else for (i=74;pos[i]!=32&&i>0;i--) {}
         if (i<=0)
            i=TMath::Min(75,lenTot);
         pos[i] = 0;
         buffer.AppendFormatted("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      buffer.AppendFormatted("// \n");
      buffer.AppendFormatted("// \n");
      buffer.AppendFormatted("// This header has been generated by the ArgusBuilder.\n");
      buffer.AppendFormatted("//                                                                            //\n");
      // Thread
      if(numOfThreadFunctions[iTab]>0) {
         buffer.AppendFormatted("// This tab has following thread functions.                                   //\n");
         for(i=0; i<numOfThreadFunctions[iTab]; i++) {
            format.SetFormatted("//    %%s%%%ds//\n", 72-threadFunctionName[iTab][i].Length());
            buffer.AppendFormatted((char*)format.Data(), threadFunctionName[iTab][i].Data(), "");
         }
         buffer.AppendFormatted("//                                                                            //\n");
      }
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");
      // Write file
      bool replaceHeader = true;
      bool replaceBody = true;
      struct stat buf;
      int nb=0;
      if( !stat( cppFile.Data(), &buf )) {
         replaceBody = false;
         fileHandle = open(cppFile.Data(),O_RDONLY);
         nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
         pBuffer = fileBuffer;
         char *pend = "/////////////////////////////////////----///////////////////////////////////////";
         pBuffer = strstr(pBuffer,pend);
         if (pBuffer==NULL) {
            if (makeOutput) cout << "\n\nError : File '" << cppFile.Data() << "' has an invalid header !!!" << endl;
            replaceHeader = false;
         }
         bufferLen = nb-(pBuffer-fileBuffer);
         close(fileHandle);
         if (replaceHeader) {
            replaceHeader = false;
            for (i=0;i<pBuffer-fileBuffer;i++) {
               if (buffer[i] != fileBuffer[i]) {
                  replaceHeader = true;
               }
            }
         }
      }
      if (replaceHeader || replaceBody) {
         fileHandle = open(cppFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(cppFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << cppFile.Data() << endl;
         if (replaceHeader) {
            nb = write(fileHandle,buffer.Data(), buffer.Length());
         }
         if (replaceBody) {
            buffer.Resize(0);
            // Header
            buffer.AppendFormatted("\n\n#include \"include/tabs/%sT%s.h\"\n",shortCut.Data(),tabName[iTab].Data());
            buffer.AppendFormatted("\nClassImp(%sT%s)\n\n",shortCut.Data(),tabName[iTab].Data());
            // Functions
            buffer.AppendFormatted("void %sT%s::Init()\n",shortCut.Data(),tabName[iTab].Data());
            buffer.AppendFormatted("{\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            buffer.AppendFormatted("void %sT%s::MenuClicked(Long_t param)\n",shortCut.Data(),tabName[iTab].Data());
            buffer.AppendFormatted("{\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            // Thread
            for(i=0; i<numOfThreadFunctions[iTab]; i++) {
               buffer.AppendFormatted("void %sT%s::%s()\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   gSystem->Sleep(10000);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            nb = write(fileHandle,buffer.Data(), buffer.Length());
         }
         else {
            nb = write(fileHandle,pBuffer+80, bufferLen-80);
         }
         close(fileHandle);
      }
   }
   return true;
}

bool ArgusBuilder::WriteTabH() {
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   ROMEString format;
   int i,j;
   int fileHandle;
   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTab=0;iTab<numOfTab;iTab++) {
      // File name
      hFile.SetFormatted("%s/include/tabs/%sT%s_Base.h",outDir.Data(),shortCut.Data(),tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");
      // Header
      buffer.AppendFormatted("#ifndef %sT%s_Base_H\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("#define %sT%s_Base_H\n\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("#include <TGFrame.h>\n");
      buffer.AppendFormatted("#include \"include/monitor/%sMonitor.h\"\n",shortCut.Data());
#if defined ( R__UNIX ) 
      buffer.AppendFormatted("#include <TThread.h>\n");
#endif
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("typedef struct %sArgs{\n",tabName[iTab].Data());
      buffer.AppendFormatted("   void* inst;\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("   Long_t msg;\n");
      buffer.AppendFormatted("   Long_t param1;\n");
      buffer.AppendFormatted("   Long_t param2;\n");
      buffer.AppendFormatted("};\n");
      // Class
      buffer.AppendFormatted("\nclass %sT%s_Base : public TGCompositeFrame\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("protected:\n");
      // Fields
      if (numOfSteering[iTab]>0) {
         WriteSteeringClass(buffer,0,iTab,1);
         buffer.AppendFormatted("\n");
      }
      if (numOfSteering[iTab]>0) {
         buffer.AppendFormatted("   Steering* fSteering; // Handle to Steering class\n\n");
      }
      // Thread
      for(i=0; i<numOfThreadFunctions[iTab]; i++) {
#if defined ( R__UNIX )
         buffer.AppendFormatted("   TThread* m%s;\n", threadFunctionName[iTab][i].Data());
#elif defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("   LPDWORD f%sId;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   LPDWORD f%sExCode;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   HANDLE  f%sHandle;\n", threadFunctionName[iTab][i].Data());
#endif
         buffer.AppendFormatted("   bool  f%sActive;\n", threadFunctionName[iTab][i].Data());
	 buffer.AppendFormatted("   Int_t f%sNumberOfLoops;\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t f%sInterval;\n",threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   Int_t fVersion; // Version number\n");
      buffer.AppendFormatted("   Bool_t fActive; // is Active\n");
      int nameLen = -1;
      // Methods
      buffer.AppendFormatted("public:\n");
      // Constructor
      buffer.AppendFormatted("   // Constructor\n");
      buffer.AppendFormatted("   %sT%s_Base():TGCompositeFrame(){\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("      fVersion = %s;\n",tabVersion[iTab].Data());
      buffer.AppendFormatted("      fActive = false;\n");
      if (numOfSteering[iTab]>0) {
         buffer.AppendFormatted("      fSteering = new Steering();\n");
      }
      for(i=0; i<numOfThreadFunctions[iTab]; i++) {
#if defined ( R__UNIX )
         buffer.AppendFormatted("      m%s = 0;\n", threadFunctionName[iTab][i].Data());
#elif defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("      f%sId = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sExCode = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sHandle = 0;\n", threadFunctionName[iTab][i].Data());
#endif
         buffer.AppendFormatted("      f%sActive = false;\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   ~%sT%s_Base(){\n",shortCut.Data(),tabName[iTab].Data());
      for(i=0; i<numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      // Thread      
      buffer.AppendFormatted("   virtual Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2){\n");
      buffer.AppendFormatted("      return RunProcessMessageThread(msg, param1, param2);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   virtual Bool_t ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2){return kTRUE;}\n");
#if defined ( R__UNIX )
      buffer.AppendFormatted("   static void ThreadProcessMessageThread(void* arg){\n");
      buffer.AppendFormatted("      ((%sT%s_Base*)((%sArgs*)arg)->inst)->ProcessMessageThread(((%sArgs*)arg)->msg, ((%sArgs*)arg)->param1, ((%sArgs*)arg)->param2);\n",shortCut.Data(),tabName[iTab].Data(),tabName[iTab].Data(),tabName[iTab].Data(),tabName[iTab].Data(),tabName[iTab].Data());
      buffer.AppendFormatted("   }\n");
#elif defined ( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("   static DWORD WINAPI ThreadProcessMessageThread(void* arg){\n");
      buffer.AppendFormatted("      %sT%s_Base* inst = (%sT%s_Base*) arg;\n",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("      ((%sArgs*)arg)->inst->ProcessMessageThread(((%sArgs*)arg)->msg, ((%sArgs)arg*)->param1, ((%sArgs*)arg)->param2);\n",tabName[iTab].Data(),tabName[iTab].Data(),tabName[iTab].Data(),tabName[iTab].Data());
      buffer.AppendFormatted("      return 0;\n");
      buffer.AppendFormatted("   }\n");
#endif      
      buffer.AppendFormatted("   bool RunProcessMessageThread(Long_t msg, Long_t param1, Long_t param2){\n");
      buffer.AppendFormatted("      %sArgs* arg = new %sArgs();\n",tabName[iTab].Data(),tabName[iTab].Data());
      buffer.AppendFormatted("      arg->inst   = this;\n");
      buffer.AppendFormatted("      arg->msg    = msg;\n");
      buffer.AppendFormatted("      arg->param1 = param1;\n");
      buffer.AppendFormatted("      arg->param2 = param2;\n");
#if defined ( R__UNIX )
      buffer.AppendFormatted("      TThread* mProcessMessageThread = new TThread(\"processMessageThread\",(void(*) (void *))&ThreadProcessMessageThread,(void*) arg);\n");
      buffer.AppendFormatted("      mProcessMessageThread->Run();\n");
#elif defined ( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("      LPDWORD fProcessMessageThreadId;\n");
      buffer.AppendFormatted("      CloseHandle(CreateThread(NULL,1024,&ThreadProcessMessageThread,(LPVOID)arg,0,&processMessageThreadId));\n");
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("      }\n");
#endif
      buffer.AppendFormatted("      return true;\n");
      buffer.AppendFormatted("   }\n");     
      for(i=0; i<numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   //%s\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   virtual void %s()\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("      cout<<endl\n");
         buffer.AppendFormatted("          <<\" Thread function %s is not implemented.\"<<endl\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
         buffer.AppendFormatted("          <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %s();\"<<endl\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %sT%s::%s()\"<<endl\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   {\"<<endl\n");
         buffer.AppendFormatted("          <<\"      cout<<\\\"Thread function %s is running.\\\"<<endl;\"<<endl\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   }\"<<endl<<endl;\n");
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
#if defined ( R__UNIX )
         buffer.AppendFormatted("   static void Thread%s(void* arg){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      TThread::SetCancelOn();\n");
         buffer.AppendFormatted("      TThread::SetCancelDeferred();\n");
         buffer.AppendFormatted("      %sT%s_Base* inst = (%sT%s_Base*) arg;\n",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
         buffer.AppendFormatted("      Int_t iLoop = 0;\n");
//         buffer.AppendFormatted("      int meid=TThread::SelfId(); // get pthread id\n");
         buffer.AppendFormatted("      while(inst->f%sActive){\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::CancelPoint();\n");
         buffer.AppendFormatted("         inst->%s(); // call the user defined threaded function\n",threadFunctionName[iTab][i].Data());
	 buffer.AppendFormatted("         if(inst->f%sNumberOfLoops != 0 && ++iLoop>=inst->f%sNumberOfLoops)\n",threadFunctionName[iTab][i].Data(),threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            inst->Stop%s();\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         gSystem->Sleep(inst->f%sInterval);\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
#elif defined ( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("   static DWORD WINAPI Thread%s(void* arg){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      %sT%s_Base* inst = (%sT%s_Base*) arg;\n",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
         buffer.AppendFormatted("      Int_t iLoop = 0;\n");
         buffer.AppendFormatted("      GetExitCodeThread(f%sHandle, &f%sExCode);\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      while(inst->f%sActive && f%sExCode == STILL_ACTIVE && (f%sNumberOfLoops == 0 || iLoop<f%sNumberOfLoops)){\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         inst->%s(); // call the user defined threaded function\n",threadFunctionName[iTab][i].Data());
	 buffer.AppendFormatted("         if(f%sNumberOfLoops != 0 && ++iLoop>=f%sNumberOfLoops)\n",threadFunctionName[iTab][i].Data(),threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            Stop%s();\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         gSystem->Sleep(f%sInterval);\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return 0;\n");
         buffer.AppendFormatted("   }\n");
#endif
         buffer.AppendFormatted("   bool Start%s(Int_t interval = 1000, Int_t nloop = 0){\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = true;\n",threadFunctionName[iTab][i].Data());
	 buffer.AppendFormatted("      f%sNumberOfLoops = nloop;\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sInterval = interval;\n",threadFunctionName[iTab][i].Data());
#if defined ( R__UNIX )
         buffer.AppendFormatted("      if(!m%s){\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = new TThread(\"Thread%s\",(void(*) (void *))&Thread%s,(void*) this);\n",threadFunctionName[iTab][i].Data(),threadFunctionName[iTab][i].Data(),threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s->Run();\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return true;\n");
#elif defined ( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("      if(!f%sHandle){\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         f%sHandle = CreateThread(NULL,1024,&%s,0,0,&%sId);\n",threadFunctionName[iTab][i].Data(),threadFunctionName[iTab][i].Data(),threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         if( f!%sHandle ){\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            f%sActive = false;\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            %sId = 0;\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            %sExCode = 0;\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            return false;\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return true;\n");
#endif
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   bool Stop%s(){\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = false;\n",threadFunctionName[iTab][i].Data());
#if defined ( R__UNIX )
         buffer.AppendFormatted("      gSystem->Sleep(1000); // wait a while for threads to halt\n");
         buffer.AppendFormatted("      if(m%s){\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::Delete(m%s);\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = 0;\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return true;\n");
#elif defined ( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("      if(f%sHanele){\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         CloseHandle(f%sHandle));\n",threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return true;\n");
#endif
         buffer.AppendFormatted("   } \n");         
         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("\n");
      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTab]>0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; };\n");
      }
      buffer.AppendFormatted("   Bool_t GetActive() { return fActive; };\n");
      buffer.AppendFormatted("   void SetActive(Bool_t active) { fActive = active; };\n");
      buffer.AppendFormatted("   virtual void MenuClicked(Long_t param)\n");
      buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("      cout<<endl\n");
      buffer.AppendFormatted("          <<\" Function MenuClicked is not implemented in %s.\"<<endl\n",tabHierarchyName[iTab].Data());
      buffer.AppendFormatted("          <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
      buffer.AppendFormatted("          <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   void MenuClicked(Long_t param);\"<<endl\n");
      buffer.AppendFormatted("          <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   void %sT%s::MenuClicked(Long_t param)\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   {\"<<endl\n");
      buffer.AppendFormatted("          <<\"      cout<<\\\"param = \\\"<< param <<endl;\"<<endl\n");
      buffer.AppendFormatted("          <<\"   }\"<<endl<<endl;\n");
      buffer.AppendFormatted("   }\n");     
      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s)\n",shortCut.Data(),tabName[iTab].Data(),tabVersion[iTab].Data());
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n",shortCut.Data(),tabName[iTab].Data());
      // Write File
      fileHandle = open(hFile.Data(),O_RDONLY);
      int nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
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
      // User H-File
      struct stat buf;
      hFile.SetFormatted("%s/include/tabs/%sT%s.h",outDir.Data(),shortCut.Data(),tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// Editable class file for %s tab.\n",tabName[iTab].Data());
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");
      // Header
      buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("#include <include/tabs/%sT%s_Base.h>\n",shortCut.Data(),tabName[iTab].Data());
      // Class
      buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("protected:\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("public:\n");
      // Constructor
      buffer.AppendFormatted("   %sT%s():%sT%s_Base()\n",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   ~%sT%s()\n",shortCut.Data(),tabName[iTab].Data());
      buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   void Init();\n");
      buffer.AppendFormatted("   void MenuClicked(Long_t param);\n");
      // Thread
      for(i=0; i<numOfThreadFunctions[iTab]; i++){
         buffer.AppendFormatted("   void %s();\n",threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("\n");
      // Fields
      buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n",shortCut.Data(),tabName[iTab].Data(),tabVersion[iTab].Data());
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),tabName[iTab].Data());
      // Write File
      if(open(hFile.Data(),O_RDONLY) == -1){
         fileHandle = open(hFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(hFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << hFile.Data() << endl;
         nb = write(fileHandle,buffer.Data(), buffer.Length());
         close(fileHandle);
      }
   }
   return true;
}

bool ArgusBuilder::WriteTabConfigWrite(ROMEString &buffer,int parentIndex,ROMEString& pointer,int tab) {
   int j,i;
   // max tab switch name length
   int switchLen = -1;
   ROMEString switchString;
   for (i=0;i<numOfTabHierarchy;i++) {
      int index = tabHierarchyParentIndex[i];
      switchString = tabHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabHierarchyName[index].Data());
         index = tabHierarchyParentIndex[index];
      }
      if (switchLen<(int)switchString.Length()) switchLen = switchString.Length();
   }
   ROMEString blank = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");
   ROMEString pointerI;
   for (i=0;i<numOfTabHierarchy;i++) {
      if (tabHierarchyParentIndex[i]!=parentIndex)
         continue;
      int index = tabHierarchyParentIndex[i];
      switchString = tabHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabHierarchyName[index].Data());
         index = tabHierarchyParentIndex[index];
      }
      pointerI = pointer;
      pointerI.AppendFormatted("->f%sTab",tabHierarchyName[i].Data());
      buffer.AppendFormatted("%s      if (fConfigData%sModified || index==0) {\n",blank.Data(),pointerI.Data());
      buffer.AppendFormatted("%s         // %s\n",blank.Data(),pointerI.Data());
      buffer.AppendFormatted("%s         xml->StartElement(\"Tab\");\n",blank.Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"TabName\",\"%s\");\n",blank.Data(),tabHierarchyName[i].Data());
      buffer.AppendFormatted("%s         if (index==0) {\n",blank.Data());
      buffer.AppendFormatted("%s            if (gWindow->GetTabSwitches()->%s)\n",blank.Data(),switchString.Data());
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"true\");\n",blank.Data());
      buffer.AppendFormatted("%s            else\n",blank.Data());
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"false\");\n",blank.Data());
      buffer.AppendFormatted("%s         }\n",blank.Data());
      buffer.AppendFormatted("%s         else if (fConfigData%s->fActiveModified)\n",blank.Data(),pointerI.Data());
      buffer.AppendFormatted("%s            xml->WriteElement(\"Active\",(char*)fConfigData%s->fActive.Data());\n",blank.Data(),pointerI.Data());
      // Steering parameter
      if (numOfSteering[tabHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("%s         // steering parameters\n",blank.Data());
         buffer.AppendFormatted("%s         if (fConfigData%s->fSteeringModified || index==0) {\n",blank.Data(),pointerI.Data());
         buffer.AppendFormatted("%s            ROMEString value;\n",blank.Data());
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("fConfigData%s->fSteering",pointerI.Data());
         steerPointerT.SetFormatted("((%sT%s*)gWindow->Get%s%03dTab())->GetSP()",shortCut.Data(),tabName[tabHierarchyClassIndex[i]].Data(),tabHierarchyName[i].Data(),i);
         WriteSteeringConfigWrite(buffer,0,tabHierarchyClassIndex[i],pointerT,steerPointerT,3+tab);
      }
      if (numOfSteering[tabHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("%s         }\n",blank.Data());
      
      WriteTabConfigWrite(buffer,i,pointerI,tab+1);
      buffer.AppendFormatted("%s         xml->EndElement();\n",blank.Data());
      buffer.AppendFormatted("%s      }\n",blank.Data());
   }
   return true;
}

bool ArgusBuilder::WriteTabConfigClass(ROMEString &buffer,int parentIndex,int tab) {
   int j,i;
   ROMEString blank = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");
   for (i=0;i<numOfTabHierarchy;i++) {
      if (tabHierarchyParentIndex[i]!=parentIndex)
         continue;
      buffer.AppendFormatted("%s      class %sTab {\n",blank.Data(),tabHierarchyName[i].Data());
      buffer.AppendFormatted("%s         public:\n",blank.Data());
      buffer.AppendFormatted("%s            ROMEString  fActive;\n",blank.Data());
      buffer.AppendFormatted("%s            bool        fActiveModified;\n",blank.Data());
      if (numOfSteering[tabHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("%s         // steering parameters\n",blank.Data());
         buffer.AppendFormatted("%s         class Steering {\n",blank.Data());
         buffer.AppendFormatted("%s         public:\n",blank.Data());
         WriteSteeringConfigClass(buffer,0,tabHierarchyClassIndex[i],tab+3);
         buffer.AppendFormatted("%s         };\n",blank.Data());
         buffer.AppendFormatted("%s         Steering *fSteering;\n",blank.Data());
         buffer.AppendFormatted("%s         bool   fSteeringModified;\n",blank.Data());
      }
      // Constructor
      buffer.AppendFormatted("%s      public:\n",blank.Data());
      buffer.AppendFormatted("%s         %sTab() {\n",blank.Data(),tabHierarchyName[i].Data());
      if (numOfSteering[tabHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("%s            fSteeringModified = false;\n",blank.Data());
         buffer.AppendFormatted("%s            fSteering = new Steering();\n",blank.Data());
      }
      for (j=0;j<numOfTabHierarchy;j++) {
         if (tabHierarchyParentIndex[j]!=i)
            continue;
         buffer.AppendFormatted("%s            f%sTab = new %sTab();\n",blank.Data(),tabHierarchyName[j].Data(),tabHierarchyName[j].Data());
      }
      buffer.AppendFormatted("%s         };\n",blank.Data());
      // Sub classes
      WriteTabConfigClass(buffer,i,tab+1);
      buffer.AppendFormatted("%s      };\n",blank.Data());
      buffer.AppendFormatted("%s      %sTab *f%sTab;\n",blank.Data(),tabHierarchyName[i].Data(),tabHierarchyName[i].Data());
      buffer.AppendFormatted("%s      bool   f%sTabModified;\n",blank.Data(),tabHierarchyName[i].Data());
   }
   return true;
}
