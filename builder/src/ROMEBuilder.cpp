/********************************************************************

  ROMEBuilder.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include "RConfig.h"
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include "TSystem.h"
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "Riostream.h"
#include "ROMEBuilder.h"

void ROMEBuilder::StartBuilder()
{

   int i,j,k,l;
   bool found;
   ROMEString str;

   TString::MaxWaste(kTStringResizeIncrement-1);
   TString::ResizeIncrement(kTStringResizeIncrement);

   ReadXMLDefinitionFile();

   int tabNumber;
   int histoNumber;
   int is,ie,ind;
   // Check Histos & Tabs
   for (i=0;i<numOfTask;i++) {
      for (j=0;j<numOfHistos[i];j++) {
         for (k=0;k<numOfHistoTabs[i][j];k++) {
            found = false;
            for (l=0;l<numOfTab;l++) {
               if (histoTabName[i][j][k]==tabName[l]) {
                  found = true;
                  tabNumber = l;
                  histoNumber = numOfTabHistos[l];
                  break;
               }
            }
            if (!found) {
               if (numOfTab<0)
                  numOfTab = 0;
               tabName[numOfTab] = histoTabName[i][j][k];
               tabTitle[numOfTab] = histoTabName[i][j][k];
               tabAuthor[numOfTab] = mainAuthor;
               tabVersion[numOfTab] = "1";
               tabDescription[numOfTab] = "";
               numOfSteering[numOfTab+numOfTaskHierarchy+1] = -1;
               numOfMenu[numOfTab] = -1;
               tabNumOfChildren[numOfTab] = 0;
               numOfThreadFunctions[numOfTab] = 0;
               tabParentIndex[numOfTab] = taskHierarchyParentIndex[numOfTab] = -1;

               tabNumber = numOfTab;
               histoNumber = 0;
               numOfTabHistos[numOfTab] = 0;
               numOfTab++;
            }
            is = histoTabArrayIndex[i][j][k].ToInteger(); ie = is;
            if ((ind=histoTabArrayIndex[i][j][k].Index("-"))!=-1) {
               str = histoTabArrayIndex[i][j][k](0,ind);
               is = str.ToInteger();
               str = histoTabArrayIndex[i][j][k](ind+1,histoTabArrayIndex[i][j][k].Length()-ind-1);
               ie = str.ToInteger();
            }
            tabHistoName[tabNumber][histoNumber] = histoName[i][j];
            tabHistoIndex[tabNumber][histoNumber] = histoTabIndex[i][j][k].ToInteger();
            tabHistoArrayIndexStart[tabNumber][histoNumber] = is;
            tabHistoArrayIndexEnd[tabNumber][histoNumber] = ie;
            tabHistoTaskIndex[tabNumber][histoNumber] = i;
            tabHistoHistoIndex[tabNumber][histoNumber] = j;
            numOfTabHistos[tabNumber]++;
         }
      }
   }

   for (i=0;i<numOfTab;i++) {
      tabHistoIndexMax[i] = 0;
      for (j=0;j<numOfTabHistos[i];j++) {
         tabHistoIndexMax[i] = TMath::Max(tabHistoIndex[i][j]+1+tabHistoArrayIndexEnd[i][j]-tabHistoArrayIndexStart[i][j],tabHistoIndexMax[i]);
      }
   }

   // test for fortran
   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         haveFortranTask = true;
         break;
      }
   }
   if (!readGlobalSteeringParameters) {
      if (numOfTaskHierarchy==-1)
         numOfTaskHierarchy++;
      steerName[numOfTaskHierarchy][0] = "GlobalSteering";
      steerParent[numOfTaskHierarchy][0] = -1;
      numOfSteering[numOfTaskHierarchy] = 0;
   }

   // make directories
   ROMEString path;
   path.SetFormatted("%sobj",outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%sdict",outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%ssrc", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%sinclude", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%ssrc/generated", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   path.SetFormatted("%sinclude/generated", outDir.Data());
   gSystem->MakeDirectory(path.Data());
   if (numOfTask > 0) {
      path.SetFormatted("%ssrc/tasks", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      if (hasTaskUserCode) {
         path.SetFormatted("%sinclude/tasks", outDir.Data());
         gSystem->MakeDirectory(path.Data());
      }
   }
   if (numOfTab > 0) {
      path.SetFormatted("%ssrc/tabs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/tabs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }
   if (hasFolderUserCode) {
      path.SetFormatted("%ssrc/folders", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/folders", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }
   if (numOfDAQ > 0) {
      path.SetFormatted("%ssrc/daqs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/daqs", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }
   if (numOfDB > 0) {
      path.SetFormatted("%ssrc/databases", outDir.Data());
      gSystem->MakeDirectory(path.Data());
      path.SetFormatted("%sinclude/databases", outDir.Data());
      gSystem->MakeDirectory(path.Data());
   }

   // write classes
   if (makeOutput) 
      cout << "\n\nAnalyzer:" << endl;
   if (!WriteAnalyzerCpp()) return;
   if (!WriteAnalyzerH()) return;
   if (!WriteAnalyzerF()) return;
   if (makeOutput) 
      cout << "\n\nWindow:" << endl;
   if (!WriteWindowCpp()) return;
   if (!WriteWindowH()) return;
   if (makeOutput) 
      cout << "\n\nFolders:" << endl;
   if (!WriteFolderCpp()) return;
   if (!WriteFolderH()) return;
   if (makeOutput) 
      cout << "\n\nTasks:" << endl;
   if (!WriteTaskCpp()) return;
   if (!WriteTaskF()) return;
   if (!WriteTaskH()) return;
   if (makeOutput)
      cout << "\n\nTabs:" << endl;
   if (!WriteTabCpp()) return;
   if (!WriteTabH()) return;
   if (makeOutput) 
      cout << "\n\nUser DAQ Systems:" << endl;
   if (!WriteDAQCpp()) return;
   if (!WriteDAQH()) return;
   if (makeOutput) 
      cout << "\n\nUser Database Interfaces:" << endl;
   if (!WriteDBCpp()) return;
   if (!WriteDBH()) return;
   if (makeOutput) 
      cout << "\n\nFramework:" << endl;
   if (!WriteSteering(numOfTaskHierarchy)) return;
   if (!AddConfigParameters()) return;
   if (!WriteConfigCpp()) return;
   if (!WriteConfigH()) return;
   if (!WriteMidasDAQCpp()) return;
   if (!WriteMidasDAQH()) return;
   if (!WriteRomeDAQCpp()) return;
   if (!WriteRomeDAQH()) return;
   if (!WriteDataBaseDAQCpp()) return;
   if (!WriteDataBaseDAQH()) return;
   if (!WriteEventLoopCpp()) return;
   if (!WriteEventLoopH()) return;
   if (!WriteMain()) return;
   if (!WriteReadTreesC()) return;

   ROMEString buffer;
   gSystem->ChangeDirectory(outDir.Data());

// Linking
   if (makeOutput && !noLink) cout << "\nLinking " << shortCut.Data() << " Project." << endl;
   WriteMakefile();
   if (!WriteLinkDefHs()) return;
   if (noLink) {
      ROMEString tempStr;
#if defined( R__UNIX )
      tempStr.SetFormatted("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      tempStr.SetFormatted("nmake -f Makefile.win");
#endif
      tempStr.AppendFormatted(" dict/ROMEDict.cpp dict/ARGUSDict.cpp dict/%sGeneratedDict.cpp",shortCut.Data());
      if (hasFolderGenerated)
         tempStr.AppendFormatted(" dict/%sGeneratedFolderDict.cpp",shortCut.Data());
      if (hasFolderUserCode)
         tempStr.AppendFormatted(" dict/%sFolderDict.cpp",shortCut.Data());
      if (hasTaskGenerated)
         tempStr.AppendFormatted(" dict/%sGeneratedTaskDict.cpp",shortCut.Data());
      if (hasTaskUserCode)
         tempStr.AppendFormatted(" dict/%sTaskDict.cpp",shortCut.Data());
      if (numOfTab>0)
         tempStr.AppendFormatted(" dict/%sGeneratedTabDict.cpp",shortCut.Data());
      if (numOfTab>0)
         tempStr.AppendFormatted(" dict/%sTabDict.cpp",shortCut.Data());
      if (numOfMFDictHeaders>0)
         tempStr.AppendFormatted(" dict/%sUserDict.cpp",shortCut.Data());
      gSystem->Exec(tempStr.Data());
   }
   else {
#if defined( R__UNIX )
      gSystem->Exec("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      gSystem->Exec("nmake -f Makefile.win");
#endif
   }
// Documentation
   if (makeOutput) cout << "\nWrite HTML Documentation." << endl;
   WriteHTMLDoku();
#if (ROOT_VERSION_CODE < ROOT_VERSION(4,0,0))
   cout << endl << endl;
   cout << "******************* WARNING *******************" << endl;
   cout << "You are using a version of ROOT which does not" << endl;
   cout << "provide all the functionality that ROME needs!" << endl;
   cout << endl;
   cout << "However, ROME will work fine." << endl;
   cout << "But you can not use the following features:" << endl;
#if (ROOT_VERSION_CODE < ROOT_VERSION(4,1,0))
   cout << endl;
   cout << " - Circular trees (e.g. MaxNumberOfEntries>0)" << endl;
   cout << " - The socket interface" << endl;
   cout << endl;
   cout << " --> All of the above are available with version 4.01/00" << endl;
   cout << endl;
#endif // 4.01/00
   cout << endl;
#endif // ROOT_VERSION

}

Bool_t ROMEBuilder::ReadCommandLineParameters(int argc, char *argv[])
{
   makeOutput = false;
   noLink = false;
   midas = false;
   orca = false;
   sql = false;
   mysql = false;
   pgsql = false;
   sqlite = false;
   sqlite3 = false;
   noVP = false;

   char workDir[kMAXPATHLEN];
   strcpy(workDir,gSystem->WorkingDirectory());

   outDir = workDir;
   outDir.Append("/");

   if (argc==1) {
      Usage();
      return false;
   }
   for (int i=1;i<argc;i++) {
      // -- only for testing (start) --
      if (!strcmp(argv[i],"-meg")) {
         makeOutput = false;
         midas = true;
         noLink = true;
         sql = true;
         mysql = true;
         outDir = "C:/meg/meganalyzer/";
         xmlFile = "C:/meg/meganalyzer/MEGAnalyzer.xml";
/*         flags.AddAtAndExpand("HAVE_DRS",0);
         flags.AddAtAndExpand("HAVE_MSCB",1);
         flags.AddAtAndExpand("HAVE_USB",2);
         flags.AddAtAndExpand("HAVE_AFG3251",3);
         flags.AddAtAndExpand("HAVE_VME",4);*/
      }
      else if (!strcmp(argv[i],"-multi")) {
         noLink = true;
         outDir = "C:/rome/examples/multirun/";
         xmlFile = "C:/rome/examples/multirun/multirun.xml";
      }
      else if (!strcmp(argv[i],"-netfolder")) {
         noLink = true;
         outDir = "C:/rome/examples/netfolder/";
         xmlFile = "C:/rome/examples/netfolder/netfolder.xml";
      }
      else if (!strcmp(argv[i],"-histogui")) {
         noLink = true;
         outDir = "C:/rome/examples/histogui/";
         xmlFile = "C:/rome/examples/histogui/histogui.xml";
      }
      else if (!strcmp(argv[i],"-lp")) {
         makeOutput = true;
         midas = true;
         noLink = true;
         outDir = "C:/lpframework/";
         xmlFile = "C:/lpframework/lpframework.xml";
      }
      // -- only for testing (end) --
      else if (!strcmp(argv[i],"-v")) {
         makeOutput = true;
      }
      else if (!strcmp(argv[i],"-nvp")) {
         noVP = true;
      }
      else if (!strcmp(argv[i],"-nl")) {
         noLink = true;
      }
      else if (!strcmp(argv[i],"-nosql")) {
         cout<<"-nosql is obsolete. SQL support is off by default."<<endl;
      }
      else if (!strcmp(argv[i],"-mysql")) {
         mysql = true;
      }
      else if (!strcmp(argv[i],"-pgsql")) {
         pgsql = true;
      }
      else if (!strcmp(argv[i],"-sqlite")) {
         sqlite = true;
      }
      else if (!strcmp(argv[i],"-sqlite3")) {
         sqlite3 = true;
      }
      else if (!strcmp(argv[i],"-midas")) {
         midas = true;
         ROMEString midasFile;
         midasFile = getenv("MIDASSYS");
         midasFile.Append("/include/midas.h");
         if (gSystem->AccessPathName(midasFile.Data(),kFileExists)) {
            cout << "Midas library not found. Have you set the MIDASSYS environment variable ?" << endl;
            return false;
         }
      }
      else if (!strcmp(argv[i],"-orca")) {
         orca = true;
      }
      else if (!strcmp(argv[i],"-f")&&i<argc-1) {
         i++;
         int j=0;
         while (argv[i][0]!='-') {
            flags.AddAtAndExpand((const char*)argv[i],j);
            if (flags[j][0]=='-' || flags[j][0]=='/')
               flags[j] = flags[j](2,flags[j].Length());
            i++;j++;
            if (i>argc-1)
               break;
         }
         i--;
      }
      else if (!strcmp(argv[i],"-i")&&i<argc-1) {
         xmlFile = argv[i+1];
         i++;
      }
      else if (!strcmp(argv[i],"-o")&&i<argc-1) {
         outDir = argv[i+1];
         if (outDir[outDir.Length()-1]!='/' && outDir[outDir.Length()-1]!='\\') 
            outDir.Append("/");
         i++;
      }
      else if (argv[i][0]=='-') {
         Usage();
         return false;
      }
      else {
         xmlFile = argv[i];
      }
   }

   sql = (mysql || pgsql || sqlite || sqlite3 );

   return true;
}

