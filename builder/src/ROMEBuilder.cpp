/********************************************************************

  ROMEBuilder.cpp, M. Schneebeli PSI

  $Log$
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
#include <Riostream.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

//MEGFrameWork.xml -o C:/Data/analysis/MEG/ROME/MEGFrameWork/ -v -nl
//MEGDriftChamber.xml -o C:/Data/analysis/MEG/ROME/MEGDriftChamber/ -v -nl
//MEGTest.xml -o C:/Data/analysis/MEG/ROME/MEGTest/ -v -nl

#include "ROMEBuilder.h"

bool ROMEBuilder::ReadXMLFolder() {
   char parent[10][100];
   char tmp[100];
   char* name;
   int type,i,j,isub=0;
   strcpy(parent[0],"GetMainFolder()");

   if (makeOutput) cout << "Folders:" << endl;
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // read subfolder
      if (type == 1 && !strcmp((const char*)name,"SubFolder")) {
         numOfFolder++;
         // default initialisation
         strcpy(version[numOfFolder],"1");
         strcpy(author[numOfFolder],mainAuthor);
         strcpy(folderDescription[numOfFolder],"");
         strcpy(folderTitle[numOfFolder],"");
         strcpy(folderArray[numOfFolder],"1");
         dataBase[numOfFolder] = false;
         numOfValue[numOfFolder] = 0;
         numOfGetters[numOfFolder] = 0;
         numOfSetters[numOfFolder] = 0;
         numOfInclude[numOfFolder] = 0;
         // set parent
         strcpy(parentFolderName[numOfFolder],parent[isub]);
         // read folder name
         if (!xml->GetAttribute("Name",folderName[numOfFolder])) {
            if (!xml->GetAttribute("FolderName",folderName[numOfFolder])) {
               cout << "Folder " << numOfFolder << " has no name defined !" << endl;
               cout << "Terminating program." << endl;
               return false;
            }
         }
         // read folder title
         xml->GetAttribute("Title",folderTitle[numOfFolder]);
         xml->GetAttribute("FolderTitle",folderTitle[numOfFolder]);
         // read array flag
         xml->GetAttribute("ArraySize",folderArray[numOfFolder]);
         xml->GetAttribute("Array",folderArray[numOfFolder]);
         // read data base flag
         xml->GetAttribute("DataBase",tmp);
         if (!strcmp(tmp,"yes")) 
            dataBase[numOfFolder] = true;

         // set folder as parent for subsequent folders
         isub++;
         strcpy(parent[isub],folderName[numOfFolder]);

         // output
         if (makeOutput) for (i=0;i<isub;i++) cout << "   ";
         if (makeOutput) cout << folderName[numOfFolder] << endl;
      }
      // end subfolder
      if (type == 15 && !strcmp((const char*)name,"SubFolder")) {
         isub--;
      }
      // read folder content
      if (type == 1) {
         // read author
         if (!strcmp((const char*)name,"Author")) {
            xml->GetAttribute("Name",author[numOfFolder]);
         }
         // read version
         else if (!strcmp((const char*)name,"Version")) {
            xml->GetAttribute("Number",version[numOfFolder]);
         }
         // read description
         else if (!strcmp((const char*)name,"Description")) {
            xml->GetAttribute("Text",folderDescription[numOfFolder]);
         }
         // read additional getters
         else if (!strcmp((const char*)name,"AdditionalGetters")) {
            numOfGetters[numOfFolder] = 0;
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  strcpy(getter[numOfFolder][numOfGetters[numOfFolder]],(const char*)name);
                  if (!xml->GetAttribute("Type",getterType[numOfFolder][numOfGetters[numOfFolder]])) {
                     cout << "Getter '" << getter[numOfFolder][numOfGetters[numOfFolder]] << "' of Folder '" << folderName[numOfFolder] << "' has no Type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  numOfGetters[numOfFolder]++;
               }
               if (type == 15 && !strcmp((const char*)name,"AdditionalGetters")) {
                  break;
               }
            }
         }
         // read additional setters
         else if (!strcmp((const char*)name,"AdditionalSetters")) {
            numOfSetters[numOfFolder] = 0;
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  strcpy(setter[numOfFolder][numOfSetters[numOfFolder]],(const char*)name);
                  if (!xml->GetAttribute("Type",setterType[numOfFolder][numOfGetters[numOfFolder]])) {
                     cout << "Setter '" << setter[numOfFolder][numOfGetters[numOfFolder]] << "' of Folder '" << folderName[numOfFolder] << "' has no Type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  numOfSetters[numOfFolder]++;
               }
               if (type == 15 && !strcmp((const char*)name,"AdditionalSetters")) {
                  break;
               }
            }
         }
         // read includes
         else if (!strcmp((const char*)name,"Include")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  strcpy(include[numOfFolder][numOfInclude[numOfFolder]],(const char*)name);
                  localFlag[numOfFolder][numOfInclude[numOfFolder]] = false;
                  xml->GetAttribute("Type",tmp);
                  if (strcmp(tmp,"local")) 
                     localFlag[numOfFolder][numOfInclude[numOfFolder]] = true;
                  numOfInclude[numOfFolder]++;
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
                  strcpy(valueName[numOfFolder][numOfValue[numOfFolder]],name);
                  // field type
                  if (!xml->GetAttribute("Type",valueType[numOfFolder][numOfValue[numOfFolder]])) {
                     cout << "Field '" << valueType[numOfFolder][numOfValue[numOfFolder]] << "' of Folder '" << folderName[numOfFolder] << "' has no Type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // field reference flag
                  strcpy(valueRef[numOfFolder][numOfValue[numOfFolder]],"no");
                  xml->GetAttribute("Reference",valueRef[numOfFolder][numOfValue[numOfFolder]]);
                  // field initialisation
                  if (!strcmp(valueType[numOfFolder][numOfValue[numOfFolder]],"TString"))
                     strcpy(valueInit[numOfFolder][numOfValue[numOfFolder]],"' '");
                  else
                     strcpy(valueInit[numOfFolder][numOfValue[numOfFolder]],"0");
                  xml->GetAttribute("Initialisation",valueInit[numOfFolder][numOfValue[numOfFolder]]);
                  xml->GetAttribute("Init",valueInit[numOfFolder][numOfValue[numOfFolder]]);
                  // field comment
                  strcpy(valueComment[numOfFolder][numOfValue[numOfFolder]],"");
                  xml->GetAttribute("Comment",valueComment[numOfFolder][numOfValue[numOfFolder]]);
                  if (valueComment[numOfFolder][numOfValue[numOfFolder]][0]!='/') {
                     strcpy(tmp,valueComment[numOfFolder][numOfValue[numOfFolder]]);
                     sprintf(valueComment[numOfFolder][numOfValue[numOfFolder]],"// %s",tmp);
                  }
                  // data base path
                  sprintf(dataBasePath[numOfFolder][numOfValue[numOfFolder]],"%s.%s",folderName[numOfFolder],valueName[numOfFolder][numOfValue[numOfFolder]]);
                  xml->GetAttribute("DataBasePath",dataBasePath[numOfFolder][numOfValue[numOfFolder]]);
                  // field count
                  numOfValue[numOfFolder]++;
               }
               if (type == 15 && !strcmp((const char*)name,"Fields")) {
                  for (i=0;i<numOfValue[numOfFolder];i++) {
                     for (j=i+1;j<numOfValue[numOfFolder];j++) {
                        if (!strcmp(valueName[numOfFolder][i],valueName[numOfFolder][j])) {
                           cout << "\nFolder '" << folderName[numOfFolder] << "' has two fields with the name '" << valueName[numOfFolder][i] << "' !" << endl;
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
               if (!strcmp(folderName[i],folderName[j])) {
                  cout << "\nFolder '" << folderName[i] << "' is defined twice !" << endl;
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
   char cppFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb,ll,i,lenTot;
   char *pos;
   int fileHandle;
   char format[100];
   bool writeFile = false;
   char *pBuffer;
   int bufferLen=0;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (numOfValue[iFold] == 0) continue;
// cpp-File
//----------

      sprintf(cppFile,"%s/src/framework/%s%s.cpp",outDir,shortCut,folderName[iFold]);
// Description
//-------------
      sprintf(buffer,"//// Author: %s\n",author[iFold]);
      sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      ll = 74-strlen(shortCut);
      sprintf(format,"// %%s%%-%d.%ds //\n",ll,ll);
      sprintf(buffer+strlen(buffer),format,shortCut,folderName[iFold]);
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      pos = folderDescription[iFold];
      lenTot = strlen(folderDescription[iFold]);
      while (pos-folderDescription[iFold] < lenTot) {
         if (lenTot+(folderDescription[iFold]-pos)<74) i=75;
         else for (i=74;pos[i]!=32&&i>0;i--) {}
         if (i<=0) i=75;
         pos[i] = 0;
         sprintf(buffer+strlen(buffer),"// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"/////////////////////////////////////----///////////////////////////////////////\n\n");

// Close cpp-File
//----------------

      struct stat buf;
      if( stat( cppFile, &buf )) {
// Header Files
//--------------

         sprintf(buffer+strlen(buffer),"#include <include/framework/%s%s.h>\n",shortCut,folderName[iFold]);

         sprintf(buffer+strlen(buffer),"\nClassImp(%s%s)\n",shortCut,folderName[iFold]);

// User Functions
//----------------

         for (i=0;i<numOfGetters[iFold];i++) {
            sprintf(buffer+strlen(buffer),"\n%s %s%s::%s()\n{\n}\n\n",getterType[iFold][i],shortCut,folderName[iFold],getter[iFold][i]);
         }
         writeFile = true;
      }
      else {
         if (numOfGetters[iFold]==0) {
            remove(cppFile);
         }
         else {
            // compare old and new file
            fileHandle = open(cppFile,O_RDONLY);
            nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
            pBuffer = fileBuffer;
            char *pend = "/////////////////////////////////////----///////////////////////////////////////";
            pBuffer = strstr(pBuffer,pend);
            if (pBuffer==NULL||nb-(pBuffer-fileBuffer)<0) {
               if (makeOutput) cout << "\n\nError : File '" << cppFile << "' has an invalid header !!!" << endl;
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
            sprintf(format,"%%-%d.%ds",bufferLen-80);
            sprintf(buffer+strlen(buffer),format,pBuffer+80);
         }
      }
      if (writeFile && numOfGetters[iFold]>0) {
         // write file
         fileHandle = open(cppFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(cppFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << cppFile << endl;

         nb = write(fileHandle,&buffer, strlen(buffer));

         close(fileHandle);
      }
   }
   return true;
}

bool ROMEBuilder::WriteFolderH() {
   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb,j,i;
   const int bufferSize = 600;
   char str[bufferSize];
   int fileHandle;
   char format[100];

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (numOfValue[iFold] == 0) continue;
      // Description
      sprintf(buffer,               "////////////////////////////////////////////////////////////////////////////////\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
      sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

      // Header Files

      sprintf(buffer+strlen(buffer),"#ifndef %s%s_H\n",shortCut,folderName[iFold]);
      sprintf(buffer+strlen(buffer),"#define %s%s_H\n\n",shortCut,folderName[iFold]);

      sprintf(buffer+strlen(buffer),"#include <TObject.h>\n");

      for (i=0;i<numOfValue[iFold];i++) {
         if (!strcmp(valueRef[iFold][i],"yes")) {
            sprintf(buffer+strlen(buffer),"#include <TRef.h>\n");
            break;
         }
      }

      for (i=0;i<numOfInclude[iFold];i++) {
         if (localFlag[iFold][i]) {
            sprintf(buffer+strlen(buffer),"#include \"%s\"\n",include[iFold][i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"#include <%s>\n",include[iFold][i]);
         }
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (!strcmp(valueType[iFold][i],"TString")) {
            sprintf(buffer+strlen(buffer),"#include <%s.h>\n",valueType[iFold][i]);
            break;
         }
      }

      for (i=0;i<numOfValue[iFold];i++) {
         for (j=0;j<numOfFolder;j++) {
            sprintf(str,"%s*",folderName[j]);
            if (!strcmp(valueType[iFold][i],folderName[j]) || !strcmp(valueType[iFold][i],str)) {
               sprintf(buffer+strlen(buffer),"#include <include/framework/%s%s.h>\n",shortCut,folderName[j]);

               strcpy(str,shortCut);
               strcat(str,valueType[iFold][i]);
               strcpy(valueType[iFold][i],str);
               continue;
            }
         }
      }



      // Class

      sprintf(buffer+strlen(buffer),"\nclass %s%s : public TObject\n",shortCut,folderName[iFold]);
      sprintf(buffer+strlen(buffer),"{\n");

      // Fields

      sprintf(buffer+strlen(buffer),"protected:\n");
      int typeLen = 5;
      int nameLen = 8;
      for (i=0;i<numOfValue[iFold];i++) {
         if (typeLen<(int)strlen(valueType[iFold][i])) typeLen = strlen(valueType[iFold][i]);
         if (nameLen<(int)strlen(valueName[iFold][i])) nameLen = strlen(valueName[iFold][i]);
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (!strcmp(valueRef[iFold][i],"yes")) {
            sprintf(format,"   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-strlen(valueName[iFold][i]));
            sprintf(buffer+strlen(buffer),format,"TRef",valueName[iFold][i],"",valueComment[iFold][i]);
         }
         else {
            sprintf(format,"   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-strlen(valueName[iFold][i]));
            sprintf(buffer+strlen(buffer),format,valueType[iFold][i],valueName[iFold][i],"",valueComment[iFold][i]);
         }
      }
      sprintf(buffer+strlen(buffer),"\n");
      sprintf(format,"   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-8);
      sprintf(buffer+strlen(buffer),format,"Bool_t","Modified","","//! Modified Folder Flag");
      sprintf(buffer+strlen(buffer),"\n");

      // Methods

      sprintf(buffer+strlen(buffer),"public:\n");
      // Constructor
      sprintf(buffer+strlen(buffer),"   %s%s(",shortCut,folderName[iFold]);
      for (i=0;i<numOfValue[iFold];i++) {
         sprintf(buffer+strlen(buffer),"%s %s=%s,",valueType[iFold][i],valueName[iFold][i],valueInit[iFold][i]);
      }
      sprintf(buffer+strlen(buffer)-1,")\n");
      sprintf(buffer+strlen(buffer),"   { ");
      for (i=0;i<numOfValue[iFold];i++) {
         sprintf(buffer+strlen(buffer),"f%s = %s; ",valueName[iFold][i],valueName[iFold][i]);
      }
      sprintf(buffer+strlen(buffer),"fModified = false; ");
      sprintf(buffer+strlen(buffer),"};\n");
      sprintf(buffer+strlen(buffer),"\n");

      // Getters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-strlen(valueName[iFold][i]);
         if (!strcmp(valueRef[iFold][i],"yes")) {
            sprintf(format,"   %%-%ds Get%%s()%%%ds { return (%%s)f%%s.GetObject();%%%ds };\n",typeLen,lb,lb+(typeLen-strlen(valueType[iFold][i])));
            sprintf(buffer+strlen(buffer),format,valueType[iFold][i],valueName[iFold][i],"",valueType[iFold][i],valueName[iFold][i],"");
         }
         else {
            sprintf(format,"   %%-%ds Get%%s()%%%ds { return f%%s;%%%ds };\n",typeLen,lb,lb);
            sprintf(buffer+strlen(buffer),format,valueType[iFold][i],valueName[iFold][i],"",valueName[iFold][i],"");
         }
      }
      // User Getters
      for (i=0;i<numOfGetters[iFold];i++) {
         int lb = nameLen-strlen(getter[iFold][i]);
         sprintf(format,"   %%-%ds %%s();\n",typeLen,lb);
         sprintf(buffer+strlen(buffer),format,getterType[iFold][i],getter[iFold][i]);
      }
      sprintf(buffer+strlen(buffer),"\n");

      // Modifier
      sprintf(format,"   %%-%ds is%%s()%%%ds  { return f%%s;%%%ds };\n",typeLen,nameLen-8,nameLen-8);
      sprintf(buffer+strlen(buffer),format,"Bool_t","Modified","","Modified","");
      sprintf(buffer+strlen(buffer),"\n");
      // Setters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-strlen(valueName[iFold][i]);
         sprintf(format,"   void Set%%s%%%ds(%%-%ds %%s%%%ds) { f%%s%%%ds = %%s;%%%ds fModified = true; };\n",lb,typeLen,lb,lb,lb);
         sprintf(buffer+strlen(buffer),format,valueName[iFold][i],"",valueType[iFold][i],valueName[iFold][i],"",valueName[iFold][i],"",valueName[iFold][i],"");
      }
      sprintf(buffer+strlen(buffer),"\n");
      // Set All
      sprintf(buffer+strlen(buffer),"   void SetAll(");
      for (i=0;i<numOfValue[iFold];i++) {
         sprintf(buffer+strlen(buffer),"%s %s=%s,",valueType[iFold][i],valueName[iFold][i],valueInit[iFold][i]);
      }
      sprintf(buffer+strlen(buffer)-1,")\n");
      sprintf(buffer+strlen(buffer),"   { ");
      for (i=0;i<numOfValue[iFold];i++) {
         sprintf(buffer+strlen(buffer),"f%s = %s; ",valueName[iFold][i],valueName[iFold][i]);
      }
      sprintf(buffer+strlen(buffer),"fModified = true; ");
      sprintf(buffer+strlen(buffer),"};\n");
      sprintf(buffer+strlen(buffer),"\n");
      // Reset
      sprintf(buffer+strlen(buffer),"   void Reset() {");
      for (i=0;i<numOfValue[iFold];i++) {
         sprintf(buffer+strlen(buffer),"f%s = %s; ",valueName[iFold][i],valueInit[iFold][i]);
      }
      sprintf(buffer+strlen(buffer),"fModified = false; ");
      sprintf(buffer+strlen(buffer),"};\n");

// Footer
//--------

      sprintf(buffer+strlen(buffer),"\n   ClassDef(%s%s,%s)\n",shortCut,folderName[iFold],version[iFold]);
      sprintf(buffer+strlen(buffer),"};\n\n");

      sprintf(buffer+strlen(buffer),"#endif   // %s%s_H\n",shortCut,folderName[iFold]);

// Close h-File
//--------------

      sprintf(hFile,"%s/include/framework/%s%s.h",outDir,shortCut,folderName[iFold]);
      fileHandle = open(hFile,O_RDONLY);
      nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      bool identical = true;
      for (i=0;i<nb||i<(int)strlen(buffer);i++) {
         if (buffer[i] != fileBuffer[i]) {
            identical = false;
         }
      }
      if (!identical) {
         fileHandle = open(hFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(hFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << hFile << endl;
         nb = write(fileHandle,&buffer, strlen(buffer));
         close(fileHandle);
      }
   }
   return true;
}

bool ROMEBuilder::ReadXMLTask() {
   char parent[10][100];
   char *name;
   int type,i,j,isub=0;
   int empty,depth=0,index[20];
   char tmp[100];
   strcpy(parent[0],"GetMainTask()");

// read XML file
//===============

   if (makeOutput) cout << "\n\nTasks:" << endl;
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      empty = xml->isEmpty();
      if (type == 1 && !strcmp((const char*)name,"SubTask")) {
         numOfTask++;
         // initialisation
         index[0] = 0;
         numOfTaskSteering[numOfTask] = 0;
         numOfTaskSteerFields[numOfTask][0] = 0;
         strcpy(taskSteerName[numOfTask][0],"Steering");
         strcpy(taskSteerParent[numOfTask][0],"");
         taskSteerDepth[numOfTask][0] = 0;
         strcpy(parentTaskName[numOfTask],parent[isub]);
         strcpy(version[numOfTask],"1");
         strcpy(author[numOfTask],mainAuthor);
         strcpy(taskDescription[numOfTask],"");
         numOfHistos[numOfTask] = 0;
         numOfInclude[numOfTask] = 0;
         // task name
         if (!xml->GetAttribute("Name",taskName[numOfTask])) {
            cout << "SubTask without a name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // trigger id
         strcpy(taskEventID[numOfTask],"all");
         xml->GetAttribute("EventID",taskEventID[numOfTask]);
         // language
         fortranFlag[numOfTask] = false;
         xml->GetAttribute("Language",tmp);
         if (!strcmp(tmp,"Fortran")) fortranFlag[numOfTask] = true;

         // handle subtask
         if (!empty) {
            isub++;
            strcpy(parent[isub],taskName[numOfTask]);
         }

         // output
         if (makeOutput) for (i=0;i<isub;i++) cout << "   ";
         if (makeOutput) cout << taskName[numOfTask] << endl;
      }
      if (type == 15 && !strcmp((const char*)name,"SubTask")) {
         isub--;
      }
      if (type == 1) {
         // author
         if (!strcmp((const char*)name,"Author")) {
            xml->GetAttribute("Name",author[numOfTask]);
         }
         // version
         else if (!strcmp((const char*)name,"Version")) {
            xml->GetAttribute("Number",version[numOfTask]);
         }
         // description
         else if (!strcmp((const char*)name,"Description")) {
            xml->GetAttribute("Text",taskDescription[numOfTask]);
         }
         // includes
         else if (!strcmp((const char*)name,"Include")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  strcpy(include[numOfTask][numOfInclude[numOfTask]],(const char*)name);
                  localFlag[numOfTask][numOfInclude[numOfTask]] = false;
                  xml->GetAttribute("Type",tmp);
                  if (strcmp(tmp,"local")) 
                     localFlag[numOfTask][numOfInclude[numOfTask]] = true;
                  numOfInclude[numOfTask]++;
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
                  depth++;
                  index[depth] = numOfTaskSteering[numOfTask];
                  numOfTaskSteerFields[numOfTask][index[depth]] = 0;
                  // steering name
                  strcpy(taskSteerName[numOfTask][index[depth]],(const char*)name);
                  // parent name
                  strcpy(taskSteerParent[numOfTask][index[depth]],taskSteerName[numOfTask][index[depth-1]]);
                  // depth
                  taskSteerDepth[numOfTask][index[depth]] = depth;
                  // output
                  if (makeOutput) for (i=0;i<depth+1;i++) cout << "   ";
                  if (makeOutput) cout << taskSteerName[numOfTask][index[depth]] << endl;
               }
               if (type == 1 && empty) {
                  // field name
                  strcpy(taskSteerFieldName[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],(const char*)name);
                  // field type
                  if (!xml->GetAttribute("Type",taskSteerFieldType[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]])) {
                     cout << "Steering Parameter " << taskSteerFieldName[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]] << " has no type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // field initialisation
                  if (!strcmp(taskSteerFieldType[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"TString"))
                     strcpy(taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"' '");
                  else
                     strcpy(taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"0");
                  xml->GetAttribute("Initialisation",taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]]);
                  xml->GetAttribute("Init",taskSteerFieldInit[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]]);
                  // field comment
                  strcpy(taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"");
                  xml->GetAttribute("Comment",taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]]);
                  if (taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]][0]!='/') {
                     strcpy(tmp,taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]]);
                     sprintf(taskSteerFieldComment[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]],"// %s",tmp);
                  }
                  // output
                  if (makeOutput) for (i=0;i<depth+2;i++) cout << "   ";
                  if (makeOutput) cout << taskSteerFieldName[numOfTask][index[depth]][numOfTaskSteerFields[numOfTask][index[depth]]] << endl;
                  numOfTaskSteerFields[numOfTask][index[depth]]++;
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
                  strcpy(histoName[numOfTask][numOfHistos[numOfTask]],(const char*)name);
                  // histo type
                  if (!xml->GetAttribute("Type",histoType[numOfTask][numOfHistos[numOfTask]])) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no type defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo array size
                  strcpy(histoArray[numOfTask][numOfHistos[numOfTask]],"1");
                  xml->GetAttribute("ArraySize",histoArray[numOfTask][numOfHistos[numOfTask]]);
                  // histo title
                  strcpy(histoTitle[numOfTask][numOfHistos[numOfTask]],"");
                  xml->GetAttribute("Title",histoTitle[numOfTask][numOfHistos[numOfTask]]);
                  // histo folder name
                  if (!xml->GetAttribute("FolderName",histoFolderName[numOfTask][numOfHistos[numOfTask]])) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Folder Name defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo folder title
                  strcpy(histoFolderTitle[numOfTask][numOfHistos[numOfTask]],"");
                  xml->GetAttribute("FolderTitle",histoFolderTitle[numOfTask][numOfHistos[numOfTask]]);
                  // histo xbins
                  if (!xml->GetAttribute("NumberOfBinsX",histoXBin[numOfTask][numOfHistos[numOfTask]])) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no number of X bins defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo xmin
                  if (!xml->GetAttribute("XMin",histoXMin[numOfTask][numOfHistos[numOfTask]])) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no X minimum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo xmax
                  if (!xml->GetAttribute("XMax",histoXMax[numOfTask][numOfHistos[numOfTask]])) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no X maximum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // histo ybins
                  if (!xml->GetAttribute("NumberOfBinsY",histoYBin[numOfTask][numOfHistos[numOfTask]])) {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no number of Y bins defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo ymin
                  if (!xml->GetAttribute("YMin",histoYMin[numOfTask][numOfHistos[numOfTask]])) {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Y minimum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo ymax
                  if (!xml->GetAttribute("YMax",histoYMax[numOfTask][numOfHistos[numOfTask]])) {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Y maximum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo zbins
                  if (!xml->GetAttribute("NumberOfBinsZ",histoZBin[numOfTask][numOfHistos[numOfTask]])) {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no number of Z bins defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo zmin
                  if (!xml->GetAttribute("ZMin",histoZMin[numOfTask][numOfHistos[numOfTask]])) {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Z minimum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  // histo zmax
                  if (!xml->GetAttribute("ZMax",histoZMax[numOfTask][numOfHistos[numOfTask]])) {
                     if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                        cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Z maximum defined !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                  }
                  numOfHistos[numOfTask]++;
               }
               if (type == 15 && !strcmp((const char*)name,"Histos")) {
                  for (i=0;i<numOfHistos[numOfTask];i++) {
                     for (j=i+1;j<numOfHistos[numOfTask];j++) {
                        if (!strcmp(histoName[numOfTask][i],histoName[numOfTask][j])) {
                           cout << "\nTask '" << taskName[numOfTask] << "' has two histos with the name '" << histoName[numOfTask][i] << "' !" << endl;
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
               if (!strcmp(taskName[i],taskName[j])) {
                  cout << "\nTask '" << taskName[i] << "' is defined twice !" << endl;
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
bool ROMEBuilder::WriteTaskSteeringClass(char *buffer,int numSteer,int numTask) {
   char format[100];
   char sc[20];
   char blank[20];
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i=0;i<numOfTaskSteerFields[numTask][numSteer];i++) {
      if (typeLen<(int)strlen(taskSteerFieldType[numTask][numSteer][i])) typeLen = strlen(taskSteerFieldType[numTask][numSteer][i]);
      if (nameLen<(int)strlen(taskSteerFieldName[numTask][numSteer][i])) nameLen = strlen(taskSteerFieldName[numTask][numSteer][i]);
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (!strcmp(taskSteerParent[numTask][i],taskSteerName[numTask][numSteer])) {
         if (typeLen<(int)strlen(taskSteerName[numTask][i])+1) typeLen = strlen(taskSteerName[numTask][i])+1;
         if (nameLen<(int)strlen(taskSteerName[numTask][i])) nameLen = strlen(taskSteerName[numTask][i]);
      }
   }
   strcpy(sc,"");

   sprintf(blank,"");   
   for (i=0;i<taskSteerDepth[numTask][numSteer]+1;i++) {
      sprintf(blank+strlen(blank),"   ");   
   }

   sprintf(buffer+strlen(buffer),"\n%sclass %s%s\n",blank,sc,taskSteerName[numTask][numSteer]);
   sprintf(buffer+strlen(buffer),"%s{\n",blank);

   sprintf(buffer+strlen(buffer),"%sprivate:\n",blank);

   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (!strcmp(taskSteerParent[numTask][i],taskSteerName[numTask][numSteer])) {
         WriteTaskSteeringClass(buffer,i,numTask);
      }
   }

   sprintf(buffer+strlen(buffer),"%sprotected:\n",blank);

   // Fields
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      sprintf(format,"%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-strlen(taskSteerFieldName[numTask][numSteer][j]));
      sprintf(buffer+strlen(buffer),format,blank,taskSteerFieldType[numTask][numSteer][j],taskSteerFieldName[numTask][numSteer][j],"",taskSteerFieldComment[numTask][numSteer][j]);
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (!strcmp(taskSteerParent[numTask][i],taskSteerName[numTask][numSteer])) {
         sprintf(format,"%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-strlen(taskSteerName[numTask][i]));
         sprintf(buffer+strlen(buffer),format,blank,taskSteerName[numTask][i],taskSteerName[numTask][i],"",taskSteerName[numTask][i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n%spublic:\n",blank);
   // Constructor
   sprintf(buffer+strlen(buffer),"%s   %s%s() { ",blank,sc,taskSteerName[numTask][numSteer]);
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      sprintf(buffer+strlen(buffer),"f%s = %s; ",taskSteerFieldName[numTask][numSteer][j],taskSteerFieldInit[numTask][numSteer][j]);
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (!strcmp(taskSteerParent[numTask][i],taskSteerName[numTask][numSteer])) {
         sprintf(buffer+strlen(buffer),"f%s = new %s(); ",taskSteerName[numTask][i],taskSteerName[numTask][i]);
      }
   }
   sprintf(buffer+strlen(buffer),"};\n");
   // Getters
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      sprintf(format,"%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen-strlen(taskSteerFieldName[numTask][numSteer][j]));
      sprintf(buffer+strlen(buffer),format,blank,taskSteerFieldType[numTask][numSteer][j],taskSteerFieldName[numTask][numSteer][j],"",taskSteerFieldName[numTask][numSteer][j]);
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (!strcmp(taskSteerParent[numTask][i],taskSteerName[numTask][numSteer])) {
         sprintf(format,"%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen-strlen(taskSteerName[numTask][i]));
         sprintf(buffer+strlen(buffer),format,blank,taskSteerName[numTask][i],taskSteerName[numTask][i],"",taskSteerName[numTask][i]);
      }
   }
   // Setters
   sprintf(buffer+strlen(buffer),"\n");
   for (j=0;j<numOfTaskSteerFields[numTask][numSteer];j++) {
      sprintf(format,"%%s   void Set%%-%ds(%%-%ds %%s)%%%ds { f%%s = %%s; };\n",nameLen,typeLen,nameLen-strlen(taskSteerFieldName[numTask][numSteer][j]));
      sprintf(buffer+strlen(buffer),format,blank,taskSteerFieldName[numTask][numSteer][j],taskSteerFieldType[numTask][numSteer][j],taskSteerFieldName[numTask][numSteer][j],"",taskSteerFieldName[numTask][numSteer][j],taskSteerFieldName[numTask][numSteer][j]);
   }

   // Footer
   sprintf(buffer+strlen(buffer),"%s};\n\n",blank);

   return true;
}
void ROMEBuilder::WriteTaskSteerConfigWrite(char *buffer,int numSteer,int numTask) {
   int i,j;
   char tmp[300];
   char getter[300];
   if (numSteer==0)
      sprintf(buffer+strlen(buffer),"   xml->StartElement(\"SteeringParameters\");\n");
   else
      sprintf(buffer+strlen(buffer),"   xml->StartElement(\"%s\");\n",taskSteerName[numTask][numSteer]);
   for (i=0;i<numOfTaskSteerFields[numTask][numSteer];i++) {
      sprintf(getter,"->Get%s()",taskSteerFieldName[numTask][numSteer][i]);
      int ind = numSteer;
      while (strcmp(taskSteerParent[numTask][ind],"")) {
         for (j=0;j<numOfTaskSteering[numTask];j++) {
            if (!strcmp(taskSteerParent[numTask][ind],taskSteerName[numTask][j])) {
               strcpy(tmp,getter);
               sprintf(getter,"->Get%s()%s",taskSteerName[numTask][ind],tmp);
               ind = j;
               break;
            }
         }
      }
      strcpy(tmp,getter);
      sprintf(getter,"((%sT%s*)%sTask)->GetSP()%s",shortCut,taskName[numTask],taskName[numTask],tmp);
      GetFormat(tmp,taskSteerFieldType[numTask][numSteer][i]);
      sprintf(buffer+strlen(buffer),"   sprintf(value,\"%s\",%s);\n",tmp,getter);
      sprintf(buffer+strlen(buffer),"   xml->WriteElement(\"%s\",value);\n",taskSteerFieldName[numTask][numSteer][i]);
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (!strcmp(taskSteerParent[numTask][i],taskSteerName[numTask][numSteer])) {
         WriteTaskSteerConfigWrite(buffer,i,numTask);
      }
   }
   sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
}
void ROMEBuilder::WriteTaskSteerConfigRead(char *buffer,int numSteer,int numTask) {
   char tmp[300];
   char value[300];
   char path[300];
   char setter[300];
   int i,j;
   char blank[100];
   strcpy(blank,"      ");

   strcpy(path,"");
   int ind = numSteer;
   while (strcmp(taskSteerParent[numTask][ind],"")) {
      for (j=0;j<numOfTaskSteering[numTask];j++) {
         if (!strcmp(taskSteerParent[numTask][ind],taskSteerName[numTask][j])) {
            strcpy(tmp,path);
            sprintf(path,"->Get%s()%s",taskSteerName[numTask][ind],tmp);
            ind = j;
            strcat(blank,"      ");
            break;
         }
      }
   }
   if (numSteer==0)
      sprintf(buffer+strlen(buffer),"%s               if (type == 1 && !strcmp((const char*)name,\"%sParameters\")) {\n",blank,taskSteerName[numTask][numSteer]);
   else
      sprintf(buffer+strlen(buffer),"%s               if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank,taskSteerName[numTask][numSteer]);

   sprintf(buffer+strlen(buffer),"%s                  while (xml->NextLine()) {\n",blank);
   sprintf(buffer+strlen(buffer),"%s                     type = xml->GetType();\n",blank);
   sprintf(buffer+strlen(buffer),"%s                     name = xml->GetName();\n",blank);
   for (i=0;i<numOfTaskSteerFields[numTask][numSteer];i++) {
      setValue(value,"","tmp",taskSteerFieldType[numTask][numSteer][i],1);
      sprintf(setter,"((%sT%s*)%sTask)->GetSP()%s->Set%s((%s)%s)",shortCut,taskName[numTask],taskName[numTask],path,taskSteerFieldName[numTask][numSteer][i],taskSteerFieldType[numTask][numSteer][i],value);
      sprintf(buffer+strlen(buffer),"%s                     if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank,taskSteerFieldName[numTask][numSteer][i]);
      sprintf(buffer+strlen(buffer),"%s                        if (xml->GetValue(tmp)) \n",blank);
      sprintf(buffer+strlen(buffer),"%s                           %s;\n",blank,setter);
      sprintf(buffer+strlen(buffer),"%s                     }\n",blank);
   }
   for (i=0;i<numOfTaskSteering[numTask];i++) {
      if (!strcmp(taskSteerParent[numTask][i],taskSteerName[numTask][numSteer])) {
         WriteTaskSteerConfigRead(buffer,i,numTask);
      }
   }
   if (numSteer==0)
      sprintf(buffer+strlen(buffer),"%s                     if (type == 15 && !strcmp((const char*)name,\"%sParameters\"))\n",blank,taskSteerName[numTask][numSteer]);
   else
      sprintf(buffer+strlen(buffer),"%s                     if (type == 15 && !strcmp((const char*)name,\"%s\"))\n",blank,taskSteerName[numTask][numSteer]);
   sprintf(buffer+strlen(buffer),"%s                        break;\n",blank);
   sprintf(buffer+strlen(buffer),"%s                  }\n",blank);
   sprintf(buffer+strlen(buffer),"%s               }\n",blank);
}
bool ROMEBuilder::WriteTaskCpp() {
   char cppFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int lenTot,ll,i;
   char *pBuffer=NULL;
   int bufferLen=0;
   char *pos;
   int fileHandle;
   char format[100];

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {

// cpp-File
//----------

// Description
//-------------
      sprintf(buffer,"//// Author: %s\n",author[iTask]);
      sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      ll = 73-strlen(shortCut);
      sprintf(format,"// %%sT%%-%d.%ds //\n",ll,ll);
      sprintf(buffer+strlen(buffer),format,shortCut,taskName[iTask]);
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      pos = taskDescription[iTask];
      lenTot = strlen(taskDescription[iTask]);
      while (pos-taskDescription[iTask] < lenTot) {
         if (lenTot+(taskDescription[iTask]-pos)<74) i=75;
         else for (i=74;pos[i]!=32&&i>0;i--) {}
         if (i<=0) i=75;
         pos[i] = 0;
         sprintf(buffer+strlen(buffer),"// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// This task contains the following histgrams :\n");
      for (i=0;i<numOfHistos[iTask];i++) {
         sprintf(buffer+strlen(buffer),"// %s\n",histoName[iTask][i]);
      }
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// The histograms are created and saved automaticaly by the task.\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// The following methods can be used to fill the histogram and to set the\n");
      sprintf(buffer+strlen(buffer),"// right name,title and binwidth of the histogram :\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// Fill<Histogram Name>(double value,double weight)\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// For histogram arrays use :\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// Fill<Histogram Name>At(int index,double value,double weight)\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// If more histogram functions are needed use the following function the get\n");
      sprintf(buffer+strlen(buffer),"// a handle to the histogram and use the root functions.\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// Get<Histogram Name>Handle()\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// For histogram arrays use :\n");
      sprintf(buffer+strlen(buffer),"//\n");
      sprintf(buffer+strlen(buffer),"// Get<Histogram Name>HandleAt(int index)\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"/////////////////////////////////////----///////////////////////////////////////");

// Tests
//-------
      char shortcut[10];
      for (i=0;i<(int)strlen(shortCut);i++) shortcut[i] = (char)tolower(shortCut[i]);
      shortcut[i] = 0;
      char taskname[20];
      for (i=0;i<(int)strlen(taskName[iTask]);i++) taskname[i] = (char)tolower(taskName[iTask][i]);
      taskname[i] = 0;

      bool replaceHeader = true;
      bool replaceBody = true;
      sprintf(cppFile,"%s/src/tasks/%sT%s.cpp",outDir,shortCut,taskName[iTask]);
      struct stat buf;
      int nb=0;
      if( !stat( cppFile, &buf )) {
         replaceBody = false;
         fileHandle = open(cppFile,O_RDONLY);
         nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
         pBuffer = fileBuffer;
         char *pend = "/////////////////////////////////////----///////////////////////////////////////";
         pBuffer = strstr(pBuffer,pend);
         if (pBuffer==NULL) {
            if (makeOutput) cout << "\n\nError : File '" << cppFile << "' has an invalid header !!!" << endl;
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
         fileHandle = open(cppFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(cppFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << cppFile << endl;

// Description
//-------------
         if (replaceHeader) {
            nb = write(fileHandle,&buffer, strlen(buffer));
         }
// Header Files
//--------------

         if (replaceBody) {
            sprintf(buffer,"\n\n#include <include/tasks/%sT%s.h>\n",shortCut,taskName[iTask]);
            sprintf(buffer+strlen(buffer),"#include <Riostream.h>\n");

            sprintf(buffer+strlen(buffer),"\nClassImp(%sT%s)\n\n",shortCut,taskName[iTask]);

// User Functions
//----------------

            if (fortranFlag[iTask]) {
               sprintf(buffer+strlen(buffer),"extern \"C\" void %st%sinit_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"void %sT%s::Init()\n{\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"   %sT%s_Init_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"}\n\n");
               sprintf(buffer+strlen(buffer),"extern \"C\" void %st%sbeginofrun_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"void %sT%s::BeginOfRun()\n{\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"   %sT%s_BeginOfRun_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"}\n\n");
               sprintf(buffer+strlen(buffer),"extern \"C\" void %st%sevent_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"void %sT%s::Event()\n{\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"   %sT%s_Event_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"}\n\n");
               sprintf(buffer+strlen(buffer),"extern \"C\" void %st%sendofrun_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"void %sT%s::EndOfRun()\n{\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"   %sT%s_EndOfRun_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"}\n\n");
               sprintf(buffer+strlen(buffer),"extern \"C\" void %st%sterminate_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"void %sT%s::Terminate()\n{\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"   %sT%s_Terminate_();\n",shortcut,taskname);
               sprintf(buffer+strlen(buffer),"}\n\n");
            }
            else {
               sprintf(buffer+strlen(buffer),"void %sT%s::Init()\n{\n}\n\n",shortCut,taskName[iTask]);
               sprintf(buffer+strlen(buffer),"void %sT%s::BeginOfRun()\n{\n}\n\n",shortCut,taskName[iTask]);
               sprintf(buffer+strlen(buffer),"void %sT%s::Event()\n{\n}\n\n",shortCut,taskName[iTask]);
               sprintf(buffer+strlen(buffer),"void %sT%s::EndOfRun()\n{\n}\n\n",shortCut,taskName[iTask]);
               sprintf(buffer+strlen(buffer),"void %sT%s::Terminate()\n{\n}\n\n",shortCut,taskName[iTask]);
            }

            nb = write(fileHandle,&buffer, strlen(buffer));

// Close cpp-File
//----------------
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
   char cppFile[500];
   char fFile[500];
   char buffer[bufferLength];

   int nb,i;
   int fileHandle;
   struct stat buf;

   if (makeOutput) cout << "\n   Output F-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (fortranFlag[iTask]) {
         char shortcut[10];
         for (i=0;i<(int)strlen(shortCut);i++) shortcut[i] = (char)tolower(shortCut[i]);
         shortcut[i] = 0;
         char taskname[20];
         for (i=0;i<(int)strlen(taskName[iTask]);i++) taskname[i] = (char)tolower(taskName[iTask][i]);
         taskname[i] = 0;
         sprintf(fFile,"%s/src/tasks/%sT%s.f",outDir,shortCut,taskName[iTask]);
         if( !stat( cppFile, &buf )) {
            fileHandle = open(fFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
            if (makeOutput) cout << "      " << fFile << endl;

            sprintf(buffer,               "      subroutine %st%sinit()\n\n",shortcut,taskname);
            sprintf(buffer+strlen(buffer),"      return\n");
            sprintf(buffer+strlen(buffer),"      end\n\n");
            sprintf(buffer+strlen(buffer),"      subroutine %st%sbeginofrun()\n\n",shortcut,taskname);
            sprintf(buffer+strlen(buffer),"      return\n");
            sprintf(buffer+strlen(buffer),"      end\n\n");
            sprintf(buffer+strlen(buffer),"      subroutine %st%sevent()\n\n",shortcut,taskname);
            sprintf(buffer+strlen(buffer),"      return\n");
            sprintf(buffer+strlen(buffer),"      end\n\n");
            sprintf(buffer+strlen(buffer),"      subroutine %st%sendofrun()\n\n",shortcut,taskname);
            sprintf(buffer+strlen(buffer),"      return\n");
            sprintf(buffer+strlen(buffer),"      end\n\n");
            sprintf(buffer+strlen(buffer),"      subroutine %st%sterminate()\n\n",shortcut,taskname);
            sprintf(buffer+strlen(buffer),"      return\n");
            sprintf(buffer+strlen(buffer),"      end\n\n");

            nb = write(fileHandle,&buffer, strlen(buffer));
            close(fileHandle);
         }
      }
   }
   return true;
}
bool ROMEBuilder::WriteTaskH() {
   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int i;
   int fileHandle;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {

// Header Files
//--------------

      sprintf(buffer,"////////////////////////////////////////////////////////////////////////////////\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.               //\n");
      sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

      sprintf(buffer+strlen(buffer),"#ifndef %sT%s_H\n",shortCut,taskName[iTask]);
      sprintf(buffer+strlen(buffer),"#define %sT%s_H\n\n",shortCut,taskName[iTask]);

      sprintf(buffer+strlen(buffer),"#include<TH1.h>\n");
      sprintf(buffer+strlen(buffer),"#include<TH2.h>\n");
      sprintf(buffer+strlen(buffer),"#include<TH3.h>\n");
      sprintf(buffer+strlen(buffer),"#include<ROMETask.h>\n");
         
      for (i=0;i<numOfInclude[iTask];i++) {
         if (localFlag[iTask][i]) {
            sprintf(buffer+strlen(buffer),"#include\"%s\"\n",include[iTask][i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"#include<%s>\n",include[iTask][i]);
         }
      }

      sprintf(buffer+strlen(buffer),"#include <include/framework/%sAnalyzer.h>\n",shortCut);

// Class
//-------

      sprintf(buffer+strlen(buffer),"\nclass %sT%s : public ROMETask\n",shortCut,taskName[iTask]);
      sprintf(buffer+strlen(buffer),"{\n");

// Fields
//--------
      if (numOfTaskSteering[iTask]>0) {
         sprintf(buffer+strlen(buffer),"private:\n");
         WriteTaskSteeringClass(buffer,0,iTask);
         sprintf(buffer+strlen(buffer),"\n");
      }

      char format[100];
      sprintf(buffer+strlen(buffer),"protected:\n");

      sprintf(buffer+strlen(buffer),"   %sAnalyzer* fAnalyzer; // Handle to %sAnalyzer class\n\n",shortCut,shortCut);
      if (numOfTaskSteering[iTask]>0) {
         sprintf(buffer+strlen(buffer),"   Steering* fSteering; // Handle to Steering class\n\n");
      }

      int nameLen = -1;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (nameLen<(int)strlen(histoName[iTask][i])) nameLen = strlen(histoName[iTask][i]);
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (!strcmp(histoArray[iTask][i],"1")) {
            sprintf(format,"   %%s*      f%%s;%%%ds // %%s\n",nameLen-strlen(histoName[iTask][i])+1);
            sprintf(buffer+strlen(buffer),format,histoType[iTask][i],histoName[iTask][i],"",histoTitle[iTask][i]);
         }
         else {
            sprintf(format,"   TObjArray* f%%ss;%%%ds // %%s\n",nameLen-strlen(histoName[iTask][i]));
            sprintf(buffer+strlen(buffer),format,histoName[iTask][i],"",histoTitle[iTask][i]);
         }
         sprintf(format,"   bool       f%%sAccumulation;%%%ds // Accumulation Flag for the %%s\n",nameLen-strlen(histoName[iTask][i]));
         sprintf(buffer+strlen(buffer),format,histoName[iTask][i],"",histoName[iTask][i]);
      }

// Methods
//---------

      sprintf(buffer+strlen(buffer),"public:\n");
      // Constructor and Event Methods
      sprintf(buffer+strlen(buffer),"   // Constructor\n");
      sprintf(buffer+strlen(buffer),"   %sT%s(const char *name,const char *title,%sAnalyzer *analyzer):ROMETask(name,title,analyzer)\n",shortCut,taskName[iTask],shortCut);
      sprintf(buffer+strlen(buffer),"   { fAnalyzer = analyzer; strcpy(fEventID,\"%s\"); fVersion = %s;",taskEventID[iTask],version[iTask]);
      if (numOfHistos[iTask]>0) 
         sprintf(buffer+strlen(buffer)," fHasHistograms = true;");
      else
         sprintf(buffer+strlen(buffer)," fHasHistograms = false;");
      for (i=0;i<numOfHistos[iTask];i++) {
         sprintf(buffer+strlen(buffer)," f%sAccumulation = true;",histoName[iTask][i]);
      }
      if (numOfTaskSteering[iTask]>0) {
         sprintf(buffer+strlen(buffer)," fSteering = new Steering();");
      }
      sprintf(buffer+strlen(buffer)," };\n");
      sprintf(buffer+strlen(buffer),"   // Event Methods\n");
      sprintf(buffer+strlen(buffer),"   virtual void Init();\n");
      sprintf(buffer+strlen(buffer),"   virtual void BeginOfRun();\n");
      sprintf(buffer+strlen(buffer),"   virtual void Event();\n");
      sprintf(buffer+strlen(buffer),"   virtual void EndOfRun();\n");
      sprintf(buffer+strlen(buffer),"   virtual void Terminate();\n\n");
      // Histo Methods
      sprintf(buffer+strlen(buffer),"   // Histo Methods\n");
      sprintf(buffer+strlen(buffer),"   virtual void BookHisto();\n");
      sprintf(buffer+strlen(buffer),"   virtual void ResetHisto();\n\n");
      // User Methods
      sprintf(buffer+strlen(buffer),"   // User Methods\n");
      if (numOfTaskSteering[iTask]>0) {
         sprintf(buffer+strlen(buffer),"   Steering* GetSteeringParameters() { return fSteering; };\n");
         sprintf(buffer+strlen(buffer),"   Steering* GetSP() { return fSteering; };\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (!strcmp(histoArray[iTask][i],"1")) {
            if (histoType[iTask][i][2]==49) {
					sprintf(buffer+strlen(buffer),"   void Fill%s(double x,double weight=1) { f%s->Fill(x,weight); };\n",histoName[iTask][i],histoName[iTask][i]);
            }
            else if (histoType[iTask][i][2]==50) {
               sprintf(buffer+strlen(buffer),"   void Fill%s(double x,double y,double weight=1) { f%s->Fill(x,y,weight); };\n",histoName[iTask][i],histoName[iTask][i]);
            }
            else if (histoType[iTask][i][2]==51) {
               sprintf(buffer+strlen(buffer),"   void Fill%s(double x,double y,double z,double weight=1) { f%s->Fill(x,y,z,weight); };\n",histoName[iTask][i],histoName[iTask][i]);
            }
				sprintf(buffer+strlen(buffer),"   void Draw%s() { f%s->Draw(); };\n",histoName[iTask][i],histoName[iTask][i]);
            sprintf(buffer+strlen(buffer),"   %s* Get%sHandle() { return f%s; };\n",histoType[iTask][i],histoName[iTask][i],histoName[iTask][i]);
         }
         else {
            if (histoType[iTask][i][2]==49) {
               sprintf(buffer+strlen(buffer),"   void Fill%sAt(int index,double x,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,weight); };\n",histoName[iTask][i],histoType[iTask][i],histoName[iTask][i]);
            }
            else if (histoType[iTask][i][2]==50) {
               sprintf(buffer+strlen(buffer),"   void Fill%sAt(int index,double x,double y,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,weight); };\n",histoName[iTask][i],histoType[iTask][i],histoName[iTask][i]);
            }
            else if (histoType[iTask][i][2]==51) {
               sprintf(buffer+strlen(buffer),"   void Fill%sAt(int index,double x,double y,double z,double weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,z,weight); };\n",histoName[iTask][i],histoType[iTask][i],histoName[iTask][i]);
            }
				sprintf(buffer+strlen(buffer),"   void Draw%sAt(int index) { ((%s*)f%ss->At(index))->Draw(); };\n",histoName[iTask][i],histoType[iTask][i],histoName[iTask][i]);
            sprintf(buffer+strlen(buffer),"   %s* Get%sHandleAt(int index) { return (%s*)f%ss->At(index); };\n",histoType[iTask][i],histoName[iTask][i],histoType[iTask][i],histoName[iTask][i]);
         }
         sprintf(buffer+strlen(buffer),"   Bool_t is%sAccumulation() { return f%sAccumulation; };\n",histoName[iTask][i],histoName[iTask][i]);
         sprintf(buffer+strlen(buffer),"   void Set%sAccumulation(Bool_t flag) { f%sAccumulation = flag; };\n",histoName[iTask][i],histoName[iTask][i]);
      }

// Footer
//--------
      sprintf(buffer+strlen(buffer),"\n   ClassDef(%sT%s,%s)\n",shortCut,taskName[iTask],version[iTask]);
      sprintf(buffer+strlen(buffer),"};\n\n");

      // Histo Inline Methods
      sprintf(buffer+strlen(buffer),"inline void %sT%s::BookHisto() {\n",shortCut,taskName[iTask]);
      bool array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (strcmp(histoArray[iTask][i],"1")) array = true;
      }
      if (array) {
         sprintf(buffer+strlen(buffer),"   int j;\n");
         sprintf(buffer+strlen(buffer),"   char name[80],title[80];\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (!strcmp(histoArray[iTask][i],"1")) {
            if (histoType[iTask][i][2]==49) {
               sprintf(buffer+strlen(buffer),"   f%s = new %s(\"%s\",\"%s\",%s,%s,%s);\n",histoName[iTask][i],histoType[iTask][i],histoName[iTask][i],histoTitle[iTask][i],histoXBin[iTask][i],histoXMin[iTask][i],histoXMax[iTask][i]);
            }
            if (histoType[iTask][i][2]==50) {
               sprintf(buffer+strlen(buffer),"   f%s = new %s(\"%s\",\"%s\",%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i],histoType[iTask][i],histoName[iTask][i],histoTitle[iTask][i],histoXBin[iTask][i],histoXMin[iTask][i],histoXMax[iTask][i],histoYBin[iTask][i],histoYMin[iTask][i],histoYMax[iTask][i]);
            }
            if (histoType[iTask][i][2]==51) {
               sprintf(buffer+strlen(buffer),"   f%s = new %s(\"%s\",\"%s\",%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i],histoType[iTask][i],histoName[iTask][i],histoTitle[iTask][i],histoXBin[iTask][i],histoXMin[iTask][i],histoXMax[iTask][i],histoYBin[iTask][i],histoYMin[iTask][i],histoYMax[iTask][i],histoZBin[iTask][i],histoZMin[iTask][i],histoZMax[iTask][i]);
            }
            sprintf(buffer+strlen(buffer),"   GetHistoFolder()->Add(f%s);\n",histoName[iTask][i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   %s *hist%d;\n",histoType[iTask][i],i);
            sprintf(buffer+strlen(buffer),"   f%ss = new TObjArray(%s);\n",histoName[iTask][i],histoArray[iTask][i]);
            sprintf(buffer+strlen(buffer),"   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",\"%s\");\n",histoFolderName[iTask][i],histoFolderName[iTask][i],histoFolderTitle[iTask][i]);
            sprintf(buffer+strlen(buffer),"   for (j=0;j<%s;j++) {\n",histoArray[iTask][i]);
            sprintf(buffer+strlen(buffer),"      sprintf(name,\"%s_%%03d\",j);\n",histoName[iTask][i]);
            sprintf(buffer+strlen(buffer),"      sprintf(title,\"%s %%03d\",j);\n",histoTitle[iTask][i]);
            if (histoType[iTask][i][2]==49) {
               sprintf(buffer+strlen(buffer),"      hist%d = new %s(name,title,%s,%s,%s);\n",i,histoType[iTask][i],histoXBin[iTask][i],histoXMin[iTask][i],histoXMax[iTask][i]);
            }
            if (histoType[iTask][i][2]==50) {
               sprintf(buffer+strlen(buffer),"      hist%d = new %s(name,title,%s,%s,%s,%s,%s,%s);\n",i,histoType[iTask][i],histoXBin[iTask][i],histoXMin[iTask][i],histoXMax[iTask][i],histoYBin[iTask][i],histoYMin[iTask][i],histoYMax[iTask][i]);
            }
            if (histoType[iTask][i][2]==51) {
               sprintf(buffer+strlen(buffer),"      hist%d = new %s(name,title,%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",i,histoType[iTask][i],histoXBin[iTask][i],histoXMin[iTask][i],histoXMax[iTask][i],histoYBin[iTask][i],histoYMin[iTask][i],histoYMax[iTask][i],histoZBin[iTask][i],histoZMin[iTask][i],histoZMax[iTask][i]);
            }
            sprintf(buffer+strlen(buffer),"      f%ss->Add(hist%d);\n",histoName[iTask][i],i);
            sprintf(buffer+strlen(buffer),"      %sFolder->Add(f%ss->At(j));\n   }\n",histoFolderName[iTask][i],histoName[iTask][i]);
         }
      }
      sprintf(buffer+strlen(buffer),"}\n\n");

      sprintf(buffer+strlen(buffer),"inline void %sT%s::ResetHisto() {\n",shortCut,taskName[iTask]);
      array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (strcmp(histoArray[iTask][i],"1")) array = true;
      }
      if (array) {
         sprintf(buffer+strlen(buffer),"   int j;\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (!strcmp(histoArray[iTask][i],"1")) {
            sprintf(buffer+strlen(buffer),"   if (!is%sAccumulation()) f%s->Reset();\n",histoName[iTask][i],histoName[iTask][i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   if (!is%sAccumulation()) {\n",histoName[iTask][i]);
            sprintf(buffer+strlen(buffer),"       for (j=0;j<%s;j++) ((%s*)f%ss->At(j))->Reset();\n",histoArray[iTask][i],histoType[iTask][i],histoName[iTask][i]);
            sprintf(buffer+strlen(buffer),"   }\n");
         }
      }
      sprintf(buffer+strlen(buffer),"}\n\n");

      sprintf(buffer+strlen(buffer),"#endif   // %sT%s_H\n",shortCut,taskName[iTask]);

// Close h-File
//--------------
      sprintf(hFile,"%s/include/tasks/%sT%s.h",outDir,shortCut,taskName[iTask]);
      fileHandle = open(hFile,O_RDONLY);
      int nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      bool identical = true;
      for (i=0;i<nb||i<(int)strlen(buffer);i++) {
         if (buffer[i] != fileBuffer[i]) {
            identical = false;
         }
      }
      if (!identical) {
         fileHandle = open(hFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(hFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << hFile << endl;
         nb = write(fileHandle,&buffer, strlen(buffer));
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
         numOfBranch[numOfTree] = 0;
         // tree name
         if (!xml->GetAttribute("Name",treeName[numOfTree])) {
            cout << "Tree without a name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // tree title
         strcpy(treeTitle[numOfTree],"");
         xml->GetAttribute("Title",treeTitle[numOfTree]);
         // output
         if (makeOutput) cout << "   " << treeName[numOfTree] << endl;

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp((const char*)name,"Branch")) {
               // branch name
               if (!xml->GetAttribute("Name",branchName[numOfTree][numOfBranch[numOfTree]])) {
                  cout << "Branch without a name in Tree '" << treeName[numOfTree] << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               // branch folder
               if (!xml->GetAttribute("Folder",branchFolder[numOfTree][numOfBranch[numOfTree]])) {
                  cout << "Branch '" << branchName[numOfTree][numOfBranch[numOfTree]] << "' of Tree '" << treeName[numOfTree] << "' has no Folder specified!" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               bool found = false;
               for (i=0;i<numOfFolder;i++) {
                  if (!strcmp(folderName[i],branchFolder[numOfTree][numOfBranch[numOfTree]]))
                     found = true;
               }
               if (!found) {
                  cout << "Folder of Branch '" << branchName[numOfTree][numOfBranch[numOfTree]] << "' of Tree '" << treeName[numOfTree] << "' not existing !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               numOfBranch[numOfTree]++;
            }
            if (type == 15 && !strcmp((const char*)name,"Tree")) {
               if (numOfBranch[numOfTree] == 0) {
                  cout << "Tree '" << treeName[numOfTree] << "' has no Branch !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               for (i=0;i<numOfBranch[numOfTree];i++) {
                  for (j=i+1;j<numOfBranch[numOfTree];j++) {
                     if (!strcmp(branchName[numOfTree][i],branchName[numOfTree][j])) {
                        cout << "\nTree '" << treeName[numOfTree] << "' has two branches with the name '" << branchName[numOfTree] << "' !" << endl;
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
               if (!strcmp(treeName[i],treeName[j])) {
                  cout << "\nTree '" << treeName[i] << "' is defined twice !" << endl;
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

   if (makeOutput) cout << "\n\nBanks:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp("EventHeader",(const char*)name)) {
         // folder
         if (!xml->GetAttribute("Folder",bankHeaderFolder)) {
            cout << "Midas event header has no folder !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // EventID
         strcpy(bankHeaderEventID,"");
         xml->GetAttribute("EventID",bankHeaderEventID);
         // TriggerMask
         strcpy(bankHeaderTriggerMask,"");
         xml->GetAttribute("TriggerMask",bankHeaderTriggerMask);
         // SerialNumber
         strcpy(bankHeaderSerialNumber,"");
         xml->GetAttribute("SerialNumber",bankHeaderSerialNumber);
         // TimeStamp
         strcpy(bankHeaderTimeStamp,"");
         xml->GetAttribute("TimeStamp",bankHeaderTimeStamp);
         // Tests
         int iFold = -1;
         for (i=0;i<numOfFolder;i++) {
            if (!strcmp(folderName[i],bankHeaderFolder)) {
               iFold = i;
               break;
            }
         }
         if (iFold==-1) {
            cout << "Midas event header : folder '" << bankHeaderFolder << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (strcmp(folderArray[iFold],"1")) {
            cout << "Midas event header : folder '" << bankHeaderFolder << "' is an array !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         bool foundID = false;
         bool foundMask = false;
         bool foundNum = false;
         bool foundTime = false;
         if (!strcmp(bankHeaderEventID,"")) 
            foundID = true;
         if (!strcmp(bankHeaderTriggerMask,"")) 
            foundMask = true;
         if (!strcmp(bankHeaderSerialNumber,"")) 
            foundNum = true;
         if (!strcmp(bankHeaderTimeStamp,"")) 
            foundTime = true;
         for (i=0;i<numOfValue[iFold];i++) {
            if (!strcmp(valueName[iFold][i],bankHeaderEventID)) {
               foundID = true;
            }
            if (!strcmp(valueName[iFold][i],bankHeaderTriggerMask)) {
               foundMask = true;
            }
            if (!strcmp(valueName[iFold][i],bankHeaderSerialNumber)) {
               foundNum = true;
            }
            if (!strcmp(valueName[iFold][i],bankHeaderTimeStamp)) {
               foundTime = true;
            }
         }
         if (!foundID) {
            cout << "Midas event header : event id field '" << bankHeaderEventID << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundMask) {
            cout << "Midas event header : trigger mask field '" << bankHeaderTriggerMask << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundNum) {
            cout << "Midas event header : serial number field '" << bankHeaderSerialNumber << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         if (!foundTime) {
            cout << "Midas event header : time stamp field '" << bankHeaderTimeStamp << "' does not exist !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
      }
      else if (type == 1) {
         numOfBank++;
         numOfStructFields[numOfBank] = 0;
         // bank name
         strcpy(bankName[numOfBank],(const char*)name);
         // bank type
         if (!xml->GetAttribute("Type",bankType[numOfBank])) {
            cout << "Bank '" << bankName[numOfBank] << "' has no type !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // bank structure name
         sprintf(bankStructName[numOfBank],"%sStruct",bankName[numOfBank]);
         xml->GetAttribute("StructName",bankStructName[numOfBank]);
         // output
         if (makeOutput) cout << "   " << bankName[numOfBank] << endl;

         if (!strcmp(bankType[numOfBank],"structure")||!strcmp(bankType[numOfBank],"struct")) {
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               if (type == 1) {
                  // field name
                  strcpy(structFieldName[numOfBank][numOfStructFields[numOfBank]],(const char*)name);
                  // field type
                  if (!xml->GetAttribute("Type",structFieldType[numOfBank][numOfStructFields[numOfBank]])) {
                     cout << "Structure field '" << structFieldName[numOfBank][numOfStructFields[numOfBank]] << "' of Bank '" << bankName[numOfBank] << "' has no type !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  // field size
                  strcpy(structFieldSize[numOfBank][numOfStructFields[numOfBank]],"");
                  xml->GetAttribute("PackedSize",structFieldSize[numOfBank][numOfStructFields[numOfBank]]);
                  // output
                  if (makeOutput) cout << "      " << structFieldName[numOfBank][numOfStructFields[numOfBank]] << endl;
                  numOfStructFields[numOfBank]++;
               }
               if (type == 15 && !strcmp((const char*)name,bankName[numOfBank])) {
                  for (i=0;i<numOfStructFields[numOfBank];i++) {
                     for (j=i+1;j<numOfStructFields[numOfBank];j++) {
                        if (!strcmp(structFieldName[numOfBank][i],structFieldName[numOfBank][j])) {
                           cout << "\nStructure of bank '" << bankName[numOfBank] << "' has two fields with the name '" << structFieldName[numOfBank] << "' !" << endl;
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
               if (!strcmp(bankName[i],bankName[j])) {
                  cout << "\nMidas bank '" << bankName[i] << "' is defined twice !" << endl;
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
   char tmp[100];
   int i,type,empty,depth=0,index[20];
   index[0] = 0;
   numOfSteerFields[0] = 0;
   strcpy(steerName[0],"GeneralSteering");
   strcpy(steerParent[0],"");
   steerDepth[0] = 0;

   if (makeOutput) cout << "\n\nSteering:" << endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      empty = xml->isEmpty();
      if (type == 1 && !empty) {
         numOfSteering++;
         depth++;
         index[depth] = numOfSteering;
         numOfSteerFields[index[depth]] = 0;
         // bank name
         strcpy(steerName[index[depth]],(const char*)name);
         // parent name
         strcpy(steerParent[index[depth]],steerName[index[depth-1]]);
         // depth
         steerDepth[index[depth]] = depth;
         // output
         if (makeOutput) for (i=0;i<depth;i++) cout << "   ";
         if (makeOutput) cout << steerName[index[depth]] << endl;
      }
      if (type == 1 && empty) {
         // field name
         strcpy(steerFieldName[index[depth]][numOfSteerFields[index[depth]]],(const char*)name);
         // field type
         if (!xml->GetAttribute("Type",steerFieldType[index[depth]][numOfSteerFields[index[depth]]])) {
            cout << "Steering Parameter " << steerFieldName[index[depth]][numOfSteerFields[index[depth]]] << " has no type !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // field initialisation
         if (!strcmp(steerFieldType[index[depth]][numOfSteerFields[index[depth]]],"TString"))
            strcpy(steerFieldInit[index[depth]][numOfSteerFields[index[depth]]],"' '");
         else
            strcpy(steerFieldInit[index[depth]][numOfSteerFields[index[depth]]],"0");
         xml->GetAttribute("Initialisation",steerFieldInit[index[depth]][numOfSteerFields[index[depth]]]);
         xml->GetAttribute("Init",steerFieldInit[index[depth]][numOfSteerFields[index[depth]]]);
         // field comment
         strcpy(steerFieldComment[index[depth]][numOfSteerFields[index[depth]]],"");
         xml->GetAttribute("Comment",steerFieldComment[index[depth]][numOfSteerFields[index[depth]]]);
         if (steerFieldComment[index[depth]][numOfSteerFields[index[depth]]][0]!='/') {
            strcpy(tmp,steerFieldComment[index[depth]][numOfSteerFields[index[depth]]]);
            sprintf(steerFieldComment[index[depth]][numOfSteerFields[index[depth]]],"// %s",tmp);
         }
         // output
         if (makeOutput) for (i=0;i<depth+1;i++) cout << "   ";
         if (makeOutput) cout << steerFieldName[index[depth]][numOfSteerFields[index[depth]]] << endl;
         numOfSteerFields[index[depth]]++;
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

   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   sprintf(hFile,"%s/include/framework/%sGeneralSteering.h",outDir,shortCut);

   if (numOfSteering==0) {
      remove(hFile);
      return true;
   }

   if (makeOutput) cout << "\n   Output Files:" << endl;

// Description
//-------------
   sprintf(buffer,"//// Author: %s\n",mainAuthor);
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Contains the general Steering Parameters                                   //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
   sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

// Header Files
//--------------

   sprintf(buffer+strlen(buffer),"#ifndef %sGeneralSteering_H\n",shortCut);
   sprintf(buffer+strlen(buffer),"#define %sGeneralSteering_H\n\n",shortCut);

   WriteSteeringClass(buffer,0);

   sprintf(buffer+strlen(buffer),"#endif   // %sGeneralSteering_H\n",shortCut);

   int fileHandle = open(hFile,O_RDONLY);
   int nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<(int)strlen(buffer);i++) {
      if (buffer[i] != fileBuffer[i]) {
         identical = false;
      }
   }
   if (!identical) {
      fileHandle = open(hFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(hFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << hFile << endl;
      nb = write(fileHandle,&buffer, strlen(buffer));
      close(fileHandle);
   }

   return true;
}
bool ROMEBuilder::WriteSteeringClass(char *buffer,int numOfSteer) {
   char format[100];
   char sc[20];
   char blank[20];
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i=0;i<numOfSteerFields[numOfSteer];i++) {
      if (typeLen<(int)strlen(steerFieldType[numOfSteer][i])) typeLen = strlen(steerFieldType[numOfSteer][i]);
      if (nameLen<(int)strlen(steerFieldName[numOfSteer][i])) nameLen = strlen(steerFieldName[numOfSteer][i]);
   }
   for (i=0;i<numOfSteering;i++) {
      if (!strcmp(steerParent[i],steerName[numOfSteer])) {
         if (typeLen<(int)strlen(steerName[i])+1) typeLen = strlen(steerName[i])+1;
         if (nameLen<(int)strlen(steerName[i])) nameLen = strlen(steerName[i]);
      }
   }
   if (numOfSteer==0)
      strcpy(sc,shortCut);
   else
      strcpy(sc,"");

   sprintf(blank,"");   
   for (i=0;i<steerDepth[numOfSteer];i++) {
      sprintf(blank+strlen(blank),"   ");   
   }

   sprintf(buffer+strlen(buffer),"\n%sclass %s%s\n",blank,sc,steerName[numOfSteer]);
   sprintf(buffer+strlen(buffer),"%s{\n",blank);

   sprintf(buffer+strlen(buffer),"%sprivate:\n",blank);

   for (i=0;i<numOfSteering;i++) {
      if (!strcmp(steerParent[i],steerName[numOfSteer])) {
         WriteSteeringClass(buffer,i);
      }
   }

   sprintf(buffer+strlen(buffer),"%sprotected:\n",blank);

   // Fields
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      sprintf(format,"%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-strlen(steerFieldName[numOfSteer][j]));
      sprintf(buffer+strlen(buffer),format,blank,steerFieldType[numOfSteer][j],steerFieldName[numOfSteer][j],"",steerFieldComment[numOfSteer][j]);
   }
   for (i=0;i<numOfSteering;i++) {
      if (!strcmp(steerParent[i],steerName[numOfSteer])) {
         sprintf(format,"%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-strlen(steerName[i]));
         sprintf(buffer+strlen(buffer),format,blank,steerName[i],steerName[i],"",steerName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n%spublic:\n",blank);
   // Constructor
   sprintf(buffer+strlen(buffer),"%s   %s%s() { ",blank,sc,steerName[numOfSteer]);
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      sprintf(buffer+strlen(buffer),"f%s = %s; ",steerFieldName[numOfSteer][j],steerFieldInit[numOfSteer][j]);
   }
   for (i=0;i<numOfSteering;i++) {
      if (!strcmp(steerParent[i],steerName[numOfSteer])) {
         sprintf(buffer+strlen(buffer),"f%s = new %s(); ",steerName[i],steerName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"};\n");
   // Getters
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      sprintf(format,"%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen-strlen(steerFieldName[numOfSteer][j]));
      sprintf(buffer+strlen(buffer),format,blank,steerFieldType[numOfSteer][j],steerFieldName[numOfSteer][j],"",steerFieldName[numOfSteer][j]);
   }
   for (i=0;i<numOfSteering;i++) {
      if (!strcmp(steerParent[i],steerName[numOfSteer])) {
         sprintf(format,"%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen-strlen(steerName[i]));
         sprintf(buffer+strlen(buffer),format,blank,steerName[i],steerName[i],"",steerName[i]);
      }
   }
   // Setters
   sprintf(buffer+strlen(buffer),"\n");
   for (j=0;j<numOfSteerFields[numOfSteer];j++) {
      sprintf(format,"%%s   void Set%%-%ds(%%-%ds %%s)%%%ds { f%%s = %%s; };\n",nameLen,typeLen,nameLen-strlen(steerFieldName[numOfSteer][j]));
      sprintf(buffer+strlen(buffer),format,blank,steerFieldName[numOfSteer][j],steerFieldType[numOfSteer][j],steerFieldName[numOfSteer][j],"",steerFieldName[numOfSteer][j],steerFieldName[numOfSteer][j]);
   }

   // Footer
   sprintf(buffer+strlen(buffer),"%s};\n\n",blank);

   return true;
}
void ROMEBuilder::WriteSteerConfigWrite(char *buffer,int numOfSteer) {
   int i,j;
   char tmp[300];
   char getter[300];
   if (numOfSteer==0)
      sprintf(buffer+strlen(buffer),"   xml->StartElement(\"GeneralSteeringParameters\");\n");
   else
      sprintf(buffer+strlen(buffer),"   xml->StartElement(\"%s\");\n",steerName[numOfSteer]);
   for (i=0;i<numOfSteerFields[numOfSteer];i++) {
      sprintf(getter,"->Get%s()",steerFieldName[numOfSteer][i]);
      int ind = numOfSteer;
      while (strcmp(steerParent[ind],"")) {
         for (j=0;j<numOfSteering;j++) {
            if (!strcmp(steerParent[ind],steerName[j])) {
               strcpy(tmp,getter);
               sprintf(getter,"->Get%s()%s",steerName[ind],tmp);
               ind = j;
               break;
            }
         }
      }
      strcpy(tmp,getter);
      sprintf(getter,"this->GetGSP()%s",tmp);
      GetFormat(tmp,steerFieldType[numOfSteer][i]);
      sprintf(buffer+strlen(buffer),"   sprintf(value,\"%s\",%s);\n",tmp,getter);
      sprintf(buffer+strlen(buffer),"   xml->WriteElement(\"%s\", value);\n",steerFieldName[numOfSteer][i],tmp,getter);
   }
   for (i=0;i<numOfSteering;i++) {
      if (!strcmp(steerParent[i],steerName[numOfSteer])) {
         WriteSteerConfigWrite(buffer,i);
      }
   }
   sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
}

void ROMEBuilder::WriteSteerConfigRead(char *buffer,int numSteer) {
   char tmp[300];
   char value[300];
   char path[300];
   char setter[300];
   int i,j;
   char blank[100];
   strcpy(blank,"");

   strcpy(path,"");
   int ind = numSteer;
   while (strcmp(steerParent[ind],"")) {
      for (j=0;j<numOfSteering;j++) {
         if (!strcmp(steerParent[ind],steerName[j])) {
            strcpy(tmp,path);
            sprintf(path,"->Get%s()%s",steerName[ind],tmp);
            ind = j;
            strcat(blank,"      ");
            break;
         }
      }
   }
   if (numSteer==0)
      sprintf(buffer+strlen(buffer),"%s      if (type == 1 && !strcmp((const char*)name,\"%sParameters\")) {\n",blank,steerName[numSteer]);
   else
      sprintf(buffer+strlen(buffer),"%s      if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank,steerName[numSteer]);

   sprintf(buffer+strlen(buffer),"%s         while (xml->NextLine()) {\n",blank);
   sprintf(buffer+strlen(buffer),"%s            type = xml->GetType();\n",blank);
   sprintf(buffer+strlen(buffer),"%s            name = xml->GetName();\n",blank);
   for (i=0;i<numOfSteerFields[numSteer];i++) {
      setValue(value,"","tmp",steerFieldType[numSteer][i],1);
      sprintf(setter,"this->GetGSP()%s->Set%s((%s)%s)",path,steerFieldName[numSteer][i],steerFieldType[numSteer][i],value);
      sprintf(buffer+strlen(buffer),"%s            if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",blank,steerFieldName[numSteer][i]);
      sprintf(buffer+strlen(buffer),"%s               if (xml->GetValue(tmp)) \n",blank);
      sprintf(buffer+strlen(buffer),"%s                  %s;\n",blank,setter);
      sprintf(buffer+strlen(buffer),"%s            }\n",blank);
   }
   for (i=0;i<numOfSteering;i++) {
      if (!strcmp(steerParent[i],steerName[numSteer])) {
         WriteSteerConfigRead(buffer,i);
      }
   }
   if (numSteer==0)
      sprintf(buffer+strlen(buffer),"%s            if (type == 15 && !strcmp((const char*)name,\"%sParameters\"))\n",blank,steerName[numSteer]);
   else
      sprintf(buffer+strlen(buffer),"%s            if (type == 15 && !strcmp((const char*)name,\"%s\"))\n",blank,steerName[numSteer]);
   sprintf(buffer+strlen(buffer),"%s               break;\n",blank);
   sprintf(buffer+strlen(buffer),"%s         }\n",blank);
   sprintf(buffer+strlen(buffer),"%s      }\n",blank);
}
bool ROMEBuilder::WriteAnalyzerCpp() {
   int i;

   char cppFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb,lenTot,j,ll;
   char *pos;
   int fileHandle;

   char classDescription[500];
   sprintf(classDescription,"Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut,mainProgName);

   char tmp[1000];
   char format[100];
   int nameLen = -1;
   int typeLen = 12;
   int scl = strlen(shortCut);
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)strlen(folderName[i])+scl) typeLen = strlen(folderName[i])+scl;
      if (nameLen<(int)strlen(folderName[i])) nameLen = strlen(folderName[i]);
   }

// Description
//-------------
   sprintf(buffer,"//// Author: %s\n",mainAuthor);
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   ll = 74-strlen(shortCut);
   sprintf(format,"// %%s%%-%d.%ds //\n",ll,ll);
   sprintf(buffer+strlen(buffer),format,shortCut,"Analyzer");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   pos = classDescription;
   lenTot = strlen(classDescription);
   while (pos-classDescription < lenTot) {
      if (lenTot+(classDescription-pos)<74) i=75;
      else for (i=74;pos[i]!=32&&i>0;i--) {}
      if (i<=0) i=75;
      pos[i] = 0;
      sprintf(buffer+strlen(buffer),"// %-74.74s   \n",pos);
      pos = pos+i+1;
   }
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
   sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

// Header Files
//--------------

   sprintf(buffer+strlen(buffer),"#if defined( _MSC_VER )\n");
   sprintf(buffer+strlen(buffer),"#include <direct.h>\n");
   sprintf(buffer+strlen(buffer),"#endif\n");

   sprintf(buffer+strlen(buffer),"#include <sys/stat.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TH1.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TROOT.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TObjArray.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMEStatic.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMESQL.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMEXML.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMEEventLoop.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROME.h>\n");
   sprintf(buffer+strlen(buffer),"#include <include/framework/%sAnalyzer.h>\n",shortCut);
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer+strlen(buffer),"#include <include/tasks/%sT%s.h>\n",shortCut,taskName[i]);
   }
   sprintf(buffer+strlen(buffer),"#include <Riostream.h>\n");

   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"#if defined( _MSC_VER )\n");
   sprintf(buffer+strlen(buffer),"#include <windows.h>\n");
   sprintf(buffer+strlen(buffer),"void CreateSplash(unsigned long time,char*,char*,TString*,int);\n");
   sprintf(buffer+strlen(buffer),"DWORD WINAPI SplashThread ( LPVOID lpvoid) {\n");
   bool same = false;
   int numAuthors = 1;
   char (*authors)[50] = new char[numOfTask+1][50];
   strcpy(authors[0],mainAuthor);
   for (i=0;i<numOfTask;i++) {
      same = false;
      for (j=0;j<i+1;j++) {
         if (!strcmp(authors[j],author[i])) {
            same = true;
            break;
         }
      }
      if (!same) {
         strcpy(authors[numAuthors],author[i]);
         numAuthors++;
      }
   }
   sprintf(buffer+strlen(buffer),"   TString *authors = new TString[%d];\n",numAuthors);
   for (i=0;i<numAuthors;i++) {
      sprintf(buffer+strlen(buffer),"   authors[%d] = \"%s\";\n",i,authors[i]);
   }
   sprintf(buffer+strlen(buffer),"   CreateSplash(3,\"%s%s\",\"%s\",authors,%d);\n",shortCut,mainProgName,romeVersion,numAuthors);
   sprintf(buffer+strlen(buffer),"   return 0;\n");
   sprintf(buffer+strlen(buffer),"}\n");
   sprintf(buffer+strlen(buffer),"#endif\n");
   sprintf(buffer+strlen(buffer),"\n");


   // Constructor
   char parentt[100];
   sprintf(buffer+strlen(buffer),"%sAnalyzer::%sAnalyzer() {\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"// Folder, Task, Tree and Data Base initialisation\n");
   sprintf(buffer+strlen(buffer),"\n");
   // Steering 
   if (numOfSteering!=0) {
      sprintf(buffer+strlen(buffer),"   fGeneralSteeringParameters = new %sGeneralSteering();\n",shortCut);
   }
   // Folder 
   sprintf(buffer+strlen(buffer),"   // Folder initialisation\n");
   sprintf(buffer+strlen(buffer),"   fMainFolder = gROOT->GetRootFolder()->AddFolder(\"%s\",\"Root Folder of %s%s\");\n",shortCut,shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"   gROOT->GetListOfBrowsables()->Add(fMainFolder,\"%s\");\n\n",shortCut);

   for (i=0;i<numOfFolder;i++) {
      if (!strcmp(parentFolderName[i],"GetMainFolder()")) strcpy(parentt,parentFolderName[i]);
      else sprintf(parentt,"%sFolder",parentFolderName[i]);
      sprintf(format,"   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-strlen(folderName[i]));
      sprintf(buffer+strlen(buffer),format,folderName[i],"",parentt,folderName[i],folderTitle[i]);
   }
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (strcmp(folderArray[i],"1")) {
            sprintf(buffer+strlen(buffer),"\n   f%sObjects = new TClonesArray(\"%s%s\");\n",folderName[i],shortCut,folderName[i]);
            sprintf(buffer+strlen(buffer),"   %sFolder->Add(f%sObjects);\n",folderName[i],folderName[i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"\n   f%sObject = new %s%s();\n",folderName[i],shortCut,folderName[i]);
            sprintf(buffer+strlen(buffer),"   %sFolder->Add(f%sObject);\n",folderName[i],folderName[i]);
         }
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

   // IO
   sprintf(buffer+strlen(buffer),"   // IO Class initialisation\n");
   sprintf(buffer+strlen(buffer),"   fIO = new %sIO(",shortCut);
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (strcmp(folderArray[i],"1"))
            sprintf(buffer+strlen(buffer),"f%sObjects,",folderName[i]);
         else
            sprintf(buffer+strlen(buffer),"f%sObject,",folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer)-1,");\n");
   sprintf(buffer+strlen(buffer),"\n");


   // Task
   sprintf(buffer+strlen(buffer),"   // Task initialisation\n");
   sprintf(buffer+strlen(buffer),"   fMainTask = new ROMEEventLoop(\"mainTask\",\"Main Task of %s%s\",this);\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"   fMainFolder->Add(fMainTask);\n");
   sprintf(buffer+strlen(buffer),"   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   int taskLen=0;
   for (i=0;i<numOfTask;i++) {
      if (taskLen<(int)strlen(taskName[i])) taskLen = strlen(taskName[i]);
   }
   for (i=0;i<numOfTask;i++) {
      sprintf(format,"   %%sTask%%%ds = new %%sT%%s(\"%%s\",\"%%s\",this);\n",taskLen-strlen(taskName[i]));
      sprintf(buffer+strlen(buffer),format,taskName[i],"",shortCut,taskName[i],taskName[i],"");
      sprintf(buffer+strlen(buffer),"   ((%sT%s*)%sTask)->SetActive(false);\n",shortCut,taskName[i],taskName[i]);
   }
   for (i=0;i<numOfTask;i++) {
      if (!strcmp(parentTaskName[i],"GetMainTask()")) strcpy(parentt,parentTaskName[i]);
      else sprintf(parentt,"%sTask",parentTaskName[i]);
      sprintf(buffer+strlen(buffer),"   %s->Add(%sTask);\n",parentt,taskName[i]);
   }
   sprintf(buffer+strlen(buffer),"\n");

   // RunTable
   sprintf(buffer+strlen(buffer),"   // RunTable\n");
   sprintf(buffer+strlen(buffer),"   fRunTable = new TList();\n");

   sprintf(buffer+strlen(buffer),"};\n");

   sprintf(buffer+strlen(buffer),"%sAnalyzer::~%sAnalyzer() {\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"   delete fIO;\n");
   sprintf(buffer+strlen(buffer),"};\n\n");


   // Initialize Folders
   sprintf(buffer+strlen(buffer),"void %sAnalyzer::InitFolders() {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (!strcmp(folderArray[i],"1")) {
            sprintf(buffer+strlen(buffer),"   new(f%sObject) %s%s(",folderName[i],shortCut,folderName[i]);
            for (j=0;j<numOfValue[i];j++) {
               sprintf(buffer+strlen(buffer),"%s,",valueInit[i][j]);
            }
            sprintf(buffer+strlen(buffer)-1,");\n");
         }
         else {
            strcpy(tmp,folderArray[i]);
            if (strstr(folderArray[i],"fAnalyzer")==folderArray[i])
               sprintf(tmp,"this%s",folderArray[i]+9);
            sprintf(buffer+strlen(buffer),"   for (i=0;i<%s;i++) {\n",tmp);
            sprintf(buffer+strlen(buffer),"     new((*f%sObjects)[i]) %s%s(",folderName[i],shortCut,folderName[i]);
            for (j=0;j<numOfValue[i];j++) {
               sprintf(buffer+strlen(buffer),"%s,",valueInit[i][j]);
            }
            sprintf(buffer+strlen(buffer)-1,");\n");
            sprintf(buffer+strlen(buffer),"   }\n");
         }
      }
   }
   sprintf(buffer+strlen(buffer),"};\n\n");

   // Configuration File
   sprintf(buffer+strlen(buffer),"\n// Configuration File\n");
   sprintf(buffer+strlen(buffer),"//--------------------\n");
   sprintf(buffer+strlen(buffer),"bool %sAnalyzer::ReadROMEConfigXML(char *configFile) {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   char *cstop;\n");
   sprintf(buffer+strlen(buffer),"   char *name;\n");
   sprintf(buffer+strlen(buffer),"   char value[100];\n");
   sprintf(buffer+strlen(buffer),"   char tmp[100];\n");
   sprintf(buffer+strlen(buffer),"   int type;\n");
   sprintf(buffer+strlen(buffer),"   ROMEXML *xml = new ROMEXML();\n");
   sprintf(buffer+strlen(buffer),"   if (!xml->OpenFileForRead(configFile)) {\n");
   sprintf(buffer+strlen(buffer),"      fprintf(stderr, \"Unable to open %%s\\n\", configFile);\n");
   sprintf(buffer+strlen(buffer),"      return false;\n");
   sprintf(buffer+strlen(buffer),"   }\n");
   sprintf(buffer+strlen(buffer),"   while (xml->NextLine()) {\n");
   sprintf(buffer+strlen(buffer),"      type = xml->GetType();\n");
   sprintf(buffer+strlen(buffer),"      name = xml->GetName();\n");
   // Modes
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"Modes\")) {\n");
   sprintf(buffer+strlen(buffer),"         strcpy(value,\"\");\n");
   sprintf(buffer+strlen(buffer),"         xml->GetAttribute(\"AnalyzingMode\",value);\n");
   sprintf(buffer+strlen(buffer),"         if (!strcmp(value,\"online\"))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetOnline();\n");
   sprintf(buffer+strlen(buffer),"         else\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetOffline();\n");
   sprintf(buffer+strlen(buffer),"         strcpy(value,\"\");\n");
   sprintf(buffer+strlen(buffer),"         xml->GetAttribute(\"InputDataFormat\",value);\n");
   sprintf(buffer+strlen(buffer),"         if (!strcmp(value,\"root\"))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetRoot();\n");
   sprintf(buffer+strlen(buffer),"         else\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetMidas();\n");
   sprintf(buffer+strlen(buffer),"         strcpy(value,\"\");\n");
   sprintf(buffer+strlen(buffer),"         xml->GetAttribute(\"BatchMode\",value);\n");
   sprintf(buffer+strlen(buffer),"         if (!strcmp(value,\"yes\"))\n");
   sprintf(buffer+strlen(buffer),"            fBatchMode = true;\n");
   sprintf(buffer+strlen(buffer),"         else\n");
   sprintf(buffer+strlen(buffer),"            fBatchMode = false;\n");
   sprintf(buffer+strlen(buffer),"         strcpy(value,\"\");\n");
   sprintf(buffer+strlen(buffer),"         xml->GetAttribute(\"ShowSplashScreen\",value);\n");
   sprintf(buffer+strlen(buffer),"         if (!strcmp(value,\"no\"))\n");
   sprintf(buffer+strlen(buffer),"            fSplashScreen = false;\n");
   sprintf(buffer+strlen(buffer),"         else\n");
   sprintf(buffer+strlen(buffer),"            fSplashScreen = true;\n");
   sprintf(buffer+strlen(buffer),"         strcpy(value,\"\");\n");
   sprintf(buffer+strlen(buffer),"         xml->GetAttribute(\"DataBaseMode\",value);\n");
   sprintf(buffer+strlen(buffer),"         if (!strcmp(value,\"sql\")||!strcmp(value,\"SQL\"))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetSQLDataBase();\n");
   sprintf(buffer+strlen(buffer),"         else if (!strcmp(value,\"xml\")||!strcmp(value,\"XML\")) {\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetXMLDataBase();\n");
   sprintf(buffer+strlen(buffer),"         }\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   // Run Numbers
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"RunNumbers\")) {\n");
   sprintf(buffer+strlen(buffer),"         if (xml->GetAttribute(\"Numbers\",value))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetRunNumbers(value);\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   // Event Numbers
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"EventNumbers\")) {\n");
   sprintf(buffer+strlen(buffer),"         if (xml->GetAttribute(\"Numbers\",value))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetEventNumbers(value);\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   // Paths
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"InputFilePath\")) {\n");
   sprintf(buffer+strlen(buffer),"         if (xml->GetValue(value))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetInputDir(value);\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"OutputFilePath\")) {\n");
   sprintf(buffer+strlen(buffer),"         if (xml->GetValue(value))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetOutputDir(value);\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"DataBaseFilePath\")) {\n");
   sprintf(buffer+strlen(buffer),"         if (xml->GetValue(value))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetDataBaseDir(value);\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   // Tasks
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"Tasks\")) {\n");
   sprintf(buffer+strlen(buffer),"         while (xml->NextLine()) {\n");
   sprintf(buffer+strlen(buffer),"            type = xml->GetType();\n");
   sprintf(buffer+strlen(buffer),"            name = xml->GetName();\n");
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer+strlen(buffer),"            if (type == 1 && !strcmp(name,\"%s\")) {\n",taskName[i]);
      sprintf(buffer+strlen(buffer),"               int empty = xml->isEmpty();\n");
      sprintf(buffer+strlen(buffer),"               strcpy(value,\"\");\n");
      sprintf(buffer+strlen(buffer),"               xml->GetAttribute(\"Active\",value);\n");
      sprintf(buffer+strlen(buffer),"               if (!strcmp(value,\"yes\"))\n");
      sprintf(buffer+strlen(buffer),"                  %sTask->SetActive();\n",taskName[i]);
      sprintf(buffer+strlen(buffer),"               if (!empty) {\n");
      sprintf(buffer+strlen(buffer),"                  while (xml->NextLine()) {\n");
      sprintf(buffer+strlen(buffer),"                     type = xml->GetType();\n");
      sprintf(buffer+strlen(buffer),"                     name = xml->GetName();\n");
      for (j=0;j<numOfHistos[i];j++) {
         sprintf(buffer+strlen(buffer),"                     if (type == 1 && !strcmp(name,\"%s\")) {\n",histoName[i][j]);
         sprintf(buffer+strlen(buffer),"                        strcpy(value,\"\");\n");
         sprintf(buffer+strlen(buffer),"                        xml->GetAttribute(\"Accumulate\",value);\n");
         sprintf(buffer+strlen(buffer),"                        if (!strcmp(value,\"no\"))\n");
         sprintf(buffer+strlen(buffer),"                           ((%sT%s*)%sTask)->Set%sAccumulation(false);\n",shortCut,taskName[i],taskName[i],histoName[i][j]);
         sprintf(buffer+strlen(buffer),"                     }\n");
      }
      WriteTaskSteerConfigRead(buffer,0,i);
      sprintf(buffer+strlen(buffer),"                     if (type == 15 && !strcmp(name,\"%s\"))\n",taskName[i]);
      sprintf(buffer+strlen(buffer),"                        break;\n");
      sprintf(buffer+strlen(buffer),"                  }\n");
      sprintf(buffer+strlen(buffer),"               }\n");
      sprintf(buffer+strlen(buffer),"            }\n");
   }
   sprintf(buffer+strlen(buffer),"            if (type == 15 && !strcmp(name,\"Tasks\"))\n");
   sprintf(buffer+strlen(buffer),"               break;\n");
   sprintf(buffer+strlen(buffer),"         }\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   // Trees
   sprintf(buffer+strlen(buffer),"      if (type == 1 && !strcmp(name,\"Trees\")) {\n");
   sprintf(buffer+strlen(buffer),"         strcpy(value,\"\");\n");
   sprintf(buffer+strlen(buffer),"         xml->GetAttribute(\"Accumulation\",value);\n");
   sprintf(buffer+strlen(buffer),"         if (!strcmp((const char*)value,\"yes\"))\n");
   sprintf(buffer+strlen(buffer),"            this->GetIO()->SetTreeAccumulation();\n");
   sprintf(buffer+strlen(buffer),"         while (xml->NextLine()) {\n");
   sprintf(buffer+strlen(buffer),"            type = xml->GetType();\n");
   sprintf(buffer+strlen(buffer),"            name = xml->GetName();\n");
   for (i=0;i<numOfTree;i++) {
      sprintf(buffer+strlen(buffer),"            if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",treeName[i]);
      sprintf(buffer+strlen(buffer),"               strcpy(value,\"\");\n");
      sprintf(buffer+strlen(buffer),"               xml->GetAttribute(\"Read\",value);\n");
      sprintf(buffer+strlen(buffer),"               if (!strcmp((const char*)value,\"yes\"))\n");
      sprintf(buffer+strlen(buffer),"                  this->GetIO()->GetTreeObjectAt(%d)->SetRead(true);\n",i);
      sprintf(buffer+strlen(buffer),"               strcpy(value,\"\");\n");
      sprintf(buffer+strlen(buffer),"               xml->GetAttribute(\"Write\",value);\n");
      sprintf(buffer+strlen(buffer),"               if (!strcmp((const char*)value,\"yes\"))\n");
      sprintf(buffer+strlen(buffer),"                  this->GetIO()->GetTreeObjectAt(%d)->SetWrite(true);\n",i);
      sprintf(buffer+strlen(buffer),"            }\n");
   }
   sprintf(buffer+strlen(buffer),"            if (type == 15 && !strcmp((const char*)name,\"Trees\"))\n");
   sprintf(buffer+strlen(buffer),"               break;\n");
   sprintf(buffer+strlen(buffer),"         }\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   // Steering
   WriteSteerConfigRead(buffer,0);

   sprintf(buffer+strlen(buffer),"      if (type == 15 && !strcmp((const char*)name,\"Configuration\"))\n");
   sprintf(buffer+strlen(buffer),"         break;\n");
   sprintf(buffer+strlen(buffer),"   }\n");
   sprintf(buffer+strlen(buffer),"   delete xml;\n");
   sprintf(buffer+strlen(buffer),"   return true;\n");
   sprintf(buffer+strlen(buffer),"}\n");

   // WriteROMEConfigXML
   //--------------------
   sprintf(buffer+strlen(buffer),"bool %sAnalyzer::WriteROMEConfigXML(char *configFile) {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   ROMEXML *xml = new ROMEXML();\n");
   sprintf(buffer+strlen(buffer),"   char value[1000];\n");
   sprintf(buffer+strlen(buffer),"   if (!xml->OpenFileForWrite(configFile))\n");
   sprintf(buffer+strlen(buffer),"      return false;\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   xml->StartElement(\"Configuration\");\n");
   //modes
   sprintf(buffer+strlen(buffer),"   xml->StartElement(\"Modes\");\n");
   sprintf(buffer+strlen(buffer),"   if (this->GetIO()->isOnline())\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"AnalyzingMode\",\"online\");\n");
   sprintf(buffer+strlen(buffer),"   else\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"AnalyzingMode\",\"offline\");\n");
   sprintf(buffer+strlen(buffer),"   if (this->GetIO()->isMidas())\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"InputDataFormat\",\"midas\");\n");
   sprintf(buffer+strlen(buffer),"   else\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"InputDataFormat\",\"root\");\n");
   sprintf(buffer+strlen(buffer),"   if (isBatchMode())\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"BatchMode\",\"yes\");\n");
   sprintf(buffer+strlen(buffer),"   else\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"BatchMode\",\"no\");\n");
   sprintf(buffer+strlen(buffer),"   if (isSplashScreen())\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"ShowSplashScreen\",\"yes\");\n");
   sprintf(buffer+strlen(buffer),"   else\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"ShowSplashScreen\",\"no\");\n");
   sprintf(buffer+strlen(buffer),"   if (this->GetIO()->isXMLDataBase())\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"DataBaseMode\",\"xml\");\n");
   sprintf(buffer+strlen(buffer),"   else if (this->GetIO()->isSQLDataBase())\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"DataBaseMode\",\"sql\");\n");
   sprintf(buffer+strlen(buffer),"   else\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"DataBaseMode\",\"none\");\n");
   sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
   //run numbers
   sprintf(buffer+strlen(buffer),"   xml->StartElement(\"RunNumbers\");\n");
   sprintf(buffer+strlen(buffer),"   xml->WriteAttribute(\"Numbers\",this->GetIO()->GetRunNumberStringOriginal());\n");
   sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
   //event numbers
   sprintf(buffer+strlen(buffer),"   xml->StartElement(\"EventNumbers\");\n");
   sprintf(buffer+strlen(buffer),"   xml->WriteAttribute(\"Numbers\",this->GetIO()->GetEventNumberStringOriginal());\n");
   sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
   //paths
   sprintf(buffer+strlen(buffer),"   xml->StartElement(\"Paths\");\n");
   sprintf(buffer+strlen(buffer),"   xml->WriteElement(\"InputFilePath\",this->GetIO()->GetInputDir());\n");
   sprintf(buffer+strlen(buffer),"   xml->WriteElement(\"OutputFilePath\",this->GetIO()->GetOutputDir());\n");
   sprintf(buffer+strlen(buffer),"   xml->WriteElement(\"DataBaseFilePath\",this->GetIO()->GetDataBaseDir());\n");
   sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
   sprintf(buffer+strlen(buffer),"\n");

   //tasks
   sprintf(buffer+strlen(buffer),"   xml->StartElement( \"Tasks\");\n");
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer+strlen(buffer),"   xml->StartElement(\"%s\");\n",taskName[i]);
      sprintf(buffer+strlen(buffer),"   if (%sTask->IsActive())\n",taskName[i]);
      sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Active\",\"yes\");\n");
      sprintf(buffer+strlen(buffer),"   else\n");
      sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Active\",\"no\");\n");
      for (j=0;j<numOfHistos[i];j++) {
         sprintf(buffer+strlen(buffer),"   xml->StartElement(\"%s\");\n",histoName[i][j]);
         sprintf(buffer+strlen(buffer),"   if (((%sT%s*)%sTask)->is%sAccumulation())\n",shortCut,taskName[i],taskName[i],histoName[i][j]);
         sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Accumulate\",\"yes\");\n");
         sprintf(buffer+strlen(buffer),"   else\n");
         sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Accumulate\",\"no\");\n");
         sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
      }
      if (numOfTaskSteering[i]>0)
         WriteTaskSteerConfigWrite(buffer,0,i);
      sprintf(buffer+strlen(buffer),"      xml->EndElement();\n");
   }
   sprintf(buffer+strlen(buffer),"      xml->EndElement();\n");

   //trees
   sprintf(buffer+strlen(buffer),"   xml->StartElement(\"Trees\");\n");
   sprintf(buffer+strlen(buffer),"   if (this->GetIO()->isTreeAccumulation())\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Accumulation\",\"yes\");\n");
   sprintf(buffer+strlen(buffer),"   else\n");
   sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Accumulation\",\"no\");\n");

   for (i=0;i<numOfTree;i++) {
      sprintf(buffer+strlen(buffer),"   xml->StartElement(\"%s\");\n",treeName[i]);
      sprintf(buffer+strlen(buffer),"   if (this->GetIO()->GetTreeObjectAt(%d)->isRead())\n",i);
      sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Read\",\"yes\");\n");
      sprintf(buffer+strlen(buffer),"   else\n");
      sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Read\",\"no\");\n");
      sprintf(buffer+strlen(buffer),"   if (this->GetIO()->GetTreeObjectAt(%d)->isWrite())\n",i);
      sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Write\",\"yes\");\n");
      sprintf(buffer+strlen(buffer),"   else\n");
      sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Write\",\"no\");\n");
      sprintf(buffer+strlen(buffer),"   xml->EndElement();\n");
   }
   sprintf(buffer+strlen(buffer),"      xml->EndElement();\n");

   //steering
   if (numOfSteering>0)
      WriteSteerConfigWrite(buffer,0);

   sprintf(buffer+strlen(buffer),"   xml->EndDocument();\n");
   sprintf(buffer+strlen(buffer),"   return true;\n");
   sprintf(buffer+strlen(buffer),"}\n");

   // Splash Screen
   sprintf(buffer+strlen(buffer),"#if defined( _MSC_VER )\n");
   sprintf(buffer+strlen(buffer),"LPDWORD lpThreadId;\n");
   sprintf(buffer+strlen(buffer),"void %sAnalyzer::startSplashScreen() {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   CloseHandle(CreateThread(NULL,1024,&SplashThread,0,0,lpThreadId));\n");
   sprintf(buffer+strlen(buffer),"}\n");
   sprintf(buffer+strlen(buffer),"#endif\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"#if defined ( __linux__ )\n");
   sprintf(buffer+strlen(buffer),"void %sAnalyzer::startSplashScreen() {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   \n");
   sprintf(buffer+strlen(buffer),"}\n");
   sprintf(buffer+strlen(buffer),"#endif\n");
   sprintf(buffer+strlen(buffer),"\n");

   // Console Screen
   char prog[100];
   sprintf(prog,"%s%s",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"void %sAnalyzer::consoleStartScreen() {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   int i;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*****************************************\" << endl;\n");   
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*\";\n");
   int len1 = (int)((39-(double)strlen(prog))/2+0.5);
   int len2 = (int)((39-(double)strlen(prog))/2);
   sprintf(buffer+strlen(buffer),"   for (i=0;i<%d;i++) cout << \" \";\n",len1);
   sprintf(buffer+strlen(buffer),"   cout << \"%s\";\n",prog);
   sprintf(buffer+strlen(buffer),"   for (i=0;i<%d;i++) cout << \" \";\n",len2);
   sprintf(buffer+strlen(buffer),"   cout << \"*\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*   generated by the ROME Environment   *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*             %s              *\" << endl;\n",romeVersion);
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*****************************************\" << endl << endl;\n");
   sprintf(buffer+strlen(buffer),"}\n");
   sprintf(buffer+strlen(buffer),"   \n");

   // Close cpp-File
   sprintf(cppFile,"%s/src/framework/%sAnalyzer.cpp",outDir,shortCut);
   fileHandle = open(cppFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<(int)strlen(buffer);i++) {
      if (buffer[i] != fileBuffer[i]) {
         identical = false;
      }
   }
   if (!identical) {
      fileHandle = open(cppFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(cppFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << cppFile << endl;
      nb = write(fileHandle,&buffer, strlen(buffer));
      close(fileHandle);
   }
   return true;
}

bool ROMEBuilder::WriteAnalyzerH() {
   int i;

   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   char format[100];
   int nameLen = -1;
   int typeLen = 12;
   int scl = strlen(shortCut);
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)strlen(folderName[i])+scl) typeLen = strlen(folderName[i])+scl;
      if (nameLen<(int)strlen(folderName[i])) nameLen = strlen(folderName[i]);
   }
   int taskLen=0;
   for (i=0;i<numOfTask;i++) {
      if (taskLen<(int)strlen(taskName[i])) taskLen = strlen(taskName[i]);
   }



   sprintf(buffer,"////////////////////////////////////////////////////////////////////////////////\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
   sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

   sprintf(buffer+strlen(buffer),"#ifndef %sAnalyzer_H\n",shortCut);
   sprintf(buffer+strlen(buffer),"#define %sAnalyzer_H\n\n",shortCut);

   sprintf(buffer+strlen(buffer),"#include <TTask.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TTree.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TFolder.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TClonesArray.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMETask.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMEAnalyzer.h>\n");
   sprintf(buffer+strlen(buffer),"#include <include/framework/%sIO.h>\n\n",shortCut);

   if (numOfSteering!=0) {
      sprintf(buffer+strlen(buffer),"#include <include/framework/%sGeneralSteering.h>\n",shortCut);
   }

   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         sprintf(buffer+strlen(buffer),"#include <include/framework/%s%s.h>\n",shortCut,folderName[i]);
      }
   }


   // Class
   //-------

   sprintf(buffer+strlen(buffer),"\nclass %sAnalyzer : public ROMEAnalyzer\n",shortCut);
   sprintf(buffer+strlen(buffer),"{\n");

   // Fields
   //--------

   sprintf(buffer+strlen(buffer),"protected:\n");

   // Folder Fields
   sprintf(buffer+strlen(buffer),"   // Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (strcmp(folderArray[i],"1")) {
            sprintf(format,"   TClonesArray*%%%ds f%%sObjects;%%%ds // Handle to %%s%%s Objects\n",typeLen-12,nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,"",folderName[i],"",shortCut,folderName[i]);
         }
         else {
            sprintf(format,"   %%s%%s*%%%ds f%%sObject; %%%ds // Handle to %%s%%s Object\n",typeLen-strlen(folderName[i])-scl,nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",shortCut,folderName[i]);
         }
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

   // Task Fields
   sprintf(buffer+strlen(buffer),"   // Task Fields\n");
   for (i=0;i<numOfTask;i++) {
      sprintf(format,"   ROMETask* %%sTask%%%ds;  // Handle to %%s Task\n",taskLen-strlen(taskName[i]));
      sprintf(buffer+strlen(buffer),format,taskName[i],"",taskName[i]);
   }
   sprintf(buffer+strlen(buffer),"\n");

   // Steering Fields
   if (numOfSteering!=0) {
      sprintf(buffer+strlen(buffer),"   // Steering Parameter Fields\n");
      sprintf(buffer+strlen(buffer),"\n   %sGeneralSteering* fGeneralSteeringParameters; // Handle to the GeneralSteering Class\n",shortCut);
   }

   // Methods
   //---------

   sprintf(buffer+strlen(buffer),"public:\n");
   // Constructor
   sprintf(buffer+strlen(buffer),"   %sAnalyzer();\n",shortCut);
   sprintf(buffer+strlen(buffer),"   ~%sAnalyzer();\n\n",shortCut);

   sprintf(buffer+strlen(buffer),"   %sIO* GetIO() { return (%sIO*)fIO; };\n\n",shortCut,shortCut);

   sprintf(buffer+strlen(buffer),"   void InitFolders();\n\n");

   // Getters
   sprintf(buffer+strlen(buffer),"   // Folder Getters\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         int lt = typeLen-strlen(folderName[i])-scl+nameLen-strlen(folderName[i]);
         if (!strcmp(folderArray[i],"1")) {
            sprintf(format,"   %%s%%s*%%%ds  Get%%sObject()%%%ds { return f%%sObject;%%%ds };\n",typeLen-strlen(folderName[i])-scl,8+nameLen-strlen(folderName[i]),15+typeLen+nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",folderName[i],"");
            sprintf(format,"   %%s%%s**%%%ds Get%%sObjectAddress()%%%ds { return &f%%sObject;%%%ds };\n",typeLen-strlen(folderName[i])-scl,1+nameLen-strlen(folderName[i]),14+typeLen+nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",folderName[i],"");
         }
         else {
            sprintf(format,"   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds { return (%%s%%s*)f%%sObjects->At(index);%%%ds };\n",typeLen-strlen(folderName[i])-scl,3+nameLen-strlen(folderName[i]),lt);
            sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",shortCut,folderName[i],folderName[i],"");
            sprintf(format,"   TClonesArray*%%%ds  Get%%sObjects()%%%ds { return f%%sObjects;%%%ds };\n",typeLen-12,7+nameLen-strlen(folderName[i]),14+typeLen+nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,"",folderName[i],"",folderName[i],"");
            sprintf(format,"   TClonesArray**%%%ds Get%%sObjectsAddress()%%%ds { return &f%%sObjects;%%%ds };\n",typeLen-12,nameLen-strlen(folderName[i]),13+typeLen+nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,"",folderName[i],"",folderName[i],"");
         }
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

   // Configuration file
   sprintf(buffer+strlen(buffer),"   // Configuration File\n");
   sprintf(buffer+strlen(buffer),"   bool ReadROMEConfigXML(char *configFile);\n");
   sprintf(buffer+strlen(buffer),"   bool WriteROMEConfigXML(char *configFile);\n");
   sprintf(buffer+strlen(buffer),"\n");
 
   // Banks
   if (numOfBank>0) {
      sprintf(buffer+strlen(buffer),"   // Bank Methodes\n");
      for (i=0;i<numOfBank;i++) {
         if (!strcmp(bankType[i],"structure")||!strcmp(bankType[i],"struct")) {
            sprintf(buffer+strlen(buffer),"   %s* Get%sBankAt(int index) { return this->GetIO()->Get%sBankAt(index); };\n",bankStructName[i],bankName[i],bankName[i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   %s Get%sBankAt(int index) { return this->GetIO()->Get%sBankAt(index); };\n",bankType[i],bankName[i],bankName[i]);
         }
         sprintf(buffer+strlen(buffer),"   int Get%sBankEntries() { return this->GetIO()->Get%sBankEntries(); };\n",bankName[i],bankName[i]);
         sprintf(buffer+strlen(buffer),"   void Init%sBank() { this->GetIO()->Init%sBank(); };\n",bankName[i],bankName[i]);
      }
      sprintf(buffer+strlen(buffer),"\n");
   }


   // Data Base
   sprintf(buffer+strlen(buffer),"   // Data Base Methodes\n");
   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"   void Write%sDataBase() { this->GetIO()->Write%sDataBase(); };\n",folderName[i],folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

   // Steering
   if (numOfSteering>0) {
      sprintf(buffer+strlen(buffer),"   // Steering Parameter Methodes\n");
      sprintf(buffer+strlen(buffer),"   %sGeneralSteering* GetGeneralSteeringParameters() { return fGeneralSteeringParameters; };\n",shortCut);
      sprintf(buffer+strlen(buffer),"   %sGeneralSteering* GetGSP() { return fGeneralSteeringParameters; };\n",shortCut);
      sprintf(buffer+strlen(buffer),"\n");
   }

   // Private
   sprintf(buffer+strlen(buffer),"private:\n");
   sprintf(buffer+strlen(buffer),"   void startSplashScreen();\n");
   sprintf(buffer+strlen(buffer),"   void consoleStartScreen();\n");

   // Footer
   //--------

   sprintf(buffer+strlen(buffer),"};\n\n");

   sprintf(buffer+strlen(buffer),"#endif   // %sAnalyzer_H\n",shortCut);

   // Close h-File
   //--------------

   sprintf(hFile,"%s/include/framework/%sAnalyzer.h",outDir,shortCut);
   fileHandle = open(hFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<(int)strlen(buffer);i++) {
      if (buffer[i] != fileBuffer[i]) {
         identical = false;
      }
   }
   if (!identical) {
      fileHandle = open(hFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(hFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << hFile << endl;
      nb = write(fileHandle,&buffer, strlen(buffer));
      close(fileHandle);
   }
   return true;
}

bool ROMEBuilder::WriteIOCpp() {
   int i;

   char cppFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   char format[100];
   int nb,j,k,iFold=0,ll;
   int fileHandle;

// Description
//-------------
   sprintf(buffer,"//// Author: %s\n",mainAuthor);
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   ll = 74-strlen(shortCut);
   sprintf(format,"// %%s%%-%d.%ds //\n",ll,ll);
   sprintf(buffer+strlen(buffer),format,shortCut,"IO");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Derivated form ROMEIO. Implements Experiment specific IO methods.          //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
   sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

// Header Files
//--------------

   sprintf(buffer+strlen(buffer),"#include <sys/stat.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TBranchElement.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMEXML.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMETree.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMETreeInfo.h>\n");
   sprintf(buffer+strlen(buffer),"#ifdef HAVE_MIDAS\n");
   sprintf(buffer+strlen(buffer),"#include <midas.h>\n");
   sprintf(buffer+strlen(buffer),"#endif\n");
   sprintf(buffer+strlen(buffer),"#include <include/framework/%sIO.h>\n",shortCut);

   sprintf(buffer+strlen(buffer),"#include \"Riostream.h\"\n");

   // User Functions
   //----------------

   // constructor
   sprintf(buffer+strlen(buffer),"// Constructor\n");
   sprintf(buffer+strlen(buffer),"%sIO::%sIO(",shortCut,shortCut);
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (strcmp(folderArray[i],"1")) {
            sprintf(buffer+strlen(buffer),"TClonesArray* %sObjects,",folderName[i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"%s%s* %sObject,",shortCut,folderName[i],folderName[i]);
         }
      }
   }
   sprintf(buffer+strlen(buffer)-1,")\n");
   sprintf(buffer+strlen(buffer),"{\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (strcmp(folderArray[i],"1")) {
            sprintf(buffer+strlen(buffer),"   f%sObjects = %sObjects;\n",folderName[i],folderName[i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   f%sObject = %sObject;\n",folderName[i],folderName[i]);
         }
      }
   }
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   TTree *tree;\n\n");
   for (i=0;i<numOfTree;i++) {
      sprintf(buffer+strlen(buffer),"   tree = new TTree(\"%s\",\"%s\");\n",treeName[i],treeTitle[i]);
      sprintf(buffer+strlen(buffer),"   tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000,99);\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (!strcmp(branchFolder[i][j],folderName[k])) iFold = k;
         }
         if (strcmp(folderArray[iFold],"1")) {
            sprintf(buffer+strlen(buffer),"   tree->Branch(\"%s\",\"TClonesArray\",&f%sObjects,32000,99);\n",branchName[i][j],branchFolder[i][j]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   tree->Branch(\"%s\",\"%s%s\",&f%sObject,32000,99);\n",branchName[i][j],shortCut,folderName[iFold],branchFolder[i][j]);
         }
      }
      sprintf(buffer+strlen(buffer),"   this->AddTree(tree);\n\n");
   }
   sprintf(buffer+strlen(buffer),"}\n\n");

   sprintf(buffer+strlen(buffer),"%sIO::~%sIO() {\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"}\n\n");

   // clear folders
   sprintf(buffer+strlen(buffer),"// Clear Folders\n");
   sprintf(buffer+strlen(buffer),"void %sIO::ClearFolders() {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !dataBase[i]) {
         if (!strcmp(folderArray[i],"1")) {
            sprintf(buffer+strlen(buffer),"   f%sObject->Reset();\n",folderName[i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   for (i=0;i<f%sObjects->GetEntriesFast();i++) {\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"      ((%s%s*)f%sObjects->At(i))->Reset();\n",shortCut,folderName[i],folderName[i]);
            sprintf(buffer+strlen(buffer),"   }\n");
         }
      }
      if (!strcmp(folderName[i],bankHeaderFolder)) {
         if (strcmp(bankHeaderEventID,""))
            sprintf(buffer+strlen(buffer),"   f%sObject->Set%s(this->GetEventHeader()->event_id);\n",folderName[i],bankHeaderEventID);
         if (strcmp(bankHeaderTriggerMask,""))
            sprintf(buffer+strlen(buffer),"   f%sObject->Set%s(this->GetEventHeader()->trigger_mask);\n",folderName[i],bankHeaderTriggerMask);
         if (strcmp(bankHeaderSerialNumber,""))
            sprintf(buffer+strlen(buffer),"   f%sObject->Set%s(this->GetEventHeader()->serial_number);\n",folderName[i],bankHeaderSerialNumber);
         if (strcmp(bankHeaderTimeStamp,""))
            sprintf(buffer+strlen(buffer),"   f%sObject->Set%s(this->GetEventHeader()->time_stamp);\n",folderName[i],bankHeaderTimeStamp);
      }
   }
   sprintf(buffer+strlen(buffer),"}\n\n");

   // fill trees
   sprintf(buffer+strlen(buffer),"// Tree Filling\n");
   sprintf(buffer+strlen(buffer),"void %sIO::FillTrees() {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   ROMETree *romeTree;\n");
   sprintf(buffer+strlen(buffer),"   int i;\n");
   sprintf(buffer+strlen(buffer),"   // Fill Trees;\n");
   sprintf(buffer+strlen(buffer),"   bool write = false;\n");
   sprintf(buffer+strlen(buffer),"   bool written = false;\n");
   for (i=0;i<numOfTree;i++) {
      sprintf(buffer+strlen(buffer),"   write = false;\n");
      sprintf(buffer+strlen(buffer),"   romeTree = (ROMETree*)fTreeObjects->At(%d);\n",i);
      sprintf(buffer+strlen(buffer),"   if (romeTree->isWrite()) {\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (!strcmp(folderName[k],branchFolder[i][j])) {
               iFold = k;
               break;
            }
         }
         if (!strcmp(folderArray[iFold],"1")) {
            sprintf(buffer+strlen(buffer),"      if (!write && f%sObject->isModified()) {\n",branchFolder[i][j]);
            sprintf(buffer+strlen(buffer),"         write = true;\n");
            sprintf(buffer+strlen(buffer),"      }\n");
         }
         else {
            sprintf(buffer+strlen(buffer),"      for (i=0;i<f%sObjects->GetEntries()&&!write;i++) {\n",branchFolder[i][j]);
            sprintf(buffer+strlen(buffer),"         if (((%s%s*)f%sObjects->At(i))->isModified()) {\n",shortCut,branchFolder[i][j],branchFolder[i][j]);
            sprintf(buffer+strlen(buffer),"            write = true;\n");
            sprintf(buffer+strlen(buffer),"            break;\n");
            sprintf(buffer+strlen(buffer),"         }\n");
            sprintf(buffer+strlen(buffer),"      }\n");
         }
      }
      sprintf(buffer+strlen(buffer),"      if (write) {\n");
      sprintf(buffer+strlen(buffer),"         written = true;\n");
      sprintf(buffer+strlen(buffer),"         fTreeInfo->SetSequentialNumber(fSequentialNumber);\n");
      sprintf(buffer+strlen(buffer),"         romeTree->GetTree()->Fill();\n");
      sprintf(buffer+strlen(buffer),"      }\n");
      sprintf(buffer+strlen(buffer),"   }\n");
   }
   sprintf(buffer+strlen(buffer),"   if (written) fSequentialNumber++;\n");
   sprintf(buffer+strlen(buffer),"}\n");
   sprintf(buffer+strlen(buffer),"\n");


   // Midas Bank Initialisation
   sprintf(buffer+strlen(buffer),"// Midas Bank Initialisation\n");
   sprintf(buffer+strlen(buffer),"void %sIO::InitMidasBanks() {\n",shortCut);
   for (i=0;i<numOfBank;i++) {
      sprintf(buffer+strlen(buffer),"   this->Init%sBank();\n",bankName[i]);
   }
   sprintf(buffer+strlen(buffer),"}\n");
   sprintf(buffer+strlen(buffer),"\n");

   // Midas Bank Methodes
   sprintf(buffer+strlen(buffer),"\n// Midas Bank Getters\n");
   for (i=0;i<numOfBank;i++) {
      if (!strcmp(bankType[i],"structure")||!strcmp(bankType[i],"struct")) {
         sprintf(buffer+strlen(buffer),"%s* %sIO::Get%sBankAt(int index) {\n",bankStructName[i],shortCut,bankName[i]);
         sprintf(buffer+strlen(buffer),"   if (this->f%sBankExists)\n",bankName[i]);
         sprintf(buffer+strlen(buffer),"      return f%sBankPointer+index;\n",bankName[i]);
         sprintf(buffer+strlen(buffer),"   return NULL;\n");
         sprintf(buffer+strlen(buffer),"}\n");
      }
      else {
         sprintf(buffer+strlen(buffer),"%s %sIO::Get%sBankAt(int index) {\n",bankType[i],shortCut,bankName[i]);
         sprintf(buffer+strlen(buffer),"   if (this->f%sBankExists)\n",bankName[i]);
         sprintf(buffer+strlen(buffer),"      return *(f%sBankPointer+index);\n",bankName[i]);
         sprintf(buffer+strlen(buffer),"   return (%s)exp(999);\n",bankType[i]);
         sprintf(buffer+strlen(buffer),"}\n");
      }
      sprintf(buffer+strlen(buffer),"void %sIO::Init%sBank() {\n",shortCut,bankName[i]);
      sprintf(buffer+strlen(buffer),"   unsigned long bktype;\n");
      sprintf(buffer+strlen(buffer),"   EVENT_HEADER *pevent = this->GetEventHeader();\n");
      sprintf(buffer+strlen(buffer),"   pevent++;\n");
      sprintf(buffer+strlen(buffer),"   if (ROMEStatic::bk_find(pevent, \"%s\", (unsigned long*)&f%sBankLength, &bktype, &f%sBankPointer)) {\n",bankName[i],bankName[i],bankName[i]);
      sprintf(buffer+strlen(buffer),"      f%sBankExists = true;\n",bankName[i]);
      sprintf(buffer+strlen(buffer),"      return;\n");
      sprintf(buffer+strlen(buffer),"   }\n");
      sprintf(buffer+strlen(buffer),"   f%sBankExists = false;\n",bankName[i]);
      sprintf(buffer+strlen(buffer),"   f%sBankPointer = NULL;\n",bankName[i]);
      sprintf(buffer+strlen(buffer),"   f%sBankLength = 0;\n",bankName[i]);
      sprintf(buffer+strlen(buffer),"   return;\n");
      sprintf(buffer+strlen(buffer),"}\n");

      sprintf(buffer+strlen(buffer),"int %sIO::Get%sBankEntries() {\n",shortCut,bankName[i]);
      sprintf(buffer+strlen(buffer),"   return f%sBankLength;\n",bankName[i]);
      sprintf(buffer+strlen(buffer),"}\n\n");
   }


   // Connect Trees
   sprintf(buffer+strlen(buffer),"// Connect Trees\n");
   sprintf(buffer+strlen(buffer),"void %sIO::ConnectTrees()\n{\n",shortCut);
   sprintf(buffer+strlen(buffer),"   TBranchElement *bb;\n");
   for (i=0;i<numOfTree;i++) {
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (!strcmp(branchFolder[i][j],folderName[k])) iFold = k;
         }
         sprintf(buffer+strlen(buffer),"   bb = (TBranchElement*)this->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"%s\");\n",i,branchName[i][j]);
         if (strcmp(folderArray[iFold],"1")) {
            sprintf(buffer+strlen(buffer),"   bb->SetAddress(&this->f%sObjects);\n",folderName[iFold]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   bb->SetAddress(&this->f%sObject);\n",folderName[iFold]);
         }
         sprintf(buffer+strlen(buffer),"   bb = (TBranchElement*)this->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"Info\");\n",i);
         sprintf(buffer+strlen(buffer),"   bb->SetAddress(&this->fTreeInfo);\n");
      }
   }
   sprintf(buffer+strlen(buffer),"}\n\n");

   int ndb = 0;
   for (i=0;i<numOfFolder;i++) if (dataBase[i]) ndb++;
   // SQL Init
   sprintf(buffer+strlen(buffer),"bool %sIO::InitSQLDataBase()\n",shortCut);
   sprintf(buffer+strlen(buffer),"{\n");
   if (ndb>0) {
      sprintf(buffer+strlen(buffer),"   fSQL = new ROMESQL();\n");
      sprintf(buffer+strlen(buffer),"   return fSQL->Connect(\"pc4466.psi.ch\",\"rome\",\"rome\",\"%sDataBase\");\n",shortCut);
   }
   else {
      sprintf(buffer+strlen(buffer),"   return true;\n");
   }
   sprintf(buffer+strlen(buffer),"}\n\n");

   // SQL Read
   char buf[200];
   sprintf(buffer+strlen(buffer),"bool %sIO::ReadSQLDataBase()\n",shortCut);
   sprintf(buffer+strlen(buffer),"{\n");
   if (ndb>0) {
      sprintf(buffer+strlen(buffer),"   char *cstop,*res;\n");
      sprintf(buffer+strlen(buffer),"   int i;\n");
      for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         if (strcmp(folderArray[i],"1")) {
            for (j=0;j<numOfValue[i];j++) {
               sprintf(buffer+strlen(buffer),"   fSQL->ReadPathFields(\"%s\",this->GetCurrentRunNumber(),\"id\");\n",dataBasePath[i][j]);
               sprintf(buffer+strlen(buffer),"   for(i=0;i<fSQL->GetNumberOfRows();i++){\n");
               sprintf(buffer+strlen(buffer),"      fSQL->NextRow();\n");
               sprintf(buffer+strlen(buffer),"      res = fSQL->GetField(0);\n");
               setValue(buf,valueName[i][j],"res",valueType[i][j],1);
               sprintf(buffer+strlen(buffer),"      ((%s%s*)f%sObjects->At(i))->Set%s((%s)%s);\n",shortCut,folderName[i],folderName[i],valueName[i][j],valueType[i][j],buf);
               sprintf(buffer+strlen(buffer),"   }\n");
            }
         }
         else {
            for (j=0;j<numOfValue[i];j++) {
               sprintf(buffer+strlen(buffer),"   fSQL->ReadPathFields(\"%s\",this->GetCurrentRunNumber(),\"id\");\n",dataBasePath[i][j]);
               sprintf(buffer+strlen(buffer),"   fSQL->NextRow();\n");
               sprintf(buffer+strlen(buffer),"   res = fSQL->GetField(0);\n");
               setValue(buf,valueName[i][j],"res",valueType[i][j],1);
               sprintf(buffer+strlen(buffer),"   ((%s%s*)f%sObjects->At(i))->Set%s((%s)%s);\n",shortCut,folderName[i],folderName[i],valueName[i][j],valueType[i][j],buf);
            }
         }
      }
   }
   }
   sprintf(buffer+strlen(buffer),"   return true;\n");
   sprintf(buffer+strlen(buffer),"}\n\n");

   // ReadXMLDataBase
   sprintf(buffer+strlen(buffer),"bool %sIO::ReadXMLDataBase() {\n",shortCut);
   if (ndb>0) {
      sprintf(buffer+strlen(buffer),"   int i;\n");
      sprintf(buffer+strlen(buffer),"   char *cstop;\n");
      sprintf(buffer+strlen(buffer),"   char *name;\n");
      sprintf(buffer+strlen(buffer),"   int type;\n");
      sprintf(buffer+strlen(buffer),"   char dbFile[200];\n");
      sprintf(buffer+strlen(buffer),"   char filename[200];\n");
      sprintf(buffer+strlen(buffer),"   char path[200];\n");
      sprintf(buffer+strlen(buffer),"   char value[200];\n");
      sprintf(buffer+strlen(buffer),"   char buf[400];\n");
      sprintf(buffer+strlen(buffer),"   char runNumberString[6];\n");
      sprintf(buffer+strlen(buffer),"   this->GetCurrentRunNumberString(runNumberString);\n");
      sprintf(buffer+strlen(buffer),"   sprintf(filename,\"%%s/RunTable.xml\",this->GetInputDir());\n");
      sprintf(buffer+strlen(buffer),"   sprintf(path,\"//RunTable/Run_%%s\",runNumberString);\n");
      sprintf(buffer+strlen(buffer),"\n");
      sprintf(buffer+strlen(buffer),"   ROMEXML *xml = new ROMEXML();\n");
      sprintf(buffer+strlen(buffer),"   if (!xml->OpenFileForPath(filename)) { \n");
      sprintf(buffer+strlen(buffer),"      cout << \"\\nFailed to load xml database : '\" << filename<< \"'\" << endl;\n");
      sprintf(buffer+strlen(buffer),"      cout << \"Do you like the framework to generate a new xml database ([y]/n) ? \";\n");
      sprintf(buffer+strlen(buffer),"      char answer[10];\n");
      sprintf(buffer+strlen(buffer),"      cin >> answer;\n");
      sprintf(buffer+strlen(buffer),"      if (strstr(answer,\"n\")==NULL) {\n");
      sprintf(buffer+strlen(buffer),"         ROMEXML *xmlNew = new ROMEXML();\n");
      sprintf(buffer+strlen(buffer),"         if (!xmlNew->OpenFileForWrite(filename))\n");
      sprintf(buffer+strlen(buffer),"            return false;\n");
      sprintf(buffer+strlen(buffer),"         xmlNew->StartElement(\"RunTable\");\n");
      sprintf(buffer+strlen(buffer),"         xmlNew->EndDocument();\n");
      sprintf(buffer+strlen(buffer),"         delete xmlNew;\n");
      sprintf(buffer+strlen(buffer),"         if (!xml->OpenFileForPath(filename))\n");
      sprintf(buffer+strlen(buffer),"            return false;\n");
      sprintf(buffer+strlen(buffer),"         cout << \"\\nThe framework generated a new xml database.\" << endl;\n");
      sprintf(buffer+strlen(buffer),"      }\n");
      sprintf(buffer+strlen(buffer),"      else\n");
      sprintf(buffer+strlen(buffer),"         return false; \n");
      sprintf(buffer+strlen(buffer),"   }; \n");
      for (i=0;i<numOfFolder;i++) {
         if (dataBase[i]) {
            sprintf(buffer+strlen(buffer),"   if (xml->GetPathAttribute(path,\"%sFile\",dbFile)) {;\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"      sprintf(filename,\"%%s%%s\",this->GetDataBaseDir(),dbFile);\n");
            sprintf(buffer+strlen(buffer),"      if (!xml->OpenFileForRead(filename)) { \n");
            sprintf(buffer+strlen(buffer),"         cout << \"Failed to load database : '\" << filename<< \"'\" << endl;\n");
            sprintf(buffer+strlen(buffer),"         return false; \n");
            sprintf(buffer+strlen(buffer),"      };\n");
            sprintf(buffer+strlen(buffer),"      while (xml->NextLine()) {\n");
            sprintf(buffer+strlen(buffer),"         type = xml->GetType();\n");
            sprintf(buffer+strlen(buffer),"         name = xml->GetName();\n");
            sprintf(buffer+strlen(buffer),"         if (type == 1 && !strcmp(name,\"%s\")) {\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"            strcpy(value,\"0\");\n");
            sprintf(buffer+strlen(buffer),"            xml->GetAttribute(\"Number\",value);\n");
            sprintf(buffer+strlen(buffer),"            int num = strtol(value,&cstop,10);\n");
            sprintf(buffer+strlen(buffer),"            while (xml->NextLine()) {\n");
            sprintf(buffer+strlen(buffer),"               type = xml->GetType();\n");
            sprintf(buffer+strlen(buffer),"               name = xml->GetName();\n");
            if (!strcmp(folderArray[i],"1")) {
               for (j=0;j<numOfValue[i];j++) {
                  sprintf(buffer+strlen(buffer),"               if (type == 1 && !strcmp(name,\"%s\")) {\n",valueName[i][j]);
                  sprintf(buffer+strlen(buffer),"                  if (xml->GetValue(value)) {\n");
                  setValue(buf,valueName[i][j],"value",valueType[i][j],1);
                  sprintf(buffer+strlen(buffer),"                     f%sObject->Set%s((%s)%s);\n",folderName[i],valueName[i][j],valueType[i][j],buf);
                  sprintf(buffer+strlen(buffer),"                  };\n");
                  sprintf(buffer+strlen(buffer),"               };\n");
               }
            }
            else {
               for (j=0;j<numOfValue[i];j++) {
                  sprintf(buffer+strlen(buffer),"               if (type == 1 && !strcmp(name,\"%s\")) {\n",valueName[i][j]);
                  sprintf(buffer+strlen(buffer),"                  if (xml->GetValue(value)) {\n");
                  setValue(buf,valueName[i][j],"value",valueType[i][j],1);
                  sprintf(buffer+strlen(buffer),"                     ((%s%s*)f%sObjects->At(num))->Set%s((%s)%s);\n",shortCut,folderName[i],folderName[i],valueName[i][j],valueType[i][j],buf);
                  sprintf(buffer+strlen(buffer),"                  };\n");
                  sprintf(buffer+strlen(buffer),"               };\n");
               }
            }
            sprintf(buffer+strlen(buffer),"               if (type == 15 && !strcmp(name,\"%s\"))\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"                  break;\n");
            sprintf(buffer+strlen(buffer),"            }\n");
            sprintf(buffer+strlen(buffer),"         }\n");
            sprintf(buffer+strlen(buffer),"         if (type == 15 && !strcmp(name,\"%ss\"))\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"            break;\n");
            sprintf(buffer+strlen(buffer),"      }\n");
            sprintf(buffer+strlen(buffer),"   }\n");
         }
      }
      sprintf(buffer+strlen(buffer),"   \n");
      sprintf(buffer+strlen(buffer),"   delete xml;\n");
      sprintf(buffer+strlen(buffer),"   return true;\n");
   } 
   sprintf(buffer+strlen(buffer),"   return true;\n");
   sprintf(buffer+strlen(buffer),"}\n");

   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"void %sIO::Write%sDataBase() {\n",shortCut,folderName[i]);
         sprintf(buffer+strlen(buffer),"   // SQL\n");
         sprintf(buffer+strlen(buffer),"   if (this->isSQLDataBase()) {\n");
         sprintf(buffer+strlen(buffer),"   }\n\n");
         sprintf(buffer+strlen(buffer),"   // XML\n");
         sprintf(buffer+strlen(buffer),"   else if (this->isXMLDataBase()) {\n");
         sprintf(buffer+strlen(buffer),"      char name[200];\n");
         sprintf(buffer+strlen(buffer),"      char value[200];\n");
         sprintf(buffer+strlen(buffer),"      char dbFile[200];\n");
         sprintf(buffer+strlen(buffer),"      char filename[200];\n");
         sprintf(buffer+strlen(buffer),"      char path[200];\n");
         sprintf(buffer+strlen(buffer),"      char runNumberString[6];\n");
         sprintf(buffer+strlen(buffer),"      int n=0,i;\n");
         sprintf(buffer+strlen(buffer),"      ROMEXML *xml = new ROMEXML();\n");
         sprintf(buffer+strlen(buffer),"      this->GetCurrentRunNumberString(runNumberString);\n");
         sprintf(buffer+strlen(buffer),"      sprintf(filename,\"%%s/RunTable.xml\",this->GetInputDir());\n");
         sprintf(buffer+strlen(buffer),"      if (!xml->OpenFileForPath(filename))\n");
         sprintf(buffer+strlen(buffer),"         return;\n");
         sprintf(buffer+strlen(buffer),"      sprintf(path,\"//RunTable/Run_%%s\",runNumberString);\n");
         sprintf(buffer+strlen(buffer),"      if (!xml->ExistPath(path)) {\n");
         sprintf(buffer+strlen(buffer),"         bool exist = true;\n");
         sprintf(buffer+strlen(buffer),"         n = this->GetCurrentRunNumber();\n");
         sprintf(buffer+strlen(buffer),"         while (!xml->ExistPath(path)) {\n");
         sprintf(buffer+strlen(buffer),"            if (n==0) {\n");
         sprintf(buffer+strlen(buffer),"               exist = false;\n");
         sprintf(buffer+strlen(buffer),"               break;\n");
         sprintf(buffer+strlen(buffer),"            }\n");
         sprintf(buffer+strlen(buffer),"            sprintf(path,\"//RunTable/Run_%%05d\",n--);\n");
         sprintf(buffer+strlen(buffer),"         }\n");
         sprintf(buffer+strlen(buffer),"         if (exist) {\n");
         sprintf(buffer+strlen(buffer),"            sprintf(name,\"Run_%%s\",runNumberString);\n");
         sprintf(buffer+strlen(buffer),"            xml->NewPathElement(path,name,NULL);\n");
         sprintf(buffer+strlen(buffer),"         }\n");
         sprintf(buffer+strlen(buffer),"         else {\n");
         sprintf(buffer+strlen(buffer),"            sprintf(name,\"Run_%%s\",runNumberString);\n");
         sprintf(buffer+strlen(buffer),"            xml->NewPathChildElement(\"//RunTable\",name,NULL);\n");
         sprintf(buffer+strlen(buffer),"         }\n");
         sprintf(buffer+strlen(buffer),"         sprintf(path,\"//RunTable/Run_%%s\",runNumberString);\n");
         sprintf(buffer+strlen(buffer),"         sprintf(dbFile,\"db%s%%s_0.xml\",runNumberString);\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"         xml->NewPathAttribute(path,\"%sFile\",dbFile);\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"      }\n");
         sprintf(buffer+strlen(buffer),"      else {\n");
         sprintf(buffer+strlen(buffer),"         if (xml->GetPathAttribute(path,\"%sFile\",dbFile)) {\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"            NextFile(dbFile,dbFile);\n");
         sprintf(buffer+strlen(buffer),"            xml->ReplacePathAttributeValue(path,\"%sFile\",dbFile);\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"         }\n");
         sprintf(buffer+strlen(buffer),"         else {\n");
         sprintf(buffer+strlen(buffer),"            sprintf(dbFile,\"db%s%%s_0.xml\",runNumberString);\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"            xml->NewPathAttribute(path,\"%sFile\",dbFile);\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"         }\n");
         sprintf(buffer+strlen(buffer),"      }\n");
         sprintf(buffer+strlen(buffer),"      xml->WritePathFile(filename);\n");
         sprintf(buffer+strlen(buffer),"      sprintf(filename,\"%%s/%%s\",this->GetDataBaseDir(),dbFile);\n");
         sprintf(buffer+strlen(buffer),"      if (!xml->OpenFileForWrite(filename))\n");
         sprintf(buffer+strlen(buffer),"         return;\n");
         sprintf(buffer+strlen(buffer),"      xml->StartElement(\"%ss\");\n",folderName[i]);
         if (!strcmp(folderArray[i],"1")) {
            sprintf(buffer+strlen(buffer),"      xml->StartElement(\"%s\");\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"      xml->WriteAttribute(\"Number\",\"0\");\n");
            for (j=0;j<numOfValue[i];j++) {
               char format[10];
               GetFormat(format,valueType[i][j]);
               if (!strcmp(valueType[i][j],"TString")) {
                  sprintf(buffer+strlen(buffer),"      sprintf(value,\"%s\",f%sObject->Get%s().Data());\n",format,folderName[i],valueName[i][j]);
               }
               else {
                  sprintf(buffer+strlen(buffer),"      sprintf(value,\"%s\",f%sObject->Get%s());\n",format,folderName[i],valueName[i][j]);
               }
               sprintf(buffer+strlen(buffer),"      xml->WriteElement(\"%s\",value);\n",valueName[i][j]);
            }
            sprintf(buffer+strlen(buffer),"      xml->EndElement();\n");
         }
         else {
            sprintf(buffer+strlen(buffer),"      for (i=0;i<f%sObjects->GetEntries();i++) {\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"         xml->StartElement(\"%s\");\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"         sprintf(value,\"%%d\",i);\n");
            sprintf(buffer+strlen(buffer),"         xml->WriteAttribute(\"Number\",value);\n");
            for (j=0;j<numOfValue[i];j++) {
               char format[10];
               GetFormat(format,valueType[i][j]);
               if (!strcmp(valueType[i][j],"TString")) {
                  sprintf(buffer+strlen(buffer),"         sprintf(value,\"%s\",((%s%s*)f%sObjects->At(i))->Get%s().Data());\n",format,shortCut,folderName[i],folderName[i],valueName[i][j]);
               }
               else {
                  sprintf(buffer+strlen(buffer),"         sprintf(value,\"%s\",((%s%s*)f%sObjects->At(i))->Get%s());\n",format,shortCut,folderName[i],folderName[i],valueName[i][j]);
               }
               sprintf(buffer+strlen(buffer),"         xml->WriteElement(\"%s\",value);\n",valueName[i][j]);
            }
            sprintf(buffer+strlen(buffer),"         xml->EndElement();\n");
            sprintf(buffer+strlen(buffer),"      }\n");
         }
         sprintf(buffer+strlen(buffer),"      xml->EndDocument();\n");
         sprintf(buffer+strlen(buffer),"      delete xml;\n");
         sprintf(buffer+strlen(buffer),"   }\n");
         sprintf(buffer+strlen(buffer),"}\n\n");
      }
   }

   sprintf(buffer+strlen(buffer),"void %sIO::NextFile(char* nextFile,char* file) {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   struct stat buf;\n");
   sprintf(buffer+strlen(buffer),"   char* body = new char[strlen(file)];\n");
   sprintf(buffer+strlen(buffer),"   char* res;\n");
   sprintf(buffer+strlen(buffer),"   int n=0,number=0;\n");
   sprintf(buffer+strlen(buffer),"   if ((res=strstr(file,\"_\"))) {\n");
   sprintf(buffer+strlen(buffer),"      n = res-file;\n");
   sprintf(buffer+strlen(buffer),"      strncpy(body,file,n);\n");
   sprintf(buffer+strlen(buffer),"      body[n] = 0;\n");
   sprintf(buffer+strlen(buffer),"   }\n");
   sprintf(buffer+strlen(buffer),"   else {\n");
   sprintf(buffer+strlen(buffer),"      if ((res=strstr(file,\".\"))) {\n");
   sprintf(buffer+strlen(buffer),"         n = res-file;\n");
   sprintf(buffer+strlen(buffer),"         strncpy(body,file,n);\n");
   sprintf(buffer+strlen(buffer),"         body[n] = 0;\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   sprintf(buffer+strlen(buffer),"      else {\n");
   sprintf(buffer+strlen(buffer),"         strcpy(body,file);\n");
   sprintf(buffer+strlen(buffer),"      }\n");
   sprintf(buffer+strlen(buffer),"   }\n");
   sprintf(buffer+strlen(buffer),"   sprintf(nextFile,\"%%s/%%s_%%d.xml\",this->GetDataBaseDir(),body,number);\n");
   sprintf(buffer+strlen(buffer),"   while (!stat(nextFile,&buf)) {\n");
   sprintf(buffer+strlen(buffer),"      number++;\n");
   sprintf(buffer+strlen(buffer),"      sprintf(nextFile,\"%%s/%%s_%%d.xml\",this->GetDataBaseDir(),body,number);\n");
   sprintf(buffer+strlen(buffer),"   }\n");
   sprintf(buffer+strlen(buffer),"   sprintf(nextFile,\"%%s_%%d.xml\",body,number);\n");
   sprintf(buffer+strlen(buffer),"   delete body;\n");
   sprintf(buffer+strlen(buffer),"}\n\n");



   // Close cpp-File
   sprintf(cppFile,"%s/src/framework/%sIO.cpp",outDir,shortCut);
   fileHandle = open(cppFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<(int)strlen(buffer);i++) {
      if (buffer[i] != fileBuffer[i]) {
         identical = false;
      }
   }
   if (!identical) {
      fileHandle = open(cppFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(cppFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << cppFile << endl;
      nb = write(fileHandle,&buffer, strlen(buffer));
      close(fileHandle);
   }
   return true;
}
bool ROMEBuilder::WriteIOH() {
   int i,j;

   char format[100];
   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   int nameLen = -1;
   int typeLen = 12;
   int scl = strlen(shortCut);
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)strlen(folderName[i])+scl) typeLen = strlen(folderName[i])+scl;
      if (nameLen<(int)strlen(folderName[i])) nameLen = strlen(folderName[i]);
   }

   sprintf(buffer,"////////////////////////////////////////////////////////////////////////////////\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
   sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

   sprintf(buffer+strlen(buffer),"#ifndef %sIO_H\n",shortCut);
   sprintf(buffer+strlen(buffer),"#define %sIO_H\n\n",shortCut);

   sprintf(buffer+strlen(buffer),"#include<ROMEIO.h>\n");
   sprintf(buffer+strlen(buffer),"#include<TClonesArray.h>\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         sprintf(buffer+strlen(buffer),"#include <include/framework/%s%s.h>\n",shortCut,folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

   // bank structures
   sprintf(buffer+strlen(buffer),"   // Bank Structures\n");
   for (i=0;i<numOfBank;i++) {
      if (!strcmp(bankType[i],"structure")||!strcmp(bankType[i],"struct")) {
         sprintf(buffer+strlen(buffer),"typedef struct {\n");
         for (j=0;j<numOfStructFields[i];j++) {
            if (strlen(structFieldSize[i][j])>0)
               sprintf(buffer+strlen(buffer),"   %s %s : %s;\n",structFieldType[i][j],structFieldName[i][j],structFieldSize[i][j]);
            else
               sprintf(buffer+strlen(buffer),"   %s %s;\n",structFieldType[i][j],structFieldName[i][j]);
         }
         sprintf(buffer+strlen(buffer),"} %s;\n",bankStructName[i]);
      }
   }



   // Class

   sprintf(buffer+strlen(buffer),"\nclass %sIO : public ROMEIO\n",shortCut);
   sprintf(buffer+strlen(buffer),"{\n");

   // Fields
   //--------

   // Pointers to Folders
   sprintf(buffer+strlen(buffer),"   // Pointers to Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (strcmp(folderArray[i],"1")) {
            sprintf(format,"   TClonesArray*%%%ds f%%sObjects;%%%ds // Handle to %%s%%s Objects\n",typeLen-12,nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,"",folderName[i],"",shortCut,folderName[i]);
         }
         else {
            sprintf(format,"   %%s%%s*%%%ds f%%sObject; %%%ds // Handle to %%s%%s Object\n",typeLen-strlen(folderName[i])-scl,nameLen-strlen(folderName[i]));
            sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",shortCut,folderName[i]);
         }
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

   // Bank Fields
   if (numOfBank>0) {
      int bankNameLen = -1;
      int bankTypeLen = -1;
      for (i=0;i<numOfBank;i++) {
         if (!strcmp(bankType[i],"structure")||!strcmp(bankType[i],"struct")) {
            if (bankTypeLen<(int)strlen(bankStructName[i])) bankTypeLen = strlen(bankStructName[i]);
         }
         else {
            if (bankTypeLen<(int)strlen(bankType[i])) bankTypeLen = strlen(bankType[i]);
         }
         if (bankNameLen<(int)strlen(bankName[i])) bankNameLen = strlen(bankName[i]);
      }
      sprintf(buffer+strlen(buffer),"   // Bank Fields\n");
      for (i=0;i<numOfBank;i++) {
         if (!strcmp(bankType[i],"structure")||!strcmp(bankType[i],"struct")) {
            sprintf(format,"   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-strlen(bankStructName[i]),bankNameLen-strlen(bankName[i]));
            sprintf(buffer+strlen(buffer),format,bankStructName[i],"",bankName[i],"",bankName[i]);
         }
         else {
            sprintf(format,"   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-strlen(bankType[i]),bankNameLen-strlen(bankName[i]));
            sprintf(buffer+strlen(buffer),format,bankType[i],"",bankName[i],"",bankName[i]);
         }
         sprintf(format,"   int%%%ds f%%sBankLength;  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-2,bankNameLen-strlen(bankName[i]));
         sprintf(buffer+strlen(buffer),format,"",bankName[i],"",bankName[i]);
         sprintf(format,"   bool%%%ds f%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-strlen(bankName[i]));
         sprintf(buffer+strlen(buffer),format,"",bankName[i],"",bankName[i]);
      }
      sprintf(buffer+strlen(buffer),"\n");
   }
 
   sprintf(buffer+strlen(buffer),"\n");
   // Methods

   sprintf(buffer+strlen(buffer),"\npublic:\n");

   // Constructor
   sprintf(buffer+strlen(buffer),"   %sIO(",shortCut);
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (strcmp(folderArray[i],"1")) {
            sprintf(buffer+strlen(buffer),"TClonesArray* %sObjects,",folderName[i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"%s%s* %sObject,",shortCut,folderName[i],folderName[i]);
         }
      }
   }
   sprintf(buffer+strlen(buffer)-1,");\n");
   sprintf(buffer+strlen(buffer),"   ~%sIO();\n",shortCut);

   // Banks
   if (numOfBank>0) {
      sprintf(buffer+strlen(buffer),"   // Bank Methodes\n");
      sprintf(buffer+strlen(buffer),"   void InitMidasBanks();\n");
      for (i=0;i<numOfBank;i++) {
         if (!strcmp(bankType[i],"structure")||!strcmp(bankType[i],"struct")) {
            sprintf(buffer+strlen(buffer),"   %s* Get%sBankAt(int index);\n",bankStructName[i],bankName[i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   %s Get%sBankAt(int index);\n",bankType[i],bankName[i]);
         }
         sprintf(buffer+strlen(buffer),"   int Get%sBankEntries();\n",bankName[i]);
         sprintf(buffer+strlen(buffer),"   void Init%sBank();\n",bankName[i]);
      }
      sprintf(buffer+strlen(buffer),"\n");
   }
   // Data Base
   sprintf(buffer+strlen(buffer),"   // DataBase Methodes\n");
   sprintf(buffer+strlen(buffer),"   bool InitSQLDataBase();\n");
   sprintf(buffer+strlen(buffer),"   bool ReadSQLDataBase();\n");
   sprintf(buffer+strlen(buffer),"   bool ReadXMLDataBase();\n\n");

   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"   void Write%sDataBase();\n",folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

   // Trees
   sprintf(buffer+strlen(buffer),"   // Tree Methodes\n");
   sprintf(buffer+strlen(buffer),"   void ConnectTrees();\n");
   sprintf(buffer+strlen(buffer),"   void FillTrees();\n");
   sprintf(buffer+strlen(buffer),"   void ClearFolders();\n");

   //private
   sprintf(buffer+strlen(buffer),"\nprivate:\n");
   sprintf(buffer+strlen(buffer),"   void NextFile(char* nextFile,char* file);\n");

   // Footer
   //--------
   sprintf(buffer+strlen(buffer),"};\n\n");

   sprintf(buffer+strlen(buffer),"#endif   // %sIO_H\n",shortCut);

   // Close h-File
   //--------------

   sprintf(hFile,"%s/include/framework/%sIO.h",outDir,shortCut);
   fileHandle = open(hFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<(int)strlen(buffer);i++) {
      if (buffer[i] != fileBuffer[i]) {
         identical = false;
      }
   }
   if (!identical) {
      fileHandle = open(hFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(hFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << hFile << endl;
      nb = write(fileHandle,&buffer, strlen(buffer));
      close(fileHandle);
   }
   return true;
}


bool ROMEBuilder::WriteRunTableH() {
   int i;

   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb,ndb=0;
   int fileHandle;

   sprintf(hFile,"%s/include/framework/%sRunTable.h",outDir,shortCut);
   for (i=0;i<numOfFolder;i++) if (dataBase[i]) ndb++;
   if (ndb==0) {
      remove(hFile);
   }

// Header Files
//--------------

   sprintf(buffer,"////////////////////////////////////////////////////////////////////////////////\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// This file has been generated by the ROMEBuilder.                           //\n");
   sprintf(buffer+strlen(buffer),"// If you intend to change this file please contact:                          //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Matthias Schneebeli (PSI), (matthias.schneebeli@psi.ch)                    //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Manual changes to this file will always be overwritten by the builder.     //\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n\n");

   sprintf(buffer+strlen(buffer),"#ifndef %sRunTable_H\n",shortCut);
   sprintf(buffer+strlen(buffer),"#define %sRunTable_H\n\n",shortCut);

   sprintf(buffer+strlen(buffer),"#include <TObject.h>\n");

// Class
//-------

   sprintf(buffer+strlen(buffer),"\nclass %sRunTable : public TObject\n",shortCut);
   sprintf(buffer+strlen(buffer),"{\n");

// Fields
//--------

   sprintf(buffer+strlen(buffer),"protected:\n");

   sprintf(buffer+strlen(buffer),"   int fRunNumber; // Run Number\n");
   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"   int f%s; // Pointer to %s Data Base\n",folderName[i],folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

// Methods
//---------

   sprintf(buffer+strlen(buffer),"\npublic:\n");
   // Constructor and Event Methods
   sprintf(buffer+strlen(buffer),"   %sRunTable():TObject() {};\n\n",shortCut);
   sprintf(buffer+strlen(buffer),"   int GetRunNumber() { return fRunNumber; };\n");
   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"   int Get%s() { return f%s; };\n",folderName[i],folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   void SetRunNumber(int runNumber) { fRunNumber = runNumber; };\n");
   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"   void Set%s(int %s) { f%s = %s; };\n",folderName[i],folderName[i],folderName[i],folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"\n");

// Footer
//--------
   sprintf(buffer+strlen(buffer),"\n   ClassDef(%sRunNumber,1)\n",shortCut);
   sprintf(buffer+strlen(buffer),"};\n\n");

   sprintf(buffer+strlen(buffer),"#endif   // %sRunNumber_H\n",shortCut);

// Write h-File
//--------------

   fileHandle = open(hFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<(int)strlen(buffer);i++) {
      if (buffer[i] != fileBuffer[i]) {
         identical = false;
      }
   }
   if (!identical) {
      fileHandle = open(hFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(hFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << hFile << endl;
      nb = write(fileHandle,&buffer, strlen(buffer));
      close(fileHandle);
   }
   return true;
}

bool ROMEBuilder::WriteMain() {
   int i;

   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   sprintf(buffer,"#include <TApplication.h>\n");
   sprintf(buffer+strlen(buffer),"#include <include/framework/%sAnalyzer.h>\n",shortCut);
   sprintf(buffer+strlen(buffer),"#include <Riostream.h>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"int main(int argc, char *argv[])\n");
   sprintf(buffer+strlen(buffer),"{\n");
   sprintf(buffer+strlen(buffer),"   int argn = 1;\n");
   sprintf(buffer+strlen(buffer),"   char arg[1][100];\n");
   sprintf(buffer+strlen(buffer),"   char *argp = &arg[0][0];\n");
   sprintf(buffer+strlen(buffer),"   strcpy(arg[0],argv[0]);\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   TApplication theApp(\"App\", &argn, &argp);\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   %sAnalyzer *analyzer = new %sAnalyzer();\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   if (!analyzer->Start(argc, argv)) {\n");
   sprintf(buffer+strlen(buffer),"      delete analyzer;\n");
   sprintf(buffer+strlen(buffer),"      return 1;\n");
   sprintf(buffer+strlen(buffer),"   }\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   theApp.Run();\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   delete analyzer;\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   return 0;\n");
   sprintf(buffer+strlen(buffer),"}\n");

   sprintf(hFile,"%s/src/framework/main.cpp",outDir);
   fileHandle = open(hFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<(int)strlen(buffer);i++) {
      if (buffer[i] != fileBuffer[i]) {
         identical = false;
      }
   }
   if (!identical) {
      fileHandle = open(hFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(hFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << hFile << endl;
      nb = write(fileHandle,&buffer, strlen(buffer));
      close(fileHandle);
   }
   return true;
}
int main(int argc, char *argv[])
{
   ROMEBuilder* romeb = new ROMEBuilder();

   strcpy(romeb->romeVersion,"Version 1.00");
   
   char xmlFile[1000];

   xmlFile[0] = 0;
   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);

   romeb->makeOutput = false;
   romeb->noLink = false;
   romeb->offline = false;
   romeb->sql = true;

   char midasFile[200];
   sprintf(midasFile,"MIDASSYS/include/midas.h");
   struct stat buf;
   if( stat( midasFile, &buf )) {
      romeb->offline = true;
   }

   strcpy(romeb->outDir,workDir);
   strcat(romeb->outDir,"/");

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
      else if (!strcmp(argv[i],"-offline")) {
         romeb->offline = true;
      }
      else if (!strcmp(argv[i],"-offline")) {
         romeb->sql = false;
      }
      else if (!strcmp(argv[i],"-i")&&i<argc-1) {
         strcpy(xmlFile,argv[i+1]);
         i++;
      }
      else if (!strcmp(argv[i],"-o")&&i<argc-1) {
         strcpy(romeb->outDir,argv[i+1]);
         if (romeb->outDir[strlen(romeb->outDir)-1]!='/' && romeb->outDir[strlen(romeb->outDir)-1]!='\\') 
            strcat(romeb->outDir,"/");
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
         strcpy(xmlFile,argv[i]);
      }
   }

   if( stat( xmlFile, &buf )) {
      if ( xmlFile[0] == 0)
         cout << "No inputfile specified." << endl;
      else
         cout << "Inputfile '" << xmlFile << "' not found." << endl;
      return 1;
   }
   char path[1000];
   strcpy(path,romeb->outDir);
   path[strlen(path)-1] = 0;
   if (stat( path, &buf )) {
      cout << "Outputpath '" << romeb->outDir << "' not found." << endl;
      return 1;
   }
   sprintf(path,"%s/src",romeb->outDir);
   _mkdir(path);
   sprintf(path,"%s/src/tasks",romeb->outDir);
   _mkdir(path);
   sprintf(path,"%s/src/framework",romeb->outDir);
   _mkdir(path);
   sprintf(path,"%s/include/",romeb->outDir);
   _mkdir(path);
   sprintf(path,"%s/include/tasks",romeb->outDir);
   _mkdir(path);
   sprintf(path,"%s/include/framework",romeb->outDir);
   _mkdir(path);

   romeb->startBuilder(xmlFile);

   return 0;
}
void ROMEBuilder::startBuilder(char* xmlFile) 
{
   xml = new ROMEXML();

   char* name;
   bool finished = false;
   int type;

   strcpy(mainAuthor,"");
   strcpy(mainInstitute,"");
   strcpy(mainCollaboration,"");
   strcpy(mainEmail,"");
   strcpy(mainProgName,"");
      
   if (!xml->OpenFileForRead(xmlFile)) return;
   while (xml->NextLine()&&!finished) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1) {
         strcpy(shortCut,(const char*)name);
         while (xml->NextLine()&&!finished) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 15 && !strcmp((const char*)name,shortCut)) {
               finished = true;
               break;
            }
            if (type == 1) {
               if (!strcmp((const char*)name,"Author")) {
                  xml->GetAttribute("Name",mainAuthor);
                  xml->GetAttribute("Institute",mainInstitute);
                  xml->GetAttribute("Collaboration",mainCollaboration);
                  xml->GetAttribute("Email",mainEmail);
               }
               if (!strcmp((const char*)name,"Programname")) {
                  xml->GetAttribute("Name",mainProgName);
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
   if (!WriteIOCpp()) return;
   if (!WriteIOH()) return;
   if (!WriteRunTableH()) return;
   if (!WriteMain()) return;
   delete xml;

   char buffer1[100];
   char buffer2[10000];
// Dictionary
   if (makeOutput) cout << "\nExecuting 'rootcint' for Root-Dictionary generation." << endl;
   WriteDictionaryBat(buffer1,buffer2);
   chdir(outDir);
   system(buffer1);
   system(buffer2);

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
   char buffer[10000];
   int i;

#if defined( _MSC_VER )
   // libs
   sprintf(buffer,"rootlibs = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libEG.lib $(ROOTSYS)/lib/libEGPythia6.lib $(ROOTSYS)/lib/libFumili.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libGeomPainter.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHbook.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMLP.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libProof.lib $(ROOTSYS)/lib/libProofGui.lib $(ROOTSYS)/lib/libRFIO.lib $(ROOTSYS)/lib/libRGL.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTable.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libVMC.lib $(ROOTSYS)/lib/libWin32gdk.lib $(ROOTSYS)/lib/libfreetype.lib\n");
   sprintf(buffer+strlen(buffer),"xmllibs = $(ROMESYS)/lib_win/libxml2.lib $(ROMESYS)/lib_win/iconv.lib $(ROMESYS)/lib_win/zlib.lib\n");
   sprintf(buffer+strlen(buffer),"sqllibs = $(ROMESYS)/lib_win/libmySQL.lib $(ROMESYS)/lib_win/mysys.lib $(ROMESYS)/lib_win/mysqlclient.lib\n");
   sprintf(buffer+strlen(buffer),"clibs = gdi32.lib user32.lib kernel32.lib\n");
   sprintf(buffer+strlen(buffer),"Libraries = $(rootlibs) $(xmllibs) $(clibs) $(sqllibs)\n");
   sprintf(buffer+strlen(buffer),"\n");
   // flags
   sprintf(buffer+strlen(buffer),"Flags = /GX /GR");
   if (!this->offline) 
      sprintf(buffer+strlen(buffer)," /DHAVE_MIDAS");
   if (this->sql) 
      sprintf(buffer+strlen(buffer)," /DHAVE_SQL");
   sprintf(buffer+strlen(buffer),"\n");
   // includes
   sprintf(buffer+strlen(buffer),"Includes = /I$(ROMESYS)/include/ /I$(ROOTSYS)/include/ /I. /Iinclude/ /Iinclude/tasks/ /Iinclude/framework/ ");
   if (!this->offline) 
      sprintf(buffer+strlen(buffer)," /I$(MIDASSYS)/include/");
   if (this->sql) 
      sprintf(buffer+strlen(buffer)," /I$(ROMESYS)/include/mysql/");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"\n");
   // objects
   sprintf(buffer+strlen(buffer),"objects =");
   for (i=0;i<numOfFolder;i++) {
      if (numOfGetters[i]==0) continue;
      sprintf(buffer+strlen(buffer)," obj/%s%s.obj",shortCut,folderName[i]);
   }
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer+strlen(buffer)," obj/%sT%s.obj",shortCut,taskName[i]);
   }
   sprintf(buffer+strlen(buffer)," obj/%sAnalyzer.obj obj/%sIO.obj obj/%sDict.obj obj/main.obj",shortCut,shortCut,shortCut);
   sprintf(buffer+strlen(buffer)," obj/ROMEAnalyzer.obj obj/ROMEEventLoop.obj obj/ROMEIO.obj obj/ROMETask.obj obj/ROMESQL.obj obj/ROMESplashScreen.obj obj/ROMEXML.obj\n\n");
   // all
   sprintf(buffer+strlen(buffer),"all:obj %s%s.exe\n",shortCut,mainProgName);
   // make obj
   sprintf(buffer+strlen(buffer),"obj:\n");
	sprintf(buffer+strlen(buffer),"\tif not exist obj mkdir obj\n");
   // link
   sprintf(buffer+strlen(buffer),"%s%s.exe: $(objects)\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"	cl /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut,mainProgName);
   // compile
   for (i=0;i<numOfFolder;i++) {
      if (numOfGetters[i]==0) continue;
      sprintf(buffer+strlen(buffer),"obj/%s%s.obj: src/framework/%s%s.cpp\n",shortCut,folderName[i],shortCut,folderName[i]);
      sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/%s%s.obj src/framework/%s%s.cpp \n",shortCut,folderName[i],shortCut,folderName[i]);
   }
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer+strlen(buffer),"obj/%sT%s.obj: src/tasks/%sT%s.cpp\n",shortCut,taskName[i],shortCut,taskName[i]);
      sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/%sT%s.obj src/tasks/%sT%s.cpp \n",shortCut,taskName[i],shortCut,taskName[i]);
   }
   sprintf(buffer+strlen(buffer),"obj/%sAnalyzer.obj: src/framework/%sAnalyzer.cpp\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/%sAnalyzer.obj src/framework/%sAnalyzer.cpp \n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"obj/%sIO.obj: src/framework/%sIO.cpp\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/%sIO.obj src/framework/%sIO.cpp \n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"obj/%sDict.obj: %sDict.cpp\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/%sDict.obj %sDict.cpp \n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"obj/main.obj: src/framework/main.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/main.obj src/framework/main.cpp \n");

   sprintf(buffer+strlen(buffer),"obj/ROMEAnalyzer.obj: $(ROMESYS)/src/ROMEAnalyzer.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/ROMEAnalyzer.obj $(ROMESYS)/src/ROMEAnalyzer.cpp \n");
   sprintf(buffer+strlen(buffer),"obj/ROMEEventLoop.obj: $(ROMESYS)/src/ROMEEventLoop.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/ROMEEventLoop.obj $(ROMESYS)/src/ROMEEventLoop.cpp \n");
   sprintf(buffer+strlen(buffer),"obj/ROMEIO.obj: $(ROMESYS)/src/ROMEIO.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/ROMEIO.obj $(ROMESYS)/src/ROMEIO.cpp \n");
   sprintf(buffer+strlen(buffer),"obj/ROMETask.obj: $(ROMESYS)/src/ROMETask.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/ROMETask.obj $(ROMESYS)/src/ROMETask.cpp \n");
   sprintf(buffer+strlen(buffer),"obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/ROMESQL.obj $(ROMESYS)/src/ROMESQL.cpp \n");
   sprintf(buffer+strlen(buffer),"obj/ROMESplashScreen.obj: $(ROMESYS)/src/ROMESplashScreen.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/ROMESplashScreen.obj $(ROMESYS)/src/ROMESplashScreen.cpp \n");
   sprintf(buffer+strlen(buffer),"obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp\n");
   sprintf(buffer+strlen(buffer),"	cl $(Flags) $(Includes) /c /Foobj/ROMEXML.obj $(ROMESYS)/src/ROMEXML.cpp \n");

#endif

#if defined ( __linux__ )
   char shortcut[20];
   char mainprogname[20];
   sprintf(buffer,"ROOTLIBS := $(shell root-config --libs)\n");
   sprintf(buffer+strlen(buffer),"ROOTGLIBS := $(shell root-config --glibs)\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"cfiles := $(wildcard *.cpp)\n");
   sprintf(buffer+strlen(buffer),"romecfiles := $(wildcard $(ROMESYS)/src/ROME*.cpp)\n");
   sprintf(buffer+strlen(buffer),"hfiles := $(wildcard *.h)\n");
   sprintf(buffer+strlen(buffer),"romehfiles := $(wildcard $(ROMESYS)/include/ROME*.h)\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"flags = ");
   if (!this->offline) 
      sprintf(buffer+strlen(buffer)," -DHAVE_MIDAS");
   if (this->sql) 
      sprintf(buffer+strlen(buffer)," -DHAVE_SQL");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"\n");
   for (i=0;i<strlen(shortCut);i++) shortcut[i] = tolower(shortCut[i]);
   shortcut[i] = 0;
   for (i=0;i<strlen(mainProgName);i++) mainprogname[i] = tolower(mainProgName[i]);
   mainprogname[i] = 0;
   sprintf(buffer+strlen(buffer),"%s%s: $(cfiles) $(romecfiles) $(hfiles) $(romehfiles)\n",shortcut,mainprogname);
   sprintf(buffer+strlen(buffer),"	g++ -o $@ $(cfiles) $(romecfiles) $(flags) -I$(ROOTSYS)/include/ -I$(ROMESYS)/include -I$(ROMESYS)/include/mysql/ -lmysql -lxml2 -lz -lpthread -lmidas -lHtml $(ROOTLIBS) $(SYSLIBS)\n");
#endif
   char makeFile[500];
#if defined ( __linux__ )
   sprintf(makeFile,"Makefile");
#endif
#if defined ( _MSC_VER )
   sprintf(makeFile,"Makefile.win");
#endif
   int fileHandle = open(makeFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(makeFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer, strlen(buffer));
   close(fileHandle);
}

void ROMEBuilder::WriteDictionaryBat(char* buffer1,char* buffer2) 
{
   // writes a script file that executes rootcint
   int i;

#if defined( _MSC_VER )
   sprintf(buffer1,"@del %sDict.cpp > nul\n",shortCut);
#endif
#if defined ( __linux__ )
   sprintf(buffer1,"rm %sDict.cpp\n",shortCut);
#endif
   sprintf(buffer2,"rootcint %sDict.cpp -c ",shortCut);
#if defined( _MSC_VER )
   sprintf(buffer2+strlen(buffer2),"-I%%ROMESYS%%/include ");
   sprintf(buffer2+strlen(buffer2),"-I%%ROOTSYS%% ");
#endif
#if defined ( __linux__ )
   sprintf(buffer2+strlen(buffer2),"-I$ROMESYS/include ");
   sprintf(buffer2+strlen(buffer2),"-I$ROOTSYS ");
#endif
   sprintf(buffer2+strlen(buffer2),"-Iinclude -Iinclude/tasks -Iinclude/framework ");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) sprintf(buffer2+strlen(buffer2),"include/framework/%s%s.h ",shortCut,folderName[i]);
   }
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer2+strlen(buffer2),"include/tasks/%sT%s.h ",shortCut,taskName[i]);
   }
   sprintf(buffer2+strlen(buffer2),"ROMETask.h ROMETreeInfo.h\n");
   strcat(buffer2,"\0");

#if defined( _MSC_VER )
   char batFile[500];
   sprintf(batFile,"%smakeDictionary.bat",outDir);
   int fileHandle = open(batFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(batFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer1, strlen(buffer1));
   write(fileHandle,buffer2, strlen(buffer2));
   close(fileHandle);
#endif
}

void ROMEBuilder::WriteHTMLDoku() {

   int i=0,j=0,k=0;
   char buffer[100000];
   char htmlFile[500];
   char parentt[100] = "";
   int depthold=0;
   int depth=0;

// Header
//--------
   sprintf(buffer,"<HTML>\n");
   sprintf(buffer+strlen(buffer),"<HEAD>\n");
   sprintf(buffer+strlen(buffer),"<TITLE>%s%s Manual</TITLE>\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"</HEAD>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<H1>%s%s Manual</H1>\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"\n");
   // Table of Contents
   sprintf(buffer+strlen(buffer),"<H2>Table of Contents</H2>\n");
   sprintf(buffer+strlen(buffer),"<ul>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#introduction\">Introduction</a></li>\n");
   sprintf(buffer+strlen(buffer),"<br>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#objects\">Objects in the %s%s</a></li>\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"<ul>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#taskobjects\">Tasks</a></li>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#folderobjects\">Folders</a></li>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#treeobjects\">Trees</a></li>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#midasbankobjects\">Midas Banks</a></li>\n");
   sprintf(buffer+strlen(buffer),"<br>\n");
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"<ul>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#foldermethods\">Folders</a></li>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#databasemethods\">Data Base</a></li>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#midasbankmethods\">Midas Banks</a></li>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#histogrammethods\">Histograms</a></li>\n");
   sprintf(buffer+strlen(buffer),"<li><a href=\"#generalmethods\">General</a></li>\n");
   sprintf(buffer+strlen(buffer),"<br>\n");
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"<li><A TARGET=_top HREF=\"%s/htmldoc/ClassIndex.html\">Class Overview</A></li>\n",outDir);
   sprintf(buffer+strlen(buffer),"<br>\n");
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");
   // Introduction
   sprintf(buffer+strlen(buffer),"<H2><a name=introduction>Introduction</a> </H2>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The %s%s consists mainly of folders and tasks.\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"Folders are objects, where you can store data in. Typically you will store the data of one detector (or subdetector) component in it.\n");
   sprintf(buffer+strlen(buffer),"Like disk folders (directories) they are hierarchically arranged.\n");
   sprintf(buffer+strlen(buffer),"Folders may have a data structure (unlike disk folders). The data objects are called fields. Folders without fields can be used to structure the project.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"Tasks are objects, which privides actions. They make calculations, store and read data in folders, fill trees and histograms and so on.\n");
   sprintf(buffer+strlen(buffer),"Tasks are also hierarchically arranged. That means that a task may have a subtask, which is executed after the task itself has been executed.\n");
   sprintf(buffer+strlen(buffer),"Task also own histograms, which means that all histograms in this frame work belong to a task. The booking and saving of histograms is made by the frame work.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   // Objects
   sprintf(buffer+strlen(buffer),"<H2><a name=objects>Objects in the %s%s</a> </H2>\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"All <a href=\"#taskobjects\">Tasks</a>, <a href=\"#folderobjects\">Folders</a> and <a href=\"#treeobjects\">Trees</a> are described here.\n");
   // Tasks
   sprintf(buffer+strlen(buffer),"<h3><a name=taskobjects>Tasks</a></h3>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The %s%s consists of the following tasks :\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"\n");

   depthold=0;
   depth=0;
   for (i=0;i<numOfTask;i++) {
      depth=0;
      if (strcmp(parentTaskName[i],"GetMainTask()")) {
         depth++;
         strcpy(parentt,parentTaskName[i]);
         for (j=0;j<100;j++) {
            for (k=0;k<numOfTask;k++) {
               if (!strcmp(parentt,taskName[k])) break;
            }
            if (k>=numOfTask) {
               cout << "Invalid task structure." << endl;
               return;
            }
            if (!strcmp(parentTaskName[k],"GetMainTask()")) break;
            depth++;
         }
      }
      if (depth<depthold) sprintf(buffer+strlen(buffer),"</ul>\n");
      if (depth>depthold) sprintf(buffer+strlen(buffer),"<ul>\n");
      sprintf(buffer+strlen(buffer),"<li type=\"circle\"><h4><a href=\"#%s\">%sT%s</a></h4></li>\n",taskName[i],shortCut,taskName[i]);
      depthold = depth;
   }
   for (i=0;i<depth;i++) sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<p><b>Note</b> : The user should write code into the Init(), BeginOfRun(), Event(), EndOfRun() and Terminate() methods of the tasks methods file (.cpp). But the user may not add code to or alter code in the class file (.h). The class file (.h) is generated by the framework. Manual changes to this file will allways be overwritten.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"In the following all tasks will be described.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   for (i=0;i<numOfTask;i++) {
      sprintf(buffer+strlen(buffer),"<h4><a name=%s><u>%s</u></a></h4>\n",taskName[i],taskName[i]);
      sprintf(buffer+strlen(buffer),"%s<p>\n",taskDescription[i]);
      if (numOfHistos[i]>0) {
         sprintf(buffer+strlen(buffer),"This task containes the following histograms :\n");
         sprintf(buffer+strlen(buffer),"<ul>\n");
         for (j=0;j<numOfHistos[i];j++) {
            sprintf(buffer+strlen(buffer),"<li type=\"circle\">%s</li>\n",histoName[i][j]);
         }
         sprintf(buffer+strlen(buffer),"</ul>\n");
         sprintf(buffer+strlen(buffer),"\n");
         sprintf(buffer+strlen(buffer),"\n");
         sprintf(buffer+strlen(buffer),"\n");
         sprintf(buffer+strlen(buffer),"\n");
      }
      else {
         sprintf(buffer+strlen(buffer),"This task containes no histograms.\n");
      }
      sprintf(buffer+strlen(buffer),"<p>\n");
      sprintf(buffer+strlen(buffer),"For more information take a look at the <A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">class file</a>\n",outDir,shortCut,taskName[i]);
      sprintf(buffer+strlen(buffer),"<p>\n");
      sprintf(buffer+strlen(buffer),"\n");
   }
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   // Folders
   sprintf(buffer+strlen(buffer),"<h3><a name=folderobjects>Folders</a></h3>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The %s%s incorporates the following folders :\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<ul>\n");

   depthold=0;
   depth=0;
   for (i=0;i<numOfFolder;i++) {
      depth=0;
      if (strcmp(parentFolderName[i],"GetMainFolder()")) {
         depth++;
         strcpy(parentt,parentFolderName[i]);
         for (j=0;j<100;j++) {
            for (k=0;k<numOfFolder;k++) {
               if (!strcmp(parentt,folderName[k])) break;
            }
            if (k>=numOfFolder) {
               cout << "Invalid folder structure." << endl;
               return;
            }
            if (!strcmp(parentFolderName[k],"GetMainFolder()")) break;
            depth++;
         }
      }
      if (depth<depthold) sprintf(buffer+strlen(buffer),"</ul>\n");
      if (depth>depthold) sprintf(buffer+strlen(buffer),"<ul>\n");
      if (numOfValue[i] > 0) {
         sprintf(buffer+strlen(buffer),"<b>\n");
         sprintf(buffer+strlen(buffer),"<li type=\"circle\"><a href=\"#%s\">%s</a></li>\n",folderName[i],folderName[i]);
         sprintf(buffer+strlen(buffer),"</b>\n");
      }
      else {
         sprintf(buffer+strlen(buffer),"<li type=\"circle\">%s</li>\n",folderName[i]);
      }
      depthold = depth;
   }
   for (i=0;i<depth;i++) sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"The bold folders are data folders. The others are only used to structure the frame work.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"In the following all folders will be described.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] <= 0) continue;
      sprintf(buffer+strlen(buffer),"<h4><a name=%s><u>%s</u></a></h4>\n",folderName[i],folderName[i]);
      sprintf(buffer+strlen(buffer),"%s\n",folderDescription[i]);
      sprintf(buffer+strlen(buffer),"<p>\n");
      sprintf(buffer+strlen(buffer),"<u>Fields</u>\n");
      sprintf(buffer+strlen(buffer),"<table border=\"1\">\n");
      sprintf(buffer+strlen(buffer),"<tr><td>Name</td><td>Type</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         sprintf(buffer+strlen(buffer),"<tr><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",valueName[i][j],valueType[i][j]);
      }
      sprintf(buffer+strlen(buffer),"</table>\n");

   }
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   // Trees
   sprintf(buffer+strlen(buffer),"<h3><a name=treeobjects>Trees</a></h3>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The %s%s incorporates the following trees :\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<ul>\n");
   for (i=0;i<numOfTree;i++) {
      sprintf(buffer+strlen(buffer),"<li type=\"disc\">%s</li>\n",treeName[i]);
   }
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   // Midas Banks
   sprintf(buffer+strlen(buffer),"<h3><a name=midasbankobjects>Midas Banks</a></h3>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The %s%s incorporates the following midas banks :\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<ul>\n");
   for (i=0;i<numOfBank;i++) {
      if (!strcmp(bankType[i],"structure")||!strcmp(bankType[i],"struct")) {
         sprintf(buffer+strlen(buffer),"<li type=\"disc\">%s</li>\n",bankName[i]);
         sprintf(buffer+strlen(buffer),"<ul>\n");
         for (j=0;j<numOfStructFields[i];j++) {
            sprintf(buffer+strlen(buffer),"<li type=\"disc\">%s</li>\n",structFieldName[i][j]);
         }
         sprintf(buffer+strlen(buffer),"</ul>\n");
      }
      else {
         sprintf(buffer+strlen(buffer),"<li type=\"disc\">%s</li>\n",bankName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   // Access Methods
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"In the following the access methods of all types of objects in the %s%s are discussed.\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"All task can access these methods over their <b>fAnalyzer</b> handle, which every task has by\n");
   sprintf(buffer+strlen(buffer),"default.<br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<h3><a name=foldermethods>Folders</a></h3>\n");
   sprintf(buffer+strlen(buffer),"To access a folder one has to get a handle to it with the following methods : <p>\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Folder Name</i>]At([<i>Index</i>])</b></td><td>&nbsp;&nbsp;&nbsp;for object arrays.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Folder Name</i>]Object()</td><td>&nbsp;&nbsp;&nbsp;for single objects.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Folder Name</i></b> stands for the name of the folder specified in the xml file (see also list above).</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Index</i></b> stands for the array index of the object.</br>\n");
   sprintf(buffer+strlen(buffer),"These methods return a pointer on the class of this folder.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"The get or set the content of a folder one has to use the following methods : <p>\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Field Name</i>]()</b></td><td>&nbsp;&nbsp;&nbsp;getter.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Set[<i>Field Name</i>]([<i>Value</i>])</td><td>&nbsp;&nbsp;&nbsp;setter.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Field Name</i></b> stands for the name of the field to access specified in the xml file (see also list above).</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Value</i></b> stands for the value given to the Field.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b>Example:</b> To give the field 'YY' of the 9 folder of a folderarray 'XX' the value 99 one has to type this :<p>\n");
   sprintf(buffer+strlen(buffer),"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fAnalyzer->GetXXAt(9)->SetYY(99) <p>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   // Data Base
   sprintf(buffer+strlen(buffer),"<h3><a name=databasemethods><u>Data Base</u></a></h3>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"To add an entry to the data base the following methods are available :\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<b>Write[<i>Folder Name</i>]DataBase(this)</b>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Folder Name</i></b> stands for the name of the folder, which is to be written to the data base (see also list above).</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");

   // Midas Banks
   sprintf(buffer+strlen(buffer),"<h3><a name=midasbankmethods><u>Midas Banks</u></a></h3>\n");
   sprintf(buffer+strlen(buffer),"To access a bank in a midas input file the following methods are available :\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"One can get the entries in a bank with the following method :\n");
   sprintf(buffer+strlen(buffer),"<b>Get[<i>Bank Name</i>]BankEntries()</b>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The data can be accessed with :\n");
   sprintf(buffer+strlen(buffer),"<b>Get[<i>Bank Name</i>]BankAt([<i>Index</i>])</b>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Bank Name</i></b> stands for the name of the bank.</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Index</i></b> stands for the arrayindex of the value.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"If the bank is a structured bank, the data access method returns a pointer on the structure.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b>Example:</b> To get the 9 value 'YY' of a structured bank 'XX' one has to type this :<p>\n");
   sprintf(buffer+strlen(buffer),"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fAnalyzer->GetXXBankAt(9)->YY <p>\n");
   sprintf(buffer+strlen(buffer),"\n");

   // Histos
   sprintf(buffer+strlen(buffer),"<h3><a name=histogrammethods><u>Histograms</u></a></h3>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"Histograms belong to a task. So a task can only access his own histograms. To do this the following methods are available :\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Fill[<i>Histo Name</i>]([<i>xValue</i>],[<i>weight</i>])</b></td>  <td>&nbsp;&nbsp;&nbsp;fills a single histogram.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Draw[<i>Histo Name</i>]()</b></td>       <td>&nbsp;&nbsp;&nbsp;draws a single histogram.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Histo Name</i>]Handle()</b></td>  <td>&nbsp;&nbsp;&nbsp;gets the handle to a single histogram.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Fill[<i>Histo Name</i>]At([<i>Index</i>],[<i>xValue</i>],[<i>weight</i>])</b></td><td>&nbsp;&nbsp;&nbsp;fills a histogram of a histogram array.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Draw[<i>Histo Name</i>]At([<i>Index</i>])</b></td>     <td>&nbsp;&nbsp;&nbsp;draws a histogram of a histogram array.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Histo Name</i>]HandleAt([<i>Index</i>])</b></td><td>&nbsp;&nbsp;&nbsp;gets the handle to a histogram of a histogram array.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Histo Name</i></b> stands for the name of the histogram.</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Index</i></b> stands for the array index of the histogram.</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>xValue</i></b> value to be filled to the histogram.</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>weight</i></b> weight of the value.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");

   // General Methods
   sprintf(buffer+strlen(buffer),"<h3><a name=generalmethods><u>General</u></a></h3>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"%s%s provides some general methods for the user.\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isOnline()</td><td> : true if the program is running online.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isOffline()</td><td> : true if the program is running offline.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isRoot()</td><td> : true if the data is read from a root file.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isMidas()</td><td> : true if the data has the midas format.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isBatchMode()</td><td> : true if the program is running in batch mode.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isEndOfRun()</td><td> : true if the EndOfRun flag is set.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isBeginOfRun()</td><td> : true if the BeginOfRun flag is set.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>isTerminate()</td><td> : true if the Terminate flag is set.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>EndOfRun()</td><td> : sets the EndOfRun flag.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>BeginOfRun()</td><td> : sets the BeginOfRun flag.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>Terminate()</td><td> : sets the Terminate flag.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>GetConfigDir()</td><td> : returns the configuration directory.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>GetDataBaseDir()</td><td> : returns the data base directory.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>GetInputDir()</td><td> : returns the input directory.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>GetOutputDir()</td><td> : returns the output directory.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>GetCurrentRunNumber()</td><td> : returns the current run number.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");

// Footer
//--------
   sprintf(buffer+strlen(buffer),"<HR>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<ADDRESS>\n");
   sprintf(buffer+strlen(buffer),"<u> Contact person from the %s Experiment</u></br>\n",shortCut);
   sprintf(buffer+strlen(buffer),"%s</br>\n",mainAuthor);
   sprintf(buffer+strlen(buffer),"%s</br>\n",mainInstitute);
   sprintf(buffer+strlen(buffer),"%s</br>\n",mainCollaboration);
   sprintf(buffer+strlen(buffer),"email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail,mainEmail);
   sprintf(buffer+strlen(buffer),"<u> Contact person from ROME</u></br>\n");
   sprintf(buffer+strlen(buffer),"Matthias Schneebeli (PSI)</br>\n");
   sprintf(buffer+strlen(buffer),"email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
   sprintf(buffer+strlen(buffer),"</ADDRESS>\n");
   sprintf(buffer+strlen(buffer),"</BODY>\n");
   sprintf(buffer+strlen(buffer),"</HTML>\n");

   sprintf(htmlFile,"%s%s%s.html",outDir,shortCut,mainProgName);
   int fileHandle = open(htmlFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(htmlFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer, strlen(buffer));
   close(fileHandle);
   if (makeOutput) cout << htmlFile << endl;
}

void ROMEBuilder::GetFormat(char *buf,char *type) 
{
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
      strcpy(buf,"%d");
   }
   else if (!strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      strcpy(buf,"%s");
   }
   else if (!strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      strcpy(buf,"%f");
   }
   else if (!strcmp(type,"bool") ||
      !strcmp(type,"Bool_t")) {
      strcpy(buf,"%d");
   }
   else {
      strcpy(buf,"%s");
   }
}

void ROMEBuilder::setValue(char *buf,char *destination,char *source,char *type,int version)
{
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
         sprintf(buf,"%s = strtol(%s,&cstop,10)",destination,source);
      else
         sprintf(buf,"strtol(%s,&cstop,10)",source);
   }
   else if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      if (version==0)
         sprintf(buf,"%s = toBool(strtol(%s,&cstop,10))",destination,source);
      else
         sprintf(buf,"toBool(strtol(%s,&cstop,10))",source);
   }
   else if (
       !strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      if (version==0)
         sprintf(buf,"strcpy(%s,%s)",destination,source);
      else
         sprintf(buf,"%s",source);
   }
   else if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      if (version==0)
         sprintf(buf,"%s = strtod(%s,&cstop)",destination,source);
      else
         sprintf(buf,"strtod(%s,&cstop)",source);
   }
   else {
      if (version==0)
         sprintf(buf,"%s = %s",destination,source);
      else
         sprintf(buf,"%s",source);
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
