/********************************************************************

  BuilderFolder.cpp, Ryu Sawada

  $Log$
  Revision 1.1  2005/01/29 22:45:07  sawada
  Initial revision


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
   folderDefinedInROME[numOfFolder] = false;
   folderConnectionType[numOfFolder] = "ROME";
   numOfFolderInclude[numOfFolder] = 0;
   numOfValue[numOfFolder] = 0;   
   // set parent
   folderParentName[numOfFolder] = parent[recursiveFolderDepth];
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subfolder
      if (type == 1 && !strcmp((const char*)name,"Folder")) {
         // set folder as parent for subsequent folders
         recursiveFolderDepth++;
         if (parent[recursiveFolderDepth].Length()==0)
            parent[recursiveFolderDepth] = folderName[numOfFolder].Data();
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
         parent[recursiveFolderDepth+1] = "";
         recursiveFolderDepth--;
         return true;
      }
      // folder name
      if (type == 1 && !strcmp((const char*)name,"FolderName")) {
         xml->GetValue(folderName[numOfFolder],folderName[numOfFolder]);
         currentFolderName = folderName[numOfFolder];
         // output
         if (makeOutput) for (i=0;i<recursiveFolderDepth;i++) cout << "   ";
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
            cout << "Maximal number of includes in folder '" << folderName[numOfFolder].Data() << "' reached : " << maxNumberOfInclude << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
      // folder is defined in ROME
      if (type == 1 && !strcmp((const char*)name,"DefinedInROME")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true") 
            folderDefinedInROME[numOfFolder] = true;
      }
      // folder data base type
      if (type == 1 && !strcmp((const char*)name,"ConnectionType"))
         xml->GetValue(folderConnectionType[numOfFolder],folderConnectionType[numOfFolder]);
      // folder field
      if (type == 1 && !strcmp((const char*)name,"Field")) {
         // field initialisation
         bool readName = false;
         bool readType = false;
         valueName[numOfFolder][numOfValue[numOfFolder]] = "";
         valueType[numOfFolder][numOfValue[numOfFolder]] = "";
         valueComment[numOfFolder][numOfValue[numOfFolder]] = "";
         valueArray[numOfFolder][numOfValue[numOfFolder]] = "1";
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
               xml->GetValue(valueInit[numOfFolder][numOfValue[numOfFolder]],valueInit[numOfFolder][numOfValue[numOfFolder]]);
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
      if (numOfValue[iFold] == 0) continue;
      // File name
      cppFile.SetFormatted("%s/src/monitor/%sF%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
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
         buffer.AppendFormatted("\n\n#include \"include/monitor/%sF%s.h\"\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("\nClassImp(%sF%s)\n",shortCut.Data(),folderName[iFold].Data());
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
     if (!folderDefinedInROME[iFold] && numOfValue[iFold] == 0) continue;
      // File name
      if (folderUserCode[iFold])
         hFile.SetFormatted("%s/include/monitor/%sF%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      else
         hFile.SetFormatted("%s/include/monitor/%sF%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
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
         buffer.AppendFormatted("#ifndef %sF%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %sF%s_Base_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }
      else {
         buffer.AppendFormatted("#ifndef %sF%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %sF%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }
      if (folderDefinedInROME[iFold]){
/*
         buffer.AppendFormatted("#include \"%sF%s.h\"\n",shortCut.Data(),folderName[iFold].Data());
*/
      }
      else{
         buffer.AppendFormatted("#include <TObject.h>\n");
      }
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
               buffer.AppendFormatted("#include \"include/monitor/%sF%s.h\"\n",shortCut.Data(),folderName[j].Data());
               valueType[iFold][i].Insert(0,shortCut);
               break;
            }
         }
      }
      // Class
      if (folderDefinedInROME[iFold]){
/*
         buffer.AppendFormatted("\nclass %sF%s : public %s%s\n",shortCut.Data(),folderName[iFold].Data()
                                ,shortCut.Data(),folderName[iFold].Data());
*/
      }
      else{
         if (folderUserCode[iFold])
            buffer.AppendFormatted("\nclass %sF%s_Base : public TObject\n",shortCut.Data(),folderName[iFold].Data());
         else
            buffer.AppendFormatted("\nclass %sF%s : public TObject\n",shortCut.Data(),folderName[iFold].Data());
      }
      buffer.AppendFormatted("{\n");
      // Fields
      buffer.AppendFormatted("protected:\n");
      int typeLen = 5;
      int nameLen = 8;
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
            buffer.AppendFormatted("   %sF%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         else
            buffer.AppendFormatted("   %sF%s( ",shortCut.Data(),folderName[iFold].Data());
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
         buffer.AppendFormatted("   %sF%s( ",shortCut.Data(),folderName[iFold].Data());
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
         buffer.AppendFormatted("\n   ClassDef(%sF%s_Base,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
      else
         buffer.AppendFormatted("\n   ClassDef(%sF%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
      buffer.AppendFormatted("};\n\n");      
      if (folderUserCode[iFold])
         buffer.AppendFormatted("#endif   // %sF%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("#endif   // %sF%s_H\n",shortCut.Data(),folderName[iFold].Data());      
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
      hFile.SetFormatted("%s/include/monitor/%sF%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      if (folderUserCode[iFold] && stat( hFile.Data(), &buf )) {
         // Description
         buffer.Resize(0);
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// Editable class file for %s folder.\n",folderName[iFold].Data());
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");
         // Header
         buffer.AppendFormatted("#ifndef %sF%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %sF%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#include \"include/monitor/%sF%s_Base.h\"\n",shortCut.Data(),folderName[iFold].Data());         
         // Class
         buffer.AppendFormatted("\nclass %sF%s : public %sF%s_Base\n",shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %sF%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="1")
               buffer.AppendFormatted("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) : %sF%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueArray[iFold][i]=="1")
               buffer.AppendFormatted("%s,",valueName[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) {}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n   ClassDef(%sF%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %sF%s_H\n",shortCut.Data(),folderName[iFold].Data());
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