Bool_t ROMEBuilder::CheckFileAndPath() 
{
   if (gSystem->AccessPathName(xmlFile.Data(),kFileExists)) {
      if ( xmlFile == "")
         cout << "No inputfile specified." << endl;
      else
         cout << "Inputfile '" << xmlFile.Data() << "' not found." << endl;
      return false;
   }
   ROMEString path;
   path = outDir;
   path.Remove(path.Length()-1);
   if (gSystem->AccessPathName(path.Data(),kFileExists)) {
      cout << "Outputpath '" << outDir.Data() << "' not found." << endl;
      return false;
   }
   return true;
}

void ROMEBuilder::Usage()
{
   cout << "  -i        Inputfile" << endl;
   cout << "  -o        Outputfile path" << endl;
   cout << "  -v        Verbose Mode (no Argument)" << endl;
   cout << "  -nl       No Linking (no Argument)" << endl;
   cout << "  -midas    Generated program can be connected to a midas online system (no Argument)" << endl;
   cout << "  -orca     Generated program can be connected to a orca DAQ system (no Argument)" << endl;
   cout << "  -mysql    Generated program can be connected to a MySQL server (no Argument)" << endl;
   cout << "  -pgsql    Generated program can be connected to a PostgreSQL server (no Argument)" << endl;
   cout << "  -sqlite   Generated program can be connected to a SQLite database (no Argument)" << endl;
   cout << "  -sqlite3  Generated program can be connected to a SQLite3 database (no Argument)" << endl;
   cout << "  -f        Compile flags added to the Makefile" << endl;
}

