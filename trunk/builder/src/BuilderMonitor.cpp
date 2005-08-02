/********************************************************************

  BuilderMonitor.cpp, Ryu Sawada

  $Log$
  Revision 1.31  2005/08/02 12:39:04  sawada
  Removed netfolder title config.
  Added netfolder reconnect config.
  Check if the root folder is found when netfolder connects server.

  Revision 1.30  2005/07/03 17:31:34  sawada
  Support folder.
  Multiple dimension fields in folders.

  Revision 1.29  2005/05/26 14:26:54  sawada
  Lots of changes.
  Made ArgusBuilder an inheriting class of ROMEBuilder.
  Remove ROMEFolder and added NetFolers.
  Added ArgusWindow class.
  and so on.

  Revision 1.28  2005/05/14 21:42:22  sawada
  Separated file writing function in builder.

  Revision 1.27  2005/05/08 00:28:54  sawada
  fixed mismathes of [Set,Append]Formatted in builder.
  added readme of examples.

  Revision 1.26  2005/05/05 21:26:03  sawada
  code clean up.

  Revision 1.25  2005/05/05 20:08:04  sawada
  code clean up.

  Revision 1.24  2005/04/25 17:06:16  schneebeli_m
  GetMenuHandle() implemented

  Revision 1.23  2005/04/22 15:29:08  schneebeli_m
  added menu id enumeration

  Revision 1.22  2005/04/12 17:50:58  sawada
  changes for mxml

  Revision 1.21  2005/03/28 10:54:37  sawada
  removed tab hierarchy.
  made ReadXMLMenu.

  Revision 1.20  2005/03/13 08:40:43  sawada
  modified handling of recursiveDepth.
  removed unused variables.

  Revision 1.19  2005/03/12 01:21:00  sawada
  Nested tab.

  Revision 1.18  2005/03/01 00:24:34  sawada
  bug fix around global steering parameter.

  Revision 1.17  2005/02/27 23:53:43  sawada
  Create placeholder of ROMEFolder at start.
  Environment variable in ROMEProjectPath.
  Bug fix of arrayed ROMEFolder.
  Bug fix of SetActive of tabs from monitor.
  Create menues of the first tab at start.

  Revision 1.16  2005/02/25 17:10:44  sawada
  small bug fix.

  Revision 1.15  2005/02/25 16:14:00  sawada
  bug fix and example for ROMEFolder

  Revision 1.14  2005/02/25 13:51:02  sawada
  added folderShortCut and ROMEProjectPath

  Revision 1.13  2005/02/24 15:04:03  sawada
  Reduced number of configuration to 1.
  Replaced ss_getchar to getchar().
  Removed LineToProcess.
  Removed bk_is32,bk_find.
  Improved help.
  Handling of midas host and experiment.

  Revision 1.12  2005/02/21 23:07:50  sawada
  several UNIX support

  Revision 1.11  2005/02/21 19:06:56  sawada
  changed platform specifying macros

  Revision 1.10  2005/02/07 17:52:35  sawada
  modification of ROMEFolder

  Revision 1.9  2005/02/06 00:39:35  sawada
   Changed TRint to TApplication
   Stop method of thread function
   Name of method for start/stop

  Revision 1.8  2005/02/05 00:51:01  sawada
  folder name of ROMEFolders.

  Revision 1.7  2005/02/04 22:58:46  sawada
  ROMEFolder

  Revision 1.6  2005/02/04 15:10:05  sawada
  ODB and ROMEDataBase folders.

  Revision 1.5  2005/02/03 11:44:54  sawada
  IO to MIDAS ODB

  Revision 1.4  2005/02/02 23:54:30  sawada
  link with midas library.

  Revision 1.3  2005/02/02 18:58:02  sawada
  small change.

  Revision 1.2  2005/01/30 20:39:39  sawada
  * Makefile of builder
  * Tab enable/disable
  * Bug fix.(fNetFolder, ConnectServer)

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

Bool_t ArgusBuilder::WriteMonitorCpp() {
   Int_t i;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString parentt;
   ROMEString buf;
   Int_t lenTot,j,ll;
   Char_t *pos;
   ROMEString classDescription;
   classDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all folders.",shortCut.Data(),mainProgName.Data());
   ROMEString tmp;
   ROMEString format;

   Int_t nameLen = -1;
   Int_t scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         nameLen = TMath::Max(nameLen,(Int_t)(folderName[i].Length()+scl+strlen("Folder")));
      }
   }

   // File name
   cppFile.SetFormatted("%s/src/framework/%sMonitor.cpp",outDir.Data(),shortCut.Data());
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
   buffer.AppendFormatted("#include \"include/framework/%sMonitor.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/framework/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ClassImp(%sMonitor);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sMonitor *gMonitor;  // global Monitor Handle\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#include <windows.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("\n");

   // Constructor
#if defined( USE_TRINT )
   buffer.AppendFormatted("%sMonitor::%sMonitor(TRint *app):ArgusMonitor(app) {\n",shortCut.Data(),shortCut.Data());
#else
   buffer.AppendFormatted("%sMonitor::%sMonitor(TApplication *app):ArgusMonitor(app) {\n",shortCut.Data(),shortCut.Data());
#endif
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   gPassToArgus = (void*)this; // Pass the handle to the monitor\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fConfiguration = new %sConfig();\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTab]>0) {
      buffer.AppendFormatted("   fGlobalSteeringParameters = new %sGlobalSteering();\n",shortCut.Data());
   }

   // Folder
   buffer.AppendFormatted("   // Folder initialisation\n");
   buffer.AppendFormatted("   fMainFolder = gROOT->GetRootFolder()->AddFolder(\"%s\",\"Root Folder of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   gROOT->GetListOfBrowsables()->Add(fMainFolder,\"%s\");\n\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (folderParentName[i]=="GetMainFolder()") parentt = folderParentName[i];
         else parentt.SetFormatted("%sFolder",folderParentName[i].Data());
         format.SetFormatted("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-folderName[i].Length()-scl-strlen("Folder"));
         buffer.AppendFormatted((Char_t*)format.Data(),folderName[i].Data(),"",parentt.Data(),folderName[i].Data(),folderTitle[i].Data());
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0 ) {
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("\n   f%sFolder = new %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolder);\n",folderName[i].Data(),folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("\n   f%sFolders = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolders);\n",folderName[i].Data(),folderName[i].Data());
            }
         }
      }
   }

   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   InitNetFolders(%d);\n",numOfNetFolder);

   // NetFolder
   buffer.AppendFormatted("   fNumberOfNetFolders = %d;\n",numOfNetFolder);
   for (i=0;i<numOfNetFolder;i++){
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   // %s\n",netFolderName[i].Data());
      buffer.AppendFormatted("   fNetFolderName[%d]   = \"%s\";\n",i,netFolderName[i].Data());
      buffer.AppendFormatted("   fNetFolderActive[%d] = kFALSE;\n",i);
      buffer.AppendFormatted("   fNetFolderReconnect[%d] = kTRUE;\n",i);
      buffer.AppendFormatted("   fNetFolder[%d]       = 0;\n",i);
      buffer.AppendFormatted("   fNetFolderSocket[%d] = 0;\n",i);
      if(netFolderHost[i].Length())
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"%s\";\n",i,netFolderHost[i].Data());
      else
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"localhost\";\n",i);
      if(netFolderPort[i].Length())
         buffer.AppendFormatted("   fNetFolderPort[%d]   = %s;\n",i,netFolderPort[i].Data());
      else
         buffer.AppendFormatted("   fNetFolderPort[%d]   = 9090;\n",i);
      if(netFolderRoot[i].Length())
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n",i,netFolderRoot[i].Data());
      else
//         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"histos\";\n",i);
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n",i,shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // Database Folder Fields
   buffer.AppendFormatted("   // Database Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("   f%s_%sDBName = \"\";\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   f%s_%sDBPath = \"\";\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   f%s_%sDBCode = new TArrayI(%d);\n",folderName[i].Data(),valueName[i][j].Data(),maxNumberOfPathObjectInterpreterCodes);
            buffer.AppendFormatted("   for (i=0;i<%d;i++)\n",maxNumberOfPathObjectInterpreterCodes);
            buffer.AppendFormatted("      f%s_%sDBCode->AddAt(-1,i);\n",folderName[i].Data(),valueName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("}\n\n");

   Int_t ndb = 0;
   for (i=0;i<numOfFolder;i++) if (folderDataBase[i]) ndb++;

   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("Bool_t %sMonitor::ReadSingleDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      for (i=0;i<numOfFolder;i++){
         if (folderDataBase[i] && folderArray[i]=="1" && !folderSupport[i]){
            buffer.AppendFormatted("   if(!Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return kFALSE;\n");
         }
      }
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");
   // ReadArrayDataBaseFolders
   buffer.AppendFormatted("\nBool_t %sMonitor::ReadArrayDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i] && folderArray[i]!="1" && !folderSupport[i])
            buffer.AppendFormatted("   if(!Read%s()) return kFALSE;\n",folderName[i].Data());
      }
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // clean up folders
   buffer.AppendFormatted("\n// Delete Unused Folders\n");
   buffer.AppendFormatted("void %sMonitor::CleanUpFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i] && folderArray[i]=="variable") {
         buffer.AppendFormatted("   Int_t i;\n");
         break;
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i] && !folderSupport[i]) {
         if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=gMonitor->Get%ss()->GetEntriesFast()-1;i>=0;i--) {\n",folderName[i].Data());
            buffer.AppendFormatted("      if (((%s%s*)gMonitor->Get%sAt(i))->isModified())\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("         break;\n");
            buffer.AppendFormatted("      gMonitor->Get%ss()->RemoveAt(i);\n",folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // reset folders
   buffer.AppendFormatted("// Reset Folders\n");
   buffer.AppendFormatted("\nvoid %sMonitor::ResetFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i] && folderArray[i]!="1" && !folderSupport[i]) {
         buffer.AppendFormatted("   Int_t i;\n");
         break;
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   gMonitor->Get%s()->Reset();\n",folderName[i].Data());
         }
         else if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=0;i<gMonitor->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gMonitor->Get%sAt(i))->Reset();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<gMonitor->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gMonitor->Get%sAt(i))->SetModified(kFALSE);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   //Update Folders
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]){
         buffer.AppendFormatted("Bool_t %sMonitor::Read%s() {\n",shortCut.Data(),folderName[i].Data());
         WriteReadDataBaseFolder(buffer,i,folderArray[i]=="1" ? 1 : 2);
         buffer.AppendFormatted("   return kTRUE;\n");
         buffer.AppendFormatted("}\n\n");
      }
   }

/*
      if (!folderDataBase[i] || numOfValue[i]==0 )
         continue;
      buffer.AppendFormatted("\nBool_t %sMonitor::Read%s() {\n",shortCut.Data(),folderName[i].Data());
      // ROMEFolder
      if(folderDefinedInROME[i]){
	 buffer.AppendFormatted("   if(!fNetFolder){\n");
	 buffer.AppendFormatted("      cout<<\"Warning: \"<<gMonitor->GetProgramName()<<\" is not connected to ROOT server.\"<<endl;\n");
	 buffer.AppendFormatted("      cout<<\"%s is not updated.\"<<endl;\n",folderName[i].Data());
	 buffer.AppendFormatted("      return kTRUE;\n");
	 buffer.AppendFormatted("   }\n");
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   if(f%sFolder)\n",folderName[i].Data());
            buffer.AppendFormatted("      delete f%sFolder;\n",folderName[i].Data());
            buffer.AppendFormatted("      f%sFolder = (%s%s*)((TFolder*) fNetFolder->FindObjectAny(\"%s\"))->FindObject(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   if (!f%sFolder){\n",folderName[i].Data());
            buffer.AppendFormatted("      cout<<\"%s not available\"<<endl;\n",folderName[i].Data());
            buffer.AppendFormatted("      return kFALSE;\n");
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   if(f%sFolders)\n",folderName[i].Data());
            buffer.AppendFormatted("      delete f%sFolders;\n",folderName[i].Data());
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)((TFolder*) fNetFolder->FindObjectAny(\"%s\"))->FindObject(\"%s%ss\");\n",folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   if (!f%sFolders){\n",folderName[i].Data());
            buffer.AppendFormatted("      cout<<\"%s not available\"<<endl;\n",folderName[i].Data());
            buffer.AppendFormatted("      return kTRUE;\n");
            buffer.AppendFormatted("   }\n");
         }
      }
      // ODB folder
      else if(folderConnectionType[i] == "ODB"){
         buffer.AppendFormatted("   INT   size;\n");
         ROMEString tid;
         for (j=0;j<numOfValue[i];j++) {
            if (valueArray[i][j]!="1") {
               buffer.AppendFormatted("   HNDLE hkey;\n");
               buffer.AppendFormatted("   Int_t i;\n");
               break;
            }
         }
         for (j=0;j<numOfValue[i];j++) {
            GetMidasTID(&tid, (Char_t*)valueType[i][j].Data());
            if (valueArray[i][j]=="1") {
               buffer.AppendFormatted("   %s %s_%s;\n",valueType[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   size = sizeof(%s_%s);\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   db_get_value(fMidasOnlineDataBase, 0, \"%s\", &%s_%s, &size, %s, FALSE);\n",valueDataBasePath[i][j].Data(),folderName[i].Data(),valueName[i][j].Data(),tid.Data());
               buffer.AppendFormatted("   f%sFolder->Set%s(%s_%s);\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
            }
            else {
               buffer.AppendFormatted("   %s %s_%s[%s];\n",valueType[i][j].Data(),folderName[i].Data(),valueName[i][j].Data(),valueArray[i][j].Data());
               buffer.AppendFormatted("   size = sizeof(%s_%s);\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   db_find_key(fMidasOnlineDataBase, 0, %s, &hkey);\n",valueDataBasePath[i][j].Data());
               buffer.AppendFormatted("   db_get_data(fMidasOnlineDataBase, hkey, %s_%s, &size, %s);\n",folderName[i].Data(),valueName[i][j].Data(),tid.Data());
               buffer.AppendFormatted("   for (i=0;i<%s;i++) {\n",valueArray[i][j].Data());
               buffer.AppendFormatted("      f%sFolder->Set%sAt(i,(%s)%s_%s[i]);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      else if(folderConnectionType[i] == "ROMEDataBase"){
         // ROMEDataBase Folder
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
         if(folderArray[i]!="1")
         buffer.AppendFormatted("   Int_t i;\n");
         for (j=0;j<numOfValue[i];j++) {
         if (valueArray[i][j]!="1") {
               buffer.AppendFormatted("   Int_t j;\n");
               break;
            }
         }
         for (j=0;j<numOfValue[i];j++) {
            if (isNumber(valueType[i][j].Data())){
               buffer.AppendFormatted("   Char_t *cstop;\n");
               break;
            }
         }
         if (folderArray[i] == "1") {
            for (j=0;j<numOfValue[i];j++) {
               if (folderArray[i]=="1") {
                  buffer.AppendFormatted("   values->RemoveAll();\n");
                  buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path,fRunNumber))\n");
                  buffer.AppendFormatted("      return kFALSE;\n");
                  if (valueArray[i][j]=="1") {
                     buffer.AppendFormatted("   if (values->At(0,0).Length()!=0)\n");
                     setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(0,0).Data()",(Char_t*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("      f%sFolder->Set%s((%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("   else\n");
                     buffer.AppendFormatted("      f%sFolder->Set%s(%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  }
                  else {
                     buffer.AppendFormatted("   for (j=0;j<%s;j++) {\n",valueArray[i][j].Data());
                     buffer.AppendFormatted("      if (values->At(0,j).Length()!=0)\n");
                     setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(0,j).Data()",(Char_t*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(j,(%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("      else\n");
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(j,%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                     buffer.AppendFormatted("   }\n");
                  }
               }
            }
         }
         else{
            for (j=0;j<numOfValue[i];j++) {
               buffer.AppendFormatted("   values->RemoveAll();\n");
               buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
               buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path,fRunNumber))\n");
               buffer.AppendFormatted("      return kFALSE;\n");
               buffer.AppendFormatted("   for (i=0;i<f%sFolders->GetEntries();i++) {\n",folderName[i].Data());
               if (valueArray[i][j]=="1") {
                  buffer.AppendFormatted("      if (values->At(i,0).Length()!=0)\n");
                  setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(i,0).Data()",(Char_t*)valueType[i][j].Data(),1);
                  buffer.AppendFormatted("         ((%s%s*)f%sFolders->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormatted("      else\n");
                  buffer.AppendFormatted("         ((%s%s*)f%sFolders->At(i))->Set%s(%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("      for (j=0;j<%s;j++) {\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("         if (values->At(i,j).Length()!=0)\n");
                  setValue(&buf,(Char_t*)valueName[i][j].Data(),"values->At(i,j).Data()",(Char_t*)valueType[i][j].Data(),1);
                  buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%sAt(j,(%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormatted("         else\n");
                  buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%sAt(j,%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  buffer.AppendFormatted("      }\n");
               }
	       buffer.AppendFormatted("   }\n");
            }
         }
         buffer.AppendFormatted("   values->RemoveAll();\n");
         buffer.AppendFormatted("   delete values;\n");
      }
      buffer.AppendFormatted("   return kTRUE;\n");
      buffer.AppendFormatted("};\n");
   }
*/

   buffer.AppendFormatted("\nBool_t %sMonitor::StartWindow() {\n",shortCut.Data());
   buffer.AppendFormatted("   return gWindow->Start();\n");
   buffer.AppendFormatted("}\n");

   // Initialize Single Folders
   buffer.AppendFormatted("\nvoid %sMonitor::InitSingleFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   new(gMonitor->Get%s()) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (isFolder(valueType[i][j].Data()))
                  continue;
               if (valueDimension[i][j]==0)
                  buffer.AppendFormatted("%s,",valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length()-1);
            buffer.AppendFormatted(" );\n");
         }
      }
   }
   buffer.AppendFormatted("};\n\n");

   // Initialize Array Folders
   buffer.AppendFormatted("\nvoid %sMonitor::InitArrayFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         buffer.AppendFormatted("   Int_t i;\n");
         break;
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]!="1" && folderArray[i]!="variable")
            buffer.AppendFormatted("   gMonitor->Allocate%s(%s);\n",folderName[i].Data(),folderArray[i].Data());
      }
   }
   buffer.AppendFormatted("};\n\n");

   // ReadUserParameter
   buffer.AppendFormatted("Bool_t %sMonitor::ReadUserParameter(const char* opt, const char* value, int& i) {\n",shortCut.Data());

   // Global Steering Parameter
   buffer.AppendFormatted("   ROMEString option = opt;\n");
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTab]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringReadParameters(buffer,0,numOfTab,pointerT,steerPointerT);
   }

   // Tab steering parameter
   ROMEString pointer;
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("   // %s tab\n",tabName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTab",tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (numOfSteering[i]>0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*)gMonitor->Get%s%03dTab())->GetSP()",shortCut.Data(),tabName[i].Data(),tabName[i].Data(),i);
         WriteSteeringReadParameters(buffer,0,i,pointerT,steerPointerT);
      }
   }
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n\n");

