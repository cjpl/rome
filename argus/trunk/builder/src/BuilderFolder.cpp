/********************************************************************

  BuilderFolder.cpp, Ryu Sawada

  $Log$
  Revision 1.10  2005/03/13 01:13:58  sawada
  small bug fix.

  Revision 1.9  2005/03/12 01:21:00  sawada
  Nested tab.

  Revision 1.8  2005/02/27 23:53:43  sawada
  Create placeholder of ROMEFolder at start.
  Environment variable in ROMEProjectPath.
  Bug fix of arrayed ROMEFolder.
  Bug fix of SetActive of tabs from monitor.
  Create menues of the first tab at start.

  Revision 1.7  2005/02/25 16:14:00  sawada
  bug fix and example for ROMEFolder

  Revision 1.6  2005/02/25 13:51:02  sawada
  added folderShortCut and ROMEProjectPath

  Revision 1.5  2005/02/04 22:58:46  sawada
  ROMEFolder

  Revision 1.4  2005/02/04 15:10:05  sawada
  ODB and ROMEDataBase folders.

  Revision 1.3  2005/02/03 11:44:54  sawada
  IO to MIDAS ODB

  Revision 1.2  2005/02/02 23:54:30  sawada
  link with midas library.

  Revision 1.1.1.1  2005/01/29 22:45:07  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

bool ArgusBuilder::ReadXMLFolder() {
   // read the folder definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i,j;
   ROMEString currentFolderName = "";
   int currentNumberOfFolders;   
   // count folders
   numOfFolder++;
   currentNumberOfFolders = numOfFolder;
   if (currentNumberOfFolders>=maxNumberOfFolders) {
      cout << "Maximal number of folders reached : " << maxNumberOfFolders << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   folderName[currentNumberOfFolders] = "";
   folderRomeProjPath[currentNumberOfFolders] = "./";
   folderTitle[currentNumberOfFolders] = "";
   folderArray[currentNumberOfFolders] = "1";
   folderDataBase[currentNumberOfFolders] = false;
   folderUserCode[currentNumberOfFolders] = false;
   folderVersion[currentNumberOfFolders] = "1";
   folderDescription[currentNumberOfFolders] = "";
   folderAuthor[currentNumberOfFolders] = mainAuthor;
   folderDefinedInROME[currentNumberOfFolders] = false;
   folderConnectionType[currentNumberOfFolders] = "ROMEDataBase";
   numOfFolderInclude[currentNumberOfFolders] = 0;
   numOfValue[currentNumberOfFolders] = 0;   
   // set parent
   folderParentName[currentNumberOfFolders] = parent[recursiveFolderDepth];
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subfolder
      if (type == 1 && (!strcmp((const char*)name,"Folder") ||  !strcmp((const char*)name,"ROMEFolder"))) {
         // set folder as parent for subsequent folders
         recursiveFolderDepth++;
         if (parent[recursiveFolderDepth].Length()==0)
            parent[recursiveFolderDepth] = folderName[currentNumberOfFolders].Data();
         // read subfolder
         if ((!strcmp((const char*)name,"Folder")&&!ReadXMLFolder()) || (!strcmp((const char*)name,"ROMEFolder")&&!ReadXMLROMEFolder())) 
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
         parent[recursiveFolderDepth+1] = "";
         recursiveFolderDepth--;
         return true;
      }
      // folder name
      if (type == 1 && !strcmp((const char*)name,"FolderName")) {
         xml->GetValue(folderName[currentNumberOfFolders],folderName[currentNumberOfFolders]);
         currentFolderName = folderName[currentNumberOfFolders];
         // output
         if (makeOutput) for (i=0;i<recursiveFolderDepth;i++) cout << "   ";
         if (makeOutput) folderName[currentNumberOfFolders].WriteLine();
      }
      // folder title
      if (type == 1 && !strcmp((const char*)name,"FolderTitle"))
         xml->GetValue(folderTitle[currentNumberOfFolders],folderTitle[currentNumberOfFolders]);
      // folder array size
      if (type == 1 && !strcmp((const char*)name,"ArraySize"))
         xml->GetValue(folderArray[currentNumberOfFolders],folderArray[currentNumberOfFolders]);
      // folder data base access
      if (type == 1 && !strcmp((const char*)name,"DataBaseAccess")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true") 
            folderDataBase[currentNumberOfFolders] = true;
      }
      // folder with changeble class file
      if (type == 1 && !strcmp((const char*)name,"ChangeableClassFile")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true") 
            folderUserCode[currentNumberOfFolders] = true;
      }
      // folder version
      if (type == 1 && !strcmp((const char*)name,"FolderVersion"))
         xml->GetValue(folderVersion[currentNumberOfFolders],folderVersion[currentNumberOfFolders]);
      // folder description
      if (type == 1 && !strcmp((const char*)name,"FolderDescription"))
         xml->GetValue(folderDescription[currentNumberOfFolders],folderDescription[currentNumberOfFolders]);
      // folder author
      if (type == 1 && !strcmp((const char*)name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // author name
            if (type == 1 && !strcmp((const char*)name,"AuthorName"))
               xml->GetValue(folderAuthor[currentNumberOfFolders],folderAuthor[currentNumberOfFolders]);
            if (type == 15 && !strcmp((const char*)name,"Author"))
               break;
         }
         continue;
      }
      // folder include
      if (type == 1 && !strcmp((const char*)name,"Include")) {
         // include initialisation
         folderInclude[currentNumberOfFolders][numOfFolderInclude[currentNumberOfFolders]] = "";
         folderLocalFlag[currentNumberOfFolders][numOfFolderInclude[currentNumberOfFolders]] = false;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include name
            if (type == 1 && !strcmp((const char*)name,"IncludeName"))
               xml->GetValue(folderInclude[currentNumberOfFolders][numOfFolderInclude[currentNumberOfFolders]],folderInclude[currentNumberOfFolders][numOfFolderInclude[currentNumberOfFolders]]);
            // include type
            if (type == 1 && !strcmp((const char*)name,"IncludeType")) {
               xml->GetValue(tmp,"false");
               if (tmp == "local") 
                  folderLocalFlag[currentNumberOfFolders][numOfFolderInclude[currentNumberOfFolders]] = true;
            }
            // include end
            if (type == 15 && !strcmp((const char*)name,"Include"))
               break;
         }
         // check input
         if (folderInclude[currentNumberOfFolders][numOfFolderInclude[currentNumberOfFolders]]=="") {
            cout << "An Include of Folder '" << folderName[currentNumberOfFolders].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // count includes
         numOfFolderInclude[currentNumberOfFolders]++;
         if (numOfFolderInclude[currentNumberOfFolders]>=maxNumberOfInclude) {
            cout << "Maximal number of includes in folder '" << folderName[currentNumberOfFolders].Data() << "' reached : " << maxNumberOfInclude << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
      // folder data base type
      if (type == 1 && !strcmp((const char*)name,"ConnectionType")){
         xml->GetValue(folderConnectionType[currentNumberOfFolders],folderConnectionType[currentNumberOfFolders]);
         if(!midas && !strcmp(folderConnectionType[currentNumberOfFolders],"ODB")){
            cout << "Need Midas support for ODB connection !!" << endl;
            cout << "Please link the midas library with \"-midas\"." << endl;
            return false;
         }
      }
      // folder field
      if (type == 1 && !strcmp((const char*)name,"Field")) {
         // field initialisation
         bool readName = false;
         bool readType = false;
         valueName[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "";
         valueType[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "";
         valueComment[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "";
         valueArray[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "1";
         valueDataBasePath[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // field name
            if (type == 1 && !strcmp((const char*)name,"FieldName")) {
               readName = true;
               xml->GetValue(valueName[currentNumberOfFolders][numOfValue[currentNumberOfFolders]],valueName[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]);
            }
            // field type
            if (type == 1 && !strcmp((const char*)name,"FieldType")) {
               readType = true;
               xml->GetValue(valueType[currentNumberOfFolders][numOfValue[currentNumberOfFolders]],valueType[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]);
               if (valueType[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] == "TString")
                  valueInit[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "' '";
               else if (valueType[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] == "TRef")
                  valueInit[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "NULL";
               else
                  valueInit[currentNumberOfFolders][numOfValue[currentNumberOfFolders]] = "0";
            }
            // field initialization
            if (type == 1 && !strcmp((const char*)name,"FieldInitialization")) {
               if (!readName) {
                  cout << "Please specify a field name befor the initial value in the " << (numOfValue[currentNumberOfFolders]+1) << ".field in folder '" << folderName[currentNumberOfFolders].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               if (!readType) {
                  cout << "Please specify a field type befor the initial value in field '" << valueName[currentNumberOfFolders][numOfValue[currentNumberOfFolders]].Data() << "' in folder  '" << folderName[currentNumberOfFolders].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               xml->GetValue(valueInit[currentNumberOfFolders][numOfValue[currentNumberOfFolders]],valueInit[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]);
            }
            // field comment
            if (type == 1 && !strcmp((const char*)name,"FieldComment")) {
               xml->GetValue(valueComment[currentNumberOfFolders][numOfValue[currentNumberOfFolders]],valueComment[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]);
               if (valueComment[currentNumberOfFolders][numOfValue[currentNumberOfFolders]][0]!='/') {
                  valueComment[currentNumberOfFolders][numOfValue[currentNumberOfFolders]].Insert(0,"// ");
               }
            }
            // field array size
            if (type == 1 && !strcmp((const char*)name,"ArraySize"))
               xml->GetValue(valueArray[currentNumberOfFolders][numOfValue[currentNumberOfFolders]],valueArray[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]);
            // field data base path
            if (type == 1 && !strcmp((const char*)name,"DataBasePath"))
               xml->GetValue(valueDataBasePath[currentNumberOfFolders][numOfValue[currentNumberOfFolders]],valueDataBasePath[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]);
            // field end
            if (type == 15 && !strcmp((const char*)name,"Field"))
               break;
         }
         // check input
         if (valueName[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]=="") {
            cout << "A Field of Folder '" << folderName[currentNumberOfFolders].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (valueType[currentNumberOfFolders][numOfValue[currentNumberOfFolders]]=="") {
            cout << "Field '" << valueName[currentNumberOfFolders][numOfValue[currentNumberOfFolders]].Data() << "' of Folder '" << folderName[currentNumberOfFolders].Data() << "' has no Type !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         for (i=0;i<numOfValue[currentNumberOfFolders];i++) {
            for (j=i+1;j<numOfValue[currentNumberOfFolders];j++) {
               if (valueName[currentNumberOfFolders][i]==valueName[currentNumberOfFolders][j]) {
                  cout << "\nFolder '" << folderName[currentNumberOfFolders].Data() << "' has two fields with the name '" << valueName[currentNumberOfFolders][i].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         // count fields
         numOfValue[currentNumberOfFolders]++;
         if (numOfValue[currentNumberOfFolders]>=maxNumberOfValues) {
            cout << "Maximal number of fields in folder '" << folderName[currentNumberOfFolders].Data() << "' reached : " << maxNumberOfValues << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
   }
   return true;
}

bool ArgusBuilder::ReadXMLROMEFolder() {
   romefolder = true;
   // read the ROMEFolder definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i,j;
   ROMEString currentFolderName = "";
   int currentNumberOfFolders;   
   // count folders
   numOfFolder++;
   currentNumberOfFolders = numOfFolder;
   if (currentNumberOfFolders>=maxNumberOfFolders) {
      cout << "Maximal number of folders reached : " << maxNumberOfFolders << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   folderName[currentNumberOfFolders] = "";
   folderTitle[currentNumberOfFolders] = "";
   folderArray[currentNumberOfFolders] = "1";
   folderDataBase[currentNumberOfFolders] = true;
   folderUserCode[currentNumberOfFolders] = false;
   folderVersion[currentNumberOfFolders] = "1";
   folderDescription[currentNumberOfFolders] = "";
   folderAuthor[currentNumberOfFolders] = mainAuthor;
   folderDefinedInROME[currentNumberOfFolders] = true;
   folderConnectionType[currentNumberOfFolders] = "Socket";
   numOfFolderInclude[currentNumberOfFolders] = 0;
   numOfValue[currentNumberOfFolders] = 1;//dummy
   folderRomeProjPath[currentNumberOfFolders] = "./";
   // set parent
   folderParentName[currentNumberOfFolders] = parent[recursiveFolderDepth];
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // end folder
      if (type == 15 && !strcmp((const char*)name,"ROMEFolder")) {
         // check input
         if (currentFolderName=="") {
            cout << "The " << (currentNumberOfFolders+1) << ". Folder has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         parent[recursiveFolderDepth+1] = "";
         recursiveFolderDepth--;
         return true;
      }
      // folder name
      if (type == 1 && !strcmp((const char*)name,"FolderName")) {
         xml->GetValue(folderName[currentNumberOfFolders],folderName[currentNumberOfFolders]);
         currentFolderName = folderName[currentNumberOfFolders];
         // output
         if (makeOutput) for (i=0;i<recursiveFolderDepth;i++) cout << "   ";
         if (makeOutput) folderName[currentNumberOfFolders].WriteLine();
      }
      // folder title
      if (type == 1 && !strcmp((const char*)name,"FolderTitle"))
         xml->GetValue(folderTitle[currentNumberOfFolders],folderTitle[currentNumberOfFolders]);
      // folder ROME project path for ROMEFolder
      if (type == 1 && !strcmp((const char*)name,"ROMEProjectPath"))
         xml->GetValue(folderRomeProjPath[currentNumberOfFolders],folderRomeProjPath[currentNumberOfFolders]);
      // folder array size
      if (type == 1 && !strcmp((const char*)name,"ArraySize"))
         xml->GetValue(folderArray[currentNumberOfFolders],folderArray[currentNumberOfFolders]);
      // folder description
      if (type == 1 && !strcmp((const char*)name,"FolderDescription"))
         xml->GetValue(folderDescription[currentNumberOfFolders],folderDescription[currentNumberOfFolders]);
      // folder with changeble class file
      if (type == 1 && !strcmp((const char*)name,"ChangeableClassFile")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true") 
            folderUserCode[currentNumberOfFolders] = true;
      }
      // folder author
      if (type == 1 && !strcmp((const char*)name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // author name
            if (type == 1 && !strcmp((const char*)name,"AuthorName"))
               xml->GetValue(folderAuthor[currentNumberOfFolders],folderAuthor[currentNumberOfFolders]);
            if (type == 15 && !strcmp((const char*)name,"Author"))
               break;
         }
         continue;
      }
   }
   return true;
}

bool ArgusBuilder::WriteFolderCpp() {
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
      if (numOfValue[iFold] == 0 || folderDefinedInROME[iFold]) continue;
      // File name
      cppFile.SetFormatted("%s/src/monitor/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
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
      format.SetFormatted("// %%sF%%-%d.%ds //\n",ll,ll);
      buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("//                                                                            //\n");
      pos = (char*)folderDescription[iFold].Data();
      lenTot = folderDescription[iFold].Length();
      while (pos-folderDescription[iFold].Data() < lenTot) {
         if (lenTot+(folderDescription[iFold].Data()-pos)<74) 
            i=lenTot+(folderDescription[iFold].Data()-pos);
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
      buffer.AppendFormatted("// This file has been generated by the ArgusBuilder.                           //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");
      // Write File
      struct stat buf;
      if( stat( cppFile.Data(), &buf )) {
         // Header Files
         buffer.AppendFormatted("\n\n#include \"include/monitor/%s%s.h\"\n",shortCut.Data(),folderName[iFold].Data());
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

bool ArgusBuilder::WriteFolderH() {
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   int nb,j,i;
   ROMEString str;
   int fileHandle;
   ROMEString format;
   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
     if (folderDefinedInROME[iFold] || numOfValue[iFold] == 0) continue;
      // File name
      if (folderUserCode[iFold])
         hFile.SetFormatted("%s/include/monitor/%s%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      else
         hFile.SetFormatted("%s/include/monitor/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// This file has been generated by the ArgusBuilder.                           //\n");
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
      // Includes
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
	       buffer.AppendFormatted("#include \"include/monitor/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
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
      int nameLen = 19;
      int nameLenT = 0;
      if(!folderDefinedInROME[iFold]){
         for (i=0;i<numOfValue[iFold];i++) {
            if (typeLen<valueType[iFold][i].Length()) typeLen = valueType[iFold][i].Length();
            if (valueArray[iFold][i]=="1")
               nameLenT = (int)valueName[iFold][i].Length();
            else
               nameLenT = (int)(valueName[iFold][i].Length()+2+valueArray[iFold][i].Length());
            if (nameLen<nameLenT) nameLen = nameLenT;
         }
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]!="1") {
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
      }
      // Methods
      buffer.AppendFormatted("public:\n");
      if(!folderDefinedInROME[iFold]){
         // Constructor
         if (folderUserCode[iFold])
            buffer.AppendFormatted("   %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         else
            buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="1") {
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
            if (valueArray[iFold][i]=="1")
               buffer.AppendFormatted("%s = %s_value; ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
            else {
               buffer.AppendFormatted("for (int i%d=0;i%d<%s;i%d++) %s[i%d] = %s; ",i,i,valueArray[iFold][i].Data(),i,valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
            }
         }
         buffer.AppendFormatted("fModified = false; ");
         buffer.AppendFormatted("};\n");
         buffer.AppendFormatted("\n");
      }
      else{
         // Constructor
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="1") {
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
         buffer.AppendFormatted("} : %s%s();",shortCut.Data(),folderName[iFold].Data());         
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
      }      
      // Getters
      if(!folderDefinedInROME[iFold]){
         for (i=0;i<numOfValue[iFold];i++) {
            int lb = nameLen-valueName[iFold][i].Length();
            if (valueArray[iFold][i]!="1") {
               if (valueType[iFold][i]=="TRef") {
                  format.SetFormatted("   %%-%ds  Get%%sAt(int index)%%%ds { return &%%s[index];%%%ds };\n",typeLen,lb,lb);
                  buffer.AppendFormatted((char*)format.Data(),"TRef*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
               else {
                  format.SetFormatted("   %%-%ds  Get%%sAt(int index)%%%ds { return %%s[index];%%%ds };\n",typeLen,lb,lb);
                  buffer.AppendFormatted((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
                  format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return &%%s[0];%%%ds };\n",typeLen,lb,lb);
                  buffer.AppendFormatted((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
            }
            else {
               if (valueType[iFold][i]=="TRef") {
                  format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return &%%s;%%%ds };\n",typeLen,lb,lb);
                  buffer.AppendFormatted((char*)format.Data(),"TRef*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
               else {
                  format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
                  buffer.AppendFormatted((char*)format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
            }
         }       
         // isDefinedInROME
         format.SetFormatted("   %%-%ds  is%%s()%%%ds  { return %%s;%%%ds };\n",typeLen,nameLen-8,nameLen-8);
         buffer.AppendFormatted((char*)format.Data(),"Bool_t","DefinedInROME","","false","");
         buffer.AppendFormatted("\n");
         // isModified
         format.SetFormatted("   %%-%ds  is%%s()%%%ds  { return f%%s;%%%ds };\n",typeLen,nameLen-8,nameLen-8);
         buffer.AppendFormatted((char*)format.Data(),"Bool_t","Modified","","Modified","");
         buffer.AppendFormatted("\n");
         // Setters
         for (i=0;i<numOfValue[iFold];i++) {
            int lb = nameLen-valueName[iFold][i].Length();
            if (valueArray[iFold][i]=="1") {
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
            if (valueArray[iFold][i]=="1") {
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
            if (valueArray[iFold][i]=="1") {
               buffer.AppendFormatted("%s = %s_value; ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
            }
         }
         buffer.AppendFormatted("fModified = true; ");
         buffer.AppendFormatted("};\n");
         buffer.AppendFormatted("\n");
         // Reset
         buffer.AppendFormatted("   void Reset() {");
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="1") {
               buffer.AppendFormatted("%s = %s; ",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
            }
            else {
               buffer.AppendFormatted("for (int i%d=0;i%d<%s;i%d++) %s[i%d] = %s; ",i,i,valueArray[iFold][i].Data(),i,valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
            }
         }
         buffer.AppendFormatted("fModified = false; ");
         buffer.AppendFormatted("};\n");
      }
      else{
         format.SetFormatted("   %%-%ds  is%%s()%%%ds  { return %%s;%%%ds };\n",typeLen,nameLen-8,nameLen-8);
         buffer.AppendFormatted((char*)format.Data(),"Bool_t","DefinedInROME","","true","");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
      }
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
      hFile.SetFormatted("%s/include/monitor/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      if (folderUserCode[iFold] && stat( hFile.Data(), &buf )) {
         // Description
         buffer.Resize(0);
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// Editable class file for %s folder.\n",folderName[iFold].Data());
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");
         // Header
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#include \"include/monitor/%s%s_Base.h\"\n",shortCut.Data(),folderName[iFold].Data());         
         // Class
         buffer.AppendFormatted("\nclass %s%s : public %s%s_Base\n",shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="1")
               buffer.AppendFormatted("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) : %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="1")
               buffer.AppendFormatted("%s,",valueName[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) {}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
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
   }
   return true;
}