void ROMEBuilder::AnalyzeFileName(const char* file,ROMEString& pathOfFile,ROMEString& nameOfFile,ROMEString& extensionOfFile)
{
   Ssiz_t ind;
   ROMEString str = file;
   ind = TMath::Max(str.Last('/'),str.Last('\\'));
   pathOfFile = str(0,ind+1);
   str = str(ind+1,str.Length()-ind-1);
   ind = str.Last('.');
   if (ind==-1) {
      nameOfFile = str;
      extensionOfFile = "";
   }
   else {
      nameOfFile = str(0,ind);
      extensionOfFile = str(ind+1,str.Length()-ind-1);
   }
}

Bool_t ROMEBuilder::WriteFile(const char* filename,const char* body,Int_t nspace, Bool_t backup)
{
   // return true when backup file is created
   fstream *fileStream;
   ROMEString fileBuffer;
   bool backupCreated = false;

   if ((fileStream = new fstream(filename,ios::in))) {
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
   }

   if (fileBuffer != body) {
      if (backup)
         backupCreated = BackUpFile(filename);
      if (!(fileStream = new fstream(filename,ios::out | ios::trunc))) {
         if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
         return backupCreated;
      }
      if (makeOutput) cout << setw(nspace)<< "" << filename << endl;
      *fileStream<<body;
      delete fileStream;
   }
   return backupCreated;
}

