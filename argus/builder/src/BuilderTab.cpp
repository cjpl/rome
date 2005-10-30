/********************************************************************

  BuilderTab.cpp, Ryu Sawada

  $Id$

********************************************************************/
#include "ArgusBuilder.h"

Bool_t ArgusBuilder::ReadXMLMenu(Int_t currentNumberOfTabs)
{
   ROMEString tmp;
   Int_t type, i, j;
   Char_t *name;

   Int_t currentNumberOfMenus = numOfMenu[currentNumberOfTabs];
   menuDepth[currentNumberOfTabs][currentNumberOfMenus] = recursiveMenuDepth;
   // count menus
   numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] = -1;
   if (currentNumberOfMenus >= maxNumberOfMenus) {
      cout << "Maximal number of menus reached : " << maxNumberOfMenus << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   menuTitle[currentNumberOfTabs][currentNumberOfMenus] = "";

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();

      // end
      if (type == 15 && !strcmp((const Char_t *) name, "Menu")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 2; i++)
               cout << "   ";
         if (makeOutput)
            menuTitle[currentNumberOfTabs][currentNumberOfMenus].WriteLine();
         numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
         for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
            if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE) {
               if (makeOutput)
                  for (i = 0; i < recursiveTabDepth + 3; i++)
                     cout << "   ";
               if (makeOutput)
                  menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j].WriteLine();
            }
         }
         break;
      }
      if (type == 1 && !strcmp((const Char_t *) name, "MenuTitle"))
         xml->GetValue(menuTitle[currentNumberOfTabs][currentNumberOfMenus], menuTitle[currentNumberOfTabs][currentNumberOfMenus]);

      // tab menu items
      if (type == 1 && !strcmp((const Char_t *) name, "MenuItems")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();

            // end
            if (type == 15 && !strcmp((const Char_t *) name, "MenuItems"))
               break;

            // menu
            if (type == 1 && !strcmp((const Char_t *) name, "Menu")) {
               recursiveMenuDepth++;
               numOfMenu[currentNumberOfTabs]++;
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = numOfMenu[currentNumberOfTabs];
               if (!ReadXMLMenu(currentNumberOfTabs))
                  return kFALSE;
               recursiveMenuDepth--;
            }
            // Splitter
            if (type == 1 && !strcmp((const Char_t *) name, "Line")) {
               // count menu items
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = LINE_TITLE;
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // end
                  if (type == 15 && !strcmp((const Char_t *) name, "Line"))
                     break;
               }
            }
            // menu item
            if (type == 1 && !strcmp((const Char_t *) name, "MenuItem")) {
               // count menu items
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  // end
                  if (type == 15 && !strcmp((const Char_t *) name, "MenuItem"))
                     break;
                  if (type == 1 && !strcmp((const Char_t *) name, "MenuItemTitle"))
                     xml->GetValue(menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]], menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                  if (type == 1 && !strcmp((const Char_t *) name, "MenuItemEnumName")) {
                     xml->GetValue(menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]], menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                  }
               }

               // check input
               if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] == "") {
                  cout << currentNumberOfTabs << endl;
                  cout << currentNumberOfMenus << endl;
                  cout << numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] << endl;
                  cout << "A menu item of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
                  if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE && menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j]
                      == menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]) {
                     cout << "Two menu items of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
                     cout << "Terminating program." << endl;
                     return kFALSE;
                  }
               }
            }
         }
      }
   }

   // check input
   if (menuTitle[currentNumberOfTabs][currentNumberOfMenus] == "") {
      cout << "A menu of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   for (j = 0; j < currentNumberOfMenus; j++) {
      if (menuTitle[currentNumberOfTabs][j] == menuTitle[currentNumberOfTabs][currentNumberOfMenus]) {
         cout << "Two menus of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
         cout << "Terminating program." << endl;
         return kFALSE;
      }
   }

   return kTRUE;
}

