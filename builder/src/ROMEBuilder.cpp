/********************************************************************

  ROMEBuilder.cpp, M. Schneebeli PSI

  $Log$
  Revision 1.56  2004/10/08 12:21:11  schneebeli_m
  Changed compile order

  Revision 1.55  2004/10/08 12:03:17  schneebeli_m
  Changed XML format, included a rome.xsd schema file

  Revision 1.54  2004/10/05 14:01:16  schneebeli_m
  Circular Trees

  Revision 1.53  2004/10/05 13:30:32  schneebeli_m
  make -e, Port number

  Revision 1.52  2004/10/05 07:52:44  schneebeli_m
  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed

  Revision 1.51  2004/10/01 13:11:33  schneebeli_m
  Tree write error removed, Database Number Problem solved, Trees in Folder for TSocket

  Revision 1.50  2004/09/30 13:08:21  schneebeli_m
  ...

  Revision 1.49  2004/09/30 10:25:03  schneebeli_m
  gAnalyzer and gROME

  Revision 1.48  2004/09/30 09:50:29  schneebeli_m
  Samples updated

  Revision 1.47  2004/09/30 08:45:28  schneebeli_m
  Samples updated

  Revision 1.46  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

  Revision 1.45  2004/09/23 02:48:10  schneebeli_m
  bug

  Revision 1.44  2004/09/23 02:43:04  schneebeli_m
  processline fo socket

  Revision 1.43  2004/09/22 00:15:10  schneebeli_m
  TRint & Socket

  Revision 1.42  2004/09/15 18:56:14  schneebeli_m
  some stuff

  Revision 1.41  2004/09/08 13:04:08  schneebeli_m
  online experiment specifier

  Revision 1.40  2004/09/08 10:09:50  schneebeli_m
  online host

  Revision 1.39  2004/09/08 09:07:12  schneebeli_m
  minor bugs

  Revision 1.38  2004/09/02 12:39:35  schneebeli_m
  minor problems

  Revision 1.37  2004/08/06 11:39:51  schneebeli_m
  TSocket

  Revision 1.36  2004/08/03 12:34:55  schneebeli_m
  ...

  Revision 1.35  2004/08/02 16:02:33  schneebeli_m
  Bugs removed

  Revision 1.34  2004/08/02 15:58:21  schneebeli_m
  Bugs removed

  Revision 1.33  2004/08/02 15:31:52  schneebeli_m
  Bugs removed

  Revision 1.32  2004/08/02 15:20:39  schneebeli_m
  Bugs removed

  Revision 1.31  2004/08/02 12:14:35  schneebeli_m
  ...

  Revision 1.30  2004/08/02 07:28:55  schneebeli_m
  linking on linux

  Revision 1.29  2004/07/30 15:33:17  schneebeli_m
  implemented ROMEString

  Revision 1.28  2004/07/28 07:32:11  schneebeli_m
  online stuff

  Revision 1.27  2004/07/20 16:18:41  schneebeli
  minor errors

  Revision 1.26  2004/07/19 13:32:22  schneebeli
  minor stuff

  Revision 1.25  2004/07/19 12:45:11  schneebeli
  romoved errors in online mode

  Revision 1.24  2004/07/15 10:23:39  schneebeli
  running on linux

  Revision 1.23  2004/07/15 10:21:19  schneebeli
  running on linux

  Revision 1.22  2004/07/15 09:13:36  schneebeli
  running on linux

  Revision 1.21  2004/07/15 08:53:37  schneebeli
  running on linux

  Revision 1.20  2004/07/14 13:39:04  schneebeli
  database write

  Revision 1.19  2004/07/13 16:07:00  schneebeli
  memory overflow (roottree), romexml

  Revision 1.18  2004/07/09 17:47:51  schneebeli
  directory structure added

  Revision 1.17  2004/07/09 16:08:36  schneebeli
  don't display empty histo folders

  Revision 1.16  2004/07/09 14:43:44  schneebeli
  removed minor errors

  Revision 1.15  2004/07/09 14:29:34  schneebeli
  removed minor errors

  Revision 1.14  2004/07/09 12:39:31  schneebeli
  error in task generation

  Revision 1.13  2004/07/08 08:19:36  schneebeli
  write midas event header to folder

  Revision 1.12  2004/07/07 14:48:22  schneebeli
  io : root trees

  Revision 1.11  2004/07/06 08:01:08  schneebeli
  lots of stuff changed

  Revision 1.10  2004/06/30 15:32:12  schneebeli
  ms

  Revision 1.9  2004/06/29 10:27:54  schneebeli
  double declaration of int i

  Revision 1.8  2004/06/28 14:41:20  schneebeli
  lots of new stuff

  Revision 1.7  2004/06/18 08:29:47  midas
  Added file header


********************************************************************/


#if defined( _MSC_VER )
#include <io.h>
#include <direct.h>
#endif
#if defined ( __linux__ )
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/io.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <float.h>
#include <TMath.h>
#include "ROMEString.h"
#include "ROMEBuilder.h"
#include <Riostream.h>

//MEGFrameWork.xml -o C:/Data/analysis/MEG/ROME/MEGFrameWork/ -v -nl
//MEGDriftChamber.xml -o C:/Data/analysis/MEG/ROME/MEGDriftChamber/ -v -nl
//MEGTest.xml -o C:/Data/analysis/MEG/ROME/MEGTest/ -v -nl



