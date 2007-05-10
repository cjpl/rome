/********************************************************************

  ROMEBuildVisualProject.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include "ROMEXML.h"
#include "ROMEBuilder.h"
#include "Riostream.h"

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectSln(Int_t version,ROMEString& projectGUID)
{
   ROMEString buffer;
   ROMEString formatVersion;

   switch (version) {
      case 2002:
         formatVersion = "7.00";
         break;
      case 2003:
         formatVersion = "8.00";
         break;
      case 2005:
         formatVersion = "9.00";
         break;
      default:
         return;
   }

   buffer.AppendFormatted("Microsoft Visual Studio Solution File, Format Version %s\n",formatVersion.Data());
   if (version==2005)
      buffer.AppendFormatted("# Visual Studio 2005\n");
   buffer.AppendFormatted("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s%s%d\", \"%s%s%d.vcproj\", \"{%s}\"\n",shortCut.Data(),mainProgName.Data(),version,shortCut.Data(),mainProgName.Data(),version,projectGUID.Data());
   if (version==2003) {
      buffer.AppendFormatted("	ProjectSection(ProjectDependencies) = postProject\n");
      buffer.AppendFormatted("	EndProjectSection\n");
   }
   buffer.AppendFormatted("EndProject\n");
   buffer.AppendFormatted("Global\n");
   if (version<2005) {
	   buffer.AppendFormatted("	GlobalSection(SolutionConfiguration) = preSolution\n");
	   if (version==2003)
		   buffer.AppendFormatted("		ConfigName.0 = Debug\n");
	   if (version==2002)
		   buffer.AppendFormatted("		Debug = Debug\n");
	   buffer.AppendFormatted("	EndGlobalSection\n");
	   buffer.AppendFormatted("	GlobalSection(ProjectConfiguration) = postSolution\n");
	   buffer.AppendFormatted("		{%s}.Debug.ActiveCfg = Debug|Win32\n",projectGUID.Data());
	   buffer.AppendFormatted("		{%s}.Debug.Build.0 = Debug|Win32\n",projectGUID.Data());
	   buffer.AppendFormatted("	EndGlobalSection\n");
	   buffer.AppendFormatted("	GlobalSection(ExtensibilityGlobals) = postSolution\n");
	   buffer.AppendFormatted("	EndGlobalSection\n");
	   buffer.AppendFormatted("	GlobalSection(ExtensibilityAddIns) = postSolution\n");
	   buffer.AppendFormatted("	EndGlobalSection\n");
   }
   else {
	   buffer.AppendFormatted("	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n");
	   buffer.AppendFormatted("		Debug|Win32 = Debug|Win32\n");
	   buffer.AppendFormatted("	EndGlobalSection\n");
	   buffer.AppendFormatted("	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n");
	   buffer.AppendFormatted("		{%s}.Debug|Win32.ActiveCfg = Debug|Win32\n",projectGUID.Data());
	   buffer.AppendFormatted("		{%s}.Debug|Win32.Build.0 = Debug|Win32\n",projectGUID.Data());
	   buffer.AppendFormatted("	EndGlobalSection\n");
	   buffer.AppendFormatted("	GlobalSection(SolutionProperties) = preSolution\n");
	   buffer.AppendFormatted("		HideSolutionNode = FALSE\n");
	   buffer.AppendFormatted("	EndGlobalSection\n");
   }
   buffer.AppendFormatted("EndGlobal\n");

   ROMEString fileName;
   fileName.SetFormatted("%s%s%d.sln",shortCut.Data(),mainProgName.Data(),version);
   WriteFile(fileName.Data(),buffer.Data(),6);
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectProjSettings(ROMEXML *xml,Int_t version,ROMEString& projectGUID)
{
   int i;
   ROMEString str;
   ROMEString formatVersion;

   switch (version) {
      case 2002:
         formatVersion = "7.00";
         break;
      case 2003:
         formatVersion = "7.10";
         break;
      case 2005:
         formatVersion = "8.00";
         break;
      default:
         return;
   }

   // VisualStudioProject
   xml->StartElement("VisualStudioProject");
   xml->WriteAttribute("ProjectType","Visual C++");
   xml->WriteAttribute("Version",formatVersion.Data());
   str.SetFormatted("%s%s%d",shortCut.Data(),mainProgName.Data(),version);
   xml->WriteAttribute("Name",str.Data());
   xml->WriteAttribute("ProjectGUID",projectGUID.Data());
   if (version>2002) {
      str.SetFormatted("%s%s%d",shortCut.Data(),mainProgName.Data(),version);
      xml->WriteAttribute("RootNamespace",str.Data());
   }
   xml->WriteAttribute("Keyword","ManagedCProj");

   // Platforms
   xml->StartElement("Platforms");
   xml->StartElement("Platform");
   xml->WriteAttribute("Name","Win32");
   xml->EndElement();
   xml->EndElement();

   if (version==2005) {
      // ToolFiles
      xml->StartElement("ToolFiles");
      xml->EndElement();
   }

   // Configurations
   xml->StartElement("Configurations");

   // Debug
   xml->StartElement("Configuration");
   xml->WriteAttribute("Name","Debug|Win32");
   if (version==2002) {
      xml->WriteAttribute("OutputDirectory","Debug");
      xml->WriteAttribute("IntermediateDirectory","Debug");
   }
   if (version>2002) {
      xml->WriteAttribute("OutputDirectory","$(SolutionDir)$(ConfigurationName)");
      xml->WriteAttribute("IntermediateDirectory","$(ConfigurationName)");
   }
   xml->WriteAttribute("ConfigurationType","1");
   xml->WriteAttribute("CharacterSet","2");
   if (version<=2003)
      xml->WriteAttribute("ManagedExtensions","FALSE");
   if (version>2003)
      xml->WriteAttribute("ManagedExtensions","0");

   // VCCLCompilerTool
   ROMEString includeDirs;
   for (i=0;i<includeDirectories->GetEntriesFast();i++)
      includeDirs.AppendFormatted(";\"%s\"",includeDirectories->At(i).Data());
   for (i=0;i<numOfMFIncDirs;i++)
      includeDirs.AppendFormatted(";\"%s\"",mfIncDir[i].Data());
   ROMEString preProcDefs = "WIN32;_DEBUG";
   if (version>2003)
      preProcDefs += ";_CRT_SECURE_NO_DEPRECATE";
   if (mysql)
      preProcDefs += ";HAVE_MYSQL";
   if (midas)
      preProcDefs += ";HAVE_MIDAS";
   for (i=0;i<flags.GetEntriesFast();i++)
      preProcDefs.AppendFormatted(";%s",flags.At(i).Data());
   for (i=0;i<affiliations.GetEntriesFast();i++)
      preProcDefs.AppendFormatted(";HAVE_%s",((ROMEString)affiliations.At(i)).ToUpper(str));
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCCLCompilerTool");
   xml->WriteAttribute("Optimization","0");
   xml->WriteAttribute("AdditionalIncludeDirectories",includeDirs.Data());
   xml->WriteAttribute("PreprocessorDefinitions",preProcDefs.Data());
   if (minRebuild)
      xml->WriteAttribute("MinimalRebuild","TRUE");
   else
      xml->WriteAttribute("MinimalRebuild","FALSE");
   xml->WriteAttribute("BasicRuntimeChecks","2");
   if (version>2003)
      xml->WriteAttribute("RuntimeLibrary","3");
   else
      xml->WriteAttribute("RuntimeLibrary","2");
   xml->WriteAttribute("RuntimeTypeInfo","TRUE");
   xml->WriteAttribute("WarningLevel","3");
   xml->WriteAttribute("DebugInformationFormat","3");
   if (version>2003)
      xml->WriteAttribute("DisableSpecificWarnings","4996");
   xml->EndElement();

   // VCCustomBuildTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCCustomBuildTool");
   xml->EndElement();

   // VCLinkerTool
   ROMEString libDirs;
   libDirs.AppendFormatted("\"$(ROOTSYS)\\lib\"");
   libDirs.AppendFormatted(";\"$(ROMESYS)\\lib_win\"");
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCLinkerTool");
   xml->WriteAttribute("AdditionalDependencies","winmm.lib wsock32.lib gdi32.lib user32.lib kernel32.lib");
   if (version==2002) {
      str.SetFormatted("$(OutDir)/%s%s.exe",shortCut.Data(),mainProgName.Data());
      xml->WriteAttribute("OutputFile",str.Data());
   }
   else
      xml->WriteAttribute("OutputFile","$(OutDir)\\$(ProjectName).exe");
   xml->WriteAttribute("LinkIncremental","2");
   xml->WriteAttribute("AdditionalLibraryDirectories",libDirs.Data());
   xml->WriteAttribute("GenerateDebugInformation","TRUE");
   if (version!=2002)
      xml->WriteAttribute("AssemblyDebug","1");
   xml->EndElement();

   // VCMIDLTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCMIDLTool");
   xml->EndElement();

   // VCPostBuildEventTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCPostBuildEventTool");
   xml->EndElement();

   // VCPreBuildEventTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCPreBuildEventTool");
   xml->EndElement();

   // VCPreLinkEventTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCPreLinkEventTool");
   xml->EndElement();

   // VCResourceCompilerTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCResourceCompilerTool");
   xml->EndElement();

   // VCWebServiceProxyGeneratorTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCWebServiceProxyGeneratorTool");
   xml->EndElement();

   // VCXMLDataGeneratorTool
   if (version!=2002) {
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCXMLDataGeneratorTool");
      xml->EndElement();
   }

   // VCWebDeploymentTool
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCWebDeploymentTool");
   xml->EndElement();

   // VCAuxiliaryManagedWrapperGeneratorTool
   if (version!=2002) {
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCAuxiliaryManagedWrapperGeneratorTool");
      xml->EndElement();
   }
   if (version>2003) {
      // VCManagedResourceCompilerTool
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCManagedResourceCompilerTool");
      xml->EndElement();

      // VCALinkTool
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCALinkTool");
      xml->EndElement();

      // VCManifestTool
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCManifestTool");
      xml->EndElement();

      // VCXDCMakeTool
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCXDCMakeTool");
      xml->EndElement();

      // VCBscMakeTool
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCBscMakeTool");
      xml->EndElement();

      // VCFxCopTool
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCFxCopTool");
      xml->EndElement();

      // VCAppVerifierTool
      xml->StartElement("Tool");
      xml->WriteAttribute("Name","VCAppVerifierTool");
      xml->EndElement();
   }

   // End Debug
   xml->EndElement();

   // End Configurations
   xml->EndElement();

   // References
   xml->StartElement("References");
   xml->EndElement();
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectProjFiles(ROMEXML *xml,ROMEStrArray* files,const char* folderName,const char* filter)
{
   int i;
   ROMEString str;
   ROMEString path;
   ROMEString name;
   ROMEString ext;
   if (files->GetEntriesFast()>0) {
      xml->StartElement("Filter");
      xml->WriteAttribute("Name",folderName);
      xml->WriteAttribute("Filter",filter);
      for (i=0;i<files->GetEntriesFast();i++) {
         xml->StartElement("File");
         str = files->At(i).Data();
         str.ReplaceAll("/","\\");
         AnalyzeFileName(str.Data(),path,name,ext);
         RelativeWindowsPath(str,outDir.Data());
         xml->WriteAttribute("RelativePath",str.Data());
         if (str.Index("\\dict\\")!=-1 || str.Index("dict\\")==0) {
            WriteVisualProjectProjFileConfiguration(xml,"0",NULL);
            ROMEString buffer;
            buffer.SetFormatted("This file is used inside the visual studio to generate the %s.", files->At(i).Data());
            str.ReplaceAll(".cpp","");
            WriteFile(str.Data(),buffer.Data(),0);
         }
         xml->EndElement();
      }
      xml->EndElement();
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectProjFileDictCreators(ROMEXML *xml)
{
   int i;
   ROMEString dictName;
   ROMEString str;
   ROMEString buffer;
   for (i=0;i<dictionaryNames->GetEntriesFast();i++) {
      if (dictionaryDependencies->At(i).Length()>0) {
         xml->StartElement("File");
         dictName = "dict/";
         dictName += dictionaryNames->At(i).Data();
         dictName.ReplaceAll("/","\\");
         RelativeWindowsPath(dictName,outDir.Data());
         xml->WriteAttribute("RelativePath",dictName.Data());
         xml->StartElement("FileConfiguration");
         xml->WriteAttribute("Name","Debug|Win32");
         xml->StartElement("Tool");
         xml->WriteAttribute("Name","VCCustomBuildTool");
         xml->WriteAttribute("CommandLine",dictionaryCommands->At(i).Data());
         str.SetFormatted("Creating %s dictionary",dictionaryNames->At(i).Data());
         xml->WriteAttribute("Description",str.Data());
         xml->WriteAttribute("AdditionalDependencies",dictionaryDependencies->At(i).Data());
         xml->WriteAttribute("Outputs",dictionaryOutputs->At(i).Data());
         xml->EndElement();
         xml->EndElement();
         buffer.SetFormatted("This file is used inside the visual studio to generate the %s dictionary", dictionaryNames->At(i).Data());
         fstream *fileStream;
         if ((fileStream = new fstream(dictName.Data(),ios::in))) {
            delete fileStream;
         }
         else
            WriteFile(dictName.Data(),buffer.Data(),0);
         xml->EndElement();
      }
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectProjFileROMEBuilder(ROMEXML *xml)
{
   ROMEString name;
   ROMEString str;
   ROMEString buffer;
   xml->StartElement("File");
   name = "$(ROMESYS)/bin/ROMEBuilder.exe";
   name.ReplaceAll("/","\\");
   name.ReplaceAll("\\\\","\\");
   RelativeWindowsPath(name,outDir.Data());
   xml->WriteAttribute("RelativePath",name.Data());
   xml->StartElement("FileConfiguration");
   xml->WriteAttribute("Name","Debug|Win32");
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCCustomBuildTool");
   xml->WriteAttribute("CommandLine","nmake -f Makefile.win /NOLOGO build");
   xml->WriteAttribute("Description","Running ROMEBuilder");
   str.SetFormatted("$(ROMESYS)/bin/ROMEBuilder.exe;%s",xmlFile.Data());
   xml->WriteAttribute("AdditionalDependencies",str.Data());
   xml->WriteAttribute("Outputs","Makefile.win");
   xml->EndElement();
   xml->EndElement();
   buffer.SetFormatted("This file is used inside the visual studio to run the ROMEBuilder");
   WriteFile(name.Data(),buffer.Data(),0);
   xml->EndElement();
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectProjUserSources(ROMEXML *xml)
{
   int i,j,k;
   bool addFile = true;
   bool flagMatched = false;
   ROMEString path;
   ROMEString name;
   ROMEString ext;
   ROMEString str;
   if (numOfMFSources>0) {
      xml->StartElement("Filter");
      xml->WriteAttribute("Name","User Classes");
      xml->WriteAttribute("Filter","cpp;c;cxx");
      for (i=0;i<numOfMFSources;i++) {
         if (!mfSourceFileUsed[i])
            continue;
         addFile = true;
         for (j=0;j<numOfMFSourceFlags[i];j++) {
            addFile = false;
            flagMatched = false;
            for (k=0;k<flags.GetEntriesFast();k++) {
               if (mfSourceFileFlag[i][j]==flags.At(k)) {
                  flagMatched = true;
                  break;
               }
            }
            if (flagMatched) {
               addFile = true;
               break;
            }
         }
         if (addFile) {
            xml->StartElement("File");
            str.SetFormatted("%s\\%s",mfSourceFilePath[i].Data(),mfSourceFileName[i].Data());
            str.ReplaceAll("//","\\");
            str.ReplaceAll("/\\","\\");
            str.ReplaceAll("\\/","\\");
            str.ReplaceAll("\\\\","\\");
            str.ReplaceAll("/","\\");
            RelativeWindowsPath(str,outDir.Data());
            xml->WriteAttribute("RelativePath",str.Data());
            xml->EndElement();
         }
      }
   }
   str.SetFormatted("%sUserDict",shortCut.Data());
   for (i=0;i<dictionaryNames->GetEntriesFast();i++) {
      if (dictionaryNames->At(i)==str && dictionaryDependencies->At(i).Length()>0) {
         if (numOfMFSources==0) {
            xml->StartElement("Filter");
            xml->WriteAttribute("Name","User Classes");
            xml->WriteAttribute("Filter","cpp;c;cxx");
         }
         xml->StartElement("File");
         str.SetFormatted("%s\\dict\\%sUserDict.cpp",outDir.Data(),shortCut.Data());
         str.ReplaceAll("//","\\");
         str.ReplaceAll("/\\","\\");
         str.ReplaceAll("\\/","\\");
         str.ReplaceAll("\\\\","\\");
         str.ReplaceAll("/","\\");
         RelativeWindowsPath(str,outDir.Data());
         xml->WriteAttribute("RelativePath",str.Data());
         WriteVisualProjectProjFileConfiguration(xml,"0",NULL);
         xml->EndElement();
         if (numOfMFSources==0) {
            xml->EndElement();
         }
      }
   }
   if (numOfMFSources>0) {
      xml->EndElement();
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectProjUserHeaders(ROMEXML *xml)
{
   int i,j,k;
   bool addFile = true;
   bool flagMatched = true;
   ROMEString path;
   ROMEString name;
   ROMEString ext;
   ROMEString str;
   if (numOfMFSources>0) {
      xml->StartElement("Filter");
      xml->WriteAttribute("Name","User Classes");
      xml->WriteAttribute("Filter","h");
      for (i=0;i<numOfMFSources;i++) {
         if (!mfSourceFileUsed[i])
            continue;
         if (mfHeaderFileName[i].Length()==0)
            continue;
         addFile = true;
         for (j=0;j<numOfMFSourceFlags[i];j++) {
            addFile = false;
            flagMatched = false;
            for (k=0;k<flags.GetEntriesFast();k++) {
               if (mfSourceFileFlag[i][j]==flags.At(k)) {
                  flagMatched = true;
                  break;
               }
            }
            if (flagMatched) {
               addFile = true;
               break;
            }
         }
         if (addFile) {
            xml->StartElement("File");
            str.SetFormatted("%s\\%s",mfHeaderFilePath[i].Data(),mfHeaderFileName[i].Data());
            str.ReplaceAll("//","\\");
            str.ReplaceAll("/\\","\\");
            str.ReplaceAll("\\/","\\");
            str.ReplaceAll("\\\\","\\");
            str.ReplaceAll("/","\\");
            RelativeWindowsPath(str,outDir.Data());
            xml->WriteAttribute("RelativePath",str.Data());
            xml->EndElement();
         }
      }
      xml->EndElement();
   }
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjectProjFileConfiguration(ROMEXML *xml,const char *warningLevel,const char* preCompiledHeaderFile)
{
   ROMEString str;
   xml->StartElement("FileConfiguration");
   xml->WriteAttribute("Name","Debug|Win32");
   xml->StartElement("Tool");
   xml->WriteAttribute("Name","VCCLCompilerTool");
   if (warningLevel!=NULL)
      xml->WriteAttribute("WarningLevel",warningLevel);
   if (preCompiledHeaderFile!=NULL) {
      xml->WriteAttribute("UsePrecompiledHeader","2");
      xml->WriteAttribute("PrecompiledHeaderThrough","");
      str.SetFormatted("$(IntDir)/%s.pch",preCompiledHeaderFile);
      xml->WriteAttribute("PrecompiledHeaderFile",str.Data());
   }
   xml->EndElement();
   xml->EndElement();
}

//______________________________________________________________________________
void ROMEBuilder::WriteVisualProjects(Int_t version)
{
   int i,j,k;
   ROMEString str;
   switch (version) {
      case 2002:
         break;
      case 2003:
         break;
      case 2005:
         break;
      default:
         return;
   }

   ROMEString projectGUID = "12345678-1234-1234-1234-123456789012";
   WriteVisualProjectSln(version,projectGUID);

   // VisualStudioProject
   ROMEXML *xml = new ROMEXML();
   ROMEString fileName;
   fileName.SetFormatted("%s%s%d.vcproj",shortCut.Data(),mainProgName.Data(),version);
   xml->OpenFileForWrite(fileName.Data());

   WriteVisualProjectProjSettings(xml,version,projectGUID);

   // Files
   xml->StartElement("Files");

   // Source Files
   xml->StartElement("Filter");
   xml->WriteAttribute("Name","Source Files");
   xml->WriteAttribute("Filter","cpp;c;cxx");
   xml->WriteAttribute("UniqueIdentifier","{4FC737F1-C7A5-4376-A066-2A32D752A2FF}");

   WriteVisualProjectProjFiles(xml,taskSources,"Tasks","cpp;c;cxx");
   WriteVisualProjectProjFiles(xml,tabSources,"Tabs","cpp;c;cxx");
   WriteVisualProjectProjFiles(xml,daqSources,"User DAQs","cpp;c;cxx");
   WriteVisualProjectProjFiles(xml,databaseSources,"User Databases","cpp;c;cxx");
   WriteVisualProjectProjFiles(xml,folderSources,"Folders","cpp;c;cxx");
   WriteVisualProjectProjUserSources(xml);
   WriteVisualProjectProjFiles(xml,generatedSources,"Generated","cpp;c;cxx");
   WriteVisualProjectProjFiles(xml,romeSources,"ROME","cpp;c;cxx");
   WriteVisualProjectProjFiles(xml,argusSources,"ARGUS","cpp;c;cxx");

   // End Source Files
   xml->EndElement();

   // Header Files
   xml->StartElement("Filter");
   xml->WriteAttribute("Name","Header Files");
   xml->WriteAttribute("Filter","h");
   xml->WriteAttribute("UniqueIdentifier","{93995380-89BD-4b04-88EB-625FBE52EBFB}");

   WriteVisualProjectProjFiles(xml,taskHeaders,"Tasks","h");
   WriteVisualProjectProjFiles(xml,tabHeaders,"Tabs","h");
   WriteVisualProjectProjFiles(xml,daqHeaders,"User DAQs","h");
   WriteVisualProjectProjFiles(xml,databaseHeaders,"User Databases","h");
   WriteVisualProjectProjFiles(xml,folderHeaders,"Folders","h");
   WriteVisualProjectProjUserHeaders(xml);
   WriteVisualProjectProjFiles(xml,generatedHeaders,"Generated","h");
   WriteVisualProjectProjFiles(xml,romeHeaders,"ROME","h");
   WriteVisualProjectProjFiles(xml,argusHeaders,"ARGUS","h");

   // End Header Files
   xml->EndElement();

   // Library Files
   xml->StartElement("Filter");
   xml->WriteAttribute("Name","Library Files");
   xml->WriteAttribute("Filter","lib");
   xml->WriteAttribute("UniqueIdentifier","{67DA6AB6-F800-4c08-8B7A-83BB121AAD01}");

   WriteVisualProjectProjFiles(xml,rootLibraries,"ROOT","lib");
   if (this->mysql)
      WriteVisualProjectProjFiles(xml,mysqlLibraries,"MySQL","lib");
   WriteVisualProjectProjFiles(xml,daqLibraries,"MIDAS","lib");

   // user
   xml->StartElement("Filter");
   xml->WriteAttribute("Name","User");
   xml->WriteAttribute("Filter","lib");
   bool addLib = true;
   bool flagMatched = false;
   for (i=0;i<numOfMFWinLibs;i++) {
      addLib = true;
      for (j=0;j<numOfMFWinLibFlags[i];j++) {
         addLib = false;
         flagMatched = false;
         for (k=0;k<flags.GetEntriesFast();k++) {
            if (mfWinLibFlag[i][j]==flags.At(k)) {
               flagMatched = true;
               break;
            }
         }
         if (flagMatched) {
            addLib = true;
            break;
         }
      }
      if (addLib) {
         xml->StartElement("File");
         str = mfWinLibName[i].Data();
         RelativeWindowsPath(str,outDir.Data());
         xml->WriteAttribute("RelativePath",str.Data());
         xml->EndElement();
      }
   }
   xml->EndElement();

   // End Library Files
   xml->EndElement();

   // Dictionary Creator Files
   xml->StartElement("Filter");
   xml->WriteAttribute("Name","Dictionary Creator Files");
   xml->WriteAttribute("Filter","");

   WriteVisualProjectProjFileDictCreators(xml);

   // End Dictionary Creator Files
   xml->EndElement();

   // ROMEBuilder Files
/*   xml->StartElement("Filter");
   xml->WriteAttribute("Name","ROMEBuilder Files");
   xml->WriteAttribute("Filter","");

   WriteVisualProjectProjFileROMEBuilder(xml);

   // End ROMEBuilder Files
   xml->EndElement();*/

   // End Files
   xml->EndElement();

   // Globals
   xml->StartElement("Globals");
   xml->EndElement();

   xml->EndDocument();
   delete xml;
}