Bool_t ArgusBuilder::ReadXMLTab()
{
   // read the tab definitions out of the xml file
   ROMEString tmp;
   Char_t *name;
   Int_t type, i, j;
   ROMEString currentTabName = "";
   Int_t currentNumberOfTabs;

   // count tabs
   numOfTab++;
   numOfTask++;
   numOfTaskHierarchy++;
   currentNumberOfTabs = numOfTab;
   if (currentNumberOfTabs >= maxNumberOfTabs) {
      cout << "Maximal number of tabs reached : " << maxNumberOfTabs << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   // initialisation
   tabName[currentNumberOfTabs] = "";
   tabTitle[currentNumberOfTabs] = "";
   tabAuthor[currentNumberOfTabs] = mainAuthor;
   tabVersion[currentNumberOfTabs] = "1";
   tabDescription[currentNumberOfTabs] = "";
   numOfSteering[currentNumberOfTabs] = -1;
   numOfThreadFunctions[currentNumberOfTabs] = -1;
   numOfMenu[currentNumberOfTabs] = -1;
   tabNumOfChildren[currentNumberOfTabs] = 0;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();

      // subtab
      if (type == 1 && !strcmp((const Char_t *) name, "Tab")) {
         // set tab as parent for subsequent tabs
         recursiveTabDepth++;
         tabParentIndex[numOfTab + 1] = taskHierarchyParentIndex[numOfTab + 1] = currentNumberOfTabs;
         tabNumOfChildren[currentNumberOfTabs]++;

         // read subtab
         if (!ReadXMLTab())
            return kFALSE;
         continue;
      }
      // end tab
      if (type == 15 && !strcmp((const Char_t *) name, "Tab")) {
         // check input
         if (currentTabName == "") {
            cout << "The " << (currentNumberOfTabs + 1) << ". Tab has no name !" << endl;
            cout << "Terminating program." << endl;
            return kFALSE;
         }
         recursiveTabDepth--;
         return kTRUE;
      }
      // tab name
      if (type == 1 && !strcmp((const Char_t *) name, "TabName")) {
         xml->GetValue(tabName[currentNumberOfTabs], tabName[currentNumberOfTabs]);
         currentTabName = tabName[currentNumberOfTabs];
         // output
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth; i++)
               cout << "   ";
         if (makeOutput)
            tabName[currentNumberOfTabs].WriteLine();
      }
      // tab title
      if (type == 1 && !strcmp((const Char_t *) name, "TabTitle"))
         xml->GetValue(tabTitle[currentNumberOfTabs], tabTitle[currentNumberOfTabs]);

      // tab author
      if (type == 1 && !strcmp((const Char_t *) name, "Author"))
         xml->GetValue(tabAuthor[currentNumberOfTabs], tabAuthor[currentNumberOfTabs]);

      // tab version
      if (type == 1 && !strcmp((const Char_t *) name, "TabVersion"))
         xml->GetValue(tabVersion[currentNumberOfTabs], tabVersion[currentNumberOfTabs]);

      // tab description
      if (type == 1 && !strcmp((const Char_t *) name, "TabDescription"))
         xml->GetValue(tabDescription[currentNumberOfTabs], tabDescription[currentNumberOfTabs]);

      // tab steering parameters
      if (type == 1 && !strcmp((const Char_t *) name, "SteeringParameters")) {
         // read steering parameter
         steerName[currentNumberOfTabs][0] = "Steering";
         steerParent[currentNumberOfTabs][0] = -1;
         actualSteerIndex = 0;
         recursiveSteerDepth = 0;
         if (!ReadXMLSteering(currentNumberOfTabs))
            return kFALSE;
         numOfSteering[currentNumberOfTabs]++;
      }
      // tab threadFunctions
      if (type == 1 && !strcmp((const Char_t *) name, "ThreadFunctions")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout << "   ";
         if (makeOutput)
            cout << "ThreadFunctions:" << endl;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // end
            if (type == 15 && !strcmp((const Char_t *) name, "ThreadFunctions"))
               break;

            // thread function
            if (type == 1 && !strcmp((const Char_t *) name, "ThreadFunction")) {
               // count thread functions
               numOfThreadFunctions[currentNumberOfTabs]++;
               if (numOfThreadFunctions[currentNumberOfTabs] >= maxNumberOfThreadFunctions) {
                  cout << "Maximal number of thread functions reached : " << maxNumberOfThreadFunctions << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] = "";
               numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  // end
                  if (type == 15 && !strcmp((const Char_t *) name, "ThreadFunction")) {
                     // output
                     if (makeOutput)
                        for (i = 0; i < recursiveTabDepth + 2; i++)
                           cout << "   ";
                     if (makeOutput)
                        threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]].WriteLine();
                     break;
                  }
                  if (type == 1 && !strcmp((const Char_t *) name, "FunctionName"))
                     xml->GetValue(threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]], threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]);
                  if (type == 1 && !strcmp((const Char_t *) name, "FunctionArgument")) {
                     xml->GetValue(threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]], threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]]);
                     numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]++;
                  }
               }

               // check input
               if (threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] == "") {
                  cout << "A thread function of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Name !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               for (j = 0; j < numOfThreadFunctions[currentNumberOfTabs]; j++) {
                  if (threadFunctionName[currentNumberOfTabs][j] == threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]) {
                     cout << "Two thread functions of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Name !" << endl;
                     cout << "Terminating program." << endl;
                     return kFALSE;
                  }
               }
            }
         }
      }
      // tab menu
      if (type == 1 && !strcmp((const Char_t *) name, "Menus")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout << "   ";
         if (makeOutput)
            cout << "Menus:" << endl;
         recursiveMenuDepth = 0;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();

            // end
            if (type == 15 && !strcmp((const Char_t *) name, "Menus")) {
               numOfMenu[currentNumberOfTabs]++;
               break;
            }
            // menu
            if (type == 1 && !strcmp((const Char_t *) name, "Menu")) {
               recursiveMenuDepth++;
               numOfMenu[currentNumberOfTabs]++;
               if (!ReadXMLMenu(currentNumberOfTabs))
                  return kFALSE;
               recursiveMenuDepth--;
            }
         }
      }

   }

   return kTRUE;
}