void ROMEBuilder::GetFormat(ROMEString* buf,const char *type)
{
   buf->Resize(0);
   // get the format specifier (like '%s') of a declaration type
   if (!strcmp(type,"int") ||
       !strcmp(type,"unsigned int") ||
       !strcmp(type,"Int_t") ||
       !strcmp(type,"UInt_t") ||

       !strcmp(type,"long") ||
       !strcmp(type,"unsigned long") ||
       !strcmp(type,"Long_t") ||
       !strcmp(type,"ULong_t") ||

       !strcmp(type,"short") ||
       !strcmp(type,"unsigned short") ||
       !strcmp(type,"Short_t") ||
       !strcmp(type,"UShort_t") ||

       !strcmp(type,"long long") ||
       !strcmp(type,"unsigned long long") ||

       !strcmp(type,"Style_t") ||
       !strcmp(type,"Marker_t") ||
       !strcmp(type,"Color_t") ||
       !strcmp(type,"Font_t") ||
       !strcmp(type,"Version_t")) {
      buf->Append("%d");
   }
   else if (!strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      buf->Append("%s");
   }
   else if (!strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      buf->Append("%g");
   }
   else if (!strcmp(type,"bool") ||
      !strcmp(type,"Bool_t")) {
      buf->Append("%d");
   }
   else {
      buf->Append("%s");
   }
}