/*   // WriteDataBaseFolders
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("void %sMonitor::Write%sDataBase() {\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("   int i,j;\n");
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEString buffer[%d];\n",maxNumberOfPathObjectInterpreterCodes);
         buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
         for (j=0;j<numOfValue[i];j++) {
            buffer.AppendFormatted("   values->RemoveAll();\n");
            if (folderArray[i]=="1") {
               if (valueArray[i][j]=="1") {
                  buf = "buffer[0]";
                  str.SetFormatted("f%sFolder->Get%s()",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("   values->SetAt(%s,0,0);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
               else {
                  buf = "buffer[0]";
                  str.SetFormatted("f%sFolder->Get%sAt(j)",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("   for (j=0;j<%s;j++)\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("      values->SetAt(%s,0,j);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
            }
            else {
               buffer.AppendFormatted("   for (i=0;i<f%sFolders->GetEntries();i++)\n",folderName[i].Data());
               if (valueArray[i][j]=="1") {
                  buf = "buffer[0]";
                  str.SetFormatted("((%s%s*)f%sFolders->At(i))->Get%s()",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      values->SetAt(%s,i,0);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
               else {
                  buf = "buffer[0]";
                  str.SetFormatted("((%s%s*)f%sFolders->At(i))->Get%sAt(j)",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      for (j=0;j<%s;j++)\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("         values->SetAt(%s,i,j);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
            }
            buffer.AppendFormatted("   path.SetFormatted(gMonitor->Get%s_%sDBPath()",folderName[i].Data(),valueName[i][j].Data());
            for (k=0;k<maxNumberOfPathObjectInterpreterCodes;k++)
               buffer.AppendFormatted(",gMonitor->GetObjectInterpreterCharValue(gMonitor->Get%s_%sDBCodeAt(%d),buffer[%d],buffer[%d]).Data()",folderName[i].Data(),valueName[i][j].Data(),k,k,k);
            buffer.AppendFormatted(");\n");
            buffer.AppendFormatted("   if (!this->GetDataBase(gMonitor->Get%s_%sDBIndex())->Write(values,path,gMonitor->GetCurrentRunNumber())) {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      gMonitor->Println\"   in Folder '%s' Value '%s'.\");\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      delete values;\n");
            buffer.AppendFormatted("      return;\n");
            buffer.AppendFormatted("   }\n");
         }
         buffer.AppendFormatted("   values->RemoveAll();\n");
         buffer.AppendFormatted("   delete values;\n");
         buffer.AppendFormatted("}\n");
      }
   }
*/

   // Get Object Interpreter Code
   int codeNumber = 0;
   buffer.AppendFormatted("int %sMonitor::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString path = objectPath;\n");
   buffer.AppendFormatted("   if (path.Index(\"/\")!=0)\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   if (path.Index(\"/GSP\")==0) {\n");
   ROMEString path = "/GSP";
   codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber,0,numOfTab,path,1);
   buffer.AppendFormatted("      cout<<\"\\nWrong path to a steering parameter in configuration file.\\n\"<<endl;\n");
   buffer.AppendFormatted("      cout<<\"   \"<<path<<endl;\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   }\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderArray[i]=="1" && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]==0) {
               buffer.AppendFormatted("   if (path==\"/%s/%s\")\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("      return %d;\n",codeNumber);
               codeNumber++;
            }
         }
      }
   }
   buffer.AppendFormatted("   cout<<\"\\nWrong path in configuration file.\\n\"<<endl;\n");
   buffer.AppendFormatted("   cout<<\"   \"<<path<<endl;\n");
   buffer.AppendFormatted("   return -1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   fNumberOfInterpreterCodes = codeNumber;

   // Get Object Interpreter Values
   WriteObjectInterpreterValue(buffer,"int","Int");
   WriteObjectInterpreterValue(buffer,"double","Double");
   WriteObjectInterpreterValue(buffer,"ROMEString&","Char");

   // UserParameterUsage
   buffer.AppendFormatted("void %sMonitor::UserParameterUsage() {\n",shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTab]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringParameterUsage(buffer,0,numOfTab,pointerT,steerPointerT);
   }

   // Tab steering parameter
   for (i=0;i<numOfTab;i++) {
      buffer.AppendFormatted("   // %s tab\n",tabName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTab",tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (numOfSteering[i]>0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*)gMonitor->Get%s%03dTab())->GetSP()",shortCut.Data(),tabName[i].Data(),tabName[i].Data(),i);
         WriteSteeringParameterUsage(buffer,0,i,pointerT,steerPointerT);
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Close cpp-File
   WriteFile(cppFile.Data(),buffer.Data(),6);
   return kTRUE;
}