//______________________________________________________________________________
void ROMEBuilder::RelativeWindowsPath(ROMEString &path,const char *referencePath)
{
   int ind,inde,i,equalChars;
   int backSteps;
   char *envPath;
   ROMEString temp;
   ROMEString str;
   ROMEString refPath = referencePath;
   ROMEString file;
   ind = refPath.Index(":");
   if (ind==-1)
      return;
   while ((ind = path.Index("$("))!=-1) {
      inde = path.Index(")");
      str = path(ind+2,inde-(ind+2));
      envPath = getenv(str.Data());
      str = path(0,ind);
      str += envPath;
      str += path(inde+1,path.Length()-inde-1);
      path = str;
   }
   ind = path.Index(":");
   if (ind==-1)
      return;
   refPath.ToLower();
   refPath.ReplaceAll("/","\\");
   if (refPath[refPath.Length()-1]!='\\')
      refPath += "\\";
   path.ReplaceAll("/","\\");
   ind = path.Last('\\');
   if (ind==-1) {
      file = path;
      path = "";
   }
   else {
      file = path(ind+1,path.Length()-ind-1);
      path = path(0,ind+1);
   }
   path.ToLower();
   for (equalChars=0;equalChars<TMath::Min(path.Length(),refPath.Length());equalChars++) {
      if (path[equalChars]!=refPath[equalChars])
         break;
   }
   while (path[equalChars]!='/' && path[equalChars]!='\\') {
      equalChars--;
      if (equalChars<0)
         break;
   }
   str.Resize(0);
   if (equalChars<refPath.Length()-1) {
      temp = refPath(equalChars+1,refPath.Length()-equalChars-1);
      backSteps = temp.NumberOfOccurrence("\\");
      for (i=0;i<backSteps;i++) {
         str += "..\\";
      }
   }
   str += path(equalChars+1,path.Length()-equalChars-1);
   str += file;
   path = str;
}