Bool_t ArgusBuilder::WriteTabCpp()
{
   ROMEString cppFile;
   ROMEString header;
   ROMEString buffer;
   Int_t lenTot, ll, i;
   Char_t *pos;
   ROMEString format;
   ROMEString discript;
   ROMEString str;

   if (makeOutput)
      cout << "\n   Output Cpp-Files:" << endl;
   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      header.Resize(0);
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%s/src/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());

      // Description
      header.AppendFormatted("//// Author: %s\n", tabAuthor[iTab].Data());
      header.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      header.AppendFormatted("//                                                                            //\n");
      ll = 73 - shortCut.Length();
      format.SetFormatted("// %%sT%%-%d.%ds //\n", ll, ll);
      header.AppendFormatted((Char_t *) format.Data(), shortCut.Data(), tabName[iTab].Data());
      header.AppendFormatted("//                                                                            //\n");
      discript = tabDescription[iTab].Data();
      pos = (Char_t *) discript.Data();
      lenTot = discript.Length();
      while (pos - discript.Data() < lenTot) {
         if (lenTot + (discript.Data() - pos) < 74)
            i = lenTot + (discript.Data() - pos);
         else
            for (i = 74; pos[i] != 32 && i > 0; i--) {
            }
         if (i <= 0)
            i = TMath::Min(75, lenTot);
         pos[i] = 0;
         header.AppendFormatted("// %-74.74s   \n", pos);
         pos = pos + i + 1;
      }
      header.AppendFormatted("// \n");
      header.AppendFormatted("// \n");
      header.AppendFormatted("// This header has been generated by the ArgusBuilder.\n");
      header.AppendFormatted("//                                                                            //\n");

      // Thread
      if (numOfThreadFunctions[iTab] > 0) {
         header.AppendFormatted("// This tab has following thread functions.                                   //\n");
         for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
            format.SetFormatted("//    %%s%%%ds//\n", 72 - threadFunctionName[iTab][i].Length());
            header.AppendFormatted((Char_t *) format.Data(), threadFunctionName[iTab][i].Data(), "");
         }
         header.AppendFormatted("//                                                                            //\n");
      }
      header.AppendFormatted("//                                                                            //\n");
      header.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");
      buffer.Resize(0);

      // Header
      buffer.AppendFormatted("\n\n#include \"include/tabs/%sT%s.h\"\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n", shortCut.Data());
      buffer.AppendFormatted("\nClassImp(%sT%s)\n\n", shortCut.Data(), tabName[iTab].Data());

      // Functions
      buffer.AppendFormatted("void %sT%s::Init()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::MenuClicked(TGPopupMenu *menu,Long_t param)\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::TabSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");

      buffer.AppendFormatted("void %sT%s::TabUnSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("void %sT%s::%s()\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   gSystem->Sleep(10000);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }

      // Write file
      ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6);
   }
   return kTRUE;
}