bool ROMEBuilder::ReadXMLFolder() {
   // read the folder definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i,j;
   ROMEString currentFolderName = "";
   int currentNumberOfFolders;

   // count folders
   numOfFolder++;
   currentNumberOfFolders = numOfFolder;
   if (numOfFolder>=maxNumberOfFolders) {
      cout << "Maximal number of folders reached : " << maxNumberOfFolders << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   folderName[numOfFolder] = "";
   folderTitle[numOfFolder] = "";
   folderArray[numOfFolder] = "1";
   folderDataBase[numOfFolder] = false;
   folderUserCode[numOfFolder] = false;
   folderVersion[numOfFolder] = "1";
   folderDescription[numOfFolder] = "";
   folderAuthor[numOfFolder] = mainAuthor;
   numOfFolderInclude[numOfFolder] = 0;
   numOfValue[numOfFolder] = 0;

   // set parent
   folderParentName[numOfFolder] = parent[recursiveDepth];
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subfolder
      if (type == 1 && !strcmp((const char*)name,"Folder")) {
         // set folder as parent for subsequent folders
         recursiveDepth++;
         parent[recursiveDepth] = folderName[numOfFolder].Data();
         // read subfolder
         if (!ReadXMLFolder()) 
            return false;
         continue;
      }
      // end folder
      if (type == 15 && !strcmp((const char*)name,"Folder")) {
         // check input
         if (currentFolderName=="") {
            cout << "The " << (currentNumberOfFolders+1) << ". Folder has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         recursiveDepth--;
         return true;
      }
      // folder name
      if (type == 1 && !strcmp((const char*)name,"FolderName")) {
         xml->GetValue(folderName[numOfFolder],folderName[numOfFolder]);
         currentFolderName = folderName[numOfFolder];
         // output
         if (makeOutput) for (i=0;i<recursiveDepth;i++) cout << "   ";
         if (makeOutput) folderName[numOfFolder].WriteLine();
      }
      // folder title
      if (type == 1 && !strcmp((const char*)name,"FolderTitle"))
         xml->GetValue(folderTitle[numOfFolder],folderTitle[numOfFolder]);
      // folder array size
      if (type == 1 && !strcmp((const char*)name,"ArraySize"))
         xml->GetValue(folderArray[numOfFolder],folderArray[numOfFolder]);
      // folder data base access
      if (type == 1 && !strcmp((const char*)name,"DataBaseAccess")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true") 
            folderDataBase[numOfFolder] = true;
      }
      // folder with changeble class file
      if (type == 1 && !strcmp((const char*)name,"ChangeableClassFile")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true") 
            folderUserCode[numOfFolder] = true;
      }
      // folder version
      if (type == 1 && !strcmp((const char*)name,"FolderVersion"))
         xml->GetValue(folderVersion[numOfFolder],folderVersion[numOfFolder]);
      // folder description
      if (type == 1 && !strcmp((const char*)name,"FolderDescription"))
         xml->GetValue(folderDescription[numOfFolder],folderDescription[numOfFolder]);
      // folder author
      if (type == 1 && !strcmp((const char*)name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // author name
            if (type == 1 && !strcmp((const char*)name,"AuthorName"))
               xml->GetValue(folderAuthor[numOfFolder],folderAuthor[numOfFolder]);
            if (type == 15 && !strcmp((const char*)name,"Author"))
               break;
         }
         continue;
      }
      // folder include
      if (type == 1 && !strcmp((const char*)name,"Include")) {
         // include initialisation
         folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]] = "";
         folderLocalFlag[numOfFolder][numOfFolderInclude[numOfFolder]] = false;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include name
            if (type == 1 && !strcmp((const char*)name,"IncludeName"))
               xml->GetValue(folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]],folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]]);
            // include type
            if (type == 1 && !strcmp((const char*)name,"IncludeType")) {
               xml->GetValue(tmp,"false");
               if (tmp == "local") 
                  folderLocalFlag[numOfFolder][numOfFolderInclude[numOfFolder]] = true;
            }
            // include end
            if (type == 15 && !strcmp((const char*)name,"Include"))
               break;
         }
         // check input
         if (folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]]=="") {
            cout << "An Include of Folder '" << folderName[numOfFolder].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // count includes
         numOfFolderInclude[numOfFolder]++;
         if (numOfFolderInclude[numOfFolder]>=maxNumberOfInclude) {
            cout << "Maximal number of inludes in folder '" << folderName[numOfFolder].Data() << "' reached : " << maxNumberOfInclude << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
      // folder field
      if (type == 1 && !strcmp((const char*)name,"Field")) {
         // field initialisation
         bool readName = false;
         bool readType = false;
         valueName[numOfFolder][numOfValue[numOfFolder]] = "";
         valueType[numOfFolder][numOfValue[numOfFolder]] = "";
         valueComment[numOfFolder][numOfValue[numOfFolder]] = "";
         valueArray[numOfFolder][numOfValue[numOfFolder]] = "0";
         valueDataBasePath[numOfFolder][numOfValue[numOfFolder]] = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // field name
            if (type == 1 && !strcmp((const char*)name,"FieldName")) {
               readName = true;
               xml->GetValue(valueName[numOfFolder][numOfValue[numOfFolder]],valueName[numOfFolder][numOfValue[numOfFolder]]);
            }
            // field type
            if (type == 1 && !strcmp((const char*)name,"FieldType")) {
               readType = true;
               xml->GetValue(valueType[numOfFolder][numOfValue[numOfFolder]],valueType[numOfFolder][numOfValue[numOfFolder]]);
               if (valueType[numOfFolder][numOfValue[numOfFolder]] == "TString")
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "' '";
               else if (valueType[numOfFolder][numOfValue[numOfFolder]] == "TRef")
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "NULL";
               else
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "0";
            }
            // field initialization
            if (type == 1 && !strcmp((const char*)name,"FieldInitialization")) {
               if (!readName) {
                  cout << "Please specify a field name befor the initial value in the " << (numOfValue[numOfFolder]+1) << ".field in folder '" << folderName[numOfFolder].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               if (!readType) {
                  cout << "Please specify a field type befor the initial value in field '" << valueName[numOfFolder][numOfValue[numOfFolder]].Data() << "' in folder  '" << folderName[numOfFolder].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               xml->GetValue(valueName[numOfFolder][numOfValue[numOfFolder]],valueName[numOfFolder][numOfValue[numOfFolder]]);
            }
            // field comment
            if (type == 1 && !strcmp((const char*)name,"FieldComment")) {
               xml->GetValue(valueComment[numOfFolder][numOfValue[numOfFolder]],valueComment[numOfFolder][numOfValue[numOfFolder]]);
               if (valueComment[numOfFolder][numOfValue[numOfFolder]][0]!='/') {
                  valueComment[numOfFolder][numOfValue[numOfFolder]].Insert(0,"// ");
               }
            }
            // field array size
            if (type == 1 && !strcmp((const char*)name,"ArraySize"))
               xml->GetValue(valueArray[numOfFolder][numOfValue[numOfFolder]],valueArray[numOfFolder][numOfValue[numOfFolder]]);
            // field data base path
            if (type == 1 && !strcmp((const char*)name,"DataBasePath"))
               xml->GetValue(valueDataBasePath[numOfFolder][numOfValue[numOfFolder]],valueDataBasePath[numOfFolder][numOfValue[numOfFolder]]);
            // field end
            if (type == 15 && !strcmp((const char*)name,"Field"))
               break;
         }
         // check input
         if (valueName[numOfFolder][numOfValue[numOfFolder]]=="") {
            cout << "A Field of Folder '" << folderName[numOfFolder].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (valueType[numOfFolder][numOfValue[numOfFolder]]=="") {
            cout << "Field '" << valueName[numOfFolder][numOfValue[numOfFolder]].Data() << "' of Folder '" << folderName[numOfFolder].Data() << "' has no Type !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         for (i=0;i<numOfValue[numOfFolder];i++) {
            for (j=i+1;j<numOfValue[numOfFolder];j++) {
               if (valueName[numOfFolder][i]==valueName[numOfFolder][j]) {
                  cout << "\nFolder '" << folderName[numOfFolder].Data() << "' has two fields with the name '" << valueName[numOfFolder][i].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         // count fields
         numOfValue[numOfFolder]++;
         if (numOfValue[numOfFolder]>=maxNumberOfValues) {
            cout << "Maximal number of fields in folder '" << folderName[numOfFolder].Data() << "' reached : " << maxNumberOfValues << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
   }
   return true;
}

bool ROMEBuilder::WriteFolderCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb,ll,i,lenTot;
   char *pos;
   int fileHandle;
   ROMEString format;
   bool writeFile = false;
   char *pBuffer;
   int bufferLen=0;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (numOfValue[iFold] == 0) continue;

      // File name
      cppFile.SetFormatted("%s/src/framework/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());

      if (!folderUserCode[iFold]) {
         remove(cppFile.Data());
         continue;
      }

      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("//// Author: %s\n",folderAuthor[iFold].Data());
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      ll = 74-shortCut.Length();
      format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
      buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("//                                                                            //\n");
      pos = (char*)folderDescription[iFold].Data();
      lenTot = folderDescription[iFold].Length();
      while (pos-folderDescription[iFold].Data() < lenTot) {
         if (lenTot+(folderDescription[iFold].Data()-pos)<74) 
            i=TMath::Min(75,lenTot);
         else 
            for (i=74;pos[i]!=32&&i>0;i--) {}
         if (i<=0) 
            i=TMath::Min(75,lenTot);
         pos[i] = 0;
         buffer.AppendFormatted("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");

      // Write File
      struct stat buf;
      if( stat( cppFile.Data(), &buf )) {
         // Header Files
         buffer.AppendFormatted("\n\n#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[iFold].Data());

         buffer.AppendFormatted("\nClassImp(%s%s)\n",shortCut.Data(),folderName[iFold].Data());

         writeFile = true;
      }
      else {
         // compare old and new file
         fileHandle = open(cppFile.Data(),O_RDONLY);
         nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
         pBuffer = fileBuffer;
         char *pend = "/////////////////////////////////////----///////////////////////////////////////";
         pBuffer = strstr(pBuffer,pend);
         if (pBuffer==NULL||nb-(pBuffer-fileBuffer)<0) {
            if (makeOutput) cout << "\n\nError : File '" << cppFile.Data() << "' has an invalid header !!!" << endl;
            continue;
         }
         bufferLen = nb-(pBuffer-fileBuffer);
         close(fileHandle);
         writeFile = false;
         for (i=0;i<pBuffer-fileBuffer;i++) {
            if (buffer[i] != fileBuffer[i]) {
               writeFile = true;
               break;
            }
         }
         fileBuffer[nb] = 0;
         format.SetFormatted("%%-%d.%ds",bufferLen-80,bufferLen-80);
         buffer.AppendFormatted((char*)format.Data(),pBuffer+80);
      }
      if (writeFile) {
         // write file
         fileHandle = open(cppFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(cppFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << cppFile.Data() << endl;

         nb = write(fileHandle,buffer.Data(), buffer.Length());

         close(fileHandle);
      }
   }
   return true;
}

bool ROMEBuilder::WriteFolderH() {
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb,j,i;
   ROMEString str;
   int fileHandle;
   ROMEString format;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (numOfValue[iFold] == 0) continue;

      // File name
      if (folderUserCode[iFold])
         hFile.SetFormatted("%s/include/framework/%s%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      else
         hFile.SetFormatted("%s/include/framework/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());

      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

      // Header
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("#ifndef %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_Base_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }
      else {
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }

      buffer.AppendFormatted("#include <TObject.h>\n");

      for (i=0;i<numOfFolderInclude[iFold];i++) {
         if (folderLocalFlag[iFold][i]) {
            buffer.AppendFormatted("#include \"%s\"\n",folderInclude[iFold][i].Data());
         }
         else {
            buffer.AppendFormatted("#include <%s>\n",folderInclude[iFold][i].Data());
         }
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i]=="TString") {
            buffer.AppendFormatted("#include <%s.h>\n",valueType[iFold][i].Data());
            break;
         }
      }

      for (i=0;i<numOfValue[iFold];i++) {
         for (j=0;j<numOfFolder;j++) {
            str.SetFormatted("%s*",folderName[j].Data());
            if (valueType[iFold][i]==folderName[j] || valueType[iFold][i]==str) {
               buffer.AppendFormatted("#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[j].Data());
               valueType[iFold][i].Insert(0,shortCut);
               break;
            }
         }
      }

      // Class
      if (folderUserCode[iFold])
         buffer.AppendFormatted("\nclass %s%s_Base : public TObject\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("\nclass %s%s : public TObject\n",shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("{\n");

      // Fields
      buffer.AppendFormatted("protected:\n");
      int typeLen = 5;
      int nameLen = 8;
      int nameLenT = 0;
      for (i=0;i<numOfValue[iFold];i++) {
         if (typeLen<valueType[iFold][i].Length()) typeLen = valueType[iFold][i].Length();
         if (valueArray[iFold][i]=="0")
            nameLenT = (int)valueName[iFold][i].Length();
         else
            nameLenT = (int)(valueName[iFold][i].Length()+2+valueArray[iFold][i].Length());
         if (nameLen<nameLenT) nameLen = nameLenT;
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]!="0") {
            format.SetFormatted("   %%-%ds %%s[%s];%%%ds %%s\n",typeLen,valueArray[iFold][i].Data(),nameLen-valueName[iFold][i].Length()+2+valueArray[iFold][i].Length());
            buffer.AppendFormatted((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
         }
         else {
            format.SetFormatted("   %%-%ds %%s;%%%ds %%s\n",typeLen,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormatted((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("\n");
      format.SetFormatted("   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-8);
      buffer.AppendFormatted((char*)format.Data(),"Bool_t","Modified","","//! Modified Folder Flag");
      buffer.AppendFormatted("\n");

      // Methods

      buffer.AppendFormatted("public:\n");
      // Constructor
      if (folderUserCode[iFold])
         buffer.AppendFormatted("   %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0") {
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("TObject* %s_value=%s,",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
            }
            else {
               buffer.AppendFormatted("%s %s_value=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
            }
         }
      }
      buffer.Resize(buffer.Length()-1);
      buffer.AppendFormatted(" )\n");
      buffer.AppendFormatted("   { ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0")
            buffer.AppendFormatted("%s = %s_value; ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
         else {
            buffer.AppendFormatted("for (int i%d=0;i%d<%s;i%d++) %s[i%d] = %s; ",i,i,valueArray[iFold][i].Data(),i,valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("fModified = false; ");
      buffer.AppendFormatted("};\n");
      buffer.AppendFormatted("\n");

      // Getters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueArray[iFold][i]!="0") {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   %%-%ds Get%%sAt(int index)%%%ds { return &%%s[index];%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),"TRef*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   %%-%ds Get%%sAt(int index)%%%ds { return %%s[index];%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   %%-%ds Get%%s()%%%ds { return &%%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),"TRef*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   %%-%ds Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // isModified
      format.SetFormatted("   %%-%ds is%%s()%%%ds  { return f%%s;%%%ds };\n",typeLen,nameLen-8,nameLen-8);
      buffer.AppendFormatted((char*)format.Data(),"Bool_t","Modified","","Modified","");
      buffer.AppendFormatted("\n");
      // Setters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueArray[iFold][i]=="0") {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds fModified = true; };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds fModified = true; };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Set%%sAt%%%ds(int index,%%-%ds %%s_value%%%ds) { %%s[index]%%%ds = %%s_value;%%%ds fModified = true; };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   void Set%%sAt%%%ds(int index,%%-%ds %%s_value%%%ds) { %%s[index]%%%ds = %%s_value;%%%ds fModified = true; };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted((char*)format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
      }
      buffer.AppendFormatted("\n");
      // SetModified
      int lb = nameLen-8;
      format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s%%%ds) { f%%s%%%ds = %%s; };\n",lb,typeLen,lb,lb,lb);
      buffer.AppendFormatted((char*)format.Data(),"Modified","","Bool_t","modified","","Modified","","modified","");
      buffer.AppendFormatted("\n");
      // Set All
      buffer.AppendFormatted("   void SetAll( ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0") {
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("TObject* %s_value=%s,",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
            }
            else {
               buffer.AppendFormatted("%s %s_value=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
            }
         }
      }
      buffer.Resize(buffer.Length()-1);
      buffer.AppendFormatted(" )\n");
      buffer.AppendFormatted("   { ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0") {
            buffer.AppendFormatted("%s = %s_value; ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("fModified = true; ");
      buffer.AppendFormatted("};\n");
      buffer.AppendFormatted("\n");
      // Reset
      buffer.AppendFormatted("   void Reset() {");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0") {
            buffer.AppendFormatted("%s = %s; ",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         else {
            buffer.AppendFormatted("for (int i%d=0;i%d<%s;i%d++) %s[i%d] = %s; ",i,i,valueArray[iFold][i].Data(),i,valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("fModified = false; ");
      buffer.AppendFormatted("};\n");

      // Footer
      if (folderUserCode[iFold])
         buffer.AppendFormatted("\n   ClassDef(%s%s_Base,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
      else
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
      buffer.AppendFormatted("};\n\n");

      if (folderUserCode[iFold])
         buffer.AppendFormatted("#endif   // %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());

      // Write File
      fileHandle = open(hFile.Data(),O_RDONLY);
      nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      bool identical = true;
      if (nb==(int)buffer.Length()) {
         for (i=0;i<nb;i++) {
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
      hFile.SetFormatted("%s/include/framework/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      if (folderUserCode[iFold] && stat( hFile.Data(), &buf )) {
         // Description
         buffer.Resize(0);
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
         buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

         // Header
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());

         buffer.AppendFormatted("#include <include/framework/%s%s_Base.h>\n",shortCut.Data(),folderName[iFold].Data());

         // Class
         buffer.AppendFormatted("\nclass %s%s : public %s%s_Base\n",shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="0")
               buffer.AppendFormatted("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) : %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="0")
               buffer.AppendFormatted("%s,",valueName[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) {}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         // Write File
         fileHandle = open(hFile.Data(),O_RDONLY);
         nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
         bool identical = true;
         if (nb==(int)buffer.Length()) {
            for (i=0;i<nb;i++) {
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
      }
   }
   return true;
}

bool ROMEBuilder::ReadXMLTask() {
   // read the task definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i,j;
   ROMEString currentTaskName = "";
   int currentNumberOfTasks;

   // count tasks
   numOfTask++;
   currentNumberOfTasks = numOfTask;
   if (numOfTask>=maxNumberOfTasks) {
      cout << "Maximal number of tasks reached : " << maxNumberOfTasks << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   taskName[numOfTask] = "";
   taskEventID[numOfTask] = "a";
   taskFortran[numOfTask] = false;
   taskAuthor[numOfTask] = mainAuthor;
   taskVersion[numOfTask] = "1";
   taskDescription[numOfTask] = "";
   numOfHistos[numOfTask] = 0;
   numOfTaskInclude[numOfTask] = 0;
   taskParentName[numOfTask] = parent[recursiveDepth];
   numOfSteering[numOfTask] = -1;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subtask
      if (type == 1 && !strcmp((const char*)name,"Task")) {
         // set task as parent for subsequent tasks
         recursiveDepth++;
         parent[recursiveDepth] = taskName[numOfTask];
         // read subtask
         if (!ReadXMLTask()) 
            return false;
         continue;
      }
      // end task
      if (type == 15 && !strcmp((const char*)name,"Task")) {
         // check input
         if (currentTaskName=="") {
            cout << "The " << (currentNumberOfTasks+1) << ". Task has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         recursiveDepth--;
         return true;
      }
      // task name
      if (type == 1 && !strcmp((const char*)name,"TaskName")) {
         xml->GetValue(taskName[numOfTask],taskName[numOfTask]);
         currentTaskName = taskName[numOfTask];
         // output
         if (makeOutput) for (i=0;i<recursiveDepth;i++) cout << "   ";
         if (makeOutput) taskName[numOfTask].WriteLine();
      }
      // task event id
      if (type == 1 && !strcmp((const char*)name,"TaskEventId"))
         xml->GetValue(taskEventID[numOfTask],taskEventID[numOfTask]);
      // task language
      if (type == 1 && !strcmp((const char*)name,"Language")) {
         xml->GetValue(tmp,"c++");
         if (tmp == "Fortran") 
            taskFortran[numOfTask] = true;
      }
      // task author
      if (type == 1 && !strcmp((const char*)name,"Author"))
         xml->GetValue(taskAuthor[numOfTask],taskAuthor[numOfTask]);
      // task version
      if (type == 1 && !strcmp((const char*)name,"TaskVersion"))
         xml->GetValue(taskVersion[numOfTask],taskVersion[numOfTask]);
      // task description
      if (type == 1 && !strcmp((const char*)name,"TaskDescription"))
         xml->GetValue(taskDescription[numOfTask],taskDescription[numOfTask]);
      // task include
      if (type == 1 && !strcmp((const char*)name,"Include")) {
         // include initialisation
         taskInclude[numOfTask][numOfTaskInclude[numOfTask]] = "";
         taskLocalFlag[numOfTask][numOfTaskInclude[numOfTask]] = false;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include name
            if (type == 1 && !strcmp((const char*)name,"IncludeName"))
               xml->GetValue(taskInclude[numOfTask][numOfTaskInclude[numOfTask]],taskInclude[numOfTask][numOfTaskInclude[numOfTask]]);
            // include type
            if (type == 1 && !strcmp((const char*)name,"IncludeType")) {
               xml->GetValue(tmp,"false");
               if (tmp == "local") 
                  taskLocalFlag[numOfTask][numOfTaskInclude[numOfTask]] = true;
            }
            // include end
            if (type == 15 && !strcmp((const char*)name,"Include"))
               break;
         }
         // check input
         if (taskInclude[numOfTask][numOfTaskInclude[numOfTask]]=="") {
            cout << "An Include of Task '" << taskName[numOfTask].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // count includes
         numOfTaskInclude[numOfTask]++;
         if (numOfTaskInclude[numOfTask]>=maxNumberOfInclude) {
            cout << "Maximal number of inludes in Task '" << taskName[numOfTask].Data() << "' reached : " << numOfTaskInclude << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
      // task histogram
      if (type == 1 && !strcmp((const char*)name,"Histogram")) {
         // histogram initialisation
         histoName[numOfTask][numOfHistos[numOfTask]] = "";
         histoTitle[numOfTask][numOfHistos[numOfTask]] = "";
         histoFolderName[numOfTask][numOfHistos[numOfTask]] = "";
         histoFolderTitle[numOfTask][numOfHistos[numOfTask]] = "";
         histoType[numOfTask][numOfHistos[numOfTask]] = "";
         histoArray[numOfTask][numOfHistos[numOfTask]] = "1";
         histoXBin[numOfTask][numOfHistos[numOfTask]] = "1";
         histoXMin[numOfTask][numOfHistos[numOfTask]] = "0";
         histoXMax[numOfTask][numOfHistos[numOfTask]] = "1";
         histoYBin[numOfTask][numOfHistos[numOfTask]] = "1";
         histoYMin[numOfTask][numOfHistos[numOfTask]] = "0";
         histoYMax[numOfTask][numOfHistos[numOfTask]] = "1";
         histoZBin[numOfTask][numOfHistos[numOfTask]] = "1";
         histoZMin[numOfTask][numOfHistos[numOfTask]] = "0";
         histoZMax[numOfTask][numOfHistos[numOfTask]] = "1";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // histo name
            if (type == 1 && !strcmp((const char*)name,"HistName"))
               xml->GetValue(histoName[numOfTask][numOfHistos[numOfTask]],histoName[numOfTask][numOfHistos[numOfTask]]);
            // histo title
            if (type == 1 && !strcmp((const char*)name,"HistTitle"))
               xml->GetValue(histoTitle[numOfTask][numOfHistos[numOfTask]],histoTitle[numOfTask][numOfHistos[numOfTask]]);
            // histo folder name
            if (type == 1 && !strcmp((const char*)name,"HistFolderName"))
               xml->GetValue(histoFolderName[numOfTask][numOfHistos[numOfTask]],histoFolderName[numOfTask][numOfHistos[numOfTask]]);
            // histo folder title
            if (type == 1 && !strcmp((const char*)name,"HistFolderTitle"))
               xml->GetValue(histoFolderTitle[numOfTask][numOfHistos[numOfTask]],histoFolderTitle[numOfTask][numOfHistos[numOfTask]]);
            // histo type
            if (type == 1 && !strcmp((const char*)name,"HistType"))
               xml->GetValue(histoType[numOfTask][numOfHistos[numOfTask]],histoType[numOfTask][numOfHistos[numOfTask]]);
            // histo array size
            if (type == 1 && !strcmp((const char*)name,"HistArraySize"))
               xml->GetValue(histoArray[numOfTask][numOfHistos[numOfTask]],histoArray[numOfTask][numOfHistos[numOfTask]]);
            // histo number of x bins
            if (type == 1 && !strcmp((const char*)name,"HistXNbins"))
               xml->GetValue(histoXBin[numOfTask][numOfHistos[numOfTask]],histoXBin[numOfTask][numOfHistos[numOfTask]]);
            // histo x min
            if (type == 1 && !strcmp((const char*)name,"HistXmin"))
               xml->GetValue(histoXMin[numOfTask][numOfHistos[numOfTask]],histoXMin[numOfTask][numOfHistos[numOfTask]]);
            // histo x max
            if (type == 1 && !strcmp((const char*)name,"HistXmax"))
               xml->GetValue(histoXMax[numOfTask][numOfHistos[numOfTask]],histoXMax[numOfTask][numOfHistos[numOfTask]]);
            // histo number of y bins
            if (type == 1 && !strcmp((const char*)name,"HistYNbins"))
               xml->GetValue(histoYBin[numOfTask][numOfHistos[numOfTask]],histoYBin[numOfTask][numOfHistos[numOfTask]]);
            // histo y min
            if (type == 1 && !strcmp((const char*)name,"HistYmin"))
               xml->GetValue(histoYMin[numOfTask][numOfHistos[numOfTask]],histoYMin[numOfTask][numOfHistos[numOfTask]]);
            // histo y max
            if (type == 1 && !strcmp((const char*)name,"HistYmax"))
               xml->GetValue(histoYMax[numOfTask][numOfHistos[numOfTask]],histoYMax[numOfTask][numOfHistos[numOfTask]]);
            // histo number of z bins
            if (type == 1 && !strcmp((const char*)name,"HistZNbins"))
               xml->GetValue(histoZBin[numOfTask][numOfHistos[numOfTask]],histoZBin[numOfTask][numOfHistos[numOfTask]]);
            // histo z min
            if (type == 1 && !strcmp((const char*)name,"HistZmin"))
               xml->GetValue(histoZMin[numOfTask][numOfHistos[numOfTask]],histoZMin[numOfTask][numOfHistos[numOfTask]]);
            // histo z max
            if (type == 1 && !strcmp((const char*)name,"HistZmax"))
               xml->GetValue(histoZMax[numOfTask][numOfHistos[numOfTask]],histoZMax[numOfTask][numOfHistos[numOfTask]]);
            // histo end
            if (type == 15 && !strcmp((const char*)name,"Histogram"))
               break;
         }
         // check input
         if (histoName[numOfTask][numOfHistos[numOfTask]]=="") {
            cout << "A Histo of Task '" << taskName[numOfTask].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (histoType[numOfTask][numOfHistos[numOfTask]]=="") {
            cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no type defined !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         for (i=0;i<numOfHistos[numOfTask];i++) {
            for (j=i+1;j<numOfHistos[numOfTask];j++) {
               if (histoName[numOfTask][i]==histoName[numOfTask][j]) {
                  cout << "\nTask '" << taskName[numOfTask].Data() << "' has two histos with the name '" << histoName[numOfTask][i].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         // count histos
         numOfHistos[numOfTask]++;
         if (numOfHistos[numOfTask]>=maxNumberOfHistos) {
            cout << "Maximal number of histos in task '" << taskName[numOfTask].Data() << "' reached : " << maxNumberOfHistos << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
     // task steering parameters
      if (type == 1 && !strcmp((const char*)name,"SteeringParameters")) {
         // read steering parameter
         steerName[numOfTask][0] = "Steering";
         recursiveSteerDepth = 0;
         parentSteer[0] = "";
         if (!ReadXMLSteering(numOfTask)) 
            return false;
         numOfSteering[numOfTask]++;
      }
   }
   return true;
}
bool ROMEBuilder::WriteSteeringClass(ROMEString &buffer,int numSteer,int numTask,int tab) {
   ROMEString format;
   ROMEString sc;
   ROMEString blank="";
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i=0;i<numOfSteerFields[numTask][numSteer];i++) {
      if (typeLen<(int)steerFieldType[numTask][numSteer][i].Length()) typeLen = steerFieldType[numTask][numSteer][i].Length();
      if (nameLen<(int)steerFieldName[numTask][numSteer][i].Length()) nameLen = steerFieldName[numTask][numSteer][i].Length();
   }
   for (i=0;i<numOfSteering[numTask];i++) {
      if (steerParent[numTask][i]==steerName[numTask][numSteer]) {
         if (typeLen<(int)steerName[numTask][i].Length()+1) typeLen = steerName[numTask][i].Length()+1;
         if (nameLen<(int)steerName[numTask][i].Length()) nameLen = steerName[numTask][i].Length();
      }
   }
   if (tab==0)
      sc = "MEG";
   else 
      sc = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");   

   buffer.AppendFormatted("\n%sclass %s%s\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("%s{\n",blank.Data());

   buffer.AppendFormatted("%sprivate:\n",blank.Data());

   for (i=0;i<numOfSteering[numTask];i++) {
      if (steerParent[numTask][i]==steerName[numTask][numSteer]) {
         WriteSteeringClass(buffer,i,numTask,tab+1);
      }
   }

   buffer.AppendFormatted("%sprotected:\n",blank.Data());

   // Fields
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      format.SetFormatted("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldComment[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfSteering[numTask];i++) {
      if (steerParent[numTask][i]==steerName[numTask][numSteer]) {
         format.SetFormatted("%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-steerName[numTask][i].Length());
         buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerName[numTask][i].Data(),steerName[numTask][i].Data(),"",steerName[numTask][i].Data());
      }
   }
   buffer.AppendFormatted("\n%spublic:\n",blank.Data());
   // Constructor
   buffer.AppendFormatted("%s   %s%s() { ",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      buffer.AppendFormatted("f%s = %s; ",steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfSteering[numTask];i++) {
      if (steerParent[numTask][i]==steerName[numTask][numSteer]) {
         buffer.AppendFormatted("f%s = new %s(); ",steerName[numTask][i].Data(),steerName[numTask][i].Data());
      }
   }
   buffer.AppendFormatted("};\n");
   // Getters
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      format.SetFormatted("%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfSteering[numTask];i++) {
      if (steerParent[numTask][i]==steerName[numTask][numSteer]) {
         format.SetFormatted("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen-steerName[numTask][i].Length());
         buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerName[numTask][i].Data(),steerName[numTask][i].Data(),"",steerName[numTask][i].Data());
      }
   }
   // Setters
   buffer.AppendFormatted("\n");
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      format.SetFormatted("%%s   void Set%%-%ds(%%-%ds %%s)%%%ds { f%%s = %%s; };\n",nameLen,typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data());
   }

   // Footer
   buffer.AppendFormatted("%s};\n\n",blank.Data());

   return true;
}
void ROMEBuilder::WriteSteerConfigWrite(ROMEString& buffer,int numSteer,int numTask,int tab) {
   int i,j;
   ROMEString tmp;
   ROMEString getter;
   if (numSteer==0) {
      if (tab==0) 
         buffer.AppendFormatted("   xml->StartElement(\"GlobalSteeringParameters\");\n");
      else
         buffer.AppendFormatted("   xml->StartElement(\"SteeringParameters\");\n");
   }
   else
      buffer.AppendFormatted("   xml->StartElement(\"%s\");\n",steerName[numTask][numSteer].Data());
   for (i=0;i<numOfSteerFields[numTask][numSteer];i++) {
      getter.SetFormatted("->Get%s()",steerFieldName[numTask][numSteer][i].Data());
      int ind = numSteer;
      while (steerParent[numTask][ind]!="") {
         for (j=0;j<numOfSteering[numTask];j++) {
            if (steerParent[numTask][ind]==steerName[numTask][j]) {
               getter.InsertFormatted(0,"->Get%s()",steerName[numTask][ind].Data());
               ind = j;
               break;
            }
         }
      }
      if (tab==0) 
         getter.InsertFormatted(0,"GetGSP()");
      else
         getter.InsertFormatted(0,"((%sT%s*)f%sTask)->GetSP()",shortCut.Data(),taskName[numTask].Data(),taskName[numTask].Data());
      GetFormat(&tmp,(char*)steerFieldType[numTask][numSteer][i].Data());
      buffer.AppendFormatted("   value.SetFormatted(\"%s\",%s);\n",tmp.Data(),getter.Data());
      buffer.AppendFormatted("   xml->WriteElement(\"%s\",value.Data());\n",steerFieldName[numTask][numSteer][i].Data());
   }
   for (i=0;i<numOfSteering[numTask];i++) {
      if (steerParent[numTask][i]==steerName[numTask][numSteer]) {
         WriteSteerConfigWrite(buffer,i,numTask,tab);
      }
   }
   buffer.AppendFormatted("   xml->EndElement();\n");
}
void ROMEBuilder::WriteSteerConfigRead(ROMEString& buffer,int numSteer,int numTask,int tab) {
   ROMEString tmp;
   ROMEString value;
   ROMEString path;
   ROMEString setter;
   int i,j;
   ROMEString blank;
   if (tab==0)
      blank = "      ";
   else
      blank = "                     ";

   path = "";
   int ind = numSteer;
   while (steerParent[numTask][ind]!="") {
      for (j=0;j<numOfSteering[numTask];j++) {
         if (steerParent[numTask][ind]==steerName[numTask][j]) {
            path.InsertFormatted(0,"->Get%s()",steerName[numTask][ind].Data());
            ind = j;
            blank.Append("      ");
            break;
         }
      }
   }
   if (numSteer==0)
      buffer.AppendFormatted("%sif (type == 1 && !strcmp((const char*)name,\"%sParameters\")) {\n",blank.Data(),steerName[numTask][numSteer].Data());
   else
      buffer.AppendFormatted("%sif (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank.Data(),steerName[numTask][numSteer].Data());

   buffer.AppendFormatted("%s   while (xml->NextLine()) {\n",blank.Data());
   buffer.AppendFormatted("%s      type = xml->GetType();\n",blank.Data());
   buffer.AppendFormatted("%s      name = xml->GetName();\n",blank.Data());
   if (numSteer==0)
      buffer.AppendFormatted("%s      char *cstop;\n",blank.Data());
   for (i=0;i<numOfSteerFields[numTask][numSteer];i++) {
      setValue(&value,"","value",(char*)steerFieldType[numTask][numSteer][i].Data(),1);
      if (tab==0)
         setter.SetFormatted("GetGSP()%s->Set%s((%s)%s)",path.Data(),steerFieldName[numTask][numSteer][i].Data(),steerFieldType[numTask][numSteer][i].Data(),value.Data());
      else
         setter.SetFormatted("((%sT%s*)f%sTask)->GetSP()%s->Set%s((%s)%s)",shortCut.Data(),taskName[numTask].Data(),taskName[numTask].Data(),path.Data(),steerFieldName[numTask][numSteer][i].Data(),steerFieldType[numTask][numSteer][i].Data(),value.Data());
      buffer.AppendFormatted("%s      if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank.Data(),steerFieldName[numTask][numSteer][i].Data());
      buffer.AppendFormatted("%s         if (xml->GetValue(value)) \n",blank.Data());
      buffer.AppendFormatted("%s            %s;\n",blank.Data(),setter.Data());
      buffer.AppendFormatted("%s      }\n",blank.Data());
   }
   for (i=0;i<numOfSteering[numTask];i++) {
      if (steerParent[numTask][i]==steerName[numTask][numSteer]) {
         WriteSteerConfigRead(buffer,i,numTask,tab);
      }
   }
   if (numSteer==0)
      buffer.AppendFormatted("%s   if (type == 15 && !strcmp((const char*)name,\"%sParameters\"))\n",blank.Data(),steerName[numTask][numSteer].Data());
   else
      buffer.AppendFormatted("%s   if (type == 15 && !strcmp((const char*)name,\"%s\"))\n",blank.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("%s      break;\n",blank.Data());
   buffer.AppendFormatted("%s   }\n",blank.Data());
   buffer.AppendFormatted("%s}\n",blank.Data());
}
bool ROMEBuilder::WriteTaskCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int lenTot,ll,i,j;
   char *pBuffer=NULL;
   int bufferLen=0;
   char *pos;
   int fileHandle;
   ROMEString format;
   ROMEString str;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      // File name
      cppFile.SetFormatted("%s/src/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("//// Author: %s\n",taskAuthor[iTask].Data());
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      ll = 73-shortCut.Length();
      format.SetFormatted("// %%sT%%-%d.%ds //\n",ll,ll);
      buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("//                                                                            //\n");
      pos = (char*)taskDescription[iTask].Data();
      lenTot = taskDescription[iTask].Length();
      while (pos-taskDescription[iTask].Data() < lenTot) {
         if (lenTot+(taskDescription[iTask].Data()-pos)<74) 
            i=TMath::Min(75,lenTot);
         else for (i=74;pos[i]!=32&&i>0;i--) {}
         if (i<=0)
            i=TMath::Min(75,lenTot);
         pos[i] = 0;
         buffer.AppendFormatted("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      buffer.AppendFormatted("// \n");
      buffer.AppendFormatted("// \n");
      buffer.AppendFormatted("// This header has been generated by the ROMEBuilder.\n");
      buffer.AppendFormatted("// The event methods have been written by %s.\n",taskAuthor[iTask].Data());
      fileHandle = open(cppFile.Data(),O_RDONLY);
      int n = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      fileBuffer[n] = 0;
      bool first = true;
      for (j=0;j<numOfFolder;j++) {
         str = "Get";
         str.Append(folderName[j]);
         if (strstr(fileBuffer,str.Data())) {
            if (first) {
               buffer.AppendFormatted("// \n");
               buffer.AppendFormatted("// Please note: The following information is only correct after executing the\n");
               buffer.AppendFormatted("//              ROMEBuilder.\n");
               buffer.AppendFormatted("// \n");
               buffer.AppendFormatted("// This task accesses the following folders :\n");
               first = false;
            }
            buffer.AppendFormatted("//    %s\n",folderName[j].Data());
         }
      }
      close(fileHandle);
      buffer.AppendFormatted("// \n");
      if (numOfHistos[iTask]>0) {
         if (first) {
            buffer.AppendFormatted("// Please note: The following information is only correct after executing the\n");
            buffer.AppendFormatted("//              ROMEBuilder.\n");
            buffer.AppendFormatted("// \n");
            first = false;
         }
         buffer.AppendFormatted("// This task contains the following histgrams :\n");
         for (i=0;i<numOfHistos[iTask];i++) {
            buffer.AppendFormatted("//    %s\n",histoName[iTask][i].Data());
         }
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// The histograms are created and saved automaticaly by the task.\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// The following method can be used to fill a histogram :\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// Fill<Histogram Name>(double value,double weight)\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// For histogram arrays use :\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// Fill<Histogram Name>At(int index,double value,double weight)\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// If more histogram functions are needed use the following function the get\n");
         buffer.AppendFormatted("// a handle to the histogram and use the root functions.\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// Get<Histogram Name>Handle()\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// For histogram arrays use :\n");
         buffer.AppendFormatted("//\n");
         buffer.AppendFormatted("// Get<Histogram Name>HandleAt(int index)\n");
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
            buffer.AppendFormatted("\n\n#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[iTask].Data());
            buffer.AppendFormatted("#include <Riostream.h>\n");

            buffer.AppendFormatted("\nClassImp(%sT%s)\n\n",shortCut.Data(),taskName[iTask].Data());

            // User Functions
            ROMEString shortcut(shortCut);
            shortcut.ToLower();
            ROMEString taskname(taskName[iTask]);
            taskname.ToLower();

            if (taskFortran[iTask]) {
               buffer.AppendFormatted("extern \"C\" void %st%sinit_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("void %sT%s::Init()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("   %sT%s_Init_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("}\n\n");
               buffer.AppendFormatted("extern \"C\" void %st%sbeginofrun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("   %sT%s_BeginOfRun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("}\n\n");
               buffer.AppendFormatted("extern \"C\" void %st%sevent_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("void %sT%s::Event()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("   %sT%s_Event_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("}\n\n");
               buffer.AppendFormatted("extern \"C\" void %st%sendofrun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("   %sT%s_EndOfRun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("}\n\n");
               buffer.AppendFormatted("extern \"C\" void %st%sterminate_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("void %sT%s::Terminate()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("   %sT%s_Terminate_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormatted("}\n\n");
            }
            else {
               buffer.AppendFormatted("void %sT%s::Init()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormatted("void %sT%s::Event()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormatted("void %sT%s::Terminate()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
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

bool ROMEBuilder::WriteTaskF() {
   ROMEString fFile;
   ROMEString buffer;

   int nb;
   int fileHandle;
   struct stat buf;

   if (makeOutput) cout << "\n   Output F-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (taskFortran[iTask]) {
         // File name
         fFile.SetFormatted("%s/src/tasks/%sT%s.f",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

         ROMEString shortcut(shortCut);
         shortcut.ToLower();
         ROMEString taskname(taskName[iTask]);
         taskname.ToLower();

         if( !stat( fFile.Data(), &buf )) {
            fileHandle = open(fFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
            if (makeOutput) cout << "      " << fFile.Data() << endl;

            // Methods
            buffer.Resize(0);
            buffer.AppendFormatted("      subroutine %st%sinit()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%sbeginofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%sevent()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%sendofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%sterminate()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");

            nb = write(fileHandle,buffer.Data(), buffer.Length());
            close(fileHandle);
         }
      }
   }
   return true;
}
bool ROMEBuilder::WriteTaskH() {
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   ROMEString format;

   int i,j;
   int fileHandle;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      // File name
      hFile.SetFormatted("%s/include/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

      // Header
      buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());

      buffer.AppendFormatted("#include<TH1.h>\n");
      buffer.AppendFormatted("#include<TH2.h>\n");
      buffer.AppendFormatted("#include<TH3.h>\n");
      buffer.AppendFormatted("#include<ROMETask.h>\n");
         
      for (i=0;i<numOfTaskInclude[iTask];i++) {
         if (taskLocalFlag[iTask][i]) {
            buffer.AppendFormatted("#include\"%s\"\n",taskInclude[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("#include<%s>\n",taskInclude[iTask][i].Data());
         }
      }

      buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());

      // Class
      buffer.AppendFormatted("\nclass %sT%s : public ROMETask\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("{\n");

      buffer.AppendFormatted("protected:\n");
      // Fields
      if (numOfSteering[iTask]>0) {
         WriteSteeringClass(buffer,0,iTask,1);
         buffer.AppendFormatted("\n");
      }

      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   Steering* fSteering; // Handle to Steering class\n\n");
      }

      int nameLen = -1;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (nameLen<(int)histoName[iTask][i].Length()) nameLen = histoName[iTask][i].Length();
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]=="1") {
            format.SetFormatted("   %%s*      f%%s;%%%ds // %%s\n",nameLen-histoName[iTask][i].Length()+1);
            buffer.AppendFormatted((char*)format.Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),"",histoTitle[iTask][i].Data());
         }
         else {
            format.SetFormatted("   TObjArray* f%%ss;%%%ds // %%s\n",nameLen-histoName[iTask][i].Length());
            buffer.AppendFormatted((char*)format.Data(),histoName[iTask][i].Data(),"",histoTitle[iTask][i].Data());
         }
         format.SetFormatted("   bool       f%%sAccumulation;%%%ds // Accumulation Flag for the %%s\n",nameLen-histoName[iTask][i].Length());
         buffer.AppendFormatted((char*)format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
      }


      // Methods
      buffer.AppendFormatted("public:\n");
      // Constructor and Event Methods
      buffer.AppendFormatted("   // Constructor\n");
      buffer.AppendFormatted("   %sT%s(const char *name,const char *title):ROMETask(name,title)\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   { fEventID = '%s'; fVersion = %s;",taskEventID[iTask].Data(),taskVersion[iTask].Data());
      if (numOfHistos[iTask]>0) 
         buffer.AppendFormatted(" fHasHistograms = true;");
      else
         buffer.AppendFormatted(" fHasHistograms = false;");
      for (i=0;i<numOfHistos[iTask];i++) {
         buffer.AppendFormatted(" f%sAccumulation = true;",histoName[iTask][i].Data());
      }
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted(" fSteering = new Steering();");
      }
      buffer.AppendFormatted(" };\n");
      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; };\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]=="1") {
            if (histoType[iTask][i][2]==49) {
					buffer.AppendFormatted("   void Fill%s(double x,double weight=1) { f%s->Fill(x,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   void Fill%s(double x,double y,double weight=1) { f%s->Fill(x,y,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   void Fill%s(double x,double y,double z,double weight=1) { f%s->Fill(x,y,z,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
				buffer.AppendFormatted("   void Draw%s() { f%s->Draw(); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   %s* Get%sHandle() { return f%s; };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   void Fill%sAt(int index,double x,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   void Fill%sAt(int index,double x,double y,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   void Fill%sAt(int index,double x,double y,double z,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,z,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
				buffer.AppendFormatted("   void Draw%sAt(int index) { ((%s*)f%ss->At(index))->Draw(); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   %s* Get%sHandleAt(int index) { return (%s*)f%ss->At(index); };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
         }
         buffer.AppendFormatted("   Bool_t is%sAccumulation() { return f%sAccumulation; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sAccumulation(Bool_t flag) { f%sAccumulation = flag; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
      }


      buffer.AppendFormatted("protected:\n");
      buffer.AppendFormatted("   // Event Methods\n");
      buffer.AppendFormatted("   virtual void Init();\n");
      buffer.AppendFormatted("   virtual void BeginOfRun();\n");
      buffer.AppendFormatted("   virtual void Event();\n");
      buffer.AppendFormatted("   virtual void EndOfRun();\n");
      buffer.AppendFormatted("   virtual void Terminate();\n\n");
      // Histo Methods
      buffer.AppendFormatted("   // Histo Methods\n");
      buffer.AppendFormatted("   virtual void BookHisto();\n");
      buffer.AppendFormatted("   virtual void ResetHisto();\n\n");
      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
      buffer.AppendFormatted("};\n\n");

      // Histo Inline Methods
      buffer.AppendFormatted("inline void %sT%s::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      bool array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]!="1") array = true;
      }
      if (array) {
         buffer.AppendFormatted("   int j;\n");
         buffer.AppendFormatted("   ROMEString name;\n");
         buffer.AppendFormatted("   ROMEString title;\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         bool sameFolder = false;
         bool homeFolder = false;
         if (histoFolderName[iTask][i]=="") {
            homeFolder = true;
         }
         else {
            for (j=0;j<i;j++) {
               if (histoFolderName[iTask][i]==histoFolderName[iTask][j])
                  sameFolder = true;
            }
         }
         if (histoArray[iTask][i]=="1") {
            if (!sameFolder&&!homeFolder)
               buffer.AppendFormatted("   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",\"%s\");\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data(),histoFolderTitle[iTask][i].Data());
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   f%s = new %s(\"%s\",\"%s\",%s,%s,%s);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoTitle[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   f%s = new %s(\"%s\",\"%s\",%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoTitle[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   f%s = new %s(\"%s\",\"%s\",%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoTitle[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data(),histoZBin[iTask][i].Data(),histoZMin[iTask][i].Data(),histoZMax[iTask][i].Data());
            }
            if (!homeFolder)
               buffer.AppendFormatted("   %sFolder->Add(f%s);\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
            else
               buffer.AppendFormatted("   GetHistoFolder()->Add(f%s);\n",histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   %s *hist%d;\n",histoType[iTask][i].Data(),i);
            buffer.AppendFormatted("   f%ss = new TObjArray(%s);\n",histoName[iTask][i].Data(),histoArray[iTask][i].Data());
            if (!sameFolder&&!homeFolder)
               buffer.AppendFormatted("   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",\"%s\");\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data(),histoFolderTitle[iTask][i].Data());
            buffer.AppendFormatted("   for (j=0;j<%s;j++) {\n",histoArray[iTask][i].Data());
            buffer.AppendFormatted("      name.SetFormatted(\"%%0*d\",3,j);\n");
            buffer.AppendFormatted("      name.Insert(0,\"%s_\");\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      title.SetFormatted(\"%%0*d\",3,j);\n");
            buffer.AppendFormatted("      title.Insert(0,\"%s \");\n",histoTitle[iTask][i].Data());
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("      hist%d = new %s(name.Data(),title.Data(),%s,%s,%s);\n",i,histoType[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("      hist%d = new %s(name,title,%s,%s,%s,%s,%s,%s);\n",i,histoType[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("      hist%d = new %s(name,title,%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",i,histoType[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data(),histoZBin[iTask][i].Data(),histoZMin[iTask][i].Data(),histoZMax[iTask][i].Data());
            }
            buffer.AppendFormatted("      f%ss->Add(hist%d);\n",histoName[iTask][i].Data(),i);
            if (!homeFolder)
               buffer.AppendFormatted("      %sFolder->Add(f%ss->At(j));\n   }\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
            else
               buffer.AppendFormatted("      GetHistoFolder()->Add(f%ss->At(j));\n   }\n",histoName[iTask][i].Data());
         }
      }
      buffer.AppendFormatted("}\n\n");

      buffer.AppendFormatted("inline void %sT%s::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]!="1") 
            array = true;
      }
      if (array) {
         buffer.AppendFormatted("   int j;\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]=="1") {
            buffer.AppendFormatted("   if (!is%sAccumulation()) f%s->Reset();\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   if (!is%sAccumulation()) {\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("       for (j=0;j<%s;j++) ((%s*)f%ss->At(j))->Reset();\n",histoArray[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

      // Write File
      fileHandle = open(hFile.Data(),O_RDONLY);
      int nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      bool identical = true;
      if (nb==(int)buffer.Length()) {
         for (i=0;i<nb;i++) {
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
   }
   return true;
}

bool ROMEBuilder::ReadXMLTree() {
   char *name;
   int type,i,j;

   // output
   if (makeOutput) cout << "\n\nTrees:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"Tree")) {
         // count trees
         numOfTree++;
         if (numOfTree>=maxNumberOfTrees) {
            cout << "Maximal number of trees reached : " << maxNumberOfTrees << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // tree initialisation
         numOfBranch[numOfTree] = 0;
         treeName[numOfTree] = "";
         treeTitle[numOfTree] = "";

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // tree name
            if (type == 1 && !strcmp((const char*)name,"TreeName")) {
               xml->GetValue(treeName[numOfTree],treeName[numOfTree]);
               // output
               if (makeOutput) cout << "   " << treeName[numOfTree].Data() << endl;
            }
            // tree title
            if (type == 1 && !strcmp((const char*)name,"TreeTitle"))
               xml->GetValue(treeTitle[numOfTree],treeTitle[numOfTree]);
            if (type == 1 && !strcmp((const char*)name,"Branch")) {
               // branch initialisation
               branchName[numOfTree][numOfBranch[numOfTree]] = "";
               branchFolder[numOfTree][numOfBranch[numOfTree]] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // branch name
                  if (type == 1 && !strcmp((const char*)name,"BranchName")) {
                     xml->GetValue(branchName[numOfTree][numOfBranch[numOfTree]],branchName[numOfTree][numOfBranch[numOfTree]]);
                     // output
                     if (makeOutput) cout << "      " << branchName[numOfTree][numOfBranch[numOfTree]].Data() << endl;
                  }
                  // branch folder
                  if (type == 1 && !strcmp((const char*)name,"RelatedFolder"))
                     xml->GetValue(branchFolder[numOfTree][numOfBranch[numOfTree]],branchFolder[numOfTree][numOfBranch[numOfTree]]);
                  // branch end
                  if (type == 15 && !strcmp((const char*)name,"Branch"))
                     break;
               }
               // input chaeck
               if (branchName[numOfTree][numOfBranch[numOfTree]]=="") {
                  cout << "Branch without a name in Tree '" << treeName[numOfTree].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               if (branchFolder[numOfTree][numOfBranch[numOfTree]]=="") {
                  cout << "Branch '" << branchName[numOfTree][numOfBranch[numOfTree]].Data() << "' of Tree '" << treeName[numOfTree].Data() << "' has no Folder specified!" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               bool found = false;
               for (i=0;i<numOfFolder;i++) {
                  if (folderName[i]==branchFolder[numOfTree][numOfBranch[numOfTree]])
                     found = true;
               }
               if (!found) {
                  cout << "Folder of Branch '" << branchName[numOfTree][numOfBranch[numOfTree]].Data() << "' of Tree '" << treeName[numOfTree].Data() << "' not existing !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               // count branches
               numOfBranch[numOfTree]++;
               if (numOfBranch[numOfTree]>=maxNumberOfBranches) {
                  cout << "Maximal number of branches in tree '" << treeName[numOfTree].Data() << "' reached : " << maxNumberOfBranches << " !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
            if (type == 15 && !strcmp((const char*)name,"Tree")) {
               // input check
               if (treeName[numOfTree]=="") {
                  cout << "Tree without a name !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               if (numOfBranch[numOfTree] == 0) {
                  cout << "Tree '" << treeName[numOfTree].Data() << "' has no Branch !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               for (i=0;i<numOfBranch[numOfTree];i++) {
                  for (j=i+1;j<numOfBranch[numOfTree];j++) {
                     if (branchName[numOfTree][i]==branchName[numOfTree][j]) {
                        cout << "\nTree '" << treeName[numOfTree].Data() << "' has two branches with the name '" << branchName[numOfTree][i].Data() << "' !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
               }
               break;
            }
         }
      }

      if (type == 15 && !strcmp((const char*)name,"Trees")) {
         // input check
         for (i=0;i<numOfTree;i++) {
            for (j=i+1;j<numOfTree;j++) {
               if (treeName[i]==treeName[j]) {
                  cout << "\nTree '" << treeName[i].Data() << "' is defined twice !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         break;
      }
   }
   numOfTree++;
   return true;
}
bool ROMEBuilder::ReadXMLMidasBanks() {
   char *name;
   int type,i,j;
   bankHasHeader = false;

   // output
   if (makeOutput) cout << "\n\nBanks:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp("EventHeader",(const char*)name)) {
         // output
         if (makeOutput) cout << "   Header" << endl;
         // header initialisation
         bankHasHeader = true;
         bankHeaderFolder = "";
         bankHeaderEventID = "";
         bankHeaderTriggerMask = "";
         bankHeaderSerialNumber = "";
         bankHeaderTimeStamp = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // header folder
            if (type == 1 && !strcmp((const char*)name,"Folder"))
               xml->GetValue(bankHeaderFolder,bankHeaderFolder);
            // header EventID
            if (type == 1 && !strcmp((const char*)name,"EventID"))
               xml->GetValue(bankHeaderEventID,bankHeaderEventID);
            // header TriggerMask
            if (type == 1 && !strcmp((const char*)name,"TriggerMask"))
               xml->GetValue(bankHeaderTriggerMask,bankHeaderTriggerMask);
            // header SerialNumber
            if (type == 1 && !strcmp((const char*)name,"SerialNumber"))
               xml->GetValue(bankHeaderSerialNumber,bankHeaderSerialNumber);
            // header TimeStamp
            if (type == 1 && !strcmp((const char*)name,"TimeStamp"))
               xml->GetValue(bankHeaderTimeStamp,bankHeaderTimeStamp);
            // header end
            if (type == 15 && !strcmp((const char*)name,"EventHeader"))
               break;
         }
         // input check
         if (bankHeaderFolder=="") {
            cout << "Midas event header has no folder !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         int iFold = -1;
         for (i=0;i<numOfFolder;i++) {
            if (folderName[i]==bankHeaderFolder) {
               iFold = i;
               break;
            }
         }
         if (iFold==-1) {
            cout << "Midas event header : folder '" << bankHeaderFolder.Data() << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (folderArray[iFold]!="1") {
            cout << "Midas event header : folder '" << bankHeaderFolder.Data() << "' is an array !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         bool foundID = false;
         bool foundMask = false;
         bool foundNum = false;
         bool foundTime = false;
         if (bankHeaderEventID == "")
            foundID = true;
         if (bankHeaderTriggerMask == "") 
            foundMask = true;
         if (bankHeaderSerialNumber=="")
            foundNum = true;
         if (bankHeaderTimeStamp=="") 
            foundTime = true;
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueName[iFold][i]==bankHeaderEventID) {
               foundID = true;
            }
            if (valueName[iFold][i]==bankHeaderTriggerMask) {
               foundMask = true;
            }
            if (valueName[iFold][i]==bankHeaderSerialNumber) {
               foundNum = true;
            }
            if (valueName[iFold][i]==bankHeaderTimeStamp) {
               foundTime = true;
            }
         }
         if (!foundID) {
            cout << "Midas event header : event id field '" << bankHeaderEventID.Data() << "' does not exist in folder '" << bankHeaderFolder << "'!" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundMask) {
            cout << "Midas event header : trigger mask field '" << bankHeaderTriggerMask.Data() << "' does not exist in folder '" << bankHeaderFolder << "'!" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundNum) {
            cout << "Midas event header : serial number field '" << bankHeaderSerialNumber.Data() << "' does not exist '" << bankHeaderFolder << "'!" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundTime) {
            cout << "Midas event header : time stamp field '" << bankHeaderTimeStamp.Data() << "' does not exist '" << bankHeaderFolder << "'!" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
      }
      else if (type == 1 && !strcmp((const char*)name,"Bank")) {
         // count banks
         numOfBank++;
         if (numOfBank>=maxNumberOfBanks) {
            cout << "Maximal number of banks reached : " << maxNumberOfBanks << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // bank initialisation
         numOfStructFields[numOfBank] = 0;
         bankName[numOfBank] = "";
         bankType[numOfBank] = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // bank name
            if (type == 1 && !strcmp((const char*)name,"BankName")) {
               xml->GetValue(bankName[numOfBank],bankName[numOfBank]);
               // output
               if (makeOutput) cout << "   " << bankName[numOfBank].Data() << endl;
            }
            // bank type
            if (type == 1 && !strcmp((const char*)name,"BankType"))
               xml->GetValue(bankType[numOfBank],bankType[numOfBank]);
            // bank end
            if (type == 15 && !strcmp((const char*)name,"Bank"))
               break;
         }
         // input check
         if (bankName[numOfBank]=="") {
            cout << "A Midas Bank has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (bankType[numOfBank]=="") {
            cout << "Bank '" << bankName[numOfBank].Data() << "' has no type !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
      }
      else if (type == 1 && !strcmp((const char*)name,"StructuredBank")) {
         // count banks
         numOfBank++;
         if (numOfBank>=maxNumberOfBanks) {
            cout << "Maximal number of banks reached : " << maxNumberOfBanks << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // structured bank initialisation
         numOfStructFields[numOfBank] = 0;
         bankName[numOfBank] = "";
         bankType[numOfBank] = "struct";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // structured bank name
            if (type == 1 && !strcmp((const char*)name,"BankName")) {
               xml->GetValue(bankName[numOfBank],bankName[numOfBank]);
               // output
               if (makeOutput) cout << "   " << bankName[numOfBank].Data() << endl;
            }
            // structured bank field
            if (type == 1 && !strcmp((const char*)name,"BankField")) {
               // structured bank field initialisation
               structFieldName[numOfBank][numOfStructFields[numOfBank]] = "";
               structFieldType[numOfBank][numOfStructFields[numOfBank]] = "";
               structFieldSize[numOfBank][numOfStructFields[numOfBank]] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // field name
                  if (type == 1 && !strcmp((const char*)name,"BankFieldName")) {
                     xml->GetValue(structFieldName[numOfBank][numOfStructFields[numOfBank]],structFieldName[numOfBank][numOfStructFields[numOfBank]]);
                     // output
                     if (makeOutput) cout << "      " << structFieldName[numOfBank][numOfStructFields[numOfBank]].Data() << endl;
                  }
                  // field type
                  if (type == 1 && !strcmp((const char*)name,"BankFieldType"))
                     xml->GetValue(structFieldType[numOfBank][numOfStructFields[numOfBank]],structFieldType[numOfBank][numOfStructFields[numOfBank]]);
                  // field size
                  if (type == 1 && !strcmp((const char*)name,"BankFieldSize"))
                     xml->GetValue(structFieldSize[numOfBank][numOfStructFields[numOfBank]],structFieldSize[numOfBank][numOfStructFields[numOfBank]]);
                  // field end
                  if (type == 15 && !strcmp((const char*)name,"BankField"))
                     break;
               }
               // input check
               if (structFieldName[numOfBank][numOfStructFields[numOfBank]]=="") {
                  cout << "A field of the midas bank '" << bankName[numOfBank].Data() << "' has no name !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               if (structFieldType[numOfBank][numOfStructFields[numOfBank]]=="") {
                  cout << "Structure field '" << structFieldName[numOfBank][numOfStructFields[numOfBank]].Data() << "' of bank '" << bankName[numOfBank].Data() << "' has no type !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               for (i=0;i<numOfStructFields[numOfBank];i++) {
                  for (j=i+1;j<numOfStructFields[numOfBank];j++) {
                     if (structFieldName[numOfBank][i]==structFieldName[numOfBank][j]) {
                        cout << "\nStructure of bank '" << bankName[numOfBank].Data() << "' has two fields with the name '" << structFieldName[numOfBank][i].Data() << "' !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
               }
               // count structured bank fields
               numOfStructFields[numOfBank]++;
               if (numOfStructFields[numOfBank]>=maxNumberOfStructFields) {
                  cout << "Maximal number of fields in bank '" << bankName[numOfBank].Data() << "' reached : " << maxNumberOfStructFields << " !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
            // structured bank end
            if (type == 15 && !strcmp((const char*)name,"StructuredBank"))
               break;
         }
         // input check
         if (bankName[numOfBank]=="") {
            cout << "A structured midas bank has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
      }
      // midas bank end
      if (type == 15 && !strcmp((const char*)name,"MidasBanks"))
         break;
   }
   for (i=0;i<numOfBank;i++) {
      for (j=i+1;j<numOfTree;j++) {
         if (bankName[i]==bankName[j]) {
            cout << "\nMidas bank '" << bankName[i].Data() << "' is defined twice !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
      }
   }
   numOfBank++;
   return true;
}

bool ROMEBuilder::ReadXMLSteering(int iTask) {
   // read the steering parameter definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i,j;
   ROMEString currentSteeringName = "";
   int currentNumberOfSteerings;

   // count steering parameters
   numOfSteering[iTask]++;
   currentNumberOfSteerings = numOfSteering[iTask];
   if (numOfSteering[iTask]>=maxNumberOfTasks+1) {
      cout << "Maximal number of steering parameters reached : " << (maxNumberOfTasks+1) << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   if (numOfSteering[iTask]>0)
      steerName[iTask][numOfSteering[iTask]] = "";
   steerParent[iTask][numOfSteering[iTask]] = parentSteer[recursiveSteerDepth];
   numOfSteerFields[iTask][numOfSteering[iTask]] = 0;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // end
      if (type == 15 && !strcmp((const char*)name,"SteeringParameters"))
         return true;
      if (type == 15 && !strcmp((const char*)name,"GlobalSteeringParameters"))
         return true;
      // subgroup
      if (type == 1 && !strcmp((const char*)name,"SteeringParameterGroup")) {
         // set steering parameter group as parent for subsequent groups
         recursiveSteerDepth++;
         parentSteer[recursiveSteerDepth] = steerName[iTask][numOfSteering[iTask]];
         // read subgroup
         if (!ReadXMLSteering(iTask)) 
            return false;
         continue;
      }
      // end group
      if (type == 15 && !strcmp((const char*)name,"SteeringParameterGroup")) {
         // check input
         if (currentSteeringName=="") {
            cout << "The " << (currentNumberOfSteerings+1) << ". Steering Parameter Group has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         recursiveSteerDepth--;
         return true;
      }
      // group name
      if (type == 1 && !strcmp((const char*)name,"SPGroupName")) {
         xml->GetValue(steerName[iTask][numOfSteering[iTask]],steerName[iTask][numOfSteering[iTask]]);
         currentSteeringName = steerName[iTask][numOfSteering[iTask]];
         // output
         if (makeOutput) for (i=0;i<recursiveSteerDepth;i++) cout << "   ";
         if (makeOutput) steerName[iTask][numOfSteering[iTask]].WriteLine();
      }
      // steering parameter field
      if (type == 1 && !strcmp((const char*)name,"SteeringParameterField")) {
         // include initialisation
         bool readName = false;
         bool readType = false;
         steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] = "";
         steerFieldType[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] = "";
         steerFieldComment[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // steering parameter field name
            if (type == 1 && !strcmp((const char*)name,"SPFieldName")) {
               readName = true;
               xml->GetValue(steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]],steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]]);
               // output
               if (makeOutput) for (i=0;i<recursiveSteerDepth+1;i++) cout << "   ";
               if (makeOutput) steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]].WriteLine();
            }
            // steering parameter field type
            if (type == 1 && !strcmp((const char*)name,"SPFieldType")) {
               readType = true;
               xml->GetValue(steerFieldType[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]],steerFieldType[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]]);
               if (steerFieldType[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] == "TString")
                  steerFieldInit[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] = "' '";
               else if (steerFieldType[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] == "TRef")
                  steerFieldInit[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] = "NULL";
               else
                  steerFieldInit[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] = "0";
            }
            // steering parameter field initialization
            if (type == 1 && !strcmp((const char*)name,"SPFieldInitialization")) {
               if (!readName) {
                  cout << "Please specify a steering parameter field name befor the initial value in the " << (numOfSteering[iTask]+1) << ". steering parameter field in task '" << taskName[iTask].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               if (!readType) {
                  cout << "Please specify a steering parameter field type befor the initial value in steering parameter field '" << steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]].Data() << "' in task  '" << taskName[iTask].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               xml->GetValue(steerFieldInit[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]],steerFieldInit[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]]);
            }
            // steering parameter field comment
            if (type == 1 && !strcmp((const char*)name,"SPFieldComment")) {
               xml->GetValue(steerFieldComment[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]],steerFieldComment[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]]);
               if (steerFieldComment[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]][0]!='/') {
                  steerFieldComment[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]].Insert(0,"// ");
               }
            }
            // steering parameter field end
            if (type == 15 && !strcmp((const char*)name,"SteeringParameterField"))
               break;
         }
         // check input
         if (steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]]=="") {
            cout << "A steering parameter field of task '" << taskName[iTask].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // count includes
         numOfSteerFields[iTask][numOfSteering[iTask]]++;
         if (numOfSteerFields[iTask][numOfSteering[iTask]]>=maxNumberOfSteering) {
            cout << "Maximal number of steering parameter fields in task '" << taskName[iTask].Data() << "' reached : " << maxNumberOfSteering << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
   }
   return true;
}

bool ROMEBuilder::WriteSteering(int iTask) {
   int i;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   hFile.SetFormatted("%s/include/framework/%sGlobalSteering.h",outDir.Data(),shortCut.Data());

   if (numOfSteering[numOfTask]==-1) {
      remove(hFile.Data());
      return true;
   }

   if (makeOutput) cout << "\n   Output Files:" << endl;

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Contains the global Steering Parameters                                   //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sGlobalSteering_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sGlobalSteering_H\n\n",shortCut.Data());

   WriteSteeringClass(buffer,0,iTask,0);

   buffer.AppendFormatted("#endif   // %sGlobalSteering_H\n",shortCut.Data());

   int fileHandle = open(hFile.Data(),O_RDONLY);
   int nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb;i++) {
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
bool ROMEBuilder::WriteAnalyzerCpp() {
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
   classDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut.Data(),mainProgName.Data());

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
   cppFile.SetFormatted("%s/src/framework/%sAnalyzer.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Analyzer");
   buffer.AppendFormatted("//                                                                            //\n");
   pos = (char*)classDescription.Data();
   lenTot = classDescription.Length();
   while (pos-classDescription.Data() < lenTot) {
      if (lenTot+(classDescription.Data()-pos)<74) 
         i=TMath::Min(75,lenTot);
      else for (i=74;pos[i]!=32&&i>0;i--) {}
      if (i<=0) 
         i=TMath::Min(75,lenTot);
      pos[i] = 0;
      buffer.AppendFormatted("// %-74.74s   \n",pos);
      pos = pos+i+1;
   }
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#if defined( _MSC_VER )\n");
   buffer.AppendFormatted("#include <direct.h>\n");
   buffer.AppendFormatted("#endif\n");

   buffer.AppendFormatted("#include <sys/stat.h>\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
   buffer.AppendFormatted("#include <TObjString.h>\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#if defined HAVE_SQL\n");
   buffer.AppendFormatted("#include <ROMESQLDataBase.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#include <ROMETree.h>\n");
   buffer.AppendFormatted("#include <ROMETreeInfo.h>\n");
   buffer.AppendFormatted("#include <ROME.h>\n");
   buffer.AppendFormatted("#include <ROMEXMLDataBase.h>\n");
   buffer.AppendFormatted("#include <include/framework/%sEventLoop.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("#include <Riostream.h>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("ClassImp(%sAnalyzer);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sAnalyzer *gAnalyzer;  // global Analyzer Handle\n",shortCut.Data());

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#if defined( _MSC_VER )\n");
   buffer.AppendFormatted("#include <windows.h>\n");
   buffer.AppendFormatted("void CreateSplash(unsigned long time,char*,char*,ROMEString*,int);\n");
   buffer.AppendFormatted("DWORD WINAPI SplashThread ( LPVOID lpvoid) {\n");
   bool same = false;
   int numAuthors = 1;
   ROMEString authors[50];
   authors[0] = mainAuthor;
   for (i=0;i<numOfFolder;i++) {
      same = false;
      for (j=0;j<i+1;j++) {
         if (authors[j]==folderAuthor[i]) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = folderAuthor[i];
         numAuthors++;
      }
   }
   for (i=0;i<numOfTask;i++) {
      same = false;
      for (j=0;j<i+1;j++) {
         if (authors[j]==taskAuthor[i]) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = taskAuthor[i];
         numAuthors++;
      }
   }
   buffer.AppendFormatted("   ROMEString *authors = new ROMEString[%d];\n",numAuthors);
   for (i=0;i<numAuthors;i++) {
      buffer.AppendFormatted("   authors[%d] = \"%s\";\n",i,authors[i].Data());
   }
   buffer.AppendFormatted("   CreateSplash(3,\"%s%s\",\"%s\",authors,%d);\n",shortCut.Data(),mainProgName.Data(),romeVersion.Data(),numAuthors);
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("\n");


   // Constructor
   buffer.AppendFormatted("%sAnalyzer::%sAnalyzer(TRint *app):ROMEAnalyzer(app) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("// Folder, Task, Tree and Data Base initialisation\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gPassToROME = (void*)this; // Pass the handle to the framework\n");
   buffer.AppendFormatted("\n");

   // Steering 
   if (numOfSteering[numOfTask]>0) {
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
            buffer.AppendFormatted("\n   f%sObject = new %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   %sFolder->Add(f%sObject);\n",folderName[i].Data(),folderName[i].Data());
         }
         else {
            buffer.AppendFormatted("\n   f%sObjects = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("   %sFolder->Add(f%sObjects);\n",folderName[i].Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   fMainTask = new %sEventLoop(\"mainTask\",\"Main Task of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   fMainFolder->Add(fMainTask);\n");
   buffer.AppendFormatted("   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   int taskLen=0;
   for (i=0;i<numOfTask;i++) {
      if (taskLen<(int)taskName[i].Length()) taskLen = taskName[i].Length();
   }
   for (i=0;i<numOfTask;i++) {
      format.SetFormatted("   f%%sTask%%%ds = new %%sT%%s(\"%%s\",\"%%s\");\n",taskLen-taskName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),taskName[i].Data(),"",shortCut.Data(),taskName[i].Data(),taskName[i].Data(),"");
      buffer.AppendFormatted("   ((%sT%s*)f%sTask)->SetActive(false);\n",shortCut.Data(),taskName[i].Data(),taskName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      if (taskParentName[i]=="GetMainTask()") parentt = taskParentName[i];
      else parentt.SetFormatted("f%sTask",taskParentName[i].Data());
      buffer.AppendFormatted("   %s->Add(f%sTask);\n",parentt.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("}\n\n");


   // Midas Bank Initialisation
   buffer.AppendFormatted("// Midas Bank Initialisation\n");
   buffer.AppendFormatted("void %sAnalyzer::InitMidasBanks() {\n",shortCut.Data());
   for (i=0;i<numOfBank;i++) {
      buffer.AppendFormatted("   this->Init%sBank();\n",bankName[i].Data());
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Midas Bank Methodes
   buffer.AppendFormatted("\n// Midas Bank Getters\n");
   for (i=0;i<numOfBank;i++) {
      if (bankType[i]=="structure"||bankType[i]=="struct") {
         buffer.AppendFormatted("%sStruct* %sAnalyzer::Get%sBankAt(int index) {\n",bankName[i].Data(),shortCut.Data(),bankName[i].Data());
         buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i].Data());
         buffer.AppendFormatted("      return f%sBankPointer+index;\n",bankName[i].Data());
         buffer.AppendFormatted("   return NULL;\n");
         buffer.AppendFormatted("}\n");
      }
      else {
         buffer.AppendFormatted("%s %sAnalyzer::Get%sBankAt(int index) {\n",bankType[i].Data(),shortCut.Data(),bankName[i].Data());
         buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i].Data());
         buffer.AppendFormatted("      return *(f%sBankPointer+index);\n",bankName[i].Data());
         buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i].Data());
         buffer.AppendFormatted("}\n");
      }
      buffer.AppendFormatted("void %sAnalyzer::Init%sBank() {\n",shortCut.Data(),bankName[i].Data());
      buffer.AppendFormatted("   unsigned long bktype;\n");
      buffer.AppendFormatted("   EVENT_HEADER *pevent = this->GetEventHeader();\n");
      buffer.AppendFormatted("   pevent++;\n");
      buffer.AppendFormatted("   if (bk_find((BANK_HEADER*)pevent, \"%s\", (unsigned long*)&f%sBankLength, &bktype, (void**)&f%sBankPointer)) {\n",bankName[i].Data(),bankName[i].Data(),bankName[i].Data());
      buffer.AppendFormatted("      f%sBankExists = true;\n",bankName[i].Data());
      buffer.AppendFormatted("      return;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   f%sBankExists = false;\n",bankName[i].Data());
      buffer.AppendFormatted("   f%sBankPointer = NULL;\n",bankName[i].Data());
      buffer.AppendFormatted("   f%sBankLength = 0;\n",bankName[i].Data());
      buffer.AppendFormatted("   return;\n");
      buffer.AppendFormatted("}\n");

      buffer.AppendFormatted("int %sAnalyzer::Get%sBankEntries() {\n",shortCut.Data(),bankName[i].Data());
      buffer.AppendFormatted("   return f%sBankLength;\n",bankName[i].Data());
      buffer.AppendFormatted("}\n\n");
   }



   int ndb = 0;
   for (i=0;i<numOfFolder;i++) if (folderDataBase[i]) ndb++;

   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("bool %sAnalyzer::ReadSingleDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      buffer.AppendFormatted("   TObjArray *values = new TObjArray(0);\n");
      buffer.AppendFormatted("   char *cstop;\n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               buffer.AppendFormatted("   values->RemoveAll();\n");
               if (folderArray[i]=="1") {
                  buffer.AppendFormatted("   this->GetDataBase()->Read(values,\"RunTable/%s.%s\",this->GetCurrentRunNumber(),1);\n",folderName[i].Data(),valueName[i][j].Data());
                  setValue(&buf,(char*)valueName[i][j].Data(),"((TObjString*)values->At(0))->GetString().Data()",(char*)valueType[i][j].Data(),1);
                  buffer.AppendFormatted("   f%sObject->Set%s((%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
               }
            }
         }
      }
      buffer.AppendFormatted("   delete values;\n");
      buffer.AppendFormatted("   return true;\n");
   }
   buffer.AppendFormatted("}\n\n");

   // ReadArrayDataBaseFolders
   buffer.AppendFormatted("bool %sAnalyzer::ReadArrayDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      buffer.AppendFormatted("   TObjArray *values = new TObjArray(0);\n");
      buffer.AppendFormatted("   char *cstop;\n");
      buffer.AppendFormatted("   int i;\n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               buffer.AppendFormatted("   values->RemoveAll();\n");
               if (folderArray[i]!="1") {
                  buffer.AppendFormatted("   this->GetDataBase()->Read(values,\"RunTable/%s.%s\",this->GetCurrentRunNumber(),f%sObjects->GetEntries());\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data());
                  buffer.AppendFormatted("   for (i=0;i<f%sObjects->GetEntries();i++) {\n",folderName[i].Data());
                  buffer.AppendFormatted("      if (i<values->GetEntriesFast()) {\n");
                  setValue(&buf,(char*)valueName[i][j].Data(),"((TObjString*)values->At(i))->GetString().Data()",(char*)valueType[i][j].Data(),1);
                  buffer.AppendFormatted("         ((%s%s*)f%sObjects->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormatted("      } else {\n");
                  buffer.AppendFormatted("         ((%s%s*)f%sObjects->At(i))->Set%s(%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  buffer.AppendFormatted("      }\n");
                  buffer.AppendFormatted("   }\n");
               }
            }
         }
      }
      buffer.AppendFormatted("   delete values;\n");
      buffer.AppendFormatted("   return true;\n");
   }
   buffer.AppendFormatted("}\n\n");

   // Write Data Base
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i]) {
         buffer.AppendFormatted("void %sAnalyzer::Write%sDataBase() {\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("/*   // XML\n");
         buffer.AppendFormatted("   else if (this->isXMLDataBase()) {\n");
         buffer.AppendFormatted("      ROMEString name;\n");
         buffer.AppendFormatted("      ROMEString value;\n");
         buffer.AppendFormatted("      ROMEString dbFile;\n");
         buffer.AppendFormatted("      ROMEString filename;\n");
         buffer.AppendFormatted("      ROMEString path;\n");
         buffer.AppendFormatted("      ROMEString runNumberString;\n");
         buffer.AppendFormatted("      int n=0,i;\n");
         buffer.AppendFormatted("      ROMEXML *xml = new ROMEXML();\n");
         buffer.AppendFormatted("      this->GetCurrentRunNumberString(runNumberString);\n");
         buffer.AppendFormatted("      filename.SetFormatted(\"%%s/RunTable.xml\",this->GetInputDir());\n");
         buffer.AppendFormatted("      if (!xml->OpenFileForPath(filename.Data()))\n");
         buffer.AppendFormatted("         return;\n");
         buffer.AppendFormatted("      path.SetFormatted(\"//RunTable/Run_%%s\",runNumberString.Data());\n");
         buffer.AppendFormatted("      if (!xml->ExistPath(path.Data())) {\n");
         buffer.AppendFormatted("         name.SetFormatted(\"Run_%%s\",runNumberString.Data());\n");
         buffer.AppendFormatted("         if (!xml->HasPathChildren(\"//RunTable\")) {\n");
         buffer.AppendFormatted("            xml->NewPathChildElement(\"//RunTable\",name.Data(),NULL);\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else {\n");
         buffer.AppendFormatted("            bool exist = true;\n");
         buffer.AppendFormatted("            n = this->GetCurrentRunNumber();\n");
         buffer.AppendFormatted("            while (!xml->ExistPath(path.Data())) {\n");
         buffer.AppendFormatted("               if (n==0) {\n");
         buffer.AppendFormatted("                  exist = false;\n");
         buffer.AppendFormatted("                  break;\n");
         buffer.AppendFormatted("               }\n");
         buffer.AppendFormatted("               path.SetFormatted(\"//RunTable/Run_%%05d\",n--);\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("            if (exist) {\n");
         buffer.AppendFormatted("               xml->NewPathNextElement(path.Data(),name.Data(),NULL);\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("            else {\n");
         buffer.AppendFormatted("               xml->NewPathPrevElement(path.Data(),name.Data(),NULL);\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         path.SetFormatted(\"//RunTable/Run_%%s\",runNumberString.Data());\n");
         buffer.AppendFormatted("         dbFile = \"db%s\";\n",folderName[i].Data());
         buffer.AppendFormatted("         dbFile.Append(runNumberString.Data());\n");
         buffer.AppendFormatted("         dbFile.Append(\"_0.xml\");\n");
         buffer.AppendFormatted("         xml->NewPathAttribute(path.Data(),\"%sFile\",dbFile.Data());\n",folderName[i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      else {\n");
         buffer.AppendFormatted("         if (xml->GetPathAttribute(path,\"%sFile\",dbFile)) {\n",folderName[i].Data());
         buffer.AppendFormatted("            NextFile(dbFile,dbFile);\n");
         buffer.AppendFormatted("            xml->ReplacePathAttributeValue(path.Data(),\"%sFile\",dbFile.Data());\n",folderName[i].Data());
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else {\n");
         buffer.AppendFormatted("            dbFile = \"db%s\";\n",folderName[i].Data());
         buffer.AppendFormatted("            dbFile.Append(runNumberString.Data());\n");
         buffer.AppendFormatted("            dbFile.Append(\"_0.xml\");\n");
         buffer.AppendFormatted("            xml->NewPathAttribute(path.Data(),\"%sFile\",dbFile.Data());\n",folderName[i].Data());
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      xml->WritePathFile(filename.Data());\n");
         buffer.AppendFormatted("      filename.SetFormatted(\"%%s/%%s\",this->GetDataBaseDir(),dbFile.Data());\n");
         buffer.AppendFormatted("      if (!xml->OpenFileForWrite(filename.Data()))\n");
         buffer.AppendFormatted("         return;\n");
         buffer.AppendFormatted("      xml->StartElement(\"%ss\");\n",folderName[i].Data());
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("      xml->StartElement(\"%s\");\n",folderName[i].Data());
            buffer.AppendFormatted("      xml->WriteAttribute(\"Number\",\"0\");\n");
            for (j=0;j<numOfValue[i];j++) {
               GetFormat(&format,(char*)valueType[i][j].Data());
               if (valueType[i][j]=="TString") {
                  buffer.AppendFormatted("      value.SetFormatted(\"%s\",f%sObject->Get%s().Data());\n",format.Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("      value.SetFormatted(\"%s\",f%sObject->Get%s());\n",format.Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               buffer.AppendFormatted("      xml->WriteElement(\"%s\",value.Data());\n",valueName[i][j].Data());
            }
            buffer.AppendFormatted("      xml->EndElement();\n");
         }
         else {
            buffer.AppendFormatted("      for (i=0;i<f%sObjects->GetEntries();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("         xml->StartElement(\"%s\");\n",folderName[i].Data());
            buffer.AppendFormatted("         value.SetFormatted(\"%%d\",i);\n");
            buffer.AppendFormatted("         xml->WriteAttribute(\"Number\",value.Data());\n");
            for (j=0;j<numOfValue[i];j++) {
               GetFormat(&format,(char*)valueType[i][j].Data());
               if (valueType[i][j]=="TString") {
                  buffer.AppendFormatted("         value.SetFormatted(\"%s\",((%s%s*)f%sObjects->At(i))->Get%s().Data());\n",format.Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("         value.SetFormatted(\"%s\",((%s%s*)f%sObjects->At(i))->Get%s());\n",format.Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               buffer.AppendFormatted("         xml->WriteElement(\"%s\",value.Data());\n",valueName[i][j].Data());
            }
            buffer.AppendFormatted("         xml->EndElement();\n");
            buffer.AppendFormatted("      }\n");
         }
         buffer.AppendFormatted("      xml->EndDocument();\n");
         buffer.AppendFormatted("      delete xml;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("*/}\n\n");
      }
   }

   buffer.AppendFormatted("void %sAnalyzer::NextFile(ROMEString& nextFile,ROMEString& file) {\n",shortCut.Data());
   buffer.AppendFormatted("   struct stat buf;\n");
   buffer.AppendFormatted("   ROMEString body;\n");
   buffer.AppendFormatted("   char* res;\n");
   buffer.AppendFormatted("   int n=0,number=0;\n");
   buffer.AppendFormatted("   if ((res=strstr(file.Data(),\"_\"))) {\n");
   buffer.AppendFormatted("      n = res-file.Data();\n");
   buffer.AppendFormatted("      body = file(0,n);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      if ((res=strstr(file.Data(),\".\"))) {\n");
   buffer.AppendFormatted("         n = res-file.Data();\n");
   buffer.AppendFormatted("         body = file(0,n);\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else {\n");
   buffer.AppendFormatted("         body = file;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   nextFile.SetFormatted(\"%%s/%%s_%%d.xml\",this->GetDataBaseDir(),body.Data(),number);\n");
   buffer.AppendFormatted("   while (!stat(nextFile.Data(),&buf)) {\n");
   buffer.AppendFormatted("      number++;\n");
   buffer.AppendFormatted("      nextFile.SetFormatted(\"%%s/%%s_%%d.xml\",this->GetDataBaseDir(),body.Data(),number);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   nextFile.SetFormatted(\"%%s_%%d.xml\",body.Data(),number);\n");
   buffer.AppendFormatted("}\n\n");


   // Configuration File
   buffer.AppendFormatted("\n// Configuration File\n");
   buffer.AppendFormatted("//--------------------\n");
   buffer.AppendFormatted("bool %sAnalyzer::ReadROMEConfigXML(char *configFile) {\n",shortCut.Data());
   buffer.AppendFormatted("   char *name;\n");
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   ROMEString value;\n");
   buffer.AppendFormatted("   int type;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   if (!xml->OpenFileForRead(configFile)) {\n");
   buffer.AppendFormatted("      fprintf(stderr, \"Unable to open %%s\\n\", configFile);\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   while (xml->NextLine()) {\n");
   buffer.AppendFormatted("      type = xml->GetType();\n");
   buffer.AppendFormatted("      name = xml->GetName();\n");
   // Modes
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"Modes\")) {\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"AnalyzingMode\",value,\"\");\n");
   buffer.AppendFormatted("         if (value==\"online\")\n");
   buffer.AppendFormatted("            this->SetOnline();\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            this->SetOffline();\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"InputDataFormat\",value,\"\");\n");
   buffer.AppendFormatted("         if (value==\"root\")\n");
   buffer.AppendFormatted("            this->SetRoot();\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            this->SetMidas();\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"DataBaseMode\",value,\"\");\n");
   buffer.AppendFormatted("         if (value==\"sql\"||value==\"SQL\") {\n");
   buffer.AppendFormatted("#ifdef HAVE_SQL\n");
   buffer.AppendFormatted("            this->SetDataBase(new ROMESQLDataBase());\n");
   buffer.AppendFormatted("            this->SetDataBaseConnection(\"\");\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         else if (value==\"xml\"||value==\"XML\") {\n");
   buffer.AppendFormatted("            delete this->GetDataBase();\n");
   buffer.AppendFormatted("            this->SetDataBase(new ROMEXMLDataBase());\n");
   buffer.AppendFormatted("            this->SetDataBaseConnection(\"RunTable.xml\");\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"BatchMode\",value,\"\");\n");
   buffer.AppendFormatted("         if (value==\"yes\")\n");
   buffer.AppendFormatted("            fBatchMode = true;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fBatchMode = false;\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"ShowSplashScreen\",value,\"\");\n");
   buffer.AppendFormatted("         if (value==\"no\")\n");
   buffer.AppendFormatted("            fSplashScreen = false;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fSplashScreen = true;\n");
   buffer.AppendFormatted("      }\n");
   // Online
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"Online\")) {\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"Host\",value,\"\");\n");
   buffer.AppendFormatted("         fOnlineHost = value;\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"Experiment\",value,\"\");\n");
   buffer.AppendFormatted("         fOnlineExperiment = value;\n");
   buffer.AppendFormatted("      }\n");
   // Online
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"SocketInterface\")) {\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"PortNumber\",value,\"\");\n");
   buffer.AppendFormatted("         fPortNumber = strtol(value.Data(),&cstop,10);\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"AvailableOffline\",value,\"\");\n");
   buffer.AppendFormatted("         if (value==\"yes\")\n");
   buffer.AppendFormatted("            fSocketOffline = true;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fSocketOffline = false;\n");
   buffer.AppendFormatted("      }\n");
   // Run Numbers
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"RunNumbers\")) {\n");
   buffer.AppendFormatted("         if (xml->GetAttribute(\"Numbers\",value,\"\"))\n");
   buffer.AppendFormatted("            this->SetRunNumbers(value);\n");
   buffer.AppendFormatted("      }\n");
   // Event Numbers
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"EventNumbers\")) {\n");
   buffer.AppendFormatted("         if (xml->GetAttribute(\"Numbers\",value,\"\"))\n");
   buffer.AppendFormatted("            this->SetEventNumbers(value);\n");
   buffer.AppendFormatted("      }\n");
   // Paths
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"InputFilePath\")) {\n");
   buffer.AppendFormatted("         if (xml->GetValue(value)) {\n");
   buffer.AppendFormatted("            if (value[value.Length()-1]!='/' && value[value.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("               value.Append(\"/\");\n");
   buffer.AppendFormatted("            this->SetInputDir(value);\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"OutputFilePath\")) {\n");
   buffer.AppendFormatted("         if (xml->GetValue(value)) {\n");
   buffer.AppendFormatted("            if (value[value.Length()-1]!='/' && value[value.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("               value.Append(\"/\");\n");
   buffer.AppendFormatted("            this->SetOutputDir(value);\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"DataBaseFilePath\")) {\n");
   buffer.AppendFormatted("         if (xml->GetValue(value)) {\n");
   buffer.AppendFormatted("            if (value[value.Length()-1]!='/' && value[value.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("               value.Append(\"/\");\n");
   buffer.AppendFormatted("            this->SetDataBaseDir(value);\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   // Tasks
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"Tasks\")) {\n");
   buffer.AppendFormatted("         while (xml->NextLine()) {\n");
   buffer.AppendFormatted("            type = xml->GetType();\n");
   buffer.AppendFormatted("            name = xml->GetName();\n");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("            if (type == 1 && !strcmp(name,\"%s\")) {\n",taskName[i].Data());
      buffer.AppendFormatted("               int empty = xml->isEmpty();\n");
      buffer.AppendFormatted("               xml->GetAttribute(\"Active\",value,\"\");\n");
      buffer.AppendFormatted("               if (value==\"yes\") {\n");
      buffer.AppendFormatted("                  f%sTask->SetActive();\n",taskName[i].Data());
      buffer.AppendFormatted("               }\n");
      buffer.AppendFormatted("               if (!empty) {\n");
      buffer.AppendFormatted("                  while (xml->NextLine()) {\n");
      buffer.AppendFormatted("                     type = xml->GetType();\n");
      buffer.AppendFormatted("                     name = xml->GetName();\n");
      for (j=0;j<numOfHistos[i];j++) {
         buffer.AppendFormatted("                     if (type == 1 && !strcmp(name,\"%s\")) {\n",histoName[i][j].Data());
         buffer.AppendFormatted("                        xml->GetAttribute(\"Accumulate\",value,\"\");\n");
         buffer.AppendFormatted("                        if (value==\"no\")\n");
         buffer.AppendFormatted("                           ((%sT%s*)f%sTask)->Set%sAccumulation(false);\n",shortCut.Data(),taskName[i].Data(),taskName[i].Data(),histoName[i][j].Data());
         buffer.AppendFormatted("                     }\n");
      }
      WriteSteerConfigRead(buffer,0,i,1);
      buffer.AppendFormatted("                     if (type == 15 && !strcmp(name,\"%s\"))\n",taskName[i].Data());
      buffer.AppendFormatted("                        break;\n");
      buffer.AppendFormatted("                  }\n");
      buffer.AppendFormatted("               }\n");
      buffer.AppendFormatted("            }\n");
   }
   buffer.AppendFormatted("            if (type == 15 && !strcmp(name,\"Tasks\"))\n");
   buffer.AppendFormatted("               break;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   // Trees
   buffer.AppendFormatted("      if (type == 1 && !strcmp(name,\"Trees\")) {\n");
   buffer.AppendFormatted("         xml->GetAttribute(\"Accumulation\",value,\"\");\n");
   buffer.AppendFormatted("         if (value==\"yes\")\n");
   buffer.AppendFormatted("            this->SetTreeAccumulation();\n");
   buffer.AppendFormatted("         while (xml->NextLine()) {\n");
   buffer.AppendFormatted("            type = xml->GetType();\n");
   buffer.AppendFormatted("            name = xml->GetName();\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("            if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",treeName[i].Data());
      buffer.AppendFormatted("               xml->GetAttribute(\"Read\",value,\"\");\n");
      buffer.AppendFormatted("               if (value==\"yes\")\n");
      buffer.AppendFormatted("                  this->GetTreeObjectAt(%d)->SetRead(true);\n",i);
      buffer.AppendFormatted("               xml->GetAttribute(\"Write\",value,\"\");\n");
      buffer.AppendFormatted("               if (value==\"yes\")\n");
      buffer.AppendFormatted("                  this->GetTreeObjectAt(%d)->SetWrite(true);\n",i);
      buffer.AppendFormatted("               xml->GetAttribute(\"MaxNumberOfEntries\",value,\"\");\n");
      buffer.AppendFormatted("               int maxNumOfEntries = strtol(value.Data(),&cstop,10);\n");
      buffer.AppendFormatted("               this->GetTreeObjectAt(%d)->SetMaxEntries(maxNumOfEntries);\n",i);
      buffer.AppendFormatted("            }\n");
   }
   buffer.AppendFormatted("            if (type == 15 && !strcmp((const char*)name,\"Trees\"))\n");
   buffer.AppendFormatted("               break;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   // Steering
   if (numOfSteering[numOfTask]>0)
      WriteSteerConfigRead(buffer,0,numOfTask,0);

   buffer.AppendFormatted("      if (type == 15 && !strcmp((const char*)name,\"Configuration\"))\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");

   // WriteROMEConfigXML
   buffer.AppendFormatted("bool %sAnalyzer::WriteROMEConfigXML(char *configFile) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   if (!xml->OpenFileForWrite(configFile))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   xml->StartElement(\"Configuration\");\n");
   //modes
   buffer.AppendFormatted("   xml->StartElement(\"Modes\");\n");
   buffer.AppendFormatted("   if (this->isOnline())\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"AnalyzingMode\",\"online\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"AnalyzingMode\",\"offline\");\n");
   buffer.AppendFormatted("   if (this->isMidas())\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"InputDataFormat\",\"midas\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"InputDataFormat\",\"root\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"DataBaseMode\",this->GetDataBase()->GetType());\n");
   buffer.AppendFormatted("   if (isBatchMode())\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"BatchMode\",\"yes\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"BatchMode\",\"no\");\n");
   buffer.AppendFormatted("   if (isSplashScreen())\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"ShowSplashScreen\",\"yes\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"ShowSplashScreen\",\"no\");\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   //online
   buffer.AppendFormatted("   xml->StartElement(\"Online\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"Host\",(char*)fOnlineHost.Data());\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"Experiment\",(char*)fOnlineExperiment.Data());\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   //socket
   buffer.AppendFormatted("   xml->StartElement(\"SocketInterface\");\n");
   buffer.AppendFormatted("   str.SetFormatted(\"%%d\",this->GetPortNumber());\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"PortNumber\",(char*)str.Data());\n");
   buffer.AppendFormatted("   if (isSocketOffline())\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"AvailableOffline\",\"yes\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"AvailableOffline\",\"no\");\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   //run numbers
   buffer.AppendFormatted("   xml->StartElement(\"RunNumbers\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"Numbers\",(char*)this->GetRunNumberStringOriginal());\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   //event numbers
   buffer.AppendFormatted("   xml->StartElement(\"EventNumbers\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"Numbers\",(char*)this->GetEventNumberStringOriginal());\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   //paths
   buffer.AppendFormatted("   xml->StartElement(\"Paths\");\n");
   buffer.AppendFormatted("   xml->WriteElement(\"InputFilePath\",(char*)this->GetInputDir());\n");
   buffer.AppendFormatted("   xml->WriteElement(\"OutputFilePath\",(char*)this->GetOutputDir());\n");
   buffer.AppendFormatted("   xml->WriteElement(\"DataBaseFilePath\",(char*)this->GetDataBaseDir());\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("\n");

   //tasks
   buffer.AppendFormatted("   xml->StartElement( \"Tasks\");\n");
   bool romeStringDeclared = false;
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("   xml->StartElement(\"%s\");\n",taskName[i].Data());
      buffer.AppendFormatted("   if (f%sTask->IsActive())\n",taskName[i].Data());
      buffer.AppendFormatted("      xml->WriteAttribute(\"Active\",\"yes\");\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      xml->WriteAttribute(\"Active\",\"no\");\n");
      for (j=0;j<numOfHistos[i];j++) {
         buffer.AppendFormatted("   xml->StartElement(\"%s\");\n",histoName[i][j].Data());
         buffer.AppendFormatted("   if (((%sT%s*)f%sTask)->is%sAccumulation())\n",shortCut.Data(),taskName[i].Data(),taskName[i].Data(),histoName[i][j].Data());
         buffer.AppendFormatted("      xml->WriteAttribute(\"Accumulate\",\"yes\");\n");
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      xml->WriteAttribute(\"Accumulate\",\"no\");\n");
         buffer.AppendFormatted("   xml->EndElement();\n");
      }
      if (numOfSteering[i]>0) {
         if (!romeStringDeclared)
            buffer.AppendFormatted("   ROMEString value;\n");
         romeStringDeclared = true;
         WriteSteerConfigWrite(buffer,0,i,1);
      }
      buffer.AppendFormatted("      xml->EndElement();\n");
   }
   buffer.AppendFormatted("      xml->EndElement();\n");

   //trees
   buffer.AppendFormatted("   xml->StartElement(\"Trees\");\n");
   buffer.AppendFormatted("   if (this->isTreeAccumulation())\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"Accumulation\",\"yes\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteAttribute(\"Accumulation\",\"no\");\n");

   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   xml->StartElement(\"%s\");\n",treeName[i].Data());
      buffer.AppendFormatted("   if (this->GetTreeObjectAt(%d)->isRead())\n",i);
      buffer.AppendFormatted("      xml->WriteAttribute(\"Read\",\"yes\");\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      xml->WriteAttribute(\"Read\",\"no\");\n");
      buffer.AppendFormatted("   if (this->GetTreeObjectAt(%d)->isWrite())\n",i);
      buffer.AppendFormatted("      xml->WriteAttribute(\"Write\",\"yes\");\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      xml->WriteAttribute(\"Write\",\"no\");\n");
      buffer.AppendFormatted("   str.SetFormatted(\"%%d\",this->GetTreeObjectAt(%d)->GetMaxEntries());\n",i);
      buffer.AppendFormatted("   xml->WriteAttribute(\"MaxNumberOfEntries\",str.Data());\n");
      buffer.AppendFormatted("   xml->EndElement();\n");
   }
   buffer.AppendFormatted("      xml->EndElement();\n");

   //steering
   if (numOfSteering>0) {
      if (!romeStringDeclared)
        buffer.AppendFormatted("   ROMEString value;\n");
      WriteSteerConfigWrite(buffer,0,numOfTask,0);
   }

   buffer.AppendFormatted("   xml->EndDocument();\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");

   // Splash Screen
   buffer.AppendFormatted("#if defined( _MSC_VER )\n");
   buffer.AppendFormatted("LPDWORD lpThreadId;\n");
   buffer.AppendFormatted("void %sAnalyzer::startSplashScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   CloseHandle(CreateThread(NULL,1024,&SplashThread,0,0,lpThreadId));\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#if defined ( __linux__ )\n");
   buffer.AppendFormatted("void %sAnalyzer::startSplashScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   \n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("\n");

   // Console Screen
   ROMEString prog;
   prog.SetFormatted("%s%s",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("void %sAnalyzer::consoleStartScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   cout << \"*****************************************\" << endl;\n");   
   buffer.AppendFormatted("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*\";\n");
   int len1 = (int)((39-(double)prog.Length())/2+0.5);
   int len2 = (int)((39-(double)prog.Length())/2);
   buffer.AppendFormatted("   for (i=0;i<%d;i++) cout << \" \";\n",len1);
   buffer.AppendFormatted("   cout << \"%s\";\n",prog.Data());
   buffer.AppendFormatted("   for (i=0;i<%d;i++) cout << \" \";\n",len2);
   buffer.AppendFormatted("   cout << \"*\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*   generated by the ROME Environment   *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*             %s              *\" << endl;\n",romeVersion.Data());
   buffer.AppendFormatted("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormatted("   cout << \"*****************************************\" << endl << endl;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("   \n");

   // Close cpp-File
   fileHandle = open(cppFile.Data(),O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb;i++) {
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

bool ROMEBuilder::WriteAnalyzerH() {
   int i,j;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   ROMEString format;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
      if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
   }
   int taskLen=0;
   for (i=0;i<numOfTask;i++) {
      if (taskLen<(int)taskName[i].Length()) taskLen = taskName[i].Length();
   }




   // File name
   hFile.SetFormatted("%s/include/framework/%sAnalyzer.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sAnalyzer_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sAnalyzer_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#include <TRint.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#include <TTree.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#include <ROMEString.h>\n");
   buffer.AppendFormatted("#include <ROMETask.h>\n");
   buffer.AppendFormatted("#include <ROMEAnalyzer.h>\n");

   // include
   if (numOfSteering[numOfTask]>0) {
      buffer.AppendFormatted("#include <include/framework/%sGlobalSteering.h>\n",shortCut.Data());
   }

   // includes
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // bank structures
   if (numOfBank>0)
      buffer.AppendFormatted("   // Bank Structures\n");
   for (i=0;i<numOfBank;i++) {
      if (bankType[i]=="structure"||bankType[i]=="struct") {
         buffer.AppendFormatted("typedef struct {\n");
         for (j=0;j<numOfStructFields[i];j++) {
            if (structFieldSize[i][j].Length()>0)
               buffer.AppendFormatted("   %s %s : %s;\n",structFieldType[i][j].Data(),structFieldName[i][j].Data(),structFieldSize[i][j].Data());
            else
               buffer.AppendFormatted("   %s %s;\n",structFieldType[i][j].Data(),structFieldName[i][j].Data());
         }
         buffer.AppendFormatted("} %sStruct;\n",bankName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Task Switches Structure
   buffer.AppendFormatted("// Task Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("   int %s;   //! %s Task\n",taskName[i].Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("} TaskSwitches;\n");

   // Class
   buffer.AppendFormatted("\nclass %sAnalyzer : public ROMEAnalyzer\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   // Fields
   buffer.AppendFormatted("protected:\n");

   // Folder Fields
   buffer.AppendFormatted("   // Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            format.SetFormatted("   %%s%%s*%%%ds f%%sObject; %%%ds // Handle to %%s%%s Object\n",typeLen-folderName[i].Length()-scl,nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
         else {
            format.SetFormatted("   TClonesArray*%%%ds f%%sObjects;%%%ds // Handle to %%s%%s Objects\n",typeLen-12,nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task Fields
   buffer.AppendFormatted("   // Task Fields\n");
   for (i=0;i<numOfTask;i++) {
      format.SetFormatted("   ROMETask* f%%sTask%%%ds;  // Handle to %%s Task\n",taskLen-taskName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),taskName[i].Data(),"",taskName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Steering Fields
   if (numOfSteering[numOfTask]>0) {
      buffer.AppendFormatted("   // Steering Parameter Fields\n");
      buffer.AppendFormatted("\n   %sGlobalSteering* fGlobalSteeringParameters; // Handle to the GlobalSteering Class\n",shortCut.Data());
   }

   // Bank Fields
   if (numOfBank>0) {
      int bankNameLen = -1;
      int bankTypeLen = -1;
      for (i=0;i<numOfBank;i++) {
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            if (bankTypeLen<(int)bankName[i].Length()+6) bankTypeLen = bankName[i].Length()+6;
         }
         else {
            if (bankTypeLen<(int)bankType[i].Length()) bankTypeLen = bankType[i].Length();
         }
         if (bankNameLen<(int)bankName[i].Length()) bankNameLen = bankName[i].Length();
      }
      buffer.AppendFormatted("   // Bank Fields\n");
      for (i=0;i<numOfBank;i++) {
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            format.SetFormatted("   %%sStruct*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankName[i].Length()-6,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),bankName[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
         }
         else {
            format.SetFormatted("   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i].Length(),bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),bankType[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
         }
         format.SetFormatted("   int%%%ds f%%sBankLength;  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-2,bankNameLen-bankName[i].Length());
         buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
         format.SetFormatted("   bool%%%ds f%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-bankName[i].Length());
         buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
      }
      buffer.AppendFormatted("\n");
   }

   // Task Switches
   buffer.AppendFormatted("   // Task Switches\n");
   buffer.AppendFormatted("   TaskSwitches fTaskSwitches;               //! Task Switches\n");
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sAnalyzer(TRint *app);\n",shortCut.Data());
   // Folder Getters
   buffer.AppendFormatted("   // Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         int lt = typeLen-folderName[i].Length()-scl+nameLen-folderName[i].Length();
         if (folderArray[i]=="1") {
            format.SetFormatted("   %%s%%s*%%%ds  Get%%sObject()%%%ds { return f%%sObject;%%%ds };\n",typeLen-folderName[i].Length()-scl,8+nameLen-folderName[i].Length(),15+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   %%s%%s**%%%ds Get%%sObjectAddress()%%%ds { return &f%%sObject;%%%ds };\n",typeLen-folderName[i].Length()-scl,1+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
         else if (folderArray[i]=="variable") {
            format.SetFormatted("   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds\n",typeLen-folderName[i].Length()-scl,3+nameLen-folderName[i].Length(),lt);
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),"");
            buffer.AppendFormatted("   { if (f%sObjects->GetEntries()<=index)\n",folderName[i].Data());
            buffer.AppendFormatted("        for (int i=f%sObjects->GetEntries();i<=index;i++)\n",folderName[i].Data());
            buffer.AppendFormatted("           new((*f%sObjects)[i]) %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("     return (%s%s*)f%sObjects->At(index); };\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            format.SetFormatted("   TClonesArray*%%%ds  Get%%sObjects()%%%ds { return f%%sObjects;%%%ds };\n",typeLen-12,7+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray**%%%ds Get%%sObjectsAddress()%%%ds { return &f%%sObjects;%%%ds };\n",typeLen-12,nameLen-folderName[i].Length(),13+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
         else {
            format.SetFormatted("   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds { return (%%s%%s*)f%%sObjects->At(index);%%%ds };\n",typeLen-folderName[i].Length()-scl,3+nameLen-folderName[i].Length(),lt);
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray*%%%ds  Get%%sObjects()%%%ds { return f%%sObjects;%%%ds };\n",typeLen-12,7+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray**%%%ds Get%%sObjectsAddress()%%%ds { return &f%%sObjects;%%%ds };\n",typeLen-12,nameLen-folderName[i].Length(),13+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task Getters
   buffer.AppendFormatted("   // Tasks\n");
   for (i=0;i<numOfTask;i++) {
      format.SetFormatted("   ROMETask* Get%%sTask()%%%ds { return f%%sTask;%%%ds };\n",taskLen-taskName[i].Length(),taskLen-taskName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),taskName[i].Data(),"",taskName[i].Data(),"");
   }

   // Banks
   if (numOfBank>0) {
      buffer.AppendFormatted("   // Bank Methodes\n");
      buffer.AppendFormatted("   void InitMidasBanks();\n");
      for (i=0;i<numOfBank;i++) {
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            buffer.AppendFormatted("   %sStruct* Get%sBankAt(int index);\n",bankName[i].Data(),bankName[i].Data());
         }
         else {
            buffer.AppendFormatted("   %s Get%sBankAt(int index);\n",bankType[i].Data(),bankName[i].Data());
         }
         buffer.AppendFormatted("   int Get%sBankEntries();\n",bankName[i].Data());
         buffer.AppendFormatted("   void Init%sBank();\n",bankName[i].Data());
      }
      buffer.AppendFormatted("\n");
   }

   // Task Switches
   buffer.AppendFormatted("   // Task Switches\n");
   buffer.AppendFormatted("   TaskSwitches* GetTaskSwitches() { return &fTaskSwitches; };\n");
   buffer.AppendFormatted("\n");

   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   buffer.AppendFormatted("   bool ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   bool ReadArrayDataBaseFolders();\n");
   buffer.AppendFormatted("\n");

   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i]) {
         buffer.AppendFormatted("   void Write%sDataBase();\n",folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Configuration file
   buffer.AppendFormatted("   // Configuration File\n");
   buffer.AppendFormatted("   bool ReadROMEConfigXML(char *configFile);\n");
   buffer.AppendFormatted("   bool WriteROMEConfigXML(char *configFile);\n");
   buffer.AppendFormatted("\n");
 
   // Steering
   if (numOfSteering[numOfTask]>0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGlobalSteeringParameters() { return fGlobalSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   void startSplashScreen();\n");
   buffer.AppendFormatted("   void consoleStartScreen();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   void NextFile(ROMEString& nextFile,ROMEString& file);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sAnalyzer,0);\n",shortCut.Data());

   // Footer
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("extern %sAnalyzer *gAnalyzer;  // global Analyzer Handle\n\n",shortCut.Data());

   buffer.AppendFormatted("#endif   // %sAnalyzer_H\n",shortCut.Data());

   // Write File
   fileHandle = open(hFile.Data(),O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb;i++) {
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

bool ROMEBuilder::WriteEventLoopCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   ROMEString parentt;

   int nb,lenTot,j,ll,k,iFold=0;
   char *pos;
   int fileHandle;

   ROMEString classDescription;
   classDescription.SetFormatted("Framework specific event loop class.");

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
   cppFile.SetFormatted("%s/src/framework/%sEventLoop.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"EventLoop");
   buffer.AppendFormatted("//                                                                            //\n");
   pos = (char*)classDescription.Data();
   lenTot = classDescription.Length();
   while (pos-classDescription.Data() < lenTot) {
      if (lenTot+(classDescription.Data()-pos)<74) 
         i=TMath::Min(75,lenTot);
      else for (i=74;pos[i]!=32&&i>0;i--) {}
      if (i<=0) 
         i=TMath::Min(75,lenTot);
      pos[i] = 0;
      buffer.AppendFormatted("// %-74.74s   \n",pos);
      pos = pos+i+1;
   }
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#if defined( _MSC_VER )\n");
   buffer.AppendFormatted("#include <direct.h>\n");
   buffer.AppendFormatted("#endif\n");

   buffer.AppendFormatted("#include <sys/stat.h>\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
   buffer.AppendFormatted("#include <TObjString.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#if defined HAVE_SQL\n");
   buffer.AppendFormatted("#include <ROMESQL.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#include <ROMEEventLoop.h>\n");
   buffer.AppendFormatted("#include <ROMETree.h>\n");
   buffer.AppendFormatted("#include <ROMETreeInfo.h>\n");
   buffer.AppendFormatted("#include <ROME.h>\n");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sEventLoop.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <Riostream.h>\n");
   buffer.AppendFormatted("\n");
//   buffer.AppendFormatted("ClassImp(%sEventLoop);\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // Constructor
   buffer.AppendFormatted("%sEventLoop::%sEventLoop(const char *name,const char *title):ROMEEventLoop(name,title) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("}\n");


   // Tree Initialization
   buffer.AppendFormatted("// Tree initialization\n");
   buffer.AppendFormatted("void %sEventLoop::InitTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TFolder *treeFolder = gAnalyzer->GetMainFolder()->AddFolder(\"Trees\",\"Trees of the %s framework\");\n",shortCut.Data());
   buffer.AppendFormatted("   TTree *tree;\n\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   tree = new TTree(\"%s\",\"%s\");\n",treeName[i].Data(),treeTitle[i].Data());
      buffer.AppendFormatted("   tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000,99);\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k]) 
               iFold = k;
         }
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"%s%s\",gAnalyzer->Get%sObjectAddress(),32000,99);\n",branchName[i][j].Data(),shortCut.Data(),folderName[iFold].Data(),branchFolder[i][j].Data());
         }
         else {
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"TClonesArray\",gAnalyzer->Get%sObjectsAddress(),32000,99);\n",branchName[i][j].Data(),branchFolder[i][j].Data());
         }
      }
      buffer.AppendFormatted("   gAnalyzer->AddTree(tree);\n");
      buffer.AppendFormatted("   treeFolder->Add(tree);\n\n");
   }
   buffer.AppendFormatted("}\n\n");

   // Task switches call back
   buffer.AppendFormatted("// Task switches call back\n");
   buffer.AppendFormatted("bool ROMEAnalyzer::fTaskSwitchesChanged = false;\n");
   buffer.AppendFormatted("void TaskSwitchesChanged(int hDB,int hKey,void *info) {\n");
   buffer.AppendFormatted("   ROMEAnalyzer::fTaskSwitchesChanged = true;\n");
   buffer.AppendFormatted("}\n\n");

   // Initialize ODB
   buffer.AppendFormatted("// InitODB\n");
   buffer.AppendFormatted("bool %sEventLoop::InitODB() {\n",shortCut.Data());
   buffer.AppendFormatted("#if defined HAVE_MIDAS\n");
   buffer.AppendFormatted("   HNDLE hKey;\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   str = \"/%s%s/Task switches\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   ROMEString taskSwitchesString =  \"");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("%s = BOOL : 0\\n",taskName[i].Data());
   }
   buffer.AppendFormatted("\";\n");
   buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), (char*)taskSwitchesString.Data(), TRUE);\n");
   buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);\n");
   buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,gAnalyzer->GetTaskSwitches(),sizeof(TaskSwitches),0) != DB_SUCCESS) {\n");
   buffer.AppendFormatted("      cout << \"Cannot write to task switches record.\" << endl;\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetTaskSwitches(), sizeof(TaskSwitches), MODE_READ, TaskSwitchesChanged, NULL) != DB_SUCCESS) {\n");
   buffer.AppendFormatted("      cout << \"Cannot open task switches record, probably other analyzer is using it\" << endl;\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // clean up folders
   buffer.AppendFormatted("// Delete Unused Folders\n");
   buffer.AppendFormatted("void %sEventLoop::CleanUpFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
         if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=gAnalyzer->Get%sObjects()->GetEntriesFast()-1;i>=0;i--) {\n",folderName[i].Data());
            buffer.AppendFormatted("      if (((%s%s*)gAnalyzer->Get%sAt(i))->isModified())\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("         break;\n");
            buffer.AppendFormatted("      gAnalyzer->Get%sObjects()->RemoveAt(i);\n",folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // reset folders
   buffer.AppendFormatted("// Reset Folders\n");
   buffer.AppendFormatted("void %sEventLoop::ResetFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   gAnalyzer->Get%sObject()->Reset();\n",folderName[i].Data());
         }
         else if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=0;i<gAnalyzer->Get%sObjects()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->Reset();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<gAnalyzer->Get%sObjects()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->SetModified(false);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      if (bankHasHeader) {
         if (folderName[i]==bankHeaderFolder) {
            if (bankHeaderEventID!="")
               buffer.AppendFormatted("   gAnalyzer->Get%sObject()->Set%s(gAnalyzer->GetEventHeader()->event_id);\n",folderName[i].Data(),bankHeaderEventID.Data());
            if (bankHeaderTriggerMask!="")
               buffer.AppendFormatted("   gAnalyzer->Get%sObject()->Set%s(gAnalyzer->GetEventHeader()->trigger_mask);\n",folderName[i].Data(),bankHeaderTriggerMask.Data());
            if (bankHeaderSerialNumber!="")
               buffer.AppendFormatted("   gAnalyzer->Get%sObject()->Set%s(gAnalyzer->GetEventHeader()->serial_number);\n",folderName[i].Data(),bankHeaderSerialNumber.Data());
            if (bankHeaderTimeStamp!="")
               buffer.AppendFormatted("   gAnalyzer->Get%sObject()->Set%s(gAnalyzer->GetEventHeader()->time_stamp);\n",folderName[i].Data(),bankHeaderTimeStamp.Data());
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Initialize Single Folders
   buffer.AppendFormatted("void %sEventLoop::InitSingleFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   new(gAnalyzer->Get%sObject()) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="0")
                  buffer.AppendFormatted("%s,",valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length()-1);
            buffer.AppendFormatted(" );\n");
         }
      }
   }
   buffer.AppendFormatted("};\n\n");

   // Initialize Array Folders
   buffer.AppendFormatted("void %sEventLoop::InitArrayFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]!="1" && folderArray[i]!="variable") {
            buffer.AppendFormatted("   for (i=0;i<%s;i++) {\n",folderArray[i].Data());
            buffer.AppendFormatted("     new((*gAnalyzer->Get%sObjects())[i]) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="0")
                  buffer.AppendFormatted("%s,",valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length()-1);
            buffer.AppendFormatted(" );\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("};\n\n");

   // fill trees
   buffer.AppendFormatted("// Tree Filling\n");
   buffer.AppendFormatted("void %sEventLoop::FillTrees() {\n",shortCut.Data());
   buffer.AppendFormatted("   Statistics *stat = gAnalyzer->GetTriggerStatistics();\n");
   buffer.AppendFormatted("   ROMETree *romeTree;\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   // Fill Trees;\n");
   buffer.AppendFormatted("   bool write = false;\n");
   buffer.AppendFormatted("   bool written = false;\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   write = false;\n");
      buffer.AppendFormatted("   romeTree = (ROMETree*)gAnalyzer->GetTreeObjectAt(%d);\n",i);
      buffer.AppendFormatted("   if (romeTree->isWrite()) {\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (folderName[k]==branchFolder[i][j]) {
               iFold = k;
               break;
            }
         }
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("      if (!write && gAnalyzer->Get%sObject()->isModified()) {\n",branchFolder[i][j].Data());
            buffer.AppendFormatted("         write = true;\n");
            buffer.AppendFormatted("      }\n");
         }
         else {
            buffer.AppendFormatted("      for (i=0;i<gAnalyzer->Get%sObjects()->GetEntries()&&!write;i++) {\n",branchFolder[i][j].Data());
            buffer.AppendFormatted("         if (((%s%s*)gAnalyzer->Get%sAt(i))->isModified()) {\n",shortCut.Data(),branchFolder[i][j].Data(),branchFolder[i][j].Data());
            buffer.AppendFormatted("            write = true;\n");
            buffer.AppendFormatted("            break;\n");
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
         }
      }
      buffer.AppendFormatted("      if (write) {\n");
      buffer.AppendFormatted("         written = true;\n");
      buffer.AppendFormatted("         fTreeInfo->SetSequentialNumber(fSequentialNumber);\n");
      buffer.AppendFormatted("         romeTree->GetTree()->Fill();\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   if (written) {\n");
   buffer.AppendFormatted("      fSequentialNumber++;\n");
   buffer.AppendFormatted("      stat->writtenEvents++;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Connect Trees
   buffer.AppendFormatted("// Connect Trees\n");
   buffer.AppendFormatted("void %sEventLoop::ConnectTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TBranchElement *bb;\n");
   for (i=0;i<numOfTree;i++) {
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k]) 
               iFold = k;
         }
         buffer.AppendFormatted("   bb = (TBranchElement*)gAnalyzer->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"%s\");\n",i,branchName[i][j].Data());
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("   bb->SetAddress(gAnalyzer->Get%sObjectAddress());\n",folderName[iFold].Data());
         }
         else {
            buffer.AppendFormatted("   bb->SetAddress(gAnalyzer->Get%sObjectsAddress());\n",folderName[iFold].Data());
         }
         buffer.AppendFormatted("   bb = (TBranchElement*)gAnalyzer->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"Info\");\n",i);
         buffer.AppendFormatted("   bb->SetAddress(&this->fTreeInfo);\n");
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Initialize Task Switches
   buffer.AppendFormatted("void %sEventLoop::InitTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("   gAnalyzer->GetTaskSwitches()->%s = gAnalyzer->Get%sTask()->IsActive();\n",taskName[i].Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("};\n\n");

   // Update Task Switches
   buffer.AppendFormatted("void %sEventLoop::UpdateTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("   if (gAnalyzer->GetTaskSwitches()->%s)\n",taskName[i].Data());
      buffer.AppendFormatted("      gAnalyzer->Get%sTask()->SetActive(true);\n",taskName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      gAnalyzer->Get%sTask()->SetActive(false);\n",taskName[i].Data());
   }
   buffer.AppendFormatted("};\n\n");

   // Close cpp-File
   fileHandle = open(cppFile.Data(),O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb;i++) {
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

bool ROMEBuilder::WriteEventLoopH() {
   int i;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   ROMEString format;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
      if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
   }
   int taskLen=0;
   for (i=0;i<numOfTask;i++) {
      if (taskLen<(int)taskName[i].Length()) taskLen = taskName[i].Length();
   }




   // File name
   hFile.SetFormatted("%s/include/framework/%sEventLoop.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sEventLoop_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sEventLoop_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#include <TTree.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#include <ROMEString.h>\n");
   buffer.AppendFormatted("#include <ROMEEventLoop.h>\n");

   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("\nclass %sEventLoop : public ROMEEventLoop\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Fields
   buffer.AppendFormatted("protected:\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sEventLoop(const char *name,const char *title);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");

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

   // Trees
   buffer.AppendFormatted("   // Tree Methodes\n");
   buffer.AppendFormatted("   void InitTrees();\n");
   buffer.AppendFormatted("   void ConnectTrees();\n");
   buffer.AppendFormatted("   void FillTrees();\n");
   buffer.AppendFormatted("\n");

   // Task Switches
   buffer.AppendFormatted("   // Task Switches\n");
   buffer.AppendFormatted("   void InitTaskSwitches();\n");
   buffer.AppendFormatted("   void UpdateTaskSwitches();\n");
   buffer.AppendFormatted("\n");

//   buffer.AppendFormatted("   ClassDef(%sEventLoop,0);\n",shortCut.Data());

   // Footer
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sEventLoop_H\n",shortCut.Data());

   // Write File
   fileHandle = open(hFile.Data(),O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb;i++) {
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

bool ROMEBuilder::WriteMain() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   cppFile.SetFormatted("%s/src/framework/main.cpp",outDir.Data());

   buffer.Resize(0);
   buffer.AppendFormatted("#include <TRint.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <Riostream.h>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("int main(int argc, char *argv[])\n");
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int argn = 1;\n");
   buffer.AppendFormatted("   char arg[1][100];\n");
   buffer.AppendFormatted("   char *argp = &arg[0][0];\n");
   buffer.AppendFormatted("   strcpy(arg[0],argv[0]);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TRint *app = new TRint(\"App\", &argn, &argp,NULL,0,true);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gAnalyzer = new %sAnalyzer(app);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TFolder *fMainFolder = gROOT->GetRootFolder()->AddFolder(\"ROME\",\"ROME Folder\");\n");
   buffer.AppendFormatted("   fMainFolder->Add(gAnalyzer);\n");
   buffer.AppendFormatted("   app->ProcessLine(\"%sAnalyzer* gAnalyzer = ((%sAnalyzer*)((TFolder*)gROOT->FindObjectAny(\\\"ROME\\\"))->GetListOfFolders()->MakeIterator()->Next());\");\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (!gAnalyzer->Start(argc, argv)) {\n");
   buffer.AppendFormatted("      delete gAnalyzer;\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   delete gAnalyzer;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");

   fileHandle = open(cppFile.Data(),O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb;i++) {
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

void usage() {
   cout << "  -i        Inputfile" << endl;
   cout << "  -o        Outputfile path" << endl;
   cout << "  -v        Verbose Mode (no Argument)" << endl;
   cout << "  -nl       No Linking (no Argument)" << endl;
   cout << "  -midas    Generated program can be connected to a midas online system (no Argument)" << endl;
   cout << "  -nosql    Generated program has no SQL data base access (no sql library needed) (no Argument)" << endl;
}

int main(int argc, char *argv[])
{
   struct stat buf;

   ROMEBuilder* romeb = new ROMEBuilder();

   romeb->romeVersion = "Version 1.00";
   
   ROMEString xmlFile = "";

   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);

   romeb->makeOutput = false;
   romeb->noLink = false;
   romeb->midas = false;
   romeb->sql = true;

   romeb->outDir = workDir;
   romeb->outDir.Append("/");

   if (argc==1) {
      usage();
      return 0;
   }
   for (int i=1;i<argc;i++) {
      // -- only for testing (start) --
      if (!strcmp(argv[i],"-dc")) {
         romeb->makeOutput = true;
         romeb->noLink = true;
         romeb->midas = true;
         romeb->outDir = "C:/Data/analysis/MEG/ROME .NET/DCAnalyzer/";
         xmlFile = "C:/Data/analysis/MEG/ROME .NET/DCAnalyzer/dc.xml";
      }
      else if (!strcmp(argv[i],"-meg")) {
         romeb->makeOutput = true;
         romeb->midas = true;
         romeb->noLink = true;
         romeb->outDir = "C:/Data/analysis/MEG/ROME .NET/MEGFrameWork/";
         xmlFile = "C:/Data/analysis/MEG/ROME .NET/MEGFrameWork/MEGFrameWork.xml";
      }
      // -- only for testing (end) --
      else if (!strcmp(argv[i],"-v")) {
         romeb->makeOutput = true;
      }
      else if (!strcmp(argv[i],"-nl")) {
         romeb->noLink = true;
      }
      else if (!strcmp(argv[i],"-nosql")) {
         romeb->sql = false;
      }
      else if (!strcmp(argv[i],"-midas")) {
         romeb->midas = true;
         ROMEString midasFile;
         midasFile = getenv("MIDASSYS");
         midasFile.Append("/include/midas.h");
         if( stat( midasFile, &buf )) {
            cout << "Midas library not found. Have you set the MIDASSYS environment variable ?" << endl;
            return 1;
         }
      }
      else if (!strcmp(argv[i],"-i")&&i<argc-1) {
         xmlFile = argv[i+1];
         i++;
      }
      else if (!strcmp(argv[i],"-o")&&i<argc-1) {
         romeb->outDir = argv[i+1];
         if (romeb->outDir[romeb->outDir.Length()-1]!='/' && romeb->outDir[romeb->outDir.Length()-1]!='\\') 
            romeb->outDir.Append("/");
         i++;
      }
      else if (argv[i][0]=='-') {
         usage();
         return 0;
      }
      else {
         xmlFile = argv[i];
      }
   }

   if( stat( xmlFile.Data(), &buf )) {
      if ( xmlFile == "")
         cout << "No inputfile specified." << endl;
      else
         cout << "Inputfile '" << xmlFile.Data() << "' not found." << endl;
      return 1;
   }
   ROMEString path;
   path = romeb->outDir;
   path.Remove(path.Length()-1);
   if (stat( path, &buf )) {
      cout << "Outputpath '" << romeb->outDir.Data() << "' not found." << endl;
      return 1;
   }
#if defined( _MSC_VER )
   path.SetFormatted("%s/src",romeb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/src/tasks",romeb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/src/framework",romeb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/include/",romeb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/include/tasks",romeb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/include/framework",romeb->outDir.Data());
   mkdir(path);
#endif

#if defined ( __linux__ )
   path.SetFormatted("%s/src",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/src/tasks",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/src/framework",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/include/",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/include/tasks",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/include/framework",romeb->outDir.Data());
   mkdir(path,0711);
#endif

   romeb->startBuilder((char*)xmlFile.Data());

   return 0;
}
void ROMEBuilder::startBuilder(char* xmlFile) 
{
   xml = new ROMEXML();

   char* name;
   bool finished = false;
   int type;
   int i,j;
   
   numOfFolder = 0;
   numOfTask = 0;
   numOfTree = 0;
   numOfBank = 0;

   experimentName = "";
   shortCut = "";
   mainProgName = "";
   mainDescription = "";
   mainAuthor = "";
   mainInstitute = "";
   mainCollaboration = "";
   mainEmail = "";

   readExperiment = false;
   readAuthor = false;
   readFolders = false;
   readTasks = false;
   readTrees = false;
   readGlobalSteeringParameters = false;
   readMidasBanks = false;

   if (!xml->OpenFileForRead(xmlFile)) return;
   while (xml->NextLine()&&!finished) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"ROMEFrameworkDefinition")) {
         while (xml->NextLine()&&!finished) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 15 && !strcmp((const char*)name,"ROMEFrameworkDefinition")) {
               if (!readExperiment) {
                  cout << "Experiment tree missing!" << endl;
                  cout << "Terminating program." << endl;
                  return;
               }
               finished = true;
               break;
            }
            if (type == 1) {
               if (!strcmp((const char*)name,"Experiment")) {
                  readExperiment = true;
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp((const char*)name,"ExperimentName"))
                        xml->GetValue(experimentName,experimentName);
                     if (type == 1 && !strcmp((const char*)name,"ExperimentShortCut"))
                        xml->GetValue(shortCut,shortCut);
                     if (type == 1 && !strcmp((const char*)name,"ProgramName"))
                        xml->GetValue(mainProgName,mainProgName);
                     if (type == 1 && !strcmp((const char*)name,"FrameworkDescription"))
                        xml->GetValue(mainDescription,mainDescription);
                     if (type == 15 && !strcmp((const char*)name,"Experiment")) {
                        if (shortCut=="") {
                           cout << "Experiment must have a shortcut!" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                        break;
                     }
                  }
                  continue;
               }
               if (!strcmp((const char*)name,"Author")) {
                  readAuthor = true;
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp((const char*)name,"AuthorName"))
                        xml->GetValue(mainAuthor,mainAuthor);
                     if (type == 1 && !strcmp((const char*)name,"AuthorInstitute"))
                        xml->GetValue(mainInstitute,mainInstitute);
                     if (type == 1 && !strcmp((const char*)name,"AuthorCollaboration"))
                        xml->GetValue(mainCollaboration,mainCollaboration);
                     if (type == 1 && !strcmp((const char*)name,"AuthorEmail"))
                        xml->GetValue(mainEmail,mainEmail);
                     if (type == 15 && !strcmp((const char*)name,"Author"))
                        break;
                  }
                  continue;
               }
               if (!strcmp((const char*)name,"Folders")) {
                  // initialization
                  numOfFolder = -1;
                  parent[0] = "GetMainFolder()";
                  // output
                  if (makeOutput) cout << "Folders:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // folder
                     if (type == 1 && !strcmp((const char*)name,"Folder")) {
                        recursiveDepth = 0;
                        if (!ReadXMLFolder()) return;
                     }
                     // folders end
                     if (type == 15 && !strcmp((const char*)name,"Folders")) {
                        break;
                     }
                  }
                  // check input
                  for (i=0;i<numOfFolder;i++) {
                     for (j=i+1;j<numOfFolder;j++) {
                        if (folderName[i]==folderName[j]) {
                           cout << "\nFolder '" << folderName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                     }
                  }
                  // count folders
                  numOfFolder++;
                  // write folder classes
                  if (!WriteFolderCpp()) return;
                  if (!WriteFolderH()) return;
               }
               if (!strcmp((const char*)name,"Tasks")) {
                  // initialization
                  numOfTask = -1;
                  parent[0] = "GetMainTask()";
                  // output
                  if (makeOutput) cout << "\n\nTasks:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // task
                     if (type == 1 && !strcmp((const char*)name,"Task")) {
                        recursiveDepth = 0;
                        if (!ReadXMLTask()) return;
                     }
                     // tasks end
                     if (type == 15 && !strcmp((const char*)name,"Tasks")) {
                        break;
                     }
                  }
                  // check input
                  for (i=0;i<numOfTask;i++) {
                     for (j=i+1;j<numOfTask;j++) {
                        if (taskName[i] == taskName[j]) {
                           cout << "\nTask '" << taskName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                     }
                  }
                  // count tasks
                  numOfTask++;
                  // write task classes
                  if (!WriteTaskCpp()) return;
                  if (!WriteTaskF()) return;
                  if (!WriteTaskH()) return;
               }
               if (!strcmp((const char*)name,"Trees")) {
                  numOfTree = -1;
                  if (!ReadXMLTree()) return;
               }
               if (!strcmp((const char*)name,"MidasBanks")) {
                  numOfBank = -1;
                  if (!ReadXMLMidasBanks()) return;
               }
               if (!strcmp((const char*)name,"GlobalSteeringParameters")) {
                  // output
                  if (makeOutput) cout << "\n\nGlobal Steering Parameters:" << endl;
                  // initialisation
                  steerName[numOfTask][0] = "GlobalSteering";
                  recursiveSteerDepth = 0;
                  parentSteer[0] = "";
                  numOfSteering[numOfTask] = -1;
                  if (!ReadXMLSteering(numOfTask)) return;
                  numOfSteering[numOfTask]++;
                  if (!WriteSteering(numOfTask)) return;
               }
            }
         }
      }
   }
   if (makeOutput) cout << "\n\nFramework :" << endl;
   if (!WriteAnalyzerCpp()) return;
   if (!WriteAnalyzerH()) return;
   if (!WriteEventLoopCpp()) return;
   if (!WriteEventLoopH()) return;
   if (!WriteMain()) return;
   delete xml;

   ROMEString buffer;
// Dictionary
   if (makeOutput) cout << "\nExecuting 'rootcint' for Root-Dictionary generation." << endl;
   WriteDictionaryBat(buffer);
   chdir(outDir.Data());
   system(buffer);

// Linking
   if (makeOutput && !noLink) cout << "\nLinking " << shortCut << " Project." << endl;
   WriteMakefile();
   if (!noLink) {
#if defined ( __linux__ )
      system("make -e");
#endif
#if defined( _MSC_VER )
   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);
   cout << "working dir = " << workDir << endl;
      system("nmake -f Makefile.win");
#endif
   }
// Documentation
   if (makeOutput) cout << "\nWrite HTML Documentation." << endl;
   WriteHTMLDoku();
}

void ROMEBuilder::WriteMakefile() {
   // write a Makefile
   ROMEString buffer;
   int i;

   ROMEString shortcut(shortCut);
   shortcut.ToLower();
   ROMEString mainprogname(mainProgName);
   mainprogname.ToLower();
#if defined( _MSC_VER )
   // libs
   buffer.Resize(0);
//   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib $(ROOTSYS)/lib/libVMC.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libGeomPainter.lib $(ROOTSYS)/lib/libMLP.lib $(ROOTSYS)/lib/libProof.lib $(ROOTSYS)/lib/libProofGui.lib $(ROOTSYS)/lib/libRGL.lib $(ROOTSYS)/lib/libfreetype.lib\n");
   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib \n");
   buffer.AppendFormatted("xmllibs = $(ROMESYS)/lib_win/libxml2.lib $(ROMESYS)/lib_win/iconv.lib $(ROMESYS)/lib_win/zlib.lib\n");
   if (this->sql) 
      buffer.AppendFormatted("sqllibs = $(ROMESYS)/lib_win/libmySQL.lib $(ROMESYS)/lib_win/mysys.lib $(ROMESYS)/lib_win/mysqlclient.lib\n");
   else
      buffer.AppendFormatted("sqllibs = \n");
   if (this->midas) 
      buffer.AppendFormatted("midaslibs = $(MIDASSYS)/nt/lib/midas.lib\n");
   else
      buffer.AppendFormatted("midaslibs = \n");
   buffer.AppendFormatted("clibs = wsock32.lib gdi32.lib user32.lib kernel32.lib\n");
   buffer.AppendFormatted("Libraries = $(rootlibs) $(xmllibs) $(clibs) $(sqllibs) $(midaslibs)\n");
   buffer.AppendFormatted("\n");
   // flags
   buffer.AppendFormatted("Flags = /GX /GR $(%suserflags)",shortcut.Data());
   if (this->midas) 
      buffer.AppendFormatted(" /DHAVE_MIDAS");
   if (this->sql) 
      buffer.AppendFormatted(" /DHAVE_SQL");
   buffer.AppendFormatted("\n");
   // includes
   buffer.AppendFormatted("Includes = /I$(ROMESYS)/include/ /I$(ROOTSYS)/include/ /I. /Iinclude/ /Iinclude/tasks/ /Iinclude/framework/ ");
   if (this->midas) 
      buffer.AppendFormatted(" /I$(MIDASSYS)/include/");
   if (this->sql) 
      buffer.AppendFormatted(" /I$(ROMESYS)/include/mysql/");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   // objects
   buffer.AppendFormatted("objects =");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormatted(" obj/%s%s.obj",shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted(" obj/%sT%s.obj",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted(" obj/%sAnalyzer.obj obj/%sEventLoop.obj obj/%sDict.obj obj/main.obj",shortCut.Data(),shortCut.Data(),shortCut.Data());
   if (this->sql) 
      buffer.AppendFormatted(" obj/ROMESQL.obj obj/ROMESQLDatabase.obj");
   buffer.AppendFormatted(" obj/ROMEAnalyzer.obj obj/ROMEEventLoop.obj obj/ROMETask.obj obj/ROMESplashScreen.obj obj/ROMEXML.obj obj/ROMEString.obj obj/ROMEXMLDatabase.obj\n\n");
   // all
   buffer.AppendFormatted("all:obj %s%s.exe\n",shortCut.Data(),mainProgName.Data());
   // make obj
   buffer.AppendFormatted("obj:\n");
	buffer.AppendFormatted("\tif not exist obj mkdir obj\n");
   // link
   buffer.AppendFormatted("%s%s.exe: $(objects)\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("	cl /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
   // compile
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormatted("obj/%s%s.obj: src/framework/%s%s.cpp\n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
      buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/%s%s.obj src/framework/%s%s.cpp \n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("obj/%sT%s.obj: src/tasks/%sT%s.cpp\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/%sT%s.obj src/tasks/%sT%s.cpp \n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("obj/%sAnalyzer.obj: src/framework/%sAnalyzer.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/%sAnalyzer.obj src/framework/%sAnalyzer.cpp \n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("obj/%sEventLoop.obj: src/framework/%sEventLoop.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/%sEventLoop.obj src/framework/%sEventLoop.cpp \n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("obj/main.obj: src/framework/main.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/main.obj src/framework/main.cpp \n");

   buffer.AppendFormatted("obj/ROMEAnalyzer.obj: $(ROMESYS)/src/ROMEAnalyzer.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMEAnalyzer.obj $(ROMESYS)/src/ROMEAnalyzer.cpp \n");
   buffer.AppendFormatted("obj/ROMEEventLoop.obj: $(ROMESYS)/src/ROMEEventLoop.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMEEventLoop.obj $(ROMESYS)/src/ROMEEventLoop.cpp \n");
   buffer.AppendFormatted("obj/ROMETask.obj: $(ROMESYS)/src/ROMETask.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMETask.obj $(ROMESYS)/src/ROMETask.cpp \n");
   buffer.AppendFormatted("obj/ROMESplashScreen.obj: $(ROMESYS)/src/ROMESplashScreen.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMESplashScreen.obj $(ROMESYS)/src/ROMESplashScreen.cpp \n");
   buffer.AppendFormatted("obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMEXML.obj $(ROMESYS)/src/ROMEXML.cpp \n");
   buffer.AppendFormatted("obj/ROMEString.obj: $(ROMESYS)/src/ROMEString.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMEString.obj $(ROMESYS)/src/ROMEString.cpp \n");
   buffer.AppendFormatted("obj/ROMEXMLDataBase.obj: $(ROMESYS)/src/ROMEXMLDataBase.cpp\n");
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMEXMLDataBase.obj $(ROMESYS)/src/ROMEXMLDataBase.cpp \n");
   if (this->sql) {
      buffer.AppendFormatted("obj/ROMESQLDataBase.obj: $(ROMESYS)/src/ROMESQLDataBase.cpp\n");
      buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMESQLDataBase.obj $(ROMESYS)/src/ROMESQLDataBase.cpp \n");
      buffer.AppendFormatted("obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp\n");
      buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/ROMESQL.obj $(ROMESYS)/src/ROMESQL.cpp \n");
   }

   buffer.AppendFormatted("obj/%sDict.obj: %sDict.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("	cl $(Flags) $(Includes) /c /Foobj/%sDict.obj %sDict.cpp \n",shortCut.Data(),shortCut.Data());
#endif

#if defined ( __linux__ )
   // libs
   buffer.Resize(0);
   buffer.AppendFormatted("rootlibs := $(shell root-config --libs)\n");
   buffer.AppendFormatted("rootglibs := $(shell root-config --glibs)\n");
   buffer.AppendFormatted("rootthreadlibs := -lThread\n");
   buffer.AppendFormatted("xmllibs := -lxml2 -lz\n");
   if (this->sql) 
      buffer.AppendFormatted("sqllibs := -lmysql\n");
   else
      buffer.AppendFormatted("sqllibs := \n");
   if (this->midas) 
      buffer.AppendFormatted("midaslibs := -lmidas\n");
   else
      buffer.AppendFormatted("midaslibs := \n");
   buffer.AppendFormatted("clibs := -lpthread -lHtml $(SYSLIBS)\n");
   buffer.AppendFormatted("Libraries := $(rootlibs) $(rootglibs) $(rootthreadlibs) $(xmllibs) $(clibs) $(sqllibs) $(midaslibs)\n");
   buffer.AppendFormatted("\n");
   // flags
   buffer.AppendFormatted("flags := $(%suserflags)",shortcut.Data());
   if (this->midas) 
      buffer.AppendFormatted(" -DHAVE_MIDAS");
   if (this->sql) 
      buffer.AppendFormatted(" -DHAVE_SQL");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   // includes
   buffer.AppendFormatted("Includes := -I$(ROMESYS)/include/ -I$(ROOTSYS)/include/ -I. -Iinclude/ -Iinclude/tasks/ -Iinclude/framework/ ");
   if (this->midas) 
      buffer.AppendFormatted(" -I$(MIDASSYS)/include/");
   if (this->sql) 
      buffer.AppendFormatted(" -I$(ROMESYS)/include/mysql/");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   // objects
   buffer.AppendFormatted("objects :=");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormatted(" obj/%s%s.obj",shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted(" obj/%sT%s.obj",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted(" obj/%sAnalyzer.obj obj/%sEventLoop.obj obj/%sDict.obj obj/main.obj",shortCut.Data(),shortCut.Data(),shortCut.Data());
   if (this->sql)
      buffer.AppendFormatted(" obj/ROMESQL.obj obj/ROMESQLDataBase.obj");
   buffer.AppendFormatted(" obj/ROMEAnalyzer.obj obj/ROMEEventLoop.obj obj/ROMETask.obj obj/ROMESplashScreen.obj obj/ROMEXML.obj obj/ROMEString.obj obj/ROMEXMLDataBase.obj\n\n");
   // all
   buffer.AppendFormatted("all:obj %s%s.exe\n",shortcut.Data(),mainprogname.Data());
   // make obj
   buffer.AppendFormatted("obj:\n");
	buffer.AppendFormatted("\t@if [ ! -d  obj ] ; then \\\n");
	buffer.AppendFormatted("\t\techo \"Making directory obj\" ; \\\n");
	buffer.AppendFormatted("\t\tmkdir obj; \\\n");
	buffer.AppendFormatted("\tfi;\n");
   // link
   buffer.AppendFormatted("%s%s.exe: $(objects)\n",shortcut.Data(),mainprogname.Data());
   buffer.AppendFormatted("	g++ -g -o $@ $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
   // compile
   for (i=0;i<numOfFolder;i++) {
      buffer.AppendFormatted("obj/%s%s.obj: src/framework/%s%s.cpp\n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
      buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) src/framework/%s%s.cpp -o obj/%s%s.obj\n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("obj/%sT%s.obj: src/tasks/%sT%s.cpp\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) src/tasks/%sT%s.cpp -o obj/%sT%s.obj\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("obj/%sAnalyzer.obj: src/framework/%sAnalyzer.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) src/framework/%sAnalyzer.cpp -o obj/%sAnalyzer.obj\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("obj/%sEventLoop.obj: src/framework/%sEventLoop.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) src/framework/%sEventLoop.cpp -o obj/%sEventLoop.obj\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("obj/main.obj: src/framework/main.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) src/framework/main.cpp -o obj/main.obj\n");

   buffer.AppendFormatted("obj/ROMEAnalyzer.obj: $(ROMESYS)/src/ROMEAnalyzer.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMEAnalyzer.cpp -o obj/ROMEAnalyzer.obj\n");
   buffer.AppendFormatted("obj/ROMEEventLoop.obj: $(ROMESYS)/src/ROMEEventLoop.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMEEventLoop.cpp -o obj/ROMEEventLoop.obj\n");
   buffer.AppendFormatted("obj/ROMETask.obj: $(ROMESYS)/src/ROMETask.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMETask.cpp -o obj/ROMETask.obj\n");
   buffer.AppendFormatted("obj/ROMESplashScreen.obj: $(ROMESYS)/src/ROMESplashScreen.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMESplashScreen.cpp -o obj/ROMESplashScreen.obj\n");
   buffer.AppendFormatted("obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMEXML.cpp -o obj/ROMEXML.obj\n");
   buffer.AppendFormatted("obj/ROMEString.obj: $(ROMESYS)/src/ROMEString.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMEString.cpp -o obj/ROMEString.obj\n");
   buffer.AppendFormatted("obj/ROMEXMLDataBase.obj: $(ROMESYS)/src/ROMEXMLDataBase.cpp\n");
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMEXMLDataBase.cpp -o obj/ROMEXMLDataBase.obj\n");
   if (this->sql) {
      buffer.AppendFormatted("obj/ROMESQLDataBase.obj: $(ROMESYS)/src/ROMESQLDataBase.cpp\n");
      buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) $(ROMESYS)/src/ROMESQLDataBase.cpp -o obj/ROMESQLDataBase.obj\n");
      buffer.AppendFormatted("obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp\n");
      buffer.AppendFormatted("	g++ -c $(flags) $(Includes) $(ROMESYS)/src/ROMESQL.cpp -o obj/ROMESQL.obj\n");
   }
   buffer.AppendFormatted("obj/%sDict.obj: %sDict.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("	g++ -g -c $(flags) $(Includes) %sDict.cpp -o obj/%sDict.obj\n",shortCut.Data(),shortCut.Data());
#endif
   ROMEString makeFile;
#if defined ( __linux__ )
   makeFile = "Makefile";
#endif
#if defined ( _MSC_VER )
   makeFile = "Makefile.win";
#endif
   int fileHandle = open(makeFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(makeFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer.Data(), buffer.Length());
   close(fileHandle);
}

void ROMEBuilder::WriteDictionaryBat(ROMEString& buffer) 
{
   // writes a script file that executes rootcint
   int i;

   buffer.Resize(0);
   buffer.AppendFormatted("rootcint -f %sDict.cpp -c ",shortCut.Data());
#if defined( _MSC_VER )
   buffer.AppendFormatted("-I%%ROMESYS%%/include ");
   buffer.AppendFormatted("-I%%ROOTSYS%% ");
#endif
#if defined ( __linux__ )
   buffer.AppendFormatted("-I$ROMESYS/include ");
   buffer.AppendFormatted("-I$ROOTSYS ");
#endif
   buffer.AppendFormatted("-Iinclude -Iinclude/tasks -Iinclude/framework ");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("include/framework/%s%s.h ",shortCut.Data(),folderName[i].Data());
         if (folderUserCode[i])
            buffer.AppendFormatted("include/framework/%s%s_Base.h ",shortCut.Data(),folderName[i].Data());
      }
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("include/tasks/%sT%s.h ",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("ROMETask.h ROMETreeInfo.h ROMEAnalyzer.h include/framework/%sAnalyzer.h\n",shortCut.Data());
   buffer.Append("\0");

#if defined( _MSC_VER )
   ROMEString batFile;
   batFile.SetFormatted("%smakeDictionary.bat",outDir.Data());
   int fileHandle = open(batFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(batFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer.Data(), buffer.Length());
   close(fileHandle);
#endif
}

void ROMEBuilder::WriteHTMLDoku() {

   int i=0,j=0,k=0;
   ROMEString buffer;
   ROMEString parentt;
   int depthold=0;
   int depth=0;

   // Header
   buffer.Resize(0);
   buffer.AppendFormatted("<HTML>\n");
   buffer.AppendFormatted("<HEAD>\n");
   buffer.AppendFormatted("<TITLE>%s%s Manual</TITLE>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("</HEAD>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<H1>%s%s Manual</H1>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   // Table of Contents
   buffer.AppendFormatted("<H2>Table of Contents</H2>\n");
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#introduction\">Introduction</a></li>\n");
   buffer.AppendFormatted("<br>\n");
   buffer.AppendFormatted("<li><a href=\"#objects\">Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#taskobjects\">Tasks</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#folderobjects\">Folders</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#treeobjects\">Trees</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#midasbankobjects\">Midas Banks</a></li>\n");
   buffer.AppendFormatted("<br>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#foldermethods\">Folders</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#databasemethods\">Data Base</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#midasbankmethods\">Midas Banks</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#histogrammethods\">Histograms</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#generalmethods\">General</a></li>\n");
   buffer.AppendFormatted("<br>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/htmldoc/ClassIndex.html\">Class Overview</A></li>\n",outDir.Data());
   buffer.AppendFormatted("<br>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
   // Introduction
   buffer.AppendFormatted("<H2><a name=introduction>Introduction</a> </H2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%s\n",mainDescription.Data());
   buffer.AppendFormatted("<p>\n");
   // Objects
   buffer.AppendFormatted("<H2><a name=objects>Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("All <a href=\"#taskobjects\">Tasks</a>, <a href=\"#folderobjects\">Folders</a>, <a href=\"#treeobjects\">Trees</a> and <a href=\"#midasbankobjects\">Midas Banks</a> are described here.\n");
   // Tasks
   buffer.AppendFormatted("<h3><a name=taskobjects>Tasks</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s consists of the following tasks :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

   depthold=0;
   depth=0;
   for (i=0;i<numOfTask;i++) {
      depth=0;
      if (taskParentName[i]!="GetMainTask()") {
         depth++;
         parentt = taskParentName[i];
         for (j=0;j<100;j++) {
            for (k=0;k<numOfTask;k++) {
               if (parentt==taskName[k]) break;
            }
            if (k>=numOfTask) {
               cout << "Invalid task structure." << endl;
               return;
            }
            if (taskParentName[k]=="GetMainTask()") break;
            depth++;
         }
      }
      if (depth<depthold) buffer.AppendFormatted("</ul>\n");
      if (depth>depthold) buffer.AppendFormatted("<ul>\n");
      buffer.AppendFormatted("<li type=\"circle\"><h4><a href=\"#%s\">%sT%s</a></h4></li>\n",taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<p><b>Note</b> : The user should write code into the Init(), BeginOfRun(), Event(), EndOfRun() and Terminate() methods of the tasks methods file (.cpp). But the user may not add code to or alter code in the class file (.h). The class file (.h) is generated by the framework. Manual changes to this file will allways be overwritten.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("In the following all tasks will be described.\n");
   buffer.AppendFormatted("<p>\n");

   ROMEString cppFile;
   ROMEString str;
   char fileBuffer[bufferLength];
   int fileHandle;
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("<h4><a name=%s><u>%s</u></a></h4>\n",taskName[i].Data(),taskName[i].Data());
      buffer.AppendFormatted("%s<p>\n",taskDescription[i].Data());
      if (numOfHistos[i]>0) {
         buffer.AppendFormatted("This task containes the following histograms :\n");
         buffer.AppendFormatted("<ul>\n");
         for (j=0;j<numOfHistos[i];j++) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",histoName[i][j].Data());
         }
         buffer.AppendFormatted("</ul>\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
      }
      else {
         buffer.AppendFormatted("This task containes no histograms.\n");
      }
      buffer.AppendFormatted("<p>\n");
      cppFile.SetFormatted("%s/src/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[i].Data());
      fileHandle = open(cppFile.Data(),O_RDONLY);
      int n = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      fileBuffer[n] = 0;
      buffer.AppendFormatted("%s accesses data from the following folders :\n",taskName[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j=0;j<numOfFolder;j++) {
         str = "Get";
         str.Append(folderName[j]);
         if (strstr(fileBuffer,str.Data())) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[j].Data());
         }
      }
      buffer.AppendFormatted("</ul>\n");

      close(fileHandle);
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">class file</a>\n",outDir.Data(),shortCut.Data(),taskName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Folders
   buffer.AppendFormatted("<h3><a name=folderobjects>Folders</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following folders :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");

   depthold=0;
   depth=0;
   for (i=0;i<numOfFolder;i++) {
      depth=0;
      if (folderParentName[i]!="GetMainFolder()") {
         depth++;
         parentt = folderParentName[i];
         for (j=0;j<100;j++) {
            for (k=0;k<numOfFolder;k++) {
               if (parentt==folderName[k]) break;
            }
            if (k>=numOfFolder) {
               cout << "Invalid folder structure." << endl;
               return;
            }
            if (folderParentName[k]=="GetMainFolder()") break;
            depth++;
         }
      }
      if (depth<depthold) buffer.AppendFormatted("</ul>\n");
      if (depth>depthold) buffer.AppendFormatted("<ul>\n");
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("<b>\n");
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%s\">%s</a></li>\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("</b>\n");
      }
      else {
         buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[i].Data());
      }
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("The bold folders are data folders. The others are only used to structure the frame work.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("In the following all folders will be described.\n");
   buffer.AppendFormatted("<p>\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] <= 0) continue;
      buffer.AppendFormatted("<h4><a name=%s><u>%s</u></a></h4>\n",folderName[i].Data(),folderName[i].Data());
      buffer.AppendFormatted("%s\n",folderDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<u>Fields</u>\n");
      buffer.AppendFormatted("<table border=\"1\">\n");
      buffer.AppendFormatted("<tr><td>Name</td><td>Type</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         buffer.AppendFormatted("<tr><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",valueName[i][j].Data(),valueType[i][j].Data());
      }
      buffer.AppendFormatted("</table>\n");

   }
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Trees
   buffer.AppendFormatted("<h3><a name=treeobjects>Trees</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following trees and branches.\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("In brackets are the folders, which are filled to the branch.\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",treeName[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j=0;j<numOfBranch[i];j++) {
         buffer.AppendFormatted("<li type=\"circle\">%s (%s)</li>\n",branchName[i][j].Data(),branchFolder[i][j].Data());
      }
      buffer.AppendFormatted("</ul>\n");
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Midas Banks
   buffer.AppendFormatted("<h3><a name=midasbankobjects>Midas Banks</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following midas banks :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   for (i=0;i<numOfBank;i++) {
      if (bankType[i]=="structure"||bankType[i]=="struct") {
         buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankName[i].Data());
         buffer.AppendFormatted("<ul>\n");
         for (j=0;j<numOfStructFields[i];j++) {
            buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",structFieldName[i][j].Data());
         }
         buffer.AppendFormatted("</ul>\n");
      }
      else {
         buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankName[i].Data());
      }
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Access Methods
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("In the following the access methods of all types of objects in the %s%s are discussed.\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("All task can access these methods over their <b>gAnalyzer</b> handle, which every task has by\n");
   buffer.AppendFormatted("default.<br>\n");
   buffer.AppendFormatted("<p>\n");

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<h3><a name=foldermethods>Folders</a></h3>\n");
   buffer.AppendFormatted("To access a folder one has to get a handle to it with the following methods : <p>\n");
   buffer.AppendFormatted("<table border=\"0\">\n");
   buffer.AppendFormatted("<tr><td><b>Get[<i>Folder Name</i>]At([<i>Index</i>])</b></td><td>&nbsp;&nbsp;&nbsp;for object arrays.</td></tr>\n");
   buffer.AppendFormatted("<tr><td><b>Get[<i>Folder Name</i>]Object()</td><td>&nbsp;&nbsp;&nbsp;for single objects.</td></tr>\n");
   buffer.AppendFormatted("</table>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<b><i>Folder Name</i></b> stands for the name of the folder specified in the xml file (see also list above).</br>\n");
   buffer.AppendFormatted("<b><i>Index</i></b> stands for the array index of the object.</br>\n");
   buffer.AppendFormatted("These methods return a pointer on the class of this folder.</br>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("The get or set the content of a folder one has to use the following methods : <p>\n");
   buffer.AppendFormatted("<table border=\"0\">\n");
   buffer.AppendFormatted("<tr><td><b>Get[<i>Field Name</i>]()</b></td><td>&nbsp;&nbsp;&nbsp;getter.</td></tr>\n");
   buffer.AppendFormatted("<tr><td><b>Set[<i>Field Name</i>]([<i>Value</i>])</td><td>&nbsp;&nbsp;&nbsp;setter.</td></tr>\n");
   buffer.AppendFormatted("</table>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<b><i>Field Name</i></b> stands for the name of the field to access specified in the xml file (see also list above).</br>\n");
   buffer.AppendFormatted("<b><i>Value</i></b> stands for the value given to the Field.</br>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<b>Example:</b> To give the field 'YY' of the 9 folder of a folderarray 'XX' the value 99 one has to type this :<p>\n");
   buffer.AppendFormatted("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;gAnalyzer->GetXXAt(9)->SetYY(99) <p>\n");
   buffer.AppendFormatted("<p>\n");

   // Data Base
   buffer.AppendFormatted("<h3><a name=databasemethods><u>Data Base</u></a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("To add an entry to the data base the following methods are available :\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<b>Write[<i>Folder Name</i>]DataBase(this)</b>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<b><i>Folder Name</i></b> stands for the name of the folder, which is to be written to the data base (see also list above).</br>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");

   // Midas Banks
   buffer.AppendFormatted("<h3><a name=midasbankmethods><u>Midas Banks</u></a></h3>\n");
   buffer.AppendFormatted("To access a bank in a midas input file the following methods are available :\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("One can get the entries in a bank with the following method :\n");
   buffer.AppendFormatted("<b>Get[<i>Bank Name</i>]BankEntries()</b>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The data can be accessed with :\n");
   buffer.AppendFormatted("<b>Get[<i>Bank Name</i>]BankAt([<i>Index</i>])</b>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<b><i>Bank Name</i></b> stands for the name of the bank.</br>\n");
   buffer.AppendFormatted("<b><i>Index</i></b> stands for the arrayindex of the value.</br>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("If the bank is a structured bank, the data access method returns a pointer on the structure.</br>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<b>Example:</b> To get the 9 value 'YY' of a structured bank 'XX' one has to type this :<p>\n");
   buffer.AppendFormatted("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;gAnalyzer->GetXXBankAt(9)->YY <p>\n");
   buffer.AppendFormatted("\n");

   // Histos
   buffer.AppendFormatted("<h3><a name=histogrammethods><u>Histograms</u></a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("Histograms belong to a task. So a task can only access his own histograms. To do this the following methods are available :\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<table border=\"0\">\n");
   buffer.AppendFormatted("<tr><td><b>Fill[<i>Histo Name</i>]([<i>xValue</i>],[<i>weight</i>])</b></td>  <td>&nbsp;&nbsp;&nbsp;fills a single histogram.</td></tr>\n");
   buffer.AppendFormatted("<tr><td><b>Draw[<i>Histo Name</i>]()</b></td>       <td>&nbsp;&nbsp;&nbsp;draws a single histogram.</td></tr>\n");
   buffer.AppendFormatted("<tr><td><b>Get[<i>Histo Name</i>]Handle()</b></td>  <td>&nbsp;&nbsp;&nbsp;gets the handle to a single histogram.</td></tr>\n");
   buffer.AppendFormatted("<tr><td><b>Fill[<i>Histo Name</i>]At([<i>Index</i>],[<i>xValue</i>],[<i>weight</i>])</b></td><td>&nbsp;&nbsp;&nbsp;fills a histogram of a histogram array.</td></tr>\n");
   buffer.AppendFormatted("<tr><td><b>Draw[<i>Histo Name</i>]At([<i>Index</i>])</b></td>     <td>&nbsp;&nbsp;&nbsp;draws a histogram of a histogram array.</td></tr>\n");
   buffer.AppendFormatted("<tr><td><b>Get[<i>Histo Name</i>]HandleAt([<i>Index</i>])</b></td><td>&nbsp;&nbsp;&nbsp;gets the handle to a histogram of a histogram array.</td></tr>\n");
   buffer.AppendFormatted("</table>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<b><i>Histo Name</i></b> stands for the name of the histogram.</br>\n");
   buffer.AppendFormatted("<b><i>Index</i></b> stands for the array index of the histogram.</br>\n");
   buffer.AppendFormatted("<b><i>xValue</i></b> value to be filled to the histogram.</br>\n");
   buffer.AppendFormatted("<b><i>weight</i></b> weight of the value.</br>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");

   // General Methods
   buffer.AppendFormatted("<h3><a name=generalmethods><u>General</u></a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%s%s provides some general methods for the user.\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<table border=\"0\">\n");
   buffer.AppendFormatted("<tr><td>isOnline()</td><td> : true if the program is running online.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>isOffline()</td><td> : true if the program is running offline.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>isRoot()</td><td> : true if the data is read from a root file.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>isMidas()</td><td> : true if the data has the midas format.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>isBatchMode()</td><td> : true if the program is running in batch mode.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>isEndOfRun()</td><td> : true if the EndOfRun flag is set.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>isBeginOfRun()</td><td> : true if the BeginOfRun flag is set.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>isTerminate()</td><td> : true if the Terminate flag is set.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>EndOfRun()</td><td> : sets the EndOfRun flag.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>BeginOfRun()</td><td> : sets the BeginOfRun flag.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>Terminate()</td><td> : sets the Terminate flag.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>GetConfigDir()</td><td> : returns the configuration directory.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>GetDataBaseDir()</td><td> : returns the data base directory.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>GetInputDir()</td><td> : returns the input directory.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>GetOutputDir()</td><td> : returns the output directory.</td></tr>\n");
   buffer.AppendFormatted("<tr><td>GetCurrentRunNumber()</td><td> : returns the current run number.</td></tr>\n");
   buffer.AppendFormatted("</table>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");

   // Footer
   buffer.AppendFormatted("<HR>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ADDRESS>\n");
   buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n",shortCut.Data());
   buffer.AppendFormatted("%s</br>\n",mainAuthor.Data());
   buffer.AppendFormatted("%s</br>\n",mainInstitute.Data());
   buffer.AppendFormatted("%s</br>\n",mainCollaboration.Data());
   buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail.Data(),mainEmail.Data());
   buffer.AppendFormatted("<u> Contact person from ROME</u></br>\n");
   buffer.AppendFormatted("Matthias Schneebeli (PSI)</br>\n");
   buffer.AppendFormatted("email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
   buffer.AppendFormatted("</ADDRESS>\n");
   buffer.AppendFormatted("</BODY>\n");
   buffer.AppendFormatted("</HTML>\n");

   ROMEString htmlFile;
   htmlFile.SetFormatted("%s%s%s.html",outDir.Data(),shortCut.Data(),mainProgName.Data());
   fileHandle = open(htmlFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(htmlFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer.Data(), buffer.Length());
   close(fileHandle);
   if (makeOutput) htmlFile.WriteLine();
}

void ROMEBuilder::GetFormat(ROMEString* buf,char *type) 
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
      buf->Append("%f");
   }
   else if (!strcmp(type,"bool") ||
      !strcmp(type,"Bool_t")) {
      buf->Append("%d");
   }
   else {
      buf->Append("%s");
   }
}

void ROMEBuilder::setValue(ROMEString* buf,char *destination,char *source,char *type,int version)
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
         buf->AppendFormatted("%s = toBool(strtol(%s,&cstop,10))",destination,source);
      else
         buf->AppendFormatted("toBool(strtol(%s,&cstop,10))",source);
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
bool ROMEBuilder::isFloatingType(char *type)
{
   if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      return true;
   }
   return false;
}
