/********************************************************************

  BuilderMonitor.cpp, Ryu Sawada

  $Log$
  Revision 1.2  2005/01/30 20:39:39  sawada
  * Makefile of builder
  * Tab enable/disable
  * Bug fix.(fNetFolder, ConnectServer)

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

bool ArgusBuilder::WriteMonitorCpp() {
   int i;
   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   ROMEString parentt;
   ROMEString buf;
   int nb,lenTot,j,ll;
   char *pos;
   int fileHandle;
   ROMEString classDescription;
   classDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all folders.",shortCut.Data(),mainProgName.Data());
   ROMEString tmp;
   ROMEString format;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
      if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
   }
   // File name
   cppFile.SetFormatted("%s/src/monitor/%sMonitor.cpp",outDir.Data(),shortCut.Data());
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
   buffer.AppendFormatted("#include \"include/monitor/%sMonitor.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/monitor/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/monitor/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ClassImp(%sMonitor);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sMonitor *gMonitor;  // global Monitor Handle\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#if defined( _MSC_VER )\n");
   buffer.AppendFormatted("#include <windows.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("\n");
   // Constructor
   buffer.AppendFormatted("%sMonitor::%sMonitor(TRint *app):ArgusMonitor(app) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   gPassToArgus = (void*)this; // Pass the handle to the monitor\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fConfiguration = new %sConfig();\n",shortCut.Data());
   buffer.AppendFormatted("   fNetFolderSocket = 0;\n");
   buffer.AppendFormatted("   fNetFolderHost = \"\";\n");
   buffer.AppendFormatted("\n");
   // Steering 
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("   fGlobalSteeringParameters = new %sGlobalSteering();\n",shortCut.Data());
   }
   // Folder 
   buffer.AppendFormatted("   // Folder initialisation\n");
   buffer.AppendFormatted("   fMainFolder = gROOT->GetRootFolder()->AddFolder(\"%s\",\"Root Folder of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   gROOT->GetListOfBrowsables()->Add(fMainFolder,\"%s\");\n\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (folderParentName[i]=="GetMainFolder()") parentt = folderParentName[i];
      else parentt.SetFormatted("%sFolder",folderParentName[i].Data());
      format.SetFormatted("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-folderName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),folderName[i].Data(),"",parentt.Data(),folderName[i].Data(),folderTitle[i].Data());
   }
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("\n   f%sFolder = new %sF%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   %sFolder->Add(f%sFolder);\n",folderName[i].Data(),folderName[i].Data());
         }
         else {
            buffer.AppendFormatted("\n   f%sFolders = new TClonesArray(\"%sF%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   %sFolder->Add(f%sFolders);\n",folderName[i].Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("}\n\n");
   int ndb = 0;
   for (i=0;i<numOfFolder;i++) if (folderDataBase[i]) ndb++;
   // StartMonitor
   buffer.AppendFormatted("\nbool %sMonitor::StartMonitor() {\n",shortCut.Data());
   buffer.AppendFormatted("   InitSingleFolders();\n");
   buffer.AppendFormatted("   // Update Data Base\n");
   buffer.AppendFormatted("   if (!ReadSingleDataBaseFolders()) {\n");
   buffer.AppendFormatted("      cout << \"Error while reading the data base !\" << endl;\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   InitArrayFolders();\n");
   buffer.AppendFormatted("   if (!ReadArrayDataBaseFolders()) {\n");
   buffer.AppendFormatted("      cout << \"Error while reading the data base !\" << endl;\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if(!gWindow->Start()){\n");
   buffer.AppendFormatted("      delete gWindow;\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   // NerFolder
   buffer.AppendFormatted("  // connect to server\n");
   buffer.AppendFormatted("   if(strlen(gMonitor->GetNetFolderHost())){\n");
   buffer.AppendFormatted("      gWindow->ConnectServer();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fApplication->Run();\n");
   buffer.AppendFormatted("   gArgus->ss_getchar(1);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("\nbool %sMonitor::ReadSingleDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      buffer.AppendFormatted("   ROMEString path;\n");
      buffer.AppendFormatted("   int i;\n");
      buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
      buffer.AppendFormatted("   char *cstop;\n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (folderArray[i]=="1") {
                  buffer.AppendFormatted("   values->RemoveAll();\n");
                  buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path))\n");
                  buffer.AppendFormatted("      return false;\n");
                  if (valueArray[i][j]=="1") {
                     buffer.AppendFormatted("   if (values->At(0,0).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(0,0).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("      f%sFolder->Set%s((%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("   else\n");
                     buffer.AppendFormatted("      f%sFolder->Set%s(%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  }
                  else {
                     buffer.AppendFormatted("   for (i=0;i<%s;i++) {\n",valueArray[i][j].Data());
                     buffer.AppendFormatted("      if (values->At(0,i).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(0,i).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(i,(%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("      else\n");
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(i,%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                     buffer.AppendFormatted("   }\n");
                  }
               }
            }
         }
      }
      buffer.AppendFormatted("   values->RemoveAll();\n");
      buffer.AppendFormatted("   delete values;\n");
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // ReadArrayDataBaseFolders
   buffer.AppendFormatted("\nbool %sMonitor::ReadArrayDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      buffer.AppendFormatted("   ROMEString path;\n");
      buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
      buffer.AppendFormatted("   char *cstop;\n");
      buffer.AppendFormatted("   int i,j;\n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (folderArray[i]!="1") {
                  buffer.AppendFormatted("   values->RemoveAll();\n");
                  buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path))\n");
                  buffer.AppendFormatted("      return false;\n");
                  buffer.AppendFormatted("   for (i=0;i<f%sFolders->GetEntries();i++) {\n",folderName[i].Data());
                  if (valueArray[i][j]=="1") {
                     buffer.AppendFormatted("      if (values->At(i,0).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(i,0).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("         ((%sF%s*)f%sFolders->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("      else\n");
                     buffer.AppendFormatted("         ((%s%Fs*)f%sFolders->At(i))->Set%s(%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  }
                  else {
                     buffer.AppendFormatted("      for (j=0;j<%s;j++) {\n",valueArray[i][j].Data());
                     buffer.AppendFormatted("         if (values->At(i,j).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(i,j).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("            ((%sF%s*)f%sFolders->At(i))->Set%sAt(j,(%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("         else\n");
                     buffer.AppendFormatted("            ((%sF%s*)f%sFolders->At(i))->Set%sAt(j,%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                     buffer.AppendFormatted("      }\n");
                  }
                  buffer.AppendFormatted("   }\n");
               }
            }
         }
      }
      buffer.AppendFormatted("   values->RemoveAll();\n");
      buffer.AppendFormatted("   delete values;\n");
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // Initialize ODB
   buffer.AppendFormatted("\n// InitODB\n");
   buffer.AppendFormatted("bool %sMonitor::InitODB() {\n",shortCut.Data());
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // clean up folders
   buffer.AppendFormatted("\n// Delete Unused Folders\n");
   buffer.AppendFormatted("void %sMonitor::CleanUpFolders() {\n",shortCut.Data());
   if(numOfFolder)
      buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
         if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=gMonitor->Get%ss()->GetEntriesFast()-1;i>=0;i--) {\n",folderName[i].Data());
            buffer.AppendFormatted("      if (((%sF%s*)gMonitor->Get%sAt(i))->isModified())\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
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
   if(numOfFolder)
      buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   gMonitor->Get%s()->Reset();\n",folderName[i].Data());
         }
         else if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=0;i<gMonitor->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%sF%s*)gMonitor->Get%sAt(i))->Reset();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<gMonitor->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%sF%s*)gMonitor->Get%sAt(i))->SetModified(false);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      if (bankHasHeader) {
         if (folderName[i]==bankHeaderFolder) {
            if (bankHeaderEventID!="")
               buffer.AppendFormatted("   gMonitor->Get%s()->Set%s(gMonitor->GetEventHeader()->event_id);\n",folderName[i].Data(),bankHeaderEventID.Data());
            if (bankHeaderTriggerMask!="")
               buffer.AppendFormatted("   gMonitor->Get%s()->Set%s(gMonitor->GetEventHeader()->trigger_mask);\n",folderName[i].Data(),bankHeaderTriggerMask.Data());
            if (bankHeaderSerialNumber!="")
               buffer.AppendFormatted("   gMonitor->Get%s()->Set%s(gMonitor->GetEventHeader()->serial_number);\n",folderName[i].Data(),bankHeaderSerialNumber.Data());
            if (bankHeaderTimeStamp!="")
               buffer.AppendFormatted("   gMonitor->Get%s()->Set%s(gMonitor->GetEventHeader()->time_stamp);\n",folderName[i].Data(),bankHeaderTimeStamp.Data());
         }
      }
   }
   buffer.AppendFormatted("}\n\n");
   // Initialize Single Folders
   buffer.AppendFormatted("\nvoid %sMonitor::InitSingleFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   new(gMonitor->Get%s()) %sF%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="1")
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
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]!="1" && folderArray[i]!="variable") {
            buffer.AppendFormatted("   for (i=0;i<%s;i++) {\n",folderArray[i].Data());
            buffer.AppendFormatted("     new((*gMonitor->Get%ss())[i]) %sF%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="1")
                  buffer.AppendFormatted("%s,",valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length()-1);
            buffer.AppendFormatted(" );\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");
   // Connect netfolder server
   buffer.AppendFormatted("\nbool %sMonitor::ConnectServer() {\n",shortCut.Data());   
   buffer.AppendFormatted("   char str[80];\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (fNetFolderSocket) {\n");
   buffer.AppendFormatted("      // disconnect first\n");
   buffer.AppendFormatted("      fNetFolderSocket->Close();\n");
   buffer.AppendFormatted("      delete fNetFolderSocket;\n");
   buffer.AppendFormatted("      fNetFolderSocket = 0;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fNetFolderSocket = new TSocket(fNetFolderHost.Data(), fNetFolderPortNumber);\n");
   buffer.AppendFormatted("   if (!fNetFolderSocket->IsValid()) {\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   fNetFolder = new TNetFolder(\"histos\",\"Online Histograms\",fNetFolderSocket);\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // Close cpp-File
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

bool ArgusBuilder::WriteMonitorH() {
   int i,j;
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   int nb;
   int fileHandle;
   ROMEString bankname;
   ROMEString format;
   // max folder name length
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
      if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
   }
   // File name
   hFile.SetFormatted("%s/include/monitor/%sMonitor.h",outDir.Data(),shortCut.Data());
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
   buffer.AppendFormatted("#include <ArgusMonitor.h>\n");
   buffer.AppendFormatted("#include <TNetFolder.h>\n");
   // include
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("#include \"include/framework/%sGlobalSteering.h\"\n",shortCut.Data());
   }
   // includes
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("#include \"include/monitor/%sF%s.h\"\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   // Class
   buffer.AppendFormatted("\nclass %sMonitor : public ArgusMonitor\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   // Fields
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   TSocket*    fNetFolderSocket;\n");
   buffer.AppendFormatted("   TNetFolder* fNetFolder;\n");
   buffer.AppendFormatted("\n");
   // Folder Fields
   buffer.AppendFormatted("   // Folder fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            format.SetFormatted("   %%sF%%s*%%%ds f%%sFolder; %%%ds // Handle to %%s%%s Folder\n",typeLen-folderName[i].Length()-scl,nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
         else {
            format.SetFormatted("   TClonesArray*%%%ds f%%sFolders;%%%ds // Handle to %%s%%s Folders\n",typeLen-12,nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");
   // Steering Fields
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("   // Steering Parameter Fields\n");
      buffer.AppendFormatted("\n   %sGlobalSteering* fGlobalSteeringParameters; // Handle to the GlobalSteering Class\n",shortCut.Data());
   }
   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sMonitor(TRint *app);\n",shortCut.Data());
   // Folder Getters
   buffer.AppendFormatted("   // Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         int lt = typeLen-folderName[i].Length()-scl+nameLen-folderName[i].Length();
         if (folderArray[i]=="1") {
            format.SetFormatted("   %%sF%%s*%%%ds  Get%%s()%%%ds { return f%%sFolder;%%%ds };\n",typeLen-folderName[i].Length()-scl,11+nameLen-folderName[i].Length(),15+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   %%sF%%s**%%%ds Get%%sAddress()%%%ds { return &f%%sFolder;%%%ds };\n",typeLen-folderName[i].Length()-scl,4+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
         else if (folderArray[i]=="variable") {
            format.SetFormatted("   %%sF%%s*%%%ds  Get%%sAt(int index)%%%ds\n",typeLen-folderName[i].Length()-scl,0+nameLen-folderName[i].Length(),lt);
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),"");
            buffer.AppendFormatted("   { if (f%sFolders->GetEntries()<=index)\n",folderName[i].Data());
            buffer.AppendFormatted("        for (int i=f%sFolders->GetEntries();i<=index;i++)\n",folderName[i].Data());
            buffer.AppendFormatted("           new((*f%sFolders)[i]) %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("     return (%sF%s*)f%sFolders->At(index); };\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            format.SetFormatted("   TClonesArray*%%%ds  Get%%ss()%%%ds { return f%%sFolders;%%%ds };\n",typeLen-12,10+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray**%%%ds Get%%sAddress()%%%ds { return &f%%sFolders;%%%ds };\n",typeLen-12,4+nameLen-folderName[i].Length(),13+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
         else {
            format.SetFormatted("   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds { return (%%sF%%s*)f%%sFolders->At(index);%%%ds };\n",typeLen-folderName[i].Length()-scl,0+nameLen-folderName[i].Length(),lt);
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray*%%%ds  Get%%ss()%%%ds { return f%%sFolders;%%%ds };\n",typeLen-12,10+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray**%%%ds Get%%sAddress()%%%ds { return &f%%sFolders;%%%ds };\n",typeLen-12,4+nameLen-folderName[i].Length(),13+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
      }
   }
   buffer.AppendFormatted("\n");
   //NetFolder
// if use net folder
   buffer.AppendFormatted("   TNetFolder* GetNetFolder(){ return fNetFolder;}\n");
   buffer.AppendFormatted("   bool ConnectServer();\n");  
   buffer.AppendFormatted("\n");
   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   buffer.AppendFormatted("   bool ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   bool ReadArrayDataBaseFolders();\n");
   buffer.AppendFormatted("   bool StartMonitor();\n");
   buffer.AppendFormatted("\n");
   // Steering
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }
   // Private
   buffer.AppendFormatted("private:\n");
   // ODB
   buffer.AppendFormatted("   // ODB Methodes\n");
   buffer.AppendFormatted("   bool InitODB();\n");
   buffer.AppendFormatted("\n");
   // Folders
   buffer.AppendFormatted("   // Folder Methodes\n");
   buffer.AppendFormatted("   void InitSingleFolders();\n");
   buffer.AppendFormatted("   void InitArrayFolders();\n");
   buffer.AppendFormatted("   void ResetFolders();\n");
   buffer.AppendFormatted("   void CleanUpFolders();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sMonitor,0);\n",shortCut.Data());
   // Footer
   buffer.AppendFormatted("};\n\n");
   buffer.AppendFormatted("extern %sMonitor *gMonitor;  // global Monitor Handle\n\n",shortCut.Data());
   buffer.AppendFormatted("#endif   // %sMonitor_H\n",shortCut.Data());
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
