/********************************************************************

  ROMEBuilder.cpp, M. Schneebeli PSI

  $Log$
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
   ROMEString parent[maxNumberOfFolders];
   ROMEString tmp;
   char* name;
   int type,i,j,isub=0;
   parent[0] = "GetMainFolder()";

   if (makeOutput) cout << "Folders:" << endl;
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // read subfolder
      if (type == 1 && !strcmp((const char*)name,"SubFolder")) {
         numOfFolder++;
         if (numOfFolder>=maxNumberOfFolders) {
            cout << "Maximal number of folders reached : " << maxNumberOfFolders << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // default initialisation
         folderAuthor[numOfFolder] = mainAuthor;
         folderVersion[numOfFolder] = "1";
         folderDescription[numOfFolder] = "";
         folderDataBase[numOfFolder] = false;
         folderUserCode[numOfFolder] = false;
         numOfValue[numOfFolder] = 0;
         // set parent
         folderParentName[numOfFolder] = parent[isub];
         // read folder name
         xml->GetAttribute("Name",folderName[numOfFolder],"");
         xml->GetAttribute("FolderName",folderName[numOfFolder],folderName[numOfFolder]);
         if (folderName[numOfFolder]=="") {
            cout << "Folder " << (numOfFolder+1) << " has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // read folder title
         xml->GetAttribute("Title",folderTitle[numOfFolder],"");
         xml->GetAttribute("FolderTitle",folderTitle[numOfFolder],folderTitle[numOfFolder]);
         // read array flag
         xml->GetAttribute("ArraySize",folderArray[numOfFolder],"1");
         xml->GetAttribute("Array",folderArray[numOfFolder],folderArray[numOfFolder]);
         // read data base flag
         xml->GetAttribute("DataBase",tmp,"no");
         if (tmp == "yes") 
            folderDataBase[numOfFolder] = true;
         // read user code flag
         xml->GetAttribute("UserCode",tmp,"no");
         if (tmp == "yes") 
            folderUserCode[numOfFolder] = true;

         // set folder as parent for subsequent folders
         isub++;
         parent[isub] = folderName[numOfFolder].Data();

         // output
         if (makeOutput) for (i=0;i<isub;i++) cout << "   ";
         if (makeOutput) folderName[numOfFolder].WriteLine();
      }
      // end subfolder
      if (type == 15 && !strcmp((const char*)name,"SubFolder")) {
         isub--;
      }
      // read folder content
      if (type == 1) {
         // read author
         if (!strcmp((const char*)name,"Author")) {
            xml->GetAttribute("Name",folderAuthor[numOfFolder],mainAuthor);
         }
         // read version
         else if (!strcmp((const char*)name,"Version")) {
            xml->GetAttribute("Number",folderVersion[numOfFolder],"1");
         }
         // read description
         else if (!strcmp((const char*)name,"Description")) {
            xml->GetAttribute("Text",folderDescription[numOfFolder],"");
         }
         // includes
         else if (!strcmp((const char*)name,"Include")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]] = (char*)name;
                  folderLocalFlag[numOfFolder][numOfFolderInclude[numOfFolder]] = false;
                  xml->GetAttribute("Type",tmp,"global");
                  if (tmp=="local") 
                     folderLocalFlag[numOfFolder][numOfFolderInclude[numOfFolder]] = true;
                  numOfFolderInclude[numOfFolder]++;
                  if (numOfFolderInclude[numOfFolder]>=maxNumberOfInclude) {
                     cout << "Maximal number of inludes in folder '" << folderName[numOfFolder].Data() << "' reached : " << maxNumberOfInclude << " !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
               if (type == 15 && !strcmp((const char*)name,"Include")) {
                  break;
               }
            }
         }
         // read fields
         else if (!strcmp((const char*)name,"Fields")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  // field name
                  valueName[numOfFolder][numOfValue[numOfFolder]] = name;
                  // field type
                  xml->GetAttribute("Type",valueType[numOfFolder][numOfValue[numOfFolder]],"");
                  if (valueType[numOfFolder][numOfValue[numOfFolder]]=="") {
                     cout << "Field '" << valueType[numOfFolder][numOfValue[numOfFolder]].Data() << "' of Folder '" << folderName[numOfFolder].Data() << "' has no Type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // field reference flag
                  xml->GetAttribute("Reference",valueRef[numOfFolder][numOfValue[numOfFolder]],"no");
                  // field initialisation
                  if (valueType[numOfFolder][numOfValue[numOfFolder]] == "TString")
                     xml->GetAttribute("Initialisation",valueInit[numOfFolder][numOfValue[numOfFolder]],"' '");
                  else
                     xml->GetAttribute("Initialisation",valueInit[numOfFolder][numOfValue[numOfFolder]],"0");
                  xml->GetAttribute("Init",valueInit[numOfFolder][numOfValue[numOfFolder]],valueInit[numOfFolder][numOfValue[numOfFolder]]);
                  // field comment
                  xml->GetAttribute("Comment",valueComment[numOfFolder][numOfValue[numOfFolder]]," ");
                  if (valueComment[numOfFolder][numOfValue[numOfFolder]][0]!='/') {
                     valueComment[numOfFolder][numOfValue[numOfFolder]].Insert(0,"// ");
                  }
                  // data base path
                  valueDataBasePath[numOfFolder][numOfValue[numOfFolder]].AppendFormated("%s.%s",folderName[numOfFolder].Data(),valueName[numOfFolder][numOfValue[numOfFolder]].Data());
                  xml->GetAttribute("DataBasePath",valueDataBasePath[numOfFolder][numOfValue[numOfFolder]],valueDataBasePath[numOfFolder][numOfValue[numOfFolder]]);
                  // array
                  xml->GetAttribute("Array",valueArray[numOfFolder][numOfValue[numOfFolder]],"0");
                  xml->GetAttribute("ArraySize",valueArray[numOfFolder][numOfValue[numOfFolder]],valueArray[numOfFolder][numOfValue[numOfFolder]]);
                  if (valueArray[numOfFolder][numOfValue[numOfFolder]]!="0" && folderDataBase[numOfFolder]) {
                     cout << "Value '" << valueName[numOfFolder][numOfValue[numOfFolder]].Data() << "' of Folder '" << folderName[numOfFolder].Data() << "', which has data base access, can not be an array !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // field count
                  numOfValue[numOfFolder]++;
                  if (numOfValue[numOfFolder]>=maxNumberOfValues) {
                     cout << "Maximal number of fields in folder '" << folderName[numOfFolder].Data() << "' reached : " << maxNumberOfValues << " !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
               if (type == 15 && !strcmp((const char*)name,"Fields")) {
                  for (i=0;i<numOfValue[numOfFolder];i++) {
                     for (j=i+1;j<numOfValue[numOfFolder];j++) {
                        if (valueName[numOfFolder][i]==valueName[numOfFolder][j]) {
                           cout << "\nFolder '" << folderName[numOfFolder].Data() << "' has two fields with the name '" << valueName[numOfFolder][i].Data() << "' !" << endl;
                           cout << "Terminating program." << endl;
                           return false;
                        }
                     }
                  }
                  break;
               }
            }
         }
      }
      if (type == 15 && !strcmp((const char*)name,"Folder")) {
         for (i=0;i<numOfFolder;i++) {
            for (j=i+1;j<numOfFolder;j++) {
               if (folderName[i]==folderName[j]) {
                  cout << "\nFolder '" << folderName[i].Data() << "' is defined twice !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         break;
      }
   }
   numOfFolder++;
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
      cppFile.SetFormated("%s/src/framework/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());

      if (!folderUserCode[iFold]) {
         remove(cppFile.Data());
         continue;
      }

      // Description
      buffer.Resize(0);
      buffer.AppendFormated("//// Author: %s\n",folderAuthor[iFold].Data());
      buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormated("//                                                                            //\n");
      ll = 74-shortCut.Length();
      format.SetFormated("// %%s%%-%d.%ds //\n",ll,ll);
      buffer.AppendFormated((char*)format.Data(),shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormated("//                                                                            //\n");
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
         buffer.AppendFormated("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("/////////////////////////////////////----///////////////////////////////////////");

      // Write File
      struct stat buf;
      if( stat( cppFile.Data(), &buf )) {
         // Header Files
         buffer.AppendFormated("\n\n#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[iFold].Data());

         buffer.AppendFormated("\nClassImp(%s%s)\n",shortCut.Data(),folderName[iFold].Data());

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
         format.SetFormated("%%-%d.%ds",bufferLen-80,bufferLen-80);
         buffer.AppendFormated((char*)format.Data(),pBuffer+80);
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
         hFile.SetFormated("%s/include/framework/%s%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      else
         hFile.SetFormated("%s/include/framework/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());

      // Description
      buffer.Resize(0);
      buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormated("// If you intend to change this file please contact:                          //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// Manual changes to this file will always be overwritten by the builder.     //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n\n");

      // Header
      if (folderUserCode[iFold]) {
         buffer.AppendFormated("#ifndef %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormated("#define %s%s_Base_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }
      else {
         buffer.AppendFormated("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormated("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }

      buffer.AppendFormated("#include <TObject.h>\n");

      for (i=0;i<numOfValue[iFold];i++) {
         if (valueRef[iFold][i]=="yes") {
            buffer.AppendFormated("#include <TRef.h>\n");
            break;
         }
      }

      for (i=0;i<numOfFolderInclude[iFold];i++) {
         if (folderLocalFlag[iFold][i]) {
            buffer.AppendFormated("#include \"%s\"\n",folderInclude[iFold][i].Data());
         }
         else {
            buffer.AppendFormated("#include <%s>\n",folderInclude[iFold][i].Data());
         }
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i]=="TString") {
            buffer.AppendFormated("#include <%s.h>\n",valueType[iFold][i].Data());
            break;
         }
      }

      for (i=0;i<numOfValue[iFold];i++) {
         for (j=0;j<numOfFolder;j++) {
            str.SetFormated("%s*",folderName[j].Data());
            if (valueType[iFold][i]==folderName[j] || valueType[iFold][i]==str) {
               buffer.AppendFormated("#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[j].Data());
               valueType[iFold][i].Insert(0,shortCut);
               break;
            }
         }
      }

      // Class
      if (folderUserCode[iFold])
         buffer.AppendFormated("\nclass %s%s_Base : public TObject\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormated("\nclass %s%s : public TObject\n",shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormated("{\n");

      // Fields
      buffer.AppendFormated("protected:\n");
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
         if (valueRef[iFold][i]=="yes") {
            format.SetFormated("   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormated((char*)format.Data(),"TRef",valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
         }
         else if (valueArray[iFold][i]!="0") {
            format.SetFormated("   %%-%ds f%%s[%s];%%%ds %%s\n",typeLen,valueArray[iFold][i].Data(),nameLen-valueName[iFold][i].Length()+2+valueArray[iFold][i].Length());
            buffer.AppendFormated((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
         }
         else {
            format.SetFormated("   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormated((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
         }
      }
      buffer.AppendFormated("\n");
      format.SetFormated("   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-8);
      buffer.AppendFormated((char*)format.Data(),"Bool_t","Modified","","//! Modified Folder Flag");
      buffer.AppendFormated("\n");

      // Methods

      buffer.AppendFormated("public:\n");
      // Constructor
      if (folderUserCode[iFold])
         buffer.AppendFormated("   %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormated("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0")
            buffer.AppendFormated("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
      }
      buffer.Resize(buffer.Length()-1);
      buffer.AppendFormated(" )\n");
      buffer.AppendFormated("   { ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0")
            buffer.AppendFormated("f%s = %s; ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
         else {
            buffer.AppendFormated("for (int i%d=0;i%d<%s;i%d++) f%s[i%d] = %s; ",i,i,valueArray[iFold][i].Data(),i,valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
         }
      }
      buffer.AppendFormated("fModified = false; ");
      buffer.AppendFormated("};\n");
      buffer.AppendFormated("\n");

      // Getters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueRef[iFold][i]=="yes") {
            format.SetFormated("   %%-%ds Get%%s()%%%ds { return (%%s)f%%s.GetObject();%%%ds };\n",typeLen,lb,lb+(typeLen-valueType[iFold][i].Length()));
            buffer.AppendFormated((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"");
         }
         else if (valueArray[iFold][i]!="0") {
            format.SetFormated("   %%-%ds Get%%sAt(int index)%%%ds { return f%%s[index];%%%ds };\n",typeLen,lb,lb);
            buffer.AppendFormated((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
         }
         else {
            format.SetFormated("   %%-%ds Get%%s()%%%ds { return f%%s;%%%ds };\n",typeLen,lb,lb);
            buffer.AppendFormated((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
         }
      }
      buffer.AppendFormated("\n");

      // Modifier
      format.SetFormated("   %%-%ds is%%s()%%%ds  { return f%%s;%%%ds };\n",typeLen,nameLen-8,nameLen-8);
      buffer.AppendFormated((char*)format.Data(),"Bool_t","Modified","","Modified","");
      buffer.AppendFormated("\n");
      // Setters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueArray[iFold][i]=="0") {
            format.SetFormated("   void Set%%s%%%ds(%%-%ds %%s%%%ds) { f%%s%%%ds = %%s;%%%ds fModified = true; };\n",lb,typeLen,lb,lb,lb);
            buffer.AppendFormated((char*)format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
         }
         else {
            format.SetFormated("   void Set%%sAt%%%ds(int index,%%-%ds %%s%%%ds) { f%%s[index]%%%ds = %%s;%%%ds fModified = true; };\n",lb,typeLen,lb,lb,lb);
            buffer.AppendFormated((char*)format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
         }
      }
      buffer.AppendFormated("\n");
      // Set All
      buffer.AppendFormated("   void SetAll( ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0") {
            buffer.AppendFormated("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
      }
      buffer.Resize(buffer.Length()-1);
      buffer.AppendFormated(" )\n");
      buffer.AppendFormated("   { ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0") {
            buffer.AppendFormated("f%s = %s; ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
         }
      }
      buffer.AppendFormated("fModified = true; ");
      buffer.AppendFormated("};\n");
      buffer.AppendFormated("\n");
      // Reset
      buffer.AppendFormated("   void Reset() {");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueArray[iFold][i]=="0") {
            buffer.AppendFormated("f%s = %s; ",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         else {
            buffer.AppendFormated("for (int i%d=0;i%d<%s;i%d++) f%s[i%d] = %s; ",i,i,valueArray[iFold][i].Data(),i,valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
         }
      }
      buffer.AppendFormated("fModified = false; ");
      buffer.AppendFormated("};\n");

      // Footer
      if (folderUserCode[iFold])
         buffer.AppendFormated("\n   ClassDef(%s%s_Base,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
      else
         buffer.AppendFormated("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
      buffer.AppendFormated("};\n\n");

      if (folderUserCode[iFold])
         buffer.AppendFormated("#endif   // %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormated("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());

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
      hFile.SetFormated("%s/include/framework/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      if (folderUserCode[iFold] && stat( hFile.Data(), &buf )) {
         // Description
         buffer.Resize(0);
         buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormated("//                                                                            //\n");
         buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
         buffer.AppendFormated("// If you intend to change this file please contact:                          //\n");
         buffer.AppendFormated("//                                                                            //\n");
         buffer.AppendFormated("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
         buffer.AppendFormated("//                                                                            //\n");
         buffer.AppendFormated("// Manual changes to this file will always be overwritten by the builder.     //\n");
         buffer.AppendFormated("//                                                                            //\n");
         buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n\n");

         // Header
         buffer.AppendFormated("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormated("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());

         buffer.AppendFormated("#include <include/framework/%s%s_Base.h>\n",shortCut.Data(),folderName[iFold].Data());

         // Class
         buffer.AppendFormated("\nclass %s%s : public %s%s_Base\n",shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormated("{\n");
         buffer.AppendFormated("public:\n");
         buffer.AppendFormated("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="0")
               buffer.AppendFormated("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormated(" ) : %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="0")
               buffer.AppendFormated("%s,",valueName[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormated(" ) {}\n");
         buffer.AppendFormated("\n");
         buffer.AppendFormated("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
         buffer.AppendFormated("};\n\n");
         buffer.AppendFormated("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
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
   ROMEString parent[maxNumberOfTasks];
   ROMEString tmp;
   char* name;
   int type,i,j,isub=0;
   int empty,depth=0,index[20];
   parent[0] = "GetMainTask()";

   if (makeOutput) cout << "\n\nTasks:" << endl;
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      empty = xml->isEmpty();
      if (type == 1 && !strcmp((const char*)name,"SubTask")) {
         numOfTask++;
         if (numOfTask>=maxNumberOfTasks) {
            cout << "Maximal number of tasks reached : " << maxNumberOfTasks << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // initialisation
         index[0] = 0;
         numOfTaskSteering[numOfTask] = 0;
         numOfTaskSteerFields[numOfTask][0] = 0;
         numOfHistos[numOfTask] = 0;
         numOfTaskInclude[numOfTask] = 0;
         taskSteerDepth[numOfTask][0] = 0;
         taskAuthor[numOfTask] = mainAuthor;
         taskVersion[numOfTask] = "1";
         taskDescription[numOfTask] = "";

         taskSteerName[numOfTask][0] = "Steering";
         taskSteerParent[numOfTask][0] = "";
         taskParentName[numOfTask] = parent[isub];

         // task name
         xml->GetAttribute("Name",taskName[numOfTask],"");
         if (taskName[numOfTask]=="") {
            cout << "Task " << (numOfTask+1) << " has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // trigger id
         xml->GetAttribute("EventID",taskEventID[numOfTask],"all");
         // language
         taskFortran[numOfTask] = false;
         xml->GetAttribute("Language",tmp,"c++");
         if (tmp == "Fortran") 
            taskFortran[numOfTask] = true;

         // output
         if (makeOutput) for (i=0;i<isub+1;i++) cout << "   ";
         if (makeOutput) taskName[numOfTask].WriteLine();

         // handle subtask
         if (!empty) {
            isub++;
            parent[isub] = taskName[numOfTask];
         }
      }
      if (type == 15 && !strcmp((const char*)name,"SubTask")) {
         isub--;
      }
      if (type == 1) {
         // author
         if (!strcmp((const char*)name,"Author")) {
            xml->GetAttribute("Name",taskAuthor[numOfTask],mainAuthor);
         }
         // version
         else if (!strcmp((const char*)name,"Version")) {
            xml->GetAttribute("Number",taskVersion[numOfTask],"1");
         }
         // description
         else if (!strcmp((const char*)name,"Description")) {
            xml->GetAttribute("Text",taskDescription[numOfTask],"");
         }
         // includes
         else if (!strcmp((const char*)name,"Include")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  taskInclude[numOfTask][numOfTaskInclude[numOfTask]] = (char*)name;
                  taskLocalFlag[numOfTask][numOfTaskInclude[numOfTask]] = false;
                  xml->GetAttribute("Type",tmp,"global");
                  if (tmp=="local") 
                     taskLocalFlag[numOfTask][numOfTaskInclude[numOfTask]] = true;
                  numOfTaskInclude[numOfTask]++;
                  if (numOfTaskInclude[numOfTask]>=maxNumberOfInclude) {
                     cout << "Maximal number of inludes in task '" << taskName[numOfTask].Data() << "' reached : " << maxNumberOfInclude << " !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
               if (type == 15 && !strcmp((const char*)name,"Include")) {
                  break;
               }
            }
         }
         // steering parameters
         else if (!strcmp((const char*)name,"SteeringParameters")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               empty = xml->isEmpty();
               if (type == 1 && !empty) {
                  numOfTaskSteering[numOfTask]++;
                  if (numOfTaskSteering[numOfTask]>=maxNumberOfSteering) {
                     cout << "Maximal number of steering parameter classes in task '" << taskName[numOfTask].Data() << "' reached : " << maxNumberOfSteering << " !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  depth++;
                  index[depth] = numOfTaskSteering[numOfTask];
                  numOfTaskSteerFields[numOfTask][index[depth]] = 0;
                  // steering name
                  taskSteerName[numOfTask][index[depth]] = (char*)name;
                  // parent name
                  taskSteerParent[numOfTask][index[depth]] = taskSteerName[numOfTask][index[depth-1]];
                  // depth
                  taskSteerDepth[numOfTask][index[depth]] = depth;
                  // output
                  if (makeOutput) for (i=0;i<depth+1;i++) cout << "   ";
                  if (makeOutput) taskSteerName[numOfTask][index[depth]].WriteLine();
               }
               if (type == 1 && empty) {
                  // field name
                  taskSteerFieldName[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]] = (char*)name;
                  // field type
                  xml->GetAttribute("Type",taskSteerFieldType[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"");
                  if (taskSteerFieldType[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]]=="") {
                     cout << "Steering Parameter " << taskSteerFieldName[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]].Data() << " has no type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // field initialisation
                  if (taskSteerFieldType[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]]=="TString")
                     xml->GetAttribute("Initialisation",taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"' '");
                  else
                     xml->GetAttribute("Initialisation",taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"0");
                  xml->GetAttribute("Init",taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]]);
                  // field comment
                  xml->GetAttribute("Comment",taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"");
                  if (taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]][0]!='/') {
                     taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]].Insert(0,"// ");
                  }
                  // output
                  if (makeOutput) for (i=0;i<depth+2;i++) cout << "   ";
                  if (makeOutput) taskSteerFieldName[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]].WriteLine();
                  numOfTaskSteerFields[numOfTask][index[depth]]++;
                  if (numOfTaskSteerFields[numOfTask][index[depth]]>=maxNumberOfSteeringField) {
                     cout << "Maximal number of fields in steering parameter class '" << taskSteerName[numOfTask][index[depth]].Data() << "' in task '" << taskName[numOfTask].Data() << "' reached : " << maxNumberOfSteeringField << " !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
               if (type == 15 && strcmp((const char*)name,"SteeringParameters")) {
                  depth--;
                  if (makeOutput) cout << endl;;
               }
               if (type == 15 && !strcmp((const char*)name,"SteeringParameters")) break;
            }
            numOfTaskSteering[numOfTask]++;
         }
         // histos
         else if (!strcmp((const char*)name,"Histos")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  // histo name
                  histoName[numOfTask][numOfHistos[numOfTask]] = (char*)name;
                  // histo type
                  xml->GetAttribute("Type",histoType[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoType[numOfTask][numOfHistos[numOfTask]]=="") {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no type defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo array size
                  xml->GetAttribute("ArraySize",histoArray[numOfTask][numOfHistos[numOfTask]],"1");
                  // histo title
                  xml->GetAttribute("Title",histoTitle[numOfTask][numOfHistos[numOfTask]],"");
                  // histo folder name
                  xml->GetAttribute("FolderName",histoFolderName[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoFolderName[numOfTask][numOfHistos[numOfTask]]=="") {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no Folder Name defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo folder title
                  xml->GetAttribute("FolderTitle",histoFolderTitle[numOfTask][numOfHistos[numOfTask]],"");
                  // histo xbins
                  xml->GetAttribute("NumberOfBinsX",histoXBin[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoXBin[numOfTask][numOfHistos[numOfTask]]=="") {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no number of X bins defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo xmin
                  xml->GetAttribute("XMin",histoXMin[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoXMin[numOfTask][numOfHistos[numOfTask]]=="") {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no X minimum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo xmax
                  xml->GetAttribute("XMax",histoXMax[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoXMax[numOfTask][numOfHistos[numOfTask]]=="") {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no X maximum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo ybins
                  xml->GetAttribute("NumberOfBinsY",histoYBin[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoYBin[numOfTask][numOfHistos[numOfTask]]=="") {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no number of Y bins defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo ymin
                  xml->GetAttribute("YMin",histoYMin[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoYMin[numOfTask][numOfHistos[numOfTask]]=="") {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no Y minimum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo ymax
                  xml->GetAttribute("YMax",histoYMax[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoYMax[numOfTask][numOfHistos[numOfTask]]=="") {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no Y maximum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo zbins
                  xml->GetAttribute("NumberOfBinsZ",histoZBin[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoZBin[numOfTask][numOfHistos[numOfTask]]=="") {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no number of Z bins defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo zmin
                  xml->GetAttribute("ZMin",histoZMin[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoZMin[numOfTask][numOfHistos[numOfTask]]=="") {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no Z minimum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo zmax
                  xml->GetAttribute("ZMax",histoZMax[numOfTask][numOfHistos[numOfTask]],"");
                  if (histoZMax[numOfTask][numOfHistos[numOfTask]]=="") {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' of Task '" << taskName[numOfTask].Data() << "' has no Z maximum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  numOfHistos[numOfTask]++;
                  if (numOfHistos[numOfTask]>=maxNumberOfHistos) {
                     cout << "Maximal number of histos in task '" << taskName[numOfTask].Data() << "' reached : " << maxNumberOfHistos << " !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
               if (type == 15 && !strcmp((const char*)name,"Histos")) {
                  for (i=0;i<numOfHistos[numOfTask];i++) {
                     for (j=i+1;j<numOfHistos[numOfTask];j++) {
                        if (histoName[numOfTask][i]==histoName[numOfTask][j]) {
                           cout << "\nTask '" << taskName[numOfTask].Data() << "' has two histos with the name '" << histoName[numOfTask][i].Data() << "' !" << endl;
                           cout << "Terminating program." << endl;
                           return false;
                        }
                     }
                  }
                  break;
               }
            }
         }
      }
      if (type == 15 && !strcmp((const char*)name,"Task")) {
         for (i=0;i<numOfTask;i++) {
            for (j=i+1;j<numOfTask;j++) {
               if (taskName[i] == taskName[j]) {
                  cout << "\nTask '" << taskName[i].Data() << "' is defined twice !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         break;
      }
   }
   numOfTask++;
   return true;
}
bool ROMEBuilder::WriteTaskSteeringClass(ROMEString &buffer,int numSteer,int numTask) {
   ROMEString format;
   ROMEString sc;
   ROMEString blank;
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i=0;i<numOfTaskSteerFields[numTask][numSteer];i++) {
      if (typeLen<(int)taskSteerFieldType[numTask][numSteer][i].Length()) typeLen = taskSteerFieldType[numTask][numSteer][i].Length();
      if (nameLen<(int)taskSteerFieldName[numTask][numSteer][i].Length()) nameLen = taskSteerFieldName[numTask][numSteer][i].Length();
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (taskSteerParent[numTask][i]==taskSteerName[numTask][numSteer]) {
         if (typeLen<(int)taskSteerName[numTask][i].Length()+1) typeLen = taskSteerName[numTask][i].Length()+1;
         if (nameLen<(int)taskSteerName[numTask][i].Length()) nameLen = taskSteerName[numTask][i].Length();
      }
   }
   sc = "";

   blank = "";
   for (i=0;i<taskSteerDepth[numTask][numSteer]+1;i++) {
      blank.Append("   ");   
   }

   buffer.AppendFormated("\n%sclass %s%s\n",blank.Data(),sc.Data(),taskSteerName[numTask][numSteer].Data());
   buffer.AppendFormated("%s{\n",blank.Data());

   buffer.AppendFormated("%sprivate:\n",blank.Data());

   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (taskSteerParent[numTask][i]==taskSteerName[numTask][numSteer]) {
         WriteTaskSteeringClass(buffer,i,numTask);
      }
   }

   buffer.AppendFormated("%sprotected:\n",blank.Data());

   // Fields
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      format.SetFormated("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-taskSteerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormated((char*)format.Data(),blank.Data(),taskSteerFieldType[numTask][numSteer][j].Data(),taskSteerFieldName[numTask][numSteer][j].Data(),"",taskSteerFieldComment[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (taskSteerParent[numTask][i]==taskSteerName[numTask][numSteer]) {
         format.SetFormated("%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-taskSteerName[numTask][i].Length());
         buffer.AppendFormated((char*)format.Data(),blank.Data(),taskSteerName[numTask][i].Data(),taskSteerName[numTask][i].Data(),"",taskSteerName[numTask][i].Data());
      }
   }
   buffer.AppendFormated("\n%spublic:\n",blank.Data());
   // Constructor
   buffer.AppendFormated("%s   %s%s() { ",blank.Data(),sc.Data(),taskSteerName[numTask][numSteer].Data());
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      buffer.AppendFormated("f%s = %s; ",taskSteerFieldName[numTask][numSteer][j].Data(),taskSteerFieldInit[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (taskSteerParent[numTask][i]==taskSteerName[numTask][numSteer]) {
         buffer.AppendFormated("f%s = new %s(); ",taskSteerName[numTask][i].Data(),taskSteerName[numTask][i].Data());
      }
   }
   buffer.AppendFormated("};\n");
   // Getters
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      format.SetFormated("%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen-taskSteerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormated((char*)format.Data(),blank.Data(),taskSteerFieldType[numTask][numSteer][j].Data(),taskSteerFieldName[numTask][numSteer][j].Data(),"",taskSteerFieldName[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (taskSteerParent[numTask][i]==taskSteerName[numTask][numSteer]) {
         format.SetFormated("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen-taskSteerName[numTask][i].Length());
         buffer.AppendFormated((char*)format.Data(),blank.Data(),taskSteerName[numTask][i].Data(),taskSteerName[numTask][i].Data(),"",taskSteerName[numTask][i].Data());
      }
   }
   // Setters
   buffer.AppendFormated("\n");
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      format.SetFormated("%%s   void Set%%-%ds(%%-%ds %%s)%%%ds { f%%s = %%s; };\n",nameLen,typeLen,nameLen-taskSteerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormated((char*)format.Data(),blank.Data(),taskSteerFieldName[numTask][numSteer][j].Data(),taskSteerFieldType[numTask][numSteer][j].Data(),taskSteerFieldName[numTask][numSteer][j].Data(),"",taskSteerFieldName[numTask][numSteer][j].Data(),taskSteerFieldName[numTask][numSteer][j].Data());
   }

   // Footer
   buffer.AppendFormated("%s};\n\n",blank.Data());

   return true;
}
void ROMEBuilder::WriteTaskSteerConfigWrite(ROMEString& buffer,int numSteer,int numTask) {
   int i,j;
   ROMEString tmp;
   ROMEString getter;
   if (numSteer==0)
      buffer.AppendFormated("   xml->StartElement(\"SteeringParameters\");\n");
   else
      buffer.AppendFormated("   xml->StartElement(\"%s\");\n",taskSteerName[numTask][numSteer].Data());
   for (i=0;i<numOfTaskSteerFields[numTask][numSteer];i++) {
      getter.SetFormated("->Get%s()",taskSteerFieldName[numTask][numSteer][i].Data());
      int ind = numSteer;
      while (taskSteerParent[numTask][ind]!="") {
         for (j=0;j<numOfTaskSteering[numTask];j++) {
            if (taskSteerParent[numTask][ind]==taskSteerName[numTask][j]) {
               getter.InsertFormated(0,"->Get%s()",taskSteerName[numTask][ind].Data());
               ind = j;
               break;
            }
         }
      }
      getter.InsertFormated(0,"((%sT%s*)f%sTask)->GetSP()",shortCut.Data(),taskName[numTask].Data(),taskName[numTask].Data());
      GetFormat(&tmp,(char*)taskSteerFieldType[numTask][numSteer][i].Data());
      buffer.AppendFormated("   value.SetFormated(\"%s\",%s);\n",tmp.Data(),getter.Data());
      buffer.AppendFormated("   xml->WriteElement(\"%s\",value.Data());\n",taskSteerFieldName[numTask][numSteer][i].Data());
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (taskSteerParent[numTask][i]==taskSteerName[numTask][numSteer]) {
         WriteTaskSteerConfigWrite(buffer,i,numTask);
      }
   }
   buffer.AppendFormated("   xml->EndElement();\n");
}
void ROMEBuilder::WriteTaskSteerConfigRead(ROMEString& buffer,int numSteer,int numTask) {
   ROMEString tmp;
   ROMEString value;
   ROMEString path;
   ROMEString setter;
   int i,j;
   ROMEString blank;
   blank = "      ";

   path = "";
   int ind = numSteer;
   while (taskSteerParent[numTask][ind]!="") {
      for (j=0;j<numOfTaskSteering[numTask];j++) {
         if (taskSteerParent[numTask][ind]==taskSteerName[numTask][j]) {
            path.InsertFormated(0,"->Get%s()",taskSteerName[numTask][ind].Data());
            ind = j;
            blank.Append("      ");
            break;
         }
      }
   }
   if (numSteer==0)
      buffer.AppendFormated("%s               if (type == 1 && !strcmp((const char*)name,\"%sParameters\")) {\n",blank.Data(),taskSteerName[numTask][numSteer].Data());
   else
      buffer.AppendFormated("%s               if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank.Data(),taskSteerName[numTask][numSteer].Data());

   buffer.AppendFormated("%s                  while (xml->NextLine()) {\n",blank.Data());
   buffer.AppendFormated("%s                     type = xml->GetType();\n",blank.Data());
   buffer.AppendFormated("%s                     name = xml->GetName();\n",blank.Data());
   if (numSteer==0)
      buffer.AppendFormated("%s                     char *cstop;\n",blank.Data());
   for (i=0;i<numOfTaskSteerFields[numTask][numSteer];i++) {
      setValue(&value,"","value",(char*)taskSteerFieldType[numTask][numSteer][i].Data(),1);
      setter.SetFormated("((%sT%s*)f%sTask)->GetSP()%s->Set%s((%s)%s)",shortCut.Data(),taskName[numTask].Data(),taskName[numTask].Data(),path.Data(),taskSteerFieldName[numTask][numSteer][i].Data(),taskSteerFieldType[numTask][numSteer][i].Data(),value.Data());
      buffer.AppendFormated("%s                     if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank.Data(),taskSteerFieldName[numTask][numSteer][i].Data());
      buffer.AppendFormated("%s                        if (xml->GetValue(value)) \n",blank.Data());
      buffer.AppendFormated("%s                           %s;\n",blank.Data(),setter.Data());
      buffer.AppendFormated("%s                     }\n",blank.Data());
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (taskSteerParent[numTask][i]==taskSteerName[numTask][numSteer]) {
         WriteTaskSteerConfigRead(buffer,i,numTask);
      }
   }
   if (numSteer==0)
      buffer.AppendFormated("%s                     if (type == 15 && !strcmp((const char*)name,\"%sParameters\"))\n",blank.Data(),taskSteerName[numTask][numSteer].Data());
   else
      buffer.AppendFormated("%s                     if (type == 15 && !strcmp((const char*)name,\"%s\"))\n",blank.Data(),taskSteerName[numTask][numSteer].Data());
   buffer.AppendFormated("%s                        break;\n",blank.Data());
   buffer.AppendFormated("%s                  }\n",blank.Data());
   buffer.AppendFormated("%s               }\n",blank.Data());
}
bool ROMEBuilder::WriteTaskCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int lenTot,ll,i;
   char *pBuffer=NULL;
   int bufferLen=0;
   char *pos;
   int fileHandle;
   ROMEString format;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      // File name
      cppFile.SetFormated("%s/src/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      buffer.AppendFormated("//// Author: %s\n",taskAuthor[iTask].Data());
      buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormated("//                                                                            //\n");
      ll = 73-shortCut.Length();
      format.SetFormated("// %%sT%%-%d.%ds //\n",ll,ll);
      buffer.AppendFormated((char*)format.Data(),shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormated("//                                                                            //\n");
      pos = (char*)taskDescription[iTask].Data();
      lenTot = taskDescription[iTask].Length();
      while (pos-taskDescription[iTask].Data() < lenTot) {
         if (lenTot+(taskDescription[iTask].Data()-pos)<74) i=lenTot;
         else for (i=74;pos[i]!=32&&i>0;i--) {}
         if (i<=0) i=75;
         pos[i] = 0;
         buffer.AppendFormated("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// This task contains the following histgrams :\n");
      for (i=0;i<numOfHistos[iTask];i++) {
         buffer.AppendFormated("// %s\n",histoName[iTask][i].Data());
      }
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// The histograms are created and saved automaticaly by the task.\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// The following methods can be used to fill the histogram and to set the\n");
      buffer.AppendFormated("// right name,title and binwidth of the histogram :\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// Fill<Histogram Name>(double value,double weight)\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// For histogram arrays use :\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// Fill<Histogram Name>At(int index,double value,double weight)\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// If more histogram functions are needed use the following function the get\n");
      buffer.AppendFormated("// a handle to the histogram and use the root functions.\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// Get<Histogram Name>Handle()\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// For histogram arrays use :\n");
      buffer.AppendFormated("//\n");
      buffer.AppendFormated("// Get<Histogram Name>HandleAt(int index)\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("/////////////////////////////////////----///////////////////////////////////////");

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
            buffer.AppendFormated("\n\n#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[iTask].Data());
            buffer.AppendFormated("#include <Riostream.h>\n");

            buffer.AppendFormated("\nClassImp(%sT%s)\n\n",shortCut.Data(),taskName[iTask].Data());

            // User Functions
            ROMEString shortcut(shortCut);
            shortcut.ToLower();
            ROMEString taskname(taskName[iTask]);
            taskname.ToLower();

            if (taskFortran[iTask]) {
               buffer.AppendFormated("extern \"C\" void %st%sinit_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("void %sT%s::Init()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("   %sT%s_Init_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("}\n\n");
               buffer.AppendFormated("extern \"C\" void %st%sbeginofrun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("void %sT%s::BeginOfRun()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("   %sT%s_BeginOfRun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("}\n\n");
               buffer.AppendFormated("extern \"C\" void %st%sevent_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("void %sT%s::Event()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("   %sT%s_Event_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("}\n\n");
               buffer.AppendFormated("extern \"C\" void %st%sendofrun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("void %sT%s::EndOfRun()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("   %sT%s_EndOfRun_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("}\n\n");
               buffer.AppendFormated("extern \"C\" void %st%sterminate_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("void %sT%s::Terminate()\n{\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("   %sT%s_Terminate_();\n",shortcut.Data(),taskname.Data());
               buffer.AppendFormated("}\n\n");
            }
            else {
               buffer.AppendFormated("void %sT%s::Init()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormated("void %sT%s::BeginOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormated("void %sT%s::Event()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormated("void %sT%s::EndOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
               buffer.AppendFormated("void %sT%s::Terminate()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
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
         fFile.SetFormated("%s/src/tasks/%sT%s.f",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

         ROMEString shortcut(shortCut);
         shortcut.ToLower();
         ROMEString taskname(taskName[iTask]);
         taskname.ToLower();

         if( !stat( fFile.Data(), &buf )) {
            fileHandle = open(fFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
            if (makeOutput) cout << "      " << fFile.Data() << endl;

            // Methods
            buffer.Resize(0);
            buffer.AppendFormated("      subroutine %st%sinit()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormated("      return\n");
            buffer.AppendFormated("      end\n\n");
            buffer.AppendFormated("      subroutine %st%sbeginofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormated("      return\n");
            buffer.AppendFormated("      end\n\n");
            buffer.AppendFormated("      subroutine %st%sevent()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormated("      return\n");
            buffer.AppendFormated("      end\n\n");
            buffer.AppendFormated("      subroutine %st%sendofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormated("      return\n");
            buffer.AppendFormated("      end\n\n");
            buffer.AppendFormated("      subroutine %st%sterminate()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormated("      return\n");
            buffer.AppendFormated("      end\n\n");

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

   int i;
   int fileHandle;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      // File name
      hFile.SetFormated("%s/include/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormated("// If you intend to change this file please contact:                          //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("// Manual changes to this file will always be overwritten by the builder.     //\n");
      buffer.AppendFormated("//                                                                            //\n");
      buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n\n");

      // Header
      buffer.AppendFormated("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormated("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());

      buffer.AppendFormated("#include<TH1.h>\n");
      buffer.AppendFormated("#include<TH2.h>\n");
      buffer.AppendFormated("#include<TH3.h>\n");
      buffer.AppendFormated("#include<ROMETask.h>\n");
         
      for (i=0;i<numOfTaskInclude[iTask];i++) {
         if (taskLocalFlag[iTask][i]) {
            buffer.AppendFormated("#include\"%s\"\n",taskInclude[iTask][i].Data());
         }
         else {
            buffer.AppendFormated("#include<%s>\n",taskInclude[iTask][i].Data());
         }
      }

      buffer.AppendFormated("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());

      // Class
      buffer.AppendFormated("\nclass %sT%s : public ROMETask\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormated("{\n");

      // Fields
      if (numOfTaskSteering[iTask]>0) {
         buffer.AppendFormated("private:\n");
         WriteTaskSteeringClass(buffer,0,iTask);
         buffer.AppendFormated("\n");
      }

      buffer.AppendFormated("protected:\n");

      buffer.AppendFormated("   %sAnalyzer* fAnalyzer; // Handle to %sAnalyzer class\n\n",shortCut.Data(),shortCut.Data());
      if (numOfTaskSteering[iTask]>0) {
         buffer.AppendFormated("   Steering* fSteering; // Handle to Steering class\n\n");
      }

      int nameLen = -1;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (nameLen<(int)histoName[iTask][i].Length()) nameLen = histoName[iTask][i].Length();
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]=="1") {
            format.SetFormated("   %%s*      f%%s;%%%ds // %%s\n",nameLen-histoName[iTask][i].Length()+1);
            buffer.AppendFormated((char*)format.Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),"",histoTitle[iTask][i].Data());
         }
         else {
            format.SetFormated("   TObjArray* f%%ss;%%%ds // %%s\n",nameLen-histoName[iTask][i].Length());
            buffer.AppendFormated((char*)format.Data(),histoName[iTask][i].Data(),"",histoTitle[iTask][i].Data());
         }
         format.SetFormated("   bool       f%%sAccumulation;%%%ds // Accumulation Flag for the %%s\n",nameLen-histoName[iTask][i].Length());
         buffer.AppendFormated((char*)format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
      }


      // Methods
      buffer.AppendFormated("public:\n");
      // Constructor and Event Methods
      buffer.AppendFormated("   // Constructor\n");
      buffer.AppendFormated("   %sT%s(const char *name,const char *title,%sAnalyzer *analyzer):ROMETask(name,title,analyzer)\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data());
      buffer.AppendFormated("   { fAnalyzer = analyzer; fEventID = \"%s\"; fVersion = %s;",taskEventID[iTask].Data(),taskVersion[iTask].Data());
      if (numOfHistos[iTask]>0) 
         buffer.AppendFormated(" fHasHistograms = true;");
      else
         buffer.AppendFormated(" fHasHistograms = false;");
      for (i=0;i<numOfHistos[iTask];i++) {
         buffer.AppendFormated(" f%sAccumulation = true;",histoName[iTask][i].Data());
      }
      if (numOfTaskSteering[iTask]>0) {
         buffer.AppendFormated(" fSteering = new Steering();");
      }
      buffer.AppendFormated(" };\n");
      buffer.AppendFormated("   // Event Methods\n");
      buffer.AppendFormated("   virtual void Init();\n");
      buffer.AppendFormated("   virtual void BeginOfRun();\n");
      buffer.AppendFormated("   virtual void Event();\n");
      buffer.AppendFormated("   virtual void EndOfRun();\n");
      buffer.AppendFormated("   virtual void Terminate();\n\n");
      // Histo Methods
      buffer.AppendFormated("   // Histo Methods\n");
      buffer.AppendFormated("   virtual void BookHisto();\n");
      buffer.AppendFormated("   virtual void ResetHisto();\n\n");
      // User Methods
      buffer.AppendFormated("   // User Methods\n");
      if (numOfTaskSteering[iTask]>0) {
         buffer.AppendFormated("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormated("   Steering* GetSP() { return fSteering; };\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]=="1") {
            if (histoType[iTask][i][2]==49) {
					buffer.AppendFormated("   void Fill%s(double x,double weight=1) { f%s->Fill(x,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormated("   void Fill%s(double x,double y,double weight=1) { f%s->Fill(x,y,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormated("   void Fill%s(double x,double y,double z,double weight=1) { f%s->Fill(x,y,z,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
				buffer.AppendFormated("   void Draw%s() { f%s->Draw(); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormated("   %s* Get%sHandle() { return f%s; };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormated("   void Fill%sAt(int index,double x,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormated("   void Fill%sAt(int index,double x,double y,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormated("   void Fill%sAt(int index,double x,double y,double z,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,z,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
				buffer.AppendFormated("   void Draw%sAt(int index) { ((%s*)f%ss->At(index))->Draw(); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormated("   %s* Get%sHandleAt(int index) { return (%s*)f%ss->At(index); };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
         }
         buffer.AppendFormated("   Bool_t is%sAccumulation() { return f%sAccumulation; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         buffer.AppendFormated("   void Set%sAccumulation(Bool_t flag) { f%sAccumulation = flag; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
      }


      // Footer
      buffer.AppendFormated("\n   ClassDef(%sT%s,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
      buffer.AppendFormated("};\n\n");

      // Histo Inline Methods
      buffer.AppendFormated("inline void %sT%s::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      bool array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]!="1") array = true;
      }
      if (array) {
         buffer.AppendFormated("   int j;\n");
         buffer.AppendFormated("   ROMEString name;\n");
         buffer.AppendFormated("   ROMEString title;\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]=="1") {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormated("   f%s = new %s(\"%s\",\"%s\",%s,%s,%s);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoTitle[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormated("   f%s = new %s(\"%s\",\"%s\",%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoTitle[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormated("   f%s = new %s(\"%s\",\"%s\",%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoTitle[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data(),histoZBin[iTask][i].Data(),histoZMin[iTask][i].Data(),histoZMax[iTask][i].Data());
            }
            buffer.AppendFormated("   GetHistoFolder()->Add(f%s);\n",histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormated("   %s *hist%d;\n",histoType[iTask][i].Data(),i);
            buffer.AppendFormated("   f%ss = new TObjArray(%s);\n",histoName[iTask][i].Data(),histoArray[iTask][i].Data());
            buffer.AppendFormated("   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",\"%s\");\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data(),histoFolderTitle[iTask][i].Data());
            buffer.AppendFormated("   for (j=0;j<%s;j++) {\n",histoArray[iTask][i].Data());
            buffer.AppendFormated("      name.SetFormated(\"%%0*d\",3,j);\n");
            buffer.AppendFormated("      name.Insert(0,\"%s_\");\n",histoName[iTask][i].Data());
            buffer.AppendFormated("      title.SetFormated(\"%%0*d\",3,j);\n");
            buffer.AppendFormated("      title.Insert(0,\"%s \");\n",histoTitle[iTask][i].Data());
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormated("      hist%d = new %s(name.Data(),title.Data(),%s,%s,%s);\n",i,histoType[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormated("      hist%d = new %s(name,title,%s,%s,%s,%s,%s,%s);\n",i,histoType[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormated("      hist%d = new %s(name,title,%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",i,histoType[iTask][i].Data(),histoXBin[iTask][i].Data(),histoXMin[iTask][i].Data(),histoXMax[iTask][i].Data(),histoYBin[iTask][i].Data(),histoYMin[iTask][i].Data(),histoYMax[iTask][i].Data(),histoZBin[iTask][i].Data(),histoZMin[iTask][i].Data(),histoZMax[iTask][i].Data());
            }
            buffer.AppendFormated("      f%ss->Add(hist%d);\n",histoName[iTask][i].Data(),i);
            buffer.AppendFormated("      %sFolder->Add(f%ss->At(j));\n   }\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
         }
      }
      buffer.AppendFormated("}\n\n");

      buffer.AppendFormated("inline void %sT%s::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]!="1") 
            array = true;
      }
      if (array) {
         buffer.AppendFormated("   int j;\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArray[iTask][i]=="1") {
            buffer.AppendFormated("   if (!is%sAccumulation()) f%s->Reset();\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormated("   if (!is%sAccumulation()) {\n",histoName[iTask][i].Data());
            buffer.AppendFormated("       for (j=0;j<%s;j++) ((%s*)f%ss->At(j))->Reset();\n",histoArray[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormated("   }\n");
         }
      }
      buffer.AppendFormated("}\n\n");

      buffer.AppendFormated("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

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

   if (makeOutput) cout << "\n\nTrees:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"Tree")) {
         numOfTree++;
         if (numOfTree>=maxNumberOfTrees) {
            cout << "Maximal number of trees reached : " << maxNumberOfTrees << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         numOfBranch[numOfTree] = 0;
         // tree name
         xml->GetAttribute("Name",treeName[numOfTree],"");
         if (treeName[numOfTree]=="") {
            cout << "Tree without a name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // tree title
         xml->GetAttribute("Title",treeTitle[numOfTree],"");
         // output
         if (makeOutput) cout << "   " << treeName[numOfTree].Data() << endl;

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp((const char*)name,"Branch")) {
               // branch name
               xml->GetAttribute("Name",branchName[numOfTree][numOfBranch[numOfTree]],"");
               if (branchName[numOfTree][numOfBranch[numOfTree]]=="") {
                  cout << "Branch without a name in Tree '" << treeName[numOfTree].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               // branch folder
               xml->GetAttribute("Folder",branchFolder[numOfTree][numOfBranch[numOfTree]],"");
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
               numOfBranch[numOfTree]++;
               if (numOfBranch[numOfTree]>=maxNumberOfBranches) {
                  cout << "Maximal number of branches in tree '" << treeName[numOfTree].Data() << "' reached : " << maxNumberOfBranches << " !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
            if (type == 15 && !strcmp((const char*)name,"Tree")) {
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

   if (makeOutput) cout << "\n\nBanks:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp("EventHeader",(const char*)name)) {
         bankHasHeader = true;
         // folder
         xml->GetAttribute("Folder",bankHeaderFolder,"");
         if (bankHeaderFolder=="") {
            cout << "Midas event header has no folder !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // EventID
         xml->GetAttribute("EventID",bankHeaderEventID,"");
         // TriggerMask
         xml->GetAttribute("TriggerMask",bankHeaderTriggerMask,"");
         // SerialNumber
         xml->GetAttribute("SerialNumber",bankHeaderSerialNumber,"");
         // TimeStamp
         xml->GetAttribute("TimeStamp",bankHeaderTimeStamp,"");
         // Tests
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
            cout << "Midas event header : event id field '" << bankHeaderEventID.Data() << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundMask) {
            cout << "Midas event header : trigger mask field '" << bankHeaderTriggerMask.Data() << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundNum) {
            cout << "Midas event header : serial number field '" << bankHeaderSerialNumber.Data() << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundTime) {
            cout << "Midas event header : time stamp field '" << bankHeaderTimeStamp.Data() << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
      }
      else if (type == 1) {
         numOfBank++;
         if (numOfBank>=maxNumberOfBanks) {
            cout << "Maximal number of banks reached : " << maxNumberOfBanks << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         numOfStructFields[numOfBank] = 0;
         // bank name
         bankName[numOfBank] = (char*)name;
         // bank type
         xml->GetAttribute("Type",bankType[numOfBank],"");
         if (bankType[numOfBank]=="") {
            cout << "Bank '" << bankName[numOfBank].Data() << "' has no type !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // bank structure name
         bankStructName[numOfBank].SetFormated("%sStruct",bankName[numOfBank].Data());
         xml->GetAttribute("StructName",bankStructName[numOfBank],bankStructName[numOfBank]);
         // output
         if (makeOutput) cout << "   " << bankName[numOfBank].Data() << endl;

         if (bankType[numOfBank]=="structure"||bankType[numOfBank]=="struct") {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  // field name
                  structFieldName[numOfBank][numOfStructFields[numOfBank]] = (char*)name;
                  // field type
                  xml->GetAttribute("Type",structFieldType[numOfBank][numOfStructFields[numOfBank]],"");
                  if (structFieldType[numOfBank][numOfStructFields[numOfBank]]=="") {
                     cout << "Structure field '" << structFieldName[numOfBank][numOfStructFields[numOfBank]].Data() << "' of Bank '" << bankName[numOfBank].Data() << "' has no type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // field size
                  xml->GetAttribute("PackedSize",structFieldSize[numOfBank][numOfStructFields[numOfBank]],"");
                  // output
                  if (makeOutput) cout << "      " << structFieldName[numOfBank][numOfStructFields[numOfBank]].Data() << endl;
                  numOfStructFields[numOfBank]++;
                  if (numOfStructFields[numOfBank]>=maxNumberOfStructFields) {
                     cout << "Maximal number of fields in bank '" << bankName[numOfBank].Data() << "' reached : " << maxNumberOfStructFields << " !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
               }
               if (type == 15 && !strcmp((const char*)name,bankName[numOfBank])) {
                  for (i=0;i<numOfStructFields[numOfBank];i++) {
                     for (j=i+1;j<numOfStructFields[numOfBank];j++) {
                        if (structFieldName[numOfBank][i]==structFieldName[numOfBank][j]) {
                           cout << "\nStructure of bank '" << bankName[numOfBank].Data() << "' has two fields with the name '" << structFieldName[numOfBank][i].Data() << "' !" << endl;
                           cout << "Terminating program." << endl;
                           return false;
                        }
                     }
                  }
                  break;
               }
            }
         }
      }
      if (type == 15 && !strcmp((const char*)name,"MidasBanks")) {
         for (i=0;i<numOfBank;i++) {
            for (j=i+1;j<numOfTree;j++) {
               if (bankName[i]==bankName[j]) {
                  cout << "\nMidas bank '" << bankName[i].Data() << "' is defined twice !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         break;
      }
   }
   numOfBank++;
   return true;
}

bool ROMEBuilder::ReadXMLSteering() {
   char *name;
   int i,type,empty,depth=0,index[20];
   index[0] = 0;
   numOfSteerFields[0] = 0;
   steerName[0] = "GeneralSteering";
   steerParent[0] = "";
   steerDepth[0] = 0;

   if (makeOutput) cout << "\n\nSteering:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      empty = xml->isEmpty();
      if (type == 1 && !empty) {
         numOfSteering++;
         if (numOfSteering>=maxNumberOfSteering) {
            cout << "Maximal number of steering parameter classes reached : " << maxNumberOfSteering << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         depth++;
         index[depth] = numOfSteering;
         numOfSteerFields[index[depth]] = 0;
         // bank name
         steerName[index[depth]] = (char*)name;
         // parent name
         steerParent[index[depth]] = steerName[index[depth-1]];
         // depth
         steerDepth[index[depth]] = depth;
         // output
         if (makeOutput) for (i=0;i<depth;i++) cout << "   ";
         if (makeOutput) steerName[index[depth]].WriteLine();
      }
      if (type == 1 && empty) {
         // field name
         steerFieldName[index[depth]][numOfSteerFields[index[depth]]] = (char*)name;
         // field type
         xml->GetAttribute("Type",steerFieldType[index[depth]][numOfSteerFields[index[depth]]],"");
         if (steerFieldType[index[depth]][numOfSteerFields[index[depth]]]=="") {
            cout << "Steering Parameter " << steerFieldName[index[depth]][numOfSteerFields[index[depth]]].Data() << " has no type !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // field initialisation
         if (steerFieldType[index[depth]][numOfSteerFields[index[depth]]]=="TString")
            xml->GetAttribute("Initialisation",steerFieldInit[index[depth]][numOfSteerFields[index[depth]]],"' '");
         else
            xml->GetAttribute("Initialisation",steerFieldInit[index[depth]][numOfSteerFields[index[depth]]],"0");
         xml->GetAttribute("Init",steerFieldInit[index[depth]][numOfSteerFields[index[depth]]],steerFieldInit[index[depth]][numOfSteerFields[index[depth]]]);
         // field comment
         xml->GetAttribute("Comment",steerFieldComment[index[depth]][numOfSteerFields[index[depth]]],"");
         if (steerFieldComment[index[depth]][numOfSteerFields[index[depth]]][0]!='/') {
            steerFieldComment[index[depth]][numOfSteerFields[index[depth]]].Insert(0,"// ");
         }
         // output
         if (makeOutput) for (i=0;i<depth+1;i++) cout << "   ";
         if (makeOutput) steerFieldName[index[depth]][numOfSteerFields[index[depth]]].WriteLine();
         numOfSteerFields[index[depth]]++;
         if (numOfSteerFields[index[depth]]>=maxNumberOfSteeringField) {
            cout << "Maximal number of fields in steering parameter class '" << steerName[index[depth]].Data() << "' reached : " << maxNumberOfSteeringField << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
      }
      if (type == 15 && strcmp((const char*)name,"GeneralSteeringParameters")) {
         depth--;
         if (makeOutput) cout << endl;;
      }
      if (type == 15 && !strcmp((const char*)name,"GeneralSteeringParameters")) {
         break;
      }
   }
   numOfSteering++;
   return true;
}

bool ROMEBuilder::WriteSteering() {
   int i;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   hFile.SetFormated("%s/include/framework/%sGeneralSteering.h",outDir.Data(),shortCut.Data());

   if (numOfSteering==0) {
      remove(hFile.Data());
      return true;
   }

   if (makeOutput) cout << "\n   Output Files:" << endl;

   // Description
   buffer.Resize(0);
   buffer.AppendFormated("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// Contains the general Steering Parameters                                   //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormated("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormated("#ifndef %sGeneralSteering_H\n",shortCut.Data());
   buffer.AppendFormated("#define %sGeneralSteering_H\n\n",shortCut.Data());

   WriteSteeringClass(buffer,0);

   buffer.AppendFormated("#endif   // %sGeneralSteering_H\n",shortCut.Data());

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
bool ROMEBuilder::WriteSteeringClass(ROMEString& buffer,int numOfSteer) {
   ROMEString format;
   ROMEString sc;
   ROMEString blank;
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i=0;i<numOfSteerFields[numOfSteer];i++) {
      if (typeLen<(int)steerFieldType[numOfSteer][i].Length()) typeLen = steerFieldType[numOfSteer][i].Length();
      if (nameLen<(int)steerFieldName[numOfSteer][i].Length()) nameLen = steerFieldName[numOfSteer][i].Length();
   }
   for (i=0;i<numOfSteering;i++) {
      if (steerParent[i]==steerName[numOfSteer]) {
         if (typeLen<(int)steerName[i].Length()+1) typeLen = steerName[i].Length()+1;
         if (nameLen<(int)steerName[i].Length()) nameLen = steerName[i].Length();
      }
   }
   if (numOfSteer==0)
      sc = shortCut;
   else
      sc = "";

   blank = "";   
   for (i=0;i<steerDepth[numOfSteer];i++) {
      blank.Append("   ");   
   }

   buffer.AppendFormated("\n%sclass %s%s\n",blank.Data(),sc.Data(),steerName[numOfSteer].Data());
   buffer.AppendFormated("%s{\n",blank.Data());

   buffer.AppendFormated("%sprivate:\n",blank.Data());

   for (i=0;i<numOfSteering;i++) {
      if (steerParent[i]==steerName[numOfSteer]) {
         WriteSteeringClass(buffer,i);
      }
   }

   buffer.AppendFormated("%sprotected:\n",blank.Data());

   // Fields
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      format.SetFormated("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-steerFieldName[numOfSteer][j].Length());
      buffer.AppendFormated((char*)format.Data(),blank.Data(),steerFieldType[numOfSteer][j].Data(),steerFieldName[numOfSteer][j].Data(),"",steerFieldComment[numOfSteer][j].Data());
   }
   for (i=0;i<numOfSteering;i++) {
      if (steerParent[i]==steerName[numOfSteer]) {
         format.SetFormated("%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-steerName[i].Length());
         buffer.AppendFormated((char*)format.Data(),blank.Data(),steerName[i].Data(),steerName[i].Data(),"",steerName[i].Data());
      }
   }
   buffer.AppendFormated("\n%spublic:\n",blank.Data());
   // Constructor
   buffer.AppendFormated("%s   %s%s() { ",blank.Data(),sc.Data(),steerName[numOfSteer].Data());
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      buffer.AppendFormated("f%s = %s; ",steerFieldName[numOfSteer][j].Data(),steerFieldInit[numOfSteer][j].Data());
   }
   for (i=0;i<numOfSteering;i++) {
      if (steerParent[i]==steerName[numOfSteer]) {
         buffer.AppendFormated("f%s = new %s(); ",steerName[i].Data(),steerName[i].Data());
      }
   }
   buffer.AppendFormated("};\n");
   // Getters
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      format.SetFormated("%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen-steerFieldName[numOfSteer][j].Length());
      buffer.AppendFormated((char*)format.Data(),blank.Data(),steerFieldType[numOfSteer][j].Data(),steerFieldName[numOfSteer][j].Data(),"",steerFieldName[numOfSteer][j].Data());
   }
   for (i=0;i<numOfSteering;i++) {
      if (steerParent[i]==steerName[numOfSteer]) {
         format.SetFormated("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen-steerName[i].Length());
         buffer.AppendFormated((char*)format.Data(),blank.Data(),steerName[i].Data(),steerName[i].Data(),"",steerName[i].Data());
      }
   }
   // Setters
   buffer.AppendFormated("\n");
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      format.SetFormated("%%s   void Set%%-%ds(%%-%ds %%s)%%%ds { f%%s = %%s; };\n",nameLen,typeLen,nameLen-steerFieldName[numOfSteer][j].Length());
      buffer.AppendFormated((char*)format.Data(),blank.Data(),steerFieldName[numOfSteer][j].Data(),steerFieldType[numOfSteer][j].Data(),steerFieldName[numOfSteer][j].Data(),"",steerFieldName[numOfSteer][j].Data(),steerFieldName[numOfSteer][j].Data());
   }

   // Footer
   buffer.AppendFormated("%s};\n\n",blank.Data());

   return true;
}
void ROMEBuilder::WriteSteerConfigWrite(ROMEString& buffer,int numOfSteer) {
   int i,j;
   ROMEString tmp;
   ROMEString getter;
   if (numOfSteer==0)
      buffer.AppendFormated("   xml->StartElement(\"GeneralSteeringParameters\");\n");
   else
      buffer.AppendFormated("   xml->StartElement(\"%s\");\n",steerName[numOfSteer].Data());
   for (i=0;i<numOfSteerFields[numOfSteer];i++) {
      getter.SetFormated("->Get%s()",steerFieldName[numOfSteer][i].Data());
      int ind = numOfSteer;
      while (steerParent[ind]!="") {
         for (j=0;j<numOfSteering;j++) {
            if (steerParent[ind]==steerName[j]) {
               getter.InsertFormated(0,"->Get%s()",steerName[ind].Data());
               ind = j;
               break;
            }
         }
      }
      getter.Insert(0,"this->GetGSP()");
      GetFormat(&tmp,(char*)steerFieldType[numOfSteer][i].Data());
      buffer.AppendFormated("   value.SetFormated(\"%s\",%s);\n",tmp.Data(),getter.Data());
      buffer.AppendFormated("   xml->WriteElement(\"%s\", value.Data());\n",steerFieldName[numOfSteer][i].Data(),tmp.Data(),getter.Data());
   }
   for (i=0;i<numOfSteering;i++) {
      if (steerParent[i]==steerName[numOfSteer]) {
         WriteSteerConfigWrite(buffer,i);
      }
   }
   buffer.AppendFormated("   xml->EndElement();\n");
}

void ROMEBuilder::WriteSteerConfigRead(ROMEString& buffer,int numSteer) {
   ROMEString tmp;
   ROMEString value;
   ROMEString path;
   ROMEString setter;
   int i,j;
   ROMEString blank;
   blank = "";

   path = "";
   int ind = numSteer;
   while (steerParent[ind]!="") {
      for (j=0;j<numOfSteering;j++) {
         if (steerParent[ind]==steerName[j]) {
            path.InsertFormated(0,"->Get%s()",steerName[ind].Data());
            ind = j;
            blank.Append("      ");
            break;
         }
      }
   }
   if (numSteer==0)
      buffer.AppendFormated("%s      if (type == 1 && !strcmp((const char*)name,\"%sParameters\")) {\n",blank.Data(),steerName[numSteer].Data());
   else
      buffer.AppendFormated("%s      if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank.Data(),steerName[numSteer].Data());

   buffer.AppendFormated("%s         while (xml->NextLine()) {\n",blank.Data());
   buffer.AppendFormated("%s            type = xml->GetType();\n",blank.Data());
   buffer.AppendFormated("%s            name = xml->GetName();\n",blank.Data());
   if (numSteer==0)
      buffer.AppendFormated("%s                     char *cstop;\n",blank.Data());
   for (i=0;i<numOfSteerFields[numSteer];i++) {
      setValue(&value,"","value",(char*)steerFieldType[numSteer][i].Data(),1);
      setter.SetFormated("this->GetGSP()%s->Set%s((%s)%s)",path.Data(),steerFieldName[numSteer][i].Data(),steerFieldType[numSteer][i].Data(),value.Data());
      buffer.AppendFormated("%s            if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank.Data(),steerFieldName[numSteer][i].Data());
      buffer.AppendFormated("%s               if (xml->GetValue(value)) \n",blank.Data());
      buffer.AppendFormated("%s                  %s;\n",blank.Data(),setter.Data());
      buffer.AppendFormated("%s            }\n",blank.Data());
   }
   for (i=0;i<numOfSteering;i++) {
      if (steerParent[i]==steerName[numSteer]) {
         WriteSteerConfigRead(buffer,i);
      }
   }
   if (numSteer==0)
      buffer.AppendFormated("%s            if (type == 15 && !strcmp((const char*)name,\"%sParameters\"))\n",blank.Data(),steerName[numSteer].Data());
   else
      buffer.AppendFormated("%s            if (type == 15 && !strcmp((const char*)name,\"%s\"))\n",blank.Data(),steerName[numSteer].Data());
   buffer.AppendFormated("%s               break;\n",blank.Data());
   buffer.AppendFormated("%s         }\n",blank.Data());
   buffer.AppendFormated("%s      }\n",blank.Data());
}
bool ROMEBuilder::WriteAnalyzerCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   ROMEString parentt;

   int nb,lenTot,j,ll,k,iFold=0;
   char *pos;
   int fileHandle;

   ROMEString classDescription;
   classDescription.SetFormated("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut.Data(),mainProgName.Data());

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
   cppFile.SetFormated("%s/src/framework/%sAnalyzer.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormated("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormated("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormated("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormated((char*)format.Data(),shortCut.Data(),"Analyzer");
   buffer.AppendFormated("//                                                                            //\n");
   pos = (char*)classDescription.Data();
   lenTot = classDescription.Length();
   while (pos-classDescription.Data() < lenTot) {
      if (lenTot+(classDescription.Data()-pos)<74) i=75;
      else for (i=74;pos[i]!=32&&i>0;i--) {}
      if (i<=0) i=75;
      pos[i] = 0;
      buffer.AppendFormated("// %-74.74s   \n",pos);
      pos = pos+i+1;
   }
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormated("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormated("#if defined( _MSC_VER )\n");
   buffer.AppendFormated("#include <direct.h>\n");
   buffer.AppendFormated("#endif\n");

   buffer.AppendFormated("#include <sys/stat.h>\n");
   buffer.AppendFormated("#include <TH1.h>\n");
   buffer.AppendFormated("#include <TROOT.h>\n");
   buffer.AppendFormated("#include <TObjArray.h>\n");
   buffer.AppendFormated("#include <TBranchElement.h>\n");
   buffer.AppendFormated("#include <TTask.h>\n");
   buffer.AppendFormated("#include <ROMEStatic.h>\n");
   buffer.AppendFormated("#if defined HAVE_SQL\n");
   buffer.AppendFormated("#include <ROMESQL.h>\n");
   buffer.AppendFormated("#endif\n");
   buffer.AppendFormated("#include <ROMEXML.h>\n");
   buffer.AppendFormated("#include <ROMEEventLoop.h>\n");
   buffer.AppendFormated("#include <ROMETree.h>\n");
   buffer.AppendFormated("#include <ROMETreeInfo.h>\n");
   buffer.AppendFormated("#include <ROME.h>\n");
   buffer.AppendFormated("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormated("#include <Riostream.h>\n");

   buffer.AppendFormated("\n");
   buffer.AppendFormated("#if defined( _MSC_VER )\n");
   buffer.AppendFormated("#include <windows.h>\n");
   buffer.AppendFormated("void CreateSplash(unsigned long time,char*,char*,ROMEString*,int);\n");
   buffer.AppendFormated("DWORD WINAPI SplashThread ( LPVOID lpvoid) {\n");
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
   buffer.AppendFormated("   ROMEString *authors = new ROMEString[%d];\n",numAuthors);
   for (i=0;i<numAuthors;i++) {
      buffer.AppendFormated("   authors[%d] = \"%s\";\n",i,authors[i].Data());
   }
   buffer.AppendFormated("   CreateSplash(3,\"%s%s\",\"%s\",authors,%d);\n",shortCut.Data(),mainProgName.Data(),romeVersion.Data(),numAuthors);
   buffer.AppendFormated("   return 0;\n");
   buffer.AppendFormated("}\n");
   buffer.AppendFormated("#endif\n");
   buffer.AppendFormated("\n");


   // Constructor
   buffer.AppendFormated("%sAnalyzer::%sAnalyzer() {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("// Folder, Task, Tree and Data Base initialisation\n");
   buffer.AppendFormated("\n");
   // Steering 
   if (numOfSteering!=0) {
      buffer.AppendFormated("   fGeneralSteeringParameters = new %sGeneralSteering();\n",shortCut.Data());
   }
   // Folder 
   buffer.AppendFormated("   // Folder initialisation\n");
   buffer.AppendFormated("   fMainFolder = gROOT->GetRootFolder()->AddFolder(\"%s\",\"Root Folder of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("   gROOT->GetListOfBrowsables()->Add(fMainFolder,\"%s\");\n\n",shortCut.Data());

   for (i=0;i<numOfFolder;i++) {
      if (folderParentName[i]=="GetMainFolder()") parentt = folderParentName[i];
      else parentt.SetFormated("%sFolder",folderParentName[i].Data());
      format.SetFormated("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-folderName[i].Length());
      buffer.AppendFormated((char*)format.Data(),folderName[i].Data(),"",parentt.Data(),folderName[i].Data(),folderTitle[i].Data());
   }
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            buffer.AppendFormated("\n   f%sObject = new %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormated("   %sFolder->Add(f%sObject);\n",folderName[i].Data(),folderName[i].Data());
         }
         else {
            buffer.AppendFormated("\n   f%sObjects = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormated("   %sFolder->Add(f%sObjects);\n",folderName[i].Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormated("\n");

   // Task
   buffer.AppendFormated("   // Task initialisation\n");
   buffer.AppendFormated("   fMainTask = new ROMEEventLoop(\"mainTask\",\"Main Task of %s%s\",this);\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("   fMainFolder->Add(fMainTask);\n");
   buffer.AppendFormated("   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   int taskLen=0;
   for (i=0;i<numOfTask;i++) {
      if (taskLen<(int)taskName[i].Length()) taskLen = taskName[i].Length();
   }
   for (i=0;i<numOfTask;i++) {
      format.SetFormated("   f%%sTask%%%ds = new %%sT%%s(\"%%s\",\"%%s\",this);\n",taskLen-taskName[i].Length());
      buffer.AppendFormated((char*)format.Data(),taskName[i].Data(),"",shortCut.Data(),taskName[i].Data(),taskName[i].Data(),"");
      buffer.AppendFormated("   ((%sT%s*)f%sTask)->SetActive(false);\n",shortCut.Data(),taskName[i].Data(),taskName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      if (taskParentName[i]=="GetMainTask()") parentt = taskParentName[i];
      else parentt.SetFormated("f%sTask",taskParentName[i].Data());
      buffer.AppendFormated("   %s->Add(f%sTask);\n",parentt.Data(),taskName[i].Data());
   }
   buffer.AppendFormated("\n");

   // Tree
   buffer.AppendFormated("   TTree *tree;\n\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormated("   tree = new TTree(\"%s\",\"%s\");\n",treeName[i].Data(),treeTitle[i].Data());
      buffer.AppendFormated("   tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000,99);\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k]) 
               iFold = k;
         }
         if (folderArray[iFold]=="1") {
            buffer.AppendFormated("   tree->Branch(\"%s\",\"%s%s\",&f%sObject,32000,99);\n",branchName[i][j].Data(),shortCut.Data(),folderName[iFold].Data(),branchFolder[i][j].Data());
         }
         else {
            buffer.AppendFormated("   tree->Branch(\"%s\",\"TClonesArray\",&f%sObjects,32000,99);\n",branchName[i][j].Data(),branchFolder[i][j].Data());
         }
      }
      buffer.AppendFormated("   this->AddTree(tree);\n\n");
   }

   buffer.AppendFormated("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   fTaskSwitchesString =  \"");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("%s = BOOL : 0\\n",taskName[i].Data());
   }
   buffer.AppendFormated("\";\n");

   buffer.AppendFormated("}\n\n");

   // Task switches call back
   buffer.AppendFormated("// Task switches call back\n");
   buffer.AppendFormated("bool ROMEAnalyzer::fTaskSwitchesChanged = false;\n");
   buffer.AppendFormated("void TaskSwitchesChanged(int hDB,int hKey,void *info) {\n");
   buffer.AppendFormated("   ROMEAnalyzer::fTaskSwitchesChanged = true;\n");
   buffer.AppendFormated("}\n\n");

   // Initialize ODB
   buffer.AppendFormated("// InitODB\n");
   buffer.AppendFormated("bool %sAnalyzer::InitODB() {\n",shortCut.Data());
   buffer.AppendFormated("   HNDLE hKey;\n");
   buffer.AppendFormated("   ROMEString str;\n");
   buffer.AppendFormated("   str = \"/%s%s/Task switches\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("   db_check_record(fMidasOnlineDataBase, 0, (char*)str.Data(), (char*)fTaskSwitchesString.Data(), TRUE);\n");
   buffer.AppendFormated("   db_find_key(fMidasOnlineDataBase, 0, (char*)str.Data(), &hKey);\n");
   buffer.AppendFormated("   if (db_set_record(fMidasOnlineDataBase,hKey,&fTaskSwitches,sizeof(TaskSwitches),0) != DB_SUCCESS) {\n");
   buffer.AppendFormated("      cout << \"Cannot write to task switches record.\" << endl;\n");
   buffer.AppendFormated("      return false;\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("   if (db_open_record(fMidasOnlineDataBase, hKey, &fTaskSwitches, sizeof(TaskSwitches), MODE_READ, TaskSwitchesChanged, NULL) != DB_SUCCESS) {\n");
   buffer.AppendFormated("      cout << \"Cannot open task switches record, probably other analyzer is using it\" << endl;\n");
   buffer.AppendFormated("      return false;\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("   return true;\n");
   buffer.AppendFormated("}\n\n");

   // clear folders
   buffer.AppendFormated("// Clear Folders\n");
   buffer.AppendFormated("void %sAnalyzer::ClearFolders() {\n",shortCut.Data());
   buffer.AppendFormated("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormated("   f%sObject->Reset();\n",folderName[i].Data());
         }
         else {
            buffer.AppendFormated("   for (i=0;i<f%sObjects->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormated("      ((%s%s*)f%sObjects->At(i))->Reset();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormated("   }\n");
         }
      }
      if (bankHasHeader) {
         if (folderName[i]==bankHeaderFolder) {
            if (bankHeaderEventID!="")
               buffer.AppendFormated("   f%sObject->Set%s(this->GetEventHeader()->event_id);\n",folderName[i].Data(),bankHeaderEventID.Data());
            if (bankHeaderTriggerMask!="")
               buffer.AppendFormated("   f%sObject->Set%s(this->GetEventHeader()->trigger_mask);\n",folderName[i].Data(),bankHeaderTriggerMask.Data());
            if (bankHeaderSerialNumber!="")
               buffer.AppendFormated("   f%sObject->Set%s(this->GetEventHeader()->serial_number);\n",folderName[i].Data(),bankHeaderSerialNumber.Data());
            if (bankHeaderTimeStamp!="")
               buffer.AppendFormated("   f%sObject->Set%s(this->GetEventHeader()->time_stamp);\n",folderName[i].Data(),bankHeaderTimeStamp.Data());
         }
      }
   }
   buffer.AppendFormated("}\n\n");

   // fill trees
   buffer.AppendFormated("// Tree Filling\n");
   buffer.AppendFormated("void %sAnalyzer::FillTrees() {\n",shortCut.Data());
   buffer.AppendFormated("   ROMETree *romeTree;\n");
   buffer.AppendFormated("   int i;\n");
   buffer.AppendFormated("   // Fill Trees;\n");
   buffer.AppendFormated("   bool write = false;\n");
   buffer.AppendFormated("   bool written = false;\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormated("   write = false;\n");
      buffer.AppendFormated("   romeTree = (ROMETree*)fTreeObjects->At(%d);\n",i);
      buffer.AppendFormated("   if (romeTree->isWrite()) {\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (folderName[k]==branchFolder[i][j]) {
               iFold = k;
               break;
            }
         }
         if (folderArray[iFold]=="1") {
            buffer.AppendFormated("      if (!write && f%sObject->isModified()) {\n",branchFolder[i][j].Data());
            buffer.AppendFormated("         write = true;\n");
            buffer.AppendFormated("      }\n");
         }
         else {
            buffer.AppendFormated("      for (i=0;i<f%sObjects->GetEntries()&&!write;i++) {\n",branchFolder[i][j].Data());
            buffer.AppendFormated("         if (((%s%s*)f%sObjects->At(i))->isModified()) {\n",shortCut.Data(),branchFolder[i][j].Data(),branchFolder[i][j].Data());
            buffer.AppendFormated("            write = true;\n");
            buffer.AppendFormated("            break;\n");
            buffer.AppendFormated("         }\n");
            buffer.AppendFormated("      }\n");
         }
      }
      buffer.AppendFormated("      if (write) {\n");
      buffer.AppendFormated("         written = true;\n");
      buffer.AppendFormated("         fTreeInfo->SetSequentialNumber(fSequentialNumber);\n");
      buffer.AppendFormated("         romeTree->GetTree()->Fill();\n");
      buffer.AppendFormated("      }\n");
      buffer.AppendFormated("   }\n");
   }
   buffer.AppendFormated("   if (written) {\n");
   buffer.AppendFormated("      fSequentialNumber++;\n");
   buffer.AppendFormated("      fTriggerStatistics.writtenEvents++;\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("}\n");
   buffer.AppendFormated("\n");


   // Midas Bank Initialisation
   buffer.AppendFormated("// Midas Bank Initialisation\n");
   buffer.AppendFormated("void %sAnalyzer::InitMidasBanks() {\n",shortCut.Data());
   for (i=0;i<numOfBank;i++) {
      buffer.AppendFormated("   this->Init%sBank();\n",bankName[i].Data());
   }
   buffer.AppendFormated("}\n");
   buffer.AppendFormated("\n");

   // Midas Bank Methodes
   buffer.AppendFormated("\n// Midas Bank Getters\n");
   for (i=0;i<numOfBank;i++) {
      if (bankType[i]=="structure"||bankType[i]=="struct") {
         buffer.AppendFormated("%s* %sAnalyzer::Get%sBankAt(int index) {\n",bankStructName[i].Data(),shortCut.Data(),bankName[i].Data());
         buffer.AppendFormated("   if (this->f%sBankExists)\n",bankName[i].Data());
         buffer.AppendFormated("      return f%sBankPointer+index;\n",bankName[i].Data());
         buffer.AppendFormated("   return NULL;\n");
         buffer.AppendFormated("}\n");
      }
      else {
         buffer.AppendFormated("%s %sAnalyzer::Get%sBankAt(int index) {\n",bankType[i].Data(),shortCut.Data(),bankName[i].Data());
         buffer.AppendFormated("   if (this->f%sBankExists)\n",bankName[i].Data());
         buffer.AppendFormated("      return *(f%sBankPointer+index);\n",bankName[i].Data());
         buffer.AppendFormated("   return (%s)exp(999);\n",bankType[i].Data());
         buffer.AppendFormated("}\n");
      }
      buffer.AppendFormated("void %sAnalyzer::Init%sBank() {\n",shortCut.Data(),bankName[i].Data());
      buffer.AppendFormated("   unsigned long bktype;\n");
      buffer.AppendFormated("   EVENT_HEADER *pevent = this->GetEventHeader();\n");
      buffer.AppendFormated("   pevent++;\n");
      buffer.AppendFormated("   if (ROMEStatic::bk_find(pevent, \"%s\", (unsigned long*)&f%sBankLength, &bktype, &f%sBankPointer)) {\n",bankName[i].Data(),bankName[i].Data(),bankName[i].Data());
      buffer.AppendFormated("      f%sBankExists = true;\n",bankName[i].Data());
      buffer.AppendFormated("      return;\n");
      buffer.AppendFormated("   }\n");
      buffer.AppendFormated("   f%sBankExists = false;\n",bankName[i].Data());
      buffer.AppendFormated("   f%sBankPointer = NULL;\n",bankName[i].Data());
      buffer.AppendFormated("   f%sBankLength = 0;\n",bankName[i].Data());
      buffer.AppendFormated("   return;\n");
      buffer.AppendFormated("}\n");

      buffer.AppendFormated("int %sAnalyzer::Get%sBankEntries() {\n",shortCut.Data(),bankName[i].Data());
      buffer.AppendFormated("   return f%sBankLength;\n",bankName[i].Data());
      buffer.AppendFormated("}\n\n");
   }


   // Connect Trees
   buffer.AppendFormated("// Connect Trees\n");
   buffer.AppendFormated("void %sAnalyzer::ConnectTrees()\n{\n",shortCut.Data());
   buffer.AppendFormated("   TBranchElement *bb;\n");
   for (i=0;i<numOfTree;i++) {
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k]) 
               iFold = k;
         }
         buffer.AppendFormated("   bb = (TBranchElement*)this->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"%s\");\n",i,branchName[i][j].Data());
         if (folderArray[iFold]=="1") {
            buffer.AppendFormated("   bb->SetAddress(&this->f%sObject);\n",folderName[iFold].Data());
         }
         else {
            buffer.AppendFormated("   bb->SetAddress(&this->f%sObjects);\n",folderName[iFold].Data());
         }
         buffer.AppendFormated("   bb = (TBranchElement*)this->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"Info\");\n",i);
         buffer.AppendFormated("   bb->SetAddress(&this->fTreeInfo);\n");
      }
   }
   buffer.AppendFormated("}\n\n");

   int ndb = 0;
   for (i=0;i<numOfFolder;i++) if (folderDataBase[i]) ndb++;
   // SQL Init
   buffer.AppendFormated("bool %sAnalyzer::InitSQLDataBase()\n",shortCut.Data());
   buffer.AppendFormated("{\n");
   buffer.AppendFormated("#ifdef HAVE_SQL\n");
   if (ndb>0) {
      buffer.AppendFormated("   fSQL = new ROMESQL();\n");
      buffer.AppendFormated("   return fSQL->Connect(\"pc4466.psi.ch\",\"rome\",\"rome\",\"%sDataBase\");\n",shortCut.Data());
   }
   else {
      buffer.AppendFormated("   return true;\n");
   }
   buffer.AppendFormated("#else\n");
   buffer.AppendFormated("   cout << \"No sql database access -> regenerate program !\" << endl;\n");
   buffer.AppendFormated("   return false;\n");
   buffer.AppendFormated("#endif\n");
   buffer.AppendFormated("}\n\n");

   // SQL Read
   ROMEString buf;
   buffer.AppendFormated("bool %sAnalyzer::ReadSQLDataBase()\n",shortCut.Data());
   buffer.AppendFormated("{\n");
   buffer.AppendFormated("#ifdef HAVE_SQL\n");
   if (ndb>0) {
      buffer.AppendFormated("   char *cstop,*res;\n");
      buffer.AppendFormated("   int i;\n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            if (folderArray[i]=="1") {
               for (j=0;j<numOfValue[i];j++) {
                  buffer.AppendFormated("   fSQL->ReadPathFields(\"%s\",this->GetCurrentRunNumber(),\"id\");\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormated("   fSQL->NextRow();\n");
                  buffer.AppendFormated("   res = fSQL->GetField(0);\n");
                  setValue(&buf,(char*)valueName[i][j].Data(),"res",(char*)valueType[i][j].Data(),1);
                  buffer.AppendFormated("   ((%s%s*)f%sObjects->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
               }
            }
            else {
               for (j=0;j<numOfValue[i];j++) {
                  buffer.AppendFormated("   fSQL->ReadPathFields(\"%s\",this->GetCurrentRunNumber(),\"id\");\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormated("   for(i=0;i<fSQL->GetNumberOfRows();i++){\n");
                  buffer.AppendFormated("      fSQL->NextRow();\n");
                  buffer.AppendFormated("      res = fSQL->GetField(0);\n");
                  setValue(&buf,(char*)valueName[i][j].Data(),"res",(char*)valueType[i][j].Data(),1);
                  buffer.AppendFormated("      ((%s%s*)f%sObjects->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormated("   }\n");
               }
            }
         }
      }
   }
   buffer.AppendFormated("   return true;\n");
   buffer.AppendFormated("#else\n");
   buffer.AppendFormated("   cout << \"No sql database access -> regenerate program !\" << endl;\n");
   buffer.AppendFormated("   return false;\n");
   buffer.AppendFormated("#endif\n");
   buffer.AppendFormated("}\n\n");

   // ReadXMLDataBase
   buffer.AppendFormated("bool %sAnalyzer::ReadXMLDataBase() {\n",shortCut.Data());
   if (ndb>0) {
      buffer.AppendFormated("   char *cstop;\n");
      buffer.AppendFormated("   char *name;\n");
      buffer.AppendFormated("   int type;\n");
      buffer.AppendFormated("   ROMEString dbFile;\n");
      buffer.AppendFormated("   ROMEString filename;\n");
      buffer.AppendFormated("   ROMEString path;\n");
      buffer.AppendFormated("   ROMEString value;\n");
      buffer.AppendFormated("   ROMEString runNumberString;\n");
      buffer.AppendFormated("   this->GetCurrentRunNumberString(runNumberString);\n");
      buffer.AppendFormated("   filename.SetFormated(\"%%s/RunTable.xml\",this->GetInputDir());\n");
      buffer.AppendFormated("   path.SetFormated(\"//RunTable/Run_%%s\",runNumberString.Data());\n");
      buffer.AppendFormated("\n");
      buffer.AppendFormated("   ROMEXML *xml = new ROMEXML();\n");
      buffer.AppendFormated("   if (!xml->OpenFileForPath(filename.Data())) { \n");
      buffer.AppendFormated("      cout << \"\\nFailed to load xml database : '\" << filename.Data() << \"'\" << endl;\n");
      buffer.AppendFormated("      cout << \"Do you like the framework to generate a new xml database ([y]/n) ? \";\n");
      buffer.AppendFormated("      char answer[10];\n");
      buffer.AppendFormated("      cin >> answer;\n");
      buffer.AppendFormated("      if (strstr(answer,\"n\")==NULL) {\n");
      buffer.AppendFormated("         ROMEXML *xmlNew = new ROMEXML();\n");
      buffer.AppendFormated("         if (!xmlNew->OpenFileForWrite(filename.Data()))\n");
      buffer.AppendFormated("            return false;\n");
      buffer.AppendFormated("         xmlNew->StartElement(\"RunTable\");\n");
      buffer.AppendFormated("         xmlNew->EndDocument();\n");
      buffer.AppendFormated("         delete xmlNew;\n");
      buffer.AppendFormated("         if (!xml->OpenFileForPath(filename.Data()))\n");
      buffer.AppendFormated("            return false;\n");
      buffer.AppendFormated("         cout << \"\\nThe framework generated a new xml database.\" << endl;\n");
      buffer.AppendFormated("      }\n");
      buffer.AppendFormated("      else\n");
      buffer.AppendFormated("         return false; \n");
      buffer.AppendFormated("   }; \n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            buffer.AppendFormated("   if (xml->GetPathAttribute(path,\"%sFile\",dbFile)) {;\n",folderName[i].Data());
            buffer.AppendFormated("      filename.SetFormated(\"%%s%%s\",this->GetDataBaseDir(),dbFile.Data());\n");
            buffer.AppendFormated("      if (!xml->OpenFileForRead(filename.Data())) { \n");
            buffer.AppendFormated("         cout << \"Failed to load database : '\" << filename.Data() << \"'\" << endl;\n");
            buffer.AppendFormated("         return false; \n");
            buffer.AppendFormated("      };\n");
            buffer.AppendFormated("      while (xml->NextLine()) {\n");
            buffer.AppendFormated("         type = xml->GetType();\n");
            buffer.AppendFormated("         name = xml->GetName();\n");
            buffer.AppendFormated("         if (type == 1 && !strcmp(name,\"%s\")) {\n",folderName[i].Data());
            buffer.AppendFormated("            xml->GetAttribute(\"Number\",value,\"0\");\n");
            buffer.AppendFormated("            int num = strtol(value.Data(),&cstop,10);\n");
            buffer.AppendFormated("            while (xml->NextLine()) {\n");
            buffer.AppendFormated("               type = xml->GetType();\n");
            buffer.AppendFormated("               name = xml->GetName();\n");
            if (folderArray[i]=="1") {
               for (j=0;j<numOfValue[i];j++) {
                  buffer.AppendFormated("               if (type == 1 && !strcmp(name,\"%s\")) {\n",valueName[i][j].Data());
                  buffer.AppendFormated("                  if (xml->GetValue(value)) {\n");
                  setValue(&buf,(char*)valueName[i][j].Data(),"value.Data()",(char*)valueType[i][j].Data(),1);
                  buffer.AppendFormated("                     f%sObject->Set%s((%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormated("                  };\n");
                  buffer.AppendFormated("               };\n");
               }
            }
            else {
               for (j=0;j<numOfValue[i];j++) {
                  buffer.AppendFormated("               if (type == 1 && !strcmp(name,\"%s\")) {\n",valueName[i][j].Data());
                  buffer.AppendFormated("                  if (xml->GetValue(value)) {\n");
                  setValue(&buf,(char*)valueName[i][j].Data(),"value.Data()",(char*)valueType[i][j].Data(),1);
                  buffer.AppendFormated("                     ((%s%s*)f%sObjects->At(num))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                  buffer.AppendFormated("                  };\n");
                  buffer.AppendFormated("               };\n");
               }
            }
            buffer.AppendFormated("               if (type == 15 && !strcmp(name,\"%s\"))\n",folderName[i].Data());
            buffer.AppendFormated("                  break;\n");
            buffer.AppendFormated("            }\n");
            buffer.AppendFormated("         }\n");
            buffer.AppendFormated("         if (type == 15 && !strcmp(name,\"%ss\"))\n",folderName[i].Data());
            buffer.AppendFormated("            break;\n");
            buffer.AppendFormated("      }\n");
            buffer.AppendFormated("   }\n");
         }
      }
      buffer.AppendFormated("   \n");
      buffer.AppendFormated("   delete xml;\n");
      buffer.AppendFormated("   return true;\n");
   } 
   buffer.AppendFormated("   return true;\n");
   buffer.AppendFormated("}\n");

   // Write Data Base
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i]) {
         buffer.AppendFormated("void %sAnalyzer::Write%sDataBase() {\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormated("   // SQL\n");
         buffer.AppendFormated("   if (this->isSQLDataBase()) {\n");
         buffer.AppendFormated("   }\n\n");
         buffer.AppendFormated("   // XML\n");
         buffer.AppendFormated("   else if (this->isXMLDataBase()) {\n");
         buffer.AppendFormated("      ROMEString name;\n");
         buffer.AppendFormated("      ROMEString value;\n");
         buffer.AppendFormated("      ROMEString dbFile;\n");
         buffer.AppendFormated("      ROMEString filename;\n");
         buffer.AppendFormated("      ROMEString path;\n");
         buffer.AppendFormated("      ROMEString runNumberString;\n");
         buffer.AppendFormated("      int n=0,i;\n");
         buffer.AppendFormated("      ROMEXML *xml = new ROMEXML();\n");
         buffer.AppendFormated("      this->GetCurrentRunNumberString(runNumberString);\n");
         buffer.AppendFormated("      filename.SetFormated(\"%%s/RunTable.xml\",this->GetInputDir());\n");
         buffer.AppendFormated("      if (!xml->OpenFileForPath(filename.Data()))\n");
         buffer.AppendFormated("         return;\n");
         buffer.AppendFormated("      path.SetFormated(\"//RunTable/Run_%%s\",runNumberString.Data());\n");
         buffer.AppendFormated("      if (!xml->ExistPath(path.Data())) {\n");
         buffer.AppendFormated("         name.SetFormated(\"Run_%%s\",runNumberString.Data());\n");
         buffer.AppendFormated("         if (!xml->HasPathChildren(\"//RunTable\")) {\n");
         buffer.AppendFormated("            xml->NewPathChildElement(\"//RunTable\",name.Data(),NULL);\n");
         buffer.AppendFormated("         }\n");
         buffer.AppendFormated("         else {\n");
         buffer.AppendFormated("            bool exist = true;\n");
         buffer.AppendFormated("            n = this->GetCurrentRunNumber();\n");
         buffer.AppendFormated("            while (!xml->ExistPath(path.Data())) {\n");
         buffer.AppendFormated("               if (n==0) {\n");
         buffer.AppendFormated("                  exist = false;\n");
         buffer.AppendFormated("                  break;\n");
         buffer.AppendFormated("               }\n");
         buffer.AppendFormated("               path.SetFormated(\"//RunTable/Run_%%05d\",n--);\n");
         buffer.AppendFormated("            }\n");
         buffer.AppendFormated("            if (exist) {\n");
         buffer.AppendFormated("               xml->NewPathNextElement(path.Data(),name.Data(),NULL);\n");
         buffer.AppendFormated("            }\n");
         buffer.AppendFormated("            else {\n");
         buffer.AppendFormated("               xml->NewPathPrevElement(path.Data(),name.Data(),NULL);\n");
         buffer.AppendFormated("            }\n");
         buffer.AppendFormated("         }\n");
         buffer.AppendFormated("         path.SetFormated(\"//RunTable/Run_%%s\",runNumberString.Data());\n");
         buffer.AppendFormated("         dbFile = \"db%s\";\n",folderName[i].Data());
         buffer.AppendFormated("         dbFile.Append(runNumberString.Data());\n");
         buffer.AppendFormated("         dbFile.Append(\"_0.xml\");\n");
         buffer.AppendFormated("         xml->NewPathAttribute(path.Data(),\"%sFile\",dbFile.Data());\n",folderName[i].Data());
         buffer.AppendFormated("      }\n");
         buffer.AppendFormated("      else {\n");
         buffer.AppendFormated("         if (xml->GetPathAttribute(path,\"%sFile\",dbFile)) {\n",folderName[i].Data());
         buffer.AppendFormated("            NextFile(dbFile,dbFile);\n");
         buffer.AppendFormated("            xml->ReplacePathAttributeValue(path.Data(),\"%sFile\",dbFile.Data());\n",folderName[i].Data());
         buffer.AppendFormated("         }\n");
         buffer.AppendFormated("         else {\n");
         buffer.AppendFormated("            dbFile = \"db%s\";\n",folderName[i].Data());
         buffer.AppendFormated("            dbFile.Append(runNumberString.Data());\n");
         buffer.AppendFormated("            dbFile.Append(\"_0.xml\");\n");
         buffer.AppendFormated("            xml->NewPathAttribute(path.Data(),\"%sFile\",dbFile.Data());\n",folderName[i].Data());
         buffer.AppendFormated("         }\n");
         buffer.AppendFormated("      }\n");
         buffer.AppendFormated("      xml->WritePathFile(filename.Data());\n");
         buffer.AppendFormated("      filename.SetFormated(\"%%s/%%s\",this->GetDataBaseDir(),dbFile.Data());\n");
         buffer.AppendFormated("      if (!xml->OpenFileForWrite(filename.Data()))\n");
         buffer.AppendFormated("         return;\n");
         buffer.AppendFormated("      xml->StartElement(\"%ss\");\n",folderName[i].Data());
         if (folderArray[i]=="1") {
            buffer.AppendFormated("      xml->StartElement(\"%s\");\n",folderName[i].Data());
            buffer.AppendFormated("      xml->WriteAttribute(\"Number\",\"0\");\n");
            for (j=0;j<numOfValue[i];j++) {
               GetFormat(&format,(char*)valueType[i][j].Data());
               if (valueType[i][j]=="TString") {
                  buffer.AppendFormated("      value.SetFormated(\"%s\",f%sObject->Get%s().Data());\n",format.Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               else {
                  buffer.AppendFormated("      value.SetFormated(\"%s\",f%sObject->Get%s());\n",format.Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               buffer.AppendFormated("      xml->WriteElement(\"%s\",value.Data());\n",valueName[i][j].Data());
            }
            buffer.AppendFormated("      xml->EndElement();\n");
         }
         else {
            buffer.AppendFormated("      for (i=0;i<f%sObjects->GetEntries();i++) {\n",folderName[i].Data());
            buffer.AppendFormated("         xml->StartElement(\"%s\");\n",folderName[i].Data());
            buffer.AppendFormated("         value.SetFormated(\"%%d\",i);\n");
            buffer.AppendFormated("         xml->WriteAttribute(\"Number\",value.Data());\n");
            for (j=0;j<numOfValue[i];j++) {
               GetFormat(&format,(char*)valueType[i][j].Data());
               if (valueType[i][j]=="TString") {
                  buffer.AppendFormated("         value.SetFormated(\"%s\",((%s%s*)f%sObjects->At(i))->Get%s().Data());\n",format.Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               else {
                  buffer.AppendFormated("         value.SetFormated(\"%s\",((%s%s*)f%sObjects->At(i))->Get%s());\n",format.Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               buffer.AppendFormated("         xml->WriteElement(\"%s\",value.Data());\n",valueName[i][j].Data());
            }
            buffer.AppendFormated("         xml->EndElement();\n");
            buffer.AppendFormated("      }\n");
         }
         buffer.AppendFormated("      xml->EndDocument();\n");
         buffer.AppendFormated("      delete xml;\n");
         buffer.AppendFormated("   }\n");
         buffer.AppendFormated("}\n\n");
      }
   }

   buffer.AppendFormated("void %sAnalyzer::NextFile(ROMEString& nextFile,ROMEString& file) {\n",shortCut.Data());
   buffer.AppendFormated("   struct stat buf;\n");
   buffer.AppendFormated("   ROMEString body;\n");
   buffer.AppendFormated("   char* res;\n");
   buffer.AppendFormated("   int n=0,number=0;\n");
   buffer.AppendFormated("   if ((res=strstr(file.Data(),\"_\"))) {\n");
   buffer.AppendFormated("      n = res-file.Data();\n");
   buffer.AppendFormated("      body = file(0,n);\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("   else {\n");
   buffer.AppendFormated("      if ((res=strstr(file.Data(),\".\"))) {\n");
   buffer.AppendFormated("         n = res-file.Data();\n");
   buffer.AppendFormated("         body = file(0,n);\n");
   buffer.AppendFormated("      }\n");
   buffer.AppendFormated("      else {\n");
   buffer.AppendFormated("         body = file;\n");
   buffer.AppendFormated("      }\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("   nextFile.SetFormated(\"%%s/%%s_%%d.xml\",this->GetDataBaseDir(),body.Data(),number);\n");
   buffer.AppendFormated("   while (!stat(nextFile.Data(),&buf)) {\n");
   buffer.AppendFormated("      number++;\n");
   buffer.AppendFormated("      nextFile.SetFormated(\"%%s/%%s_%%d.xml\",this->GetDataBaseDir(),body.Data(),number);\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("   nextFile.SetFormated(\"%%s_%%d.xml\",body.Data(),number);\n");
   buffer.AppendFormated("}\n\n");



   // Initialize Folders
   buffer.AppendFormated("void %sAnalyzer::InitFolders() {\n",shortCut.Data());
   buffer.AppendFormated("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            buffer.AppendFormated("   new(f%sObject) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="0")
                  buffer.AppendFormated("%s,",valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length()-1);
            buffer.AppendFormated(" );\n");
         }
         else {
            tmp = folderArray[i];
            if (folderArray[i].Contains("fAnalyzer"))
               tmp.Replace(0,9,"this",4);
            buffer.AppendFormated("   for (i=0;i<%s;i++) {\n",tmp.Data());
            buffer.AppendFormated("     new((*f%sObjects)[i]) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="0")
                  buffer.AppendFormated("%s,",valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length()-1);
            buffer.AppendFormated(" );\n");
            buffer.AppendFormated("   }\n");
         }
      }
   }
   buffer.AppendFormated("};\n\n");

   // Initialize Task Switches
   buffer.AppendFormated("void %sAnalyzer::InitTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("   GetTaskSwitches()->%s = f%sTask->IsActive();\n",taskName[i].Data(),taskName[i].Data());
   }
   buffer.AppendFormated("};\n\n");

   // Update Task Switches
   buffer.AppendFormated("void %sAnalyzer::UpdateTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("   if (GetTaskSwitches()->%s)\n",taskName[i].Data());
      buffer.AppendFormated("      f%sTask->SetActive(true);\n",taskName[i].Data());
      buffer.AppendFormated("   else\n");
      buffer.AppendFormated("      f%sTask->SetActive(false);\n",taskName[i].Data());
   }
   buffer.AppendFormated("};\n\n");

   // Configuration File
   buffer.AppendFormated("\n// Configuration File\n");
   buffer.AppendFormated("//--------------------\n");
   buffer.AppendFormated("bool %sAnalyzer::ReadROMEConfigXML(char *configFile) {\n",shortCut.Data());
   buffer.AppendFormated("   char *name;\n");
   buffer.AppendFormated("   ROMEString value;\n");
   buffer.AppendFormated("   int type;\n");
   buffer.AppendFormated("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormated("   if (!xml->OpenFileForRead(configFile)) {\n");
   buffer.AppendFormated("      fprintf(stderr, \"Unable to open %%s\\n\", configFile);\n");
   buffer.AppendFormated("      return false;\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("   while (xml->NextLine()) {\n");
   buffer.AppendFormated("      type = xml->GetType();\n");
   buffer.AppendFormated("      name = xml->GetName();\n");
   // Modes
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"Modes\")) {\n");
   buffer.AppendFormated("         xml->GetAttribute(\"AnalyzingMode\",value,\"\");\n");
   buffer.AppendFormated("         if (value==\"online\")\n");
   buffer.AppendFormated("            this->SetOnline();\n");
   buffer.AppendFormated("         else\n");
   buffer.AppendFormated("            this->SetOffline();\n");
   buffer.AppendFormated("         xml->GetAttribute(\"InputDataFormat\",value,\"\");\n");
   buffer.AppendFormated("         if (value==\"root\")\n");
   buffer.AppendFormated("            this->SetRoot();\n");
   buffer.AppendFormated("         else\n");
   buffer.AppendFormated("            this->SetMidas();\n");
   buffer.AppendFormated("         xml->GetAttribute(\"BatchMode\",value,\"\");\n");
   buffer.AppendFormated("         if (value==\"yes\")\n");
   buffer.AppendFormated("            fBatchMode = true;\n");
   buffer.AppendFormated("         else\n");
   buffer.AppendFormated("            fBatchMode = false;\n");
   buffer.AppendFormated("         xml->GetAttribute(\"ShowSplashScreen\",value,\"\");\n");
   buffer.AppendFormated("         if (value==\"no\")\n");
   buffer.AppendFormated("            fSplashScreen = false;\n");
   buffer.AppendFormated("         else\n");
   buffer.AppendFormated("            fSplashScreen = true;\n");
   buffer.AppendFormated("         xml->GetAttribute(\"DataBaseMode\",value,\"\");\n");
   buffer.AppendFormated("         if (value==\"sql\"||value==\"SQL\")\n");
   buffer.AppendFormated("            this->SetSQLDataBase();\n");
   buffer.AppendFormated("         else if (value==\"xml\"||value==\"XML\") {\n");
   buffer.AppendFormated("            this->SetXMLDataBase();\n");
   buffer.AppendFormated("         }\n");
   buffer.AppendFormated("      }\n");
   // Run Numbers
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"RunNumbers\")) {\n");
   buffer.AppendFormated("         if (xml->GetAttribute(\"Numbers\",value,\"\"))\n");
   buffer.AppendFormated("            this->SetRunNumbers(value);\n");
   buffer.AppendFormated("      }\n");
   // Event Numbers
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"EventNumbers\")) {\n");
   buffer.AppendFormated("         if (xml->GetAttribute(\"Numbers\",value,\"\"))\n");
   buffer.AppendFormated("            this->SetEventNumbers(value);\n");
   buffer.AppendFormated("      }\n");
   // Paths
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"InputFilePath\")) {\n");
   buffer.AppendFormated("         if (xml->GetValue(value)) {\n");
   buffer.AppendFormated("            if (value[value.Length()-1]!='/' && value[value.Length()-1]!='\\\\')\n");
   buffer.AppendFormated("               value.Append(\"/\");\n");
   buffer.AppendFormated("            this->SetInputDir(value);\n");
   buffer.AppendFormated("         }\n");
   buffer.AppendFormated("      }\n");
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"OutputFilePath\")) {\n");
   buffer.AppendFormated("         if (xml->GetValue(value)) {\n");
   buffer.AppendFormated("            if (value[value.Length()-1]!='/' && value[value.Length()-1]!='\\\\')\n");
   buffer.AppendFormated("               value.Append(\"/\");\n");
   buffer.AppendFormated("            this->SetOutputDir(value);\n");
   buffer.AppendFormated("         }\n");
   buffer.AppendFormated("      }\n");
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"DataBaseFilePath\")) {\n");
   buffer.AppendFormated("         if (xml->GetValue(value)) {\n");
   buffer.AppendFormated("            if (value[value.Length()-1]!='/' && value[value.Length()-1]!='\\\\')\n");
   buffer.AppendFormated("               value.Append(\"/\");\n");
   buffer.AppendFormated("            this->SetDataBaseDir(value);\n");
   buffer.AppendFormated("         }\n");
   buffer.AppendFormated("      }\n");
   // Tasks
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"Tasks\")) {\n");
   buffer.AppendFormated("         while (xml->NextLine()) {\n");
   buffer.AppendFormated("            type = xml->GetType();\n");
   buffer.AppendFormated("            name = xml->GetName();\n");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("            if (type == 1 && !strcmp(name,\"%s\")) {\n",taskName[i].Data());
      buffer.AppendFormated("               int empty = xml->isEmpty();\n");
      buffer.AppendFormated("               xml->GetAttribute(\"Active\",value,\"\");\n");
      buffer.AppendFormated("               if (value==\"yes\") {\n");
      buffer.AppendFormated("                  f%sTask->SetActive();\n",taskName[i].Data());
      buffer.AppendFormated("               }\n");
      buffer.AppendFormated("               if (!empty) {\n");
      buffer.AppendFormated("                  while (xml->NextLine()) {\n");
      buffer.AppendFormated("                     type = xml->GetType();\n");
      buffer.AppendFormated("                     name = xml->GetName();\n");
      for (j=0;j<numOfHistos[i];j++) {
         buffer.AppendFormated("                     if (type == 1 && !strcmp(name,\"%s\")) {\n",histoName[i][j].Data());
         buffer.AppendFormated("                        xml->GetAttribute(\"Accumulate\",value,\"\");\n");
         buffer.AppendFormated("                        if (value==\"no\")\n");
         buffer.AppendFormated("                           ((%sT%s*)f%sTask)->Set%sAccumulation(false);\n",shortCut.Data(),taskName[i].Data(),taskName[i].Data(),histoName[i][j].Data());
         buffer.AppendFormated("                     }\n");
      }
      WriteTaskSteerConfigRead(buffer,0,i);
      buffer.AppendFormated("                     if (type == 15 && !strcmp(name,\"%s\"))\n",taskName[i].Data());
      buffer.AppendFormated("                        break;\n");
      buffer.AppendFormated("                  }\n");
      buffer.AppendFormated("               }\n");
      buffer.AppendFormated("            }\n");
   }
   buffer.AppendFormated("            if (type == 15 && !strcmp(name,\"Tasks\"))\n");
   buffer.AppendFormated("               break;\n");
   buffer.AppendFormated("         }\n");
   buffer.AppendFormated("      }\n");
   // Trees
   buffer.AppendFormated("      if (type == 1 && !strcmp(name,\"Trees\")) {\n");
   buffer.AppendFormated("         xml->GetAttribute(\"Accumulation\",value,\"\");\n");
   buffer.AppendFormated("         if (value==\"yes\")\n");
   buffer.AppendFormated("            this->SetTreeAccumulation();\n");
   buffer.AppendFormated("         while (xml->NextLine()) {\n");
   buffer.AppendFormated("            type = xml->GetType();\n");
   buffer.AppendFormated("            name = xml->GetName();\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormated("            if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",treeName[i].Data());
      buffer.AppendFormated("               xml->GetAttribute(\"Read\",value,\"\");\n");
      buffer.AppendFormated("               if (value==\"yes\")\n");
      buffer.AppendFormated("                  this->GetTreeObjectAt(%d)->SetRead(true);\n",i);
      buffer.AppendFormated("               xml->GetAttribute(\"Write\",value,\"\");\n");
      buffer.AppendFormated("               if (value==\"yes\")\n");
      buffer.AppendFormated("                  this->GetTreeObjectAt(%d)->SetWrite(true);\n",i);
      buffer.AppendFormated("            }\n");
   }
   buffer.AppendFormated("            if (type == 15 && !strcmp((const char*)name,\"Trees\"))\n");
   buffer.AppendFormated("               break;\n");
   buffer.AppendFormated("         }\n");
   buffer.AppendFormated("      }\n");
   // Steering
   if (numOfSteering>0)
      WriteSteerConfigRead(buffer,0);

   buffer.AppendFormated("      if (type == 15 && !strcmp((const char*)name,\"Configuration\"))\n");
   buffer.AppendFormated("         break;\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("   delete xml;\n");
   buffer.AppendFormated("   return true;\n");
   buffer.AppendFormated("}\n");

   // WriteROMEConfigXML
   buffer.AppendFormated("bool %sAnalyzer::WriteROMEConfigXML(char *configFile) {\n",shortCut.Data());
   buffer.AppendFormated("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormated("   if (!xml->OpenFileForWrite(configFile))\n");
   buffer.AppendFormated("      return false;\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   xml->StartElement(\"Configuration\");\n");
   //modes
   buffer.AppendFormated("   xml->StartElement(\"Modes\");\n");
   buffer.AppendFormated("   if (this->isOnline())\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"AnalyzingMode\",\"online\");\n");
   buffer.AppendFormated("   else\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"AnalyzingMode\",\"offline\");\n");
   buffer.AppendFormated("   if (this->isMidas())\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"InputDataFormat\",\"midas\");\n");
   buffer.AppendFormated("   else\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"InputDataFormat\",\"root\");\n");
   buffer.AppendFormated("   if (isBatchMode())\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"BatchMode\",\"yes\");\n");
   buffer.AppendFormated("   else\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"BatchMode\",\"no\");\n");
   buffer.AppendFormated("   if (isSplashScreen())\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"ShowSplashScreen\",\"yes\");\n");
   buffer.AppendFormated("   else\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"ShowSplashScreen\",\"no\");\n");
   buffer.AppendFormated("   if (this->isXMLDataBase())\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"DataBaseMode\",\"xml\");\n");
   buffer.AppendFormated("   else if (this->isSQLDataBase())\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"DataBaseMode\",\"sql\");\n");
   buffer.AppendFormated("   else\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"DataBaseMode\",\"none\");\n");
   buffer.AppendFormated("   xml->EndElement();\n");
   //run numbers
   buffer.AppendFormated("   xml->StartElement(\"RunNumbers\");\n");
   buffer.AppendFormated("   xml->WriteAttribute(\"Numbers\",(char*)this->GetRunNumberStringOriginal());\n");
   buffer.AppendFormated("   xml->EndElement();\n");
   //event numbers
   buffer.AppendFormated("   xml->StartElement(\"EventNumbers\");\n");
   buffer.AppendFormated("   xml->WriteAttribute(\"Numbers\",(char*)this->GetEventNumberStringOriginal());\n");
   buffer.AppendFormated("   xml->EndElement();\n");
   //paths
   buffer.AppendFormated("   xml->StartElement(\"Paths\");\n");
   buffer.AppendFormated("   xml->WriteElement(\"InputFilePath\",(char*)this->GetInputDir());\n");
   buffer.AppendFormated("   xml->WriteElement(\"OutputFilePath\",(char*)this->GetOutputDir());\n");
   buffer.AppendFormated("   xml->WriteElement(\"DataBaseFilePath\",(char*)this->GetDataBaseDir());\n");
   buffer.AppendFormated("   xml->EndElement();\n");
   buffer.AppendFormated("\n");

   //tasks
   buffer.AppendFormated("   xml->StartElement( \"Tasks\");\n");
   bool romeStringDeclared = false;
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("   xml->StartElement(\"%s\");\n",taskName[i].Data());
      buffer.AppendFormated("   if (f%sTask->IsActive())\n",taskName[i].Data());
      buffer.AppendFormated("      xml->WriteAttribute(\"Active\",\"yes\");\n");
      buffer.AppendFormated("   else\n");
      buffer.AppendFormated("      xml->WriteAttribute(\"Active\",\"no\");\n");
      for (j=0;j<numOfHistos[i];j++) {
         buffer.AppendFormated("   xml->StartElement(\"%s\");\n",histoName[i][j].Data());
         buffer.AppendFormated("   if (((%sT%s*)f%sTask)->is%sAccumulation())\n",shortCut.Data(),taskName[i].Data(),taskName[i].Data(),histoName[i][j].Data());
         buffer.AppendFormated("      xml->WriteAttribute(\"Accumulate\",\"yes\");\n");
         buffer.AppendFormated("   else\n");
         buffer.AppendFormated("      xml->WriteAttribute(\"Accumulate\",\"no\");\n");
         buffer.AppendFormated("   xml->EndElement();\n");
      }
      if (numOfTaskSteering[i]>0) {
         if (!romeStringDeclared)
            buffer.AppendFormated("   ROMEString value;\n");
         romeStringDeclared = true;
         WriteTaskSteerConfigWrite(buffer,0,i);
      }
      buffer.AppendFormated("      xml->EndElement();\n");
   }
   buffer.AppendFormated("      xml->EndElement();\n");

   //trees
   buffer.AppendFormated("   xml->StartElement(\"Trees\");\n");
   buffer.AppendFormated("   if (this->isTreeAccumulation())\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"Accumulation\",\"yes\");\n");
   buffer.AppendFormated("   else\n");
   buffer.AppendFormated("      xml->WriteAttribute(\"Accumulation\",\"no\");\n");

   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormated("   xml->StartElement(\"%s\");\n",treeName[i].Data());
      buffer.AppendFormated("   if (this->GetTreeObjectAt(%d)->isRead())\n",i);
      buffer.AppendFormated("      xml->WriteAttribute(\"Read\",\"yes\");\n");
      buffer.AppendFormated("   else\n");
      buffer.AppendFormated("      xml->WriteAttribute(\"Read\",\"no\");\n");
      buffer.AppendFormated("   if (this->GetTreeObjectAt(%d)->isWrite())\n",i);
      buffer.AppendFormated("      xml->WriteAttribute(\"Write\",\"yes\");\n");
      buffer.AppendFormated("   else\n");
      buffer.AppendFormated("      xml->WriteAttribute(\"Write\",\"no\");\n");
      buffer.AppendFormated("   xml->EndElement();\n");
   }
   buffer.AppendFormated("      xml->EndElement();\n");

   //steering
   if (numOfSteering>0) {
      if (!romeStringDeclared)
        buffer.AppendFormated("   ROMEString value;\n");
      WriteSteerConfigWrite(buffer,0);
   }

   buffer.AppendFormated("   xml->EndDocument();\n");
   buffer.AppendFormated("   return true;\n");
   buffer.AppendFormated("}\n");

   // Splash Screen
   buffer.AppendFormated("#if defined( _MSC_VER )\n");
   buffer.AppendFormated("LPDWORD lpThreadId;\n");
   buffer.AppendFormated("void %sAnalyzer::startSplashScreen() {\n",shortCut.Data());
   buffer.AppendFormated("   CloseHandle(CreateThread(NULL,1024,&SplashThread,0,0,lpThreadId));\n");
   buffer.AppendFormated("}\n");
   buffer.AppendFormated("#endif\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("#if defined ( __linux__ )\n");
   buffer.AppendFormated("void %sAnalyzer::startSplashScreen() {\n",shortCut.Data());
   buffer.AppendFormated("   \n");
   buffer.AppendFormated("}\n");
   buffer.AppendFormated("#endif\n");
   buffer.AppendFormated("\n");

   // Console Screen
   ROMEString prog;
   prog.SetFormated("%s%s",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("void %sAnalyzer::consoleStartScreen() {\n",shortCut.Data());
   buffer.AppendFormated("   int i;\n");
   buffer.AppendFormated("   cout << \"*****************************************\" << endl;\n");   
   buffer.AppendFormated("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*\";\n");
   int len1 = (int)((39-(double)prog.Length())/2+0.5);
   int len2 = (int)((39-(double)prog.Length())/2);
   buffer.AppendFormated("   for (i=0;i<%d;i++) cout << \" \";\n",len1);
   buffer.AppendFormated("   cout << \"%s\";\n",prog.Data());
   buffer.AppendFormated("   for (i=0;i<%d;i++) cout << \" \";\n",len2);
   buffer.AppendFormated("   cout << \"*\" << endl;\n");
   buffer.AppendFormated("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*   generated by the ROME Environment   *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*             %s              *\" << endl;\n",romeVersion.Data());
   buffer.AppendFormated("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*                                       *\" << endl;\n");
   buffer.AppendFormated("   cout << \"*****************************************\" << endl << endl;\n");
   buffer.AppendFormated("}\n");
   buffer.AppendFormated("   \n");

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
   hFile.SetFormated("%s/include/framework/%sAnalyzer.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormated("// If you intend to change this file please contact:                          //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormated("//                                                                            //\n");
   buffer.AppendFormated("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormated("#ifndef %sAnalyzer_H\n",shortCut.Data());
   buffer.AppendFormated("#define %sAnalyzer_H\n\n",shortCut.Data());

   buffer.AppendFormated("#include <TTask.h>\n");
   buffer.AppendFormated("#include <TTree.h>\n");
   buffer.AppendFormated("#include <TFolder.h>\n");
   buffer.AppendFormated("#include <TClonesArray.h>\n");
   buffer.AppendFormated("#include <ROMEString.h>\n");
   buffer.AppendFormated("#include <ROMETask.h>\n");
   buffer.AppendFormated("#include <ROMEAnalyzer.h>\n");

   if (numOfSteering!=0) {
      buffer.AppendFormated("#include <include/framework/%sGeneralSteering.h>\n",shortCut.Data());
   }

   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         buffer.AppendFormated("#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormated("\n");

   // bank structures
   if (numOfBank>0)
      buffer.AppendFormated("   // Bank Structures\n");
   for (i=0;i<numOfBank;i++) {
      if (bankType[i]=="structure"||bankType[i]=="struct") {
         buffer.AppendFormated("typedef struct {\n");
         for (j=0;j<numOfStructFields[i];j++) {
            if (structFieldSize[i][j].Length()>0)
               buffer.AppendFormated("   %s %s : %s;\n",structFieldType[i][j].Data(),structFieldName[i][j].Data(),structFieldSize[i][j].Data());
            else
               buffer.AppendFormated("   %s %s;\n",structFieldType[i][j].Data(),structFieldName[i][j].Data());
         }
         buffer.AppendFormated("} %s;\n",bankStructName[i].Data());
      }
   }

   // Class
   buffer.AppendFormated("\nclass %sAnalyzer : public ROMEAnalyzer\n",shortCut.Data());
   buffer.AppendFormated("{\n");

   // Fields
   buffer.AppendFormated("private:\n");

   // Task Switches Structure
   buffer.AppendFormated("   // Task Switches Structure\n");
   buffer.AppendFormated("   struct TaskSwitches{\n");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("      int %s;   //! %s Task\n",taskName[i].Data(),taskName[i].Data());
   }
   buffer.AppendFormated("   } fTaskSwitches;               //! Task Switches\n\n");
   buffer.AppendFormated("   ROMEString fTaskSwitchesString;   //! String describing Task Switches\n");
   buffer.AppendFormated("\n");


   buffer.AppendFormated("protected:\n");

   // Folder Fields
   buffer.AppendFormated("   // Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            format.SetFormated("   %%s%%s*%%%ds f%%sObject; %%%ds // Handle to %%s%%s Object\n",typeLen-folderName[i].Length()-scl,nameLen-folderName[i].Length());
            buffer.AppendFormated((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
         else {
            format.SetFormated("   TClonesArray*%%%ds f%%sObjects;%%%ds // Handle to %%s%%s Objects\n",typeLen-12,nameLen-folderName[i].Length());
            buffer.AppendFormated((char*)format.Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormated("\n");

   // Task Fields
   buffer.AppendFormated("   // Task Fields\n");
   for (i=0;i<numOfTask;i++) {
      format.SetFormated("   ROMETask* f%%sTask%%%ds;  // Handle to %%s Task\n",taskLen-taskName[i].Length());
      buffer.AppendFormated((char*)format.Data(),taskName[i].Data(),"",taskName[i].Data());
   }
   buffer.AppendFormated("\n");

   // Steering Fields
   if (numOfSteering!=0) {
      buffer.AppendFormated("   // Steering Parameter Fields\n");
      buffer.AppendFormated("\n   %sGeneralSteering* fGeneralSteeringParameters; // Handle to the GeneralSteering Class\n",shortCut.Data());
   }

   // Bank Fields
   if (numOfBank>0) {
      int bankNameLen = -1;
      int bankTypeLen = -1;
      for (i=0;i<numOfBank;i++) {
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            if (bankTypeLen<(int)bankStructName[i].Length()) bankTypeLen = bankStructName[i].Length();
         }
         else {
            if (bankTypeLen<(int)bankType[i].Length()) bankTypeLen = bankType[i].Length();
         }
         if (bankNameLen<(int)bankName[i].Length()) bankNameLen = bankName[i].Length();
      }
      buffer.AppendFormated("   // Bank Fields\n");
      for (i=0;i<numOfBank;i++) {
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            format.SetFormated("   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankStructName[i].Length(),bankNameLen-bankName[i].Length());
            buffer.AppendFormated((char*)format.Data(),bankStructName[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
         }
         else {
            format.SetFormated("   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i].Length(),bankNameLen-bankName[i].Length());
            buffer.AppendFormated((char*)format.Data(),bankType[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
         }
         format.SetFormated("   int%%%ds f%%sBankLength;  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-2,bankNameLen-bankName[i].Length());
         buffer.AppendFormated((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
         format.SetFormated("   bool%%%ds f%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-bankName[i].Length());
         buffer.AppendFormated((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
      }
      buffer.AppendFormated("\n");
   }

   // Methods
   buffer.AppendFormated("public:\n");
   // Constructor
   buffer.AppendFormated("   %sAnalyzer();\n",shortCut.Data());

   // Getters
   buffer.AppendFormated("   // Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         int lt = typeLen-folderName[i].Length()-scl+nameLen-folderName[i].Length();
         if (folderArray[i]=="1") {
            format.SetFormated("   %%s%%s*%%%ds  Get%%sObject()%%%ds { return f%%sObject;%%%ds };\n",typeLen-folderName[i].Length()-scl,8+nameLen-folderName[i].Length(),15+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormated((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormated("   %%s%%s**%%%ds Get%%sObjectAddress()%%%ds { return &f%%sObject;%%%ds };\n",typeLen-folderName[i].Length()-scl,1+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormated((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
         else {
            format.SetFormated("   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds { return (%%s%%s*)f%%sObjects->At(index);%%%ds };\n",typeLen-folderName[i].Length()-scl,3+nameLen-folderName[i].Length(),lt);
            buffer.AppendFormated((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),"");
            format.SetFormated("   TClonesArray*%%%ds  Get%%sObjects()%%%ds { return f%%sObjects;%%%ds };\n",typeLen-12,7+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormated((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormated("   TClonesArray**%%%ds Get%%sObjectsAddress()%%%ds { return &f%%sObjects;%%%ds };\n",typeLen-12,nameLen-folderName[i].Length(),13+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormated((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
      }
   }
   buffer.AppendFormated("\n");

   // Banks
   if (numOfBank>0) {
      buffer.AppendFormated("   // Bank Methodes\n");
      buffer.AppendFormated("   void InitMidasBanks();\n");
      for (i=0;i<numOfBank;i++) {
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            buffer.AppendFormated("   %s* Get%sBankAt(int index);\n",bankStructName[i].Data(),bankName[i].Data());
         }
         else {
            buffer.AppendFormated("   %s Get%sBankAt(int index);\n",bankType[i].Data(),bankName[i].Data());
         }
         buffer.AppendFormated("   int Get%sBankEntries();\n",bankName[i].Data());
         buffer.AppendFormated("   void Init%sBank();\n",bankName[i].Data());
      }
      buffer.AppendFormated("\n");
   }

   // Task Switches
   buffer.AppendFormated("   // Task Switches\n");
   buffer.AppendFormated("   TaskSwitches* GetTaskSwitches() { return &fTaskSwitches; };\n");
   buffer.AppendFormated("   int           GetTaskSwitchesSize() { return sizeof(TaskSwitches); };\n");
   buffer.AppendFormated("   char*         GetTaskSwitchesString() { return (char*)fTaskSwitchesString.Data(); };\n");
   buffer.AppendFormated("\n");

   // ODB
   buffer.AppendFormated("   // ODB Methodes\n");
   buffer.AppendFormated("   bool InitODB();\n");
   buffer.AppendFormated("\n");

   // Data Base
   buffer.AppendFormated("   // DataBase Methodes\n");
   buffer.AppendFormated("   bool InitSQLDataBase();\n");
   buffer.AppendFormated("   bool ReadSQLDataBase();\n");
   buffer.AppendFormated("   bool ReadXMLDataBase();\n");
   buffer.AppendFormated("\n");

   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i]) {
         buffer.AppendFormated("   void Write%sDataBase();\n",folderName[i].Data());
      }
   }
   buffer.AppendFormated("\n");

   // Trees
   buffer.AppendFormated("   // Tree Methodes\n");
   buffer.AppendFormated("   void ConnectTrees();\n");
   buffer.AppendFormated("   void FillTrees();\n");
   buffer.AppendFormated("   void ClearFolders();\n");

   // Configuration file
   buffer.AppendFormated("   // Configuration File\n");
   buffer.AppendFormated("   bool ReadROMEConfigXML(char *configFile);\n");
   buffer.AppendFormated("   bool WriteROMEConfigXML(char *configFile);\n");
   buffer.AppendFormated("\n");
 
   // Steering
   if (numOfSteering>0) {
      buffer.AppendFormated("   // Steering Parameter Methodes\n");
      buffer.AppendFormated("   %sGeneralSteering* GetGeneralSteeringParameters() { return fGeneralSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormated("   %sGeneralSteering* GetGSP() { return fGeneralSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormated("\n");
   }

   buffer.AppendFormated("   void InitFolders();\n\n");
   buffer.AppendFormated("   void InitTaskSwitches();\n\n");
   buffer.AppendFormated("   void UpdateTaskSwitches();\n\n");

   // Private
   buffer.AppendFormated("private:\n");
   buffer.AppendFormated("   void startSplashScreen();\n");
   buffer.AppendFormated("   void consoleStartScreen();\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   void NextFile(ROMEString& nextFile,ROMEString& file);\n");

   // Footer
   buffer.AppendFormated("};\n\n");

   buffer.AppendFormated("#endif   // %sAnalyzer_H\n",shortCut.Data());

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

   cppFile.SetFormated("%s/src/framework/main.cpp",outDir.Data());

   buffer.Resize(0);
   buffer.AppendFormated("#include <TApplication.h>\n");
   buffer.AppendFormated("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormated("#include <Riostream.h>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("int main(int argc, char *argv[])\n");
   buffer.AppendFormated("{\n");
   buffer.AppendFormated("   int argn = 1;\n");
   buffer.AppendFormated("   char arg[1][100];\n");
   buffer.AppendFormated("   char *argp = &arg[0][0];\n");
   buffer.AppendFormated("   strcpy(arg[0],argv[0]);\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   TApplication theApp(\"App\", &argn, &argp);\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   %sAnalyzer *analyzer = new %sAnalyzer();\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   if (!analyzer->Start(argc, argv)) {\n");
   buffer.AppendFormated("      delete analyzer;\n");
   buffer.AppendFormated("      return 1;\n");
   buffer.AppendFormated("   }\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   theApp.Run();\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   delete analyzer;\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("   return 0;\n");
   buffer.AppendFormated("}\n");

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
int main(int argc, char *argv[])
{
   ROMEBuilder* romeb = new ROMEBuilder();

   romeb->romeVersion = "Version 1.00";
   
   ROMEString xmlFile = "";

   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);

   romeb->makeOutput = false;
   romeb->noLink = false;
   romeb->offline = false;
   romeb->sql = true;

   ROMEString midasFile;
   midasFile = getenv("MIDASSYS");
   midasFile.Append("/include/midas.h");
   struct stat buf;
   if( stat( midasFile, &buf )) {
      romeb->offline = true;
   }
   
   romeb->outDir = workDir;
   romeb->outDir.Append("/");

   if (argc==1) {
      cout << "  -i        Inputfile" << endl;
      cout << "  -o        Outputfile path" << endl;
      cout << "  -v        Verbose Mode (no Argument)" << endl;
      cout << "  -nl       No Linking (no Argument)" << endl;
      cout << "  -offline  Generated program works only offline (no midas library needed) (no Argument)" << endl;
      cout << "  -nosql    Generated program has no SQL data base access (no sql library needed) (no Argument)" << endl;
      return 0;
   }
   for (int i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-v")) {
         romeb->makeOutput = true;
      }
      else if (!strcmp(argv[i],"-nl")) {
         romeb->noLink = true;
      }
      else if (!strcmp(argv[i],"-nosql")) {
         romeb->sql = false;
      }
      else if (!strcmp(argv[i],"-offline")) {
         romeb->offline = true;
      }
      else if (!strcmp(argv[i],"-offline")) {
         romeb->sql = false;
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
         cout << "  -i         Inputfile" << endl;
         cout << "  -o         Outputfile path" << endl;
         cout << "  -v         Verbose Mode (no Argument)" << endl;
         cout << "  -nl        No Linking (no Argument)" << endl;
         cout << "  -offline  Generated program works only offline (no midas library needed) (no Argument)" << endl;
         cout << "  -nosql    Generated program has no SQL data base access (no sql library needed) (no Argument)" << endl;
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
   path.SetFormated("%s/src",romeb->outDir.Data());
   mkdir(path);
   path.SetFormated("%s/src/tasks",romeb->outDir.Data());
   mkdir(path);
   path.SetFormated("%s/src/framework",romeb->outDir.Data());
   mkdir(path);
   path.SetFormated("%s/include/",romeb->outDir.Data());
   mkdir(path);
   path.SetFormated("%s/include/tasks",romeb->outDir.Data());
   mkdir(path);
   path.SetFormated("%s/include/framework",romeb->outDir.Data());
   mkdir(path);
#endif

#if defined ( __linux__ )
   path.SetFormated("%s/src",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormated("%s/src/tasks",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormated("%s/src/framework",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormated("%s/include/",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormated("%s/include/tasks",romeb->outDir.Data());
   mkdir(path,0711);
   path.SetFormated("%s/include/framework",romeb->outDir.Data());
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
   
   numOfFolder = 0;
   numOfTask = 0;
   numOfTree = 0;
   numOfBank = 0;
   numOfSteering = 0;

   mainAuthor = "";
   mainInstitute = "";
   mainCollaboration = "";
   mainEmail = "";
   mainProgName = "";
      
   if (!xml->OpenFileForRead(xmlFile)) return;
   while (xml->NextLine()&&!finished) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1) {
         shortCut = (char*)name;
         while (xml->NextLine()&&!finished) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 15 && !strcmp((const char*)name,shortCut)) {
               finished = true;
               break;
            }
            if (type == 1) {
               if (!strcmp((const char*)name,"Author")) {
                  xml->GetAttribute("Name",mainAuthor,mainAuthor);
                  xml->GetAttribute("Institute",mainInstitute,mainInstitute);
                  xml->GetAttribute("Collaboration",mainCollaboration,mainCollaboration);
                  xml->GetAttribute("Email",mainEmail,mainEmail);
               }
               if (!strcmp((const char*)name,"Programname")) {
                  xml->GetAttribute("Name",mainProgName,mainProgName);
               }
               if (!strcmp((const char*)name,"Folder")) {
                  numOfFolder = -1;
                  if (!ReadXMLFolder()) return;
                  if (!WriteFolderCpp()) return;
                  if (!WriteFolderH()) return;
               }
               if (!strcmp((const char*)name,"Task")) {
                  numOfTask = -1;
                  if (!ReadXMLTask()) return;
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
               if (!strcmp((const char*)name,"GeneralSteeringParameters")) {
                  numOfSteering = 0;
                  if (!ReadXMLSteering()) return;
                  if (!WriteSteering()) return;
               }
            }
         }
      }
   }
   if (makeOutput) cout << "\n\nFramework :" << endl;
   if (!WriteAnalyzerCpp()) return;
   if (!WriteAnalyzerH()) return;
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
      system("make");
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

#if defined( _MSC_VER )
   // libs
   buffer.Resize(0);
   buffer.AppendFormated("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libEG.lib $(ROOTSYS)/lib/libEGPythia6.lib $(ROOTSYS)/lib/libFumili.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libGeomPainter.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHbook.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMLP.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libProof.lib $(ROOTSYS)/lib/libProofGui.lib $(ROOTSYS)/lib/libRFIO.lib $(ROOTSYS)/lib/libRGL.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTable.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libVMC.lib $(ROOTSYS)/lib/libWin32gdk.lib $(ROOTSYS)/lib/libfreetype.lib\n");
   buffer.AppendFormated("xmllibs = $(ROMESYS)/lib_win/libxml2.lib $(ROMESYS)/lib_win/iconv.lib $(ROMESYS)/lib_win/zlib.lib\n");
   if (this->sql) 
      buffer.AppendFormated("sqllibs = $(ROMESYS)/lib_win/libmySQL.lib $(ROMESYS)/lib_win/mysys.lib $(ROMESYS)/lib_win/mysqlclient.lib\n");
   else
      buffer.AppendFormated("sqllibs = \n");
   if (!this->offline) 
      buffer.AppendFormated("midaslibs = $(MIDASSYS)/nt/lib/midas.lib\n");
   else
      buffer.AppendFormated("midaslibs = \n");
   buffer.AppendFormated("clibs = gdi32.lib user32.lib kernel32.lib\n");
   buffer.AppendFormated("Libraries = $(rootlibs) $(xmllibs) $(clibs) $(sqllibs) $(midaslibs)\n");
   buffer.AppendFormated("\n");
   // flags
   buffer.AppendFormated("Flags = /GX /GR");
   if (!this->offline) 
      buffer.AppendFormated(" /DHAVE_MIDAS");
   if (this->sql) 
      buffer.AppendFormated(" /DHAVE_SQL");
   buffer.AppendFormated("\n");
   // includes
   buffer.AppendFormated("Includes = /I$(ROMESYS)/include/ /I$(ROOTSYS)/include/ /I. /Iinclude/ /Iinclude/tasks/ /Iinclude/framework/ ");
   if (!this->offline) 
      buffer.AppendFormated(" /I$(MIDASSYS)/include/");
   if (this->sql) 
      buffer.AppendFormated(" /I$(ROMESYS)/include/mysql/");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("\n");
   // objects
   buffer.AppendFormated("objects =");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormated(" obj/%s%s.obj",shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated(" obj/%sT%s.obj",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormated(" obj/%sAnalyzer.obj obj/%sDict.obj obj/main.obj",shortCut.Data(),shortCut.Data(),shortCut.Data());
   if (this->sql) 
      buffer.AppendFormated(" obj/ROMESQL.obj");
   buffer.AppendFormated(" obj/ROMEAnalyzer.obj obj/ROMEEventLoop.obj obj/ROMETask.obj obj/ROMESplashScreen.obj obj/ROMEXML.obj obj/ROMEString.obj\n\n");
   // all
   buffer.AppendFormated("all:obj %s%s.exe\n",shortCut.Data(),mainProgName.Data());
   // make obj
   buffer.AppendFormated("obj:\n");
	buffer.AppendFormated("\tif not exist obj mkdir obj\n");
   // link
   buffer.AppendFormated("%s%s.exe: $(objects)\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("	cl /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
   // compile
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormated("obj/%s%s.obj: src/framework/%s%s.cpp\n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
      buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/%s%s.obj src/framework/%s%s.cpp \n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("obj/%sT%s.obj: src/tasks/%sT%s.cpp\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/%sT%s.obj src/tasks/%sT%s.cpp \n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormated("obj/%sAnalyzer.obj: src/framework/%sAnalyzer.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/%sAnalyzer.obj src/framework/%sAnalyzer.cpp \n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("obj/%sDict.obj: %sDict.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/%sDict.obj %sDict.cpp \n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("obj/main.obj: src/framework/main.cpp\n");
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/main.obj src/framework/main.cpp \n");

   buffer.AppendFormated("obj/ROMEAnalyzer.obj: $(ROMESYS)/src/ROMEAnalyzer.cpp\n");
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/ROMEAnalyzer.obj $(ROMESYS)/src/ROMEAnalyzer.cpp \n");
   buffer.AppendFormated("obj/ROMEEventLoop.obj: $(ROMESYS)/src/ROMEEventLoop.cpp\n");
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/ROMEEventLoop.obj $(ROMESYS)/src/ROMEEventLoop.cpp \n");
   buffer.AppendFormated("obj/ROMETask.obj: $(ROMESYS)/src/ROMETask.cpp\n");
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/ROMETask.obj $(ROMESYS)/src/ROMETask.cpp \n");
   buffer.AppendFormated("obj/ROMESplashScreen.obj: $(ROMESYS)/src/ROMESplashScreen.cpp\n");
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/ROMESplashScreen.obj $(ROMESYS)/src/ROMESplashScreen.cpp \n");
   buffer.AppendFormated("obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp\n");
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/ROMEXML.obj $(ROMESYS)/src/ROMEXML.cpp \n");
   buffer.AppendFormated("obj/ROMEString.obj: $(ROMESYS)/src/ROMEString.cpp\n");
   buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/ROMEString.obj $(ROMESYS)/src/ROMEString.cpp \n");
   if (this->sql) {
      buffer.AppendFormated("obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp\n");
      buffer.AppendFormated("	cl $(Flags) $(Includes) /c /Foobj/ROMESQL.obj $(ROMESYS)/src/ROMESQL.cpp \n");
   }

#endif

#if defined ( __linux__ )
   // libs
   buffer.Resize(0);
   buffer.AppendFormated("rootlibs := $(shell root-config --libs)\n");
   buffer.AppendFormated("rootglibs := $(shell root-config --glibs)\n");
   buffer.AppendFormated("rootthreadlibs := -lThread\n");
   buffer.AppendFormated("xmllibs := -lxml2 -lz -liconv\n");
   if (this->sql) 
      buffer.AppendFormated("sqllibs := -lmysql\n");
   else
      buffer.AppendFormated("sqllibs := \n");
   if (!this->offline) 
      buffer.AppendFormated("midaslibs := -lmidas\n");
   else
      buffer.AppendFormated("midaslibs := \n");
   buffer.AppendFormated("clibs := -lpthread -lHtml $(SYSLIBS)\n");
   buffer.AppendFormated("Libraries := $(rootlibs) $(rootglibs) $(rootthreadlibs) $(xmllibs) $(clibs) $(sqllibs) $(midaslibs)\n");
   buffer.AppendFormated("\n");
   // flags
   buffer.AppendFormated("flags := ");
   if (!this->offline) 
      buffer.AppendFormated(" -DHAVE_MIDAS");
   if (this->sql) 
      buffer.AppendFormated(" -DHAVE_SQL");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("\n");
   // includes
   buffer.AppendFormated("Includes := -I$(ROMESYS)/include/ -I$(ROOTSYS)/include/ -I. -Iinclude/ -Iinclude/tasks/ -Iinclude/framework/ ");
   if (!this->offline) 
      buffer.AppendFormated(" -I$(MIDASSYS)/include/");
   if (this->sql) 
      buffer.AppendFormated(" -I$(ROMESYS)/include/mysql/");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("\n");
   // objects
   buffer.AppendFormated("objects :=");
   for (i=0;i<numOfFolder;i++) {
      buffer.AppendFormated(" obj/%s%s.obj",shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated(" obj/%sT%s.obj",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormated(" obj/%sAnalyzer.obj obj/%sDict.obj obj/main.obj",shortCut.Data(),shortCut.Data(),shortCut.Data());
   if (this->sql)
      buffer.AppendFormated(" obj/ROMESQL.obj");
   buffer.AppendFormated(" obj/ROMEAnalyzer.obj obj/ROMEEventLoop.obj obj/ROMETask.obj obj/ROMESplashScreen.obj obj/ROMEXML.obj obj/ROMEString.obj\n\n");
   // all
   ROMEString shortcut(shortCut);
   shortcut.ToLower();
   ROMEString mainprogname(mainProgName);
   mainprogname.ToLower();
   buffer.AppendFormated("all:obj %s%s.exe\n",shortcut.Data(),mainprogname.Data());
   // make obj
   buffer.AppendFormated("obj:\n");
	buffer.AppendFormated("\t@if [ ! -d  obj ] ; then \\\n");
	buffer.AppendFormated("\t\techo \"Making directory obj\" ; \\\n");
	buffer.AppendFormated("\t\tmkdir obj; \\\n");
	buffer.AppendFormated("\tfi;\n");
   // link
   buffer.AppendFormated("%s%s.exe: $(objects)\n",shortcut.Data(),mainprogname.Data());
   buffer.AppendFormated("	g++ -g -o $@ $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
   // compile
   for (i=0;i<numOfFolder;i++) {
      buffer.AppendFormated("obj/%s%s.obj: src/framework/%s%s.cpp\n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
      buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) src/framework/%s%s.cpp -o obj/%s%s.obj\n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("obj/%sT%s.obj: src/tasks/%sT%s.cpp\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) src/tasks/%sT%s.cpp -o obj/%sT%s.obj\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormated("obj/%sAnalyzer.obj: src/framework/%sAnalyzer.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) src/framework/%sAnalyzer.cpp -o obj/%sAnalyzer.obj\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("obj/%sDict.obj: %sDict.cpp\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) %sDict.cpp -o obj/%sDict.obj\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormated("obj/main.obj: src/framework/main.cpp\n");
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) src/framework/main.cpp -o obj/main.obj\n");

   buffer.AppendFormated("obj/ROMEAnalyzer.obj: $(ROMESYS)/src/ROMEAnalyzer.cpp\n");
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) $(ROMESYS)/src/ROMEAnalyzer.cpp -o obj/ROMEAnalyzer.obj\n");
   buffer.AppendFormated("obj/ROMEEventLoop.obj: $(ROMESYS)/src/ROMEEventLoop.cpp\n");
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) $(ROMESYS)/src/ROMEEventLoop.cpp -o obj/ROMEEventLoop.obj\n");
   buffer.AppendFormated("obj/ROMETask.obj: $(ROMESYS)/src/ROMETask.cpp\n");
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) $(ROMESYS)/src/ROMETask.cpp -o obj/ROMETask.obj\n");
   buffer.AppendFormated("obj/ROMESplashScreen.obj: $(ROMESYS)/src/ROMESplashScreen.cpp\n");
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) $(ROMESYS)/src/ROMESplashScreen.cpp -o obj/ROMESplashScreen.obj\n");
   buffer.AppendFormated("obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp\n");
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) $(ROMESYS)/src/ROMEXML.cpp -o obj/ROMEXML.obj\n");
   buffer.AppendFormated("obj/ROMEString.obj: $(ROMESYS)/src/ROMEString.cpp\n");
   buffer.AppendFormated("	g++ -g -c $(Flags) $(Includes) $(ROMESYS)/src/ROMEString.cpp -o obj/ROMEString.obj\n");
   if (this->sql) {
      buffer.AppendFormated("obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp\n");
      buffer.AppendFormated("	g++ -c $(Flags) $(Includes) $(ROMESYS)/src/ROMESQL.cpp -o obj/ROMESQL.obj\n");
   }
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
   buffer.AppendFormated("rootcint -f %sDict.cpp -c ",shortCut.Data());
#if defined( _MSC_VER )
   buffer.AppendFormated("-I%%ROMESYS%%/include ");
   buffer.AppendFormated("-I%%ROOTSYS%% ");
#endif
#if defined ( __linux__ )
   buffer.AppendFormated("-I$ROMESYS/include ");
   buffer.AppendFormated("-I$ROOTSYS ");
#endif
   buffer.AppendFormated("-Iinclude -Iinclude/tasks -Iinclude/framework ");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         buffer.AppendFormated("include/framework/%s%s.h ",shortCut.Data(),folderName[i].Data());
         if (folderUserCode[i])
            buffer.AppendFormated("include/framework/%s%s_Base.h ",shortCut.Data(),folderName[i].Data());
      }
   }
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("include/tasks/%sT%s.h ",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormated("ROMETask.h ROMETreeInfo.h \n");
   buffer.Append("\0");

#if defined( _MSC_VER )
   ROMEString batFile;
   batFile.SetFormated("%smakeDictionary.bat",outDir.Data());
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
   buffer.AppendFormated("<HTML>\n");
   buffer.AppendFormated("<HEAD>\n");
   buffer.AppendFormated("<TITLE>%s%s Manual</TITLE>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("</HEAD>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<H1>%s%s Manual</H1>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("\n");
   // Table of Contents
   buffer.AppendFormated("<H2>Table of Contents</H2>\n");
   buffer.AppendFormated("<ul>\n");
   buffer.AppendFormated("<li><a href=\"#introduction\">Introduction</a></li>\n");
   buffer.AppendFormated("<br>\n");
   buffer.AppendFormated("<li><a href=\"#objects\">Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("<ul>\n");
   buffer.AppendFormated("<li><a href=\"#taskobjects\">Tasks</a></li>\n");
   buffer.AppendFormated("<li><a href=\"#folderobjects\">Folders</a></li>\n");
   buffer.AppendFormated("<li><a href=\"#treeobjects\">Trees</a></li>\n");
   buffer.AppendFormated("<li><a href=\"#midasbankobjects\">Midas Banks</a></li>\n");
   buffer.AppendFormated("<br>\n");
   buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("<ul>\n");
   buffer.AppendFormated("<li><a href=\"#foldermethods\">Folders</a></li>\n");
   buffer.AppendFormated("<li><a href=\"#databasemethods\">Data Base</a></li>\n");
   buffer.AppendFormated("<li><a href=\"#midasbankmethods\">Midas Banks</a></li>\n");
   buffer.AppendFormated("<li><a href=\"#histogrammethods\">Histograms</a></li>\n");
   buffer.AppendFormated("<li><a href=\"#generalmethods\">General</a></li>\n");
   buffer.AppendFormated("<br>\n");
   buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("<li><A TARGET=_top HREF=\"%s/htmldoc/ClassIndex.html\">Class Overview</A></li>\n",outDir.Data());
   buffer.AppendFormated("<br>\n");
   buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<hr>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("\n");
   // Introduction
   buffer.AppendFormated("<H2><a name=introduction>Introduction</a> </H2>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("The %s%s consists mainly of folders and tasks.\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("Folders are objects, where you can store data in. Typically you will store the data of one detector (or subdetector) component in it.\n");
   buffer.AppendFormated("Like disk folders (directories) they are hierarchically arranged.\n");
   buffer.AppendFormated("Folders may have a data structure (unlike disk folders). The data objects are called fields. Folders without fields can be used to structure the project.\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("Tasks are objects, which privides actions. They make calculations, store and read data in folders, fill trees and histograms and so on.\n");
   buffer.AppendFormated("Tasks are also hierarchically arranged. That means that a task may have a subtask, which is executed after the task itself has been executed.\n");
   buffer.AppendFormated("Task also own histograms, which means that all histograms in this frame work belong to a task. The booking and saving of histograms is made by the frame work.\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<hr>\n");
   buffer.AppendFormated("<p>\n");
   // Objects
   buffer.AppendFormated("<H2><a name=objects>Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("All <a href=\"#taskobjects\">Tasks</a>, <a href=\"#folderobjects\">Folders</a> and <a href=\"#treeobjects\">Trees</a> are described here.\n");
   // Tasks
   buffer.AppendFormated("<h3><a name=taskobjects>Tasks</a></h3>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("The %s%s consists of the following tasks :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("\n");

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
      if (depth<depthold) buffer.AppendFormated("</ul>\n");
      if (depth>depthold) buffer.AppendFormated("<ul>\n");
      buffer.AppendFormated("<li type=\"circle\"><h4><a href=\"#%s\">%sT%s</a></h4></li>\n",taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<p><b>Note</b> : The user should write code into the Init(), BeginOfRun(), Event(), EndOfRun() and Terminate() methods of the tasks methods file (.cpp). But the user may not add code to or alter code in the class file (.h). The class file (.h) is generated by the framework. Manual changes to this file will allways be overwritten.\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("In the following all tasks will be described.\n");
   buffer.AppendFormated("<p>\n");

   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormated("<h4><a name=%s><u>%s</u></a></h4>\n",taskName[i].Data(),taskName[i].Data());
      buffer.AppendFormated("%s<p>\n",taskDescription[i].Data());
      if (numOfHistos[i]>0) {
         buffer.AppendFormated("This task containes the following histograms :\n");
         buffer.AppendFormated("<ul>\n");
         for (j=0;j<numOfHistos[i];j++) {
            buffer.AppendFormated("<li type=\"circle\">%s</li>\n",histoName[i][j].Data());
         }
         buffer.AppendFormated("</ul>\n");
         buffer.AppendFormated("\n");
         buffer.AppendFormated("\n");
         buffer.AppendFormated("\n");
         buffer.AppendFormated("\n");
      }
      else {
         buffer.AppendFormated("This task containes no histograms.\n");
      }
      buffer.AppendFormated("<p>\n");
      buffer.AppendFormated("For more information take a look at the <A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">class file</a>\n",outDir.Data(),shortCut.Data(),taskName[i].Data());
      buffer.AppendFormated("<p>\n");
      buffer.AppendFormated("\n");
   }
   buffer.AppendFormated("<hr>\n");
   buffer.AppendFormated("<p>\n");

   // Folders
   buffer.AppendFormated("<h3><a name=folderobjects>Folders</a></h3>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("The %s%s incorporates the following folders :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<ul>\n");

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
      if (depth<depthold) buffer.AppendFormated("</ul>\n");
      if (depth>depthold) buffer.AppendFormated("<ul>\n");
      if (numOfValue[i] > 0) {
         buffer.AppendFormated("<b>\n");
         buffer.AppendFormated("<li type=\"circle\"><a href=\"#%s\">%s</a></li>\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormated("</b>\n");
      }
      else {
         buffer.AppendFormated("<li type=\"circle\">%s</li>\n",folderName[i].Data());
      }
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("The bold folders are data folders. The others are only used to structure the frame work.\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("In the following all folders will be described.\n");
   buffer.AppendFormated("<p>\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] <= 0) continue;
      buffer.AppendFormated("<h4><a name=%s><u>%s</u></a></h4>\n",folderName[i].Data(),folderName[i].Data());
      buffer.AppendFormated("%s\n",folderDescription[i].Data());
      buffer.AppendFormated("<p>\n");
      buffer.AppendFormated("<u>Fields</u>\n");
      buffer.AppendFormated("<table border=\"1\">\n");
      buffer.AppendFormated("<tr><td>Name</td><td>Type</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         buffer.AppendFormated("<tr><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",valueName[i][j].Data(),valueType[i][j].Data());
      }
      buffer.AppendFormated("</table>\n");

   }
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<hr>\n");
   buffer.AppendFormated("<p>\n");

   // Trees
   buffer.AppendFormated("<h3><a name=treeobjects>Trees</a></h3>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("The %s%s incorporates the following trees :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<ul>\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormated("<li type=\"disc\">%s</li>\n",treeName[i].Data());
   }
   buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<hr>\n");
   buffer.AppendFormated("<p>\n");

   // Midas Banks
   buffer.AppendFormated("<h3><a name=midasbankobjects>Midas Banks</a></h3>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("The %s%s incorporates the following midas banks :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<ul>\n");
   for (i=0;i<numOfBank;i++) {
      if (bankType[i]=="structure"||bankType[i]=="struct") {
         buffer.AppendFormated("<li type=\"disc\">%s</li>\n",bankName[i].Data());
         buffer.AppendFormated("<ul>\n");
         for (j=0;j<numOfStructFields[i];j++) {
            buffer.AppendFormated("<li type=\"disc\">%s</li>\n",structFieldName[i][j].Data());
         }
         buffer.AppendFormated("</ul>\n");
      }
      else {
         buffer.AppendFormated("<li type=\"disc\">%s</li>\n",bankName[i].Data());
      }
   }
   buffer.AppendFormated("</ul>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<hr>\n");
   buffer.AppendFormated("<p>\n");

   // Access Methods
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("In the following the access methods of all types of objects in the %s%s are discussed.\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("All task can access these methods over their <b>fAnalyzer</b> handle, which every task has by\n");
   buffer.AppendFormated("default.<br>\n");
   buffer.AppendFormated("<p>\n");

   buffer.AppendFormated("\n");
   buffer.AppendFormated("<h3><a name=foldermethods>Folders</a></h3>\n");
   buffer.AppendFormated("To access a folder one has to get a handle to it with the following methods : <p>\n");
   buffer.AppendFormated("<table border=\"0\">\n");
   buffer.AppendFormated("<tr><td><b>Get[<i>Folder Name</i>]At([<i>Index</i>])</b></td><td>&nbsp;&nbsp;&nbsp;for object arrays.</td></tr>\n");
   buffer.AppendFormated("<tr><td><b>Get[<i>Folder Name</i>]Object()</td><td>&nbsp;&nbsp;&nbsp;for single objects.</td></tr>\n");
   buffer.AppendFormated("</table>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<b><i>Folder Name</i></b> stands for the name of the folder specified in the xml file (see also list above).</br>\n");
   buffer.AppendFormated("<b><i>Index</i></b> stands for the array index of the object.</br>\n");
   buffer.AppendFormated("These methods return a pointer on the class of this folder.</br>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("The get or set the content of a folder one has to use the following methods : <p>\n");
   buffer.AppendFormated("<table border=\"0\">\n");
   buffer.AppendFormated("<tr><td><b>Get[<i>Field Name</i>]()</b></td><td>&nbsp;&nbsp;&nbsp;getter.</td></tr>\n");
   buffer.AppendFormated("<tr><td><b>Set[<i>Field Name</i>]([<i>Value</i>])</td><td>&nbsp;&nbsp;&nbsp;setter.</td></tr>\n");
   buffer.AppendFormated("</table>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<b><i>Field Name</i></b> stands for the name of the field to access specified in the xml file (see also list above).</br>\n");
   buffer.AppendFormated("<b><i>Value</i></b> stands for the value given to the Field.</br>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<b>Example:</b> To give the field 'YY' of the 9 folder of a folderarray 'XX' the value 99 one has to type this :<p>\n");
   buffer.AppendFormated("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fAnalyzer->GetXXAt(9)->SetYY(99) <p>\n");
   buffer.AppendFormated("<p>\n");

   // Data Base
   buffer.AppendFormated("<h3><a name=databasemethods><u>Data Base</u></a></h3>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("To add an entry to the data base the following methods are available :\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<b>Write[<i>Folder Name</i>]DataBase(this)</b>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<b><i>Folder Name</i></b> stands for the name of the folder, which is to be written to the data base (see also list above).</br>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("\n");

   // Midas Banks
   buffer.AppendFormated("<h3><a name=midasbankmethods><u>Midas Banks</u></a></h3>\n");
   buffer.AppendFormated("To access a bank in a midas input file the following methods are available :\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("One can get the entries in a bank with the following method :\n");
   buffer.AppendFormated("<b>Get[<i>Bank Name</i>]BankEntries()</b>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("The data can be accessed with :\n");
   buffer.AppendFormated("<b>Get[<i>Bank Name</i>]BankAt([<i>Index</i>])</b>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<b><i>Bank Name</i></b> stands for the name of the bank.</br>\n");
   buffer.AppendFormated("<b><i>Index</i></b> stands for the arrayindex of the value.</br>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("If the bank is a structured bank, the data access method returns a pointer on the structure.</br>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<b>Example:</b> To get the 9 value 'YY' of a structured bank 'XX' one has to type this :<p>\n");
   buffer.AppendFormated("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fAnalyzer->GetXXBankAt(9)->YY <p>\n");
   buffer.AppendFormated("\n");

   // Histos
   buffer.AppendFormated("<h3><a name=histogrammethods><u>Histograms</u></a></h3>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("Histograms belong to a task. So a task can only access his own histograms. To do this the following methods are available :\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<table border=\"0\">\n");
   buffer.AppendFormated("<tr><td><b>Fill[<i>Histo Name</i>]([<i>xValue</i>],[<i>weight</i>])</b></td>  <td>&nbsp;&nbsp;&nbsp;fills a single histogram.</td></tr>\n");
   buffer.AppendFormated("<tr><td><b>Draw[<i>Histo Name</i>]()</b></td>       <td>&nbsp;&nbsp;&nbsp;draws a single histogram.</td></tr>\n");
   buffer.AppendFormated("<tr><td><b>Get[<i>Histo Name</i>]Handle()</b></td>  <td>&nbsp;&nbsp;&nbsp;gets the handle to a single histogram.</td></tr>\n");
   buffer.AppendFormated("<tr><td><b>Fill[<i>Histo Name</i>]At([<i>Index</i>],[<i>xValue</i>],[<i>weight</i>])</b></td><td>&nbsp;&nbsp;&nbsp;fills a histogram of a histogram array.</td></tr>\n");
   buffer.AppendFormated("<tr><td><b>Draw[<i>Histo Name</i>]At([<i>Index</i>])</b></td>     <td>&nbsp;&nbsp;&nbsp;draws a histogram of a histogram array.</td></tr>\n");
   buffer.AppendFormated("<tr><td><b>Get[<i>Histo Name</i>]HandleAt([<i>Index</i>])</b></td><td>&nbsp;&nbsp;&nbsp;gets the handle to a histogram of a histogram array.</td></tr>\n");
   buffer.AppendFormated("</table>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<b><i>Histo Name</i></b> stands for the name of the histogram.</br>\n");
   buffer.AppendFormated("<b><i>Index</i></b> stands for the array index of the histogram.</br>\n");
   buffer.AppendFormated("<b><i>xValue</i></b> value to be filled to the histogram.</br>\n");
   buffer.AppendFormated("<b><i>weight</i></b> weight of the value.</br>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("\n");

   // General Methods
   buffer.AppendFormated("<h3><a name=generalmethods><u>General</u></a></h3>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("%s%s provides some general methods for the user.\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("<table border=\"0\">\n");
   buffer.AppendFormated("<tr><td>isOnline()</td><td> : true if the program is running online.</td></tr>\n");
   buffer.AppendFormated("<tr><td>isOffline()</td><td> : true if the program is running offline.</td></tr>\n");
   buffer.AppendFormated("<tr><td>isRoot()</td><td> : true if the data is read from a root file.</td></tr>\n");
   buffer.AppendFormated("<tr><td>isMidas()</td><td> : true if the data has the midas format.</td></tr>\n");
   buffer.AppendFormated("<tr><td>isBatchMode()</td><td> : true if the program is running in batch mode.</td></tr>\n");
   buffer.AppendFormated("<tr><td>isEndOfRun()</td><td> : true if the EndOfRun flag is set.</td></tr>\n");
   buffer.AppendFormated("<tr><td>isBeginOfRun()</td><td> : true if the BeginOfRun flag is set.</td></tr>\n");
   buffer.AppendFormated("<tr><td>isTerminate()</td><td> : true if the Terminate flag is set.</td></tr>\n");
   buffer.AppendFormated("<tr><td>EndOfRun()</td><td> : sets the EndOfRun flag.</td></tr>\n");
   buffer.AppendFormated("<tr><td>BeginOfRun()</td><td> : sets the BeginOfRun flag.</td></tr>\n");
   buffer.AppendFormated("<tr><td>Terminate()</td><td> : sets the Terminate flag.</td></tr>\n");
   buffer.AppendFormated("<tr><td>GetConfigDir()</td><td> : returns the configuration directory.</td></tr>\n");
   buffer.AppendFormated("<tr><td>GetDataBaseDir()</td><td> : returns the data base directory.</td></tr>\n");
   buffer.AppendFormated("<tr><td>GetInputDir()</td><td> : returns the input directory.</td></tr>\n");
   buffer.AppendFormated("<tr><td>GetOutputDir()</td><td> : returns the output directory.</td></tr>\n");
   buffer.AppendFormated("<tr><td>GetCurrentRunNumber()</td><td> : returns the current run number.</td></tr>\n");
   buffer.AppendFormated("</table>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("\n");

   // Footer
   buffer.AppendFormated("<HR>\n");
   buffer.AppendFormated("<p>\n");
   buffer.AppendFormated("\n");
   buffer.AppendFormated("<ADDRESS>\n");
   buffer.AppendFormated("<u> Contact person from the %s Experiment</u></br>\n",shortCut.Data());
   buffer.AppendFormated("%s</br>\n",mainAuthor.Data());
   buffer.AppendFormated("%s</br>\n",mainInstitute.Data());
   buffer.AppendFormated("%s</br>\n",mainCollaboration.Data());
   buffer.AppendFormated("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail.Data(),mainEmail.Data());
   buffer.AppendFormated("<u> Contact person from ROME</u></br>\n");
   buffer.AppendFormated("Matthias Schneebeli (PSI)</br>\n");
   buffer.AppendFormated("email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
   buffer.AppendFormated("</ADDRESS>\n");
   buffer.AppendFormated("</BODY>\n");
   buffer.AppendFormated("</HTML>\n");

   ROMEString htmlFile;
   htmlFile.SetFormated("%s%s%s.html",outDir.Data(),shortCut.Data(),mainProgName.Data());
   int fileHandle = open(htmlFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
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
         buf->AppendFormated("%s = strtol(%s,&cstop,10)",destination,source);
      else
         buf->AppendFormated("strtol(%s,&cstop,10)",source);
   }
   else if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      if (version==0)
         buf->AppendFormated("%s = toBool(strtol(%s,&cstop,10))",destination,source);
      else
         buf->AppendFormated("toBool(strtol(%s,&cstop,10))",source);
   }
   else if (
       !strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      if (version==0)
         buf->AppendFormated("strcpy(%s,%s)",destination,source);
      else
         buf->AppendFormated("%s",source);
   }
   else if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      if (version==0)
         buf->AppendFormated("%s = strtod(%s,&cstop)",destination,source);
      else
         buf->AppendFormated("strtod(%s,&cstop)",source);
   }
   else {
      if (version==0)
         buf->AppendFormated("%s = %s",destination,source);
      else
         buf->AppendFormated("%s",source);
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