void ROMEBuilder::setValue(ROMEString* buf,const char *destination,const char *source,const char *type,Int_t version)
{
   buf->Resize(0);
   // returns code which transformes a source variable of any type into a destination variable of type character
   if (
       !strcmp(type,"int") ||
       !strcmp(type,"unsigned int") ||
       !strcmp(type,"Int_t") ||
       !strcmp(type,"UInt_t") ||

       !strcmp(type,"long") ||
       !strcmp(type,"unsigned long") ||
       !strcmp(type,"Long_t") ||
       !strcmp(type,"ULong_t") ||

       !strcmp(type,"short") ||
       !strcmp(type,"unsigned short") ||
       !strcmp(type,"Short_t") ||
       !strcmp(type,"UShort_t") ||

       !strcmp(type,"long long") ||
       !strcmp(type,"unsigned long long") ||

       !strcmp(type,"Style_t") ||
       !strcmp(type,"Marker_t") ||
       !strcmp(type,"Color_t") ||
       !strcmp(type,"Font_t") ||
       !strcmp(type,"Version_t")) {
      if (version==0)
         buf->AppendFormatted("%s = strtol(%s,&cstop,10)",destination,source);
      else
         buf->AppendFormatted("strtol(%s,&cstop,10)",source);
   }
   else if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      if (version==0)
         buf->AppendFormatted("%s = gAnalyzer->strtobool(%s)",destination,source);
      else
         buf->AppendFormatted("gAnalyzer->strtobool(%s)",source);
   }
   else if (
       !strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      if (version==0)
         buf->AppendFormatted("strcpy(%s,%s)",destination,source);
      else
         buf->AppendFormatted("%s",source);
   }
   else if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||
       !strcmp(type,"Double32_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      if (version==0)
         buf->AppendFormatted("%s = strtod(%s,&cstop)",destination,source);
      else
         buf->AppendFormatted("strtod(%s,&cstop)",source);
   }
   else {
      if (version==0)
         buf->AppendFormatted("%s = %s",destination,source);
      else
         buf->AppendFormatted("%s",source);
   }
}

Bool_t ROMEBuilder::isNumber(const char* str)
{
   if (strcmp(str,"float")&&strcmp(str,"Float_t")&&
       strcmp(str,"double")&&strcmp(str,"Double_t")&&strcmp(str,"Double32_t")&&
       strcmp(str,"int")&&strcmp(str,"Int_t")&&
       strcmp(str,"unsigned int")&&strcmp(str,"UInt_t")&&
       strcmp(str,"long")&&strcmp(str,"Long_t")&&
       strcmp(str,"unsigned long")&&strcmp(str,"ULong_t")&&
       strcmp(str,"short")&&strcmp(str,"Short_t")&&
       strcmp(str,"unsigned short")&&strcmp(str,"UShort_t")&&
       strcmp(str,"Long64_t")&&strcmp(str,"ULong64_t")&&
       strcmp(str,"long long")&&strcmp(str,"unsigned long long"))
      return false;
   return true;
}

Bool_t ROMEBuilder::isFloatingType(const char *type)
{
   if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||
       !strcmp(type,"Double32_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      return true;
   }
   return false;
}