Bool_t ArgusBuilder::WriteMonitorH() {
   Int_t i,j;
   ROMEString hFile;
   ROMEString buffer;
   ROMEString bankname;
   ROMEString format;

   // max folder name length
   Int_t nameLen = -1;
   Int_t fieldLen = -1;
   Int_t typeLen = -1;
   Int_t scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         typeLen = TMath::Max(typeLen,(Int_t)(folderName[i].Length()+scl+strlen("*")));
         typeLen = TMath::Max(typeLen,(Int_t)(folderName[i].Length()+scl+strlen("**")));
         nameLen = TMath::Max(nameLen,(Int_t)(folderName[i].Length()));
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++)
               fieldLen = TMath::Max(fieldLen,valueName[i][j].Length());
         }
      }
   }
   typeLen = TMath::Max(typeLen,(Int_t)strlen("TClonesArray*"));
   typeLen = TMath::Max(typeLen,(Int_t)strlen("TClonesArray**"));

   // File name
   hFile.SetFormatted("%s/include/framework/%sMonitor.h",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ArgusBuilder.                          //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sMonitor_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sMonitor_H\n\n",shortCut.Data());
   buffer.AppendFormatted("#include <ROME.h>\n");
   buffer.AppendFormatted("#include <ArgusMonitor.h>\n");
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#include <TNetFolder.h>\n");
   buffer.AppendFormatted("#include <TArrayI.h>\n",shortCut.Data());
   // include
   if (numOfSteering[numOfTab]>0) {
      buffer.AppendFormatted("#include \"include/framework/%sGlobalSteering.h\"\n",shortCut.Data());
   }
   // Folder includes
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && !folderSupport[i])
         buffer.AppendFormatted("#include \"include/framework/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("\nclass %sMonitor : public ArgusMonitor\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Fields
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("\n");

   // Folder Fields
   buffer.AppendFormatted("   // Folder fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0 ) {
            if (folderArray[i]=="1") {
               format.SetFormatted("   %%s%%s*%%%ds f%%sFolder; %%%ds // Handle to %%s%%s Folder\n",typeLen-folderName[i].Length()-scl-strlen("*"),nameLen-folderName[i].Length());
               buffer.AppendFormatted((Char_t*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
            }
            else {
               format.SetFormatted("   TClonesArray*%%%ds f%%sFolders;%%%ds // Handle to %%s%%s Folders\n",typeLen-strlen("TClonesArray*"),nameLen-folderName[i].Length());
               buffer.AppendFormatted((Char_t*)format.Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Database Folder Fields
   buffer.AppendFormatted("   // Database Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            format.SetFormatted("   ROMEString f%%s_%%sDBName; %%%ds // Name of the database from which to read the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
            format.SetFormatted("   ROMEString f%%s_%%sDBPath; %%%ds // Database path to the value of the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
            format.SetFormatted("   TArrayI   *f%%s_%%sDBCode; %%%ds // Object Interpreter codes for database path to the value of the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Steering Fields
   if (numOfSteering[numOfTab]>0) {
      buffer.AppendFormatted("   // Steering Parameter Fields\n");
      buffer.AppendFormatted("\n   %sGlobalSteering* fGlobalSteeringParameters; // Handle to the GlobalSteering Class\n",shortCut.Data());
   }

   // Methods
   buffer.AppendFormatted("public:\n");

   // Constructor
#if defined( USE_TRINT )
   buffer.AppendFormatted("   %sMonitor(TRint *app);\n",shortCut.Data());
#else
   buffer.AppendFormatted("   %sMonitor(TApplication *app);\n",shortCut.Data());
#endif

   // Folder Getters
   buffer.AppendFormatted("   // Folders\n");
   for(i=0;i<numOfFolder;i++)
      WriteFolderGetter(buffer,i,scl,nameLen,typeLen);
   buffer.AppendFormatted("\n");

   // Allocate
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i]=="variable"){
         buffer.AppendFormatted("   void Allocate%s(int number) {\n",folderName[i].Data());
         buffer.AppendFormatted("      int i;\n");
         buffer.AppendFormatted("      if(f%sFolders) f%sFolders->Delete();\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("      for (i=0;i<number;i++) {\n");
         buffer.AppendFormatted("         new((*f%sFolders)[i]) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if (isFolder(valueType[i][j].Data()))
               continue;
            if (valueDimension[i][j]==0)
               buffer.AppendFormatted("%s,",valueInit[i][j].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" );\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
      }
   }
   buffer.AppendFormatted("\n");

   // Object Interpreter
   buffer.AppendFormatted("   // Object Interpreter\n");
   buffer.AppendFormatted("   Int_t          GetObjectInterpreterCode(const char* objectPath);\n");
   buffer.AppendFormatted("   Int_t          GetObjectInterpreterIntValue(int code,int defaultValue);\n");
   buffer.AppendFormatted("   Double_t       GetObjectInterpreterDoubleValue(int code,double defaultValue);\n");
   buffer.AppendFormatted("   ROMEString&    GetObjectInterpreterCharValue(int code,ROMEString& defaultValue,ROMEString& buffer);\n");
   buffer.AppendFormatted("\n");

   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   // Read each folders
   for (i=0;i<numOfFolder;i++){
      if (folderDataBase[i] && !folderSupport[i])
         buffer.AppendFormatted("   Bool_t         Read%s();\n",folderName[i].Data());
   }
   buffer.AppendFormatted("   Bool_t         ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   Bool_t         ReadArrayDataBaseFolders();\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfDB;i++) {
      buffer.AppendFormatted("   ROMEDataBase* Get%sDataBase() { return GetDataBase(\"%s\"); };\n",dbName[i].Data(),dbName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Database Folder Field Getters
   buffer.AppendFormatted("   // Database Folder Field Getters\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            format.SetFormatted("   const char* Get%%s_%%sDBName()%%%ds          { return f%%s_%%sDBName; };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   const char* Get%%s_%%sDBPath()%%%ds          { return f%%s_%%sDBPath; };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   int         Get%%s_%%sDBNumberOfCode()%%%ds  { return f%%s_%%sDBCode->GetSize(); };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   int         Get%%s_%%sDBCodeAt(int i)%%%ds   { return f%%s_%%sDBCode->At(i); };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
         }
      }
   }

   // Database Folder Field Setters
   buffer.AppendFormatted("   // Database Folder Field Setters\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            format.SetFormatted("   void Set%%s_%%sDBName(const char* name)%%%ds  { f%%s_%%sDBName = name; };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBPath(const char* path)%%%ds  { f%%s_%%sDBPath = path; };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBNumberOfCode(int num)%%%ds   { f%%s_%%sDBCode->Set(num); };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBCodeAt(int i,int code)%%%ds  { f%%s_%%sDBCode->AddAt(code,i); };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTab]>0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   Bool_t         StartWindow();\n");
   buffer.AppendFormatted("   void           InitSingleFolders();\n");
   buffer.AppendFormatted("   void           InitArrayFolders();\n");
   buffer.AppendFormatted("   void           ResetFolders();\n");
   buffer.AppendFormatted("   void           CleanUpFolders();\n");
   buffer.AppendFormatted("   Bool_t         ReadUserParameter(const char* opt, const char* value, int& i);\n");
   buffer.AppendFormatted("   void           UserParameterUsage();\n");
   // Allocate
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i]!="1" && folderArray[i]!="variable"){
         buffer.AppendFormatted("   void Allocate%s(int number) {\n",folderName[i].Data());
         buffer.AppendFormatted("      int i;\n");
         buffer.AppendFormatted("      if(f%sFolders) f%sFolders->Delete();\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("      for (i=0;i<number;i++) {\n");
         buffer.AppendFormatted("         new((*f%sFolders)[i]) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if (isFolder(valueType[i][j].Data()))
               continue;
            if (valueDimension[i][j]==0)
               buffer.AppendFormatted("%s,",valueInit[i][j].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" );\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
      }
   }
   buffer.AppendFormatted("\n");
   
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sMonitor,0);\n",shortCut.Data());

   // Footer
   buffer.AppendFormatted("};\n\n");
   buffer.AppendFormatted("extern %sMonitor *gMonitor;  // global Monitor Handle\n",shortCut.Data());
   buffer.AppendFormatted("#endif   // %sMonitor_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);
   return kTRUE;
}
