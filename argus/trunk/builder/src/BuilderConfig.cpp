/********************************************************************

  BuilderConfig.cpp, Ryu Sawada

  $Log$
  Revision 1.9  2005/03/18 12:10:09  sawada
  added status bar.

  Revision 1.8  2005/03/13 08:40:43  sawada
  modified handling of recursiveDepth.
  removed unused variables.

  Revision 1.7  2005/03/12 01:21:00  sawada
  Nested tab.

  Revision 1.6  2005/02/24 15:04:03  sawada
  Reduced number of configuration to 1.
  Replaced ss_getchar to getchar().
  Removed LineToProcess.
  Removed bk_is32,bk_find.
  Improved help.
  Handling of midas host and experiment.

  Revision 1.5  2005/02/06 02:15:18  sawada
  reduced number of configuration file to 1.

  Revision 1.4  2005/02/02 18:58:02  sawada
  small change.

  Revision 1.3  2005/02/01 14:44:33  sawada
  Resize frames in window.

  Revision 1.2  2005/01/30 20:39:39  sawada
  * Makefile of builder
  * Tab enable/disable
  * Bug fix.(fNetFolder, ConnectServer)

  Revision 1.1.1.1  2005/01/29 22:45:07  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

bool ArgusBuilder::WriteConfigCpp() {
   int i,j,k;
   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   int lenTot,ll;
   char* pos;
   int fileHandle;
   ROMEString format;
   ROMEString classDescription;
   classDescription.SetFormatted("This class handles the monitor configuration file (argusConfig.xml).");
   // File name
   cppFile.SetFormatted("%s/src/monitor/%sConfig.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Monitor");
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
   buffer.AppendFormatted("#include <ROMEXMLDataBase.h>\n");
   buffer.AppendFormatted("#ifdef HAVE_SQL\n");
   buffer.AppendFormatted("#include <ROMESQLDataBase.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include \"include/monitor/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/monitor/%sMonitor.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/monitor/%sWindow.h\"\n",shortCut.Data());
   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sConfig::%sConfig() {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   fConfigData = new ConfigData();\n");
   buffer.AppendFormatted("}\n\n");
   // Read Configuration File
   buffer.AppendFormatted("\n// Read Configuration File\n");
   buffer.AppendFormatted("bool %sConfig::ReadConfigurationFile(char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   xml->OpenFileForPath(fXMLFile);\n");
   buffer.AppendFormatted("   delete fConfigData;\n");
   buffer.AppendFormatted("   fConfigData = new ConfigData;\n");
   buffer.AppendFormatted("   ROMEString path = \"//Configuration/MainConfiguration\";\n");
   buffer.AppendFormatted("   ReadConfiguration(xml,path,0);\n");
   buffer.AppendFormatted("   if (!SetConfiguration())\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // Read Configuration
   buffer.AppendFormatted("\n// Read Configuration\n");
   buffer.AppendFormatted("bool %sConfig::ReadConfiguration(ROMEXML *xml,ROMEString& path,int index) {\n",shortCut.Data());
   // Window
   buffer.AppendFormatted("   // window\n");
   buffer.AppendFormatted("   fConfigData->fWindow = new ConfigData::Window();\n");
   // Window/Scale
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Window/Scale\",fConfigData->fWindow->fScale,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData->fWindow->fScale==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fWindow->fScaleModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fWindow->fScaleModified = true;\n");
   // Window/StatusBar
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Window/StatusBar\",fConfigData->fWindow->fStatusBar,\"\");\n");
   buffer.AppendFormatted("   fConfigData->fWindow->fStatusBar.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData->fWindow->fStatusBar==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fWindow->fStatusBarModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fWindow->fStatusBarModified = true;\n");
   // --Window
   buffer.AppendFormatted("   if (fConfigData->fWindow->fScaleModified ||\n");
   buffer.AppendFormatted("       fConfigData->fWindow->fStatusBarModified)\n");
   buffer.AppendFormatted("      fConfigData->fWindowModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fWindowModified = false;\n");
   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   fConfigData->fDataBase = new ConfigData::DataBase();\n");
   // DataBase/Type
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/DataBase/Type\",fConfigData->fDataBase->fType,\"\");\n");
   buffer.AppendFormatted("   fConfigData->fDataBase->fType.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData->fDataBase->fType==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fDataBase->fTypeModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fDataBase->fTypeModified = true;\n");
   // DataBase/Connection
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/DataBase/Connection\",fConfigData->fDataBase->fConnection,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData->fDataBase->fConnection==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fDataBase->fConnectionModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fDataBase->fConnectionModified = true;\n");
   // --DataBase
   buffer.AppendFormatted("   if (fConfigData->fDataBase->fTypeModified ||\n");
   buffer.AppendFormatted("       fConfigData->fDataBase->fConnectionModified)\n");
   buffer.AppendFormatted("      fConfigData->fDataBaseModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fDataBaseModified = false;\n");
   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   fConfigData->fOnline = new ConfigData::Online();\n");
   // Online/Host
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Online/Host\",fConfigData->fOnline->fHost,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData->fOnline->fHost==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fOnline->fHostModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fOnline->fHostModified = true;\n");
   // Online/Experiment
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Online/Experiment\",fConfigData->fOnline->fExperiment,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData->fOnline->fExperiment==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fOnline->fExperimentModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fOnline->fExperimentModified = true;\n");
   // --Online
   buffer.AppendFormatted("   if (fConfigData->fOnline->fHostModified ||\n");
   buffer.AppendFormatted("       fConfigData->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      fConfigData->fOnlineModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fOnlineModified = false;\n");
   // SocketInterface
   buffer.AppendFormatted("   // SocketInterface\n");
   buffer.AppendFormatted("   fConfigData->fSocketInterface = new ConfigData::SocketInterface();\n");
   // SocketInterface/Host
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/SocketInterface/Host\",fConfigData->fSocketInterface->fHost,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData->fSocketInterface->fHost==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fSocketInterface->fHostModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fSocketInterface->fHostModified = true;\n");
   // SocketInterface/PortNumber
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/SocketInterface/PortNumber\",fConfigData->fSocketInterface->fPortNumber,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData->fSocketInterface->fPortNumber==\"\")\n");
   buffer.AppendFormatted("      fConfigData->fSocketInterface->fPortNumberModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fSocketInterface->fPortNumberModified = true;\n");
   // --SocketInterface
   buffer.AppendFormatted("   if (fConfigData->fSocketInterface->fHostModified ||\n");
   buffer.AppendFormatted("       fConfigData->fSocketInterface->fPortNumberModified)\n");
   buffer.AppendFormatted("      fConfigData->fSocketInterfaceModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData->fSocketInterfaceModified = false;\n");
   // tabs
   buffer.AppendFormatted("   // tabs\n");
   buffer.AppendFormatted("   fConfigData->fTabsModified = false;\n");
   ROMEString pointer;
   ROMEString path;
   ROMEString classname;
   for (i=0;i<numOfTabHierarchy;i++) {
      buffer.AppendFormatted("   // %s Tab\n",tabHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      path.Resize(0);
      classname.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTab",tabHierarchyName[index].Data());
         path.InsertFormatted(0,"/child::Tab[child::TabName='%s']",tabHierarchyName[index].Data());
         classname.InsertFormatted(0,"::%sTab",tabHierarchyName[index].Data());
         index = tabHierarchyParentIndex[index];
      }
      buffer.AppendFormatted("   fConfigData%s = new ConfigData%s();\n",pointer.Data(),classname.Data());
      // Active
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tabs%s/Active\",fConfigData%s->fActive,\"\");\n",path.Data(),pointer.Data());
      buffer.AppendFormatted("   if (fConfigData%s->fActive==\"\")\n",pointer.Data());
      buffer.AppendFormatted("      fConfigData%s->fActiveModified = false;\n",pointer.Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData%s->fActiveModified = true;\n",pointer.Data());
      // Steering parameter
      if (numOfSteering[tabHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("   // steering parameters\n");
         buffer.AppendFormatted("   fConfigData%s->fSteering = new ConfigData%s::Steering();\n",pointer.Data(),classname.Data());
         ROMEString pathT;
         ROMEString pointerT;
         ROMEString classT;
         pathT.SetFormatted("path+\"/Tabs%s",path.Data());
         pointerT.SetFormatted("fConfigData%s->fSteering",pointer.Data());
         classT.SetFormatted("ConfigData%s::Steering",classname.Data());
         WriteSteeringConfigRead(buffer,0,tabHierarchyClassIndex[i],xml,pathT,pointerT,classT);
      }
      // all
      buffer.AppendFormatted("   if (fConfigData%s->fActiveModified",pointer.Data());
      if (numOfSteering[tabHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("\n    || fConfigData%s->fSteeringModified",pointer.Data());
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("      fConfigData->fTabsModified = true;\n");
      buffer.AppendFormatted("      fConfigData%sModified = true;\n",pointer.Data());
      ROMEString tempPointer = pointer;
      while (true) {
         for (j=tempPointer.Length()-1;tempPointer[j]!='>' && j>0;j--) {}
         if (j<=1) 
            break;
         tempPointer = tempPointer(0,j-1);
         buffer.AppendFormatted("      fConfigData%sModified = true;\n",tempPointer.Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData%sModified = false;\n",pointer.Data());
   }
   // Global Steering Parameters
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("   // global steering parameters\n");
      buffer.AppendFormatted("   fConfigData->fGlobalSteering = new ConfigData::GlobalSteering();\n");
      ROMEString pathT;
      ROMEString pointerT;
      ROMEString classT;
      pathT.SetFormatted("path+\"/GlobalSteeringParameters");
      pointerT.SetFormatted("fConfigData->fGlobalSteering");
      classT.SetFormatted("ConfigData::GlobalSteering");
      WriteSteeringConfigRead(buffer,0,numOfTabHierarchy,xml,pathT,pointerT,classT);
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // Set Configuration
   buffer.AppendFormatted("\n// Set Configuration\n");
   buffer.AppendFormatted("bool %sConfig::SetConfiguration() {\n",shortCut.Data());
   bool need_cstop = false;
   for (i=0;i<numOfTabHierarchy;i++) {
      for (j=0;j<numOfSteering[i];j++) {
         for (k=0;k<numOfSteerFields[i][j];k++) {
            if(IsNumber(steerFieldType[i][j][k].Data()))
               need_cstop = true;
         }
      }
   }
   if(need_cstop)
      buffer.AppendFormatted("   char *cstop;\n");
   // Window
   buffer.AppendFormatted("   if (fConfigData->fWindow->fScaleModified) {\n");
   buffer.AppendFormatted("      gMonitor->SetWindowScale(atof(fConfigData->fWindow->fScale.Data()));\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData->fWindow->fStatusBarModified) {\n");
   buffer.AppendFormatted("      if (fConfigData->fWindow->fStatusBar==\"false\")\n");
   buffer.AppendFormatted("         gWindow->SetStatusBarSwitch(false);\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gWindow->SetStatusBarSwitch(true);\n");
   buffer.AppendFormatted("   }\n");
   // DataBase
   buffer.AppendFormatted("   if (fConfigData->fDataBase->fConnectionModified) {\n");
   buffer.AppendFormatted("      gMonitor->SetDataBaseConnection(fConfigData->fDataBase->fConnection);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData->fDataBase->fTypeModified) {\n");
   buffer.AppendFormatted("      if (fConfigData->fDataBase->fType==\"sql\") {\n");
   buffer.AppendFormatted("#ifdef HAVE_SQL\n");
   buffer.AppendFormatted("         delete gMonitor->GetDataBase();\n");
   buffer.AppendFormatted("         gMonitor->SetDataBase(new ROMESQLDataBase());\n");
   buffer.AppendFormatted("         if (!gMonitor->GetDataBase()->Init(\"\",gMonitor->GetDataBaseConnection()))\n");
   buffer.AppendFormatted("            return false;\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData->fDataBase->fType==\"xml\") {\n");
   buffer.AppendFormatted("         delete gMonitor->GetDataBase();\n");
   buffer.AppendFormatted("         gMonitor->SetDataBase(new ROMEXMLDataBase());\n");
   buffer.AppendFormatted("         ROMEString str = gMonitor->GetDataBaseConnection();\n");
   buffer.AppendFormatted("         int index;\n");
   buffer.AppendFormatted("         if ((index=str.Index(\";\",1,0,TString::kExact))==-1) {\n");
   buffer.AppendFormatted("            cout << \"Invalid database connection\" << endl;\n");
   buffer.AppendFormatted("            return false;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         ROMEString path = str(0,index);\n");
   buffer.AppendFormatted("         if (path[path.Length()-1]!='/' && path[path.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("            path += \"/\";\n");
   buffer.AppendFormatted("         gMonitor->SetDataBaseDir((char*)path.Data());\n");
   buffer.AppendFormatted("         if (!gMonitor->GetDataBase()->Init(gMonitor->GetDataBaseDir(),((TString)str(index+1,str.Length()-index-1)).Data()))\n");
   buffer.AppendFormatted("            return false;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   if (fConfigData->fOnline->fHostModified)\n");
   buffer.AppendFormatted("      gMonitor->SetOnlineHost((char*)fConfigData->fOnline->fHost.Data());\n");
   buffer.AppendFormatted("   if (fConfigData->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      gMonitor->SetOnlineExperiment((char*)fConfigData->fOnline->fExperiment.Data());\n");
   // SocketInterface
   buffer.AppendFormatted("   // SocketInterface\n");
   buffer.AppendFormatted("   if (fConfigData->fSocketInterface->fHostModified)\n");
   buffer.AppendFormatted("      gMonitor->SetSocketInterfaceHost((char*)fConfigData->fSocketInterface->fHost.Data());\n");
   buffer.AppendFormatted("   if (fConfigData->fSocketInterface->fPortNumberModified)\n");
   buffer.AppendFormatted("      gMonitor->SetSocketInterfacePortNumber((char*)fConfigData->fSocketInterface->fPortNumber.Data());\n");
   // Tabs
   buffer.AppendFormatted("   // tabs\n");
   for (i=0;i<numOfTabHierarchy;i++) {
      buffer.AppendFormatted("   // %s tab\n",tabHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTab",tabHierarchyName[index].Data());
         index = tabHierarchyParentIndex[index];
      }
      ROMEString switchString = tabHierarchyName[i].Data();
      index = tabHierarchyParentIndex[i];
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabHierarchyName[index].Data());
         index = tabHierarchyParentIndex[index];
      }
      buffer.AppendFormatted("   if (fConfigData%s->fActiveModified) {\n",pointer.Data());
      buffer.AppendFormatted("      if (fConfigData%s->fActive==\"false\")\n",pointer.Data());
      buffer.AppendFormatted("         gWindow->GetTabSwitches()->%s = false;\n",switchString.Data());
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gWindow->GetTabSwitches()->%s = true;\n",switchString.Data());
      buffer.AppendFormatted("   }\n");
      // Steering parameter
      if (numOfSteering[tabHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("   // steering parameters\n");
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*) gWindow->Get%s%03dTab())->GetSP()",shortCut.Data(),tabName[tabHierarchyClassIndex[i]].Data(),tabHierarchyName[i].Data(),i);
         WriteSteeringConfigSet(buffer,0,tabHierarchyClassIndex[i],pointerT,steerPointerT);
      }
   }
   for (i=0;i<numOfTabHierarchy;i++) {
      int index = tabHierarchyParentIndex[i];
      ROMEString switchString = tabHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,tabHierarchyName[index].Data());
         index = tabHierarchyParentIndex[index];
      }
      buffer.AppendFormatted("   // %s tab enabled hierarchy\n",tabHierarchyName[i].Data());
      buffer.AppendFormatted("   gWindow->GetTabSwitches()->%s = gWindow->GetTabSwitches()->%s ",switchString.Data(),switchString.Data());
      index = tabHierarchyParentIndex[i];
      while (index!=-1) {
         int index2 = tabHierarchyParentIndex[index];
         ROMEString switchString2 = tabHierarchyName[index].Data();
         while (index2!=-1) {
            switchString2.Insert(0,"_");
            switchString2.Insert(0,tabHierarchyName[index2].Data());
            index2 = tabHierarchyParentIndex[index2];
         }
         buffer.AppendFormatted(" * gWindow->GetTabSwitches()->%s",switchString2.Data());
         index = tabHierarchyParentIndex[index];
      }
      buffer.AppendFormatted(";\n",pointer.Data());
   }
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTabHierarchy]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringConfigSet(buffer,0,numOfTabHierarchy,pointerT,steerPointerT);
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");
   // Write Configuration File
   buffer.AppendFormatted("\n// Write Configuration File\n");
   buffer.AppendFormatted("bool %sConfig::WriteConfigurationFile(char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   if (!xml->OpenFileForWrite(fXMLFile))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   xml->StartElement(\"Configuration\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xmlns:xsi\",\"http://www.w3.org/2001/XMLSchema-instance\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xsi:noNamespaceSchemaLocation\",\"c:/Argus/ArgusConfig.xsd\");\n");
   buffer.AppendFormatted("   xml->StartElement(\"MainConfiguration\");\n");
   buffer.AppendFormatted("   WriteConfiguration(xml,0);\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   xml->EndDocument();\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   // Write Configuration
   buffer.AppendFormatted("\n// Write Configuration\n");
   buffer.AppendFormatted("bool %sConfig::WriteConfiguration(ROMEXML *xml,int index) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   // Window
   buffer.AppendFormatted("   // window\n");
   buffer.AppendFormatted("   if (fConfigData->fWindowModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Window\");\n");
   // Window/Scale
   buffer.AppendFormatted("      if (index==0){\n");
   buffer.AppendFormatted("         str.SetFormatted(\"%%2.1f\",gMonitor->GetWindowScale());\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Scale\",(char*)str.Data());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData->fWindow->fScaleModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Scale\",(char*)fConfigData->fWindow->fScale.Data());\n");
   // Window/StatusBar
   buffer.AppendFormatted("      if (index==0){\n");
   buffer.AppendFormatted("         if(gWindow->GetStatusBarSwitch())\n");
   buffer.AppendFormatted("            str.SetFormatted(\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            str.SetFormatted(\"false\");\n");
   buffer.AppendFormatted("         xml->WriteElement(\"StatusBar\",(char*)str.Data());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData->fWindow->fStatusBarModified){\n");
   buffer.AppendFormatted("         if(gWindow->GetStatusBarSwitch())\n");
   buffer.AppendFormatted("            str.SetFormatted(\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            str.SetFormatted(\"false\");\n");
   buffer.AppendFormatted("         xml->WriteElement(\"StatusBar\",(char*)str.Data());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (fConfigData->fDataBaseModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"DataBase\");\n");
   // DataBase/Type
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Type\",gMonitor->GetDataBase()->GetType());\n");
   buffer.AppendFormatted("      else if (fConfigData->fDataBase->fTypeModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Type\",(char*)fConfigData->fDataBase->fType.Data());\n");
   // DataBase/Connection
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Connection\",gMonitor->GetDataBaseConnection());\n");
   buffer.AppendFormatted("      else if (fConfigData->fDataBase->fConnectionModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Connection\",(char*)fConfigData->fDataBase->fConnection.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   if (fConfigData->fOnlineModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Online\");\n");
   // Online/Host
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",gMonitor->GetOnlineHost());\n");
   buffer.AppendFormatted("      else if (fConfigData->fOnline->fHostModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",(char*)fConfigData->fOnline->fHost.Data());\n");
   // Online/Experiment
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",gMonitor->GetOnlineExperiment());\n");
   buffer.AppendFormatted("      else if (fConfigData->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",(char*)fConfigData->fOnline->fExperiment.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // SocketInterface
   buffer.AppendFormatted("   // SocketInterface\n");
   buffer.AppendFormatted("   if (fConfigData->fSocketInterfaceModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"SocketInterface\");\n");
   // SocketInterface/Host
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",gMonitor->GetSocketInterfaceHost());\n");
   buffer.AppendFormatted("      else if (fConfigData->fSocketInterface->fHostModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",(char*)fConfigData->fSocketInterface->fHost.Data());\n");
   // SocketInterface/PortNumber
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         str.SetFormatted(\"%%d\",gMonitor->GetSocketInterfacePortNumber());\n");
   buffer.AppendFormatted("         xml->WriteElement(\"PortNumber\",(char*)str.Data());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData->fSocketInterface->fPortNumberModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"PortNumber\",(char*)fConfigData->fSocketInterface->fPortNumber.Data());\n");
   // Tabs
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   // tabs\n");
   buffer.AppendFormatted("   if (fConfigData->fTabsModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Tabs\");\n");
   pointer.Resize(0);
   WriteTabConfigWrite(buffer,-1,pointer,0);
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("   if (fConfigData->fGlobalSteeringModified || index==0) {\n");
      buffer.AppendFormatted("      ROMEString value;\n");
      buffer.AppendFormatted("      xml->StartElement(\"GlobalSteeringParameters\");\n");
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("fConfigData->fGlobalSteering");
      steerPointerT.SetFormatted("gMonitor->GetGSP()");
      WriteSteeringConfigWrite(buffer,0,numOfTabHierarchy,pointerT,steerPointerT,1);
   }
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("      xml->EndElement();\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   // Close cpp-File
   fileHandle = open(cppFile.Data(),O_RDONLY);
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
      fileHandle = open(cppFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(cppFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << cppFile.Data() << endl;
      nb = write(fileHandle,buffer.Data(), buffer.Length());
      close(fileHandle);
   }
   return true;
}

bool ArgusBuilder::WriteConfigH() {
   int i;
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   int nb;
   int fileHandle;
   // File name
   hFile.SetFormatted("%s/include/monitor/%sConfig.h",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ArgusBuilder.                          //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");
   // Header
   buffer.AppendFormatted("#ifndef %sConfig_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sConfig_H\n\n",shortCut.Data());
   buffer.AppendFormatted("#include <ROMEString.h>\n");
   buffer.AppendFormatted("#ifndef __CINT__\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <ArgusConfig.h>\n");
   // Class
   buffer.AppendFormatted("\nclass %sConfig : public ArgusConfig\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#ifndef __CINT__\n");
   // sub classes
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   class ConfigData\n");
   buffer.AppendFormatted("   {\n");
   buffer.AppendFormatted("   public:\n");
   // window
   buffer.AppendFormatted("      // window;\n");
   buffer.AppendFormatted("      class Window {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fScale;\n");
   buffer.AppendFormatted("         bool        fScaleModified;\n");
   buffer.AppendFormatted("         ROMEString  fStatusBar;\n");
   buffer.AppendFormatted("         bool        fStatusBarModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Window *fWindow;\n");
   buffer.AppendFormatted("      bool   fWindowModified;\n");
   // database
   buffer.AppendFormatted("      // database;\n");
   buffer.AppendFormatted("      class DataBase {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fType;\n");
   buffer.AppendFormatted("         bool        fTypeModified;\n");
   buffer.AppendFormatted("         ROMEString  fConnection;\n");
   buffer.AppendFormatted("         bool        fConnectionModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      DataBase *fDataBase;\n");
   buffer.AppendFormatted("      bool   fDataBaseModified;\n");
   // online
   buffer.AppendFormatted("      // online;\n");
   buffer.AppendFormatted("      class Online {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fHost;\n");
   buffer.AppendFormatted("         bool        fHostModified;\n");
   buffer.AppendFormatted("         ROMEString  fExperiment;\n");
   buffer.AppendFormatted("         bool        fExperimentModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Online *fOnline;\n");
   buffer.AppendFormatted("      bool   fOnlineModified;\n");
   // SocketInterface
   buffer.AppendFormatted("      // SocketInterface;\n");
   buffer.AppendFormatted("      class SocketInterface {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fHost;\n");
   buffer.AppendFormatted("         bool        fHostModified;\n");
   buffer.AppendFormatted("         ROMEString  fPortNumber;\n");
   buffer.AppendFormatted("         bool        fPortNumberModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      SocketInterface *fSocketInterface;\n");
   buffer.AppendFormatted("      bool fSocketInterfaceModified;\n");
   // tabs
   buffer.AppendFormatted("      // tabs\n");
   WriteTabConfigClass(buffer,-1,0);
   buffer.AppendFormatted("      bool   fTabsModified;\n");
   // steering parameters
   buffer.AppendFormatted("      class GlobalSteering {\n");
   buffer.AppendFormatted("      public:\n");
   if (numOfSteering[numOfTabHierarchy]>0) {
      WriteSteeringConfigClass(buffer,0,numOfTabHierarchy,2);
   }
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      GlobalSteering *fGlobalSteering;\n");
   buffer.AppendFormatted("      bool            fGlobalSteeringModified;\n");
   buffer.AppendFormatted("   public:\n");
   // Constructor
   buffer.AppendFormatted("      ConfigData() {\n");
   buffer.AppendFormatted("         fDataBaseModified = false;\n");
   buffer.AppendFormatted("         fOnlineModified = false;\n");
   buffer.AppendFormatted("         fSocketInterfaceModified = false;\n");
   buffer.AppendFormatted("         fTabsModified = false;\n");
   for (i=0;i<numOfTabHierarchy;i++) {
      if( tabHierarchyParentIndex[i] != -1 )
         continue;
      buffer.AppendFormatted("         f%sTabModified = false;\n",tabName[i].Data());
      buffer.AppendFormatted("         f%sTab = new %sTab();\n",tabName[i].Data(),tabName[i].Data());
   }
   if (numOfSteering[numOfTabHierarchy]>0) {
      buffer.AppendFormatted("         fGlobalSteeringModified = false;\n");
      buffer.AppendFormatted("         fGlobalSteering = new GlobalSteering();\n");
   }
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");
   // Fields
   buffer.AppendFormatted("   ConfigData *fConfigData;\n");
   buffer.AppendFormatted("   ROMEString fXMLFile;\n");
   buffer.AppendFormatted("\n");
   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sConfig();\n",shortCut.Data());
   // methods
   buffer.AppendFormatted("   bool WriteConfigurationFile(char *file);\n");
   buffer.AppendFormatted("   bool ReadConfigurationFile(char *file);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   bool ReadConfiguration(ROMEXML *xml,ROMEString& path,int index);\n");
   buffer.AppendFormatted("   bool WriteConfiguration(ROMEXML *xml,int index);\n");
   buffer.AppendFormatted("   bool SetConfiguration();\n");
   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("};\n\n");
   buffer.AppendFormatted("#endif   // %sConfig_H\n",shortCut.Data());
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