Bool_t ROMEBuilder::isBoolType(const char *type)
{
   if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      return true;
   }
   return false;
}

Bool_t ROMEBuilder::isIntType(const char *type)
{
// check if the type is valid for array length specifier in TTree.
// unsigned, char, short, Long64_t are not valid.
   return (
      !strcmp(type,"int")
      || !strcmp(type,"Int_t")
      );
}

ROMEString& ROMEBuilder::convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer)
{
   int type = 0;
   if (!strcmp(newType,"int") || !strcmp(newType,"Int_t"))
      type = 1;
   else if (!strcmp(newType,"double") || !strcmp(newType,"Double_t"))
      type = 2;
   else if (!strcmp(newType,"ROMEString&"))
      type = 3;
   else
      return stringBuffer;
   if (type==1 || type==2) {
      if (isNumber(oldType) || isBoolType(oldType)) {
         if (type==1)
            return stringBuffer.SetFormatted("(int)%s",value);
         if (type==2)
            return stringBuffer.SetFormatted("(double)%s",value);
      }
      else {
         if (type==1)
            stringBuffer.SetFormatted("strtol(%s,&cstop,10)",value);
         if (type==2)
            stringBuffer.SetFormatted("strtod(%s,&cstop)",value);
         if (!strcmp(oldType,"std::string"))
            stringBuffer.Insert(strlen(value)+7,".c_str()");
         else if (!strcmp(oldType,"ROMEString") || !strcmp(oldType,"TString"))
            stringBuffer.Insert(strlen(value)+7,".Data()");
         return stringBuffer;
      }
   }
   else if (type==3) {
      if (isBoolType(oldType))
         return stringBuffer;
      ROMEString tmp = stringBuffer;
      if (!strcmp(oldType,"std::string"))
         return stringBuffer.SetFormatted("%s=%s.c_str()",tmp.Data(),value);
      if (!strcmp(oldType,"ROMEString") || !strcmp(oldType,"TString"))
         return stringBuffer.SetFormatted("%s=%s",tmp.Data(),value);
      if (isFloatingType(oldType))
         return stringBuffer.SetFormatted("%s.SetFormatted(\"%%f\",%s)",tmp.Data(),value);
      else
         return stringBuffer.SetFormatted("%s.SetFormatted(\"%%d\",%s)",tmp.Data(),value);
   }
   return stringBuffer;
}

Bool_t ROMEBuilder::isFolder(const char *type)
{
   int j;
   ROMEString str;
   for (j=0;j<numOfFolder;j++) {
      if (!strcmp(type,folderName[j].Data()))
         return true;
      str.SetFormatted("%s*",folderName[j].Data());
      if (!strcmp(type,str.Data()))
         return true;
      str.SetFormatted("%s%s*",shortCut.Data(),folderName[j].Data());
      if (!strcmp(type,str.Data()))
         return true;
   }
   return false;
}

Bool_t ROMEBuilder::isTArrayType(const char *type)
{
   int j;
   ROMEString str;
   const char arrayTypes[][8]
      = {"TArrayC"
         ,"TArrayI"
         ,"TArrayD"
         ,"TArrayL"
         ,"TArrayF"
         ,"TArrayS"
      };

   for (j=0;j<6;j++) {
      if (!strcmp(type,arrayTypes[j]))
         return true;
      str.SetFormatted("%s*",arrayTypes[j]);
      if (!strcmp(type,str.Data()))
         return true;
   }
   return false;
}

const char* ROMEBuilder::TArray2StandardType(const char *type,ROMEString &standardType)
{
   int j;
   ROMEString str;
   const char arrayTypes[][2][9]
      = { {"TArrayC","Char_t  "}
         ,{"TArrayI","Int_t   "}
         ,{"TArrayD","Double_t"}
         ,{"TArrayL","Long_t  "}
         ,{"TArrayF","Float_t "}
         ,{"TArrayS","Short_t "}
      };

   for (j=0;j<6;j++) {
      if (!strcmp(type,arrayTypes[j][0])) {
         standardType = arrayTypes[j][1];
         return standardType.Data();
      }
      str.SetFormatted("%s*",(char*)arrayTypes[j]);
      if (!strcmp(type,str.Data())) {
         standardType = arrayTypes[j][1];
         return standardType.Data();
      }
   }
   return type;
}

