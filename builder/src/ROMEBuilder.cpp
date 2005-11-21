/********************************************************************

  ROMEBuilder.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   include <direct.h>
#endif
#if defined( R__UNIX )
#   include <unistd.h>
#   include <string.h>
#   include <ctype.h>
#endif
#ifndef R__MACOSX
#   if defined( R__VISUAL_CPLUSPLUS )
#      include <io.h>
#   endif
#   if defined( R__UNIX )
#      include <sys/io.h>
#   endif
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <float.h>
#include <TMath.h>
#include "ROMEString.h"
#include "ROMEBuilder.h"
#include <Riostream.h>

bool ROMEBuilder::ReadXMLFolder() {
   // read the folder definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i,j;
   int iDm;
   int istart,iend;
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
   folderNoReset[numOfFolder] = false;

   // set parent
   folderParentName[numOfFolder] = parent[recursiveDepth];
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subfolder
      if (type == 1 && !strcmp((const char*)name,"Folder")) {
         // set folder as parent for subsequent folders
         recursiveDepth++;
         folderSupport[numOfFolder+1] = false;
         if (parent[recursiveDepth].Length()==0)
            parent[recursiveDepth] = folderName[numOfFolder].Data();
         // read subfolder
         if (!ReadXMLFolder())
            return false;
         continue;
      }
      // end folder
      if (type == 15 && (!strcmp((const char*)name,"Folder") || !strcmp((const char*)name,"SupportFolder"))) {
         // check input
         if (currentFolderName=="") {
            cout << "The " << (currentNumberOfFolders+1) << ". Folder has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         parent[recursiveDepth+1] = "";
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
         if (folderSupport[numOfFolder]) {
            cout << "Support folders can not have database access!!" << endl;
            continue;
         }
         xml->GetValue(tmp,"false");
         if (tmp == "true") {
            folderDataBase[numOfFolder] = true;
            folderNoReset[numOfFolder] = true;
         }
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
      // folder no reset by framework
      if (type == 1 && !strcmp((const char*)name,"NoResetByFramework")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true")
            folderNoReset[numOfFolder] = true;
      }
      // folder field
      if (type == 1 && !strcmp((const char*)name,"Field")) {
         // field initialisation
         bool readName = false;
         bool readType = false;
         valueName[numOfFolder][numOfValue[numOfFolder]] = "";
         valueType[numOfFolder][numOfValue[numOfFolder]] = "";
         valueComment[numOfFolder][numOfValue[numOfFolder]] = "";
         valueDimension[numOfFolder][numOfValue[numOfFolder]] = 0;
         for(iDm=0;iDm<maxNumberOfValueDimension;iDm++)
            valueArray[numOfFolder][numOfValue[numOfFolder]][iDm] = "1";
         valueDBName[numOfFolder][numOfValue[numOfFolder]] = "";
         valueDBPath[numOfFolder][numOfValue[numOfFolder]] = "";
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
               else if (isTArrayType(valueType[numOfFolder][numOfValue[numOfFolder]])) {
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "0";
                  valueDimension[numOfFolder][numOfValue[numOfFolder]] = 1;
                  valueArray[numOfFolder][numOfValue[numOfFolder]][0] = "0";
               }
               else if (isPointerType(valueType[numOfFolder][numOfValue[numOfFolder]])) {
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "NULL";
               }
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
            if (type == 1 && !strcmp((const char*)name,"ArraySize")){
               tmp = "1";
               xml->GetValue(tmp,tmp);
               istart = iend = 0;
               valueDimension[numOfFolder][numOfValue[numOfFolder]]++;
               while((iend=tmp.Index(":",1,istart,TString::kExact))!=-1){
                  valueDimension[numOfFolder][numOfValue[numOfFolder]]++;
                  if(valueDimension[numOfFolder][numOfValue[numOfFolder]] > maxNumberOfValueDimension){
                     cout << "Error: dimension of "<<valueName[numOfFolder][numOfValue[numOfFolder]]<<" is larger than "<<maxNumberOfValueDimension<<" in "<<folderName[numOfFolder]<<"."<<endl;
                     cout << "Terminating program." << endl;
                     return false;

                  }
                  valueArray[numOfFolder][numOfValue[numOfFolder]][valueDimension[numOfFolder][numOfValue[numOfFolder]]-2] = tmp(istart,iend-istart);
                  istart = iend+1;
               }
               valueArray[numOfFolder][numOfValue[numOfFolder]][valueDimension[numOfFolder][numOfValue[numOfFolder]]-1] = tmp(istart,tmp.Length()-istart);
            }
            // database name
            if (type == 1 && !strcmp((const char*)name,"DataBaseName"))
               xml->GetValue(valueDBName[numOfFolder][numOfValue[numOfFolder]],valueDBName[numOfFolder][numOfValue[numOfFolder]]);
            // database path
            if (type == 1 && !strcmp((const char*)name,"DataBasePath"))
               xml->GetValue(valueDBPath[numOfFolder][numOfValue[numOfFolder]],valueDBPath[numOfFolder][numOfValue[numOfFolder]]);
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
         if (valueDimension[numOfFolder][numOfValue[numOfFolder]]>1 &&
             (valueDBName[numOfFolder][numOfValue[numOfFolder]].Length()
              || valueDBPath[numOfFolder][numOfValue[numOfFolder]].Length())
            ) {
            cout << "Multiple dimension field '" << valueName[numOfFolder][numOfValue[numOfFolder]] << "' can not have database connection"<<endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (valueDimension[numOfFolder][numOfValue[numOfFolder]]>1){
            for(iDm=1;iDm<3;iDm++){
               if(valueArray[numOfFolder][numOfValue[numOfFolder]][iDm]=="variable") {
                  cout << "Multiple dimension field '" << valueName[numOfFolder][numOfValue[numOfFolder]] << "' can not have variable length."<<endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         if (valueArray[numOfFolder][numOfValue[numOfFolder]][0]=="variable"
             && ( valueDBName[numOfFolder][numOfValue[numOfFolder]].Length()
                  || valueDBPath[numOfFolder][numOfValue[numOfFolder]].Length())) {
            cout << "Variable length array field '" << valueName[numOfFolder][numOfValue[numOfFolder]] << "' can not have database connection"<<endl;
            cout << "Terminating program." << endl;
            return false;
         }
         tmp = valueComment[numOfFolder][numOfValue[numOfFolder]];
         tmp.ReplaceAll("//","");
         tmp.ReplaceAll(" ","");
         tmp.ReplaceAll("\t","");
         if ((isNumber(valueType[numOfFolder][numOfValue[numOfFolder]].Data()) || isBoolType(valueType[numOfFolder][numOfValue[numOfFolder]].Data())) && valueArray[numOfFolder][numOfValue[numOfFolder]][0]=="variable" && tmp[0]!='!'){
            for (i=0;i<numOfValue[numOfFolder];i++) {
               tmp.SetFormatted("[%s]",valueName[numOfFolder][i].Data());
               if (valueComment[numOfFolder][numOfValue[numOfFolder]].Contains(tmp) && isIntType(valueType[numOfFolder][i].Data())) {
                  break;
               }
            }
            if(i==numOfValue[numOfFolder]){
               cout << "\nError: Variable length array member '"<< valueName[numOfFolder][numOfValue[numOfFolder]].Data()<<"' should have '!' or '[length]' at the top of comment."<<endl;
               cout << "       If '!', field will not be stored in tree."<<endl;
               cout << "       If '[length]', field will be stored in tree with the length. 'length' needs to be an integer field name of the folder. It must be defined ahead of its use."<<endl;
               cout << "Terminating program." << endl;
               return false;
            }
         }
         if (isTArrayType(valueType[numOfFolder][numOfValue[numOfFolder]])) {
            if(valueDimension[numOfFolder][numOfValue[numOfFolder]]>2) {
               cout << "Array of TArray is not supported. ( '" << valueName[numOfFolder][numOfValue[numOfFolder]] << "' )"<<endl;
               cout << "Terminating program." << endl;
               return false;
            }
            if(valueDBName[numOfFolder][numOfValue[numOfFolder]].Length()
               || valueDBPath[numOfFolder][numOfValue[numOfFolder]].Length()) {
               cout << "TArray type field '" << valueName[numOfFolder][numOfValue[numOfFolder]] << "' can not have database connection"<<endl;
               cout << "Terminating program." << endl;
               return false;
            }
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
   ROMEString header;
   ROMEString buffer;

   int ll,i,lenTot;
   char *pos;
   ROMEString format;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (numOfValue[iFold] == 0) continue;

      header.Resize(0);
      buffer.Resize(0);

      // File name
      cppFile.SetFormatted("%s/src/framework/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());

      if (!folderUserCode[iFold]) {
         remove(cppFile.Data());
         continue;
      }

      // Description
      header.AppendFormatted("//// Author: %s\n",folderAuthor[iFold].Data());
      header.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      header.AppendFormatted("//                                                                            //\n");
      ll = 74-shortCut.Length();
      format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
      header.AppendFormatted(format.Data(),shortCut.Data(),folderName[iFold].Data());
      header.AppendFormatted("//                                                                            //\n");
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
         header.AppendFormatted("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      header.AppendFormatted("//                                                                            //\n");
      header.AppendFormatted("//                                                                            //\n");
      header.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
      header.AppendFormatted("//                                                                            //\n");
      header.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");

      buffer.AppendFormatted("\n\n#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("\nClassImp(%s%s)\n",shortCut.Data(),folderName[iFold].Data());

      // Write File
      ReplaceHeader(cppFile.Data(),header.Data(),buffer.Data(),6);
   }
   return true;
}

bool ROMEBuilder::WriteFolderH() {
   ROMEString hFile;
   ROMEString buffer;

   int j,i;
   ROMEString str;
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
      buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
      buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
      buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

      // Header
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("#ifndef %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_Base_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }
      else {
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }

      // Includes
      buffer.AppendFormatted("#include <RConfig.h>\n");
      buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4244 )\n");
      buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
      buffer.AppendFormatted("#include <TObject.h>\n");
      buffer.AppendFormatted("#include <TClass.h>\n");
      buffer.AppendFormatted("#include <TClonesArray.h>\n");
      for (i=0;i<numOfFolderInclude[iFold];i++) {
         if (folderLocalFlag[iFold][i]) {
            buffer.AppendFormatted("#include \"%s\"\n",folderInclude[iFold][i].Data());
         }
         else {
            buffer.AppendFormatted("#include <%s>\n",folderInclude[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
      buffer.AppendFormatted("#pragma warning( pop )\n");
      buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i]=="TString") {
            buffer.AppendFormatted("#include <%s.h>\n",valueType[iFold][i].Data());
            break;
         }
         if (isTArrayType(valueType[iFold][i])) {
            buffer.AppendFormatted("#include <%s.h>\n",valueType[iFold][i].Data());
            valueType[iFold][i] += "*";
            break;
         }
      }

      for (i=0;i<numOfValue[iFold];i++) {
         for (j=0;j<numOfFolder;j++) {
            str.SetFormatted("%s*",folderName[j].Data());
            if (valueType[iFold][i]==folderName[j] || valueType[iFold][i]==str) {
               buffer.AppendFormatted("#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[j].Data());
               if (valueType[iFold][i]==folderName[j])
                  valueType[iFold][i] += "*";
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
      int typeLen = 13;
      int nameLen = 8;
      int nameLenT = 0;
      int arrayLen = 0;
      int iDm;
      for (i=0;i<numOfValue[iFold];i++) {
         if (typeLen<valueType[iFold][i].Length()) typeLen = valueType[iFold][i].Length();
         nameLenT = (int)valueName[iFold][i].Length();
         for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
            nameLenT += (int)(2+valueArray[iFold][i][iDm].Length());
         }
         if (nameLen<nameLenT) nameLen = nameLenT;
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i]>0) {
            if(valueDimension[iFold][i]>1){
               cout<<"You can not have multiple dimensional array of support folders."<<endl;
               cout << "Terminating program." << endl;
               return false;
            }
            format.SetFormatted("   TClonesArray*%%%ds %%s;%%%ds %%s\n",typeLen-13,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
         }
         else if(valueArray[iFold][i][0]=="variable"){
            format.SetFormatted("   %%-%ds* %%s;%%%ds %%s\n",typeLen-1,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
            format.SetFormatted("   %%-%ds %%sSize;%%%ds // ! number of elements of %%s\n",typeLen,nameLen-valueName[iFold][i].Length()-4);
            buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
            format.SetFormatted("   %%-%ds %%sActualSize;%%%ds // ! actual size of %%s allocated in memory\n",typeLen,nameLen-valueName[iFold][i].Length()-10);
            buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
         }
         else {
            format.SetFormatted("   %%-%ds %%s",typeLen);
            buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
            arrayLen = 0;
            if(!isTArrayType(valueType[iFold][i])){
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
                  buffer.AppendFormatted("[%s]",valueArray[iFold][i][iDm].Data());
                  arrayLen += 2+valueArray[iFold][i][iDm].Length();
               }
            }
            format.SetFormatted(";%%%ds %%s\n",nameLen-valueName[iFold][i].Length()-arrayLen);
            buffer.AppendFormatted(format.Data(),"",valueComment[iFold][i].Data());
         }
      }
      format.SetFormatted("   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-9);
      buffer.AppendFormatted(format.Data(),"Bool_t","Modified","","//! Modified Folder Flag");
      buffer.AppendFormatted("\n");

      // Methods

      buffer.AppendFormatted("public:\n");
      // Constructor
      if (folderUserCode[iFold])
         buffer.AppendFormatted("   %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueDimension[iFold][i]==0) {
            if (isFolder(valueType[iFold][i].Data()))
               continue;
            if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
               continue;
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
      buffer.AppendFormatted("   {\n");
      if (folderUserCode[iFold])
         buffer.AppendFormatted("      %s%s_Base::Class()->IgnoreTObjectStreamer();\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("      %s%s::Class()->IgnoreTObjectStreamer();\n",shortCut.Data(),folderName[iFold].Data());
      ROMEString tmp;
      for (i=0;i<numOfValue[iFold];i++) {
         if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         else if (isFolder(valueType[iFold][i].Data())){
            tmp = valueType[iFold][i];
            tmp.ReplaceAll("*","");
            if (valueDimension[iFold][i]==0){
               buffer.AppendFormatted("      %s = new %s();\n",valueName[iFold][i].Data(),tmp.Data());
            }
            else {
               buffer.AppendFormatted("      %s = new TClonesArray(\"%s\");\n",valueName[iFold][i].Data(),tmp.Data());
               if (valueArray[iFold][i][0]!="variable")
                  buffer.AppendFormatted("      Set%sSize(%s);\n",valueName[iFold][i].Data(),valueArray[iFold][i][0].Data());
            }
         }
         else if(isTArrayType(valueType[iFold][i])) {
            tmp = valueType[iFold][i];
            tmp.ReplaceAll("*","");
            buffer.AppendFormatted("      %s = new %s(%s);\n",valueName[iFold][i].Data(),tmp.Data(),valueArray[iFold][i][0].Data());
            buffer.AppendFormatted("      %s->Reset(%s);\n",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         else {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("      %s = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
            else if(valueArray[iFold][i][0]=="variable"){
               buffer.AppendFormatted("      %s = NULL;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %sActualSize = 0;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %sSize = 0;\n",valueName[iFold][i].Data());
            }
            else{
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
                  format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++){\n",iDm*3);
                  buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
               }
               format.SetFormatted("%%%ds      %%s",valueDimension[iFold][i]*3);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
               buffer.AppendFormatted(" = %s;\n",valueInit[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
                  format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
                  buffer.AppendFormatted(format.Data(),"");
               }
            }
         }
      }
      buffer.AppendFormatted("      fModified = false;\n");
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("\n");

      // Destructor
      if (folderUserCode[iFold])
         buffer.AppendFormatted("   virtual ~%s%s_Base()\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("   virtual ~%s%s()\n",shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("   {\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            buffer.AppendFormatted("      delete %s;\n",valueName[iFold][i].Data());
         else if(isTArrayType(valueType[iFold][i])) 
            buffer.AppendFormatted("      delete %s;\n",valueName[iFold][i].Data());
         else if(valueDimension[iFold][i]!=0 && valueArray[iFold][i][0]=="variable")
            buffer.AppendFormatted("      delete [] %s;\n",valueName[iFold][i].Data());
      }
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("\n");

      // Getters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i]>0) {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   %%-%ds  Get%%sAt(",typeLen);
               buffer.AppendFormatted(format.Data(),"TRef*",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("int %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               format.SetFormatted("%%%ds { return &%%s",lb);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               format.SetFormatted(";%%%ds };\n",lb);
               buffer.AppendFormatted(format.Data(),"");
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               format.SetFormatted("   %%-%ds  Get%%sAt(int index)%%%ds { return (%%-%ds)(%%s->At(index));%%%ds };\n",typeLen,lb,typeLen,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"TClonesArray*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%s->GetEntries();%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if(isTArrayType(valueType[iFold][i])) {
               format.SetFormatted("   %%-%ds  Get%%sAt(int index)%%%ds { return %%s->At(index);%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),TArray2StandardType(valueType[iFold][i]),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%s->GetSize();%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* array)%%%ds {\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i]),"");
               buffer.AppendFormatted("      if(!%s->GetSize() || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if(!array) array = new %s[n];\n",TArray2StandardType(valueType[iFold][i]));
               buffer.AppendFormatted("      memcpy(array,%s->GetArray(),n*sizeof(%s));\n",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i]));
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")){
               format.SetFormatted("   %%-%ds* Get%%sAt(",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("int %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               format.SetFormatted(")%%%ds { return &%%s",lb);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               format.SetFormatted(";%%%ds };\n",lb);
               buffer.AppendFormatted(format.Data(),"");
            }
            else if(isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")){
               format.SetFormatted("   %%-%ds  Get%%sAt(int index)%%%ds { return %%s[index];%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if(valueArray[iFold][i][0]=="variable"){
               format.SetFormatted("   %%-%ds  Get%%sAt(int index)%%%ds { return %%s[index];%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%sSize;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* array)%%%ds {\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),valueType[iFold][i].Data(),"");
               buffer.AppendFormatted("      if(!%s || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if(!array) array = new %s[n];\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("      memcpy(array,%s,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else {
               format.SetFormatted("   %%-%ds  Get%%sAt(",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("int %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               format.SetFormatted(")%%%ds { return %%s",lb);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               format.SetFormatted(";%%%ds };\n",lb);
               buffer.AppendFormatted(format.Data(),"");
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return &%%s",typeLen,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[0]");
               format.SetFormatted(";%%%ds };\n",lb);
               buffer.AppendFormatted(format.Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* array)%%%ds {\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),valueType[iFold][i].Data(),"");
               buffer.AppendFormatted("      if(!%s || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if(!array) array = new %s[n];\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("      memcpy(array,&%s",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[0]");
               buffer.AppendFormatted(",n*sizeof(%s));\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
         }
         else {
            if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")){
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return &%%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if(isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")){
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return &%%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"TRef*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // isModified
      format.SetFormatted("   %%-%ds  is%%s()%%%ds  {\n",typeLen,nameLen-8);
      buffer.AppendFormatted(format.Data(),"Bool_t","Modified","");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            buffer.AppendFormatted("      int i;\n");
            break;
         }
      }
      buffer.AppendFormatted("      if( fModified ) return true;\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i]==0) {
               buffer.AppendFormatted("      if( %s->isModified() ) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("         SetModified(true);\n");
               buffer.AppendFormatted("         return true;\n");
               buffer.AppendFormatted("      }\n");
            }
            else {
               // check only the first element for speed up.
               buffer.AppendFormatted("      if( %s->GetEntries() ) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("         if( ((%s)%s->At(0))->isModified() ) {\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("            SetModified(true);\n");
               buffer.AppendFormatted("            return true;\n");
               buffer.AppendFormatted("         }\n");
               buffer.AppendFormatted("      }\n");
            }
         }
      }
      // check remaining elements
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i]>0) {
               buffer.AppendFormatted("      for(i=1;i<%s->GetEntries();i++) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("         if( ((%s)%s->At(i))->isModified() ) {\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("            SetModified(true);\n");
               buffer.AppendFormatted("            return true;\n");
               buffer.AppendFormatted("         }\n");
               buffer.AppendFormatted("      }\n");
            }
         }
      }
      buffer.AppendFormatted("      return false;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      // Setters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Set%%sAt%%%ds(",lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"");
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("int %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value%%%ds) { %%s",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               format.SetFormatted("%%%ds = %%s_value;%%%ds SetModified(true); };\n",lb,lb);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               if(valueArray[iFold][i][0]!="variable")
                  buffer.AppendFormatted("private:\n");
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted("   void Set%sSize(int number) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      int i;\n");
               buffer.AppendFormatted("      if(%s) %s->Delete();\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("      for(i=0;i<number;i++) {\n");
               buffer.AppendFormatted("         new((*%s)[i]) %s;\n",valueName[iFold][i].Data(),tmp.Data());
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }\n");
               if(valueArray[iFold][i][0]!="variable")
                  buffer.AppendFormatted("public:\n");
            }
            else if(isTArrayType(valueType[iFold][i])) {
               format.SetFormatted("   void Set%%sAt%%%ds(int index,%%-%ds %%s_value%%%ds) { %%s->AddAt(%%s_value,index)%%%ds;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",TArray2StandardType(valueType[iFold][i]),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),valueName[iFold][i].Data(),"","");
               buffer.AppendFormatted("   void Set%sSize(int number) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s->Set(number);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }\n");
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array)%%%ds {\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i]),"");
               buffer.AppendFormatted("      if(!array || !n) return;\n");
               buffer.AppendFormatted("      if(!%s || %s->GetSize()<n) Set%sSize(n);\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s->Set(n,array);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
               continue;
            else if(isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")){
               format.SetFormatted("   void Set%%sAt%%%ds(int index,%%-%ds %%s_value%%%ds) { %%s[index] = %%s_value%%%ds;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),valueName[iFold][i].Data(),"","");
            }
            else if(valueArray[iFold][i][0]=="variable") {
               format.SetFormatted("   void Set%%sAt%%%ds(int index,%%-%ds %%s_value%%%ds) { %%s[index] = %%s_value%%%ds;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),valueName[iFold][i].Data(),"","");
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds* %%s_value%%%ds) { %%s = %%s_value%%%ds;%%%ds SetModified(true); };\n",lb,typeLen-1,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),valueName[iFold][i].Data(),"","");
               buffer.AppendFormatted("   void Set%sSize(int number) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if(number==%sSize) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if(number<0) return;\n");
               buffer.AppendFormatted("      if(number>%sActualSize){\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("         %s *tmp = new %s[number];\n",valueType[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("         memcpy(tmp,%s,%sSize*sizeof(%s));\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("         delete [] %s;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("         %s = tmp;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("         %sActualSize = number;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("      %sSize = number;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }\n");
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array)%%%ds {\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),valueType[iFold][i].Data(),"");
               buffer.AppendFormatted("      if(!array || n<=0) return;\n");
               buffer.AppendFormatted("      if(%sActualSize<n) Set%sSize(n);\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("      memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else {
               format.SetFormatted("   void Set%%sAt%%%ds(",lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"");
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("int %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value%%%ds) { %%s",typeLen,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               format.SetFormatted("%%%ds = %%s_value;%%%ds SetModified(true); };\n",lb,lb);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array)%%%ds {\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i]),"");
               buffer.AppendFormatted("      if(!array || !n) return;\n");
               buffer.AppendFormatted("      memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      buffer.AppendFormatted("\n");
      // Add
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if (isPointerType(valueType[iFold][i].Data()))
            continue;
         if(isRootClassType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Add%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds += %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               if (!isBoolType(valueType[iFold][i].Data())) {
                  format.SetFormatted("   void Add%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds += %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
                  buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Add%%sAt%%%ds(",lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"");
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("int %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value%%%ds) { %%s",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               format.SetFormatted("%%%ds += %%s_value;%%%ds SetModified(true); };\n",lb,lb);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data(),"");
            }
            else if(isTArrayType(valueType[iFold][i])) {
               format.SetFormatted("   void Add%%sAt%%%ds(int index,%%-%ds %%s_value%%%ds) {\n",lb,typeLen,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",TArray2StandardType(valueType[iFold][i]),valueName[iFold][i].Data(),"");
               buffer.AppendFormatted("      %s tmp = %s->At(index);\n",TArray2StandardType(valueType[iFold][i]),valueName[iFold][i].Data());
               buffer.AppendFormatted("      tmp += %s_value;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s->AddAt(tmp,index);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   };\n");
            }
            else if(!isBoolType(valueType[iFold][i].Data())) {
               format.SetFormatted("   void Add%%sAt%%%ds(",lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"");
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("int %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value%%%ds) {\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"");
/*
               if(valueArray[iFold][i][0]!="variable"){
                  buffer.AppendFormatted("      if(%c>%sSize) return;\n",valueCounter[0],valueName[iFold][i].Data());
               }
*/
               buffer.AppendFormatted("      %s",valueName[iFold][i].Data());
               for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               format.SetFormatted("%%%ds += %%s_value;\n",lb);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // SetModified
      int lb = nameLen-8;
      format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s%%%ds) { f%%s%%%ds = %%s;%%%ds};\n",lb,typeLen,lb,lb,lb);
      buffer.AppendFormatted(format.Data(),"Modified","","Bool_t","modified","","Modified","","modified","");
      buffer.AppendFormatted("\n");

      // ResetModified
      buffer.AppendFormatted("   void ResetModified() {\n");
      buffer.AppendFormatted("      int i=0;\n");
      buffer.AppendFormatted("      if(isModified()){\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (!isFolder(valueType[iFold][i].Data()))
            continue;
         if (valueDimension[iFold][i]==0)
            buffer.AppendFormatted("         %s->ResetModified();\n",valueName[iFold][i].Data());
         else
            buffer.AppendFormatted("         for(i=0;i<%s->GetEntries();i++) { ((%s)%s->At(i))->ResetModified(); }\n",valueName[iFold][i].Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
      }
      buffer.AppendFormatted("         SetModified(false);\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("\n");

      // Set All
      buffer.AppendFormatted("   void SetAll( ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         if (valueDimension[iFold][i]==0) {
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
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         if (valueDimension[iFold][i]==0) {
            buffer.AppendFormatted("%s = %s_value; ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("SetModified(true); ");
      buffer.AppendFormatted("};\n");
      buffer.AppendFormatted("\n");
      // Reset
      buffer.AppendFormatted("   void Reset() {\n");
      buffer.AppendFormatted("      if( !isModified() ) return;\n");
      buffer.AppendFormatted("      int i=0;\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if(isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")){
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
               format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++){\n",iDm*3);
               buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
            }
            format.SetFormatted("%%%ds      %%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted(".Clear();\n");
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
               format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
               buffer.AppendFormatted(format.Data(),"");
            }
         }
         else if(isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                 && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")){
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
               format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++){\n",iDm*3);
               buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
            }
            format.SetFormatted("%%%ds      if(%%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted(")\n");
            format.SetFormatted("%%%ds         %%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted("->Clear();\n",valueInit[iFold][i].Data());
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
               format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
               buffer.AppendFormatted(format.Data(),"");
            }
         }
         else if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("      %s->Reset();\n",valueName[iFold][i].Data());
            else
               buffer.AppendFormatted("      for(i=0;i<%s->GetEntries();i++) { ((%s)%s->At(i))->Reset(); }\n",valueName[iFold][i].Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
         }
         else if(isTArrayType(valueType[iFold][i])) {
            buffer.AppendFormatted("      %s->Reset(%s);\n",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         else if(valueArray[iFold][i][0]=="variable") {
            buffer.AppendFormatted("      for (int i%d=0;i%d<%sSize;i%d++){\n",i,i,valueName[iFold][i].Data(),i);
            buffer.AppendFormatted("         %s[i%d] = %s;\n",valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
            buffer.AppendFormatted("      }\n");
         }
         else {
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
               format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++){\n",iDm*3);
               buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
            }
            format.SetFormatted("%%%ds      %%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted(" = (%s)%s;\n",valueType[iFold][i].Data(),valueInit[iFold][i].Data());
            for(iDm=0;iDm<valueDimension[iFold][i];iDm++){
               format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
               buffer.AppendFormatted(format.Data(),"");
            }
         }
      }
      buffer.AppendFormatted("      fModified = false;\n");
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("private:\n");

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
      WriteFile(hFile.Data(),buffer.Data(),6);

      // User H-File
      struct stat buf;
      hFile.SetFormatted("%s/include/framework/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
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

         buffer.AppendFormatted("#include <include/framework/%s%s_Base.h>\n",shortCut.Data(),folderName[iFold].Data());

         // Class
         buffer.AppendFormatted("\nclass %s%s : public %s%s_Base\n",shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) : %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("%s,",valueName[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) {}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
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
   taskUserCode[numOfTask] = false;
   taskAuthor[numOfTask] = mainAuthor;
   taskVersion[numOfTask] = "1";
   taskDescription[numOfTask] = "";
   numOfHistos[numOfTask] = 0;
   numOfTaskInclude[numOfTask] = 0;
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
      // task with changeble class file
      if (type == 1 && !strcmp((const char*)name,"ChangeableClassFile")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true")
            taskUserCode[numOfTask] = true;
      }
      // task author
      if (type == 1 && !strcmp((const char*)name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp((const char*)name,"AuthorName"))
               xml->GetValue(taskAuthor[numOfTask],taskAuthor[numOfTask]);
            if (type == 1 && !strcmp((const char*)name,"AuthorInstitute"))
               xml->GetValue(mainInstitute,mainInstitute);
            if (type == 1 && !strcmp((const char*)name,"AuthorCollaboration"))
               xml->GetValue(mainCollaboration,mainCollaboration);
            if (type == 1 && !strcmp((const char*)name,"AuthorEmail"))
               xml->GetValue(mainEmail,mainEmail);
            if (type == 15 && !strcmp((const char*)name,"Author"))
               break;
         }
      }
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
            cout << "Maximal number of includes in Task '" << taskName[numOfTask].Data() << "' reached : " << numOfTaskInclude << " !" << endl;
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
         histoType[numOfTask][numOfHistos[numOfTask]] = "TH1F";
         histoArraySize[numOfTask][numOfHistos[numOfTask]] = "1";
         histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]] = "0";
         histoXLabel[numOfTask][numOfHistos[numOfTask]] = "X";
         histoYLabel[numOfTask][numOfHistos[numOfTask]] = "Y";
         histoZLabel[numOfTask][numOfHistos[numOfTask]] = "Z";
         histoXNbins[numOfTask][numOfHistos[numOfTask]] = "1";
         histoXmin[numOfTask][numOfHistos[numOfTask]] = "0";
         histoXmax[numOfTask][numOfHistos[numOfTask]] = "1";
         histoYNbins[numOfTask][numOfHistos[numOfTask]] = "1";
         histoYmin[numOfTask][numOfHistos[numOfTask]] = "0";
         histoYmax[numOfTask][numOfHistos[numOfTask]] = "1";
         histoZNbins[numOfTask][numOfHistos[numOfTask]] = "1";
         histoZmin[numOfTask][numOfHistos[numOfTask]] = "0";
         histoZmax[numOfTask][numOfHistos[numOfTask]] = "1";
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
               xml->GetValue(histoArraySize[numOfTask][numOfHistos[numOfTask]],histoArraySize[numOfTask][numOfHistos[numOfTask]]);
            // histo array start index
            if (type == 1 && !strcmp((const char*)name,"HistArrayStartIndex"))
               xml->GetValue(histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]],histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]]);
            // histo x label
            if (type == 1 && !strcmp((const char*)name,"HistXLabel"))
               xml->GetValue(histoXLabel[numOfTask][numOfHistos[numOfTask]],histoXLabel[numOfTask][numOfHistos[numOfTask]]);
            // histo y label
            if (type == 1 && !strcmp((const char*)name,"HistYLabel"))
               xml->GetValue(histoYLabel[numOfTask][numOfHistos[numOfTask]],histoYLabel[numOfTask][numOfHistos[numOfTask]]);
            // histo z label
            if (type == 1 && !strcmp((const char*)name,"HistZLabel"))
               xml->GetValue(histoZLabel[numOfTask][numOfHistos[numOfTask]],histoZLabel[numOfTask][numOfHistos[numOfTask]]);
            // histo number of bins in x
            if (type == 1 && !strcmp((const char*)name,"HistXNbins"))
               xml->GetValue(histoXNbins[numOfTask][numOfHistos[numOfTask]],histoXNbins[numOfTask][numOfHistos[numOfTask]]);
            // histo min value in x
            if (type == 1 && !strcmp((const char*)name,"HistXmin"))
               xml->GetValue(histoXmin[numOfTask][numOfHistos[numOfTask]],histoXmin[numOfTask][numOfHistos[numOfTask]]);
            // histo max value in x
            if (type == 1 && !strcmp((const char*)name,"HistXmax"))
               xml->GetValue(histoXmax[numOfTask][numOfHistos[numOfTask]],histoXmax[numOfTask][numOfHistos[numOfTask]]);
            // histo number of bins in y
            if (type == 1 && !strcmp((const char*)name,"HistYNbins"))
               xml->GetValue(histoYNbins[numOfTask][numOfHistos[numOfTask]],histoYNbins[numOfTask][numOfHistos[numOfTask]]);
            // histo min value in y
            if (type == 1 && !strcmp((const char*)name,"HistYmin"))
               xml->GetValue(histoYmin[numOfTask][numOfHistos[numOfTask]],histoYmin[numOfTask][numOfHistos[numOfTask]]);
            // histo max value in y
            if (type == 1 && !strcmp((const char*)name,"HistYmax"))
               xml->GetValue(histoYmax[numOfTask][numOfHistos[numOfTask]],histoYmax[numOfTask][numOfHistos[numOfTask]]);
            // histo number of bins in z
            if (type == 1 && !strcmp((const char*)name,"HistZNbins"))
               xml->GetValue(histoZNbins[numOfTask][numOfHistos[numOfTask]],histoZNbins[numOfTask][numOfHistos[numOfTask]]);
            // histo min value in z
            if (type == 1 && !strcmp((const char*)name,"HistZmin"))
               xml->GetValue(histoZmin[numOfTask][numOfHistos[numOfTask]],histoZmin[numOfTask][numOfHistos[numOfTask]]);
            // histo max value in z
            if (type == 1 && !strcmp((const char*)name,"HistZmax"))
               xml->GetValue(histoZmax[numOfTask][numOfHistos[numOfTask]],histoZmax[numOfTask][numOfHistos[numOfTask]]);
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
         steerArraySize[numOfTask][0] = "1";
         steerParent[numOfTask][0] = -1;
         actualSteerIndex = 0;
         recursiveSteerDepth = 0;
         if (!ReadXMLSteering(numOfTask))
            return false;
         numOfSteering[numOfTask]++;
      }
   }
   return true;
}
bool ROMEBuilder::WriteTaskCpp() {
   ROMEString cppFile;
   ROMEString header;
   ROMEString buffer;
   ROMEString fileBuffer;

   int lenTot,ll,i,j;
   char *pos;
   ROMEString format;
   ROMEString discript;
   ROMEString str;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      header.Resize(0);
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%s/src/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      header.AppendFormatted("//// Author: %s\n",taskAuthor[iTask].Data());
      header.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      header.AppendFormatted("//                                                                            //\n");
      ll = 73-shortCut.Length();
      format.SetFormatted("// %%sT%%-%d.%ds //\n",ll,ll);
      header.AppendFormatted(format.Data(),shortCut.Data(),taskName[iTask].Data());
      header.AppendFormatted("//                                                                            //\n");
      discript = taskDescription[iTask].Data();
      pos = (char*)discript.Data();
      lenTot = discript.Length();
      while (pos-discript.Data() < lenTot) {
         if (lenTot+(discript.Data()-pos)<74)
            i=lenTot+(discript.Data()-pos);
         else for (i=74;pos[i]!=32&&i>0;i--) {}
         if (i<=0)
            i=TMath::Min(75,lenTot);
         pos[i] = 0;
         header.AppendFormatted("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      header.AppendFormatted("// \n");
      header.AppendFormatted("// \n");
      header.AppendFormatted("// This header has been generated by the ROMEBuilder.\n");
      if (!taskFortran[iTask]) {
         header.AppendFormatted("// The event methods have been written by %s.\n",taskAuthor[iTask].Data());
         fstream *fileStream = new fstream(cppFile.Data(),ios::in);
         fileBuffer.ReadFile(*fileStream);
         delete fileStream;
         bool first = true;
         for (j=0;j<numOfFolder;j++) {
            if (accessFolder(fileBuffer,j)) {
               if (first) {
                  header.AppendFormatted("// \n");
                  header.AppendFormatted("// Please note: The following information is only correct after executing the\n");
                  header.AppendFormatted("//              ROMEBuilder.\n");
                  header.AppendFormatted("// \n");
                  header.AppendFormatted("// This task accesses the following folders :\n");
                  first = false;
               }
               header.AppendFormatted("//    %s\n",folderName[j].Data());
            }
         }
         header.AppendFormatted("// \n");
         if (numOfHistos[iTask]>0) {
            if (first) {
               header.AppendFormatted("// Please note: The following information is only correct after executing the\n");
               header.AppendFormatted("//              ROMEBuilder.\n");
               header.AppendFormatted("// \n");
               first = false;
            }
            header.AppendFormatted("// This task contains the following histgrams :\n");
            for (i=0;i<numOfHistos[iTask];i++) {
               header.AppendFormatted("//    %s\n",histoName[iTask][i].Data());
            }
            header.AppendFormatted("//\n");
            header.AppendFormatted("// The histograms are created and saved automaticaly by the task.\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// The following method can be used to fill a histogram :\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// Fill<Histogram Name>(double value,double weight)\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// For histogram arrays use :\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// Fill<Histogram Name>At(int index,double value,double weight)\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// If more histogram functions are needed use the following function the get\n");
            header.AppendFormatted("// a handle to the histogram and use the root functions.\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// Get<Histogram Name>()\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// For histogram arrays use :\n");
            header.AppendFormatted("//\n");
            header.AppendFormatted("// Get<Histogram Name>At(int index)\n");
         }
      }
      header.AppendFormatted("//                                                                            //\n");
      header.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");

      // fortran task
      if (taskFortran[iTask]) {
         ROMEString shortcut(shortCut);
         shortcut.ToLower();
         ROMEString taskname(taskName[iTask]);
         taskname.ToLower();
         ROMEString ShortCut(shortCut);
         ShortCut.ToUpper();
         ROMEString TaskName(taskName[iTask]);
         TaskName.ToUpper();

         // Header
         buffer.AppendFormatted("\n\n#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#include <Riostream.h>\n");

         buffer.AppendFormatted("\nClassImp(%sT%s)\n\n",shortCut.Data(),taskName[iTask].Data());

         // User Functions
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("extern \"C\" void %sT%s_INIT();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("void %sT%s::Init()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_INIT();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_BEGINOFRUN();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_BEGINOFRUN();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_EVENT();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("void %sT%s::Event()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_EVENT();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_ENDOFRUN();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_ENDOFRUN();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_TERMINATE();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("void %sT%s::Terminate()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_TERMINATE();\n",ShortCut.Data(),TaskName.Data());
         buffer.AppendFormatted("}\n\n");
#endif
#if defined( R__UNIX )
         buffer.AppendFormatted("extern \"C\" void %st%s_init_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("void %sT%s::Init()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_init_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_beginofrun_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_beginofrun_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_event_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("void %sT%s::Event()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_event_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_endofrun_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_endofrun_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_terminate_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("void %sT%s::Terminate()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_terminate_();\n",shortcut.Data(),taskname.Data());
         buffer.AppendFormatted("}\n\n");
         //Write File
         header += buffer;
         WriteFile(cppFile.Data(),header.Data(),6);
#endif
      }
      else{ // c++ task
         buffer.Resize(0);
         // Header
         buffer.AppendFormatted("\n\n#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#include <Riostream.h>\n");

         buffer.AppendFormatted("\nClassImp(%sT%s)\n\n",shortCut.Data(),taskName[iTask].Data());

         // User Functions
         buffer.AppendFormatted("void %sT%s::Init()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("void %sT%s::Event()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("void %sT%s::Terminate()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());

         //Write File
         ReplaceHeader(cppFile.Data(),header.Data(),buffer.Data(),6);
      }
   }
   return true;
}

bool ROMEBuilder::WriteTaskF() {
   ROMEString fFile;
   ROMEString buffer;

   struct stat buf;

   if (makeOutput) cout << "\n   Output F-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (taskFortran[iTask]) {
         // File name
         fFile.SetFormatted("%s/src/tasks/%sTF%s.f",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

         ROMEString shortcut(shortCut);
         shortcut.ToLower();
         ROMEString taskname(taskName[iTask]);
         taskname.ToLower();

         if( stat( fFile.Data(), &buf )) {
            if (makeOutput) cout << "      " << fFile.Data() << endl;

            // Methods
            buffer.Resize(0);
            buffer.AppendFormatted("      subroutine %st%s_init()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_beginofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_event()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_endofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_terminate()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");

            // Write File
            WriteFile(fFile.Data(),buffer.Data(),6);
         }
      }
   }
   return true;
}
bool ROMEBuilder::WriteTaskH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;

   int i,j;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      buffer.Resize(0);
      // File name
      if (taskUserCode[iTask])
         hFile.SetFormatted("%s/include/tasks/%sT%s_Base.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      else
         hFile.SetFormatted("%s/include/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
      buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
      buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

      // Header
      if (taskUserCode[iTask]) {
         buffer.AppendFormatted("#ifndef %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_Base_H\n\n",shortCut.Data(),taskName[iTask].Data());
      }
      else {
         buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());
      }

      buffer.AppendFormatted("#include <RConfig.h>\n");
      buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
      buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
      buffer.AppendFormatted("#include<TH1.h>\n");
      buffer.AppendFormatted("#include<TH2.h>\n");
      buffer.AppendFormatted("#include<TH3.h>\n");
      buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
      buffer.AppendFormatted("#pragma warning( pop )\n");
      buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
      buffer.AppendFormatted("#include<ROMETask.h>\n");

      for (i=0;i<numOfTaskInclude[iTask];i++) {
         if (taskLocalFlag[iTask][i]) {
            buffer.AppendFormatted("#include\"%s\"\n",taskInclude[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("#include<%s>\n",taskInclude[iTask][i].Data());
         }
      }

      buffer.AppendFormatted("#include <include/framework/%sGlobalSteering.h>\n",shortCut.Data());
      buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());

      // Class
      if (taskUserCode[iTask])
         buffer.AppendFormatted("\nclass %sT%s_Base : public ROMETask\n",shortCut.Data(),taskName[iTask].Data());
      else
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
         if (histoArraySize[iTask][i]=="1") {
            format.SetFormatted("   %%s*      f%%s;%%%ds // %%s Handle\n",20+nameLen-histoName[iTask][i].Length()+1);
            buffer.AppendFormatted(format.Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         }
         else {
            format.SetFormatted("   TObjArray* f%%ss;%%%ds // %%s Handle\n",10+nameLen-histoName[iTask][i].Length());
            buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         }
         format.SetFormatted("   ROMEString f%%sTitle;%%%ds // Title of %%s\n",20+nameLen-histoName[iTask][i].Length()-5);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sTitleCode;%%%ds // Interpreter code of Title of %%s\n",20+nameLen-histoName[iTask][i].Length()-9);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sFolderTitle;%%%ds // Title of the Folder of %%s\n",20+nameLen-histoName[iTask][i].Length()-11);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sFolderTitleCode;%%%ds // Interpreter code of Title of the Folder of %%s\n",20+nameLen-histoName[iTask][i].Length()-15);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sArraySize;%%%ds // Array Size of %%s\n",20+nameLen-histoName[iTask][i].Length()-9);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sArraySizeCode;%%%ds // Interpreter code of Array Size of %%s\n",20+nameLen-histoName[iTask][i].Length()-13);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sArrayStartIndex;%%%ds // Array Start Index of %%s\n",20+nameLen-histoName[iTask][i].Length()-15);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sArrayStartIndexCode;%%%ds // Interpreter code of Array Start Index of %%s\n",20+nameLen-histoName[iTask][i].Length()-19);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sXLabel;%%%ds // Label of the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sXLabelCode;%%%ds // Interpreter code of Label of the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sYLabel;%%%ds // Label of the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sYLabelCode;%%%ds // Interpreter code of Label of the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sZLabel;%%%ds // Label of the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sZLabelCode;%%%ds // Interpreter code of Label of the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sXNbins;%%%ds // Number of bins on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sXNbinsCode;%%%ds // Interpreter code of Number of bins on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   double     f%%sXmin;%%%ds // Minimum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sXminCode;%%%ds // Interpreter code of Minimum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   double     f%%sXmax;%%%ds // Maximum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sXmaxCode;%%%ds // Interpreter code of Minimum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sYNbins;%%%ds // Number of bins on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sYNbinsCode;%%%ds // Interpreter code of Number of bins on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   double     f%%sYmin;%%%ds // Minimum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sYminCode;%%%ds // Interpreter code of Minimum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   double     f%%sYmax;%%%ds // Maximum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sYmaxCode;%%%ds // Interpreter code of Maximum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sZNbins;%%%ds // Number of bins on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sZNbinsCode;%%%ds // Interpreter code of Number of bins on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   double     f%%sZmin;%%%ds // Minimum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sZminCode;%%%ds // Interpreter code of Minimum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   double     f%%sZmax;%%%ds // Maximum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   int        f%%sZmaxCode;%%%ds // Interpreter code of Maximum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   bool       f%%sAccumulation;%%%ds // Accumulation Flag for the %%s\n",20+nameLen-histoName[iTask][i].Length()-12);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
      }
      buffer.AppendFormatted("\n   ROMEString fHistoSuffix; //!\n");
      buffer.AppendFormatted("\n   char* cstop; //!\n\n");


      // Methods
      buffer.AppendFormatted("public:\n");
      // Constructor and Event Methods
      buffer.AppendFormatted("   // Constructor\n");
      if (taskUserCode[iTask])
         buffer.AppendFormatted("   %sT%s_Base(const char *name,const char *title,const char *histoSuffix,TFolder *histoFolder);\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("   %sT%s(const char *name,const char *title,const char *histoSuffix,TFolder *histoFolder);\n",shortCut.Data(),taskName[iTask].Data());
      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; };\n");
      }
      // Access Methods
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]=="1") {
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
            buffer.AppendFormatted("   %s* Get%s() { return f%s; };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
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
            buffer.AppendFormatted("   %s* Get%sAt(int index) { return (%s*)f%ss->At(index); };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
         }
         buffer.AppendFormatted("   void Set%sTitle(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sFolderTitle(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sArraySize(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sArrayStartIndex(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXLabel(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYLabel(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZLabel(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXNbins(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXmin(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXmax(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYNbins(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYmin(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYmax(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZNbins(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZmin(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZmax(const char* value);\n",histoName[iTask][i].Data());

         buffer.AppendFormatted("   Bool_t is%sAccumulation()                { return f%sAccumulation; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sAccumulation(Bool_t flag)      { f%sAccumulation = flag; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
      }


      // Object Interpreter
      buffer.AppendFormatted("   int GetObjectInterpreterCode(const char* objectPath);\n");
      buffer.AppendFormatted("   int GetObjectInterpreterIntValue(int code,int defaultValue);\n");
      buffer.AppendFormatted("   double GetObjectInterpreterDoubleValue(int code,double defaultValue);\n");
      buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(int code,ROMEString& defaultValue,ROMEString& buffer);\n");
      // Protected
      buffer.AppendFormatted("protected:\n");
      buffer.AppendFormatted("   // Event Methods\n");
      if (taskUserCode[iTask]) {
         buffer.AppendFormatted("   virtual void Init() = 0;\n");
         buffer.AppendFormatted("   virtual void BeginOfRun() = 0;\n");
         buffer.AppendFormatted("   virtual void Event() = 0;\n");
         buffer.AppendFormatted("   virtual void EndOfRun() = 0;\n");
         buffer.AppendFormatted("   virtual void Terminate() = 0;\n\n");
      }
      else {
         buffer.AppendFormatted("   virtual void Init();\n");
         buffer.AppendFormatted("   virtual void BeginOfRun();\n");
         buffer.AppendFormatted("   virtual void Event();\n");
         buffer.AppendFormatted("   virtual void EndOfRun();\n");
         buffer.AppendFormatted("   virtual void Terminate();\n\n");
      }
      // Histo Methods
      buffer.AppendFormatted("   // Histo Methods\n");
      buffer.AppendFormatted("   virtual void BookHisto();\n");
      buffer.AppendFormatted("   virtual void ResetHisto();\n\n");
      // Footer
      if (taskUserCode[iTask])
         buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
      else
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
      buffer.AppendFormatted("};\n\n");

      // Histo Inline Methods
      // Constructor
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline %sT%s_Base::%sT%s_Base(const char *name,const char *title,const char *histoSuffix,TFolder *histoFolder):ROMETask(name,title) {\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline %sT%s::%sT%s(const char *name,const char *title,const char *histoSuffix,TFolder *histoFolder):ROMETask(name,title) {\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   fHistoSuffix = histoSuffix;\n");
      buffer.AppendFormatted("   fHistoFolder = histoFolder;\n");
      buffer.AppendFormatted("   fEventID = '%s';\n",taskEventID[iTask].Data());
      buffer.AppendFormatted("   fVersion = %s;\n",taskVersion[iTask].Data());
      if (numOfHistos[iTask]>0)
         buffer.AppendFormatted("   fHasHistograms = true;\n");
      else
         buffer.AppendFormatted("   fHasHistograms = false;\n");
      for (i=0;i<numOfHistos[iTask];i++) {
         buffer.AppendFormatted("   f%sTitle = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sTitleCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sFolderTitle = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sFolderTitleCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXLabel = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXLabelCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYLabel = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYLabelCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZLabel = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZLabelCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArraySize = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArraySizeCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArrayStartIndex = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArrayStartIndexCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXNbins = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXNbinsCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXmin = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXminCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXmax = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXmaxCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYNbins = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYNbinsCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYmin = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYminCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYmax = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYmaxCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZNbins = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZNbinsCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZmin = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZminCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZmax = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZmaxCode = -1;\n",histoName[iTask][i].Data());
      }
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   fSteering = new Steering();\n");
      }
      buffer.AppendFormatted("};\n");

      // Book
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline void %sT%s_Base::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline void %sT%s::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      bool array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]!="1") array = true;
      }
      if (array) {
         buffer.AppendFormatted("   int j;\n");
         buffer.AppendFormatted("   ROMEString name;\n");
         buffer.AppendFormatted("   ROMEString title;\n");
      }
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("   ROMEString histoName;\n");
         buffer.AppendFormatted("   ROMEString histoTitle;\n");
         buffer.AppendFormatted("   ROMEString folderTitle;\n");
         buffer.AppendFormatted("   ROMEString xLabel;\n");
         buffer.AppendFormatted("   ROMEString yLabel;\n");
         buffer.AppendFormatted("   ROMEString zLabel;\n");
         buffer.AppendFormatted("   int arraySize;\n");
         buffer.AppendFormatted("   int arrayStartIndex;\n");
         buffer.AppendFormatted("   int xNbins;\n");
         buffer.AppendFormatted("   double xmin;\n");
         buffer.AppendFormatted("   double xmax;\n");
         buffer.AppendFormatted("   int yNbins;\n");
         buffer.AppendFormatted("   double ymin;\n");
         buffer.AppendFormatted("   double ymax;\n");
         buffer.AppendFormatted("   int zNbins;\n");
         buffer.AppendFormatted("   double zmin;\n");
         buffer.AppendFormatted("   double zmax;\n");
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
         buffer.AppendFormatted("   if (f%sTitle==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      histoTitle = \"%s\";\n",histoTitle[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      histoTitle = f%sTitle;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sFolderTitle==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      folderTitle = \"%s\";\n",histoFolderTitle[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      folderTitle = f%sFolderTitle;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sXLabel==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      xLabel = \"%s\";\n",histoXLabel[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      xLabel = f%sXLabel;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sYLabel==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      yLabel = \"%s\";\n",histoYLabel[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      yLabel = f%sYLabel;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sZLabel==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      zLabel = \"%s\";\n",histoZLabel[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      zLabel = f%sZLabel;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sArraySize==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      arraySize = %s;\n",histoArraySize[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      arraySize = f%sArraySize;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sArrayStartIndex==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      arrayStartIndex = %s;\n",histoArrayStartIndex[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      arrayStartIndex = f%sArrayStartIndex;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sXNbins==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      xNbins = %s;\n",histoXNbins[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      xNbins = f%sXNbins;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sXmin==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      xmin = %s;\n",histoXmin[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      xmin = f%sXmin;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sXmax==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      xmax = %s;\n",histoXmax[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      xmax = f%sXmax;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sYNbins==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      yNbins = %s;\n",histoYNbins[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      yNbins = f%sYNbins;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sYmin==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      ymin = %s;\n",histoYmin[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      ymin = f%sYmin;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sYmax==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      ymax = %s;\n",histoYmax[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      ymax = f%sYmax;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sZNbins==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      zNbins = %s;\n",histoZNbins[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      zNbins = f%sZNbins;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sZmin==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      zmin = %s;\n",histoZmin[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      zmin = f%sZmin;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   if (f%sZmax==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      zmax = %s;\n",histoZmax[iTask][i].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      zmax = f%sZmax;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   histoTitle = GetObjectInterpreterCharValue(f%sTitleCode,histoTitle,histoTitle);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   folderTitle = GetObjectInterpreterCharValue(f%sFolderTitleCode,folderTitle,folderTitle);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xLabel = GetObjectInterpreterCharValue(f%sXLabelCode,xLabel,xLabel);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   yLabel = GetObjectInterpreterCharValue(f%sYLabelCode,yLabel,yLabel);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zLabel = GetObjectInterpreterCharValue(f%sZLabelCode,zLabel,zLabel);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   arraySize = GetObjectInterpreterIntValue(f%sArraySizeCode,arraySize);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   arrayStartIndex = GetObjectInterpreterIntValue(f%sArrayStartIndexCode,arrayStartIndex);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xNbins = GetObjectInterpreterIntValue(f%sXNbinsCode,xNbins);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xmin = GetObjectInterpreterDoubleValue(f%sXminCode,xmin );\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xmax = GetObjectInterpreterDoubleValue(f%sXmaxCode,xmax );\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   yNbins = GetObjectInterpreterIntValue(f%sYNbinsCode,yNbins);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   ymin = GetObjectInterpreterDoubleValue(f%sYminCode,ymin );\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   ymax = GetObjectInterpreterDoubleValue(f%sYmaxCode,ymax);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zNbins = GetObjectInterpreterIntValue(f%sZNbinsCode,zNbins);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zmin = GetObjectInterpreterDoubleValue(f%sZminCode,zmin);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zmax = GetObjectInterpreterDoubleValue(f%sZmaxCode,zmax);\n",histoName[iTask][i].Data());

         // create histo folder
         if (!sameFolder&&!homeFolder) {
            buffer.AppendFormatted("   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",folderTitle.Data());\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data());
         }
         // create histos
         buffer.AppendFormatted("   histoName = \"%s\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   histoName+=fHistoSuffix;\n");
         if (histoArraySize[iTask][i]=="1") {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zNbins,zmin,zmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            if (!homeFolder)
               buffer.AppendFormatted("   %sFolder->Add(f%s);\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
            else
               buffer.AppendFormatted("   GetHistoFolder()->Add(f%s);\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetXaxis()->SetTitle(xLabel.Data());\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetYaxis()->SetTitle(yLabel.Data());\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetZaxis()->SetTitle(zLabel.Data());\n",histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   %s *hist%d;\n",histoType[iTask][i].Data(),i);
            buffer.AppendFormatted("   f%ss = new TObjArray(arraySize);\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   for (j=0;j<arraySize;j++) {\n");
            buffer.AppendFormatted("      name.SetFormatted(\"_%%0*d\",3,j+arrayStartIndex);\n");
            buffer.AppendFormatted("      name.Insert(0,histoName.Data());\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      title.SetFormatted(\" %%0*d\",3,j+arrayStartIndex);\n");
            buffer.AppendFormatted("      title.Insert(0,histoTitle.Data());\n");
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("      hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax);\n",i,histoType[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("      hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax);\n",i,histoType[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("      hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zNbins,zmin,zmax);\n",i,histoType[iTask][i].Data());
            }
            buffer.AppendFormatted("      f%ss->Add(hist%d);\n",histoName[iTask][i].Data(),i);
            if (!homeFolder)
               buffer.AppendFormatted("      %sFolder->Add(f%ss->At(j));\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
            else
               buffer.AppendFormatted("      GetHistoFolder()->Add(f%ss->At(j));\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      hist%d->GetXaxis()->SetTitle(xLabel.Data());\n",i);
            buffer.AppendFormatted("      hist%d->GetYaxis()->SetTitle(yLabel.Data());\n",i);
            buffer.AppendFormatted("      hist%d->GetZaxis()->SetTitle(zLabel.Data());\n",i);
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // Reset
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline void %sT%s_Base::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline void %sT%s::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]!="1")
            array = true;
      }
      if (array) {
         buffer.AppendFormatted("   int j;\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   if (!is%sAccumulation()) f%s->Reset();\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   if (!is%sAccumulation()) {\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("       for (j=0;j<f%ss->GetEntries();j++) ((%s*)f%ss->At(j))->Reset();\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      ROMEString fctHead;
      if (taskUserCode[iTask])
         fctHead.SetFormatted("inline void %sT%s_Base::",shortCut.Data(),taskName[iTask].Data());
      else
         fctHead.SetFormatted("inline void %sT%s::",shortCut.Data(),taskName[iTask].Data());
      // Parameter Setters
      for (i=0;i<numOfHistos[iTask];i++) {
         buffer.AppendFormatted("%sSet%sTitle(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sTitleCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sTitle=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sFolderTitle(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sFolderTitleCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sFolderTitle=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sArraySize(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sArraySizeCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sArraySize=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sArrayStartIndex(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sArrayStartIndexCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sArrayStartIndex=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXLabel(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXLabelCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXLabel=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYLabel(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYLabelCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYLabel=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZLabel(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZLabelCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZLabel=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXNbins(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXNbinsCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXNbins=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXmin(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXminCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXmin=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXmax(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXmaxCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXmax=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYNbins(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYNbinsCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYNbins=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYmin(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYminCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYmin=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYmax(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYmaxCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYmax=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZNbins(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZNbinsCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZNbins=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZmin(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZminCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZmin=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZmax(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZmaxCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZmax=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }


      // Get Object Interpreter Code
      int codeNumber = fNumberOfInterpreterCodes;
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline int %sT%s_Base::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline int %sT%s::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   ROMEString path = objectPath;\n");
      buffer.AppendFormatted("   if (path.Index(\"/\")!=0)\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   if (path.Index(\"/SP\")==0) {\n");
      ROMEString path = "/SP";
      WriteSteeringInterpreterCode(buffer,codeNumber,0,iTask,path,1);
      buffer.AppendFormatted("      gAnalyzer->Println(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
      buffer.AppendFormatted("      gAnalyzer->Println(\"   \"+path+\"\\n\");\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterCode(objectPath);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Int Value
      codeNumber = fNumberOfInterpreterCodes;
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline int %sT%s_Base::GetObjectInterpreterIntValue(int code,int defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline int %sT%s::GetObjectInterpreterIntValue(int code,int defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   char *cstop=NULL;\n");
      buffer.AppendFormatted("   switch (code) {\n");
      buffer.AppendFormatted("      case -1:\n");
      buffer.AppendFormatted("         return defaultValue;\n");
      ROMEString steerPointer = "GetSP()";
      WriteSteeringInterpreterValue(buffer,"int",codeNumber,0,iTask,steerPointer,1);
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterIntValue(code,defaultValue);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Double Value
      codeNumber = fNumberOfInterpreterCodes;
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline double %sT%s_Base::GetObjectInterpreterDoubleValue(int code,double defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline double %sT%s::GetObjectInterpreterDoubleValue(int code,double defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   char *cstop=NULL;\n");
      buffer.AppendFormatted("   switch (code) {\n");
      buffer.AppendFormatted("      case -1:\n");
      buffer.AppendFormatted("         return defaultValue;\n");
      steerPointer = "GetSP()";
      WriteSteeringInterpreterValue(buffer,"double",codeNumber,0,iTask,steerPointer,1);
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterDoubleValue(code,defaultValue);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Char Value
      codeNumber = fNumberOfInterpreterCodes;
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline ROMEString& %sT%s_Base::GetObjectInterpreterCharValue(int code,ROMEString& defaultValue,ROMEString& buffer) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline ROMEString& %sT%s::GetObjectInterpreterCharValue(int code,ROMEString& defaultValue,ROMEString& buffer) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   ROMEString str;\n");
      buffer.AppendFormatted("   switch (code) {\n");
      buffer.AppendFormatted("      case -1:\n");
      buffer.AppendFormatted("         return defaultValue;\n");
      steerPointer = "GetSP()";
      WriteSteeringInterpreterValue(buffer,"const char*",codeNumber,0,iTask,steerPointer,1);
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterCharValue(code,defaultValue,buffer);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");


      if (taskUserCode[iTask])
         buffer.AppendFormatted("#endif   // %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

      // Write File
      WriteFile(hFile.Data(),buffer.Data(),6);

      // User H-File
      struct stat buf;
      hFile.SetFormatted("%s/include/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      if (taskUserCode[iTask] && stat( hFile.Data(), &buf )) {
         // Description
         buffer.Resize(0);
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("// Editable class file for %s task.\n",taskName[iTask].Data());
         buffer.AppendFormatted("//                                                                            //\n");
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

         // Header
         buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());

         buffer.AppendFormatted("#include <include/tasks/%sT%s_Base.h>\n",shortCut.Data(),taskName[iTask].Data());

         // Class
         buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %sT%s(const char *name,const char *title,const char *histoSuffix,TFolder *histoFolder):%sT%s_Base(name,title,histoSuffix,histoFolder) {};\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("protected:\n");
         buffer.AppendFormatted("   // Event Methods\n");
         buffer.AppendFormatted("   virtual void Init();\n");
         buffer.AppendFormatted("   virtual void BeginOfRun();\n");
         buffer.AppendFormatted("   virtual void Event();\n");
         buffer.AppendFormatted("   virtual void EndOfRun();\n");
         buffer.AppendFormatted("   virtual void Terminate();\n\n");
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
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
         treeFileName[numOfTree] = "";

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
            // tree file name
            if (type == 1 && !strcmp((const char*)name,"TreeOutputFileName"))
               xml->GetValue(treeFileName[numOfTree],treeFileName[numOfTree]);
            if (type == 1 && !strcmp((const char*)name,"Branch")) {
               // branch initialisation
               branchName[numOfTree][numOfBranch[numOfTree]] = "";
               branchFolder[numOfTree][numOfBranch[numOfTree]] = "";
               branchBufferSize[numOfTree][numOfBranch[numOfTree]] = "32000";
               branchSplitLevel[numOfTree][numOfBranch[numOfTree]] = "99";
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
                  // branch buffer size
                  if (type == 1 && !strcmp((const char*)name,"BufferSize"))
                     xml->GetValue(branchBufferSize[numOfTree][numOfBranch[numOfTree]],branchBufferSize[numOfTree][numOfBranch[numOfTree]]);
                  // branch split level
                  if (type == 1 && !strcmp((const char*)name,"SplitLevel"))
                     xml->GetValue(branchSplitLevel[numOfTree][numOfBranch[numOfTree]],branchSplitLevel[numOfTree][numOfBranch[numOfTree]]);
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
                  if (!folderSupport[i] && folderName[i]==branchFolder[numOfTree][numOfBranch[numOfTree]])
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
bool ROMEBuilder::ReadXMLDAQ() {
   char *name;
   int type,i,j;

   // output
   if (makeOutput) cout << "\n\nUser DAQ Systems:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"UserDAQSystem")) {
         // count trees
         numOfDAQ++;
         if (numOfDAQ>=maxNumberOfDAQ) {
            cout << "Maximal number of user DAQ systems reached : " << maxNumberOfDAQ << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // daq initialisation
         daqName[numOfDAQ] = "";

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // daq name
            if (type == 1 && !strcmp((const char*)name,"DAQSystemName")) {
               xml->GetValue(daqName[numOfDAQ],daqName[numOfDAQ]);
               // output
               if (makeOutput) cout << "   " << daqName[numOfDAQ].Data() << endl;
            }
            if (type == 15 && !strcmp((const char*)name,"UserDAQSystem")) {
               // input check
               if (daqName[numOfDAQ]=="") {
                  cout << "User DAQ system without a name !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               break;
            }
         }
      }

      if (type == 15 && !strcmp((const char*)name,"UserDAQSystems")) {
         // input check
         for (i=0;i<numOfDAQ;i++) {
            for (j=i+1;j<numOfDAQ;j++) {
               if (daqName[i]==daqName[j]) {
                  cout << "\nDAQ system '" << daqName[i].Data() << "' is defined twice !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         break;
      }
   }
   numOfDAQ++;
   return true;
}
bool ROMEBuilder::ReadXMLDB() {
   char *name;
   int type,i,j;

   // output
   if (makeOutput) cout << "\n\nUser Database Interfaces:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"UserDatabase")) {
         // count trees
         numOfDB++;
         if (numOfDB>=maxNumberOfDB) {
            cout << "Maximal number of user database interfaces reached : " << maxNumberOfDB << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // database initialisation
         dbName[numOfDB] = "";

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // database name
            if (type == 1 && !strcmp((const char*)name,"DatabaseName")) {
               xml->GetValue(dbName[numOfDB],dbName[numOfDB]);
               // output
               if (makeOutput) cout << "   " << dbName[numOfDB].Data() << endl;
            }
            // database description
            if (type == 1 && !strcmp((const char*)name,"DatabaseDescription"))
               xml->GetValue(dbDescription[numOfDB],dbDescription[numOfDB]);
            if (type == 15 && !strcmp((const char*)name,"UserDatabase")) {
               // input check
               if (dbName[numOfDB]=="") {
                  cout << "User database interface without a name !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               break;
            }
         }
      }

      if (type == 15 && !strcmp((const char*)name,"UserDatabases")) {
         // input check
         for (i=0;i<numOfDB;i++) {
            for (j=i+1;j<numOfDB;j++) {
               if (dbName[i]==dbName[j]) {
                  cout << "\nDatabase '" << dbName[i].Data() << "' is defined twice !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
         break;
      }
   }
   numOfDB++;
   return true;
}
bool ROMEBuilder::ReadXMLMidasBanks() {
   char *name;
   int type,i,j,k,kk;
   bankHasHeader = false;
   ROMEString tmp;
   char *cstop=NULL;

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
            if (!folderSupport[i] && folderName[i]==bankHeaderFolder) {
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
      else if (type == 1 && !strcmp((const char*)name,"EventDefinition")) {
         numOfEvent++;
         if (numOfEvent>=maxNumberOfEvents) {
            cout << "Maximal number of midas events reached : " << maxNumberOfEvents << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // event initialisation
         eventName[numOfEvent] = "";
         eventID[numOfEvent] = "-1";
         eventTriggerMask[numOfEvent] = "-1";
         eventSamplingRate[numOfEvent] = "1";
         numOfBank[numOfEvent] = -1;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // event name
            if (type == 1 && !strcmp((const char*)name,"EventName")) {
               xml->GetValue(eventName[numOfEvent],eventName[numOfEvent]);
               // output
               if (makeOutput) cout << "   " << eventName[numOfEvent].Data() << endl;
            }
            // event id
            if (type == 1 && !strcmp((const char*)name,"EventID"))
               xml->GetValue(eventID[numOfEvent],eventID[numOfEvent]);
            // event trigger mask
            if (type == 1 && !strcmp((const char*)name,"EventTriggerMask"))
               xml->GetValue(eventTriggerMask[numOfEvent],eventTriggerMask[numOfEvent]);
            // event sampling rate
            if (type == 1 && !strcmp((const char*)name,"EventSamplingRate"))
               xml->GetValue(eventSamplingRate[numOfEvent],eventSamplingRate[numOfEvent]);
            if (type == 1 && !strcmp((const char*)name,"Banks")) {
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  if (type == 1 && !strcmp((const char*)name,"Bank")) {
                     // count banks
                     numOfBank[numOfEvent]++;
                     if (numOfBank[numOfEvent]>=maxNumberOfBanks) {
                        cout << "Maximal number of banks reached : " << maxNumberOfBanks << " !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                     // bank initialisation
                     numOfStructFields[numOfEvent][numOfBank[numOfEvent]] = 0;
                     bankName[numOfEvent][numOfBank[numOfEvent]] = "";
                     bankType[numOfEvent][numOfBank[numOfEvent]] = "";
                     bankArraySize[numOfEvent][numOfBank[numOfEvent]] = 0;
                     bankArrayStart[numOfEvent][numOfBank[numOfEvent]] = 0;
                     bankArrayDigit[numOfEvent][numOfBank[numOfEvent]] = 0;
                     while (xml->NextLine()) {
                        type = xml->GetType();
                        name = xml->GetName();
                        // bank name
                        if (type == 1 && !strcmp((const char*)name,"BankName")) {
                           xml->GetValue(bankName[numOfEvent][numOfBank[numOfEvent]],bankName[numOfEvent][numOfBank[numOfEvent]]);
                           // output
                           if (makeOutput) cout << "      " << bankName[numOfEvent][numOfBank[numOfEvent]].Data() << endl;
                        }
                        // bank type
                        if (type == 1 && !strcmp((const char*)name,"BankType"))
                           xml->GetValue(bankType[numOfEvent][numOfBank[numOfEvent]],bankType[numOfEvent][numOfBank[numOfEvent]]);
                        // bank array
                        if (type == 1 && !strcmp((const char*)name,"BankArray")) {
                           while (xml->NextLine()) {
                              type = xml->GetType();
                              name = xml->GetName();
                              // lower boundary
                              if (type == 1 && !strcmp((const char*)name,"BASize")) {
                                 xml->GetValue(tmp,"");
                                 bankArraySize[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // upper boundary
                              if (type == 1 && !strcmp((const char*)name,"BAStartIndex")) {
                                 xml->GetValue(tmp,"");
                                 bankArrayStart[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // number of digits
                              if (type == 1 && !strcmp((const char*)name,"BANumberOfDigits")) {
                                 xml->GetValue(tmp,"");
                                 bankArrayDigit[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              if (type == 15 && !strcmp((const char*)name,"BankArray"))
                                 break;
                           }
                        }
                        // bank end
                        if (type == 15 && !strcmp((const char*)name,"Bank"))
                           break;
                     }
                     // input check
                     if (bankName[numOfEvent][numOfBank[numOfEvent]]=="") {
                        cout << "A Midas Bank has no Name !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                     if (bankType[numOfEvent][numOfBank[numOfEvent]]=="") {
                        cout << "Bank '" << bankName[numOfEvent][numOfBank[numOfEvent]].Data() << "' has no type !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  else if (type == 1 && !strcmp((const char*)name,"StructuredBank")) {
                     // count banks
                     numOfBank[numOfEvent]++;
                     if (numOfBank[numOfEvent]>=maxNumberOfBanks) {
                        cout << "Maximal number of banks reached : " << maxNumberOfBanks << " !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                     // structured bank initialisation
                     numOfStructFields[numOfEvent][numOfBank[numOfEvent]] = 0;
                     bankName[numOfEvent][numOfBank[numOfEvent]] = "";
                     bankType[numOfEvent][numOfBank[numOfEvent]] = "struct";
                     bankArraySize[numOfEvent][numOfBank[numOfEvent]] = 0;
                     bankArrayStart[numOfEvent][numOfBank[numOfEvent]] = 0;
                     bankArrayDigit[numOfEvent][numOfBank[numOfEvent]] = 0;
                     while (xml->NextLine()) {
                        type = xml->GetType();
                        name = xml->GetName();
                        // structured bank name
                        if (type == 1 && !strcmp((const char*)name,"BankName")) {
                           xml->GetValue(bankName[numOfEvent][numOfBank[numOfEvent]],bankName[numOfEvent][numOfBank[numOfEvent]]);
                           // output
                           if (makeOutput) cout << "      " << bankName[numOfEvent][numOfBank[numOfEvent]].Data() << endl;
                        }
                        // structured bank field
                        if (type == 1 && !strcmp((const char*)name,"BankField")) {
                           // structured bank field initialisation
                           structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]] = "";
                           structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]] = "";
                           structFieldSize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]] = "";
                           bankFieldArraySize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]] = "1";
                           while (xml->NextLine()) {
                              type = xml->GetType();
                              name = xml->GetName();
                              // field name
                              if (type == 1 && !strcmp((const char*)name,"BankFieldName")) {
                                 xml->GetValue(structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                                 // output
                                 if (makeOutput) cout << "         " << structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]].Data() << endl;
                              }
                              // field type
                              if (type == 1 && !strcmp((const char*)name,"BankFieldType"))
                                 xml->GetValue(structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                              // field size
                              if (type == 1 && !strcmp((const char*)name,"BankFieldSize"))
                                 xml->GetValue(structFieldSize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],structFieldSize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                              // bank field array size
                              if (type == 1 && !strcmp((const char*)name,"BankFieldArraySize"))
                                 xml->GetValue(bankFieldArraySize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],bankFieldArraySize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                              // field end
                              if (type == 15 && !strcmp((const char*)name,"BankField"))
                                 break;
                           }
                           // input check
                           if (structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]=="") {
                              cout << "A field of the midas bank '" << bankName[numOfEvent][numOfBank[numOfEvent]].Data() << "' has no name !" << endl;
                              cout << "Terminating program." << endl;
                              return false;
                           }
                           if (structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]=="") {
                              cout << "Structure field '" << structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]].Data() << "' of bank '" << bankName[numOfEvent][numOfBank[numOfEvent]].Data() << "' has no type !" << endl;
                              cout << "Terminating program." << endl;
                              return false;
                           }
                           for (i=0;i<numOfStructFields[numOfEvent][numOfBank[numOfEvent]];i++) {
                              for (j=i+1;j<numOfStructFields[numOfEvent][numOfBank[numOfEvent]];j++) {
                                 if (structFieldName[numOfEvent][numOfBank[numOfEvent]][i]==structFieldName[numOfEvent][numOfBank[numOfEvent]][j]) {
                                    cout << "\nStructure of bank '" << bankName[numOfEvent][numOfBank[numOfEvent]].Data() << "' has two fields with the name '" << structFieldName[numOfEvent][numOfBank[numOfEvent]][i].Data() << "' !" << endl;
                                    cout << "Terminating program." << endl;
                                    return false;
                                 }
                              }
                           }
                           // count structured bank fields
                           numOfStructFields[numOfEvent][numOfBank[numOfEvent]]++;
                           if (numOfStructFields[numOfEvent][numOfBank[numOfEvent]]>=maxNumberOfStructFields) {
                              cout << "Maximal number of fields in bank '" << bankName[numOfEvent][numOfBank[numOfEvent]].Data() << "' reached : " << maxNumberOfStructFields << " !" << endl;
                              cout << "Terminating program." << endl;
                              return false;
                           }
                        }
                        // bank array
                        if (type == 1 && !strcmp((const char*)name,"BankArray")) {
                           while (xml->NextLine()) {
                              type = xml->GetType();
                              name = xml->GetName();
                              // lower boundary
                              if (type == 1 && !strcmp((const char*)name,"BASize")) {
                                 xml->GetValue(tmp,"");
                                 bankArraySize[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // upper boundary
                              if (type == 1 && !strcmp((const char*)name,"BAStartIndex")) {
                                 xml->GetValue(tmp,"");
                                 bankArrayStart[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // number of digits
                              if (type == 1 && !strcmp((const char*)name,"BANumberOfDigits")) {
                                 xml->GetValue(tmp,"");
                                 bankArrayDigit[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              if (type == 15 && !strcmp((const char*)name,"BankArray"))
                                 break;
                           }
                        }
                        // structured bank end
                        if (type == 15 && !strcmp((const char*)name,"StructuredBank"))
                           break;
                     }
                     // input check
                     if (bankName[numOfEvent][numOfBank[numOfEvent]]=="") {
                        cout << "A structured midas bank has no Name !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // banks end
                  if (type == 15 && !strcmp((const char*)name,"Banks"))
                     break;
               }
            }
            // event end
            if (type == 15 && !strcmp((const char*)name,"EventDefinition")) {
               numOfBank[numOfEvent]++;
               if (eventName[numOfEvent]=="") {
                  cout << "Midas event definition without a name !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               break;
            }
         }
      }
      // midas bank end
      if (type == 15 && !strcmp((const char*)name,"MidasBanks")) {
         numOfEvent++;
         break;
      }
   }
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         for (k=0;k<numOfEvent;k++) {
            for (kk=0;kk<numOfBank[k];kk++) {
               if (bankName[i][j]==bankName[k][kk] && (i!=k || j!=kk)) {
                  cout << "\nMidas bank '" << bankName[i][j].Data() << "' is defined twice !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
            }
         }
      }
   }
   return true;
}

bool ROMEBuilder::ReadXMLSteering(int iTask) {
   // read the steering parameter definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i;
   ROMEString currentSteeringName = "";
   int currentNumberOfSteerings;

   // count steering parameters
   numOfSteering[iTask]++;
   currentNumberOfSteerings = numOfSteering[iTask];
   if (numOfSteering[iTask]>=maxNumberOfSteering) {
      cout << "Maximal number of steering parameters reached : " << maxNumberOfSteering << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   if (numOfSteering[iTask]>0) {
      steerName[iTask][numOfSteering[iTask]] = "";
      steerArraySize[iTask][numOfSteering[iTask]] = "1";
   }
   numOfSteerFields[iTask][numOfSteering[iTask]] = 0;
   numOfSteerChildren[iTask][numOfSteering[iTask]] = 0;

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
         steerParent[iTask][numOfSteering[iTask]+1] = actualSteerIndex;
         actualSteerIndex = numOfSteering[iTask]+1;
         recursiveSteerDepth++;
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
         actualSteerIndex = steerParent[iTask][actualSteerIndex];
         recursiveSteerDepth--;
         return true;
      }
      // group name
      if (type == 1 && !strcmp((const char*)name,"SPGroupName")) {
         xml->GetValue(steerName[iTask][actualSteerIndex],steerName[iTask][actualSteerIndex]);
         currentSteeringName = steerName[iTask][actualSteerIndex];
         if (steerParent[iTask][numOfSteering[iTask]]!= -1) {
            steerChildren[iTask][steerParent[iTask][actualSteerIndex]][numOfSteerChildren[iTask][steerParent[iTask][actualSteerIndex]]] = actualSteerIndex;
            numOfSteerChildren[iTask][steerParent[iTask][actualSteerIndex]]++;
         }
         // output
         if (makeOutput) for (i=0;i<recursiveSteerDepth;i++) cout << "   ";
         if (makeOutput) steerName[iTask][actualSteerIndex].WriteLine();
      }
      if (type == 1 && !strcmp((const char*)name,"SPGroupArraySize"))
         xml->GetValue(steerArraySize[iTask][actualSteerIndex],steerArraySize[iTask][actualSteerIndex]);
      // steering parameter field
      if (type == 1 && !strcmp((const char*)name,"SteeringParameterField")) {
         // include initialisation
         bool readName = false;
         bool readType = false;
         steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldArraySize[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "1";
         steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldCLDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // steering parameter field name
            if (type == 1 && !strcmp((const char*)name,"SPFieldName")) {
               readName = true;
               xml->GetValue(steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               // output
               if (makeOutput) for (i=0;i<recursiveSteerDepth+1;i++) cout << "   ";
               if (makeOutput) steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].WriteLine();
            }
            // steering parameter field type
            if (type == 1 && !strcmp((const char*)name,"SPFieldType")) {
               readType = true;
               xml->GetValue(steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "TString")
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "' '";
               else if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "std::string")
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "\" \"";
               else if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "TRef")
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "NULL";
               else
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "0";
            }
            // steering parameter field array size
            if (type == 1 && !strcmp((const char*)name,"SPFieldArraySize")) {
               xml->GetValue(steerFieldArraySize[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldArraySize[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
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
               xml->GetValue(steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
            }
            // steering parameter field comment
            if (type == 1 && !strcmp((const char*)name,"SPFieldComment")) {
               xml->GetValue(steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               if (steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][0]!='/') {
                  steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].Insert(0,"// ");
               }
            }
            // steering parameter command line option
            if (type == 1 && !strcmp((const char*)name,"SPFieldCommandLineOption")) {
               xml->GetValue(steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               ROMEString cloption = cloSeparator + steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] + cloSeparator;
               if( usedCLO.Contains(cloption) ){
                  cout << "Command line option \"" << steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] <<"\" is reserved by ROME or already used. Please use other word." <<endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               usedCLO += cloption;
            }
            // steering parameter command line option description
            if (type == 1 && !strcmp((const char*)name,"SPFieldCommandLineDescription")) {
               xml->GetValue(steerFieldCLDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldCLDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
            }
            // steering parameter field end
            if (type == 15 && !strcmp((const char*)name,"SteeringParameterField")){
               if(steerArraySize[iTask][actualSteerIndex].Length() && steerArraySize[iTask][actualSteerIndex]!="1" && steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].Length()){
                  cout << "Error. Command line option of field '"<< steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]] <<"' in arrayed steering parameter group '"<<steerName[iTask][actualSteerIndex]<<"' is not supported!" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               break;
            }
         }
         // check input
         if (steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]=="") {
            cout << "A steering parameter field of task '" << taskName[iTask].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // count includes
         numOfSteerFields[iTask][actualSteerIndex]++;
         if (numOfSteerFields[iTask][actualSteerIndex]>=maxNumberOfSteeringField) {
            cout << "Maximal number of steering parameter fields in task '" << taskName[iTask].Data() << "' reached : " << maxNumberOfSteeringField << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
   }
   return true;
}

bool ROMEBuilder::ReadXMLUserMakefile() {
   char *name;
   int type,ind;
   ROMEString temp;

   numOfMFDictHeaders = 0;
   numOfMFDictIncDirs = 0;
   numOfMFWinLibs = 0;
   numOfMFUnixLibs = 0;
   numOfMFIncDirs = 0;
   numOfMFPreDefs = 0;
   numOfMFSources = 0;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"DictionaryHeaders")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // header name
            if (type == 1 && !strcmp((const char*)name,"HeaderName")) {
               mfDictHeaderName[numOfMFDictHeaders] = "";
               xml->GetValue(mfDictHeaderName[numOfMFDictHeaders],mfDictHeaderName[numOfMFDictHeaders]);
               if (mfDictHeaderName[numOfMFDictHeaders].Length()>0) {
                  if (mfDictHeaderName[numOfMFDictHeaders].Index(".")==-1)
                     mfDictHeaderName[numOfMFDictHeaders].Append(".h");
                  numOfMFDictHeaders++;
               }
            }
            if (type == 15 && !strcmp((const char*)name,"DictionaryHeaders"))
               break;
         }
      }
      if (type == 1 && !strcmp((const char*)name,"DictionaryIncludesDirectories")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include directory
            if (type == 1 && !strcmp((const char*)name,"IncludeDirectory")) {
               mfDictIncDir[numOfMFDictIncDirs] = "";
               xml->GetValue(mfDictIncDir[numOfMFDictIncDirs],mfDictIncDir[numOfMFDictIncDirs]);
               if (mfDictIncDir[numOfMFDictIncDirs].Length()>0) {
                  if (mfDictIncDir[numOfMFDictIncDirs][mfDictIncDir[numOfMFDictIncDirs].Length()-1]!='/' &&
                     mfDictIncDir[numOfMFDictIncDirs][mfDictIncDir[numOfMFDictIncDirs].Length()-1]!='\\')
                     mfDictIncDir[numOfMFDictIncDirs].Append("/");
#if defined( R__VISUAL_CPLUSPLUS )
                  mfDictIncDir[numOfMFDictIncDirs].ReplaceAll("$(","%");
                  mfDictIncDir[numOfMFDictIncDirs].ReplaceAll(")","%");
#endif
                  numOfMFDictIncDirs++;
               }
            }
            if (type == 15 && !strcmp((const char*)name,"DictionaryIncludesDirectories"))
               break;
         }
      }
      if (type == 1 && !strcmp((const char*)name,"WindowsLibraries")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // library name
            if (type == 1 && !strcmp((const char*)name,"LibraryName")) {
               mfWinLibName[numOfMFWinLibs] = "";
               xml->GetValue(mfWinLibName[numOfMFWinLibs],mfWinLibName[numOfMFWinLibs]);
               if (mfWinLibName[numOfMFWinLibs].Length()>0) {
                  if (mfWinLibName[numOfMFWinLibs].Index(".")==-1)
                     mfWinLibName[numOfMFWinLibs].Append(".lib");
                  numOfMFWinLibs++;
               }
            }
            if (type == 15 && !strcmp((const char*)name,"WindowsLibraries"))
               break;
         }
      }
      if (type == 1 && !strcmp((const char*)name,"UnixLibraries")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // library name
            if (type == 1 && !strcmp((const char*)name,"LibraryName")) {
               mfUnixLibName[numOfMFUnixLibs] = "";
               xml->GetValue(mfUnixLibName[numOfMFUnixLibs],mfUnixLibName[numOfMFUnixLibs]);
               if (mfUnixLibName[numOfMFUnixLibs].Length()>0)
                  numOfMFUnixLibs++;
            }
            if (type == 15 && !strcmp((const char*)name,"UnixLibraries"))
               break;
         }
      }
      if (type == 1 && !strcmp((const char*)name,"IncludeDirectories")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include directory
            if (type == 1 && !strcmp((const char*)name,"IncludeDirectory")) {
               mfIncDir[numOfMFIncDirs] = "";
               xml->GetValue(mfIncDir[numOfMFIncDirs],mfIncDir[numOfMFIncDirs]);
               if (mfIncDir[numOfMFIncDirs].Length()>0) {
                  if (mfIncDir[numOfMFIncDirs][mfIncDir[numOfMFIncDirs].Length()-1]!='/' &&
                     mfIncDir[numOfMFIncDirs][mfIncDir[numOfMFIncDirs].Length()-1]!='\\')
                     mfIncDir[numOfMFIncDirs].Append("/");
                  numOfMFIncDirs++;
               }
            }
            if (type == 15 && !strcmp((const char*)name,"IncludeDirectories"))
               break;
         }
      }
      if (type == 1 && !strcmp((const char*)name,"PreprocessorDefinition")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // name
            if (type == 1 && !strcmp((const char*)name,"Name")) {
               mfPreDefName[numOfMFPreDefs] = "";
               xml->GetValue(mfPreDefName[numOfMFPreDefs],mfPreDefName[numOfMFPreDefs]);
               if (mfPreDefName[numOfMFPreDefs].Length()>0)
                  numOfMFPreDefs++;
            }
            if (type == 15 && !strcmp((const char*)name,"PreprocessorDefinition"))
               break;
         }
      }
      if (type == 1 && !strcmp((const char*)name,"AdditionalSourceFiles")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // source file
            if (type == 1 && !strcmp((const char*)name,"SourceFile")) {
               numOfMFSourceFlags[numOfMFSources] = 0;
               mfSourceFileName[numOfMFSources] = "";
               mfSourceFilePath[numOfMFSources] = "";
               mfSourceFileDep[numOfMFSources] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // file name
                  if (type == 1 && !strcmp((const char*)name,"FileName")) {
                     xml->GetValue(mfSourceFileName[numOfMFSources],mfSourceFileName[numOfMFSources]);
                     if ((ind=mfSourceFileName[numOfMFSources].Index("."))!=-1) {
                        mfSourceFileType[numOfMFSources] = mfSourceFileName[numOfMFSources](ind+1,mfSourceFileName[numOfMFSources].Length());
                        mfSourceFileName[numOfMFSources] = mfSourceFileName[numOfMFSources](0,ind);
                     }
                     else
                        mfSourceFileType[numOfMFSources] = "cpp";
                     mfSourceFileType[numOfMFSources].ToLower();
                  }
                  // file path
                  if (type == 1 && !strcmp((const char*)name,"FilePath")) {
                     xml->GetValue(mfSourceFilePath[numOfMFSources],mfSourceFilePath[numOfMFSources]);
                     if (mfSourceFilePath[numOfMFSources].Length()>0) {
                        if (mfSourceFilePath[numOfMFSources][mfSourceFilePath[numOfMFSources].Length()-1]!='/' ||
                           mfSourceFilePath[numOfMFSources][mfSourceFilePath[numOfMFSources].Length()-1]!='\\')
                           mfSourceFilePath[numOfMFSources].Append("/");
                     }
                  }
                  // dependencies
                  if (type == 1 && !strcmp((const char*)name,"Dependencies"))
                     xml->GetValue(mfSourceFileDep[numOfMFSources],mfSourceFileDep[numOfMFSources]);
                  // flags
                  if (type == 1 && !strcmp((const char*)name,"NeededFlag")) {
                     mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]] = "";
                     xml->GetValue(mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]],mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]]);
                     numOfMFSourceFlags[numOfMFSources]++;
                  }   
                  if (type == 15 && !strcmp((const char*)name,"SourceFile")) {
                     numOfMFSources++;
                     break;
                  }
               }
            }
            if (type == 15 && !strcmp((const char*)name,"AdditionalSourceFiles"))
               break;
         }
      }

      if (type == 15 && !strcmp((const char*)name,"UserMakeFile")) {
         break;
      }
   }
   return true;
}
bool ROMEBuilder::WriteSteering(int iTask) {
   ROMEString hFile;
   ROMEString buffer;

   hFile.SetFormatted("%s/include/framework/%sGlobalSteering.h",outDir.Data(),shortCut.Data());

   if (numOfSteering[numOfTaskHierarchy]==-1) {
      remove(hFile.Data());
      return true;
   }

   if (makeOutput) cout << "\n   Output Files:" << endl;

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sGlobalSteering_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sGlobalSteering_H\n\n",shortCut.Data());
   buffer.AppendFormatted("#include <string>\n\n");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());

   WriteSteeringClass(buffer,0,iTask,0);

   buffer.AppendFormatted("#endif   // %sGlobalSteering_H\n",shortCut.Data());

   //Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}
bool ROMEBuilder::WriteAnalyzerCpp() {
   int i,j;

   ROMEString cppFile;
   ROMEString buffer;

   ROMEString parentt;
   ROMEString buf;
   ROMEString str;
   ROMEString pointer;

   if (makeOutput) cout << "\n   Output Cpp-File:" << endl;
   ROMEString classDescription;
   classDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut.Data(),mainProgName.Data());

   ROMEString tmp;
   ROMEString format;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
         if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
      }
   }


   // File name
   cppFile.SetFormatted("%s/src/framework/%sAnalyzer.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#include <direct.h>\n");
   buffer.AppendFormatted("#endif\n");

   buffer.AppendFormatted("#include <sys/stat.h>\n");
   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
   buffer.AppendFormatted("#include <TObjString.h>\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <ROMEStr2DArray.h>\n");
   analyzerDep.AppendFormatted(" $(ROMESYS)/include/ROMEStr2DArray.h");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[i].Data());
   }
   analyzerDep.AppendFormatted(" $(TaskIncludes)");
   buffer.AppendFormatted("#include <include/framework/%sConfig.h>\n",shortCut.Data());
   analyzerDep.AppendFormatted(" include/framework/%sConfig.h",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sEventLoop.h>\n",shortCut.Data());
   analyzerDep.AppendFormatted(" include/framework/%sEventLoop.h",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   analyzerDep.AppendFormatted(" include/framework/%sAnalyzer.h",shortCut.Data());

   buffer.AppendFormatted("#include <Riostream.h>\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sAnalyzer);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sAnalyzer *gAnalyzer;  // global Analyzer Handle\n",shortCut.Data());

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
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
   buffer.AppendFormatted("%sAnalyzer::%sAnalyzer(TApplication *app):ROMEAnalyzer(app) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("// Folder and Task initialisation\n");
   buffer.AppendFormatted("   int i=0;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gPassToROME = (void*)this; // Pass the handle to the framework\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fConfiguration = new %sConfig();\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fMidasDAQ = NULL;\n");
   buffer.AppendFormatted("   fRomeDAQ = NULL;\n");
   for (i=0;i<numOfDAQ;i++)
      buffer.AppendFormatted("   f%s = NULL;\n",daqName[i].Data());
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTaskHierarchy]>0) {
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
         format.SetFormatted("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-folderName[i].Length());
         buffer.AppendFormatted(format.Data(),folderName[i].Data(),"",parentt.Data(),folderName[i].Data(),folderTitle[i].Data());
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
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
   buffer.AppendFormatted("\n");

   // Task
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   fMainTask = new %sEventLoop(\"mainTask\",\"Main Task of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   fMainFolder->Add(fMainTask);\n");
   buffer.AppendFormatted("   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   int taskLen=0;
   int suffixNumber;
   int multiplicity;
   ROMEString suffix;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (taskLen<(int)taskHierarchyName[i].Length()) taskLen = taskHierarchyName[i].Length();
   }

   buffer.AppendFormatted("   fMainHistoFolder = fMainFolder->AddFolder(\"histos\",\"Histogram Folder\");\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      // create suffix
      suffixNumber = 0;
      multiplicity = 0;
      for (j=0;j<numOfTaskHierarchy;j++) {
         if (j!=i && taskHierarchyName[i]==taskHierarchyName[j]) {
            multiplicity++;
            if (j<i)
               suffixNumber++;
         }
      }
      if (multiplicity>0) {
         if (multiplicity<=9)
            suffix.SetFormatted("_%01d",suffixNumber);
         if (multiplicity>9)
            suffix.SetFormatted("_%02d",suffixNumber);
         if (multiplicity>99)
            suffix.SetFormatted("_%03d",suffixNumber);
      }
      else
         suffix = "";
      // create histo folder
      if (taskHierarchyParentIndex[i]==-1)
         buffer.AppendFormatted("   fHistoFolders->AddAtAndExpand(fMainHistoFolder->AddFolder(\"%sHistos%s\",\"Histograms of Task '%s%s'\"),%d);\n",taskHierarchyName[i].Data(),suffix.Data(),taskHierarchyName[i].Data(),suffix.Data(),i);
      else
         buffer.AppendFormatted("   fHistoFolders->AddAtAndExpand(GetHistoFolderAt(%d)->AddFolder(\"%sHistos%s\",\"Histograms of Task '%s%s'\"),%d);\n",taskHierarchyParentIndex[i],taskHierarchyName[i].Data(),suffix.Data(),taskHierarchyName[i].Data(),suffix.Data(),i);
      // create task
      buffer.AppendFormatted("   f%s%03dTask = new %sT%s(\"%s\",\"%s\",\"%s\",GetHistoFolderAt(%d));\n",taskHierarchyName[i].Data(),i,shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),"",suffix.Data(),i);
      buffer.AppendFormatted("   ((%sT%s*)f%s%03dTask)->SetActive(false);\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),i);
      if (taskHierarchyParentIndex[i]==-1)
         parentt = "GetMainTask()";
      else
         parentt.SetFormatted("f%s%03dTask",taskHierarchyName[taskHierarchyParentIndex[i]].Data(),taskHierarchyParentIndex[i]);
      buffer.AppendFormatted("   %s->Add(f%s%03dTask);\n",parentt.Data(),taskHierarchyName[i].Data(),i);
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

   // Database Folder Fields
   buffer.AppendFormatted("   // Database Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if( valueDimension[i][j]>1 )
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

   int ndb = 0;
   for (i=0;i<numOfFolder;i++) if (folderDataBase[i]) ndb++;

   // Read each folders
   for (i=0;i<numOfFolder;i++){
      if (folderDataBase[i] && !folderSupport[i]){
         buffer.AppendFormatted("bool %sAnalyzer::Read%s() {\n",shortCut.Data(),folderName[i].Data());
         WriteReadDataBaseFolder(buffer,i,folderArray[i]=="1" ? 1 : 2);
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n\n");
      }
   }

   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("bool %sAnalyzer::ReadSingleDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      for (i=0;i<numOfFolder;i++){
         if (folderDataBase[i] && folderArray[i]=="1" && !folderSupport[i]){
            buffer.AppendFormatted("   if(!Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return false;\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // ReadArrayDataBaseFolders
   buffer.AppendFormatted("bool %sAnalyzer::ReadArrayDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      for (i=0;i<numOfFolder;i++){
         if (folderDataBase[i] && folderArray[i]!="1" && !folderSupport[i]){
            buffer.AppendFormatted("   if(!Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return false;\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // ReadUserParameter
   buffer.AppendFormatted("bool %sAnalyzer::ReadUserParameter(const char* opt, const char* value, int& i) {\n",shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   ROMEString option = opt;\n");
   buffer.AppendFormatted("   ROMEString tmp;\n");
   buffer.AppendFormatted("   char *cstop=NULL;\n");
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTaskHierarchy]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
      WriteSteeringReadParameters(buffer,0,numOfTaskHierarchy,pointerT,steerPointerT);
   }
   // Task steering parameter
   for (i=0;i<numOfTaskHierarchy;i++) {
      buffer.AppendFormatted("   // %s task\n",taskHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTask",taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->Get%s%03dTask())->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i);
         WriteSteeringReadParameters(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT);
      }
   }
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n\n");

   // UserParameterUsage
   buffer.AppendFormatted("void %sAnalyzer::UserParameterUsage() {\n",shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTaskHierarchy]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
      WriteSteeringParameterUsage(buffer,0,numOfTaskHierarchy,pointerT,steerPointerT);
   }
   // Task steering parameter
   for (i=0;i<numOfTaskHierarchy;i++) {
      buffer.AppendFormatted("   // %s task\n",taskHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTask",taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->Get%s%03dTask())->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i);
         WriteSteeringParameterUsage(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT);
      }
   }
   buffer.AppendFormatted("}\n\n");


/*   // WriteDataBaseFolders
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("void %sAnalyzer::Write%sDataBase() {\n",shortCut.Data(),folderName[i].Data());
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
            buffer.AppendFormatted("   path.SetFormatted(gAnalyzer->Get%s_%sDBPath()",folderName[i].Data(),valueName[i][j].Data());
            for (k=0;k<maxNumberOfPathObjectInterpreterCodes;k++)
               buffer.AppendFormatted(",gAnalyzer->GetObjectInterpreterCharValue(gAnalyzer->Get%s_%sDBCodeAt(%d),buffer[%d],buffer[%d]).Data()",folderName[i].Data(),valueName[i][j].Data(),k,k,k);
            buffer.AppendFormatted(");\n");
            buffer.AppendFormatted("   if (!this->GetDataBase(gAnalyzer->Get%s_%sDBIndex())->Write(values,path,gAnalyzer->GetCurrentRunNumber())) {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      gAnalyzer->Println(\"   in Folder '%s' Value '%s'.\");\n",folderName[i].Data(),valueName[i][j].Data());
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
   buffer.AppendFormatted("int %sAnalyzer::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString path = objectPath;\n");
   buffer.AppendFormatted("   if (path.Index(\"/\")!=0)\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   if (path.Index(\"/GSP\")==0) {\n");
   ROMEString path = "/GSP";
   codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber,0,numOfTaskHierarchy,path,1);
   buffer.AppendFormatted("      gAnalyzer->Println(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
   buffer.AppendFormatted("      gAnalyzer->Println(\"   \"+path+\"\\n\");\n");
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
   buffer.AppendFormatted("   gAnalyzer->Println(\"\\nWrong path in configuration file.\\n\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"   \"+path+\"\\n\");\n");
   buffer.AppendFormatted("   return -1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   fNumberOfInterpreterCodes = codeNumber;

   // Get Object Interpreter Values
   WriteObjectInterpreterValue(buffer,"int","Int");
   WriteObjectInterpreterValue(buffer,"double","Double");
   WriteObjectInterpreterValue(buffer,"ROMEString&","Char");

   // Splash Screen
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("LPDWORD lpThreadId;\n");
   buffer.AppendFormatted("void %sAnalyzer::startSplashScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   CloseHandle(CreateThread(NULL,1024,&SplashThread,0,0,lpThreadId));\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#if defined( R__UNIX )\n");
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
   buffer.AppendFormatted("   gAnalyzer->Println(\"*****************************************\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Print(\"*\");\n");
   int len1 = (int)((39-(double)prog.Length())/2+0.5);
   int len2 = (int)((39-(double)prog.Length())/2);
   buffer.AppendFormatted("   for (i=0;i<%d;i++) gAnalyzer->Print(\" \");\n",len1);
   buffer.AppendFormatted("   gAnalyzer->Print(\"%s\");\n",prog.Data());
   buffer.AppendFormatted("   for (i=0;i<%d;i++) gAnalyzer->Print(\" \");\n",len2);
   buffer.AppendFormatted("   gAnalyzer->Println(\"*\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*   generated by the ROME Environment   *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*             %s              *\");\n",romeVersion.Data());
   buffer.AppendFormatted("   gAnalyzer->Println(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->Println(\"*****************************************\\n\");\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("   \n");

   // Redirect Output
   buffer.AppendFormatted("void %sAnalyzer::redirectOutput() {\n",shortCut.Data());
   buffer.AppendFormatted("   ofstream *romeOutputFile;\n");
   buffer.AppendFormatted("   romeOutputFile = new ofstream(\"romeOutput.txt\");\n");
   buffer.AppendFormatted("   cout.rdbuf(romeOutputFile->rdbuf());\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   if (haveFortranTask) {
      buffer.AppendFormatted("// Interface to Fortran Tasks\n");
      for (i=0;i<numOfFolder;i++) {
         if (!folderSupport[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (!isNumber(valueType[i][j].Data()))
                  continue;
               if (folderArray[i]=="1") {
                  if (valueArray[i][j][0]=="1") {
                     buffer.AppendFormatted("extern \"C\" float Get%s_Get%s_C() {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%s()->Get%s();\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
                  else {
                     buffer.AppendFormatted("extern \"C\" float Get%s_Get%sAt_C(int j) {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%s()->Get%sAt(j);\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
               }
               else {
                  if (valueArray[i][j][0]=="1") {
                     buffer.AppendFormatted("extern \"C\" float Get%sAt_Get%s_C(int i) {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%sAt(i)->Get%s();\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
                  else {
                     buffer.AppendFormatted("extern \"C\" float Get%sAt_Get%sAt_C(int i,int j) {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%sAt(j)->Get%sAt(j);\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
               }
            }
         }
      }
   }

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

bool ROMEBuilder::WriteAnalyzerH() {
   int i,j;

   ROMEString hFile;
   ROMEString buffer;

   ROMEString bankname;
   ROMEString format;
   if (makeOutput) cout << "\n   Output H-File:" << endl;
   // max folder name length
   int nameLen = -1;
   int fieldLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (typeLen<(int)folderName[i].Length()+scl)
            typeLen = folderName[i].Length()+scl;
         if (nameLen<(int)folderName[i].Length())
            nameLen = folderName[i].Length();
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (fieldLen<(int)valueName[i][j].Length())
                  fieldLen = valueName[i][j].Length();
            }
         }
      }
   }
   // max task name length
   int taskLen=0;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (taskLen<(int)taskHierarchyName[i].Length()) taskLen = taskHierarchyName[i].Length();
   }
   // max tree name length
   int treeLen=0;
   for (i=0;i<numOfTree;i++) {
      if (treeLen<(int)treeName[i].Length()) treeLen = treeName[i].Length();
   }

   // max task switch name length
   int switchLen = -1;
   ROMEString switchString;
   for (i=0;i<numOfTaskHierarchy;i++) {
      int index = taskHierarchyParentIndex[i];
      switchString = taskHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      if (switchLen<(int)switchString.Length()) switchLen = switchString.Length();
   }

   // File name
   hFile.SetFormatted("%s/include/framework/%sAnalyzer.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sAnalyzer_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sAnalyzer_H\n\n",shortCut.Data());

   // includes
   buffer.AppendFormatted("#if defined( R__UNIX )\n");
   buffer.AppendFormatted("#include <unistd.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <TRint.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#include <TTree.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <ROMETask.h>\n");
   buffer.AppendFormatted("#include <ROMEAnalyzer.h>\n");

   // DAQ includes
   buffer.AppendFormatted("#include <include/framework/%sMidasDAQ.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sRomeDAQ.h>\n",shortCut.Data());
   if (this->orca)
      buffer.AppendFormatted("#include <ROMEOrcaDAQ.h>\n",shortCut.Data());
   for (i=0;i<numOfDAQ;i++)
      buffer.AppendFormatted("#include <include/framework/%s%s.h>\n",shortCut.Data(),daqName[i].Data());

   // Folder includes
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         buffer.AppendFormatted("#include <include/framework/%s%s.h>\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Task Switches Structure
   buffer.AppendFormatted("// Task Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      int index = taskHierarchyParentIndex[i];
      switchString = taskHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      format.SetFormatted("   int %%s;%%%ds  //! %%s Task\n",switchLen-switchString.Length());
      buffer.AppendFormatted(format.Data(),switchString.Data(),"",switchString.Data());
//      buffer.AppendFormatted("   int %s;   //! %s Task\n",switchString.Data(),switchString.Data());
   }
   buffer.AppendFormatted("} TaskSwitches;\n");

   // Class
   buffer.AppendFormatted("\nclass %sAnalyzer : public ROMEAnalyzer\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("friend class %sEventLoop;\n",shortCut.Data());
   buffer.AppendFormatted("friend class %sConfig;\n",shortCut.Data());
   buffer.AppendFormatted("friend class %sGlobalSteering;\n",shortCut.Data());
   // Fields
   buffer.AppendFormatted("protected:\n");

   // Folder Fields
   buffer.AppendFormatted("   // Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (folderArray[i]=="1") {
               format.SetFormatted("   %%s%%s*%%%ds f%%sFolder; %%%ds // Handle to %%s%%s Folder\n",typeLen-folderName[i].Length()-scl,nameLen-folderName[i].Length());
               buffer.AppendFormatted(format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
            }
            else {
               format.SetFormatted("   TClonesArray*%%%ds f%%sFolders;%%%ds // Handle to %%s%%s Folders\n",typeLen-12,nameLen-folderName[i].Length());
               buffer.AppendFormatted(format.Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
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
            format.SetFormatted("   ROMEString f%%s_%%sDBName;%%%ds // Name of the database from which to read the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
            format.SetFormatted("   ROMEString f%%s_%%sDBPath; %%%ds // Database path to the value of the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
            format.SetFormatted("   TArrayI   *f%%s_%%sDBCode; %%%ds // Object Interpreter codes for database path to the value of the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task Fields
   buffer.AppendFormatted("   // Task Fields\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      format.SetFormatted("   ROMETask* f%%s%%03dTask;%%%ds  // Handle to %%s Task\n",taskLen-taskHierarchyName[i].Length());
      buffer.AppendFormatted(format.Data(),taskHierarchyName[i].Data(),i,"",taskHierarchyName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Steering Parameter Fields
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("   // Steering Parameter Fields\n");
      buffer.AppendFormatted("   %sGlobalSteering* fGlobalSteeringParameters; // Handle to the GlobalSteering Class\n",shortCut.Data());
   }
   buffer.AppendFormatted("\n");

   // DAQ Handle
   buffer.AppendFormatted("   // DAQ Handle\n");
   buffer.AppendFormatted("   %sMidasDAQ* fMidasDAQ; // Handle to the Midas DAQ Class\n",shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ*  fRomeDAQ; // Handle to the Rome DAQ Class\n",shortCut.Data());
   if (this->orca)
      buffer.AppendFormatted("   ROMEOrcaDAQ* fOrcaDAQ; // Handle to the Orca DAQ Class\n",shortCut.Data());
   for (i=0;i<numOfDAQ;i++)
      buffer.AppendFormatted("   %s%s*  f%s; // Handle to the %s DAQ Class\n",shortCut.Data(),daqName[i].Data(),daqName[i].Data(),daqName[i].Data());
   buffer.AppendFormatted("\n");

   // Task Switches
   buffer.AppendFormatted("   // Task Switches\n");
   buffer.AppendFormatted("   TaskSwitches fTaskSwitches;               //! Task Switches\n");
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sAnalyzer(TApplication *app);\n",shortCut.Data());
   // Folder Getters
   buffer.AppendFormatted("   // Folders\n");
   for(i=0;i<numOfFolder;i++)
      WriteFolderGetter(buffer,i,scl,nameLen,typeLen);
   buffer.AppendFormatted("\n");
   // Set size
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i]!="1"){
         if(folderArray[i]!="variable")
            buffer.AppendFormatted("private:\n");
         buffer.AppendFormatted("   void Set%sSize(int number) {\n",folderName[i].Data());
         buffer.AppendFormatted("      int i;\n");
         buffer.AppendFormatted("      if(f%sFolders) f%sFolders->Delete();\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("      for (i=0;i<number;i++) {\n");
         buffer.AppendFormatted("         new((*f%sFolders)[i]) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if (isFolder(valueType[i][j].Data()))
               continue;
            if(isRootClassType(valueType[i][j].Data()) && !isPointerType(valueType[i][j].Data())
               && !valueType[i][j].Contains("TRef") && !valueType[i][j].Contains("TString"))
            continue;
            if (valueDimension[i][j]==0)
               buffer.AppendFormatted("(%s)%s,",valueType[i][j].Data(),valueInit[i][j].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" );\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         if(folderArray[i]!="variable")
            buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   Int_t Get%sSize() { return f%sFolders->GetEntries(); }\n",folderName[i].Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   // Tree Getters
   buffer.AppendFormatted("   // Trees\n");
   for (i=0;i<numOfTree;i++) {
      format.SetFormatted("   TTree* Get%%sTree()%%%ds { return ((ROMETree*)fTreeObjects->At(%%d))->GetTree(); };\n",treeLen-treeName[i].Length());
      buffer.AppendFormatted(format.Data(),treeName[i].Data(),"",i);
   }
   buffer.AppendFormatted("\n");

   // Database Getters
   buffer.AppendFormatted("   // Database\n");
   buffer.AppendFormatted("   ROMEDataBase* GetXMLDataBase() { return GetDataBase(\"XML\"); };\n");
   buffer.AppendFormatted("   ROMEDataBase* GetTextDataBase() { return GetDataBase(\"TEXT\"); };\n");
   buffer.AppendFormatted("   ROMEDataBase* GetODBDataBase() { return GetDataBase(\"ODB\"); };\n");
   buffer.AppendFormatted("#if defined( HAVE_SQL )\n");
   buffer.AppendFormatted("   ROMEDataBase* GetSQLDataBase() { return GetDataBase(\"SQL\"); };\n");
   buffer.AppendFormatted("#endif\n");
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
   buffer.AppendFormatted("\n");

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

   // Task Getters
   buffer.AppendFormatted("   // Tasks\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      format.SetFormatted("   ROMETask* Get%%s%%03dTask()%%%ds { return f%%s%%03dTask;%%%ds };\n",taskLen-taskHierarchyName[i].Length(),taskLen-taskHierarchyName[i].Length());
      buffer.AppendFormatted(format.Data(),taskHierarchyName[i].Data(),i,"",taskHierarchyName[i].Data(),i,"");
   }


   // Task Switches
   buffer.AppendFormatted("   // Task Switches\n");
   buffer.AppendFormatted("   TaskSwitches* GetTaskSwitches() { return &fTaskSwitches; };\n");
   buffer.AppendFormatted("\n");

   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   // Read each folders
   for (i=0;i<numOfFolder;i++){
      if (folderDataBase[i] && !folderSupport[i])
         buffer.AppendFormatted("   bool Read%s();\n",folderName[i].Data());
   }
   buffer.AppendFormatted("   bool ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   bool ReadArrayDataBaseFolders();\n");
   buffer.AppendFormatted("\n");

/*   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("   void Write%sDataBase();\n",folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");
*/
   // Object Interpreter
   buffer.AppendFormatted("   // Object Interpreter\n");
   buffer.AppendFormatted("   int         GetObjectInterpreterCode(const char* objectPath);\n");
   buffer.AppendFormatted("   int         GetObjectInterpreterIntValue(int code,int defaultValue);\n");
   buffer.AppendFormatted("   double      GetObjectInterpreterDoubleValue(int code,double defaultValue);\n");
   buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(int code,ROMEString& defaultValue,ROMEString& buffer);\n");
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // DAQ Access Methods
   buffer.AppendFormatted("   // DAQ Access Methods\n");
   buffer.AppendFormatted("   %sMidasDAQ* GetMidas() { return GetMidasDAQ(); };\n",shortCut.Data());
   buffer.AppendFormatted("   %sMidasDAQ* GetMidasDAQ() {\n",shortCut.Data());
   buffer.AppendFormatted("      if (fMidasDAQ==NULL) {\n");
   buffer.AppendFormatted("         this->Println(\"\\nYou have tried to access the midas DAQ system over a gAnalyzer->GetMidasDAQ()\\nhandle but the current DAQ system is not 'Midas'.\\n\\nShutting down the program.\\n\");\n");
   buffer.AppendFormatted("         ((TRint*)fApplication)->Terminate(1);\n");
   buffer.AppendFormatted("         return NULL;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      return fMidasDAQ;\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("   void     SetMidasDAQ(%sMidasDAQ* handle) { fMidasDAQ = handle; };\n",shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ*  GetRome() { return GetRomeDAQ(); };\n",shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ*  GetRomeDAQ() {\n",shortCut.Data());
   buffer.AppendFormatted("      if (fRomeDAQ==NULL) {\n");
   buffer.AppendFormatted("         this->Println(\"\\nYou have tried to access the rome DAQ system over a gAnalyzer->GetRomeDAQ()\\nhandle but the current DAQ system is not 'Rome'.\\n\\nShutting down the program.\\n\");\n");
   buffer.AppendFormatted("         ((TRint*)fApplication)->Terminate(1);\n");
   buffer.AppendFormatted("         return NULL;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      return fRomeDAQ;\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("   void     SetRomeDAQ (%sRomeDAQ*  handle) { fRomeDAQ  = handle; };\n",shortCut.Data());
   if (this->orca) {
      buffer.AppendFormatted("   ROMEOrcaDAQ*  GetOrcaDAQ() {\n");
      buffer.AppendFormatted("      if (fOrcaDAQ==NULL) {\n");
      buffer.AppendFormatted("         this->Println(\"\\nYou have tried to access the orca DAQ system over a gAnalyzer->GetOrcaDAQ()\\nhandle but the current DAQ system is not 'Orca'.\\n\\nShutting down the program.\\n\");\n");
      buffer.AppendFormatted("         ((TRint*)fApplication)->Terminate(1);\n");
      buffer.AppendFormatted("         return NULL;\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("      return fOrcaDAQ;\n");
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("   void     SetOrcaDAQ (ROMEOrcaDAQ*  handle) { fOrcaDAQ  = handle; };\n");
   }
   for (i=0;i<numOfDAQ;i++) {
      buffer.AppendFormatted("   %s%s*  Get%s()                 { return f%s;    };\n",shortCut.Data(),daqName[i].Data(),daqName[i].Data(),daqName[i].Data());
      buffer.AppendFormatted("   void     Set%s (%s%s*  handle) { f%s  = handle; };\n",daqName[i].Data(),shortCut.Data(),daqName[i].Data(),daqName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   bool ReadUserParameter(const char* opt, const char *value, int& i);\n");
   buffer.AppendFormatted("   void UserParameterUsage();\n");
   buffer.AppendFormatted("   void startSplashScreen();\n");
   buffer.AppendFormatted("   void consoleStartScreen();\n");
   buffer.AppendFormatted("   void redirectOutput();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sAnalyzer,0);\n",shortCut.Data());

   // Footer
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("extern %sAnalyzer *gAnalyzer;  // global Analyzer Handle\n\n",shortCut.Data());

   buffer.AppendFormatted("#endif   // %sAnalyzer_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

bool ROMEBuilder::WriteAnalyzerF() {
   int i,j;

   ROMEString fFile;
   ROMEString buffer;

   bool writeFortran = false;
   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         writeFortran = true;
         break;
      }
   }
   if (!writeFortran)
      return true;

   // File name
   fFile.SetFormatted("%s/src/framework/%sFAnalyzer.f",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);

   // Folder Fields
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (!isNumber(valueType[i][j].Data()))
               continue;
            if (folderArray[i]=="1") {
               if (valueArray[i][j][0]=="1") {
                  buffer.AppendFormatted("      real function Get%s_Get%s\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%s_Get%s_C\n     &      [C, ALIAS:'_Get%s_Get%s_C'] ()\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%s_Get%s = Get%s_Get%s_C()\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%s_Set%s(value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
               else {
                  buffer.AppendFormatted("      real function Get%s_Get%sAt(j)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%s_Get%sAt_C\n     &      [C, ALIAS:'_Get%s_Get%sAt_C'] (j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("            integer j[value]\n");
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%s_Get%sAt = Get%s_Get%sAt_C(j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%s_Set%sAt(j,value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
            }
            else {
               if (valueArray[i][j][0]=="1") {
                  buffer.AppendFormatted("      real function Get%sAt_Get%s(i)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%sAt_Get%s_C\n     &      [C, ALIAS:'_Get%sAt_Get%s_C'] (i)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("            integer i[value]\n");
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%sAt_Get%s = Get%sAt_Get%s_C(i)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%sAt_Set%s(i,value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
               else {
                  buffer.AppendFormatted("      real function Get%sAt_Get%sAt(i,j)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%sAt_Get%sAt_C\n     &      [C, ALIAS:'_Get%sAt_Get%sAt_C'] (i,j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("            integer i[value]\n");
                  buffer.AppendFormatted("            integer j[value]\n");
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%sAt_Get%sAt = Get%sAt_Get%sAt_C(i,j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%sAt_Set%sAt(i,j,value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(fFile.Data(),buffer.Data(),6);

   return true;
}

bool ROMEBuilder::WriteConfigCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;

   int j;
   ROMEString format;

   ROMEString classDescription;
   classDescription.SetFormatted("This class handles the framework configuration file (romeConfig.xml).");

   // File name
   cppFile.SetFormatted("%s/src/framework/%sConfig.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("#include <ROMEString.h>\n");
   configDep.AppendFormatted(" $(ROMESYS)/include/ROMEString.h");

   buffer.AppendFormatted("#include <ROMEXMLDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMETextDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMENoDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMEODBOfflineDataBase.h>\n");
   buffer.AppendFormatted("#include <ROMEODBOnlineDataBase.h>\n");
   buffer.AppendFormatted("#if defined( HAVE_SQL )\n");
   buffer.AppendFormatted("#include <ROMESQLDataBase.h>\n");
   buffer.AppendFormatted("#endif\n");
   for (i=0;i<numOfDB;i++)
      buffer.AppendFormatted("#include <include/framework/%s%sDataBase.h>\n",shortCut.Data(),dbName[i].Data());
   configDep.AppendFormatted(" $(DataBaseIncludes)");

   buffer.AppendFormatted("#include <include/framework/%sMidasDAQ.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sRomeDAQ.h>\n",shortCut.Data());
   buffer.AppendFormatted("#if defined( HAVE_ORCA )\n");
   buffer.AppendFormatted("#include <ROMEOrcaDAQ.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <ROMENoDAQSystem.h>\n");
   for (i=0;i<numOfDAQ;i++)
      buffer.AppendFormatted("#include <include/framework/%s%s.h>\n",shortCut.Data(),daqName[i].Data());
   configDep.AppendFormatted(" $(DAQIncludes)");

   for (i=0;i<numOfTask;i++)
      buffer.AppendFormatted("#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[i].Data());
   configDep.AppendFormatted(" $(TaskIncludes)");

   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   configDep.AppendFormatted(" include/framework/%sAnalyzer.h",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sConfig.h>\n",shortCut.Data());
   configDep.AppendFormatted(" include/framework/%sConfig.h",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sConfig::%sConfig() {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   fConfigData = new ConfigData*[1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = 0;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Configuration File
   buffer.AppendFormatted("\n// Read Configuration File\n");
   buffer.AppendFormatted("bool %sConfig::ReadConfigurationFile(const char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   xml->OpenFileForPath(fXMLFile);\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = xml->NumberOfOccurrenceOfPath(\"/Configuration/RunConfiguration\");\n");
   buffer.AppendFormatted("   delete [] fConfigData;\n");
   buffer.AppendFormatted("   fConfigData = new ConfigData*[fNumberOfRunConfigs+1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   ROMEString path = \"/Configuration/MainConfiguration\";\n");
   buffer.AppendFormatted("   ReadConfiguration(xml,path,0);\n");
   buffer.AppendFormatted("   if (!SetConfiguration(0,0))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   for (int i=0;i<fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      fConfigData[i+1] = new ConfigData();\n");
   buffer.AppendFormatted("      path.SetFormatted(\"/Configuration/RunConfiguration[%%d]\",i+1);\n");
   buffer.AppendFormatted("      ReadConfiguration(xml,path,i+1);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[0]->fRunNumbersModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetRunNumbers(fConfigData[0]->fRunNumbers);\n");
   buffer.AppendFormatted("   if (fConfigData[0]->fInputFileNamesModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetInputFileNames(fConfigData[0]->fInputFileNames);\n");

   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Configuration
   buffer.AppendFormatted("\n// Read Configuration\n");
   buffer.AppendFormatted("bool %sConfig::ReadConfiguration(ROMEXML *xml,ROMEString& path,int index) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   ROMEString buf = \"\";\n");
   // RunNumbers
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/RunNumbers\",fConfigData[index]->fRunNumbers,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fRunNumbers==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fRunNumbersModified = false;\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      fConfigData[index]->fRunNumbersModified = true;\n");
   buffer.AppendFormatted("      fConfigData[index]->fLastRunNumberIndex = 0;\n");
   buffer.AppendFormatted("      gAnalyzer->DecodeRunNumbers(fConfigData[index]->fRunNumbers,fConfigData[index]->fRunNumberArray);\n");
   buffer.AppendFormatted("   }\n");

   // EventNumbers
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/EventNumbers\",fConfigData[index]->fEventNumbers,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fEventNumbers==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fEventNumbersModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fEventNumbersModified = true;\n");
   // InputFileNames
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/InputFileNames\",fConfigData[index]->fInputFileNames,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fInputFileNames==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fInputFileNamesModified = false;\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      fConfigData[index]->fInputFileNamesModified = true;\n");
   buffer.AppendFormatted("      fConfigData[index]->fLastInputFileNameIndex = 0;\n");
   buffer.AppendFormatted("      gAnalyzer->DecodeInputFileNames(fConfigData[index]->fInputFileNames,fConfigData[index]->fInputFileNameArray);\n");
   buffer.AppendFormatted("   }\n");
   // modes
   buffer.AppendFormatted("   // modes\n");
   // AnalyzingMode
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/AnalyzingMode\",fConfigData[index]->fModes->fAnalyzingMode,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fModes->fAnalyzingMode.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fAnalyzingMode==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fAnalyzingModeModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fAnalyzingModeModified = true;\n");
   // DAQSystem
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/DAQSystem\",fConfigData[index]->fModes->fDAQSystem,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fModes->fDAQSystem.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fDAQSystem==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fDAQSystemModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fDAQSystemModified = true;\n");
   // BatchMode
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/BatchMode\",fConfigData[index]->fModes->fBatchMode,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fBatchMode==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fBatchModeModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fBatchModeModified = true;\n");
   // ShowSplashScreen
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/ShowSplashScreen\",fConfigData[index]->fModes->fShowSplashScreen,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fShowSplashScreen==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fShowSplashScreenModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fShowSplashScreenModified = true;\n");
   // Check Modified
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fAnalyzingModeModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fModes->fDAQSystemModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fModes->fBatchModeModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fModes->fShowSplashScreenModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fModesModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModesModified = false;\n");
   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (index==0) {\n");
   buffer.AppendFormatted("      int size = xml->NumberOfOccurrenceOfPath(path+\"/DataBases/DataBase\");\n");
   buffer.AppendFormatted("      gAnalyzer->InitDataBases(size);\n");
   buffer.AppendFormatted("      ROMEString dataBasePath;\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBase = new ConfigData::DataBase*[size];\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBaseModified = new bool[size];\n");
   buffer.AppendFormatted("      for (i=0;i<size;i++) {\n");
   buffer.AppendFormatted("         fConfigData[index]->fDataBase[i] = new ConfigData::DataBase();\n");
   // Name
   buffer.AppendFormatted("         dataBasePath.SetFormatted(\"/DataBases/DataBase[%%d]/Name\",i+1);\n");
   buffer.AppendFormatted("         xml->GetPathValue(path+dataBasePath,fConfigData[index]->fDataBase[i]->fName,\"\");\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fName==\"\")\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fNameModified = false;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fNameModified = true;\n");
   // Type
   buffer.AppendFormatted("         dataBasePath.SetFormatted(\"/DataBases/DataBase[%%d]/Type\",i+1);\n");
   buffer.AppendFormatted("         xml->GetPathValue(path+dataBasePath,fConfigData[index]->fDataBase[i]->fType,\"\");\n");
   buffer.AppendFormatted("         fConfigData[index]->fDataBase[i]->fType.ToLower();\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fType==\"\")\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fTypeModified = false;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fTypeModified = true;\n");
   // Connection
   buffer.AppendFormatted("         dataBasePath.SetFormatted(\"/DataBases/DataBase[%%d]/Connection\",i+1);\n");
   buffer.AppendFormatted("         xml->GetPathValue(path+dataBasePath,fConfigData[index]->fDataBase[i]->fConnection,\"\");\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fConnection==\"\")\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fConnectionModified = false;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fConnectionModified = true;\n");
   // EventBased
   buffer.AppendFormatted("         dataBasePath.SetFormatted(\"/DataBases/DataBase[%%d]/EventBased\",i+1);\n");
   buffer.AppendFormatted("         xml->GetPathValue(path+dataBasePath,fConfigData[index]->fDataBase[i]->fEventBased,\"\");\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fEventBased==\"\")\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fEventBasedModified = false;\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBase[i]->fEventBasedModified = true;\n");
   // Check Modified
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBase[i]->fNameModified ||\n");
   buffer.AppendFormatted("             fConfigData[index]->fDataBase[i]->fTypeModified ||\n");
   buffer.AppendFormatted("             fConfigData[index]->fDataBase[i]->fConnectionModified ||\n");
   buffer.AppendFormatted("             fConfigData[index]->fDataBase[i]->fEventBasedModified) {\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBaseModified[i] = true;\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBasesModified = true;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            fConfigData[index]->fDataBaseModified[i] = false;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   // Online
   buffer.AppendFormatted("   // online\n");
   // Host
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Online/Host\",fConfigData[index]->fOnline->fHost,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fHost==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fHostModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fHostModified = true;\n");
   // Experiment
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Online/Experiment\",fConfigData[index]->fOnline->fExperiment,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fExperiment==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fExperimentModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnline->fExperimentModified = true;\n");
   // Check Modified
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fHostModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnlineModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnlineModified = false;\n");
   // socket interface
   buffer.AppendFormatted("   // socket interface\n");
   // PortNumber
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/SocketInterface/PortNumber\",fConfigData[index]->fSocketInterface->fPortNumber,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fSocketInterface->fPortNumber==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterface->fPortNumberModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterface->fPortNumberModified = true;\n");
   // AvailableOffline
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/SocketInterface/AvailableOffline\",fConfigData[index]->fSocketInterface->fAvailableOffline,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fSocketInterface->fAvailableOffline==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterface->fAvailableOfflineModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterface->fAvailableOfflineModified = true;\n");
   // Check Modified
   buffer.AppendFormatted("   if (fConfigData[index]->fSocketInterface->fPortNumberModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fSocketInterface->fAvailableOfflineModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterfaceModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterfaceModified = false;\n");
   // paths
   buffer.AppendFormatted("   // paths\n");
   // InputFilePath
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Paths/InputFilePath\",fConfigData[index]->fPaths->fInputFilePath,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fPaths->fInputFilePath==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fPaths->fInputFilePathModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fPaths->fInputFilePathModified = true;\n");
   // OutputFilePath
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Paths/OutputFilePath\",fConfigData[index]->fPaths->fOutputFilePath,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fPaths->fOutputFilePath==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fPaths->fOutputFilePathModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fPaths->fOutputFilePathModified = true;\n");
   // Check Modified
   buffer.AppendFormatted("   if (fConfigData[index]->fPaths->fInputFilePathModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fPaths->fOutputFilePathModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fPathsModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fPathsModified = false;\n");
   // folders
   buffer.AppendFormatted("   // folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("   // %s Folder\n",folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("   // %s Field\n",valueName[i][j].Data());
            // Name
            buffer.AppendFormatted("   xml->GetPathValue(path+\"/Folders/Folder[FolderName='%s']/Field[FieldName='%s']/DataBaseName\",fConfigData[index]->f%sFolder->f%sField->fName,\"\");\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   if (fConfigData[index]->f%sFolder->f%sField->fName==\"\")\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fNameModified = false;\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fNameModified = true;\n",folderName[i].Data(),valueName[i][j].Data());
            // Path
            buffer.AppendFormatted("   xml->GetPathValue(path+\"/Folders/Folder[FolderName='%s']/Field[FieldName='%s']/DataBasePath\",fConfigData[index]->f%sFolder->f%sField->fPath,\"\");\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   if (fConfigData[index]->f%sFolder->f%sField->fPath==\"\")\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fPathModified = false;\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sField->fPathModified = true;\n",folderName[i].Data(),valueName[i][j].Data());
            // Check Modified
            buffer.AppendFormatted("   if (fConfigData[index]->f%sFolder->f%sField->fNameModified ||\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("       fConfigData[index]->f%sFolder->f%sField->fPathModified) {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->fFoldersModified = true;\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sFieldModified = true;\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      fConfigData[index]->f%sFolderModified = true;\n",folderName[i].Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      fConfigData[index]->f%sFolder->f%sFieldModified = false;\n",folderName[i].Data(),valueName[i][j].Data());
         }
      }
   }
   // tasks
   buffer.AppendFormatted("   // tasks\n");
   buffer.AppendFormatted("   fConfigData[index]->fTasksModified = false;\n");
   ROMEString pointer;
   ROMEString path;
   ROMEString classname;
   for (i=0;i<numOfTaskHierarchy;i++) {
      buffer.AppendFormatted("   // %s Task\n",taskHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      path.Resize(0);
      classname.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTask",taskHierarchyName[index].Data());
         path.InsertFormatted(0,"/Task[TaskName='%s']",taskHierarchyName[index].Data());
         classname.InsertFormatted(0,"::%sTask",taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      // Active
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Active\",fConfigData[index]%s->fActive,\"\");\n",path.Data(),pointer.Data());
      buffer.AppendFormatted("   if (fConfigData[index]%s->fActive==\"\")\n",pointer.Data());
      buffer.AppendFormatted("      fConfigData[index]%s->fActiveModified = false;\n",pointer.Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]%s->fActiveModified = true;\n",pointer.Data());
      // Histogram
      if (numOfHistos[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("   // histograms\n");
         buffer.AppendFormatted("   fConfigData[index]%s->fHistogramsModified = false;\n",pointer.Data());
      }
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         // Title
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistTitle\",fConfigData[index]%s->f%sHisto->fTitle,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fTitle==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fTitleModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fTitleModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // FolderTitle
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistFolderTitle\",fConfigData[index]%s->f%sHisto->fFolderTitle,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fFolderTitle==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fFolderTitleModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fFolderTitleModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // ArraySize
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistArraySize\",fConfigData[index]%s->f%sHisto->fArraySize,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fArraySize==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fArraySizeModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fArraySizeModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // ArrayStartIndex
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistArrayStartIndex\",fConfigData[index]%s->f%sHisto->fArrayStartIndex,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fArrayStartIndex==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fArrayStartIndexModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fArrayStartIndexModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // XLabel
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistXLabel\",fConfigData[index]%s->f%sHisto->fXLabel,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fXLabel==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXLabelModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXLabelModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // YLabel
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistYLabel\",fConfigData[index]%s->f%sHisto->fYLabel,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fYLabel==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYLabelModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYLabelModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // ZLabel
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistZLabel\",fConfigData[index]%s->f%sHisto->fZLabel,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fZLabel==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZLabelModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZLabelModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // XNbins
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistXNbins\",fConfigData[index]%s->f%sHisto->fXNbins,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fXNbins==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXNbinsModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXNbinsModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Xmin
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistXmin\",fConfigData[index]%s->f%sHisto->fXmin,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fXmin==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXminModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXminModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Xmax
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistXmax\",fConfigData[index]%s->f%sHisto->fXmax,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fXmax==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXmaxModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fXmaxModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // YNbins
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistYNbins\",fConfigData[index]%s->f%sHisto->fYNbins,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fYNbins==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYNbinsModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYNbinsModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Ymin
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistYmin\",fConfigData[index]%s->f%sHisto->fYmin,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fYmin==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYminModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYminModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Ymax
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistYmax\",fConfigData[index]%s->f%sHisto->fYmax,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fYmax==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYmaxModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fYmaxModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // ZNbins
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistZNbins\",fConfigData[index]%s->f%sHisto->fZNbins,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fZNbins==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZNbinsModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZNbinsModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Zmin
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistZmin\",fConfigData[index]%s->f%sHisto->fZmin,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fZmin==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZminModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZminModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Zmax
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistZmax\",fConfigData[index]%s->f%sHisto->fZmax,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fZmax==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZmaxModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fZmaxModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Accumulate
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/Histogram[HistName='%s']/HistAccumulate\",fConfigData[index]%s->f%sHisto->fAccumulate,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fAccumulate==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fAccumulateModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fAccumulateModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // all
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fTitleModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fFolderTitleModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fArraySizeModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fArrayStartIndexModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fXLabelModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fYLabelModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fZLabelModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fXNbinsModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fXminModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fXmaxModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fYNbinsModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fYminModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fYmaxModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fZNbinsModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fZminModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fZmaxModified ||\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("       fConfigData[index]%s->f%sHisto->fAccumulateModified) {\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->fHistogramsModified = true;\n",pointer.Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHistoModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHistoModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
      }
      // Steering parameter
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("   // steering parameters\n");
         ROMEString pathT;
         ROMEString pointerT;
         ROMEString classT;
         ROMEString indexT;
         ROMEString blankT;
         int indexCounter = 0;
         pathT.SetFormatted("path+\"/Tasks%s",path.Data());
         pointerT.SetFormatted("fConfigData[index]%s->fSteering",pointer.Data());
         classT.SetFormatted("ConfigData%s::Steering",classname.Data());
         WriteSteeringConfigRead(buffer,0,taskHierarchyClassIndex[i],xml,pathT,pointerT,classT,indexT,blankT,&indexCounter);
      }
      // all
      buffer.AppendFormatted("   if (fConfigData[index]%s->fActiveModified",pointer.Data());
      if (numOfHistos[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("\n    || fConfigData[index]%s->fHistogramsModified",pointer.Data());
      if (numOfSteering[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("\n    || fConfigData[index]%s->fSteeringModified",pointer.Data());
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("      fConfigData[index]->fTasksModified = true;\n");
      buffer.AppendFormatted("      fConfigData[index]%sModified = true;\n",pointer.Data());
      ROMEString tempPointer = pointer;
      while (true) {
         for (j=tempPointer.Length()-1;tempPointer[j]!='>' && j>0;j--) {}
         if (j<=1)
            break;
         tempPointer = tempPointer(0,j-1);
         buffer.AppendFormatted("      fConfigData[index]%sModified = true;\n",tempPointer.Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]%sModified = false;\n",pointer.Data());
   }
   // trees
   buffer.AppendFormatted("   // trees\n");
   // Accumulate
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Accumulate\",fConfigData[index]->fTreeAccumulate,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fTreeAccumulate==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fTreeAccumulateModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fTreeAccumulateModified = true;\n");
   // Check Modified
   buffer.AppendFormatted("   if (fConfigData[index]->fTreeAccumulateModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fTreesModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fTreesModified = false;\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   // %s Tree\n",treeName[i].Data());
      // Read
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/Read\",fConfigData[index]->f%sTree->fRead,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fRead==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fReadModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fReadModified = true;\n",treeName[i].Data());
      // Write
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/Write\",fConfigData[index]->f%sTree->fWrite,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fWrite==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fWriteModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fWriteModified = true;\n",treeName[i].Data());
      // Fill
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fWrite==\"true\") {\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fFill = \"true\";\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fFillModified = true;\n",treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else {\n");
      buffer.AppendFormatted("      xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/Fill\",fConfigData[index]->f%sTree->fFill,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("      if (fConfigData[index]->f%sTree->fFill==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("         fConfigData[index]->f%sTree->fFillModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         fConfigData[index]->f%sTree->fFillModified = true;\n",treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      // CompressionLevel
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/CompressionLevel\",fConfigData[index]->f%sTree->fCompressionLevel,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fCompressionLevel==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fCompressionLevelModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fCompressionLevelModified = true;\n",treeName[i].Data());
      // MaxNumberOfEntries
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/MaxNumberOfEntries\",fConfigData[index]->f%sTree->fMaxNumberOfEntries,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fMaxNumberOfEntries==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified = true;\n",treeName[i].Data());
      // FileName
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/Tree[TreeName='%s']/TreeOutputFileName\",fConfigData[index]->f%sTree->fFileName,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fFileName==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fFileNameModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fFileNameModified = true;\n",treeName[i].Data());
      // Check Modified
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fReadModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fWriteModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fFillModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fCompressionLevelModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fFileNameModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->fTreesModified = true;\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTreeModified = true;\n",treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTreeModified = false;\n",treeName[i].Data());
   }
   // Global Steering Parameters
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("   // global steering parameters\n");
      ROMEString pathT;
      ROMEString pointerT;
      ROMEString classT;
      ROMEString indexT;
      ROMEString blankT;
      int indexCounter = 0;
      pathT.SetFormatted("path+\"/GlobalSteeringParameters");
      pointerT.SetFormatted("fConfigData[index]->fGlobalSteering");
      classT.SetFormatted("ConfigData::GlobalSteering");
      WriteSteeringConfigRead(buffer,0,numOfTaskHierarchy,xml,pathT,pointerT,classT,indexT,blankT,&indexCounter);
   }
   // midas banks
   for (i=0;i<numOfEvent;i++) {
      // Active
      buffer.AppendFormatted("   // %s Event\n",eventName[i].Data());
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Midas/Event[EventName='%s']/Active\",fConfigData[index]->f%sEvent->fActive,\"\");\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sEvent->fActive==\"\")\n",eventName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sEvent->fActiveModified = false;\n",eventName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sEvent->fActiveModified = true;\n",eventName[i].Data());
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("   // %s Bank\n",bankName[i][j].Data());
         // Read
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Midas/Event[EventName='%s']/Bank[BankName='%s']/Active\",fConfigData[index]->f%sEvent->f%sBank->fActive,\"\");\n",eventName[i].Data(),bankName[i][j].Data(),eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]->f%sEvent->f%sBank->fActive==\"\")\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBank->fActiveModified = false;\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBank->fActiveModified = true;\n",eventName[i].Data(),bankName[i][j].Data());
         // Check Modified
         buffer.AppendFormatted("   if (fConfigData[index]->f%sEvent->f%sBank->fActiveModified)\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBankModified = true;\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]->f%sEvent->f%sBankModified = false;\n",eventName[i].Data(),bankName[i][j].Data());
      }
      // Check Modified
      buffer.AppendFormatted("   if (\n");
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("       fConfigData[index]->f%sEvent->f%sBankModified ||\n",eventName[i].Data(),bankName[i][j].Data());
      }
      buffer.AppendFormatted("       fConfigData[index]->f%sEvent->fActiveModified) {\n",eventName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sEventModified = true;\n",eventName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->fMidasModified = true;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sEventModified = false;\n",eventName[i].Data());
   }
   // end
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");



   // Check Configuration
   buffer.AppendFormatted("\n// Check Configuration\n");
   buffer.AppendFormatted("bool %sConfig::CheckConfiguration(int runNumber) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i,j;\n");
   buffer.AppendFormatted("   if (fActiveConfiguration!=0) {\n");
   buffer.AppendFormatted("      if (!SetConfiguration(fActiveConfiguration,0))\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   for (i=1;i<=fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      if (fConfigData[i]->fRunNumbersModified) {\n");
   buffer.AppendFormatted("         if (fConfigData[i]->fLastRunNumberIndex!=-1) {\n");
   buffer.AppendFormatted("            for (j=fConfigData[i]->fLastRunNumberIndex;j<fConfigData[i]->fRunNumberArray.GetSize();j++) {\n");
   buffer.AppendFormatted("               if (fConfigData[i]->fRunNumberArray.At(j)==runNumber) {\n");
   buffer.AppendFormatted("                  if (!SetConfiguration(i,i))\n");
   buffer.AppendFormatted("                     return false;\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastRunNumberIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else if (fConfigData[i]->fRunNumberArray.At(j)<runNumber) {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastRunNumberIndex = -1;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastRunNumberIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Check Configuration
   buffer.AppendFormatted("\n// Check Configuration\n");
   buffer.AppendFormatted("bool %sConfig::CheckConfiguration(const char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString fileName = file;\n");
   buffer.AppendFormatted("   int i,j;\n");
   buffer.AppendFormatted("   if (fActiveConfiguration!=0) {\n");
   buffer.AppendFormatted("      if (!SetConfiguration(fActiveConfiguration,0))\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   for (i=1;i<=fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      if (fConfigData[i]->fInputFileNamesModified) {\n");
   buffer.AppendFormatted("         if (fConfigData[i]->fLastInputFileNameIndex!=-1) {\n");
   buffer.AppendFormatted("            for (j=fConfigData[i]->fLastInputFileNameIndex;j<fConfigData[i]->fInputFileNameArray.GetEntriesFast();j++) {\n");
   buffer.AppendFormatted("               if (fConfigData[i]->fInputFileNameArray.At(j)==fileName) {\n");
   buffer.AppendFormatted("                  if (!SetConfiguration(i,i))\n");
   buffer.AppendFormatted("                     return false;\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastInputFileNameIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else if (fConfigData[i]->fInputFileNameArray.At(j)<fileName) {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastInputFileNameIndex = -1;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastInputFileNameIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");


   // Set Configuration
   buffer.AppendFormatted("\n// Set Configuration\n");
   buffer.AppendFormatted("bool %sConfig::SetConfiguration(int modIndex,int index) {\n",shortCut.Data());
   buffer.AppendFormatted("   char* cstop;\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   fActiveConfiguration = index;\n");
   // Event Numbers
   buffer.AppendFormatted("   // event numbers\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fEventNumbersModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetEventNumbers(fConfigData[index]->fEventNumbers);\n");
   // Paths
   buffer.AppendFormatted("   // paths\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fPaths->fInputFilePathModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fPaths->fInputFilePath[fConfigData[index]->fPaths->fInputFilePath.Length()-1]!='/' && fConfigData[index]->fPaths->fInputFilePath[fConfigData[index]->fPaths->fInputFilePath.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("         fConfigData[index]->fPaths->fInputFilePath.Append(\"/\");\n");
   buffer.AppendFormatted("      gAnalyzer->SetInputDir(fConfigData[index]->fPaths->fInputFilePath);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fPaths->fOutputFilePathModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fPaths->fOutputFilePath[fConfigData[index]->fPaths->fOutputFilePath.Length()-1]!='/' && fConfigData[index]->fPaths->fOutputFilePath[fConfigData[index]->fPaths->fOutputFilePath.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("         fConfigData[index]->fPaths->fOutputFilePath.Append(\"/\");\n");
   buffer.AppendFormatted("      gAnalyzer->SetOutputDir(fConfigData[index]->fPaths->fOutputFilePath);\n");
   buffer.AppendFormatted("   }\n");
   // Modes
   buffer.AppendFormatted("   // modes\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fModes->fAnalyzingModeModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fModes->fAnalyzingMode==\"online\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetOnline();\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gAnalyzer->SetOffline();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fModes->fDAQSystemModified) {\n");
   buffer.AppendFormatted("      if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"midas\",TString::kIgnoreCase)) {\n");
   buffer.AppendFormatted("         gAnalyzer->SetMidasDAQ(new %sMidasDAQ());\n",shortCut.Data());
   buffer.AppendFormatted("         gAnalyzer->SetActiveDAQ(gAnalyzer->GetMidasDAQ());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"rome\",TString::kIgnoreCase)) {\n");
   buffer.AppendFormatted("         gAnalyzer->SetRomeDAQ(new %sRomeDAQ());\n",shortCut.Data());
   buffer.AppendFormatted("         gAnalyzer->SetActiveDAQ(gAnalyzer->GetRomeDAQ());\n");
   buffer.AppendFormatted("      }\n");
   if (this->orca) {
      buffer.AppendFormatted("      else if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"orca\",TString::kIgnoreCase)) {\n");
      buffer.AppendFormatted("         gAnalyzer->SetOrcaDAQ(new ROMEOrcaDAQ());\n",shortCut.Data());
      buffer.AppendFormatted("         gAnalyzer->SetActiveDAQ(gAnalyzer->GetOrcaDAQ());\n");
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("      else if (!fConfigData[index]->fModes->fDAQSystem.CompareTo(\"none\",TString::kIgnoreCase) || fConfigData[index]->fModes->fDAQSystem.Length()==0) {\n");
   buffer.AppendFormatted("         gAnalyzer->SetActiveDAQ(new ROMENoDAQSystem());\n");
   buffer.AppendFormatted("      }\n");
   for (i=0;i<numOfDAQ;i++) {
      buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fDAQSystem==\"%s\") {\n",daqName[i].Data());
      buffer.AppendFormatted("         gAnalyzer->Set%s(new %s%s());\n",daqName[i].Data(),shortCut.Data(),daqName[i].Data());
      buffer.AppendFormatted("         gAnalyzer->SetActiveDAQ(gAnalyzer->Get%s());\n",daqName[i].Data());
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fModes->fBatchModeModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fModes->fBatchMode==\"true\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetBatchMode(true);\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gAnalyzer->SetBatchMode(false);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fModes->fShowSplashScreenModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fModes->fShowSplashScreen==\"false\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetSplashScreen(false);\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gAnalyzer->SetSplashScreen(true);\n");
   buffer.AppendFormatted("   }\n");
   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (index==0) {\n");
   buffer.AppendFormatted("      if (fConfigData[modIndex]->fDataBasesModified) {\n");
   buffer.AppendFormatted("         for (i=0;i<gAnalyzer->GetNumberOfDataBases();i++) {\n");
   buffer.AppendFormatted("            if (fConfigData[modIndex]->fDataBaseModified[i]) {\n");
   buffer.AppendFormatted("               if (fConfigData[modIndex]->fDataBase[i]->fEventBasedModified) {\n");
   buffer.AppendFormatted("                  if (fConfigData[index]->fDataBase[i]->fEventBased==\"true\")\n");
   buffer.AppendFormatted("                     gAnalyzer->SetEventBasedDataBase(true);\n");
   buffer.AppendFormatted("                  else\n");
   buffer.AppendFormatted("                     gAnalyzer->SetEventBasedDataBase(false);\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               if (fConfigData[modIndex]->fDataBase[i]->fConnectionModified) {\n");
   buffer.AppendFormatted("                  gAnalyzer->SetDataBaseConnection(i,fConfigData[index]->fDataBase[i]->fConnection.Data());\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               if (fConfigData[modIndex]->fDataBase[i]->fTypeModified) {\n");
   buffer.AppendFormatted("                  if (fConfigData[index]->fDataBase[i]->fType==\"sql\") {\n");
   buffer.AppendFormatted("#if defined( HAVE_SQL )\n");
   buffer.AppendFormatted("                     gAnalyzer->SetDataBase(i,new ROMESQLDataBase());\n");
   buffer.AppendFormatted("                     if (!gAnalyzer->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",gAnalyzer->GetDataBaseConnection(i)))\n");
   buffer.AppendFormatted("                        return false;\n");
   buffer.AppendFormatted("#else\n");
   buffer.AppendFormatted("                        cout<<gAnalyzer->GetProgramName()<<\" is not linked with sql support.\"<<endl;\n");
   buffer.AppendFormatted("                        return false;\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("                  else if (fConfigData[index]->fDataBase[i]->fType==\"none\" ||\n");
   buffer.AppendFormatted("                           fConfigData[index]->fDataBase[i]->fType==\"\") {\n");
   buffer.AppendFormatted("                     gAnalyzer->SetDataBase(i,new ROMENoDataBase());\n");
   buffer.AppendFormatted("                     if (!gAnalyzer->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",\"\"))\n");
   buffer.AppendFormatted("                        return false;\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("                  else if (fConfigData[index]->fDataBase[i]->fType==\"xml\") {\n");
   buffer.AppendFormatted("                     gAnalyzer->SetDataBase(i,new ROMEXMLDataBase());\n");
   buffer.AppendFormatted("                     ROMEString str = gAnalyzer->GetDataBaseConnection(i);\n");
   buffer.AppendFormatted("                     int ind;\n");
   buffer.AppendFormatted("                     if ((ind=str.Index(\";\",1,0,TString::kExact))==-1) {\n");
   buffer.AppendFormatted("                        gAnalyzer->Println(\"Invalid database connection\");\n");
   buffer.AppendFormatted("                        return false;\n");
   buffer.AppendFormatted("                     }\n");
   buffer.AppendFormatted("                     ROMEString path = str(0,ind);\n");
   buffer.AppendFormatted("                     if (path[path.Length()-1]!='/' && path[path.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("                        path += \"/\";\n");
   buffer.AppendFormatted("                     gAnalyzer->SetDataBaseDir(i,path.Data());\n");
   buffer.AppendFormatted("                     if (!gAnalyzer->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),gAnalyzer->GetDataBaseDir(i),((TString)str(ind+1,str.Length()-ind-1)).Data()))\n");
   buffer.AppendFormatted("                        return false;\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("                  else if (fConfigData[index]->fDataBase[i]->fType==\"text\") {\n");
   buffer.AppendFormatted("                     gAnalyzer->SetDataBase(i,new ROMETextDataBase());\n");
   buffer.AppendFormatted("                     if (!gAnalyzer->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),gAnalyzer->GetDataBaseConnection(i),\"\"))\n");
   buffer.AppendFormatted("                        return false;\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("                  if (fConfigData[index]->fDataBase[i]->fType==\"odb\") {\n");
   buffer.AppendFormatted("                     if (gAnalyzer->isOffline())\n");
   buffer.AppendFormatted("                        gAnalyzer->SetDataBase(i,new ROMEODBOfflineDataBase());\n");
   buffer.AppendFormatted("                     else\n");
   buffer.AppendFormatted("                        gAnalyzer->SetDataBase(i,new ROMEODBOnlineDataBase());\n");
   buffer.AppendFormatted("                     if (!gAnalyzer->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",\"\"))\n");
   buffer.AppendFormatted("                        return false;\n");
   buffer.AppendFormatted("                  }\n");
   for (i=0;i<numOfDB;i++) {
      buffer.AppendFormatted("                  else if (fConfigData[index]->fDataBase[i]->fType==\"%s\") {\n",dbName[i].Data());
      buffer.AppendFormatted("                     gAnalyzer->SetDataBase(i,new %s%sDataBase());\n",shortCut.Data(),dbName[i].Data());
      buffer.AppendFormatted("                     if (!gAnalyzer->GetDataBase(i)->Init(fConfigData[index]->fDataBase[i]->fName.Data(),\"\",gAnalyzer->GetDataBaseConnection(i)))\n");
      buffer.AppendFormatted("                        return false;\n");
      buffer.AppendFormatted("                  }\n");
   }
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fOnline->fHostModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetOnlineHost(fConfigData[index]->fOnline->fHost.Data());\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetOnlineExperiment(fConfigData[index]->fOnline->fExperiment.Data());\n");
   // Socket Interface
   buffer.AppendFormatted("   // socket interface\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fSocketInterface->fPortNumberModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetPortNumber(fConfigData[index]->fSocketInterface->fPortNumber.Data());\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fSocketInterface->fAvailableOfflineModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fSocketInterface->fAvailableOffline==\"true\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetSocketOffline(true);\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gAnalyzer->SetSocketOffline(false);\n");
   buffer.AppendFormatted("   }\n");
   // Folders
   buffer.AppendFormatted("   // folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i]) {
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEString subPath;\n");
         break;
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("   // %s folder\n",folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("   // %s field\n",valueName[i][j].Data());
            // Name
            buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sFolder->f%sField->fNameModified)\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      gAnalyzer->Set%s_%sDBName(fConfigData[modIndex]->f%sFolder->f%sField->fName.Data());\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
            // Path
            buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sFolder->f%sField->fPathModified) {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      path = fConfigData[modIndex]->f%sFolder->f%sField->fPath.Data();\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      int ind = path.Last('\"');\n");
            buffer.AppendFormatted("      if (ind==-1 || ind==path.Length()-1) {\n");
            buffer.AppendFormatted("         subPath = path.ReplaceAll(\"\\\"\",\"\");\n");
            buffer.AppendFormatted("         gAnalyzer->Set%s_%sDBPath(subPath.Data());\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      else {\n");
            buffer.AppendFormatted("         subPath = path(1,ind-1);\n");
            buffer.AppendFormatted("         gAnalyzer->Set%s_%sDBPath(subPath.Data());\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("         path = path(ind+2,path.Length()-ind+2);\n");
            buffer.AppendFormatted("         int num = path.CountChar(',')+1;\n");
            buffer.AppendFormatted("         for (i=0;i<num;i++) {\n");
            buffer.AppendFormatted("            if (i<num-1)\n");
            buffer.AppendFormatted("               ind = path.First(',');\n");
            buffer.AppendFormatted("            else\n");
            buffer.AppendFormatted("               ind = path.Length();\n");
            buffer.AppendFormatted("            subPath = path(0,ind);\n");
            buffer.AppendFormatted("            gAnalyzer->Set%s_%sDBCodeAt(i,gAnalyzer->GetObjectInterpreterCode(subPath.Data()));\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("            path = path(ind+1,path.Length()-ind+1);\n");
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   // Tasks
   buffer.AppendFormatted("   // tasks\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      buffer.AppendFormatted("   // %s task\n",taskHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTask",taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      buffer.AppendFormatted("   if (fConfigData[modIndex]%s->fActiveModified) {\n",pointer.Data());
      buffer.AppendFormatted("      if (fConfigData[index]%s->fActive==\"true\")\n",pointer.Data());
      buffer.AppendFormatted("         gAnalyzer->Get%s%03dTask()->SetActive(true);\n",taskHierarchyName[i].Data(),i);
      buffer.AppendFormatted("      else {\n");
      if (taskHierarchyParentIndex[i]==-1)
         buffer.AppendFormatted("         gAnalyzer->GetMainHistoFolder()->Remove(gAnalyzer->GetHistoFolderAt(%d));\n",i);
      else
         buffer.AppendFormatted("         gAnalyzer->GetHistoFolderAt(%d)->Remove(gAnalyzer->GetHistoFolderAt(%d));\n",taskHierarchyParentIndex[i],i);
      buffer.AppendFormatted("         gAnalyzer->Get%s%03dTask()->SetActive(false);\n",taskHierarchyName[i].Data(),i);
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
      // Histogram
      if (numOfHistos[i]>0)
         buffer.AppendFormatted("   // histograms\n");
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fTitleModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sTitle(fConfigData[index]%s->f%sHisto->fTitle.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fFolderTitleModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sFolderTitle(fConfigData[index]%s->f%sHisto->fFolderTitle.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fArraySizeModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sArraySize(fConfigData[index]%s->f%sHisto->fArraySize.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fArrayStartIndexModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sArrayStartIndex(fConfigData[index]%s->f%sHisto->fArrayStartIndex.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fXLabelModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sXLabel(fConfigData[index]%s->f%sHisto->fXLabel.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fYLabelModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sYLabel(fConfigData[index]%s->f%sHisto->fYLabel.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fZLabelModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sZLabel(fConfigData[index]%s->f%sHisto->fZLabel.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fXNbinsModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sXNbins(fConfigData[index]%s->f%sHisto->fXNbins.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fXminModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sXmin(fConfigData[index]%s->f%sHisto->fXmin.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fXmaxModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sXmax(fConfigData[index]%s->f%sHisto->fXmax.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fYNbinsModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sYNbins(fConfigData[index]%s->f%sHisto->fYNbins.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fYminModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sYmin(fConfigData[index]%s->f%sHisto->fYmin.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fYmaxModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sYmax(fConfigData[index]%s->f%sHisto->fYmax.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fZNbinsModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sZNbins(fConfigData[index]%s->f%sHisto->fZNbins.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fZminModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sZmin(fConfigData[index]%s->f%sHisto->fZmin.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fZmaxModified)\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sZmax(fConfigData[index]%s->f%sHisto->fZmax.Data());\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sHisto->fAccumulateModified) {\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      if (fConfigData[index]%s->f%sHisto->fAccumulate==\"false\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("         ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sAccumulation(false);\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         ((%sT%s*)gAnalyzer->Get%s%03dTask())->Set%sAccumulation(false);\n",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   }\n");
      }
      // Steering parameter
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("   // steering parameters\n");
         ROMEString pointerT;
         ROMEString steerPointerT;
         ROMEString blankT;
         int indexCounter = 0;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->Get%s%03dTask())->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i);
         WriteSteeringConfigSet(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT,blankT,&indexCounter);
      }
   }
   // Trees
   buffer.AppendFormatted("   // trees\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fTreeAccumulateModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fTreeAccumulate==\"true\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetTreeAccumulation(true);\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gAnalyzer->SetTreeAccumulation(false);\n");
   buffer.AppendFormatted("   }\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   // %s tree\n",treeName[i].Data());
      // Read
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fReadModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      if (fConfigData[index]->f%sTree->fRead==\"true\")\n",treeName[i].Data());
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetRead(true);\n",i);
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetRead(false);\n",i);
      buffer.AppendFormatted("   }\n");
      // Write
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fWriteModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      if (fConfigData[index]->f%sTree->fWrite==\"true\")\n",treeName[i].Data());
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetWrite(true);\n",i);
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetWrite(false);\n",i);
      buffer.AppendFormatted("   }\n");
      // Fill
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fFillModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      if (fConfigData[index]->f%sTree->fFill==\"true\")\n",treeName[i].Data());
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetFill(true);\n",i);
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetFill(false);\n",i);
      buffer.AppendFormatted("   }\n");
      // CompressionLevel
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fCompressionLevelModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      gAnalyzer->GetTreeObjectAt(%d)->SetCompressionLevel(strtol(fConfigData[modIndex]->f%sTree->fCompressionLevel.Data(),&cstop,10));\n",i,treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      // MaxNumberOfEntries
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fMaxNumberOfEntriesModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      int maxNumOfEntries = strtol(fConfigData[index]->f%sTree->fMaxNumberOfEntries.Data(),&cstop,10);\n",treeName[i].Data());
      buffer.AppendFormatted("      gAnalyzer->GetTreeObjectAt(%d)->SetMaxEntries(maxNumOfEntries);\n",i);
      buffer.AppendFormatted("   }\n");
      // FileName
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fFileNameModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      gAnalyzer->GetTreeObjectAt(%d)->SetConfigFileName(fConfigData[index]->f%sTree->fFileName);\n",i,treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else {\n");
      buffer.AppendFormatted("      gAnalyzer->GetTreeObjectAt(%d)->SetConfigFileName(fConfigData[0]->f%sTree->fFileName);\n",i,treeName[i].Data());
      buffer.AppendFormatted("   }\n");
   }
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTaskHierarchy]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      ROMEString blankT;
      int indexCounter = 0;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
      WriteSteeringConfigSet(buffer,0,numOfTaskHierarchy,pointerT,steerPointerT,blankT,&indexCounter);
   }

   // midas banks
   buffer.AppendFormatted("   // midas banks\n");
   for (i=0;i<numOfEvent;i++) {
      buffer.AppendFormatted("   // %s event\n",eventName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sEventModified) {\n",eventName[i].Data());
      // Active
      buffer.AppendFormatted("      if (fConfigData[modIndex]->f%sEvent->fActiveModified && !strcmp(gAnalyzer->GetNameOfActiveDAQ(),\"midas\")) {\n",eventName[i].Data());
      buffer.AppendFormatted("         if (fConfigData[index]->f%sEvent->fActive==\"true\")\n",eventName[i].Data());
      buffer.AppendFormatted("            gAnalyzer->GetMidasDAQ()->Set%sEventActive(true);\n",eventName[i].Data());
      buffer.AppendFormatted("         else\n");
      buffer.AppendFormatted("            gAnalyzer->GetMidasDAQ()->Set%sEventActive(false);\n",eventName[i].Data());
      buffer.AppendFormatted("      }\n");
      // Banks
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("      // %s bank\n",bankName[i][j].Data());
         buffer.AppendFormatted("      if (fConfigData[modIndex]->f%sEvent->f%sBankModified) {\n",eventName[i].Data(),bankName[i][j].Data());
         // Active
         buffer.AppendFormatted("         if (fConfigData[modIndex]->f%sEvent->f%sBank->fActiveModified && !strcmp(gAnalyzer->GetNameOfActiveDAQ(),\"midas\")) {\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("            if (fConfigData[index]->f%sEvent->f%sBank->fActive==\"true\")\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("               gAnalyzer->GetMidasDAQ()->Set%sBankActive(true);\n",bankName[i][j].Data());
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               gAnalyzer->GetMidasDAQ()->Set%sBankActive(false);\n",bankName[i][j].Data());
         buffer.AppendFormatted("         }\n");
         // end
         buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("   }\n");
   }
   // end
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");


   // Write Configuration File
   buffer.AppendFormatted("\n// Write Configuration File\n");
   buffer.AppendFormatted("bool %sConfig::WriteConfigurationFile(const char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   if (!xml->OpenFileForWrite(fXMLFile))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   xml->SetTranslate(0);\n");
   buffer.AppendFormatted("   xml->StartElement(\"Configuration\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xmlns:xsi\",\"http://www.w3.org/2001/XMLSchema-instance\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xsi:noNamespaceSchemaLocation\",\"c:/rome/rome/romeConfig.xsd\");\n");
   buffer.AppendFormatted("   xml->StartElement(\"MainConfiguration\");\n");
   buffer.AppendFormatted("   WriteConfiguration(xml,0);\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   for (int i=0;i<fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"RunConfiguration\");\n");
   buffer.AppendFormatted("      WriteConfiguration(xml,i+1);\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   xml->EndDocument();\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");

   // Write Configuration
   buffer.AppendFormatted("\n// Write Configuration\n");
   buffer.AppendFormatted("bool %sConfig::WriteConfiguration(ROMEXML *xml,int index) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   buffer.AppendFormatted("   int i;\n");
   // run numbers
   buffer.AppendFormatted("   if (index==0)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"RunNumbers\",gAnalyzer->GetRunNumberStringOriginal());\n");
   buffer.AppendFormatted("   else if (fConfigData[index]->fRunNumbersModified)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"RunNumbers\",fConfigData[index]->fRunNumbers.Data());\n");
   // event numbers
   buffer.AppendFormatted("   if (index==0)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"EventNumbers\",gAnalyzer->GetEventNumberStringOriginal());\n");
   buffer.AppendFormatted("   else if (fConfigData[index]->fRunNumbersModified)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"EventNumbers\",fConfigData[index]->fEventNumbers.Data());\n");
   // input file names
   buffer.AppendFormatted("   if (index==0)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"InputFileNames\",gAnalyzer->GetInputFileNamesStringOriginal());\n");
   buffer.AppendFormatted("   else if (fConfigData[index]->fInputFileNamesModified)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"InputFileNames\",fConfigData[index]->fInputFileNames.Data());\n");
   // modes
   buffer.AppendFormatted("   // modes\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModesModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Modes\");\n");
   // AnalyzingMode
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isOnline())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AnalyzingMode\",\"online\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AnalyzingMode\",\"offline\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fAnalyzingModeModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"AnalyzingMode\",fConfigData[index]->fModes->fAnalyzingMode.Data());\n");
   // DAQSystem
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isActiveDAQSet())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"DAQSystem\",gAnalyzer->GetActiveDAQ()->GetName());\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"DAQSystem\",\"none\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fDAQSystemModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"DAQSystem\",fConfigData[index]->fModes->fDAQSystem.Data());\n");
   // BatchMode
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isBatchMode())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"BatchMode\",\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"BatchMode\",\"false\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fBatchModeModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"BatchMode\",fConfigData[index]->fModes->fBatchMode.Data());\n");
   // ShowSplashScreen
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isSplashScreen())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"ShowSplashScreen\",\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"ShowSplashScreen\",\"false\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fShowSplashScreenModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"ShowSplashScreen\",fConfigData[index]->fModes->fShowSplashScreen.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fDataBasesModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"DataBases\");\n");
   buffer.AppendFormatted("      for (i=0;i<gAnalyzer->GetNumberOfDataBases();i++) {\n");
   buffer.AppendFormatted("         if (fConfigData[index]->fDataBaseModified[i]) {\n");
   buffer.AppendFormatted("            xml->StartElement(\"DataBase\");\n");
   // Name
   buffer.AppendFormatted("            xml->WriteElement(\"Name\",gAnalyzer->GetDataBase(i)->GetName());\n");
   // Type
   buffer.AppendFormatted("            xml->WriteElement(\"Type\",gAnalyzer->GetDataBase(i)->GetType());\n");
   // Connection
   buffer.AppendFormatted("            xml->WriteElement(\"Connection\",gAnalyzer->GetDataBaseConnection(i));\n");
   // EventBased
   buffer.AppendFormatted("            if (gAnalyzer->IsEventBasedDataBase())\n");
   buffer.AppendFormatted("               xml->WriteElement(\"EventBased\",\"true\");\n");
   buffer.AppendFormatted("            else\n");
   buffer.AppendFormatted("               xml->WriteElement(\"EventBased\",\"false\");\n");
   buffer.AppendFormatted("            xml->EndElement();\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fOnlineModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Online\");\n");
   // Host
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",gAnalyzer->GetOnlineHost());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fOnline->fHostModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",fConfigData[index]->fOnline->fHost.Data());\n");
   // Experiment
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",gAnalyzer->GetOnlineExperiment());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",fConfigData[index]->fOnline->fExperiment.Data());\n");

   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // SocketInterface
   buffer.AppendFormatted("   // socket interface\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fSocketInterfaceModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"SocketInterface\");\n");
   // PortNumber
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         str.SetFormatted(\"%%d\",gAnalyzer->GetPortNumber());\n");
   buffer.AppendFormatted("         xml->WriteElement(\"PortNumber\",str.Data());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fSocketInterface->fPortNumberModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"PortNumber\",fConfigData[index]->fSocketInterface->fPortNumber.Data());\n");
   // AvailableOffline
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isSocketOffline())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AvailableOffline\",\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AvailableOffline\",\"false\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fSocketInterface->fAvailableOfflineModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"AvailableOffline\",fConfigData[index]->fSocketInterface->fAvailableOffline.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // Paths
   buffer.AppendFormatted("   // paths\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fPathsModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Paths\");\n");
   // InputFilePath
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputFilePath\",gAnalyzer->GetInputDir());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fPaths->fInputFilePathModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputFilePath\",fConfigData[index]->fPaths->fInputFilePath.Data());\n");
   // OutputFilePath
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"OutputFilePath\",gAnalyzer->GetOutputDir());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fPaths->fOutputFilePathModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"OutputFilePath\",fConfigData[index]->fPaths->fOutputFilePath.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // Folders
   buffer.AppendFormatted("   // folders\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fFoldersModified) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Folders\");\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("      if (fConfigData[index]->f%sFolderModified) {\n",folderName[i].Data());
         buffer.AppendFormatted("         // %s folder\n",folderName[i].Data());
         buffer.AppendFormatted("         xml->StartElement(\"Folder\");\n");
         buffer.AppendFormatted("         xml->WriteElement(\"FolderName\",\"%s\");\n",folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("         if (fConfigData[index]->f%sFolder->f%sFieldModified) {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("            // %s field\n",valueName[i][j].Data());
            buffer.AppendFormatted("            xml->StartElement(\"Field\");\n");
            buffer.AppendFormatted("            xml->WriteElement(\"FieldName\",\"%s\");\n",valueName[i][j].Data());
            // name
            buffer.AppendFormatted("            if (fConfigData[index]->f%sFolder->f%sField->fNameModified)\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("               xml->WriteElement(\"DataBaseName\",fConfigData[index]->f%sFolder->f%sField->fName.Data());\n",folderName[i].Data(),valueName[i][j].Data());
            // path
            buffer.AppendFormatted("            if (fConfigData[index]->f%sFolder->f%sField->fNameModified)\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("               xml->WriteElement(\"DataBasePath\",fConfigData[index]->f%sFolder->f%sField->fPath.Data());\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("            xml->EndElement();\n");
            buffer.AppendFormatted("         }\n");
         }
         buffer.AppendFormatted("         xml->EndElement();\n");
         buffer.AppendFormatted("      }\n");
      }
   }
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // Tasks
   buffer.AppendFormatted("   // tasks\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fTasksModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Tasks\");\n");
   pointer.Resize(0);
   WriteTaskConfigWrite(buffer,-1,pointer,0);
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // Trees
   buffer.AppendFormatted("   // trees\n");
   if (numOfTree>0) {
      buffer.AppendFormatted("   if (fConfigData[index]->fTreesModified || index==0) {\n");
      buffer.AppendFormatted("      xml->StartElement(\"Trees\");\n");
      buffer.AppendFormatted("      if (index==0) {\n");
      buffer.AppendFormatted("         if (gAnalyzer->isTreeAccumulation())\n");
      buffer.AppendFormatted("            xml->WriteElement(\"Accumulate\",\"true\");\n");
      buffer.AppendFormatted("         else\n");
      buffer.AppendFormatted("            xml->WriteElement(\"Accumulate\",\"false\");\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("      else if (fConfigData[index]->fTreeAccumulateModified)\n");
      buffer.AppendFormatted("         xml->WriteElement(\"Accumulate\",fConfigData[index]->fTreeAccumulate.Data());\n");
      for (i=0;i<numOfTree;i++) {
         buffer.AppendFormatted("      if (fConfigData[index]->f%sTreeModified || index==0) {\n",treeName[i].Data());
         buffer.AppendFormatted("         // %s tree\n",treeName[i].Data());
         buffer.AppendFormatted("         xml->StartElement(\"Tree\");\n");
         buffer.AppendFormatted("         xml->WriteElement(\"TreeName\",\"%s\");\n",treeName[i].Data());
         // read
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            if (gAnalyzer->GetTreeObjectAt(%d)->isRead())\n",i);
         buffer.AppendFormatted("               xml->WriteElement(\"Read\",\"true\");\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               xml->WriteElement(\"Read\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fReadModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"Read\",fConfigData[index]->f%sTree->fRead.Data());\n",treeName[i].Data());
         // write
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            if (gAnalyzer->GetTreeObjectAt(%d)->isWrite())\n",i);
         buffer.AppendFormatted("               xml->WriteElement(\"Write\",\"true\");\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               xml->WriteElement(\"Write\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fWriteModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"Write\",fConfigData[index]->f%sTree->fWrite.Data());\n",treeName[i].Data());
         // fill
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            if (gAnalyzer->GetTreeObjectAt(%d)->isFill())\n",i);
         buffer.AppendFormatted("               xml->WriteElement(\"Fill\",\"true\");\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               xml->WriteElement(\"Fill\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fFillModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"Fill\",fConfigData[index]->f%sTree->fFill.Data());\n",treeName[i].Data());
         // compression level
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            str.SetFormatted(\"%%d\",gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",i);
         buffer.AppendFormatted("            xml->WriteElement(\"CompressionLevel\",str.Data());\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fCompressionLevelModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"CompressionLevel\",fConfigData[index]->f%sTree->fCompressionLevel.Data());\n",treeName[i].Data());
         // MaxNumberOfEntries
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            str.SetFormatted(\"%%d\",gAnalyzer->GetTreeObjectAt(%d)->GetMaxEntries());\n",i);
         buffer.AppendFormatted("            xml->WriteElement(\"MaxNumberOfEntries\",str.Data());\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"MaxNumberOfEntries\",fConfigData[index]->f%sTree->fMaxNumberOfEntries.Data());\n",treeName[i].Data());
         // FileName
         buffer.AppendFormatted("         if (index==0)\n");
         buffer.AppendFormatted("            xml->WriteElement(\"TreeOutputFileName\",gAnalyzer->GetTreeObjectAt(%d)->GetConfigFileName().Data());\n",i);
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fFileNameModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"TreeOutputFileName\",fConfigData[index]->f%sTree->fFileName.Data());\n",treeName[i].Data());

         buffer.AppendFormatted("         xml->EndElement();\n");
         buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("      xml->EndElement();\n");
      buffer.AppendFormatted("   }\n");
   }
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("   if (fConfigData[index]->fGlobalSteeringModified || index==0) {\n");
      buffer.AppendFormatted("      ROMEString value;\n");
      buffer.AppendFormatted("      xml->StartElement(\"GlobalSteeringParameters\");\n");
      ROMEString pointerT;
      ROMEString steerPointerT;
      int indexCounter = 0;
      pointerT.SetFormatted("fConfigData[index]->fGlobalSteering");
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
      WriteSteeringConfigWrite(buffer,0,numOfTaskHierarchy,pointerT,steerPointerT,1,&indexCounter);
   }
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("      xml->EndElement();\n");
      buffer.AppendFormatted("   }\n");
   }

   // Midas banks
   buffer.AppendFormatted("   // midas banks\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fMidasModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Midas\");\n");
   for (i=0;i<numOfEvent;i++) {
      buffer.AppendFormatted("      if (fConfigData[index]->f%sEventModified || index==0) {\n",eventName[i].Data());
      buffer.AppendFormatted("         xml->StartElement(\"Event\");\n");
      buffer.AppendFormatted("         xml->WriteElement(\"EventName\",\"%s\");\n",eventName[i].Data());
      // active
      buffer.AppendFormatted("         if (index==0) {\n");
      buffer.AppendFormatted("            if (!strcmp(gAnalyzer->GetNameOfActiveDAQ(),\"midas\")) {\n");
      buffer.AppendFormatted("               if (gAnalyzer->GetMidasDAQ()->is%sEventActive())\n",eventName[i].Data());
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",\"true\");\n");
      buffer.AppendFormatted("               else\n");
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",\"false\");\n");
      buffer.AppendFormatted("            }\n");
      buffer.AppendFormatted("            else {\n");
      buffer.AppendFormatted("               if (fConfigData[index]->f%sEvent->fActiveModified)\n",eventName[i].Data());
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->fActive.Data());\n",eventName[i].Data());
      buffer.AppendFormatted("               else\n");
      buffer.AppendFormatted("                  xml->WriteElement(\"Active\",\"false\");\n");
      buffer.AppendFormatted("            }\n");
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         else if (fConfigData[index]->f%sEvent->fActiveModified)\n",eventName[i].Data());
      buffer.AppendFormatted("            xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->fActive.Data());\n",eventName[i].Data());
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("         if (fConfigData[index]->f%sEvent->f%sBankModified || index==0) {\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("            // %s bank\n",bankName[i][j].Data());
         buffer.AppendFormatted("            xml->StartElement(\"Bank\");\n");
         buffer.AppendFormatted("            xml->WriteElement(\"BankName\",\"%s\");\n",bankName[i][j].Data());
         // active
         buffer.AppendFormatted("            if (index==0) {\n");
         buffer.AppendFormatted("               if (!strcmp(gAnalyzer->GetNameOfActiveDAQ(),\"midas\")) {\n");
         buffer.AppendFormatted("                  if (gAnalyzer->GetMidasDAQ()->is%sBankActive())\n",bankName[i][j].Data());
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",\"true\");\n");
         buffer.AppendFormatted("                  else\n");
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",\"false\");\n");
         buffer.AppendFormatted("               }\n");
         buffer.AppendFormatted("               else {\n");
         buffer.AppendFormatted("                  if (fConfigData[index]->f%sEvent->f%sBank->fActiveModified)\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->f%sBank->fActive.Data());\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("                  else\n");
         buffer.AppendFormatted("                     xml->WriteElement(\"Active\",\"false\");\n");
         buffer.AppendFormatted("               }\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("            else if (fConfigData[index]->f%sEvent->f%sBank->fActiveModified)\n",eventName[i].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("               xml->WriteElement(\"Active\",fConfigData[index]->f%sEvent->f%sBank->fActive.Data());\n",eventName[i].Data(),bankName[i][j].Data());
         // end
         buffer.AppendFormatted("            xml->EndElement();\n");
         buffer.AppendFormatted("         }\n");
      }
      buffer.AppendFormatted("         xml->EndElement();\n");
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // end
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}
bool ROMEBuilder::WriteConfigH() {
   int i,j;

   ROMEString hFile;
   ROMEString buffer;

   // File name
   hFile.SetFormatted("%s/include/framework/%sConfig.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sConfig_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sConfig_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#ifndef __CINT__\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <include/framework/%sGlobalSteering.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <ROMEConfig.h>\n");

   // Class
   buffer.AppendFormatted("\nclass %sConfig : public ROMEConfig\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#ifndef __CINT__\n");
   // sub classes
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   class ConfigData\n");
   buffer.AppendFormatted("   {\n");
   buffer.AppendFormatted("   public:\n");
   buffer.AppendFormatted("      ROMEString    fRunNumbers;\n");
   buffer.AppendFormatted("      bool          fRunNumbersModified;\n");
   buffer.AppendFormatted("      int           fLastRunNumberIndex;\n");
   buffer.AppendFormatted("      TArrayI       fRunNumberArray;\n");
   buffer.AppendFormatted("      ROMEString    fEventNumbers;\n");
   buffer.AppendFormatted("      bool          fEventNumbersModified;\n");
   buffer.AppendFormatted("      ROMEString    fInputFileNames;\n");
   buffer.AppendFormatted("      bool          fInputFileNamesModified;\n");
   buffer.AppendFormatted("      int           fLastInputFileNameIndex;\n");
   buffer.AppendFormatted("      ROMEStrArray  fInputFileNameArray;\n");
   // modes
   buffer.AppendFormatted("      // modes;\n");
   buffer.AppendFormatted("      class Modes {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fAnalyzingMode;\n");
   buffer.AppendFormatted("         bool        fAnalyzingModeModified;\n");
   buffer.AppendFormatted("         ROMEString  fDAQSystem;\n");
   buffer.AppendFormatted("         bool        fDAQSystemModified;\n");
   buffer.AppendFormatted("         ROMEString  fBatchMode;\n");
   buffer.AppendFormatted("         bool        fBatchModeModified;\n");
   buffer.AppendFormatted("         ROMEString  fShowSplashScreen;\n");
   buffer.AppendFormatted("         bool        fShowSplashScreenModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Modes *fModes;\n");
   buffer.AppendFormatted("      bool   fModesModified;\n");
   // database
   buffer.AppendFormatted("      // database;\n");
   buffer.AppendFormatted("      class DataBase {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fName;\n");
   buffer.AppendFormatted("         bool        fNameModified;\n");
   buffer.AppendFormatted("         ROMEString  fType;\n");
   buffer.AppendFormatted("         bool        fTypeModified;\n");
   buffer.AppendFormatted("         ROMEString  fConnection;\n");
   buffer.AppendFormatted("         bool        fConnectionModified;\n");
   buffer.AppendFormatted("         ROMEString  fEventBased;\n");
   buffer.AppendFormatted("         bool        fEventBasedModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      DataBase **fDataBase;\n");
   buffer.AppendFormatted("      bool      *fDataBaseModified;\n");
   buffer.AppendFormatted("      bool       fDataBasesModified;\n");
   // online
   buffer.AppendFormatted("      // online;\n");
   buffer.AppendFormatted("      class Online {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fHost;\n");
   buffer.AppendFormatted("         bool        fHostModified;\n");
   buffer.AppendFormatted("         ROMEString  fExperiment;\n");
   buffer.AppendFormatted("         bool        fExperimentModified;\n");
   buffer.AppendFormatted("         Online() {\n");
   buffer.AppendFormatted("         };\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Online *fOnline;\n");
   buffer.AppendFormatted("      bool   fOnlineModified;\n");
   // socket interface
   buffer.AppendFormatted("      // socket interface;\n");
   buffer.AppendFormatted("      class SocketInterface {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fPortNumber;\n");
   buffer.AppendFormatted("         bool        fPortNumberModified;\n");
   buffer.AppendFormatted("         ROMEString  fAvailableOffline;\n");
   buffer.AppendFormatted("         bool        fAvailableOfflineModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      SocketInterface *fSocketInterface;\n");
   buffer.AppendFormatted("      bool   fSocketInterfaceModified;\n");
   // folders
   buffer.AppendFormatted("      // folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("      class %sFolder {\n",folderName[i].Data());
         buffer.AppendFormatted("      public:\n");
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("         class %sField {\n",valueName[i][j].Data());
            buffer.AppendFormatted("         public:\n");
            buffer.AppendFormatted("            ROMEString  fName;\n");
            buffer.AppendFormatted("            bool        fNameModified;\n");
            buffer.AppendFormatted("            ROMEString  fPath;\n");
            buffer.AppendFormatted("            bool        fPathModified;\n");
            buffer.AppendFormatted("         };\n");
            buffer.AppendFormatted("         %sField *f%sField;\n",valueName[i][j].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("         bool   f%sFieldModified;\n",valueName[i][j].Data());
         }
         buffer.AppendFormatted("         %sFolder() {\n",folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if(valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("            f%sFieldModified = false;\n",valueName[i][j].Data());
            buffer.AppendFormatted("            f%sField = new %sField();\n",valueName[i][j].Data(),valueName[i][j].Data());
         }
         buffer.AppendFormatted("         };\n");
         buffer.AppendFormatted("      };\n");
         buffer.AppendFormatted("      %sFolder *f%sFolder;\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("      bool   f%sFolderModified;\n",folderName[i].Data());
      }
   }
   buffer.AppendFormatted("      bool   fFoldersModified;\n");
   // paths
   buffer.AppendFormatted("      // paths;\n");
   buffer.AppendFormatted("      class Paths {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fInputFilePath;\n");
   buffer.AppendFormatted("         bool        fInputFilePathModified;\n");
   buffer.AppendFormatted("         ROMEString  fOutputFilePath;\n");
   buffer.AppendFormatted("         bool        fOutputFilePathModified;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      Paths *fPaths;\n");
   buffer.AppendFormatted("      bool   fPathsModified;\n");
   // tasks
   buffer.AppendFormatted("      // tasks\n");
   WriteTaskConfigClass(buffer,-1,0);
   buffer.AppendFormatted("      bool   fTasksModified;\n");
   // trees
   buffer.AppendFormatted("      // trees\n");
   buffer.AppendFormatted("      ROMEString  fTreeAccumulate;\n");
   buffer.AppendFormatted("      bool        fTreeAccumulateModified;\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("      class %sTree {\n",treeName[i].Data());
      buffer.AppendFormatted("      public:\n");
      buffer.AppendFormatted("         ROMEString  fRead;\n");
      buffer.AppendFormatted("         bool        fReadModified;\n");
      buffer.AppendFormatted("         ROMEString  fWrite;\n");
      buffer.AppendFormatted("         bool        fWriteModified;\n");
      buffer.AppendFormatted("         ROMEString  fFill;\n");
      buffer.AppendFormatted("         bool        fFillModified;\n");
      buffer.AppendFormatted("         ROMEString  fCompressionLevel;\n");
      buffer.AppendFormatted("         bool        fCompressionLevelModified;\n");
      buffer.AppendFormatted("         ROMEString  fMaxNumberOfEntries;\n");
      buffer.AppendFormatted("         bool        fMaxNumberOfEntriesModified;\n");
      buffer.AppendFormatted("         ROMEString  fFileName;\n");
      buffer.AppendFormatted("         bool        fFileNameModified;\n");
      buffer.AppendFormatted("      };\n");
      buffer.AppendFormatted("      %sTree *f%sTree;\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("      bool   f%sTreeModified;\n",treeName[i].Data());
   }
   buffer.AppendFormatted("      bool   fTreesModified;\n");
   // steering parameters
   buffer.AppendFormatted("      class GlobalSteering {\n");
   buffer.AppendFormatted("      public:\n");
   if (numOfSteering[numOfTaskHierarchy]>0) {
      WriteSteeringConfigClass(buffer,0,numOfTaskHierarchy,2);
   }
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      GlobalSteering *fGlobalSteering;\n");
   buffer.AppendFormatted("      bool            fGlobalSteeringModified;\n");
   // midas banks
   buffer.AppendFormatted("      // midas banks\n");
   for (i=0;i<numOfEvent;i++) {
      buffer.AppendFormatted("      // %s event\n",eventName[i].Data());
      buffer.AppendFormatted("      class %sEvent {\n",eventName[i].Data());
      buffer.AppendFormatted("      public:\n");
      buffer.AppendFormatted("         ROMEString  fActive;\n");
      buffer.AppendFormatted("         bool        fActiveModified;\n");
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("         // %s bank\n",bankName[i][j].Data());
         buffer.AppendFormatted("         class %sBank {\n",bankName[i][j].Data());
         buffer.AppendFormatted("         public:\n");
         buffer.AppendFormatted("            ROMEString  fActive;\n");
         buffer.AppendFormatted("            bool        fActiveModified;\n");
         buffer.AppendFormatted("            %sBank() {\n",bankName[i][j].Data());
         buffer.AppendFormatted("               fActiveModified = false;\n");
         buffer.AppendFormatted("            };\n");
         buffer.AppendFormatted("         };\n");
         buffer.AppendFormatted("         %sBank *f%sBank;\n",bankName[i][j].Data(),bankName[i][j].Data());
         buffer.AppendFormatted("         bool   f%sBankModified;\n",bankName[i][j].Data());
      }
      buffer.AppendFormatted("         %sEvent() {\n",eventName[i].Data());
      buffer.AppendFormatted("            fActiveModified = false;\n");
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("            f%sBankModified = false;\n",bankName[i][j].Data());
         buffer.AppendFormatted("            f%sBank = new %sBank();\n",bankName[i][j].Data(),bankName[i][j].Data());
      }
      buffer.AppendFormatted("         };\n");
      buffer.AppendFormatted("      };\n");
      buffer.AppendFormatted("      %sEvent *f%sEvent;\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("      bool   f%sEventModified;\n",eventName[i].Data());
   }
   buffer.AppendFormatted("      bool   fMidasModified;\n");
   // Constructor
   buffer.AppendFormatted("   public:\n");
   buffer.AppendFormatted("      ConfigData() {\n");
   buffer.AppendFormatted("         fRunNumbersModified = false;\n");
   buffer.AppendFormatted("         fEventNumbersModified = false;\n");
   buffer.AppendFormatted("         fInputFileNamesModified = false;\n");
   buffer.AppendFormatted("         fModesModified = false;\n");
   buffer.AppendFormatted("         fModes = new Modes();\n");
   buffer.AppendFormatted("         fDataBasesModified = false;\n");
   buffer.AppendFormatted("         fOnlineModified = false;\n");
   buffer.AppendFormatted("         fOnline = new Online();\n");
   buffer.AppendFormatted("         fSocketInterfaceModified = false;\n");
   buffer.AppendFormatted("         fSocketInterface = new SocketInterface();\n");
   buffer.AppendFormatted("         fPathsModified = false;\n");
   buffer.AppendFormatted("         fPaths = new Paths();\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("         f%sFolderModified = false;\n",folderName[i].Data());
         buffer.AppendFormatted("         f%sFolder = new %sFolder();\n",folderName[i].Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("         fFoldersModified = false;\n");
   buffer.AppendFormatted("         fTasksModified = false;\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (taskHierarchyParentIndex[i]==-1) {
         buffer.AppendFormatted("         f%sTaskModified = false;\n",taskHierarchyName[i].Data());
         buffer.AppendFormatted("         f%sTask = new %sTask();\n",taskHierarchyName[i].Data(),taskHierarchyName[i].Data());
      }
   }
   buffer.AppendFormatted("         fTreesModified = false;\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("         f%sTreeModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("         f%sTree = new %sTree();\n",treeName[i].Data(),treeName[i].Data());
   }
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("         fGlobalSteeringModified = false;\n");
      buffer.AppendFormatted("         fGlobalSteering = new GlobalSteering();\n");
   }
   for (i=0;i<numOfEvent;i++) {
      buffer.AppendFormatted("         f%sEventModified = false;\n",eventName[i].Data());
      buffer.AppendFormatted("         f%sEvent = new %sEvent();\n",eventName[i].Data(),eventName[i].Data());
   }
   buffer.AppendFormatted("         fMidasModified = false;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");

   // Fields
   buffer.AppendFormatted("   ConfigData **fConfigData;\n");
   buffer.AppendFormatted("   ROMEString fXMLFile;\n");
   buffer.AppendFormatted("   int   fNumberOfRunConfigs;\n");
   buffer.AppendFormatted("   int   fActiveConfiguration;\n");
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sConfig();\n",shortCut.Data());

   // methods
   buffer.AppendFormatted("   bool WriteConfigurationFile(const char *file);\n");
   buffer.AppendFormatted("   bool ReadConfigurationFile(const char *file);\n");
   buffer.AppendFormatted("   bool CheckConfiguration(int runNumber);\n");
   buffer.AppendFormatted("   bool CheckConfiguration(const char *file);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   bool ReadConfiguration(ROMEXML *xml,ROMEString& path,int index);\n");
   buffer.AppendFormatted("   bool WriteConfiguration(ROMEXML *xml,int index);\n");
   buffer.AppendFormatted("   bool SetConfiguration(int modIndex,int index);\n");

   buffer.AppendFormatted("\n");
   // Footer

   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sConfig_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}
bool ROMEBuilder::WriteMidasDAQCpp() {
   int i,j;

   ROMEString cppFile;
   ROMEString buffer;

   ROMEString format;

   hasStructuredBank = false;

   ROMEString classDescription;
   classDescription.SetFormatted("This class implements the midas odb access.");

   // File name
   cppFile.SetFormatted("%s/src/framework/%sMidasDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include <ROMEEventLoop.h>\n");
   midasDep.AppendFormatted(" $(ROMESYS)/include/ROMEEventLoop.h");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   midasDep.AppendFormatted(" include/framework/%sAnalyzer.h",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sMidasDAQ.h>\n",shortCut.Data());
   midasDep.AppendFormatted(" include/framework/%sMidasDAQ.h",shortCut.Data());
   buffer.AppendFormatted( " #include <ROMEUtilities.h>\n" );
   midasDep.AppendFormatted( " $(ROMESYS)/include/ROMEUtilities.h" );

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sMidasDAQ::%sMidasDAQ() {\n",shortCut.Data(),shortCut.Data());
   // Midas Banks
   buffer.AppendFormatted("   // Midas Banks\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("   f%sBankActive = true;\n",bankName[i][j].Data());
      }
      buffer.AppendFormatted("   f%sEventActive = true;\n",eventName[i].Data());
   }
   // Event Requests
   buffer.AppendFormatted("   // Event Requests\n");
   buffer.AppendFormatted("   SetNumberOfEventRequests(%d);\n",numOfEvent);
   for (i=0;i<numOfEvent;i++) {
      buffer.AppendFormatted("   SetEventRequestID(%d,%s);\n",i,eventID[i].Data());
      buffer.AppendFormatted("   SetEventRequestMask(%d,%s);\n",i,eventTriggerMask[i].Data());
      buffer.AppendFormatted("   SetEventRequestRate(%d,%s);\n",i,eventSamplingRate[i].Data());
   }

   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Task switches call back
   buffer.AppendFormatted("// Task switches call back\n");
   buffer.AppendFormatted("void TaskSwitchesChanged(int hDB,int hKey,void *info) {\n");
   buffer.AppendFormatted("   ROMEEventLoop::fTaskSwitchesChanged = true;\n");
   buffer.AppendFormatted("}\n\n");

   // Fill Bank Header to Folder
   buffer.AppendFormatted("// Fill Bank Header to Folder\n");
   buffer.AppendFormatted("bool %sMidasDAQ::InitHeader() {\n",shortCut.Data());
   if (bankHasHeader) {
      if (bankHeaderEventID!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->event_id);\n",bankHeaderFolder.Data(),bankHeaderEventID.Data());
      if (bankHeaderTriggerMask!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->trigger_mask);\n",bankHeaderFolder.Data(),bankHeaderTriggerMask.Data());
      if (bankHeaderSerialNumber!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->serial_number);\n",bankHeaderFolder.Data(),bankHeaderSerialNumber.Data());
      if (bankHeaderTimeStamp!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->time_stamp);\n",bankHeaderFolder.Data(),bankHeaderTimeStamp.Data());
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Initialize ODB
   buffer.AppendFormatted("// InitODB\n");
   buffer.AppendFormatted("bool %sMidasDAQ::InitODB() {\n",shortCut.Data());
#if !defined( __ARGUS__ )  // Argus does not make switches in ODB.
   buffer.AppendFormatted("#if defined( HAVE_MIDAS )\n");
   buffer.AppendFormatted("   HNDLE hKey;\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   str = \"/%s%s/Task switches\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   ROMEString taskSwitchesString =  \"");
   ROMEString switchString;
   for (i=0;i<numOfTaskHierarchy;i++) {
      int index = taskHierarchyParentIndex[i];
      switchString = taskHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      buffer.AppendFormatted("%s = BOOL : 0\\n",switchString.Data());
   }
   buffer.AppendFormatted("\";\n");
   buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), (char*)taskSwitchesString.Data(), TRUE);\n");
   buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);\n");
   buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,gAnalyzer->GetTaskSwitches(),sizeof(TaskSwitches),0) != DB_SUCCESS) {\n");
   buffer.AppendFormatted("      gAnalyzer->Println(\"Cannot write to task switches record.\");\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetTaskSwitches(), sizeof(TaskSwitches), MODE_READ, TaskSwitchesChanged, NULL) != DB_SUCCESS) {\n");
   buffer.AppendFormatted("      gAnalyzer->Println(\"Cannot open task switches record, probably other analyzer is using it\");\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("#endif\n");
#endif // __ARGUS__
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Midas Bank Initialisation
   buffer.AppendFormatted("// Midas Bank Initialisation\n");
   buffer.AppendFormatted("void %sMidasDAQ::InitMidasBanks() {\n",shortCut.Data());
   bool done = false;
   for (i=0;i<numOfEvent && !done;i++) {
      for (j=0;j<numOfBank[i] && !done;j++) {
         if (bankArrayDigit[i][j]>0) {
            buffer.AppendFormatted("   int i;\n");
            done = true;
         }
      }
   }
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            buffer.AppendFormatted("   for (i=%d;i<%d;i++)\n",bankArrayStart[i][j],bankArraySize[i][j]+bankArrayStart[i][j]);
            buffer.AppendFormatted("      this->Init%sBank(i);\n",bankName[i][j].Data());
         }
         // Single Bank
         else {
            buffer.AppendFormatted("   this->Init%sBank();\n",bankName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Midas Bank Methodes
   buffer.AppendFormatted("\n// Midas Bank Getters\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("// %s bank\n",bankName[i][j].Data());
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            // Functions
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankAt(int bankIndex,int index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex]+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(int bankIndex,int index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex]+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
            }
            else {
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(int bankIndex,int index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer[bankIndex]+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(int bankIndex,int index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer[bankIndex]+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
            }
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank(int bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID()==%s) {\n",eventName[i].Data(),bankName[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists[bankIndex] = f%sBankExists[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer[bankIndex] = f%sBankPointer[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength[bankIndex] = f%sBankLength[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      unsigned long bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = (EVENT_HEADER*)this->GetRawDataEvent();\n");
            buffer.AppendFormatted("      pevent++;\n");
            buffer.AppendFormatted("      ROMEString bankname;\n");
            buffer.AppendFormatted("      bankname.SetFormatted(\"%s%%0%dd\",bankIndex);\n",bankName[i][j].Data(),bankArrayDigit[i][j]);
            buffer.AppendFormatted("      if (bk_find((BANK_HEADER*)pevent, bankname.Data(), (unsigned long*)&f%sBankLength[bankIndex], &bktype, (void**)&f%sBankPointer[bankIndex])) {\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("         f%sBankExists[bankIndex] = true;\n",bankName[i][j].Data());
            buffer.AppendFormatted("         return;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   f%sBankExists[bankIndex] = false;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankPointer[bankIndex] = NULL;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankLength[bankIndex] = 0;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   return;\n");
            buffer.AppendFormatted("}\n");

            buffer.AppendFormatted("int %sMidasDAQ::Get%sBankEntries(int bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength[bankIndex];\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("int %sMidasDAQ::GetLast%sBankEntries(int bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return fLast%sBankLength[bankIndex];\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n\n");
         }
         // Single Bank
         else {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankAt(int index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(int index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
            }
            else {
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(int index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(int index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
            }
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID()==%s) {\n",eventName[i].Data(),bankName[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists = f%sBankExists;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer = f%sBankPointer;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength = f%sBankLength;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      unsigned long bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = (EVENT_HEADER*)this->GetRawDataEvent();\n");
            buffer.AppendFormatted("      pevent++;\n");
            buffer.AppendFormatted("      if (bk_find((BANK_HEADER*)pevent, \"%s\", (unsigned long*)&f%sBankLength, &bktype, (void**)&f%sBankPointer)) {\n",bankName[i][j].Data(),bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("         f%sBankExists = true;\n",bankName[i][j].Data());
            buffer.AppendFormatted("         return;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   f%sBankExists = false;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankPointer = NULL;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankLength = 0;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   return;\n");
            buffer.AppendFormatted("}\n");

            buffer.AppendFormatted("int %sMidasDAQ::Get%sBankEntries() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength;\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("int %sMidasDAQ::GetLast%sBankEntries() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return fLast%sBankLength;\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n\n");
         }
      }
   }

   buffer.AppendFormatted("bool %sMidasDAQ::IsActiveEventID(int id){\n",shortCut.Data());
   for (i=0;i<numOfEvent;i++) {
      buffer.AppendFormatted("   if (f%sEventActive && id==%s)\n",eventName[i].Data(),eventID[i].Data());
      buffer.AppendFormatted("      return true;\n");
   }
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n");

   // -- Append method for byte swapping bank structures that are defined ----------
#ifndef R__BYTESWAP
   buffer.AppendFormatted( "\n//Used for byte swapping banks which are structs\n" );
   buffer.AppendFormatted( "void* %sMidasDAQ::ByteSwapStruct( char* aName, void* pData )\n", shortCut.Data() );
   buffer.AppendFormatted( "{\n" );

   for ( long iEvent = 0; iEvent < numOfEvent; iEvent++ )
   {
      for ( long iBank = 0; iBank < numOfBank[ iEvent ]; iBank++ )
      {

         // We have a struct bank so create byte swapping code.
         if ( bankType[ iEvent ][ iBank ] == "structure" || bankType[ iEvent ][ iBank ] == "struct" )
         {
            if ( !hasStructuredBank )
            {
               buffer.AppendFormatted( "    if ( strstr( aName, \"%s\" ) )\n", bankName[ iEvent ][ iBank ].Data() );
               hasStructuredBank = true;
            }
            else
            {
               buffer.AppendFormatted( "    else if ( strstr( aName, \"%s\" ) )\n", bankName[ iEvent ][ iBank ].Data() );
            }

            buffer.AppendFormatted( "    {\n" );

            // Loop through all fields for this structure and write out code to perform byte swapping.
            for ( long iField = 0; iField < numOfStructFields[ iEvent ][ iBank ]; iField++ )
            {
               if ( structFieldType[ iEvent ][ iBank ][ iField ] == "char" )
               {
                  buffer.AppendFormatted( "        pData = (void *)( ( (char *)pData ) + %s ); // %s\n",
                              bankFieldArraySize[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
               }

               else if ( bankFieldArraySize[ iEvent ][ iBank ][ iField ] == "1" )
               {
                  buffer.AppendFormatted( "        ByteSwap( (%s *)pData ); // %s\n",
                              structFieldType[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        pData = (void *)( ( (%s *)pData ) + 1 );\n", structFieldType[ iEvent ][ iBank ][ iField ].Data() );
               }
               else
               {
                  buffer.AppendFormatted( "        for ( long i = 0; i < %s; i++ )\n", bankFieldArraySize[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        {\n" );
                  buffer.AppendFormatted( "            ByteSwap( (%s *)pData ); // %s\n",
                              structFieldType[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "            pData = (void *)( ( (%s *)pData ) + 1 );\n", structFieldType[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        }\n" );
               }
            }    // End loop through fields.
            buffer.AppendFormatted( "    }\n" );

         }        // End if is structured bank.
      }            // End loop through banks.
   }                // End loop through events.

   buffer.AppendFormatted( "    return( pData );\n" );
   buffer.AppendFormatted( "}\n" );
#endif // R__BYTESWAP

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}
bool ROMEBuilder::WriteMidasDAQH() {
   int i,j,k;

   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;

   // File name
   hFile.SetFormatted("%s/include/framework/%sMidasDAQ.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sMidasDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sMidasDAQ_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#include <ROMEMidasDAQ.h>\n");
   buffer.AppendFormatted("\n");

   // bank structures
   if (numOfBank>0)
      buffer.AppendFormatted("// Bank Structures\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
            buffer.AppendFormatted("typedef struct {\n");
            for (k=0;k<numOfStructFields[i][j];k++) {
               if (bankFieldArraySize[i][j][k]=="1") {
                  if (structFieldSize[i][j][k].Length()>0)
                     buffer.AppendFormatted("   %s %s : %s;\n",structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data(),structFieldSize[i][j][k].Data());
                  else
                     buffer.AppendFormatted("   %s %s;\n",structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data());
               }
               else {
                  if (structFieldSize[i][j][k].Length()>0) {
                     format.SetFormatted("   %%s %%s[%%s] : %%s;\n");
                     buffer.AppendFormatted(format.Data(),structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data(),structFieldSize[i][j][k].Data());
                  }
                  else {
                     format.SetFormatted("   %%s %%s[%%s];\n");
                     buffer.AppendFormatted(format.Data(),structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data());
                  }
               }
            }
            buffer.AppendFormatted("} %sStruct;\n",bankName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("\nclass %sMidasDAQ : public ROMEMidasDAQ\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Bank Fields
   int bankNameLen = -1;
   int bankTypeLen = -1;
   for (i=0;i<numOfEvent;i++) {
      if (numOfBank[i]>0) {
         for (j=0;j<numOfBank[i];j++) {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               if (bankTypeLen<(int)bankName[i][j].Length()+6) bankTypeLen = bankName[i][j].Length()+6;
            }
            else {
               if (bankTypeLen<(int)bankType[i][j].Length()) bankTypeLen = bankType[i][j].Length();
            }
            if (bankNameLen<(int)bankName[i][j].Length()+bankArrayDigit[i][j]) bankNameLen = bankName[i][j].Length()+bankArrayDigit[i][j];
         }
      }
   }
   buffer.AppendFormatted("   // Bank Fields\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               format.SetFormatted("   %%sStruct*%%%ds f%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
               format.SetFormatted("   %%sStruct*%%%ds fLast%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            }
            else {
               format.SetFormatted("   %%s*%%%ds f%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
               format.SetFormatted("   %%s*%%%ds fLast%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            }
            format.SetFormatted("   int%%%ds f%%sBankLength[%%d];  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-2,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            format.SetFormatted("   int%%%ds fLast%%sBankLength[%%d];  %%%ds //! Length  of the %%s Bank of the last event\n",bankTypeLen-2,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            format.SetFormatted("   bool%%%ds f%%sBankExists[%%d];  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            format.SetFormatted("   bool%%%ds fLast%%sBankExists[%%d];  %%%ds //! Exist Flags of the %%s Bank of the last event\n",bankTypeLen-3,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
         }
         // Single Bank
         else {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               format.SetFormatted("   %%sStruct*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
               format.SetFormatted("   %%sStruct*%%%ds fLast%%sBankPointer; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            }
            else {
               format.SetFormatted("   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
               format.SetFormatted("   %%s*%%%ds fLast%%sBankPointer; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            }
            format.SetFormatted("   int%%%ds f%%sBankLength;  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-2,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            format.SetFormatted("   int%%%ds fLast%%sBankLength;  %%%ds //! Length  of the %%s Bank of the last event\n",bankTypeLen-2,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            format.SetFormatted("   bool%%%ds f%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            format.SetFormatted("   bool%%%ds fLast%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank of the last event\n",bankTypeLen-3,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
         }
         format.SetFormatted("   bool%%%ds f%%sBankActive;  %%%ds //! Active Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-bankName[i][j].Length());
         buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
      }
      format.SetFormatted("   bool%%%ds f%%sEventActive;  %%%ds //! Active Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-eventName[i].Length());
      buffer.AppendFormatted(format.Data(),"",eventName[i].Data(),"",eventName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sMidasDAQ();\n",shortCut.Data());

   // Banks
   buffer.AppendFormatted("   // Bank Methodes\n");
   buffer.AppendFormatted("   void InitMidasBanks();\n");
   for (i=0;i<numOfEvent;i++) {
      if (numOfBank[i]>0) {
         for (j=0;j<numOfBank[i];j++) {
            // Bank Array
            if (bankArrayDigit[i][j]>0) {
               if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
                  buffer.AppendFormatted("   %sStruct* Get%sBankAt(int bankIndex,int index);\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(int bankIndex,int index);\n",bankName[i][j].Data(),bankName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("   %s Get%sBankAt(int bankIndex,int index);\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s GetLast%sBankAt(int bankIndex,int index);\n",bankType[i][j].Data(),bankName[i][j].Data());
               }
               buffer.AppendFormatted("   int Get%sBankEntries(int bankIndex);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   int GetLast%sBankEntries(int bankIndex);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   void Init%sBank(int bankIndex);\n",bankName[i][j].Data());
            }
            // Single Bank
            else {
               if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
                  buffer.AppendFormatted("   %sStruct* Get%sBankAt(int index);\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(int index);\n",bankName[i][j].Data(),bankName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("   %s Get%sBankAt(int index);\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s GetLast%sBankAt(int index);\n",bankType[i][j].Data(),bankName[i][j].Data());
               }
               buffer.AppendFormatted("   int Get%sBankEntries();\n",bankName[i][j].Data());
               buffer.AppendFormatted("   int GetLast%sBankEntries();\n",bankName[i][j].Data());
               buffer.AppendFormatted("   void Init%sBank();\n",bankName[i][j].Data());
            }
            buffer.AppendFormatted("   bool is%sBankActive() { return f%sBankActive; };\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   void Set%sBankActive(bool flag) { f%sBankActive = flag; };\n",bankName[i][j].Data(),bankName[i][j].Data());
         }
      }
      buffer.AppendFormatted("   bool is%sEventActive() { return f%sEventActive; };\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("   void Set%sEventActive(bool flag) { f%sEventActive = flag; };\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("\n");
   }

   // protected
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   bool  InitODB();\n");
   buffer.AppendFormatted("   bool  InitHeader();\n");
   buffer.AppendFormatted("   bool  IsActiveEventID(int id);\n");
   buffer.AppendFormatted("   void* ByteSwapStruct( char* aName, void* pData );\n");

   buffer.AppendFormatted("\n");
   // Footer

   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sMidasDAQ_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}
bool ROMEBuilder::WriteRomeDAQCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;

   ROMEString format;

   // File name
   cppFile.SetFormatted("%s/src/framework/%sRomeDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   romeDep.AppendFormatted(" include/framework/%sAnalyzer.h",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sRomeDAQ.h>\n",shortCut.Data());
   romeDep.AppendFormatted(" include/framework/%sRomeDAQ.h",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sRomeDAQ::%sRomeDAQ() { }\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");

   // Connect Trees
   int iFold=0,j,k;
   buffer.AppendFormatted("// Connect Trees\n");
   buffer.AppendFormatted("void %sRomeDAQ::ConnectTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TBranchElement *bb;\n");
   for (i=0;i<numOfTree;i++) {
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         buffer.AppendFormatted("   bb = (TBranchElement*)gAnalyzer->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"%s\");\n",i,branchName[i][j].Data());
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("   bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         }
         else {
            buffer.AppendFormatted("   bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         }
         buffer.AppendFormatted("   bb = (TBranchElement*)gAnalyzer->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"Info\");\n",i);
         buffer.AppendFormatted("   bb->SetAddress(&this->fTreeInfo);\n");
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}
bool ROMEBuilder::WriteRomeDAQH() {
   ROMEString hFile;
   ROMEString buffer;

   // File name
   hFile.SetFormatted("%s/include/framework/%sRomeDAQ.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sRomeDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sRomeDAQ_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#include <ROMERomeDAQ.h>\n");

   // Class
   buffer.AppendFormatted("\nclass %sRomeDAQ : public ROMERomeDAQ\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sRomeDAQ();\n",shortCut.Data());

   // methods
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   void ConnectTrees();\n");

   buffer.AppendFormatted("\n");
   // Footer

   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sRomeDAQ_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}
bool ROMEBuilder::WriteDAQCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString format;
   struct stat buf;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iDAQ=0;iDAQ<numOfDAQ;iDAQ++) {
      // File name
      cppFile.SetFormatted("%s/src/framework/%s%s.cpp",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

      if( stat( cppFile.Data(), &buf )) {

         buffer.Resize(0);

         buffer.AppendFormatted("#include <include/framework/%s%s.h>\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
         buffer.AppendFormatted("#include <include/framework/%sConfig.h>\n",shortCut.Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("%s%s::%s%s()\n",shortCut.Data(),daqName[iDAQ].Data(),shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("bool %s%s::Init()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("bool %s%s::BeginOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("bool %s%s::Event(int event)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("bool %s%s::EndOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("bool %s%s::Terminate()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         // Write File
         WriteFile(cppFile.Data(),buffer.Data(),6);
      }
  }
   return true;
}
bool ROMEBuilder::WriteDAQH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   struct stat buf;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iDAQ=0;iDAQ<numOfDAQ;iDAQ++) {
      // File name
      hFile.SetFormatted("%s/include/framework/%s%s.h",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

      if( stat( hFile.Data(), &buf )) {

         // Description
         buffer.Resize(0);

         // Header
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),daqName[iDAQ].Data());

         buffer.AppendFormatted("#include <ROMEDAQSystem.h>\n");

         // Class
         buffer.AppendFormatted("\nclass %s%s : public ROMEDAQSystem\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");

         buffer.AppendFormatted("protected:\n");
         // Methods
         buffer.AppendFormatted("public:\n");
         // Constructor and Methods
         buffer.AppendFormatted("   %s%s();\n",shortCut.Data(),daqName[iDAQ].Data());

         // Methods
         buffer.AppendFormatted("   int   GetTimeStamp() { return 0; };\n");
         buffer.AppendFormatted("   const char* GetName() { return \"%s\"; };\n",daqName[iDAQ].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   bool Init();\n");
         buffer.AppendFormatted("   bool BeginOfRun();\n");
         buffer.AppendFormatted("   bool Event(int event);\n");
         buffer.AppendFormatted("   bool EndOfRun();\n");
         buffer.AppendFormatted("   bool Terminate();\n");

         buffer.AppendFormatted("};\n\n");


         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),daqName[iDAQ].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
  }
   return true;
}
bool ROMEBuilder::WriteDBCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString format;
   struct stat buf;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iDB=0;iDB<numOfDB;iDB++) {
      // File name
      cppFile.SetFormatted("%s/src/framework/%s%sDataBase.cpp",outDir.Data(),shortCut.Data(),dbName[iDB].Data());

      if( stat( cppFile.Data(), &buf )) {

         buffer.Resize(0);

         buffer.AppendFormatted("#include <include/framework/%s%sDataBase.h>\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("%s%sDataBase::%s%sDataBase()\n",shortCut.Data(),dbName[iDB].Data(),shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("%s%sDataBase::~%s%sDataBase()\n",shortCut.Data(),dbName[iDB].Data(),shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("bool %s%sDataBase::Init(const char* name,const char* path,const char* connection)\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("bool %s%sDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber)\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("bool %s%sDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber)\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         // Write File
         WriteFile(cppFile.Data(),buffer.Data(),6);
      }
  }
   return true;
}
bool ROMEBuilder::WriteDBH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   struct stat buf;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iDB=0;iDB<numOfDB;iDB++) {
      // File name
      hFile.SetFormatted("%s/include/framework/%s%sDataBase.h",outDir.Data(),shortCut.Data(),dbName[iDB].Data());

      if( stat( hFile.Data(), &buf )) {

         // Description
         buffer.Resize(0);

         // Header
         buffer.AppendFormatted("#ifndef %s%sDataBase_H\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("#define %s%sDataBase_H\n\n",shortCut.Data(),dbName[iDB].Data());

         buffer.AppendFormatted("#include <ROMEDataBase.h>\n");

         // Class
         buffer.AppendFormatted("\nclass %s%sDataBase : public ROMEDataBase\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");

         buffer.AppendFormatted("protected:\n");
         // Methods
         buffer.AppendFormatted("public:\n");
         // Constructor and Methods
         buffer.AppendFormatted("   %s%sDataBase();\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("   ~%s%sDataBase();\n",shortCut.Data(),dbName[iDB].Data());

         // Methods
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   bool   Init(const char* name,const char* path,const char* connection);\n");
         buffer.AppendFormatted("   bool   Read(ROMEStr2DArray *values,const char *path,int runNumber);\n");
         buffer.AppendFormatted("   bool   Write(ROMEStr2DArray* values,const char *path,int runNumber);\n");
         buffer.AppendFormatted("   char*  GetType() { return \"%s\"; }\n",dbName[iDB].Data());
         buffer.AppendFormatted("   char*  GetDescription() { return \"%s\"; }\n",dbDescription[iDB].Data());

         buffer.AppendFormatted("};\n\n");


         buffer.AppendFormatted("#endif   // %s%sDataBase_H\n",shortCut.Data(),dbName[iDB].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
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
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (typeLen<(int)steerName[numTask][steerChildren[numTask][numSteer][i]].Length()+1) typeLen = steerName[numTask][steerChildren[numTask][numSteer][i]].Length()+1;
      if (nameLen<(int)steerName[numTask][steerChildren[numTask][numSteer][i]].Length()) nameLen = steerName[numTask][steerChildren[numTask][numSteer][i]].Length();
   }
   if (tab==0)
      sc = shortCut;
   else
      sc = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");

   buffer.AppendFormatted("\n%sclass %s%s\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("%s{\n",blank.Data());

   buffer.AppendFormatted("%spublic:\n",blank.Data());

   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      WriteSteeringClass(buffer,steerChildren[numTask][numSteer][i],numTask,tab+1);
   }

   buffer.AppendFormatted("%sprotected:\n",blank.Data());

   // Fields
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen+5-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldComment[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds f%%s[%%s];%%%ds %%s\n",typeLen,nameLen+3-steerFieldName[numTask][numSteer][j].Length()-steerFieldArraySize[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldArraySize[numTask][numSteer][j].Data(),"",steerFieldComment[numTask][numSteer][j].Data());
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         format.SetFormatted("%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen+1-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds **f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("\n%spublic:\n",blank.Data());
   // Constructor
   buffer.AppendFormatted("%s   %s%s() { ",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (steerFieldArraySize[numTask][numSteer][j]!="1") {
         buffer.AppendFormatted("int j; ");
         break;
      }
   }
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         if (steerFieldType[numTask][numSteer][j]=="std::string")
            buffer.AppendFormatted("f%s = %s; ",steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
         else
            buffer.AppendFormatted("f%s = (%s)%s; ",steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
      }
      else {
         if (steerFieldType[numTask][numSteer][j]=="std::string")
            buffer.AppendFormatted("for (j=0;j<%s;j++) f%s[j] = %s; ",steerFieldArraySize[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
         else
            buffer.AppendFormatted("for (j=0;j<%s;j++) f%s[j] = (%s)%s; ",steerFieldArraySize[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]!="1") {
         buffer.AppendFormatted("int i; ");
         break;
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         buffer.AppendFormatted("f%s = new %s(); ",steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         buffer.AppendFormatted("f%s = new %s*[%s]; for (i=0;i<%s;i++) f%s[i] = new %s(); ",steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("};\n");
   // Getters
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen+7-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds Get%%sAt(int i)%%%ds { return f%%s[i]; };\n",typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         format.SetFormatted("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen+7-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds *Get%%sAt(int i)%%%ds { return f%%s[i]; };\n",typeLen-1,nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   // Setters
   buffer.AppendFormatted("\n");
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   void Set%%s(%%s %%s)%%%ds { f%%s = %%s; };\n",2*nameLen+typeLen+8-2*steerFieldName[numTask][numSteer][j].Length()-steerFieldType[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   void Set%%sAt(int i,%%s %%s)%%%ds { f%%s[i] = %%s; };\n",2*nameLen+typeLen-2*steerFieldName[numTask][numSteer][j].Length()-steerFieldType[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data());
      }
   }

   // Footer
   buffer.AppendFormatted("%s};\n\n",blank.Data());

   return true;
}
bool ROMEBuilder::WriteSteeringConfigClass(ROMEString &buffer,int numSteer,int numTask,int tab) {
   ROMEString format;
   ROMEString sc;
   ROMEString blank="";
   bool intDefined = false;
   int i,k;
   for (i=0;i<tab;i++)
      blank.Append("   ");

   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   ROMEString  f%s;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s   bool        f%sModified;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      }
      else {
         buffer.AppendFormatted("%s   ROMEString  f%s[%s];\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldArraySize[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s   bool        f%sModified[%s];\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldArraySize[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s   bool        f%sArrayModified;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      }
   }
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   class %s {\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("%s   public:\n",blank.Data());
      WriteSteeringConfigClass(buffer,steerChildren[numTask][numSteer][k],numTask,tab+1);
      buffer.AppendFormatted("%s   };\n",blank.Data());
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         buffer.AppendFormatted("%s   %s *f%s;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s   bool f%sModified;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      }
      else {
         buffer.AppendFormatted("%s   %s **f%s;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s   bool *f%sModified;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s   bool f%sArrayModified;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      }
   }
   buffer.AppendFormatted("%spublic:\n",blank.Data());
   buffer.AppendFormatted("%s   %s() {\n",blank.Data(),steerName[numTask][numSteer].Data());
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (steerFieldArraySize[numTask][numSteer][k]!="1") {
         buffer.AppendFormatted("%s      int i;\n",blank.Data());
         intDefined = true;
         break;
      }
   }
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s      f%sModified = false;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      }
      else {
         buffer.AppendFormatted("%s      for (i=0;i<%s;i++)\n",blank.Data(),steerFieldArraySize[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s         f%sModified[i] = false;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      f%sArrayModified = false;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      }
   }
   for (k=0;k<numOfSteerChildren[numTask][numSteer] && !intDefined;k++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]!="1") {
         buffer.AppendFormatted("%s      int i;\n",blank.Data());
         break;
      }
   }
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         buffer.AppendFormatted("%s      f%sModified = false;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      f%s = new %s();\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      }
      else {
         buffer.AppendFormatted("%s      f%sModified = new bool[%s];\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      f%s = new %s*[%s];\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      for (i=0;i<%s;i++) {\n",blank.Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s         f%sModified[i] = false;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s         f%s[i] = new %s();\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      }\n",blank.Data());
         buffer.AppendFormatted("%s      f%sArrayModified = false;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      }
   }
   buffer.AppendFormatted("%s   }\n",blank.Data());
   return true;
}
bool ROMEBuilder::WriteSteeringConfigRead(ROMEString &buffer,int numSteer,int numTask,ROMEXML *xml,ROMEString& path,ROMEString& pointer,ROMEString& classPath,ROMEString& index,ROMEString& blank,int *indexCounter) {
   int k;
   ROMEString pathT;
   ROMEString pointerT;
   ROMEString classPathT;
   ROMEString indexT;
   ROMEString blankT;

   ROMEString currentIndex;
   ROMEString currentPointer = pointer;
   if (pointer[pointer.Length()-1]==']') {
      currentIndex = pointer(pointer.Last('['),pointer.Length()-1-pointer.Last('['))+"]";
      currentPointer = pointer(0,pointer.Last('['));
   }

   buffer.AppendFormatted("%s   %sModified%s = false;\n",blank.Data(),currentPointer.Data(),currentIndex.Data());
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Field\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   buf.SetFormatted(%s/SteeringParameterField[SPName='%s']/SPValue\"%s);\n",blank.Data(),path.Data(),steerFieldName[numTask][numSteer][k].Data(),index.Data());
         buffer.AppendFormatted("%s   xml->GetPathValue(buf.Data(),%s->f%s,\"\");\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s   if (%s->f%s==\"\")\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      %s->f%sModified = false;\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s   else {\n",blank.Data());
         buffer.AppendFormatted("%s      %s->f%sModified = true;\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      %sModified%s = true;\n",blank.Data(),currentPointer.Data(),currentIndex.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
      else {
         buffer.AppendFormatted("%s   for (i=0;i<%s;i++) {\n",blank.Data(),steerFieldArraySize[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      buf.SetFormatted(%s/SteeringParameterField[SPName='%s']/SPFieldArray[SPFieldArrayIndex=%%d]/SPFieldArrayValue\"%s,i);\n",blank.Data(),path.Data(),steerFieldName[numTask][numSteer][k].Data(),index.Data());
         buffer.AppendFormatted("%s      xml->GetPathValue(buf.Data(),%s->f%s[i],\"\");\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      if (%s->f%s[i]==\"\")\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s         %s->f%sModified[i] = false;\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      else {\n",blank.Data());
         buffer.AppendFormatted("%s         %s->f%sModified[i] = true;\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s         %s->f%sArrayModified = true;\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s         %sModified%s = true;\n",blank.Data(),currentPointer.Data(),currentIndex.Data());
         buffer.AppendFormatted("%s      }\n",blank.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Group\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pathT = path;
      pointerT = pointer;
      classPathT = classPath;
      indexT = index;
      blankT = blank;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         path.AppendFormatted("/SteeringParameterGroup[SPGroupName='%s']",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         classPath.AppendFormatted("::%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         WriteSteeringConfigRead(buffer,steerChildren[numTask][numSteer][k],numTask,xml,path,pointer,classPath,index,blank,indexCounter);
         path = pathT;
         pointer = pointerT;
         classPath = classPathT;
         index = indexT;
         blank = blankT;
         buffer.AppendFormatted("%s   if (%s->f%sModified)\n",blank.Data(),pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      %sModified%s = true;\n",blank.Data(),currentPointer.Data(),currentIndex.Data());
      }
      else {
         (*indexCounter)++;
         buffer.AppendFormatted("%s   for (int i%d=0;i%d<%s;i%d++) {\n",blank.Data(),*indexCounter,*indexCounter,steerArraySize[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         index.AppendFormatted(",i%d",*indexCounter);
         blank.AppendFormatted("   ");
         path.AppendFormatted("/SteeringParameterGroup[SPGroupName='%s']/SPGroupArray[SPGroupArrayIndex=%%d]",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         pointer.AppendFormatted("->f%s[i%d]",steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         classPath.AppendFormatted("::%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         WriteSteeringConfigRead(buffer,steerChildren[numTask][numSteer][k],numTask,xml,path,pointer,classPath,index,blank,indexCounter);
         path = pathT;
         pointer = pointerT;
         classPath = classPathT;
         index = indexT;
         blank = blankT;
         buffer.AppendFormatted("%s      if (%s->f%sModified[i%d])\n",blank.Data(),pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         buffer.AppendFormatted("%s         %sModified%s = true;\n",blank.Data(),currentPointer.Data(),currentIndex.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
   }
   return true;
}

bool ROMEBuilder::WriteSteeringConfigSet(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer,ROMEString& blank,int *indexCounter) {
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString blankT;
   ROMEString decodedValue;
   ROMEString value;
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Field\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   if (fConfigData[modIndex]%s->f%sModified) {\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         value.SetFormatted("fConfigData[index]%s->f%s",pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         setValue(&decodedValue,"",value.Data(),steerFieldType[numTask][numSteer][k].Data(),1);
         if (steerFieldType[numTask][numSteer][k]=="std::string")
            buffer.AppendFormatted("%s      %s->Set%s(%s.Data());\n",blank.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),value.Data());
         else
            buffer.AppendFormatted("%s      %s->Set%s((%s)%s);\n",blank.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
      else {
         buffer.AppendFormatted("%s   for (i=0;i<%s;i++) {\n",blank.Data(),steerFieldArraySize[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      if (fConfigData[modIndex]%s->f%sModified[i]) {\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         value.SetFormatted("fConfigData[index]%s->f%s[i]",pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         setValue(&decodedValue,"",value.Data(),steerFieldType[numTask][numSteer][k].Data(),1);
         if (steerFieldType[numTask][numSteer][k]=="std::string")
            buffer.AppendFormatted("%s         %s->Set%sAt(i,%s.Data());\n",blank.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),value.Data());
         else
            buffer.AppendFormatted("%s         %s->Set%sAt(i,(%s)%s);\n",blank.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
         buffer.AppendFormatted("%s      }\n",blank.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Group\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      blankT = blank;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         WriteSteeringConfigSet(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer,blank,indexCounter);
      }
      else {
         (*indexCounter)++;
         buffer.AppendFormatted("%s   for (int i%d=0;i%d<%s;i%d++) {\n",blank.Data(),*indexCounter,*indexCounter,steerArraySize[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         pointer.AppendFormatted("->f%s[i%d]",steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         steerPointer.AppendFormatted("->Get%sAt(i%d)",steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         blank.AppendFormatted("   ");
         WriteSteeringConfigSet(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer,blank,indexCounter);
         buffer.AppendFormatted("%s   }\n",blankT.Data());
      }
      pointer = pointerT;
      steerPointer = steerPointerT;
      blank = blankT;
   }
   return true;
}
bool ROMEBuilder::WriteSteeringConfigWrite(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer,int tab,int *indexCounter) {
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   ROMEString blank = "";
   for (int i=0;i<tab;i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Field\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   if (%s->f%sModified || index==0) {\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterField\");\n",blank.Data());
         buffer.AppendFormatted("%s      xml->WriteElement(\"SPName\",\"%s\");\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      if (index==0) {\n",blank.Data());
         GetFormat(&value,steerFieldType[numTask][numSteer][k].Data());
         if (steerFieldType[numTask][numSteer][k]=="TString")
            buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s().Data());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         else if (steerFieldType[numTask][numSteer][k]=="std::string")
            buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s().c_str());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         else if (isBoolType(steerFieldType[numTask][numSteer][k].Data())) {
            buffer.AppendFormatted("%s         if (%s->Get%s())\n",blank.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("%s            value = \"true\";\n",blank.Data());
            buffer.AppendFormatted("%s         else\n",blank.Data());
            buffer.AppendFormatted("%s            value = \"false\";\n",blank.Data());
         }
         else
            buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s         xml->WriteElement(\"SPValue\",value.Data());\n",blank.Data());
         buffer.AppendFormatted("%s      }\n",blank.Data());
         buffer.AppendFormatted("%s      else if (%s->f%sModified)\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s         xml->WriteElement(\"SPValue\",%s->f%s.Data());\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
      else {
         buffer.AppendFormatted("%s   if (%s->f%sArrayModified || index==0) {\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterField\");\n",blank.Data());
         buffer.AppendFormatted("%s      xml->WriteElement(\"SPName\",\"%s\");\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      for (i=0;i<%s;i++) {\n",blank.Data(),steerFieldArraySize[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s         if (%s->f%sModified[i] || index==0) {\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s            xml->StartElement(\"SPFieldArray\");\n",blank.Data());
         buffer.AppendFormatted("%s            value.SetFormatted(\"%%d\",i);\n",blank.Data());
         buffer.AppendFormatted("%s            xml->WriteElement(\"SPFieldArrayIndex\",value.Data());\n",blank.Data());
         buffer.AppendFormatted("%s            if (index==0) {\n",blank.Data());
         GetFormat(&value,steerFieldType[numTask][numSteer][k].Data());
         if (steerFieldType[numTask][numSteer][k]=="TString")
            buffer.AppendFormatted("%s               value.SetFormatted(\"%s\",%s->Get%sAt(i).Data());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         else if (steerFieldType[numTask][numSteer][k]=="std::string")
            buffer.AppendFormatted("%s               value.SetFormatted(\"%s\",%s->Get%sAt(i).c_str());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         else if (isBoolType(steerFieldType[numTask][numSteer][k].Data())) {
            buffer.AppendFormatted("%s               if (%s->Get%sAt(i))\n",blank.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("%s                  value = \"true\";\n",blank.Data());
            buffer.AppendFormatted("%s               else\n",blank.Data());
            buffer.AppendFormatted("%s                  value = \"false\";\n",blank.Data());
         }
         else
            buffer.AppendFormatted("%s               value.SetFormatted(\"%s\",%s->Get%sAt(i));\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"SPFieldArrayValue\",value.Data());\n",blank.Data());
         buffer.AppendFormatted("%s            }\n",blank.Data());
         buffer.AppendFormatted("%s            else if (%s->f%sModified)\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"SPFieldArrayValue\",%s->f%s[i].Data());\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s            xml->EndElement();\n",blank.Data());
         buffer.AppendFormatted("%s         }\n",blank.Data());
         buffer.AppendFormatted("%s      }\n",blank.Data());
         buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Group\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         buffer.AppendFormatted("%s   if (%s->f%sModified || index==0) {\n",blank.Data(),pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterGroup\");\n",blank.Data());
         buffer.AppendFormatted("%s      xml->WriteElement(\"SPGroupName\",\"%s\");\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         pointerT = pointer;
         steerPointerT = steerPointer;
         pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         WriteSteeringConfigWrite(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer,tab+1,indexCounter);
         pointer = pointerT;
         steerPointer = steerPointerT;
         buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
      }
      else {
         (*indexCounter)++;
         buffer.AppendFormatted("%s   for (int i%d=0;i%d<%s;i%d++) {\n",blank.Data(),*indexCounter,*indexCounter,steerArraySize[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         blank.Append("   ");
         tab++;
         buffer.AppendFormatted("%s   if (%s->f%sArrayModified || index==0) {\n",blank.Data(),pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterGroup\");\n",blank.Data());
         buffer.AppendFormatted("%s      xml->WriteElement(\"SPGroupName\",\"%s\");\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         buffer.AppendFormatted("%s      if (%s->f%sModified[i%d] || index==0) {\n",blank.Data(),pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         buffer.AppendFormatted("%s         xml->StartElement(\"SPGroupArray\");\n",blank.Data());
         buffer.AppendFormatted("%s         value.SetFormatted(\"%%d\",i%d);\n",blank.Data(),*indexCounter);
         buffer.AppendFormatted("%s         xml->WriteElement(\"SPGroupArrayIndex\",value.Data());\n",blank.Data());
         pointerT = pointer;
         steerPointerT = steerPointer;
         pointer.AppendFormatted("->f%s[i%d]",steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         steerPointer.AppendFormatted("->Get%sAt(i%d)",steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),*indexCounter);
         WriteSteeringConfigWrite(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer,tab+2,indexCounter);
         pointer = pointerT;
         steerPointer = steerPointerT;
         buffer.AppendFormatted("%s         xml->EndElement();\n",blank.Data());
         buffer.AppendFormatted("%s      }\n",blank.Data());
         buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
         buffer.AppendFormatted("%s   }\n",blank.Data());
         tab--;
         blank.Resize(blank.Length()-3);
         buffer.AppendFormatted("%s   }\n",blank.Data());

      }
   }
   return true;
}

bool ROMEBuilder::WriteSteeringReadParameters(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer) {
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if(steerFieldCLOption[numTask][numSteer][k].Length()){
         if (steerFieldArraySize[numTask][numSteer][k]=="1") {
            buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("   if (option == \"-%s\") {\n",steerFieldCLOption[numTask][numSteer][k].Data());
            if (isBoolType(steerFieldType[numTask][numSteer][k].Data())){
               buffer.AppendFormatted("      %s->Set%s(true);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
            }
            else{
               buffer.AppendFormatted("      if(!strlen(value))\n");
               buffer.AppendFormatted("         gAnalyzer->Println(\"warning: %s might not be specified properly.\");\n",steerFieldName[numTask][numSteer][k].Data());
               setValue(&decodedValue,"","value",steerFieldType[numTask][numSteer][k].Data(),1);
               buffer.AppendFormatted("      %s->Set%s((%s)%s);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
               buffer.AppendFormatted("      i++;\n");
            }
            buffer.AppendFormatted("      return true;\n");
            buffer.AppendFormatted("   }\n");
         }
         else{
            buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("   for(int i_%s=0;i_%s<%s;i_%s++){\n",steerFieldCLOption[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data(),steerFieldArraySize[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      tmp = \"-%s\";\n",steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      tmp += i_%s;\n",steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      if (option == tmp) {\n");
            if (isBoolType(steerFieldType[numTask][numSteer][k].Data())){
               buffer.AppendFormatted("         %s->Set%sAt(i_%s,true);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data());
            }
            else{
               buffer.AppendFormatted("         if(!strlen(value))\n");
               buffer.AppendFormatted("            gAnalyzer->Println(\"warning: %s might not be specified properly.\");\n",steerFieldName[numTask][numSteer][k].Data());
               setValue(&decodedValue,"","value",steerFieldType[numTask][numSteer][k].Data(),1);
               buffer.AppendFormatted("         %s->Set%sAt(i_%s,(%s)%s);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
               buffer.AppendFormatted("         i++;\n");
            }
            buffer.AppendFormatted("         return true;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringReadParameters(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}

bool ROMEBuilder::WriteSteeringParameterUsage(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer) {
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   ROMEString format;
   ROMEString arrayIndex;
   int nspace;
   char *cstop=NULL;
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if(steerFieldCLOption[numTask][numSteer][k].Length()){
         arrayIndex = "";
         if (steerFieldArraySize[numTask][numSteer][k]!="1")
            arrayIndex.SetFormatted("[0-%d]",strtol(steerFieldArraySize[numTask][numSteer][k].Data(),&cstop,10)-1);
         nspace = 8-steerFieldCLOption[numTask][numSteer][k].Length()-arrayIndex.Length();
         if(nspace<1)
            nspace = 1;
         format.SetFormatted("   gAnalyzer->Println(\"  -%%s%%s%%%ds%%s",nspace);
         buffer.AppendFormatted(format.Data(),steerFieldCLOption[numTask][numSteer][k].Data(),arrayIndex.Data(),"",steerFieldCLDescription[numTask][numSteer][k].Data());
         if(isBoolType(steerFieldCLOption[numTask][numSteer][k].Data()))
            buffer.AppendFormatted(" (no Argument)");
         buffer.AppendFormatted("\");\n");
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringParameterUsage(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}

int  ROMEBuilder::WriteSteeringInterpreterCode(ROMEString &buffer,int codeNumber,int numSteer,int numTask,ROMEString& path,int tab) {
   ROMEString pathT;
   ROMEString blank = "";
   for (int i=0;i<tab;i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   if (path==\"%s/%s\")\n",blank.Data(),path.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return %d;\n",blank.Data(),codeNumber);
         codeNumber++;
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         pathT = path;
         path.AppendFormatted("/%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber,steerChildren[numTask][numSteer][k],numTask,path,tab);
         path = pathT;
      }
   }
   return codeNumber;
}
int  ROMEBuilder::WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,int codeNumber,int numSteer,int numTask,ROMEString& steerPointer,int tab) {
   ROMEString steerPointerT;
   ROMEString str;
   ROMEString blank = "";
   ROMEString stringBuffer = "buffer";
   for (int i=0;i<tab;i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   case %d:\n",blank.Data(),codeNumber);
         str.SetFormatted("%s->Get%s()",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         stringBuffer = "buffer";
         buffer.AppendFormatted("%s      return %s;\n",blank.Data(),convertType(str.Data(),steerFieldType[numTask][numSteer][k].Data(),type,stringBuffer).Data());
         codeNumber++;
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         steerPointerT = steerPointer;
         steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         codeNumber = WriteSteeringInterpreterValue(buffer,type,codeNumber,steerChildren[numTask][numSteer][k],numTask,steerPointer,tab);
         steerPointer = steerPointerT;
      }
   }
   return codeNumber;
}
void ROMEBuilder::WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName) {
   int i,j;
   ROMEString tmp;
   ROMEString stringBuffer = "buffer";
   if (!strcmp(type,"ROMEString&"))
      buffer.AppendFormatted("%s %sAnalyzer::GetObjectInterpreter%sValue(int code,%s defaultValue,%s %s) {\n",type,shortCut.Data(),fctName,type,type,stringBuffer.Data());
   else {
      buffer.AppendFormatted("%s %sAnalyzer::GetObjectInterpreter%sValue(int code,%s defaultValue) {\n",type,shortCut.Data(),fctName,type);
      buffer.AppendFormatted("   ROMEString buffer;\n");
   }
   buffer.AppendFormatted("   char *cstop=NULL;\n");
   buffer.AppendFormatted("   switch (code) {\n");
   buffer.AppendFormatted("      case -1:\n");
   buffer.AppendFormatted("         return defaultValue;\n");
   ROMEString steerPointer = "gAnalyzer->GetGSP()";
   int codeNumber = WriteSteeringInterpreterValue(buffer,type,0,0,numOfTaskHierarchy,steerPointer,1);
   for (i=0;i<numOfFolder;i++) {
      if (folderArray[i]=="1" && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]==0 && !isFolder(valueType[i][j].Data())) {
               buffer.AppendFormatted("      case %d:\n",codeNumber);
               tmp.SetFormatted("gAnalyzer->Get%s()->Get%s()",folderName[i].Data(),valueName[i][j].Data());
               stringBuffer = "buffer";
               buffer.AppendFormatted("         return %s;\n",convertType(tmp.Data(),valueType[i][j].Data(),type,stringBuffer).Data());
               codeNumber++;
            }
         }
      }
   }
   buffer.AppendFormatted("   }\n");
   if (!strcmp(type,"ROMEString&"))
      buffer.AppendFormatted("   return %s;\n",stringBuffer.Data());
   else
      buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
}
void ROMEBuilder::WriteReadDataBaseFolder(ROMEString &buffer,int numFolder,int type) {
   int j,k;
   ROMEString valueString;
   ROMEString buf;
   ROMEString blank = "";
   ROMEString iValue = "0";
   if (type==2) {
      blank = "   ";
      iValue= "i";
   }
   if (type==2)
      buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   int j;\n");
   buffer.AppendFormatted("   ROMEString path;\n");
   buffer.AppendFormatted("   ROMEString name;\n");
   buffer.AppendFormatted("   ROMEString buffer[%d];\n",maxNumberOfPathObjectInterpreterCodes);
   buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
   buffer.AppendFormatted("   char *cstop=NULL;\n");
   buffer.AppendFormatted("   int number;\n");
   buffer.AppendFormatted("   if (IsEventBasedDataBase())\n");
   buffer.AppendFormatted("      number = gAnalyzer->GetCurrentEventNumber();\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      number = gAnalyzer->GetCurrentRunNumber();\n");
   for (j=0;j<numOfValue[numFolder];j++) {
      if( valueDimension[numFolder][j]>1 || valueArray[numFolder][j][0] == "variable")
         continue;
      if (folderArray[numFolder]=="1" && type==1 || folderArray[numFolder]!="1" && type==2 && !folderSupport[numFolder]) {
         buffer.AppendFormatted("   values->RemoveAll();\n");
         buffer.AppendFormatted("   if (strlen(gAnalyzer->Get%s_%sDBName())==0)\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("      name.SetFormatted(\"%s\");\n",valueDBName[numFolder][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      name.SetFormatted(gAnalyzer->Get%s_%sDBName());\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("   if (strlen(gAnalyzer->Get%s_%sDBPath())==0)\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         if(valueDBPath[numFolder][j].Length())
            buffer.AppendFormatted("      path.SetFormatted(%s);\n",valueDBPath[numFolder][j].Data());
         else
            buffer.AppendFormatted("      path.SetFormatted(\"\");\n");

         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      path.SetFormatted(gAnalyzer->Get%s_%sDBPath()",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         for (k=0;k<maxNumberOfPathObjectInterpreterCodes;k++)
            buffer.AppendFormatted(",gAnalyzer->GetObjectInterpreterCharValue(gAnalyzer->Get%s_%sDBCodeAt(%d),buffer[%d],buffer[%d]).Data()",folderName[numFolder].Data(),valueName[numFolder][j].Data(),k,k,k);
         buffer.AppendFormatted(");\n");
         buffer.AppendFormatted("   if (name.Length() && path.Length() && isDataBaseActive(name.Data())){\n");
         buffer.AppendFormatted("      if (!this->GetDataBase(name.Data())->Read(values,path,number)) {\n");
         buffer.AppendFormatted("         gAnalyzer->Println(\"   in Folder '%s' Value '%s'.\");\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("         return false;\n");
         buffer.AppendFormatted("      }\n");
         if (type==2)
            buffer.AppendFormatted("      for (i=0;i<f%sFolders->GetEntries();i++) {\n",folderName[numFolder].Data());
         if (valueDimension[numFolder][j]==0) {
            buffer.AppendFormatted("%s      if (values->At(%s,0).Length()!=0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s,0).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),valueType[numFolder][j].Data(),1);
            if (type==1) {
               buffer.AppendFormatted("%s         f%sFolder->Set%s((%s)%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("%s      else\n",blank.Data());
               buffer.AppendFormatted("%s         f%sFolder->Set%s(%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            else {
               buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("         else\n");
               buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%s(%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
         }
         else if (valueDimension[numFolder][j]==1){
            buffer.AppendFormatted("%s      for (j=0;j<%s;j++) {\n",blank.Data(),valueArray[numFolder][j][0].Data());
            buffer.AppendFormatted("%s         if (values->At(%s,j).Length()!=0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s,j).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),TArray2StandardType(valueType[numFolder][j]),1);
            if (type==1) {
               buffer.AppendFormatted("%s            f%sFolder->Set%sAt(j,(%s)%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),TArray2StandardType(valueType[numFolder][j]),buf.Data());
               buffer.AppendFormatted("%s         else\n",blank.Data());
               buffer.AppendFormatted("%s            f%sFolder->Set%sAt(j,%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            else {
               buffer.AppendFormatted("               ((%s%s*)f%sFolders->At(i))->Set%sAt(j,(%s)%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),TArray2StandardType(valueType[numFolder][j]),buf.Data());
               buffer.AppendFormatted("            else\n");
               buffer.AppendFormatted("               ((%s%s*)f%sFolders->At(i))->Set%sAt(j,%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            buffer.AppendFormatted("%s      }\n",blank.Data());
         }
         else {
            cout<<"Warning: multiple dimension field can not have database connection."<<endl;
            return;
         }
         if (type==2)
            buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   values->RemoveAll();\n");
   buffer.AppendFormatted("   delete values;\n");
}

void ROMEBuilder::WriteFolderGetter(ROMEString &buffer,int numFolder,int scl,int nameLen,int typeLen) {
   if (folderSupport[numFolder])
      return;
   ROMEString format;
   if (numOfValue[numFolder] > 0) {
      int lt = typeLen-folderName[numFolder].Length()-scl+nameLen-folderName[numFolder].Length();
      if (folderArray[numFolder]=="1") {
         format.SetFormatted("   %%s%%s*%%%ds  Get%%s()%%%ds { return f%%sFolder;%%%ds };\n",typeLen-folderName[numFolder].Length()-scl,11+nameLen-folderName[numFolder].Length(),15+typeLen+nameLen-folderName[numFolder].Length());
         buffer.AppendFormatted(format.Data(),shortCut.Data(),folderName[numFolder].Data(),"",folderName[numFolder].Data(),"",folderName[numFolder].Data(),"");
         format.SetFormatted("   %%s%%s**%%%ds Get%%sAddress()%%%ds { return &f%%sFolder;%%%ds };\n",typeLen-folderName[numFolder].Length()-scl,4+nameLen-folderName[numFolder].Length(),14+typeLen+nameLen-folderName[numFolder].Length());
         buffer.AppendFormatted(format.Data(),shortCut.Data(),folderName[numFolder].Data(),"",folderName[numFolder].Data(),"",folderName[numFolder].Data(),"");
      }
      else if (folderArray[numFolder]=="variable") {
         format.SetFormatted("   %%s%%s*%%%ds  Get%%sAt(int index)\n",typeLen-folderName[numFolder].Length()-scl);
         buffer.AppendFormatted(format.Data(),shortCut.Data(),folderName[numFolder].Data(),"",folderName[numFolder].Data());
         buffer.AppendFormatted("   { if (f%sFolders->GetEntries()<=index)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("        for (int i=f%sFolders->GetEntries();i<=index;i++)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("           new((*f%sFolders)[i]) %s%s();\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("     return (%s%s*)f%sFolders->At(index); };\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         format.SetFormatted("   TClonesArray*%%%ds  Get%%ss()%%%ds { return f%%sFolders;%%%ds };\n",typeLen-12,10+nameLen-folderName[numFolder].Length(),14+typeLen+nameLen-folderName[numFolder].Length());
         buffer.AppendFormatted(format.Data(),"",folderName[numFolder].Data(),"",folderName[numFolder].Data(),"");
         format.SetFormatted("   TClonesArray**%%%ds Get%%sAddress()%%%ds { return &f%%sFolders;%%%ds };\n",typeLen-12,4+nameLen-folderName[numFolder].Length(),13+typeLen+nameLen-folderName[numFolder].Length());
         buffer.AppendFormatted(format.Data(),"",folderName[numFolder].Data(),"",folderName[numFolder].Data(),"");
      }
      else {
         format.SetFormatted("   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds { return (%%s%%s*)f%%sFolders->At(index);%%%ds };\n",typeLen-folderName[numFolder].Length()-scl,0+nameLen-folderName[numFolder].Length(),lt);
         buffer.AppendFormatted(format.Data(),shortCut.Data(),folderName[numFolder].Data(),"",folderName[numFolder].Data(),"",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),"");
         format.SetFormatted("   TClonesArray*%%%ds  Get%%ss()%%%ds { return f%%sFolders;%%%ds };\n",typeLen-12,10+nameLen-folderName[numFolder].Length(),14+typeLen+nameLen-folderName[numFolder].Length());
         buffer.AppendFormatted(format.Data(),"",folderName[numFolder].Data(),"",folderName[numFolder].Data(),"");
         format.SetFormatted("   TClonesArray**%%%ds Get%%sAddress()%%%ds { return &f%%sFolders;%%%ds };\n",typeLen-12,4+nameLen-folderName[numFolder].Length(),13+typeLen+nameLen-folderName[numFolder].Length());
         buffer.AppendFormatted(format.Data(),"",folderName[numFolder].Data(),"",folderName[numFolder].Data(),"");
      }
   }
}
bool ROMEBuilder::WriteTaskConfigWrite(ROMEString &buffer,int parentIndex,ROMEString& pointer,int tab) {
   int j,i;
   ROMEString blank = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");

   ROMEString pointerI;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (taskHierarchyParentIndex[i]!=parentIndex)
         continue;
      pointerI = pointer;
      pointerI.AppendFormatted("->f%sTask",taskHierarchyName[i].Data());
      buffer.AppendFormatted("%s      if (fConfigData[index]%sModified || index==0) {\n",blank.Data(),pointerI.Data());
      buffer.AppendFormatted("%s         // %s\n",blank.Data(),pointerI.Data());
      buffer.AppendFormatted("%s         xml->StartElement(\"Task\");\n",blank.Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"TaskName\",\"%s\");\n",blank.Data(),taskHierarchyName[i].Data());
      buffer.AppendFormatted("%s         if (index==0) {\n",blank.Data());
      buffer.AppendFormatted("%s            if (gAnalyzer->Get%s%03dTask()->IsActive())\n",blank.Data(),taskHierarchyName[i].Data(),i);
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"true\");\n",blank.Data());
      buffer.AppendFormatted("%s            else\n",blank.Data());
      buffer.AppendFormatted("%s               xml->WriteElement(\"Active\",\"false\");\n",blank.Data());
      buffer.AppendFormatted("%s         }\n",blank.Data());
      buffer.AppendFormatted("%s         else if (fConfigData[index]%s->fActiveModified)\n",blank.Data(),pointerI.Data());
      buffer.AppendFormatted("%s            xml->WriteElement(\"Active\",fConfigData[index]%s->fActive.Data());\n",blank.Data(),pointerI.Data());
      if (numOfHistos[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("%s         // histograms\n",blank.Data());
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("%s         if (fConfigData[index]%s->f%sHistoModified) {\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            ROMEString tmp;\n",blank.Data());
         buffer.AppendFormatted("%s            xml->StartElement(\"Histogram\");\n",blank.Data());
         buffer.AppendFormatted("%s            xml->WriteElement(\"HistName\",\"%s\");\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fTitleModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistTitle\",fConfigData[index]%s->f%sHisto->fTitle.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fFolderTitleModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistFolderTitle\",fConfigData[index]%s->f%sHisto->fFolderTitle.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fArraySizeModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistArraySize\",fConfigData[index]%s->f%sHisto->fArraySize.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fArrayStartIndexModified) {\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               if (fConfigData[index]%s->f%sHisto->fArrayStartIndex==\"\")\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s                  xml->WriteElement(\"HistArrayStartIndex\",\"0\");\n",blank.Data());
         buffer.AppendFormatted("%s               else\n",blank.Data());
         buffer.AppendFormatted("%s                  xml->WriteElement(\"HistArrayStartIndex\",fConfigData[index]%s->f%sHisto->fArrayStartIndex.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            }\n",blank.Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fXLabelModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistXLabel\",fConfigData[index]%s->f%sHisto->fXLabel.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fYLabelModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistYLabel\",fConfigData[index]%s->f%sHisto->fYLabel.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fZLabelModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistZLabel\",fConfigData[index]%s->f%sHisto->fZLabel.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fXNbinsModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistXNbins\",fConfigData[index]%s->f%sHisto->fXNbins.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fXminModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistXmin\",fConfigData[index]%s->f%sHisto->fXmin.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fXmaxModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistXmax\",fConfigData[index]%s->f%sHisto->fXmax.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         if (histoType[taskHierarchyClassIndex[i]][j].Index("TH2")==0 || histoType[taskHierarchyClassIndex[i]][j].Index("TH3")==0) {
            buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fYNbinsModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s               xml->WriteElement(\"HistYNbins\",fConfigData[index]%s->f%sHisto->fYNbins.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fYminModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s               xml->WriteElement(\"HistYmin\",fConfigData[index]%s->f%sHisto->fYmin.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fYmaxModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s               xml->WriteElement(\"HistYmax\",fConfigData[index]%s->f%sHisto->fYmax.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         }
         if (histoType[taskHierarchyClassIndex[i]][j].Index("TH3")==0) {
            buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fZNbinsModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s               xml->WriteElement(\"HistZNbins\",fConfigData[index]%s->f%sHisto->fZNbins.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fZminModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s               xml->WriteElement(\"HistZmin\",fConfigData[index]%s->f%sHisto->fZmin.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s            if (fConfigData[index]%s->f%sHisto->fZmaxModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("%s               xml->WriteElement(\"HistZmax\",fConfigData[index]%s->f%sHisto->fZmax.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         }
         buffer.AppendFormatted("%s            if (index==0) {\n",blank.Data());
         buffer.AppendFormatted("%s               if (((%sT%s*)gAnalyzer->Get%s%03dTask())->is%sAccumulation())\n",blank.Data(),shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s                  xml->WriteElement(\"HistAccumulate\",\"true\");\n",blank.Data());
         buffer.AppendFormatted("%s               else\n",blank.Data());
         buffer.AppendFormatted("%s                  xml->WriteElement(\"HistAccumulate\",\"false\");\n",blank.Data());
         buffer.AppendFormatted("%s            }\n",blank.Data());
         buffer.AppendFormatted("%s            else if (fConfigData[index]%s->f%sHisto->fAccumulateModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"HistAccumulate\",fConfigData[index]%s->f%sHisto->fAccumulate.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            xml->EndElement();\n",blank.Data());
         buffer.AppendFormatted("%s         }\n",blank.Data());
      }
      // Steering parameter
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("%s         // steering parameters\n",blank.Data());
         buffer.AppendFormatted("%s         if (fConfigData[index]%s->fSteeringModified || index==0) {\n",blank.Data(),pointerI.Data());
         buffer.AppendFormatted("%s            ROMEString value;\n",blank.Data());
         ROMEString pointerT;
         ROMEString steerPointerT;
         int indexCounter = 0;
         pointerT.SetFormatted("fConfigData[index]%s->fSteering",pointerI.Data());
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->Get%s%03dTask())->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i);
         WriteSteeringConfigWrite(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT,3+tab,&indexCounter);
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("%s         }\n",blank.Data());

      WriteTaskConfigWrite(buffer,i,pointerI,tab+1);

      buffer.AppendFormatted("%s         xml->EndElement();\n",blank.Data());
      buffer.AppendFormatted("%s      }\n",blank.Data());
   }
   return true;
}
bool ROMEBuilder::WriteTaskConfigClass(ROMEString &buffer,int parentIndex,int tab) {
   int j,i;
   ROMEString blank = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");

   for (i=0;i<numOfTaskHierarchy;i++) {
      if (taskHierarchyParentIndex[i]!=parentIndex)
         continue;
      buffer.AppendFormatted("%s      class %sTask {\n",blank.Data(),taskHierarchyName[i].Data());
      buffer.AppendFormatted("%s      public:\n",blank.Data());
      buffer.AppendFormatted("%s         ROMEString  fActive;\n",blank.Data());
      buffer.AppendFormatted("%s         bool        fActiveModified;\n",blank.Data());
      if (numOfHistos[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("%s         // histograms\n",blank.Data());
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("%s         class %sHisto {\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s         public:\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fTitle;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fTitleModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fFolderTitle;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fFolderTitleModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fArraySize;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fArraySizeModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fArrayStartIndex;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fArrayStartIndexModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fXLabel;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fXLabelModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fYLabel;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fYLabelModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fZLabel;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fZLabelModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fXNbins;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fXNbinsModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fXmin;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fXminModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fXmax;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fXmaxModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fYNbins;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fYNbinsModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fYmin;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fYminModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fYmax;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fYmaxModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fZNbins;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fZNbinsModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fZmin;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fZminModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fZmax;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fZmaxModified;\n",blank.Data());
         buffer.AppendFormatted("%s            ROMEString  fAccumulate;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fAccumulateModified;\n",blank.Data());
         buffer.AppendFormatted("%s         public:\n",blank.Data());
         buffer.AppendFormatted("%s            %sHisto() {\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               fTitle = \"%s Histogram\";\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               fFolderTitle = \"%s\";\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               fArraySize = \"1\";\n",blank.Data());
         buffer.AppendFormatted("%s               fArrayStartIndex = \"0\";\n",blank.Data());
         buffer.AppendFormatted("%s               fXLabel = \"X\";\n",blank.Data());
         buffer.AppendFormatted("%s               fYLabel = \"Y\";\n",blank.Data());
         buffer.AppendFormatted("%s               fZLabel = \"Z\";\n",blank.Data());
         buffer.AppendFormatted("%s               fXNbins = \"1\";\n",blank.Data());
         buffer.AppendFormatted("%s               fXmin = \"0\";\n",blank.Data());
         buffer.AppendFormatted("%s               fXmax = \"1\";\n",blank.Data());
         buffer.AppendFormatted("%s               fYNbins = \"1\";\n",blank.Data());
         buffer.AppendFormatted("%s               fYmin = \"0\";\n",blank.Data());
         buffer.AppendFormatted("%s               fYmax = \"1\";\n",blank.Data());
         buffer.AppendFormatted("%s               fZNbins = \"1\";\n",blank.Data());
         buffer.AppendFormatted("%s               fZmin = \"0\";\n",blank.Data());
         buffer.AppendFormatted("%s               fZmax = \"1\";\n",blank.Data());
         buffer.AppendFormatted("%s            };\n",blank.Data());
         buffer.AppendFormatted("%s         };\n",blank.Data());
         buffer.AppendFormatted("%s         %sHisto *f%sHisto;\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s         bool     f%sHistoModified;\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
      }
      if (numOfHistos[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("%s         bool     fHistogramsModified;\n",blank.Data());
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("%s         // steering parameters\n",blank.Data());
         buffer.AppendFormatted("%s         class Steering {\n",blank.Data());
         buffer.AppendFormatted("%s         public:\n",blank.Data());
         WriteSteeringConfigClass(buffer,0,taskHierarchyClassIndex[i],tab+3);
         buffer.AppendFormatted("%s         };\n",blank.Data());
         buffer.AppendFormatted("%s         Steering *fSteering;\n",blank.Data());
         buffer.AppendFormatted("%s         bool   fSteeringModified;\n",blank.Data());
      }
      // Constructor
      buffer.AppendFormatted("%s      public:\n",blank.Data());
      buffer.AppendFormatted("%s         %sTask() {\n",blank.Data(),taskHierarchyName[i].Data());
      if (numOfHistos[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("%s            fHistogramsModified = false;\n",blank.Data());
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("%s            f%sHistoModified = false;\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            f%sHisto = new %sHisto();\n",blank.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("%s            fSteeringModified = false;\n",blank.Data());
         buffer.AppendFormatted("%s            fSteering = new Steering();\n",blank.Data());
      }
      for (j=0;j<numOfTaskHierarchy;j++) {
         if (taskHierarchyParentIndex[j]!=i)
            continue;
         buffer.AppendFormatted("%s            f%sTask = new %sTask();\n",blank.Data(),taskHierarchyName[j].Data(),taskHierarchyName[j].Data());
      }
      buffer.AppendFormatted("%s         };\n",blank.Data());
      // Sub classes
      WriteTaskConfigClass(buffer,i,tab+1);
      buffer.AppendFormatted("%s      };\n",blank.Data());
      buffer.AppendFormatted("%s      %sTask *f%sTask;\n",blank.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data());
      buffer.AppendFormatted("%s      bool   f%sTaskModified;\n",blank.Data(),taskHierarchyName[i].Data());
   }
   return true;
}

bool ROMEBuilder::WriteEventLoopCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;

   ROMEString parentt;

   int j,k,iFold=0;

   ROMEString classDescription;
   classDescription.SetFormatted("Framework specific event loop class.");

   ROMEString tmp;
   ROMEString format;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderSupport[i]) {
         if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
         if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
      }
   }


   // File name
   cppFile.SetFormatted("%s/src/framework/%sEventLoop.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#include <direct.h>\n");
   buffer.AppendFormatted("#endif\n");

   buffer.AppendFormatted("#include <sys/stat.h>\n");
   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
   buffer.AppendFormatted("#include <TObjString.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");

   buffer.AppendFormatted("#include <ROMETree.h>\n");
   eventLoopDep.AppendFormatted(" $(ROMESYS)/include/ROMETree.h");
   buffer.AppendFormatted("#include <ROMETreeInfo.h>\n");
   eventLoopDep.AppendFormatted(" $(ROMESYS)/include/ROMETreeInfo.h");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   eventLoopDep.AppendFormatted(" include/framework/%sAnalyzer.h",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sEventLoop.h>\n",shortCut.Data());
   eventLoopDep.AppendFormatted(" include/framework/%sEventLoop.h",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sConfig.h>\n",shortCut.Data());
   eventLoopDep.AppendFormatted(" include/framework/%sConfig.h",shortCut.Data());

   buffer.AppendFormatted("#include <Riostream.h>\n");
   buffer.AppendFormatted("\n");

   // Constructor
   buffer.AppendFormatted("%sEventLoop::%sEventLoop(const char *name,const char *title):ROMEEventLoop(name,title) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("}\n");


   // Tree Initialization
   buffer.AppendFormatted("// Tree initialization\n");
   buffer.AppendFormatted("void %sEventLoop::InitTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TFolder *treeFolder = gAnalyzer->GetMainFolder()->AddFolder(\"Trees\",\"Trees of the %s framework\");\n",shortCut.Data());
   buffer.AppendFormatted("   TTree *tree;\n");
   buffer.AppendFormatted("   ROMEString fileName;\n\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   tree = new TTree(\"%s\",\"%s\");\n",treeName[i].Data(),treeTitle[i].Data());
      buffer.AppendFormatted("   tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000,99);\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"%s%s\",gAnalyzer->Get%sAddress(),%s,%s);\n",branchName[i][j].Data(),shortCut.Data(),folderName[iFold].Data(),branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),branchSplitLevel[i][j].Data());
         }
         else {
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"TClonesArray\",gAnalyzer->Get%sAddress(),%s,%s);\n",branchName[i][j].Data(),branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),branchSplitLevel[i][j].Data());
         }
      }
      buffer.AppendFormatted("   gAnalyzer->AddTree(tree);\n");
      buffer.AppendFormatted("   treeFolder->Add(tree);\n\n");
   }
   buffer.AppendFormatted("}\n\n");

   // clean up folders
   buffer.AppendFormatted("// Delete Unused Folders\n");
   buffer.AppendFormatted("void %sEventLoop::CleanUpFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderNoReset[i] && !folderSupport[i]) {
         if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   int i;\n");
            break;
         }
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderNoReset[i] && !folderSupport[i]) {
         if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=gAnalyzer->Get%ss()->GetEntriesFast()-1;i>=0;i--) {\n",folderName[i].Data());
            buffer.AppendFormatted("      if (((%s%s*)gAnalyzer->Get%sAt(i))->isModified())\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("         break;\n");
            buffer.AppendFormatted("      gAnalyzer->Get%ss()->RemoveAt(i);\n",folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // reset folders
   buffer.AppendFormatted("// Reset Folders\n");
   buffer.AppendFormatted("void %sEventLoop::ResetFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if(numOfValue[i]>0 && !folderSupport[i] && folderArray[i]!="1") {
         buffer.AppendFormatted("   int i;\n");
         break;
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if(numOfValue[i]<=0 || folderSupport[i])
         continue;
      if (folderNoReset[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   gAnalyzer->Get%s()->ResetModified();\n",folderName[i].Data());
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<gAnalyzer->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->ResetModified();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      else {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   gAnalyzer->Get%s()->Reset();\n",folderName[i].Data());
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<gAnalyzer->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->Reset();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Initialize Single Folders
   buffer.AppendFormatted("void %sEventLoop::InitSingleFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   new(gAnalyzer->Get%s()) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
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
   buffer.AppendFormatted("void %sEventLoop::InitArrayFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i=0;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]!="1" && folderArray[i]!="variable")
            buffer.AppendFormatted("   gAnalyzer->Set%sSize(%s);\n",folderName[i].Data(),folderArray[i].Data());
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
      buffer.AppendFormatted("   if (romeTree->isFill()) {\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (folderName[k]==branchFolder[i][j] && !folderSupport[k]) {
               iFold = k;
               break;
            }
         }
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("      if (!write && gAnalyzer->Get%s()->isModified()) {\n",branchFolder[i][j].Data());
            buffer.AppendFormatted("         write = true;\n");
            buffer.AppendFormatted("      }\n");
         }
         else {
            buffer.AppendFormatted("      for (i=0;i<gAnalyzer->Get%ss()->GetEntries()&&!write;i++) {\n",branchFolder[i][j].Data());
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

   // get tree file names
   buffer.AppendFormatted("// Get Tree File Names\n");
   buffer.AppendFormatted("void %sEventLoop::GetTreeFileName(ROMEString& buffer,int treeIndex,const char* runNumber) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMETree *romeTree;\n");
   buffer.AppendFormatted("   switch (treeIndex) {\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("      case %d:\n",i);
      buffer.AppendFormatted("         romeTree = (ROMETree*)gAnalyzer->GetTreeObjectAt(%d);\n",i);
      buffer.AppendFormatted("         buffer = romeTree->GetConfigFileName();\n");
      buffer.AppendFormatted("         if (buffer.Length()==0) {\n");
      if (treeFileName[i].Length()==0)
         buffer.AppendFormatted("            buffer.SetFormatted(\"%s#.root\");\n",treeName[i].Data());
      else
         buffer.AppendFormatted("            buffer.SetFormatted(%s);\n",treeFileName[i].Data());
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         buffer.ReplaceAll(\"#\",runNumber);\n");
      buffer.AppendFormatted("         break;\n");
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Initialize Task Switches
   ROMEString taskNameT;
   int parentInd;
   buffer.AppendFormatted("void %sEventLoop::InitTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTaskHierarchy;i++) {
      taskNameT = taskHierarchyName[i];
      parentInd = taskHierarchyParentIndex[i];
      while (parentInd!=-1) {
         taskNameT = taskNameT.Insert(0,"_");
         taskNameT = taskNameT.Insert(0,taskHierarchyName[parentInd]);
         parentInd = taskHierarchyParentIndex[parentInd];
      }
      buffer.AppendFormatted("   gAnalyzer->GetTaskSwitches()->%s = gAnalyzer->Get%s%03dTask()->IsActive();\n",taskNameT.Data(),taskHierarchyName[i].Data(),i);
   }
   buffer.AppendFormatted("};\n\n");

   // Update Task Switches
   buffer.AppendFormatted("void %sEventLoop::UpdateTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTaskHierarchy;i++) {
      taskNameT = taskHierarchyName[i];
      parentInd = taskHierarchyParentIndex[i];
      while (parentInd!=-1) {
         taskNameT = taskNameT.Insert(0,"_");
         taskNameT = taskNameT.Insert(0,taskHierarchyName[parentInd]);
         parentInd = taskHierarchyParentIndex[parentInd];
      }
      buffer.AppendFormatted("   if (gAnalyzer->GetTaskSwitches()->%s)\n",taskNameT.Data());
      buffer.AppendFormatted("      gAnalyzer->Get%s%03dTask()->SetActive(true);\n",taskHierarchyName[i].Data(),i);
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      gAnalyzer->Get%s%03dTask()->SetActive(false);\n",taskHierarchyName[i].Data(),i);
   }
   buffer.AppendFormatted("};\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

bool ROMEBuilder::WriteEventLoopH() {
   ROMEString hFile;
   ROMEString buffer;

   // File name
   hFile.SetFormatted("%s/include/framework/%sEventLoop.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("// *** This file will be overwritten by the ROMEBuilder *** //\n");
   buffer.AppendFormatted("// ***      Don't make manual changes to this file      *** //\n");
   buffer.AppendFormatted("//////////////////////////////////////////////////////////////\n\n");
   // Header
   buffer.AppendFormatted("#ifndef %sEventLoop_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sEventLoop_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <TTree.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
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
   buffer.AppendFormatted("   void FillTrees();\n");
   buffer.AppendFormatted("   void GetTreeFileName(ROMEString& buffer,int treeIndex,const char* runNumber=\"\");\n");
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
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}


bool ROMEBuilder::WriteMain() {
   ROMEString cppFile;
   ROMEString buffer;

   cppFile.SetFormatted("%s/src/framework/main.cpp",outDir.Data());

   buffer.Resize(0);
   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
   buffer.AppendFormatted("#include <TRint.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
   buffer.AppendFormatted("#endif // R__VISUAL_CPLUSPLUS\n");
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

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

void ROMEBuilder::Usage() {
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
bool ROMEBuilder::ReadCommandLineParameters(int argc, char *argv[]) {
   struct stat buf;
   makeOutput = false;
   noLink = false;
   midas = false;
   orca = false;
   sql = false;
   mysql = false;
   pgsql = false;
   sqlite = false;
   sqlite3 = false;

   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);

   outDir = workDir;
   outDir.Append("/");

   if (argc==1) {
      Usage();
      return false;
   }
   for (int i=1;i<argc;i++) {
      // -- only for testing (start) --
      if (!strcmp(argv[i],"-dc")) {
         makeOutput = true;
         noLink = true;
         midas = true;
         outDir = "C:/Data/analysis/MEG/ROME .NET/DCAnalyzer/";
         xmlFile = "C:/Data/analysis/MEG/ROME .NET/DCAnalyzer/dc.xml";
      }
      else if (!strcmp(argv[i],"-meg")) {
         makeOutput = true;
         midas = true;
         orca = true;
         noLink = true;
         sql = true;
         mysql = true;
         sqlite = false;
//         sqlite3 = true;
         outDir = "C:/Data/analysis/MEG/ROME .NET/MEGFrameWork/";
         xmlFile = "C:/Data/analysis/MEG/ROME .NET/MEGFrameWork/MEGFrameWork.xml";
      }
      else if (!strcmp(argv[i],"-lp")) {
         makeOutput = true;
         midas = true;
         noLink = true;
         outDir = "C:/lpframework/";
         xmlFile = "C:/lpframework/lpframework.xml";
      }
      else if (!strcmp(argv[i],"-sample")) {
         makeOutput = true;
         midas = false;
         noLink = false;
         outDir = "C:/rome/examples/sample/";
         xmlFile = "C:/rome/examples/sample/sample.xml";
      }
      else if (!strcmp(argv[i],"-dance")) {
         makeOutput = true;
         midas = false;
         noLink = true;
         outDir = "C:/Data/Testprojects/dance2/test/";
         xmlFile = "C:/Data/Testprojects/dance2/test/dance.xml";
      }
      // -- only for testing (end) --
      else if (!strcmp(argv[i],"-v")) {
         makeOutput = true;
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
         if( stat( midasFile, &buf )) {
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

bool ROMEBuilder::CheckFileAndPath() 
{
   struct stat buf;
   if( stat( xmlFile.Data(), &buf )) {
      if ( xmlFile == "")
         cout << "No inputfile specified." << endl;
      else
         cout << "Inputfile '" << xmlFile.Data() << "' not found." << endl;
      return false;
   }
   ROMEString path;
   path = outDir;
   path.Remove(path.Length()-1);
   if (stat( path, &buf )) {
      cout << "Outputpath '" << outDir.Data() << "' not found." << endl;
      return false;
   }
   return true;
}
void ROMEBuilder::MakeDir(ROMEString &path)
{
#if defined( R__VISUAL_CPLUSPLUS )
   mkdir(path.Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
   mkdir(path.Data(),0711);
#endif // R__UNIX
}
void ROMEBuilder::StartBuilder()
{
   xml = new ROMEXML();

   char* name;
   bool finished = false;
   int type;
   int i,j;

   experimentName = "";
   styleSheet = "";
   shortCut = "";
   mainProgName = "";
   mainDescription = "";
   mainAuthor = "";
   mainInstitute = "";
   mainCollaboration = "";
   mainEmail = "";
   usedCLO = ROMECommandLineOptions;

   readExperiment = false;
   readAuthor = false;
   readFolders = false;
   readTasks = false;
   readTrees = false;
   readGlobalSteeringParameters = false;
   readMidasBanks = false;

   numOfFolder = -1;
   numOfTask = -1;
   numOfTree = -1;
   numOfEvent = -1;

   TString::MaxWaste(kTStringResizeIncrement-1);
   TString::ResizeIncrement(kTStringResizeIncrement);

   if (!xml->OpenFileForRead(xmlFile.Data())) return;
   // read in configuration file
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
                     if (type == 1 && !strcmp((const char*)name,"DocumentStyleSheet"))
                        xml->GetValue(styleSheet,styleSheet);
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
                        folderSupport[numOfFolder+1] = false;
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
               }
               if (!strcmp((const char*)name,"SupportFolders")) {
                  if (numOfFolder==-1)
                     continue;
                  numOfFolder--;
                  // output
                  if (makeOutput) cout << "SupportFolders:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // folder
                     if (type == 1 && !strcmp((const char*)name,"SupportFolder")) {
                        recursiveDepth = 0;
                        folderSupport[numOfFolder+1] = true;
                        if (!ReadXMLFolder()) return;
                     }
                     // folders end
                     if (type == 15 && !strcmp((const char*)name,"SupportFolders")) {
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
                  // default task hierarchy
                  for (i=0;i<numOfTask;i++) {
                     taskHierarchyName[i] = taskName[i];
                     taskHierarchyParentIndex[i] = -1;
                     taskHierarchyClassIndex[i] = i;
                  }
                  numOfTaskHierarchy = numOfTask;
               }
               if (!strcmp((const char*)name,"TaskHierarchy")) {
                  int depth = 0;
                  int parentIndex[2*maxNumberOfTasks];
                  parentIndex[0] = -1;
                  numOfTaskHierarchy = -1;
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp((const char*)name,"TaskName")) {
                        xml->GetValue(taskHierarchyName[numOfTaskHierarchy],taskHierarchyName[numOfTaskHierarchy]);
                        taskHierarchyClassIndex[numOfTaskHierarchy] = -1;
                        for (i=0;i<numOfTask;i++) {
                           if (taskName[i]==taskHierarchyName[numOfTaskHierarchy])
                              taskHierarchyClassIndex[numOfTaskHierarchy] = i;
                        }
                        if (taskHierarchyClassIndex[numOfTaskHierarchy] == -1) {
                           cout << "The task '" << taskHierarchyName[numOfTaskHierarchy].Data() << "' used in the task hierarchy is not defined !" << endl;
                           cout << "Terminating program." << endl;
                           return;
                        }
                     }
                     if (type == 1 && !strcmp((const char*)name,"Task")) {
                        depth++;
                        numOfTaskHierarchy++;
                        parentIndex[depth] = numOfTaskHierarchy;
                        taskHierarchyName[numOfTaskHierarchy] = "";
                        taskHierarchyParentIndex[numOfTaskHierarchy] = parentIndex[depth-1];
                     }
                     if (type == 15 && !strcmp((const char*)name,"Task")) {
                        depth--;
                     }
                     if (type == 15 && !strcmp((const char*)name,"TaskHierarchy"))
                        break;
                  }
                  numOfTaskHierarchy++;
                  continue;
               }
               if (!strcmp((const char*)name,"Trees")) {
                  numOfTree = -1;
                  if (!ReadXMLTree()) return;
               }
               if (!strcmp((const char*)name,"UserDAQSystems")) {
                  numOfDAQ = -1;
                  if (!ReadXMLDAQ()) return;
               }
               if (!strcmp((const char*)name,"UserDatabases")) {
                  numOfDB = -1;
                  if (!ReadXMLDB()) return;
               }
               if (!strcmp((const char*)name,"MidasBanks")) {
                  numOfEvent = -1;
                  if (!ReadXMLMidasBanks()) return;
               }
               if (!strcmp((const char*)name,"GlobalSteeringParameters")) {
                  // output
                  if (makeOutput) cout << "\n\nGlobal Steering Parameters:" << endl;
                  // initialisation
                  steerName[numOfTaskHierarchy][0] = "GlobalSteering";
                  actualSteerIndex = 0;
                  recursiveSteerDepth = 0;
                  steerParent[numOfTaskHierarchy][0] = -1;
                  numOfSteering[numOfTaskHierarchy] = -1;
                  if (!ReadXMLSteering(numOfTaskHierarchy)) return;
                  numOfSteering[numOfTaskHierarchy]++;
               }
               if (!strcmp((const char*)name,"UserMakeFile")) {
                  if (!ReadXMLUserMakefile()) return;
               }
            }
         }
      }
   }
   // test for fortran
   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         haveFortranTask = true;
         break;
      }
   }
   // write classes
   if (makeOutput) cout << "\n\nFramework :" << endl;
   if (makeOutput) cout << "\n\nAnalyzer:" << endl;
   if (!WriteAnalyzerCpp()) return;
   if (!WriteAnalyzerH()) return;
   if (!WriteAnalyzerF()) return;
   if (makeOutput) cout << "\n\nFolders:" << endl;
   if (!WriteFolderCpp()) return;
   if (!WriteFolderH()) return;
   if (makeOutput) cout << "\n\nTasks:" << endl;
   if (!WriteTaskCpp()) return;
   if (!WriteTaskF()) return;
   if (!WriteTaskH()) return;
   if (makeOutput) cout << "\n\nUser DAQ Systems:" << endl;
   if (!WriteDAQCpp()) return;
   if (!WriteDAQH()) return;
   if (makeOutput) cout << "\n\nUser Database Interfaces:" << endl;
   if (!WriteDBCpp()) return;
   if (!WriteDBH()) return;
   if (makeOutput) cout << "\n\nFramework:" << endl;
   if (!WriteSteering(numOfTaskHierarchy)) return;
   if (!WriteConfigCpp()) return;
   if (!WriteConfigH()) return;
   if (!WriteMidasDAQCpp()) return;
   if (!WriteMidasDAQH()) return;
   if (!WriteRomeDAQCpp()) return;
   if (!WriteRomeDAQH()) return;
   if (!WriteEventLoopCpp()) return;
   if (!WriteEventLoopH()) return;
   if (!WriteMain()) return;
   delete xml;

   ROMEString buffer;
   chdir(outDir.Data());

// Linking
   if (makeOutput && !noLink) cout << "\nLinking " << shortCut.Data() << " Project." << endl;
   WriteMakefile();
   if (noLink) {
      ROMEString tempStr;
#if defined( R__UNIX )
      tempStr.SetFormatted("make -e %sROMEDict.cpp %sFrameworkDict.cpp %sFolderDict.cpp %sTaskDict.cpp %sUserDict.cpp",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
      system(tempStr.Data());
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      const int workDirLen = 1000;
      char workDir[workDirLen];
      getcwd(workDir,workDirLen);
      tempStr.SetFormatted("nmake -f Makefile.win %sROMEDict.cpp %sFrameworkDict.cpp %sFolderDict.cpp %sTaskDict.cpp %sUserDict.cpp",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
      system(tempStr.Data());
#endif
   }
   else {
#if defined( R__UNIX )
      system("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      const int workDirLen = 1000;
      char workDir[workDirLen];
      getcwd(workDir,workDirLen);
      system("nmake -f Makefile.win");
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
char* ROMEBuilder::EqualSign() {
#if defined( R__VISUAL_CPLUSPLUS )
   return "=";
#else
   return ":=";
#endif
}

void ROMEBuilder::WriteMakefile() {
   // write a Makefile
   ROMEString buffer;
   ROMEString tempBuffer;
   ROMEString tmp;
   int i;

   ROMEString shortcut(shortCut);
   shortcut.ToLower();
   ROMEString mainprogname(mainProgName);
   mainprogname.ToLower();

   analyzerDep += " obj/ROMEAnalyzer.obj";
   eventLoopDep += " obj/ROMEEventLoop.obj";
   daqDep += " $(ROMESYS)/include/ROMEDAQSystem.h";
   midasDep += daqDep;
   romeDep += daqDep;

   buffer.Resize(0);
// Libraries, Flags and Includes
// -----------------------------
#if defined( R__VISUAL_CPLUSPLUS )
//   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib $(ROOTSYS)/lib/libVMC.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libGeomPainter.lib $(ROOTSYS)/lib/libMLP.lib $(ROOTSYS)/lib/libProof.lib $(ROOTSYS)/lib/libProofGui.lib $(ROOTSYS)/lib/libRGL.lib $(ROOTSYS)/lib/libfreetype.lib\n");
   buffer.AppendFormatted("rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libWin32gdk.lib");
#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))
   buffer.AppendFormatted(" $(ROOTSYS)/lib/libThread.lib \n");
#endif // ROOT_VERSION
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("sqllibs =");
   if (this->mysql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libmySQL.lib $(ROMESYS)/lib_win/mysys.lib $(ROMESYS)/lib_win/mysqlclient.lib");
   if (this->pgsql)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/libpq.lib");
   if (this->sqlite)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/sqlite.lib");
   if (this->sqlite3)
      buffer.AppendFormatted(" $(ROMESYS)/lib_win/sqlite3.lib");
   buffer.AppendFormatted("\n");
   if (this->midas)
      buffer.AppendFormatted("midaslibs = $(MIDASSYS)/nt/lib/midas.lib\n");
   else
      buffer.AppendFormatted("midaslibs = \n");
   buffer.AppendFormatted("clibs = wsock32.lib gdi32.lib user32.lib kernel32.lib\n");
   buffer.AppendFormatted("Libraries = $(rootlibs) $(clibs) $(sqllibs) $(midaslibs)");
   for (i=0;i<numOfMFWinLibs;i++)
      buffer.AppendFormatted(" %s",mfWinLibName[i].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   // flags
   buffer.AppendFormatted("Flags = /GX /GR $(%suserflags)",shortcut.Data());
   if (this->midas)
      buffer.AppendFormatted(" /DHAVE_MIDAS");
   if (this->orca)
      buffer.AppendFormatted(" /DHAVE_ORCA");
   if (this->sql)
      buffer.AppendFormatted(" /DHAVE_SQL");
   if (this->mysql)
      buffer.AppendFormatted(" /DHAVE_MYSQL");
   if (this->pgsql)
      buffer.AppendFormatted(" /DHAVE_PGSQL");
   if (this->sqlite)
      buffer.AppendFormatted(" /DHAVE_SQLITE");
   if (this->sqlite3)
      buffer.AppendFormatted(" /DHAVE_SQLITE3");
   for (i=0;i<this->flags.GetEntriesFast();i++)
      buffer.AppendFormatted(" /D%s",this->flags.At(i).Data());
   for (i=0;i<numOfMFPreDefs;i++)
      buffer.AppendFormatted(" /D%s",mfPreDefName[i].Data());
   buffer.AppendFormatted("\n");
   // fortran flags
   buffer.AppendFormatted("FortranFlags = $(%suserflags)\n",shortcut.Data());
   // includes
   buffer.AppendFormatted("Includes = /I$(ROMESYS)/include/ /I$(ROOTSYS)/include/ /I. /Iinclude/ /Iinclude/tasks/ /Iinclude/framework/ ");
   if (this->midas)
      buffer.AppendFormatted(" /I$(MIDASSYS)/include/");
   if (this->mysql)
      buffer.AppendFormatted(" /I$(ROMESYS)/include/mysql/");
   if (this->sqlite)
      buffer.AppendFormatted(" /I$(ROMESYS)/include/sqlite/");
   if (this->sqlite3)
      buffer.AppendFormatted(" /I$(ROMESYS)/include/sqlite3/");
   for (i=0;i<numOfMFIncDirs;i++)
      buffer.AppendFormatted(" /I%s",mfIncDir[i].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
   buffer.AppendFormatted("rootlibs := $(shell $(ROOTSYS)/bin/root-config --libs)\n");
   buffer.AppendFormatted("rootglibs := $(shell  $(ROOTSYS)/bin/root-config --glibs)\n");
   buffer.AppendFormatted("rootcflags := $(shell  $(ROOTSYS)/bin/root-config --cflags)\n");
   buffer.AppendFormatted("rootthreadlibs := -lThread\n");
   buffer.AppendFormatted("sqllibs :=");
   if (this->mysql)
      buffer.AppendFormatted(" $(shell mysql_config --libs)");
   if (this->pgsql)
      buffer.AppendFormatted(" -L$(shell pg_config --libdir) -lpq");
   if (this->sqlite)
      buffer.AppendFormatted(" -lsqlite");
   if (this->sqlite3)
      buffer.AppendFormatted(" -lsqlite3");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("sqlcflags :=");
   if (this->sql)
      buffer.AppendFormatted(" -DHAVE_SQL");
   if (this->mysql)
      buffer.AppendFormatted(" $(shell mysql_config --cflags) -DHAVE_MYSQL");
   if (this->pgsql)
      buffer.AppendFormatted(" -I$(shell pg_config --includedir) -DHAVE_PGSQL");
   if (this->sqlite)
      buffer.AppendFormatted(" -DHAVE_SQLITE");
   if (this->sqlite3)
      buffer.AppendFormatted(" -DHAVE_SQLITE3");
   buffer.AppendFormatted("\n");
#if defined( R__ALPHA )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lc -lbsd\n");
   buffer.AppendFormatted("soflags := -Wl,-expect_unresolved,* -shared\n");
#elif defined ( R__SGI )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
   buffer.AppendFormatted("soflags := -shared\n");
#elif defined ( R__FBSD )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lbsd -lcompat\n");
   buffer.AppendFormatted("soflags := -shared -Wl,-x\n");
#elif defined ( R__MACOSX )
   buffer.AppendFormatted("FINK_DIR := $(shell which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\")\n");
   buffer.AppendFormatted("MACOSX_MAJOR := $(shell sw_vers | sed -n 's/ProductVersion:[^0-9]*//p' | cut -d . -f 1)\n");
   buffer.AppendFormatted("MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion:[^0-9]*//p' | cut -d . -f 2)\n");
   buffer.AppendFormatted("MACOSX_DEPLOYMENT_TARGET := $(MACOSX_MAJOR).$(MACOSX_MINOR)\n");
   buffer.AppendFormatted("MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=$(MACOSX_MAJOR).$(MACOSX_MINOR)\n");
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function  $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)\n");
   buffer.AppendFormatted("oslibs := -lpthread -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)\n");
   buffer.AppendFormatted("ifeq ($(MACOSX_DEPLOYMENT_TARGET),10.1)\n");
   buffer.AppendFormatted("soflags := -dynamiclib -single_module -undefined suppress\n");
   buffer.AppendFormatted("endif\n");
   buffer.AppendFormatted("ifeq ($(MACOSX_DEPLOYMENT_TARGET),10.2)\n");
   buffer.AppendFormatted("soflags := -dynamiclib -single_module -undefined suppress\n");
   buffer.AppendFormatted("else\n");
   buffer.AppendFormatted("soflags := -dynamiclib -single_module -undefined dynamic_lookup\n");
   buffer.AppendFormatted("endif\n");
#elif defined ( R__LINUX )
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function\n");
   buffer.AppendFormatted("oslibs := -lutil -lpthread\n");
   buffer.AppendFormatted("soflags := -shared -Wl\n");
#elif defined ( R__SOLARIS )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lsocket -lnsl\n");
   buffer.AppendFormatted("soflags := -G\n");
#else
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
   buffer.AppendFormatted("soflags := -shared\n");
#endif
   if (this->midas) {
#if defined( R__ALPHA )
      buffer.AppendFormatted("midascflags := -DOSF1 -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/osf1/lib -lmidas\n");
#elif defined ( R__SGI )
      buffer.AppendFormatted("midascflags := -DOS_ULTRIX -DNO_PTY -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/ultrix/lib -lmidas\n");
#elif defined ( R__FBSD )
      buffer.AppendFormatted("midascflags := -DOS_FREEBSD -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/freeBSD/lib -lmidas\n");
#elif defined ( R__MACOSX )
      buffer.AppendFormatted("midascflags := -DOS_LINUX -DOS_DARWIN -DHAVE_STRLCPY -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/darwin/lib -lmidas\n");
#elif defined ( R__LINUX )
      buffer.AppendFormatted("midascflags := -DOS_LINUX -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/linux/lib -lmidas\n");
#elif defined ( R__SOLARIS )
      buffer.AppendFormatted("midascflags := -DOS_SOLARIS -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -L$(MIDASSYS)/solaris/lib -lmidas\n");
#else
      buffer.AppendFormatted("midascflags := -I$(MIDASSYS)/include/ -DHAVE_MIDAS\n");
      buffer.AppendFormatted("midaslibs := -lmidas\n");
#endif
   }
   else{
      buffer.AppendFormatted("midaslibs := \n");
      buffer.AppendFormatted("midascflags := \n");
   }
   buffer.AppendFormatted("clibs := -lHtml -lz $(SYSLIBS)");
   if (haveFortranTask)
      buffer.AppendFormatted(" -lg2c");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   // libs
   buffer.AppendFormatted("Libraries := $(oslibs) $(rootlibs) $(rootglibs) $(rootthreadlibs) $(clibs) $(sqllibs) $(midaslibs)");
   for (i=0;i<numOfMFUnixLibs;i++)
      buffer.AppendFormatted(" -l%s",mfUnixLibName[i].Data());
   buffer.AppendFormatted("\n");
   // flags
   buffer.AppendFormatted("Flags := $(%suserflags) $(oscflags) $(rootcflags) $(sqlcflags) $(midascflags)",shortcut.Data());
   for (i=0;i<numOfMFPreDefs;i++)
      buffer.AppendFormatted(" -D%s",mfPreDefName[i].Data());
   buffer.AppendFormatted("\n");
   // includes
   buffer.AppendFormatted("Includes := -I$(ROMESYS)/include/ -I. -Iinclude/ -Iinclude/tasks/ -Iinclude/framework/");
   for (i=0;i<numOfMFIncDirs;i++)
      buffer.AppendFormatted(" -I%s",mfIncDir[i].Data());
   buffer.AppendFormatted("\n");
#endif // R__UNIX
// Dependencies
// ------------
// folders
   buffer.AppendFormatted("FolderIncludes %s",EqualSign());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0)
         buffer.AppendFormatted(" include/framework/%s%s.h",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("BaseFolderIncludes %s",EqualSign());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0 && folderUserCode[i])
         buffer.AppendFormatted(" include/framework/%s%s_Base.h",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted("\n");

// tasks
   buffer.AppendFormatted("TaskIncludes %s",EqualSign());
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted(" include/tasks/%sT%s.h",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("BaseTaskIncludes %s",EqualSign());
   for (i=0;i<numOfTask;i++) {
      if (taskUserCode[i])
         buffer.AppendFormatted(" include/tasks/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("TaskObjects %s",EqualSign());
   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         buffer.AppendFormatted(" obj/%sTF%s.obj",shortCut.Data(),taskName[i].Data());
      }
      buffer.AppendFormatted(" obj/%sT%s.obj",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("\n");

// daq
   buffer.AppendFormatted("DAQIncludes %s",EqualSign());
   buffer.AppendFormatted(" include/framework/%sMidasDAQ.h",shortCut.Data());
   buffer.AppendFormatted(" include/framework/%sRomeDAQ.h",shortCut.Data());
   if (this->orca)
      buffer.AppendFormatted(" $(ROMESYS)/include/ROMEOrcaDAQ.h");
   buffer.AppendFormatted(" $(ROMESYS)/include/ROMENoDAQSystem.h");
   for (i=0;i<numOfDAQ;i++)
      buffer.AppendFormatted(" include/framework/%s%s.h",shortCut.Data(),daqName[i].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DAQObjects %s",EqualSign());
   buffer.AppendFormatted(" obj/%sMidasDAQ.obj",shortCut.Data());
   buffer.AppendFormatted(" obj/%sRomeDAQ.obj",shortCut.Data());
   for (i=0;i<numOfDAQ;i++)
      buffer.AppendFormatted(" obj/%s%s.obj",shortCut.Data(),daqName[i].Data());
   buffer.AppendFormatted("\n");

// data base
   buffer.AppendFormatted("DataBaseIncludes %s",EqualSign());
   buffer.AppendFormatted(" $(ROMESYS)/include/ROMEXMLDataBase.h");
   buffer.AppendFormatted(" $(ROMESYS)/include/ROMETextDataBase.h");
   buffer.AppendFormatted(" $(ROMESYS)/include/ROMEODBOfflineDataBase.h");
   buffer.AppendFormatted(" $(ROMESYS)/include/ROMEODBOnlineDataBase.h");
   if (this->sql){
      buffer.AppendFormatted(" $(ROMESYS)/include/ROMESQL.h");
      buffer.AppendFormatted(" $(ROMESYS)/include/ROMESQLDataBase.h");
   }
   if (this->mysql)
      buffer.AppendFormatted(" $(ROMESYS)/include/ROMEMySQL.h");
   if (this->pgsql)
      buffer.AppendFormatted(" $(ROMESYS)/include/ROMEPgSQL.h");
   if (this->sqlite)
      buffer.AppendFormatted(" $(ROMESYS)/include/ROMESQLite.h");
   if (this->sqlite3)
      buffer.AppendFormatted(" $(ROMESYS)/include/ROMESQLite3.h");
   for (i=0;i<numOfDB;i++)
      buffer.AppendFormatted(" include/framework/%s%sDataBase.h",shortCut.Data(),dbName[i].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DataBaseObjects %s",EqualSign());
   buffer.AppendFormatted(" obj/ROMEXMLDataBase.obj");
   buffer.AppendFormatted(" obj/ROMETextDataBase.obj");
   buffer.AppendFormatted(" obj/ROMEODBOfflineDataBase.obj");
   buffer.AppendFormatted(" obj/ROMEODBOnlineDataBase.obj");
   if (this->sql){
      buffer.AppendFormatted(" obj/ROMESQL.obj");
      buffer.AppendFormatted(" obj/ROMESQLDataBase.obj");
   }
   if (this->mysql)
      buffer.AppendFormatted(" obj/ROMEMySQL.obj");
   if (this->pgsql)
      buffer.AppendFormatted(" obj/ROMEPgSQL.obj");
   if (this->sqlite)
      buffer.AppendFormatted(" obj/ROMESQLite.obj");
   if (this->sqlite3)
      buffer.AppendFormatted(" obj/ROMESQLite3.obj");
   for (i=0;i<numOfDB;i++)
      buffer.AppendFormatted(" obj/%s%sDataBase.obj",shortCut.Data(),dbName[i].Data());
   buffer.AppendFormatted("\n");

   // root cint headers
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DictionaryHeaders %s",EqualSign());
   buffer.AppendFormatted("\n");

   // root cint includes
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("DictionaryIncludes %s",EqualSign());
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("LD_LIBRARY_PATH=$(ROOTSYS)/lib\n");
#endif

// task dependences
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("%sT%sDep %s $(ROMESYS)/include/ROMETask.h include/framework/%sAnalyzer.h obj/%sT%s.d\n",shortCut.Data(),taskName[i].Data(),EqualSign(),shortCut.Data(),shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("\n");

// daq dependences
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfDAQ;i++) {
      buffer.AppendFormatted("%s%sDep %s include/framework/%sAnalyzer.h obj/%s%s.d %s\n",shortCut.Data(),daqName[i].Data(),EqualSign(),shortCut.Data(),shortCut.Data(),daqName[i].Data(),daqDep.Data());
   }
   buffer.AppendFormatted("\n");

// database dependences
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfDB;i++) {
      buffer.AppendFormatted("%s%sDataBaseDep %s include/framework/%sAnalyzer.h obj/%s%sDataBase.d\n",shortCut.Data(),dbName[i].Data(),EqualSign(),shortCut.Data(),shortCut.Data(),dbName[i].Data());
   }
   buffer.AppendFormatted("\n");

// Objects
// -------
   WriteROMEBaseObjects(buffer);
   WriteROMEAdditionalObjects(buffer);
   WriteFolderObjects(buffer);
   WriteTaskObjects(buffer);
   WriteFrameWorkBaseObjects(buffer);
   WriteFrameWorkAdditionalObjects(buffer);
   buffer.AppendFormatted("\n");

// all
   buffer.AppendFormatted("all:obj blank.d %s%s.exe",shortcut.Data(),mainprogname.Data());
#if defined( R__UNIX )
   buffer.AppendFormatted(" lib%s%s.so",shortcut.Data(),mainprogname.Data(),shortcut.Data(),mainprogname.Data());
#endif // R__UNIX
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
// user makefile
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("!INCLUDE Makefile.winusr\n");
#else
   buffer.AppendFormatted("-include Makefile.usr\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("\n");

// make obj
   buffer.AppendFormatted("obj:\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\t@mkdir obj\n\n");
#else
   buffer.AppendFormatted("\t@if [ ! -d  obj ] ; then \\\n");
   buffer.AppendFormatted("\t\techo \"Making directory obj\" ; \\\n");
   buffer.AppendFormatted("\t\tmkdir obj; \\\n");
   buffer.AppendFormatted("\tfi;\n");
#endif // R__VISUAL_CPLUSPLUS

// make blank
   buffer.AppendFormatted("blank.d:\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("\tcopy nul blank.d\n\n");
#endif // R__VISUAL_CPLUSPLUS

// Dictionary
   WriteROMEDictionary(buffer);
   WriteFrameworkDictionary(buffer);
   WriteFolderDictionary(buffer);
   WriteTaskDictionary(buffer);
   WriteUserDictionary(buffer);

// Link Statement
// --------------
   buffer.AppendFormatted("%s%s.exe: $(objects)\n",shortcut.Data(),mainprogname.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   if (haveFortranTask)
      buffer.AppendFormatted("\tcl /Fe%s%s.exe $(objects) $(Libraries) /link /nodefaultlib:\"libcmtd.lib\" /FORCE:MULTIPLE\n\n",shortCut.Data(),mainProgName.Data());
   else
      buffer.AppendFormatted("\tcl /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
   buffer.AppendFormatted("\tg++ $(Flags) -o $@ $(objects) $(Libraries)\n");
   buffer.AppendFormatted("lib%s%s.so: $(objects)\n",shortcut.Data(),mainprogname.Data());
   buffer.AppendFormatted("\t");
#if defined ( R__MACOSX )
   buffer.AppendFormatted("$(MACOSXTARGET) ");
#endif // R__MACOSX
   buffer.AppendFormatted("g++ $(Flags) $(soflags) -o lib%s%s.so $(objects) $(Libraries)\n",shortcut.Data(),mainprogname.Data());
#if defined ( R__MACOSX )
   buffer.AppendFormatted("\tln -sf lib%s%s.so lib%s%s.dylib",shortcut.Data(),mainprogname.Data(),shortcut.Data(),mainprogname.Data());
#endif // R__MACOSX
   buffer.AppendFormatted("\n");
#endif // R__UNIX

// Compile Statements
// ------------------
   WriteDefineFormats(buffer);
   WriteROMEBaseCompileStatements(buffer);
   WriteROMEAdditionalCompileStatements(buffer);
   WriteFolderCompileStatements(buffer);
   WriteTaskCompileStatements(buffer);
   WriteFrameWorkBaseCompileStatements(buffer);
   WriteFrameWorkAdditionalCompileStatements(buffer);

   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
//   buffer.AppendFormatted("!INCLUDE obj/*.d\n");
#else
   buffer.AppendFormatted("-include obj/*.d\n");
#endif // R__VISUAL_CPLUSPLUS

   buffer.AppendFormatted("clean: userclean\n");
   buffer.AppendFormatted("\t-rm -f obj/*.obj obj/*.d %sFolderDict.cpp %sFolderDict.h %sROMEDict.cpp %sROMEDict.h %sUserDict.cpp %sUserDict.h %sFrameworkDict.cpp %sFrameworkDict.h %sTaskDict.cpp %sTaskDict.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   tmp = shortCut;
   tmp.ToLower();
   buffer.AppendFormatted("%sclean:\n",tmp.Data());
   buffer.AppendFormatted("\t-rm -f obj/%s*.obj obj/%s*.d  %sFolderDict.cpp %sFolderDict.h %sUserDict.cpp %sUserDict.h %sFrameworkDict.cpp %sFrameworkDict.h %sTaskDict.cpp %sTaskDict.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   Int_t pdnameend = 0;
   Int_t pbnamestart = 0;
   ROMEString xmlfilename = xmlFile;
   while((pdnameend = xmlfilename.Index("/",1,pbnamestart,TString::kExact))!=-1)
      pbnamestart = pdnameend+1;
   ROMEString xmlbasename = xmlfilename(pbnamestart,xmlfilename.Length());
#ifndef R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("build:\n");
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
   buffer.AppendFormatted("$(ROMESYS)/bin/romebuilder.exe");
   buffer.AppendFormatted(" -i %s -o .",xmlbasename.Data());
   if (makeOutput)
      buffer.AppendFormatted(" -v");
   if(noLink)
      buffer.AppendFormatted(" -nl");
   if(midas)
      buffer.AppendFormatted(" -midas");
   if(orca)
      buffer.AppendFormatted(" -orca");
   if(mysql)
      buffer.AppendFormatted(" -mysql");
   if(pgsql)
      buffer.AppendFormatted(" -pgsql");
   if(sqlite)
      buffer.AppendFormatted(" -sqlite");
   if(sqlite3)
      buffer.AppendFormatted(" -sqlite3");
   buffer.AppendFormatted("\n");
#endif // NOT R__VISUAL_CPLUSPLUS

   ROMEString makeFile;
#if defined( R__UNIX )
   makeFile = "Makefile";
#endif // R__UNIX
#if defined ( R__VISUAL_CPLUSPLUS )
   makeFile = "Makefile.win";
#endif // R__VISUAL_CPLUSPLUS
   // Write File
   WriteFile(makeFile.Data(),buffer.Data(),6);

   // Write Makefile.usr
   WriteUserMakeFile();
}
void ROMEBuilder::WriteROMEBaseObjects(ROMEString& buffer) {
   buffer.AppendFormatted("objects %s $(objects)",EqualSign());
   buffer.AppendFormatted(" obj/ROMETask.obj obj/ROMEEventLoop.obj obj/TNetFolderServer.obj obj/ROMEAnalyzer.obj obj/ROMEXML.obj obj/ROMEString.obj obj/ROMEStrArray.obj obj/ROMEStr2DArray.obj obj/ROMEPath.obj obj/ROMEMidasDAQ.obj obj/ROMERomeDAQ.obj obj/ROMEUtilities.obj obj/mxml.obj obj/strlcpy.obj");
   if (this->orca)
      buffer.AppendFormatted(" obj/ROMEOrcaDAQ.obj");
   buffer.AppendFormatted(" obj/%sROMEDict.obj",shortCut.Data());
   buffer.AppendFormatted("\n");
}
void ROMEBuilder::WriteROMEAdditionalObjects(ROMEString& buffer) {
   buffer.AppendFormatted("objects %s $(objects)",EqualSign());
   buffer.AppendFormatted(" obj/ROMESplashScreen.obj");
   buffer.AppendFormatted("\n");
}
void ROMEBuilder::WriteFolderObjects(ROMEString& buffer) {
   int i;
   buffer.AppendFormatted("objects %s $(objects)",EqualSign());
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormatted(" obj/%s%s.obj",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted("\n");
#if defined( R__UNIX )
   if (numOfFolder>0) {
      buffer.AppendFormatted("objects += obj/%sFolderDict.obj\n",shortCut.Data());
   }
#else
   buffer.AppendFormatted("objects = $(objects) obj/%sFolderDict.obj\n",shortCut.Data());
#endif // R__UNIX
}
void ROMEBuilder::WriteTaskObjects(ROMEString& buffer) {
   buffer.AppendFormatted("objects %s $(objects)",EqualSign());
   buffer.AppendFormatted(" $(TaskObjects)\n");
#if defined( R__UNIX )
   if (numOfTask>0) {
      buffer.AppendFormatted("objects += obj/%sTaskDict.obj\n",shortCut.Data());
   }
#else
   buffer.AppendFormatted("objects = $(objects) obj/%sTaskDict.obj\n",shortCut.Data());
#endif // R__UNIX
}
void ROMEBuilder::WriteFrameWorkBaseObjects(ROMEString& buffer) {
   buffer.AppendFormatted("objects %s $(objects)",EqualSign());
   buffer.AppendFormatted(" $(DAQObjects)");
   buffer.AppendFormatted(" $(DataBaseObjects)");
   buffer.AppendFormatted(" obj/%sAnalyzer.obj obj/%sConfig.obj obj/main.obj",shortCut.Data(),shortCut.Data());
   if (haveFortranTask)
      buffer.AppendFormatted(" obj/%sFAnalyzer.obj",shortCut.Data());
   buffer.AppendFormatted(" obj/%sFrameworkDict.obj",shortCut.Data());
   buffer.AppendFormatted("\n");
   WriteAdditionalSourceFilesObjects(buffer);
   buffer.AppendFormatted("\n");
#if defined( R__UNIX )
   if (numOfMFDictHeaders==0) {
      buffer.AppendFormatted("ifdef DictionaryHeaders\n");
      buffer.AppendFormatted("objects += obj/%sUserDict.obj\n",shortCut.Data());
      buffer.AppendFormatted("endif\n");
   }
   else{
      buffer.AppendFormatted("objects += obj/%sUserDict.obj\n",shortCut.Data());
   }
#else
   buffer.AppendFormatted("objects = $(objects) obj/%sUserDict.obj\n",shortCut.Data());
#endif // R__UNIX
   buffer.AppendFormatted("\n");
}
void ROMEBuilder::WriteFrameWorkAdditionalObjects(ROMEString& buffer) {
   buffer.AppendFormatted("objects %s $(objects)",EqualSign());
   buffer.AppendFormatted(" obj/%sEventLoop.obj",shortCut.Data());
   buffer.AppendFormatted("\n");
}
void ROMEBuilder::WriteDefineFormats(ROMEString& buffer) {
   dependFormatFrame.SetFormatted("obj/%s%%s.d: src/framework/%s%%s.cpp include/framework/%s%%s.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   dependFormatTasks.SetFormatted("obj/%sT%%s.d: src/tasks/%sT%%s.cpp include/tasks/%sT%%s.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   dependFormatBlank.SetFormatted("obj/%%s.d: %%s.cpp\n");
   dependFormatROME.SetFormatted ("obj/ROME%%s.d: $(ROMESYS)/src/ROME%%s.cpp $(ROMESYS)/include/ROME%%s.h\n");
   dependFormatRANY.SetFormatted ("obj/%%s.d: $(ROMESYS)/src/%%s\n");
   dependFormatAny.SetFormatted  ("obj/%%s.d: %%s\n");
#if defined( R__UNIX )
   compileFormatFrame.SetFormatted("\tg++ -c $(Flags) $(Includes) src/framework/%s%%s.cpp -o obj/%s%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatFramF.SetFormatted("\t@echo fortran tasks not implemented on unix\n");
   compileFormatTasks.SetFormatted("\tg++ -c $(Flags) $(Includes) src/tasks/%sT%%s.cpp -o obj/%sT%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTaskF.SetFormatted("\t@echo fortran tasks not implemented on unix\n");
   compileFormatBlank.SetFormatted("\tg++ -c $(Flags) $(Includes) %%s.cpp -o obj/%%s.obj\n");
   compileFormatROME.SetFormatted ("\tg++ -c $(Flags) $(Includes) $(ROMESYS)/src/ROME%%s.cpp -o obj/ROME%%s.obj\n");
   compileFormatRANY.SetFormatted ("\tg++ -c $(Flags) $(Includes) $(ROMESYS)/src/%%s -o obj/%%s.obj\n");
   compileFormatAny.SetFormatted  ("\tg++ -c $(Flags) $(Includes) %%s -o obj/%%s.obj\n");
   dependFormatFrame.AppendFormatted("\tg++ $(Flags) $(Includes) -M -MF $@ -MT obj/%s%%s.obj $<\n",shortCut.Data());
   dependFormatTasks.AppendFormatted("\tg++ $(Flags) $(Includes) -M -MF $@ -MT obj/%sT%%s.obj $<\n",shortCut.Data());
   dependFormatBlank.AppendFormatted("\tg++ $(Flags) $(Includes) -M -MF $@ -MT obj/%%s.obj $<\n");
   dependFormatROME.AppendFormatted ("\tg++ $(Flags) $(Includes) -M -MF $@ -MT obj/ROME%%s.obj $<\n");
   dependFormatRANY.AppendFormatted ("\tg++ $(Flags) $(Includes) -M -MF $@ -MT obj/%%s.obj $<\n");
   dependFormatAny.AppendFormatted  ("\tg++ $(Flags) $(Includes) -M -MF $@ -MT obj/%%s.obj $<\n");
   dependFormatFramF.SetFormatted("\n");
   dependFormatTaskF.SetFormatted("\n");
#endif // R__UNIX
#if defined( R__VISUAL_CPLUSPLUS )
   compileFormatROME.SetFormatted ("\tcl /c $(Flags) $(Includes) $(ROMESYS)/src/ROME%%s.cpp /Foobj/ROME%%s.obj\n");
   compileFormatFrame.SetFormatted("\tcl /c $(Flags) $(Includes) src/framework/%s%%s.cpp /Foobj/%s%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatFramF.SetFormatted("\tdf $(FortranFlags) /compile_only src\\framework\\%sF%%s.f /object:obj\\%sF%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTasks.SetFormatted("\tcl /c $(Flags) $(Includes) src/tasks/%sT%%s.cpp /Foobj/%sT%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTaskF.SetFormatted("\tdf $(FortranFlags) /compile_only src\\tasks\\%sTF%%s.f /object:obj\\%sTF%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatBlank.SetFormatted("\tcl /c $(Flags) $(Includes) %%s.cpp /Foobj/%%s.obj\n");
   compileFormatRANY.SetFormatted ("\tcl /c $(Flags) $(Includes) $(ROMESYS)/src/%%s /Foobj/%%s.obj\n");
   compileFormatAny.SetFormatted  ("\tcl /c $(Flags) $(Includes) %%s /Foobj/%%s.obj\n");
   dependFormatFrame.AppendFormatted("\tcopy blank.d obj\\%s%%s.d\n",shortCut.Data());
   dependFormatTasks.AppendFormatted("\tcopy blank.d obj\\%sT%%s.d\n",shortCut.Data());
   dependFormatBlank.AppendFormatted("\tcopy blank.d obj\\%%s.d\n");
   dependFormatROME.AppendFormatted ("\tcopy blank.d obj\\ROME%%s.d\n");
   dependFormatRANY.AppendFormatted ("\tcopy blank.d obj\\%%s.d\n");
   dependFormatAny.AppendFormatted  ("\tcopy blank.d obj\\%%s.d\n");
   dependFormatFramF.SetFormatted("");
   dependFormatTaskF.SetFormatted("");
#endif // R__VISUAL_CPLUSPLUS
}
void ROMEBuilder::WriteROMEBaseCompileStatements(ROMEString& buffer) {
   ROMEString tempBuffer;

   buffer.AppendFormatted(dependFormatROME.Data(),"Analyzer","Analyzer","Analyzer","Analyzer","Analyzer");
   buffer.AppendFormatted("obj/ROMEAnalyzer.obj: $(ROMESYS)/src/ROMEAnalyzer.cpp $(ROMESYS)/include/ROMEAnalyzer.h $(ROMESYS)/include/ROMETask.h obj/ROMEAnalyzer.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"Analyzer","Analyzer");

   buffer.AppendFormatted(dependFormatROME.Data(),"EventLoop","EventLoop","EventLoop","EventLoop","EventLoop");
   buffer.AppendFormatted("obj/ROMEEventLoop.obj: $(ROMESYS)/src/ROMEEventLoop.cpp $(ROMESYS)/include/ROMEEventLoop.h obj/ROMEEventLoop.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"EventLoop","EventLoop");

   buffer.AppendFormatted(dependFormatRANY.Data(),"TNetFolderServer","TNetFolderServer.cpp","TNetFolderServer","TNetFolderServer");
   buffer.AppendFormatted("obj/TNetFolderServer.obj: $(ROMESYS)/src/TNetFolderServer.cpp $(ROMESYS)/include/TNetFolderServer.h obj/TNetFolderServer.d\n");
   buffer.AppendFormatted(compileFormatRANY.Data(),"TNetFolderServer.cpp","TNetFolderServer");

   buffer.AppendFormatted(dependFormatROME.Data(),"Task","Task","Task","Task","Task");
   buffer.AppendFormatted("obj/ROMETask.obj: $(ROMESYS)/src/ROMETask.cpp $(ROMESYS)/include/ROMETask.h obj/ROMETask.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"Task","Task");

   buffer.AppendFormatted(dependFormatROME.Data(),"String","String","String","String","String");
   buffer.AppendFormatted("obj/ROMEString.obj: $(ROMESYS)/src/ROMEString.cpp $(ROMESYS)/include/ROMEString.h obj/ROMEString.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"String","String");

   buffer.AppendFormatted(dependFormatROME.Data(),"StrArray","StrArray","StrArray","StrArray","StrArray");
   buffer.AppendFormatted("obj/ROMEStrArray.obj: $(ROMESYS)/src/ROMEStrArray.cpp $(ROMESYS)/include/ROMEStrArray.h obj/ROMEStrArray.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"StrArray","StrArray");

   buffer.AppendFormatted(dependFormatROME.Data(),"Str2DArray","Str2DArray","Str2DArray","Str2DArray","Str2DArray");
   buffer.AppendFormatted("obj/ROMEStr2DArray.obj: $(ROMESYS)/src/ROMEStr2DArray.cpp $(ROMESYS)/include/ROMEStr2DArray.h obj/ROMEStr2DArray.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"Str2DArray","Str2DArray");

   buffer.AppendFormatted(dependFormatROME.Data(),"Path","Path","Path","Path","Path");
   buffer.AppendFormatted("obj/ROMEPath.obj: $(ROMESYS)/src/ROMEPath.cpp $(ROMESYS)/include/ROMEPath.h obj/ROMEPath.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"Path","Path");

   buffer.AppendFormatted(dependFormatROME.Data(),"XML","XML","XML","XML","XML");
   buffer.AppendFormatted("obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp $(ROMESYS)/include/ROMEXML.h obj/ROMEXML.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"XML","XML");

   buffer.AppendFormatted(dependFormatRANY.Data(),"mxml","mxml.c","mxml","mxml");
   buffer.AppendFormatted("obj/mxml.obj: $(ROMESYS)/src/mxml.c $(ROMESYS)/include/mxml.h obj/mxml.d\n");
   buffer.AppendFormatted(compileFormatRANY.Data(),"mxml.c","mxml");

   buffer.AppendFormatted(dependFormatRANY.Data(),"strlcpy","strlcpy.c","strlcpy","strlcpy");
   buffer.AppendFormatted("obj/strlcpy.obj: $(ROMESYS)/src/strlcpy.c $(ROMESYS)/include/strlcpy.h obj/strlcpy.d\n");
   buffer.AppendFormatted(compileFormatRANY.Data(),"strlcpy.c","strlcpy");

   buffer.AppendFormatted(dependFormatROME.Data(),"XMLDataBase","XMLDataBase","XMLDataBase","XMLDataBase","XMLDataBase");
   buffer.AppendFormatted("obj/ROMEXMLDataBase.obj: $(ROMESYS)/src/ROMEXMLDataBase.cpp $(ROMESYS)/include/ROMEXMLDataBase.h obj/ROMEXMLDataBase.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"XMLDataBase","XMLDataBase");

   buffer.AppendFormatted(dependFormatROME.Data(),"TextDataBase","TextDataBase","TextDataBase","TextDataBase","TextDataBase");
   buffer.AppendFormatted("obj/ROMETextDataBase.obj: $(ROMESYS)/src/ROMETextDataBase.cpp $(ROMESYS)/include/ROMETextDataBase.h obj/ROMETextDataBase.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"TextDataBase","TextDataBase");

   buffer.AppendFormatted(dependFormatROME.Data(), "Utilities","Utilities","Utilities","Utilities","Utilities");
   buffer.AppendFormatted("obj/ROMEUtilities.obj: $(ROMESYS)/src/ROMEUtilities.cpp $(ROMESYS)/include/ROMEUtilities.h obj/ROMEUtilities.d\n" );
   buffer.AppendFormatted(compileFormatROME.Data(), "Utilities", "Utilities" );

   buffer.AppendFormatted(dependFormatROME.Data(),"RomeDAQ","RomeDAQ","RomeDAQ","RomeDAQ","RomeDAQ");
   buffer.AppendFormatted("obj/ROMERomeDAQ.obj: $(ROMESYS)/src/ROMERomeDAQ.cpp $(ROMESYS)/include/ROMERomeDAQ.h obj/ROMERomeDAQ.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"RomeDAQ","RomeDAQ");

   buffer.AppendFormatted(dependFormatROME.Data(),"MidasDAQ","MidasDAQ","MidasDAQ","MidasDAQ","MidasDAQ");
   buffer.AppendFormatted("obj/ROMEMidasDAQ.obj: $(ROMESYS)/src/ROMEMidasDAQ.cpp $(ROMESYS)/include/ROMEMidasDAQ.h obj/ROMEMidasDAQ.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"MidasDAQ","MidasDAQ");
   if (this->orca) {
      buffer.AppendFormatted(dependFormatROME.Data(),"OrcaDAQ","OrcaDAQ","OrcaDAQ","OrcaDAQ","OrcaDAQ");
      buffer.AppendFormatted("obj/ROMEOrcaDAQ.obj: $(ROMESYS)/src/ROMEOrcaDAQ.cpp $(ROMESYS)/include/ROMEOrcaDAQ.h obj/ROMEOrcaDAQ.d\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"OrcaDAQ","OrcaDAQ");
   }

   buffer.AppendFormatted(dependFormatROME.Data(),"ODBOfflineDataBase","ODBOfflineDataBase","ODBOfflineDataBase","ODBOfflineDataBase","ODBOfflineDataBase");
   buffer.AppendFormatted("obj/ROMEODBOfflineDataBase.obj: $(ROMESYS)/src/ROMEODBOfflineDataBase.cpp $(ROMESYS)/include/ROMEODBOfflineDataBase.h obj/ROMEODBOfflineDataBase.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"ODBOfflineDataBase","ODBOfflineDataBase");

   buffer.AppendFormatted(dependFormatROME.Data(),"ODBOnlineDataBase","ODBOnlineDataBase","ODBOnlineDataBase","ODBOnlineDataBase","ODBOnlineDataBase");
   buffer.AppendFormatted("obj/ROMEODBOnlineDataBase.obj: $(ROMESYS)/src/ROMEODBOnlineDataBase.cpp $(ROMESYS)/include/ROMEODBOnlineDataBase.h obj/ROMEODBOnlineDataBase.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"ODBOnlineDataBase","ODBOnlineDataBase");
   if (this->mysql) {
      buffer.AppendFormatted(dependFormatROME.Data(),"MySQL","MySQL","MySQL","MySQL","MySQL");
      buffer.AppendFormatted("obj/ROMEMySQL.obj: $(ROMESYS)/src/ROMEMySQL.cpp $(ROMESYS)/include/ROMEMySQL.h obj/ROMESQL.obj obj/ROMEMySQL.d\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"MySQL","MySQL");
   }
   if (this->pgsql) {
      buffer.AppendFormatted(dependFormatROME.Data(),"PgSQL","PgSQL","PgSQL","PgSQL","PgSQL");
      buffer.AppendFormatted("obj/ROMEPgSQL.obj: $(ROMESYS)/src/ROMEPgSQL.cpp $(ROMESYS)/include/ROMEPgSQL.h obj/ROMESQL.obj obj/ROMEPgSQL.d\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"PgSQL","PgSQL");
   }
   if (this->sqlite) {
      buffer.AppendFormatted(dependFormatROME.Data(),"SQLite","SQLite","SQLite","SQLite","SQLite");
      buffer.AppendFormatted("obj/ROMESQLite.obj: $(ROMESYS)/src/ROMESQLite.cpp $(ROMESYS)/include/ROMESQLite.h obj/ROMESQL.obj obj/ROMESQLite.d\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"SQLite","SQLite");
   }
   if (this->sqlite3) {
      buffer.AppendFormatted(dependFormatROME.Data(),"SQLite3","SQLite3","SQLite3","SQLite3","SQLite3");
      buffer.AppendFormatted("obj/ROMESQLite3.obj: $(ROMESYS)/src/ROMESQLite3.cpp $(ROMESYS)/include/ROMESQLite3.h obj/ROMESQL.obj obj/ROMESQLite3.d\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"SQLite3","SQLite3");
   }
   if (this->sql) {
      buffer.AppendFormatted(dependFormatROME.Data(),"SQLDataBase","SQLDataBase","SQLDataBase","SQLDataBase","SQLDataBase");
      buffer.AppendFormatted("obj/ROMESQLDataBase.obj: $(ROMESYS)/src/ROMESQLDataBase.cpp $(ROMESYS)/include/ROMESQLDataBase.h obj/ROMESQLDataBase.d\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"SQLDataBase","SQLDataBase");

      buffer.AppendFormatted(dependFormatROME.Data(),"SQL","SQL","SQL","SQL","SQL");
      buffer.AppendFormatted("obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp $(ROMESYS)/include/ROMESQL.h obj/ROMESQL.d\n");
      buffer.AppendFormatted(compileFormatROME.Data(),"SQL","SQL");
   }

   buffer.AppendFormatted("obj/%sROMEDict.obj: %sROMEDict.h %sROMEDict.cpp\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   tempBuffer.SetFormatted("%sROMEDict",shortCut.Data());
   buffer.AppendFormatted(compileFormatBlank.Data(),tempBuffer.Data(),tempBuffer.Data());
}
void ROMEBuilder::WriteROMEAdditionalCompileStatements(ROMEString& buffer) {
   buffer.AppendFormatted(dependFormatRANY.Data(),"ROMESplashScreen","ROMESplashScreen.cpp","ROMESplashScreen","ROMESplashScreen");
   buffer.AppendFormatted("obj/ROMESplashScreen.obj: $(ROMESYS)/src/ROMESplashScreen.cpp $(ROMESYS)/include/ROMEString.h obj/ROMESplashScreen.d\n");
   buffer.AppendFormatted(compileFormatROME.Data(),"SplashScreen","SplashScreen");
}
void ROMEBuilder::WriteFolderCompileStatements(ROMEString& buffer) {
   ROMEString tempBuffer;
   int i;

   for (i=0;i<numOfFolder;i++) {
      if (folderUserCode[i]) {
         buffer.AppendFormatted(dependFormatFrame.Data(),folderName[i].Data(),folderName[i].Data(),folderName[i].Data(),folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("obj/%s%s.obj: src/framework/%s%s.cpp include/framework/%s%s.h",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted(" include/framework/%s%s_Base.h obj/%s%s.d\n",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted(compileFormatFrame.Data(),folderName[i].Data(),folderName[i].Data());
      }
   }

   buffer.AppendFormatted("obj/%sFolderDict.obj: %sFolderDict.h %sFolderDict.cpp\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   tempBuffer.SetFormatted("%sFolderDict",shortCut.Data());
   buffer.AppendFormatted(compileFormatBlank.Data(),tempBuffer.Data(),tempBuffer.Data());
}
void ROMEBuilder::WriteTaskCompileStatements(ROMEString& buffer) {
   ROMEString tempBuffer;
   int i;

   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         buffer.AppendFormatted("obj/%sTF%s.obj: src/tasks/%sTF%s.f include/tasks/%sT%s.h $(%sT%sDep)\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
         buffer.AppendFormatted(compileFormatTaskF.Data(),taskName[i].Data(),taskName[i].Data());
      }
      buffer.AppendFormatted(dependFormatTasks.Data(),taskName[i].Data(),taskName[i].Data(),taskName[i].Data(),taskName[i].Data(),taskName[i].Data());
      buffer.AppendFormatted("obj/%sT%s.obj: src/tasks/%sT%s.cpp include/tasks/%sT%s.h $(%sT%sDep)",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      if (taskUserCode[i])
         buffer.AppendFormatted(" include/tasks/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted(compileFormatTasks.Data(),taskName[i].Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("obj/%sTaskDict.obj: %sTaskDict.h %sTaskDict.cpp\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   tempBuffer.SetFormatted("%sTaskDict",shortCut.Data());
   buffer.AppendFormatted(compileFormatBlank.Data(),tempBuffer.Data(),tempBuffer.Data());
}
void ROMEBuilder::WriteFrameWorkBaseCompileStatements(ROMEString& buffer) {
   ROMEString tempBuffer;
   int i;

   // DAQs
   for (i=0;i<numOfDAQ;i++) {
      buffer.AppendFormatted(dependFormatFrame.Data(),daqName[i].Data(),daqName[i].Data(),daqName[i].Data(),daqName[i].Data(),daqName[i].Data());
      buffer.AppendFormatted("obj/%s%s.obj: src/framework/%s%s.cpp include/framework/%s%s.h $(%s%sDep)\n",shortCut.Data(),daqName[i].Data(),shortCut.Data(),daqName[i].Data(),shortCut.Data(),daqName[i].Data(),shortCut.Data(),daqName[i].Data());
      buffer.AppendFormatted(compileFormatFrame.Data(),daqName[i].Data(),daqName[i].Data());
   }
   // Databases
   for (i=0;i<numOfDB;i++) {
      tempBuffer = dbName[i]+"DataBase";
      buffer.AppendFormatted(dependFormatFrame.Data(),tempBuffer.Data(),tempBuffer.Data(),tempBuffer.Data(),tempBuffer.Data(),tempBuffer.Data());
      buffer.AppendFormatted("obj/%s%sDataBase.obj: src/framework/%s%sDataBase.cpp include/framework/%s%sDataBase.h $(%s%sDataBaseDep)\n",shortCut.Data(),dbName[i].Data(),shortCut.Data(),dbName[i].Data(),shortCut.Data(),dbName[i].Data(),shortCut.Data(),dbName[i].Data());
      buffer.AppendFormatted(compileFormatFrame.Data(),tempBuffer.Data(),tempBuffer.Data());
   }
   // Fortran Analyzer
   if (haveFortranTask) {
      buffer.AppendFormatted("obj/%sFAnalyzer.obj: src/framework/%sFAnalyzer.f src/framework/%sAnalyzer.cpp include/framework/%sAnalyzer.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
      buffer.AppendFormatted(compileFormatFramF.Data(),"Analyzer","Analyzer");
   }
   // Analyzer
   buffer.AppendFormatted(dependFormatFrame.Data(),"Analyzer","Analyzer","Analyzer","Analyzer","Analyzer");
   buffer.AppendFormatted("obj/%sAnalyzer.obj: src/framework/%sAnalyzer.cpp obj/%sAnalyzer.d",shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("%s",analyzerDep.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted(compileFormatFrame.Data(),"Analyzer","Analyzer");
   // Config
   buffer.AppendFormatted(dependFormatFrame.Data(),"Config","Config","Config","Config","Config");
   buffer.AppendFormatted("obj/%sConfig.obj: src/framework/%sConfig.cpp obj/%sConfig.d",shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("%s",configDep.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted(compileFormatFrame.Data(),"Config","Config");
   // Midas
   buffer.AppendFormatted(dependFormatFrame.Data(),"MidasDAQ","MidasDAQ","MidasDAQ","MidasDAQ","MidasDAQ");
   buffer.AppendFormatted("obj/%sMidasDAQ.obj: src/framework/%sMidasDAQ.cpp obj/%sMidasDAQ.d",shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("%s",midasDep.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted(compileFormatFrame.Data(),"MidasDAQ","MidasDAQ");
   // Rome
   buffer.AppendFormatted(dependFormatFrame.Data(),"RomeDAQ","RomeDAQ","RomeDAQ","RomeDAQ","RomeDAQ");
   buffer.AppendFormatted("obj/%sRomeDAQ.obj: src/framework/%sRomeDAQ.cpp obj/%sRomeDAQ.d",shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("%s",romeDep.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted(compileFormatFrame.Data(),"RomeDAQ","RomeDAQ");
   // main
   buffer.AppendFormatted(dependFormatBlank.Data(),"main","src/framework/main","main","main");
   buffer.AppendFormatted("obj/main.obj: src/framework/main.cpp include/framework/%sAnalyzer.h obj/main.d\n",shortCut.Data());
   buffer.AppendFormatted(compileFormatBlank.Data(),"src/framework/main","main");

   // Framework dict
   buffer.AppendFormatted("obj/%sFrameworkDict.obj: %sFrameworkDict.h %sFrameworkDict.cpp\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   tempBuffer.SetFormatted("%sFrameworkDict",shortCut.Data());
   buffer.AppendFormatted(compileFormatBlank.Data(),tempBuffer.Data(),tempBuffer.Data());

   // User dict
   buffer.AppendFormatted("obj/%sUserDict.obj: %sUserDict.h %sUserDict.cpp\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   tempBuffer.SetFormatted("%sUserDict",shortCut.Data());
   buffer.AppendFormatted(compileFormatBlank.Data(),tempBuffer.Data(),tempBuffer.Data());

   WriteAdditionalSourceFilesCompileCommands(buffer);
}
void ROMEBuilder::WriteFrameWorkAdditionalCompileStatements(ROMEString& buffer) {
   // EventLoop
   buffer.AppendFormatted(dependFormatFrame.Data(),"EventLoop","EventLoop","EventLoop","EventLoop","EventLoop");
   buffer.AppendFormatted("obj/%sEventLoop.obj: src/framework/%sEventLoop.cpp obj/%sEventLoop.d",shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("%s",eventLoopDep.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted(compileFormatFrame.Data(),"EventLoop","EventLoop");
}

void ROMEBuilder::WriteAdditionalSourceFilesObjects(ROMEString& buffer) {
   // Write Additional Source Files Objects
   int i,j,k;
   for (i=0;i<numOfMFSources;i++) {
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         commandLineFlag[j] = false;
         for (k=0;k<flags.GetEntriesFast();k++) {
            if (mfSourceFileFlag[i][j]==flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!IFDEF %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("ifdef %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      buffer.AppendFormatted("objects %s $(objects) obj/%s.obj\n",EqualSign(),mfSourceFileName[i].Data());
      for (j=numOfMFSourceFlags[i]-1;j>=0;j--) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!ENDIF # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("endif # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      delete [] commandLineFlag;
   }
}
void ROMEBuilder::WriteAdditionalSourceFilesCompileCommands(ROMEString& buffer) {
   // Write Additional Source Files Compile Commands
   int i,j,k;
   ROMEString compileFormatAny;
   ROMEString tempBuffer;
#if defined( R__UNIX )
   compileFormatAny.SetFormatted  ("\tg++ -c $(Flags) $(Includes) %%s -o obj/%%s.obj\n");
#endif // R__UNIX
#if defined( R__VISUAL_CPLUSPLUS )
   compileFormatAny.SetFormatted  ("\tcl /c $(Flags) $(Includes) %%s /Foobj/%%s.obj\n");
#endif // R__VISUAL_CPLUSPLUS

   for (i=0;i<numOfMFSources;i++) {
      bool *commandLineFlag = new bool[numOfMFSourceFlags[i]];
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         commandLineFlag[j] = false;
         for (k=0;k<flags.GetEntriesFast();k++) {
            if (mfSourceFileFlag[i][j]==flags[k])
               commandLineFlag[j] = true;
         }
      }
      for (j=0;j<numOfMFSourceFlags[i];j++) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!IFDEF %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("ifdef %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      buffer.AppendFormatted("obj/%s.obj: %s\n",mfSourceFileName[i].Data(),mfSourceFileDep[i].Data());
      tempBuffer.SetFormatted("%s%s.%s",mfSourceFilePath[i].Data(),mfSourceFileName[i].Data(),mfSourceFileType[i].Data());
      buffer.AppendFormatted(compileFormatAny.Data(),tempBuffer.Data(),mfSourceFileName[i].Data());
      for (j=numOfMFSourceFlags[i]-1;j>=0;j--) {
         if (commandLineFlag[j])
            continue;
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("!ENDIF # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
         buffer.AppendFormatted("endif # %s\n",mfSourceFileFlag[i][j].Data());
#endif // R__UNIX
      }
      delete [] commandLineFlag;
   }
}
void ROMEBuilder::WriteUserMakeFile()
{
   // Write Makefile.usr
   struct stat buf;
   ROMEString makeFile;
#if defined( R__UNIX )
   makeFile = "Makefile.usr";
   ROMEString usrBuffer;
   if( stat( makeFile.Data(), &buf )) {
      usrBuffer.SetFormatted("# User editable Makefile for the %s%s\n",shortCut.Data(),mainProgName.Data());
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("# Description:\n");
      usrBuffer.AppendFormatted("# 1) Add compile(link) options to Flags(Libraries), e.g. Flags += -g -O2 -Wall\n");
      usrBuffer.AppendFormatted("# 2) Add mySource.obj to the list of objects, e.g. objects += mySource.obj\n");
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource.obj: mySource.cpp\n");
      usrBuffer.AppendFormatted("#\tg++ -c $(Flags) $(Includes) mySource.cpp -o obj/mySource.obj\n");
      usrBuffer.AppendFormatted("# 4) Add include paths for the rootcint, e.g. DictionaryIncludes += -ImyPath\n");
      usrBuffer.AppendFormatted("# 5) Add header files for the rootcint, e.g. DictionaryHeaders += myHeader.h/\n");
      usrBuffer.AppendFormatted("# 6) Add clean target, e.g.\n");
      usrBuffer.AppendFormatted("#       userclean:\n");
      usrBuffer.AppendFormatted("#\trm your_file.h\n");
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("userclean:\n",shortCut.Data());
      usrBuffer.AppendFormatted("\t@echo ''\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
      WriteFile(makeFile.Data(),usrBuffer.Data(),0);
   }
#endif
#if defined ( R__VISUAL_CPLUSPLUS )
   makeFile = "Makefile.winusr";
   ROMEString usrBuffer;
   if( stat( makeFile.Data(), &buf )) {
      usrBuffer.SetFormatted("# User editable Makefile for the %s%s\n",shortCut.Data(),mainProgName.Data());
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("# Description:\n");
      usrBuffer.AppendFormatted("# 1) Add compile(link) options to Flags(Libraries), e.g. Flags = $(Flags) /GX /GR\n");
      usrBuffer.AppendFormatted("# 2) Add mySource.obj to the list of objects, e.g. objects = $(objects) mySource.obj\n");
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource.obj: mySource.cpp\n");
      usrBuffer.AppendFormatted("#\tcl /c $(Flags) $(Includes) mySource.cpp /Foobj/mySource.obj\n");
      usrBuffer.AppendFormatted("# 4) Add include paths for the rootcint, e.g. DictionaryIncludes = $(DictionaryIncludes) -ImyPath\n");
      usrBuffer.AppendFormatted("# 5) Add header files for the rootcint, e.g. DictionaryHeaders = $(DictionaryHeaders) myHeader.h/\n");
      usrBuffer.AppendFormatted("# 6) Add clean target, e.g.\n");
      usrBuffer.AppendFormatted("#       userclean:\n");
      usrBuffer.AppendFormatted("#\trm your_file.h\n");
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("userclean:\n",shortCut.Data());
      usrBuffer.AppendFormatted("\t@echo ''\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
      WriteFile(makeFile.Data(),usrBuffer.Data(),0);
   }
#endif
}
void ROMEBuilder::WriteROMEDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the ROME headers
   buffer.AppendFormatted("%sROMEDict.h %sROMEDict.cpp:",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(ROMESYS)/include/ROMETask.h $(ROMESYS)/include/ROMETreeInfo.h $(ROMESYS)/include/ROMEAnalyzer.h\n");
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f %sROMEDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f %sROMEDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
#endif
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   buffer.AppendFormatted(" ROMETask.h ROMETreeInfo.h ROMEAnalyzer.h");
   buffer.Append("\n\n");
}

void ROMEBuilder::WriteFolderDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the folder headers
   int i;
   buffer.AppendFormatted("%sFolderDict.h %sFolderDict.cpp:",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(FolderIncludes)");
   buffer.AppendFormatted(" $(BaseFolderIncludes)");
   buffer.Append("\n");
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f %sFolderDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f %sFolderDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
#endif
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   buffer.AppendFormatted(" -I. -Iinclude -Iinclude/framework");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted(" include/framework/%s%s.h",shortCut.Data(),folderName[i].Data());
         if (folderUserCode[i])
            buffer.AppendFormatted(" include/framework/%s%s_Base.h",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.Append("\n\n");
}
void ROMEBuilder::WriteFrameworkDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the framework headers
   buffer.AppendFormatted("%sFrameworkDict.h %sFrameworkDict.cpp:",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" include/framework/%sAnalyzer.h\n",shortCut.Data());
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f %sFrameworkDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f %sFrameworkDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
#endif
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   buffer.AppendFormatted(" -I. -Iinclude -Iinclude/framework");
   buffer.AppendFormatted(" include/framework/%sAnalyzer.h",shortCut.Data());
   buffer.Append("\n\n");
}
void ROMEBuilder::WriteTaskDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the task headers
   int i;
   buffer.AppendFormatted("%sTaskDict.h %sTaskDict.cpp:",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(TaskIncludes)");
   buffer.AppendFormatted(" $(BaseTaskIncludes)");
   buffer.AppendFormatted("\n");
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\tDYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\tLD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f %sTaskDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f %sTaskDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
#endif
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   buffer.AppendFormatted(" -I. -Iinclude -Iinclude/framework");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("  include/tasks/%sT%s.h",shortCut.Data(),taskName[i].Data());
      if (taskUserCode[i])
         buffer.AppendFormatted(" include/tasks/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
   }
   buffer.Append("\n\n");
}
void ROMEBuilder::WriteUserDictionary(ROMEString& buffer)
{
   // writes a script file that executes rootcint for the user headers
   int i;
   buffer.AppendFormatted("%sUserDict.h %sUserDict.cpp:",shortCut.Data(),shortCut.Data());
   for (i=0;i<numOfMFDictHeaders;i++)
      buffer.AppendFormatted(" %s",mfDictHeaderName[i].Data());
   buffer.AppendFormatted(" $(DictionaryHeaders)\n",shortCut.Data());
#if defined( R__UNIX )
#   if defined( R__MACOSX )
   buffer.AppendFormatted("\t-DYLD_LIBRARY_PATH=$(DYLD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   else
   buffer.AppendFormatted("\t-LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(shell $(ROOTSYS)/bin/root-config --libdir) ");
#   endif
#else
   buffer.AppendFormatted("\t-");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("%%ROOTSYS%%\\bin\\rootcint -f %sUserDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I%%ROMESYS%%/include");
   buffer.AppendFormatted(" -I%%ROOTSYS%%/include");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("$(ROOTSYS)/bin/rootcint -f %sUserDict.cpp -c -p",shortCut.Data());
   buffer.AppendFormatted(" -I$(ROMESYS)/include");
   buffer.AppendFormatted(" -I$(shell $(ROOTSYS)/bin/root-config --incdir)");
#endif
   buffer.AppendFormatted(" $(DictionaryIncludes)");
   buffer.AppendFormatted(" -I. -Iinclude -Iinclude/framework");
   for (i=0;i<numOfMFDictIncDirs;i++)
      buffer.AppendFormatted(" -I%s",mfDictIncDir[i].Data());
   buffer.AppendFormatted(" $(DictionaryHeaders)");
   for (i=0;i<numOfMFDictHeaders;i++)
      buffer.AppendFormatted(" %s",mfDictHeaderName[i].Data());
   buffer.Append("\n\n");
}

void ROMEBuilder::WriteHTMLDoku() {

   int i=0,j=0,k=0;
   ROMEString buffer;
   ROMEString parentt;
   ROMEString format;
   int depthold=0;
   int depth=0;
   bool trodd = true;

   // Header
   buffer.Resize(0);
   buffer.AppendFormatted("<HTML>\n");
   buffer.AppendFormatted("<HEAD>\n");
   buffer.AppendFormatted("<TITLE>%s%s Manual</TITLE>\n",shortCut.Data(),mainProgName.Data());
   if(styleSheet.Length()){
      buffer.AppendFormatted("<LINK rel=\"stylesheet\" type=\"text/css\" href=\"%s/%s\">\n",outDir.Data(),styleSheet.Data());
   }
   else{
      buffer.AppendFormatted("<STYLE>\n");
      WriteHTMLStyle(buffer);
      buffer.AppendFormatted("</STYLE>\n");
   }
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
   buffer.AppendFormatted("<li><a href=\"#steers\">Steering Parameters</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#objects\">Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#taskobjects\">Tasks</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#folderobjects\">Folders</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#treeobjects\">Trees</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#midasbankobjects\">Midas Banks</a></li>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/htmldoc/ClassIndex.html\">Class Overview</A></li>\n",outDir.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/%sUserHTML.html\">Additional Info</A></li>\n",outDir.Data(),shortCut.Data());
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
   // Steering parameters
   buffer.AppendFormatted("<H2><a name=steers>Steering Parameters</a> </H2>\n");
   buffer.AppendFormatted("\n");
   for(i=0;i<=numOfTask;i++){
      if(numOfSteering[i] < 1)
         continue;
      if(i<numOfTask)
         buffer.AppendFormatted("<a class=\"object\">%s</a><br>\n",taskName[i].Data());
      else
         buffer.AppendFormatted("<a class=\"object\">Global Steering Parameters</a><br>\n");
      buffer.AppendFormatted("<table>\n");
      buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      trodd = !trodd;
      WriteHTMLSteering(buffer,0,i,"");
      buffer.AppendFormatted("</table><br>\n");
   }
   buffer.AppendFormatted("\n");
   // Objects
   buffer.AppendFormatted("<H2><a name=objects>Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("All <a href=\"#taskobjects\">Tasks</a>, <a href=\"#folderobjects\">Folders</a>, <a href=\"#treeobjects\">Trees</a> and <a href=\"#midasbankobjects\">Midas Banks</a> are described here.\n");
   // Tasks
   buffer.AppendFormatted("<h3><a name=taskobjects>Tasks</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s has the following task hierarchy :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

   int index;
   int ddelta;
   buffer.AppendFormatted("<ul>\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      index = i;
      depth=0;
      while (index!=-1) {
         depth++;
         index = taskHierarchyParentIndex[index];
      }
      depth--;
      ddelta = depth-depthold;
      if (ddelta>0) for (k=0;k<ddelta;k++)  buffer.AppendFormatted("<ul>\n");
      if (ddelta<0) for (k=0;k<-ddelta;k++) buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<li type=\"circle\"><h4><a href=\"#%s\">%sT%s</a></h4></li>\n",taskHierarchyName[i].Data(),shortCut.Data(),taskHierarchyName[i].Data());
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
   fstream *fileStream;
   ROMEString fileBuffer;
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("<a name=%s class=\"object\">%s</a><br>\n",taskName[i].Data(),taskName[i].Data());
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
      fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      buffer.AppendFormatted("%s accesses data from the following folders :\n",taskName[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j=0;j<numOfFolder;j++) {
         if (accessFolder(fileBuffer,j))
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[j].Data());
      }
      buffer.AppendFormatted("</ul>\n");
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
         for (j=0;j<maxNumberOfFolders;j++) {
            for (k=0;k<numOfFolder;k++) {
               if (parentt==folderName[k]) {
                  parentt = folderParentName[k];
                  break;
               }
            }
            if (k>=numOfFolder) {
               cout << "Invalid folder structure." << endl;
               return;
            }
            if (folderParentName[k]=="GetMainFolder()")
               break;
            depth++;
         }
      }
      ddelta = depth-depthold;
      if (ddelta>0) for (k=0;k<ddelta;k++)  buffer.AppendFormatted("<ul>\n");
      if (ddelta<0) for (k=0;k<-ddelta;k++) buffer.AppendFormatted("</ul>\n");
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
      buffer.AppendFormatted("<a name=%s class=\"object\">%s</a><br>\n",folderName[i].Data(),folderName[i].Data());
      buffer.AppendFormatted("%s\n",folderDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<u>Fields</u>\n");
      buffer.AppendFormatted("<table>\n");
      buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         ROMEString comment = valueComment[i][j];
         if (valueComment[i][j].Length()>3) {
            if (valueComment[i][j][0]=='/') {
               comment = valueComment[i][j](3,valueComment[i][j].Length()-3);
            }
         }
         buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",trodd ? "odd" : "even",valueName[i][j].Data(),valueType[i][j].Data(),comment.Data());
         trodd = !trodd;
      }
      buffer.AppendFormatted("</table><br>\n");

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
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
            buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankName[i][j].Data());
            buffer.AppendFormatted("<ul>\n");
            for (k=0;k<numOfStructFields[i][j];k++) {
               if (bankFieldArraySize[i][j][k]=="1") {
                  buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",structFieldName[i][j][k].Data());
               }
               else {
                  buffer.AppendFormatted("<li type=\"disc\">%s[%s]</li>\n",structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data());
               }
            }
            buffer.AppendFormatted("</ul>\n");
         }
         else {
            buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Access Methods
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("For a description of all access methods to all types of objects in the %s%s please visit the\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<A TARGET=_top HREF=\"http://midas.psi.ch/rome/usersGuide.html\">users guide on the ROME homepage</A>.\n");
   buffer.AppendFormatted("The <A TARGET=_top HREF=\"http://midas.psi.ch/rome/usersGuide.html#fwaccess\">Access Methods</A> subsection of the\n");
   buffer.AppendFormatted("<A TARGET=_top HREF=\"http://midas.psi.ch/rome/usersGuide.html#framework\">The generated Framework</A> section\n");
   buffer.AppendFormatted("describes all access methods of frameworks generated with ROME.\n");
   buffer.AppendFormatted("<p>\n");

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

   // Write documentation
   ROMEString htmlFile;
   htmlFile.SetFormatted("%s/%s%s.html",outDir.Data(),shortCut.Data(),mainProgName.Data());
   WriteFile(htmlFile.Data(),buffer.Data(),0);

   struct stat buf;
   // Write style sheet
   ROMEString css;
   if(styleSheet.Length()) {
      css.SetFormatted("%s/%s",outDir.Data(),styleSheet.Data());
      if( stat( css.Data(), &buf )) {
         buffer.Resize(0);
         WriteHTMLStyle(buffer);
         WriteFile(css.Data(),buffer.Data(),0);
      }
   }

   // Write UserHTML
   ROMEString userHtmlFile;
   userHtmlFile.SetFormatted("%s/%sUserHTML.html",outDir.Data(),shortCut.Data());
   if( stat( userHtmlFile.Data(), &buf )) {
      buffer.Resize(0);
      buffer.AppendFormatted("<html>\n");
      buffer.AppendFormatted("<head>\n");
      buffer.AppendFormatted("  <title>%s%s Additional Info</title>\n",shortCut.Data(),mainProgName.Data());
      if(styleSheet.Length()){
         buffer.AppendFormatted("<LINK rel=\"stylesheet\" type=\"text/css\" href=\"%s/%s\">\n",outDir.Data(),styleSheet.Data());
      }
      else{
         buffer.AppendFormatted("<STYLE>\n");
         WriteHTMLStyle(buffer);
         buffer.AppendFormatted("</STYLE>\n");
      }
      buffer.AppendFormatted("</head>\n");
      buffer.AppendFormatted("<body>\n");
      buffer.AppendFormatted("  <h1>Additional Info</h1>\n");
      buffer.AppendFormatted("  <a href=\"%s%s.html\">Main page</a>\n",shortCut.Data(),mainProgName.Data());
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("\n\n\n\n");
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("<address>\n");
      buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n",shortCut.Data());
      buffer.AppendFormatted("%s</br>\n",mainAuthor.Data());
      buffer.AppendFormatted("%s</br>\n",mainInstitute.Data());
      buffer.AppendFormatted("%s</br>\n",mainCollaboration.Data());
      buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail.Data(),mainEmail.Data());
      buffer.AppendFormatted("<u> Contact person from ROME</u></br>\n");
      buffer.AppendFormatted("Matthias Schneebeli (PSI)</br>\n");
      buffer.AppendFormatted("email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
      buffer.AppendFormatted("</address>\n");
      buffer.AppendFormatted("</body>\n");
      buffer.AppendFormatted("</html>");
      WriteFile(userHtmlFile.Data(),buffer.Data(),0);
   }
}

void ROMEBuilder::WriteHTMLSteering(ROMEString &buffer,int numSteer,int numTask,const char* group)
{
   int k;
   ROMEString comment;
   ROMEString groupName;
   bool trodd = true;
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      comment = steerFieldComment[numTask][numSteer][k];
      if (steerFieldComment[numTask][numSteer][k].Length()>3) {
         if (steerFieldComment[numTask][numSteer][k][0]=='/') {
            comment = steerFieldComment[numTask][numSteer][k](3,steerFieldComment[numTask][numSteer][k].Length()-3);
         }
      }
      buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",trodd ? "odd" : "even",steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),comment.Data());
      trodd = !trodd;
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      groupName = group;
      if(groupName.Length())
         groupName.AppendFormatted("/");
      groupName.AppendFormatted("%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("<tr class=\"group\"><td colspan=\"3\">&nbsp;%s&nbsp;</td></tr>\n",groupName.Data());
      WriteHTMLSteering(buffer,steerChildren[numTask][numSteer][k],numTask,groupName.Data());
   }
}

void ROMEBuilder::WriteHTMLStyle(ROMEString& buffer){
      buffer.AppendFormatted("   body { color: #000000; background-color: #ffffff; }\n");
      buffer.AppendFormatted("   h1 { color: blue; }\n");
      buffer.AppendFormatted("   h2 { color: green; }\n");
      buffer.AppendFormatted("   table { border: 1; }\n");
      buffer.AppendFormatted("   a.object { text-decoration: underline; font-weight: bold; }\n");
      buffer.AppendFormatted("   tr.cont { color: #ffffff; background-color: #819cc8; font-weight: bold; }\n");
      buffer.AppendFormatted("   tr.group { background-color: #d0d0ee; }\n");
      buffer.AppendFormatted("   tr.even { background-color: #e0e0e0; }\n");
      buffer.AppendFormatted("   tr.odd { background-color: #f0f0f0; }\n");
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

void ROMEBuilder::setValue(ROMEString* buf,const char *destination,const char *source,const char *type,int version)
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
bool ROMEBuilder::isNumber(const char* string)
{
   if (strcmp(string,"float")&&strcmp(string,"Float_t")&&
       strcmp(string,"double")&&strcmp(string,"Double_t")&&strcmp(string,"Double32_t")&&
       strcmp(string,"int")&&strcmp(string,"Int_t")&&
       strcmp(string,"unsigned int")&&strcmp(string,"UInt_t")&&
       strcmp(string,"long")&&strcmp(string,"Long_t")&&
       strcmp(string,"unsigned long")&&strcmp(string,"ULong_t")&&
       strcmp(string,"short")&&strcmp(string,"Short_t")&&
       strcmp(string,"unsigned short")&&strcmp(string,"UShort_t")&&
       strcmp(string,"Long64_t")&&strcmp(string,"ULong64_t")&&
       strcmp(string,"long long")&&strcmp(string,"unsigned long long"))
      return false;
   return true;
}

bool ROMEBuilder::isFloatingType(const char *type)
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
bool ROMEBuilder::isBoolType(const char *type)
{
   if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      return true;
   }
   return false;
}
bool ROMEBuilder::isIntType(const char *type)
{
   if (
       !strcmp(type,"int") ||
       !strcmp(type,"unsigned int") ||
       !strcmp(type,"Int_t") ||
       !strcmp(type,"UInt_t") ||

       !strcmp(type,"long") ||
       !strcmp(type,"unsigned long") ||
       !strcmp(type,"Long_t") ||
       !strcmp(type,"ULong_t")) {
      return true;
   }
   return false;
}

ROMEString& ROMEBuilder::convertType(const char *value,const char *oldType,const char *newType,ROMEString& stringBuffer) {
   int type = 0;
   if (!strcmp(newType,"int"))
      type = 1;
   else if (!strcmp(newType,"double"))
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

bool ROMEBuilder::isFolder(const char *type) {
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
bool ROMEBuilder::isTArrayType(const char *type) {
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
const char* ROMEBuilder::TArray2StandardType(const char *type) {
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
      if (!strcmp(type,arrayTypes[j][0]))
         return arrayTypes[j][1];
      str.SetFormatted("%s*",arrayTypes[j]);
      if (!strcmp(type,str.Data()))
         return arrayTypes[j][1];
   }
   return type;
}
bool ROMEBuilder::ReplaceHeader(const char* filename,const char* header,const char* body,int nspace) {
   bool writeFile = false;
   fstream *fileStream;
   ROMEString fileBuffer;
   int pBuffer=-1;
   struct stat buf;
   ROMEString buffer = header;
   if( stat( filename, &buf )) {
      writeFile = true;
      buffer += body;
   }
   else {
      // compare old and new file
      if(!(fileStream = new fstream(filename,ios::in))){
         if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
         return false;
      }
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      pBuffer = fileBuffer.Index("/////////////////////////////////////----///////////////////////////////////////");
      if (pBuffer<0) {
         if (makeOutput) cout << "\n\nError : File '" << filename << "' has an invalid header !!!" << endl;
         return false;
      }
      if(fileBuffer(0,pBuffer+80) != header)
         writeFile = true;
   }
   if (writeFile) {
      if(pBuffer>=0)
         buffer += fileBuffer(pBuffer+80,fileBuffer.Length());
      if(!(fileStream = new fstream(filename,ios::out | ios::trunc))){
         if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
         return false;
      }
      if (makeOutput) cout << setw(nspace) << "" << filename << endl;
      *fileStream<<buffer.Data();
      delete fileStream;
   }
   return true;
}

bool ROMEBuilder::WriteFile(const char* filename,const char* body,int nspace) {
   fstream *fileStream;
   ROMEString fileBuffer;

   if((fileStream = new fstream(filename,ios::in))){
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
   }
   if(fileBuffer != body){
      if(!(fileStream = new fstream(filename,ios::out | ios::trunc))){
         if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
         return false;
      }
      if (makeOutput) cout << setw(nspace)<< "" << filename << endl;
      *fileStream<<body;
      delete fileStream;
   }
   return true;
}
bool ROMEBuilder::accessFolder(ROMEString &fileBuffer, int numFolder) {
   if (folderSupport[numFolder])
      return false;

   ROMEString str;

   // Get
   str = "Get";
   str += folderName[numFolder];
   if (folderArray[numFolder]!="1")
      str += "At";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Get copy
   str = "Set";
   str += folderName[numFolder];
   str += "Copy";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Get size
   str = "Set";
   str += folderName[numFolder];
   str += "Size";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Set
   str = "Set";
   str += folderName[numFolder];
   if (folderArray[numFolder]!="1")
      str += "At";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Set copy
   str = "Set";
   str += folderName[numFolder];
   str += "Copy";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Set size
   if (folderArray[numFolder]=="variable"){
      str = "Set";
      str += folderName[numFolder];
      str += "Size(";
      if (fileBuffer.Contains(str))
         return true;
   }

   // Add
   str = "Add";
   str += folderName[numFolder];
   if (folderArray[numFolder]!="1")
      str += "At";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   return false;
}