Bool_t ArgusBuilder::WriteTabH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   Int_t i, j;
   if (makeOutput)
      cout << "\n   Output H-Files:" << endl;

   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      // File name
      hFile.SetFormatted("%s/include/tabs/%sT%s_Base.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");
      // Header
      buffer.AppendFormatted("#ifndef %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("#define %sT%s_Base_H\n\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("#include <TGFrame.h>\n");
      buffer.AppendFormatted("#include <TGMenu.h>\n");
      buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n", shortCut.Data());
      buffer.AppendFormatted("#include <TThread.h>\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("struct %sArgs{\n", tabName[iTab].Data());
      buffer.AppendFormatted("   void*  inst;\n");
      buffer.AppendFormatted("   Long_t msg;\n");
      buffer.AppendFormatted("   Long_t param1;\n");
      buffer.AppendFormatted("   Long_t param2;\n");
      buffer.AppendFormatted("};\n");

      // Class
      buffer.AppendFormatted("\nclass %sT%s_Base : public TGCompositeFrame\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("protected:\n");

      // Fields
      if (numOfSteering[iTab] > 0) {
         WriteSteeringClass(buffer, 0, iTab, 1);
         buffer.AppendFormatted("\n");
      }
      if (numOfSteering[iTab] > 0) {
         buffer.AppendFormatted("   Steering* fSteering; //! Handle to Steering class\n\n");
      }
      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   TThread* m%s; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Bool_t   f%sActive; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sNumberOfLoops; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sInterval; //!\n", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("   %s f%sArgument_%d; //!\n", threadFunctionArgument[iTab][i][j].Data(), threadFunctionName[iTab][i].Data(), j);
      }
      buffer.AppendFormatted("   Int_t    fVersion; //! Version number\n");
      buffer.AppendFormatted("   Bool_t   fActive; //! is Active\n");

      // Methods
      buffer.AppendFormatted("public:\n");

      // Constructor
      buffer.AppendFormatted("   // Constructor\n");
      buffer.AppendFormatted("   %sT%s_Base():TGCompositeFrame(){\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("      fVersion = %s;\n", tabVersion[iTab].Data());
      buffer.AppendFormatted("      fActive  = kFALSE;\n");
      if (numOfSteering[iTab] > 0) {
         buffer.AppendFormatted("      fSteering = new Steering();\n");
      }
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("      m%s       = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   ~%sT%s_Base(){\n", shortCut.Data(), tabName[iTab].Data());
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");

      // Thread
      buffer.AppendFormatted("   virtual Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2){\n");
      buffer.AppendFormatted("      return RunProcessMessageThread(msg, param1, param2);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   virtual Bool_t ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2){return kTRUE;}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   static void ThreadProcessMessageThread(void* arg){\n");
      buffer.AppendFormatted("      ((%sT%s_Base*)((%sArgs*)arg)->inst)->ProcessMessageThread(((%sArgs*)arg)->msg, ((%sArgs*)arg)->param1, ((%sArgs*)arg)->param2);\n", shortCut.Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   Bool_t RunProcessMessageThread(Long_t msg, Long_t param1, Long_t param2){\n");
      buffer.AppendFormatted("      %sArgs* arg = new %sArgs();\n", tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("      arg->inst   = this;\n");
      buffer.AppendFormatted("      arg->msg    = msg;\n");
      buffer.AppendFormatted("      arg->param1 = param1;\n");
      buffer.AppendFormatted("      arg->param2 = param2;\n");
      buffer.AppendFormatted("      TThread* mProcessMessageThread = new TThread(\"processMessageThread\",(void(*) (void *))&ThreadProcessMessageThread,(void*) arg);\n");
      buffer.AppendFormatted("      mProcessMessageThread->Run();\n");
      buffer.AppendFormatted("      return kTRUE;\n");
      buffer.AppendFormatted("   }\n");
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   //%s\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   virtual void %s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("%s", threadFunctionArgument[iTab][i][j].Data());
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted(")\n");
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("      cout<<endl\n");
         buffer.AppendFormatted("          <<\" Thread function %s is not implemented.\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
         buffer.AppendFormatted("          <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %s();\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %sT%s::%s()\"<<endl\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   {\"<<endl\n");
         buffer.AppendFormatted("          <<\"      cout<<\\\"Thread function %s is running.\\\"<<endl;\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   }\"<<endl<<endl;\n");
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   static void Thread%s(void* arg){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      TThread::SetCancelOn();\n");
         buffer.AppendFormatted("      TThread::SetCancelDeferred();\n");
         buffer.AppendFormatted("      %sT%s_Base* inst = (%sT%s_Base*) arg;\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("      Int_t iLoop = 0;\n");
//         buffer.AppendFormatted("      Int_t meid=TThread::SelfId(); // get pthread id\n");
         buffer.AppendFormatted("      while(inst->f%sActive){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::CancelPoint();\n");
         buffer.AppendFormatted("         inst->%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("inst->f%sArgument_%d", threadFunctionName[iTab][i].Data(), j);
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted("); // call the user defined threaded function\n");
         buffer.AppendFormatted("         if(inst->f%sNumberOfLoops != 0 && ++iLoop>=inst->f%sNumberOfLoops) {\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            inst->f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            if(inst->m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("               TThread::Delete(inst->m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("               inst->m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         gSystem->Sleep(inst->f%sInterval);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Start%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("%s arg%d,", threadFunctionArgument[iTab][i][j].Data(), j);
         buffer.AppendFormatted("Int_t interval = 1000, Int_t nloop = 0){\n");

         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("      f%sArgument_%d = arg%d;\n", threadFunctionName[iTab][i].Data(), j, j);
         buffer.AppendFormatted("      f%sActive        = kTRUE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sNumberOfLoops = nloop;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sInterval      = interval;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      if(!m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = new TThread(\"Thread%s\",(void(*) (void *))&Thread%s,(void*) this);\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s->Run();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Stop%s(){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      gSystem->Sleep(1000); // wait a while for threads to halt\n");
         buffer.AppendFormatted("      if(m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::Delete(m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   } \n");
         buffer.AppendFormatted("   Bool_t Kill%s(){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      if(m%s){\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s->Kill();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   } \n");

         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("\n");

      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTab] > 0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; };\n");
      }
      buffer.AppendFormatted("   Bool_t       GetActive() { return fActive; };\n");
      buffer.AppendFormatted("   void         SetActive(Bool_t active) { fActive = active; };\n");
      buffer.AppendFormatted("   virtual void TabSelected(){};\n");
      buffer.AppendFormatted("   virtual void TabUnSelected(){};\n");
      buffer.AppendFormatted("   virtual void MenuClicked(TGPopupMenu *menu,Long_t param)\n");
      buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("      cout<<endl\n");
      buffer.AppendFormatted("          <<\" Function MenuClicked is not implemented in %s.\"<<endl\n", tabName[iTab].Data());
      buffer.AppendFormatted("          <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
      buffer.AppendFormatted("          <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   void MenuClicked(TGPopupMenu *menu,Long_t param);\"<<endl\n");
      buffer.AppendFormatted("          <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   void %sT%s::MenuClicked(TGPopupMenu *menu,Long_t param)\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("          <<\"   {\"<<endl\n");
      buffer.AppendFormatted("          <<\"      cout<<\\\"param = \\\"<< param <<endl;\"<<endl\n");
      buffer.AppendFormatted("          <<\"   }\"<<endl<<endl;\n");
      buffer.AppendFormatted("   }\n");

      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s)\n", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data());
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());

      // Write File
      WriteFile(hFile.Data(), buffer.Data(), 6);

      // User H-File
      hFile.SetFormatted("%s/include/tabs/%sT%s.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      struct stat buf;
      if (stat(hFile.Data(), &buf)) {
         // Description
         buffer.Resize(0);
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// Editable class file for %s tab.\n", tabName[iTab].Data());
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

         // Header
         buffer.AppendFormatted("#ifndef %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("#include <include/tabs/%sT%s_Base.h>\n", shortCut.Data(), tabName[iTab].Data());

         // Class
         buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("protected:\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("public:\n");

         // Constructor
         buffer.AppendFormatted("   %sT%s():%sT%s_Base()\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   ~%sT%s()\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   void Init();\n");
         buffer.AppendFormatted("   void MenuClicked(TGPopupMenu *menu,Long_t param);\n");
         buffer.AppendFormatted("   void TabSelected();\n");
         buffer.AppendFormatted("   void TabUnSelected();\n");

         // Thread
         for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
            buffer.AppendFormatted("   void %s();\n", threadFunctionName[iTab][i].Data());
         }
         buffer.AppendFormatted("\n");

         // Fields
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());

         // Write File
         WriteFile(hFile.Data(), buffer.Data(), 6);
      }
   }

   return kTRUE;
}

Bool_t ArgusBuilder::WriteTabConfigWrite(ROMEString & buffer, Int_t parentIndex, ROMEString & pointer, Int_t tab)
{
   Int_t i;

   // max tab switch name length
   Int_t switchLen = -1;
   ROMEString switchString;
   for (i = 0; i < numOfTab; i++) {
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (switchLen < (Int_t) switchString.Length())
         switchLen = switchString.Length();
   }

   ROMEString blank = "";
   for (i = 0; i < tab; i++)
      blank.Append("   ");

   ROMEString pointerI;
   for (i = 0; i < numOfTab; i++) {
      if (tabParentIndex[i] != parentIndex)
         continue;
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      pointerI = pointer;
      pointerI.AppendFormatted("->f%sTab", tabName[i].Data());
      buffer.AppendFormatted("%s      if (fConfigData[index]%sModified || index==0) {\n", blank.Data(), pointerI.Data());
      buffer.AppendFormatted("%s         // %s\n", blank.Data(), pointerI.Data());
      buffer.AppendFormatted("%s         xml->StartElement(\"Tab\");\n", blank.Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"TabName\",\"%s\");\n", blank.Data(), tabName[i].Data());
      buffer.AppendFormatted("%s         if (index==0) {\n", blank.Data());
      buffer.AppendFormatted("%s            if (gWindow->GetTabSwitches()->%s)\n", blank.Data(), switchString.Data());
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"true\");\n", blank.Data());
      buffer.AppendFormatted("%s            else\n", blank.Data());
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"false\");\n", blank.Data());
      buffer.AppendFormatted("%s         }\n", blank.Data());
      buffer.AppendFormatted("%s         else if (fConfigData[index]%s->fActiveModified)\n", blank.Data(), pointerI.Data());
      buffer.AppendFormatted("%s            xml->WriteElement(\"Active\",(Char_t*)fConfigData[index]%s->fActive.Data());\n", blank.Data(), pointerI.Data());

      // Steering parameter
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("%s         // steering parameters\n", blank.Data());
         buffer.AppendFormatted("%s         if (fConfigData[index]%s->fSteeringModified || index==0) {\n", blank.Data(), pointerI.Data());
         buffer.AppendFormatted("%s            ROMEString value;\n", blank.Data());
         ROMEString pointerT;
         ROMEString steerPointerT;
         Int_t indexCounter = 0;
         pointerT.SetFormatted("fConfigData[index]%s->fSteering", pointerI.Data());
         steerPointerT.SetFormatted("((%sT%s*)gWindow->Get%s%03dTab())->GetSP()", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), i);
         WriteSteeringConfigWrite(buffer, 0, i, pointerT, steerPointerT, 3 + tab, &indexCounter);
      }
      if (numOfSteering[i] > 0)
         buffer.AppendFormatted("%s         }\n", blank.Data());
      WriteTabConfigWrite(buffer, i, pointerI, tab + 1);
      buffer.AppendFormatted("%s         xml->EndElement();\n", blank.Data());
      buffer.AppendFormatted("%s      }\n", blank.Data());
   }

   return kTRUE;
}

Bool_t ArgusBuilder::WriteTabConfigClass(ROMEString & buffer, Int_t parentIndex, Int_t tab)
{
   Int_t j, i;

   ROMEString blank = "";
   for (i = 0; i < tab; i++)
      blank.Append("   ");

   for (i = 0; i < numOfTab; i++) {
      if (tabParentIndex[i] != parentIndex)
         continue;
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("%s      class %sTab {\n", blank.Data(), tabName[i].Data());
      buffer.AppendFormatted("%s      public:\n", blank.Data());
      buffer.AppendFormatted("%s         ROMEString       fActive;\n", blank.Data());
      buffer.AppendFormatted("%s         Bool_t           fActiveModified;\n", blank.Data());
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("%s         // steering parameters\n", blank.Data());
         buffer.AppendFormatted("%s         class Steering {\n", blank.Data());
         buffer.AppendFormatted("%s         public:\n", blank.Data());
         WriteSteeringConfigClass(buffer, 0, i, tab + 3);
         buffer.AppendFormatted("%s         };\n", blank.Data());
         buffer.AppendFormatted("%s         Steering*        fSteering;\n", blank.Data());
         buffer.AppendFormatted("%s         Bool_t           fSteeringModified;\n", blank.Data());
      }
      // Constructor
      buffer.AppendFormatted("%s      public:\n", blank.Data());
      buffer.AppendFormatted("%s         %sTab() {\n", blank.Data(), tabName[i].Data());
      if (numOfSteering[i] > 0) {
         buffer.AppendFormatted("%s         fSteeringModified = kFALSE;\n", blank.Data());
         buffer.AppendFormatted("%s         fSteering         = new Steering();\n", blank.Data());
      }
      for (j = 0; j < numOfTab; j++) {
         if (tabParentIndex[j] != i)
            continue;
         buffer.AppendFormatted("%s            f%sTab = new %sTab();\n", blank.Data(), tabName[j].Data(), tabName[j].Data());
      }
      buffer.AppendFormatted("%s         };\n", blank.Data());

      // Sub classes
      WriteTabConfigClass(buffer, i, tab + 1);
      buffer.AppendFormatted("%s      };\n", blank.Data());
      buffer.AppendFormatted("%s      %sTab*          f%sTab;\n", blank.Data(), tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("%s      Bool_t           f%sTabModified;\n", blank.Data(), tabName[i].Data());
   }
   return kTRUE;
}
