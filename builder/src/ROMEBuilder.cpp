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
#include <libxml/xmlreader.h>

//MEGFrameWork.xml -o C:/Data/analysis/MEG/ROME/MEGFrameWork/ -v -nl
//MEGDriftChamber.xml -o C:/Data/analysis/MEG/ROME/MEGDriftChamber/ -v -nl
//MEGTest.xml -o C:/Data/analysis/MEG/ROME/MEGTest/ -v -nl

#include "ROMEBuilder.h"

//============================================================
//                     Folders
//============================================================

bool ROMEBuilder::XMLToFolder(xmlTextReaderPtr reader) {
   char parent[10][100];
   const xmlChar *name, *value;
   bool finished = false;
   int type,i,isub=0;
   strcpy(parent[0],"GetMainFolder()");

// read XML file
//===============

   if (makeOutput) cout << "Folders:" << endl;
   while (xmlTextReaderRead(reader)) {
      type = xmlTextReaderNodeType(reader);
      name = xmlTextReaderConstName(reader);
      // read subfolder
      if (type == 1 && !strcmp((const char*)name,"SubFolder")) {
         numOfFolder++;
         // default initialisation
         strcpy(version[numOfFolder],"1");
         strcpy(author[numOfFolder],"");
         strcpy(classDescription[numOfFolder],"");
         numOfValue[numOfFolder] = 0;
         numOfGetters[numOfFolder] = 0;
         numOfSetters[numOfFolder] = 0;
         numOfInclude[numOfFolder] = 0;
         // set parent
         strcpy(parentFolderName[numOfFolder],parent[isub]);
         // read folder name
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"FolderName");
         if (value!=NULL) strcpy(folderName[numOfFolder],(const char*)value);
         else {
            cout << "Folder " << numOfFolder << " has no name defined !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         xmlFree((void*)value);
         // read folder title
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"FolderTitle");
         if (value!=NULL) strcpy(folderTitle[numOfFolder],(const char*)value);
         else strcpy(folderTitle[numOfFolder],"");
         xmlFree((void*)value);
         // read array flag
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Array");
         if (value!=NULL) strcpy(folderArray[numOfFolder],(const char*)value);
         else strcpy(folderArray[numOfFolder],"no");
         xmlFree((void*)value);
         // read data base flag
         dataBase[numOfFolder] = false;
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"DataBase");
         if (value!=NULL && !strcmp((char*)value,"yes")) dataBase[numOfFolder] = true;
         else dataBase[numOfFolder] = false;
         xmlFree((void*)value);

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
            value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Name");
            if (value != NULL) strcpy(author[numOfFolder],(const char*)value);
            xmlFree((void*)value);
         }
         // read version
         else if (!strcmp((const char*)name,"Version")) {
            value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Number");
            strcpy(version[numOfFolder],(const char*)value);
            xmlFree((void*)value);
         }
         // read description
         else if (!strcmp((const char*)name,"ClassDescription")) {
            value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Text");
            strcpy(classDescription[numOfFolder],(const char*)value);
            xmlFree((void*)value);
         }
         // read additional getters
         else if (!strcmp((const char*)name,"AdditionalGetters")) {
            numOfGetters[numOfFolder] = 0;
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  strcpy(getter[numOfFolder][numOfGetters[numOfFolder]],(const char*)name);
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Type");
                  strcpy(getterType[numOfFolder][numOfGetters[numOfFolder]],(const char*)value);
                  xmlFree((void*)value);
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
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  strcpy(setter[numOfFolder][numOfSetters[numOfFolder]],(const char*)name);
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Type");
                  strcpy(setterType[numOfFolder][numOfSetters[numOfFolder]],(const char*)value);
                  xmlFree((void*)value);
                  numOfSetters[numOfFolder]++;
               }
               if (type == 15 && !strcmp((const char*)name,"AdditionalSetters")) {
                  break;
               }
            }
         }
         // read includes
         else if (!strcmp((const char*)name,"Include")) {
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  strcpy(include[numOfFolder][numOfInclude[numOfFolder]],(const char*)name);
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Type");
                  localFlag[numOfFolder][numOfInclude[numOfFolder]] = false;
                  if (value!=NULL && !strcmp((char*)value,"local")) localFlag[numOfFolder][numOfInclude[numOfFolder]] = true;
                  xmlFree((void*)value);
                  numOfInclude[numOfFolder]++;
               }
               if (type == 15 && !strcmp((const char*)name,"Include")) {
                  break;
               }
            }
         }
         // read fields
         else if (!strcmp((const char*)name,"Fields")) {
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  strcpy(valueName[numOfFolder][numOfValue[numOfFolder]],(const char*)name);
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Type");
                  if (value!=NULL) strcpy(valueType[numOfFolder][numOfValue[numOfFolder]],(const char*)value);
                  xmlFree((void*)value);
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Reference");
                  if (value!=NULL) strcpy(valueRef[numOfFolder][numOfValue[numOfFolder]],(const char*)value);
                  xmlFree((void*)value);
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Initialisation");
                  if (value!=NULL) strcpy(valueInit[numOfFolder][numOfValue[numOfFolder]],(const char*)value);
                  xmlFree((void*)value);
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Comment");
                  if (value!=NULL) strcpy(valueComment[numOfFolder][numOfValue[numOfFolder]],(const char*)value);
                  xmlFree((void*)value);
                  numOfValue[numOfFolder]++;
               }
               if (type == 15 && !strcmp((const char*)name,"Fields")) {
                  break;
               }
            }
         }
      }
      if (type == 15 && !strcmp((const char*)name,"Folder")) break;
   }
   numOfFolder++;

// write source files
//====================

   char cppFile[500];
   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb,lenTot,j,ll;
   const int bufferSize = 600;
   char str[bufferSize];
   char tmp[bufferSize];
   char *pos;
   int fileHandle;
   char format[100];
   bool writeFile = false;
   char *pBuffer;
   int bufferLen=0;



   if (makeOutput) cout << "\n   Output Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (numOfValue[iFold] == 0) continue;
// cpp-File
//----------

      sprintf(cppFile,"%s%s%s.cpp",outDir,shortCut,folderName[iFold]);
// Description
//-------------
      sprintf(buffer,"//// Author: %s\n",author[iFold]);
      sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n");
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      ll = 74-strlen(shortCut);
      sprintf(format,"// %%s%%-%d.%ds //\n",ll,ll);
      sprintf(buffer+strlen(buffer),format,shortCut,folderName[iFold]);
      sprintf(buffer+strlen(buffer),"//                                                                            //\n");
      pos = classDescription[iFold];
      lenTot = strlen(classDescription[iFold]);
      while (pos-classDescription[iFold] < lenTot) {
         if (lenTot+(classDescription[iFold]-pos)<74) i=75;
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

         sprintf(buffer+strlen(buffer),"#include %c%s%s.h%c\n",34,shortCut,folderName[iFold],34);

         sprintf(buffer+strlen(buffer),"\nClassImp(%s%s)\n",shortCut,folderName[iFold]);

// User Functions
//----------------

         for (i=0;i<numOfGetters[iFold];i++) {
            sprintf(buffer+strlen(buffer),"\n%s %s%s::%s()\n{\n}\n\n",getterType[iFold][i],shortCut,folderName[iFold],getter[iFold][i]);
         }
         writeFile = true;
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
      if (writeFile) {
         // write file
         fileHandle = open(cppFile,O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(cppFile,O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << cppFile << endl;

         nb = write(fileHandle,&buffer, strlen(buffer));

         close(fileHandle);
      }

// h-File
//--------

// Description
//-------------
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
//--------------
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
               sprintf(buffer+strlen(buffer),"#include \"%s%s.h\"\n",shortCut,folderName[j]);

               strcpy(str,shortCut);
               strcat(str,valueType[iFold][i]);
               strcpy(valueType[iFold][i],str);
               continue;
            }
         }
      }


// Class
//-------

      sprintf(buffer+strlen(buffer),"\nclass %s%s : public TObject\n",shortCut,folderName[iFold]);
      sprintf(buffer+strlen(buffer),"{\n");

// Fields
//--------

      sprintf(buffer+strlen(buffer),"protected:\n");
      int typeLen = -1;
      int nameLen = -1;
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

// Methods
//---------

      sprintf(buffer+strlen(buffer),"public:\n");
      // Constructor
      sprintf(str,"   %s%s(",shortCut,folderName[iFold]);
      for (i=0;i<numOfValue[iFold];i++) {
         sprintf(tmp,"%s %s=%s",valueType[iFold][i],valueName[iFold][i],valueInit[iFold][i]);
         strcat(str,tmp);
         if (i<numOfValue[iFold]-1) strcat(str,",");
      }
      strcat(str,")\n");
      strcat(buffer,str);
      sprintf(str,"   { ");
      for (i=0;i<numOfValue[iFold];i++) {
         sprintf(tmp,"f%s = %s; ",valueName[iFold][i],valueName[iFold][i]);
         strcat(str,tmp);
      }
      strcat(str,"};\n");
      strcat(buffer,str);

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
      // Setters
      for (i=0;i<numOfValue[iFold];i++) {
         int lb = nameLen-strlen(valueName[iFold][i]);
         sprintf(format,"   void Set%%s%%%ds(%%-%ds %%s%%%ds) { f%%s%%%ds = %%s;%%%ds };\n",lb,typeLen,lb,lb,lb);
         sprintf(buffer+strlen(buffer),format,valueName[iFold][i],"",valueType[iFold][i],valueName[iFold][i],"",valueName[iFold][i],"",valueName[iFold][i],"");
      }

// Footer
//--------

      sprintf(buffer+strlen(buffer),"\n   ClassDef(%s%s,%s)\n",shortCut,folderName[iFold],version[iFold]);
      sprintf(buffer+strlen(buffer),"};\n\n");

      sprintf(buffer+strlen(buffer),"#endif   // %s%s_H\n",shortCut,folderName[iFold]);

// Close h-File
//--------------

      sprintf(hFile,"%s%s%s.h",outDir,shortCut,folderName[iFold]);
      fileHandle = open(hFile,O_RDONLY);
      nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      bool identical = true;
      for (i=0;i<nb||i<strlen(buffer);i++) {
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

//============================================================
//                     Tasks
//============================================================

bool ROMEBuilder::XMLToTask(xmlTextReaderPtr reader) {
   char parent[10][100];
   const xmlChar *name, *value;
   bool finished = false;
   int type,i,isub=0;
   strcpy(parent[0],"GetMainTask()");

// read XML file
//===============

   if (makeOutput) cout << "\n\nTasks:" << endl;
   while (xmlTextReaderRead(reader)) {
      type = xmlTextReaderNodeType(reader);
      name = xmlTextReaderConstName(reader);
      if (type == 1 && !strcmp((const char*)name,"SubTask")) {
         numOfTask++;
         // initialisation
         strcpy(parentTaskName[numOfTask],parent[isub]);
         strcpy(version[numOfTask],"1");
         strcpy(author[numOfTask],"");
         strcpy(classDescription[numOfTask],"");
         numOfHistos[numOfTask] = 0;
         numOfInclude[numOfTask] = 0;
         // task name
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Name");
         strcpy(taskName[numOfTask],(const char*)value);
         xmlFree((void*)value);
         // language
         fortranFlag[numOfTask] = false;
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Language");
         if (value!=NULL && !strcmp((char*)value,"Fortran")) fortranFlag[numOfTask] = true;
         xmlFree((void*)value);

         // handle subtask
         isub++;
         strcpy(parent[isub],taskName[numOfTask]);

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
            value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Name");
            strcpy(author[numOfTask],(const char*)value);
            xmlFree((void*)value);
         }
         // version
         else if (!strcmp((const char*)name,"Version")) {
            value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Number");
            strcpy(version[numOfTask],(const char*)value);
            xmlFree((void*)value);
         }
         // description
         else if (!strcmp((const char*)name,"ClassDescription")) {
            value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Text");
            strcpy(classDescription[numOfTask],(const char*)value);
            xmlFree((void*)value);
         }
         // includes
         else if (!strcmp((const char*)name,"Include")) {
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  // name
                  strcpy(include[numOfTask][numOfInclude[numOfTask]],(const char*)name);
                  // type
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Type");
                  localFlag[numOfTask][numOfInclude[numOfTask]] = false;
                  if (value!=NULL && !strcmp((char*)value,"local")) localFlag[numOfTask][numOfInclude[numOfTask]] = true;
                  xmlFree((void*)value);
                  numOfInclude[numOfTask]++;
               }
               if (type == 15 && !strcmp((const char*)name,"Include")) {
                  break;
               }
            }
         }
         // histos
         else if (!strcmp((const char*)name,"Histos")) {
            while (xmlTextReaderRead(reader)) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 1) {
                  // histo name
                  strcpy(histoName[numOfTask][numOfHistos[numOfTask]],(const char*)name);
                  // histo type
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Type");
                  if (value!=NULL) strcpy(histoType[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no type defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo array size
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"ArraySize");
                  if (value!=NULL) strcpy(histoArray[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else strcpy(histoArray[numOfTask][numOfHistos[numOfTask]],"1");
                  xmlFree((void*)value);
                  // histo title
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Title");
                  if (value!=NULL) strcpy(histoTitle[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else strcpy(histoTitle[numOfTask][numOfHistos[numOfTask]],"");
                  xmlFree((void*)value);
                  // histo folder name
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"FolderName");
                  if (value!=NULL) strcpy(histoFolderName[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Folder Name defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo folder title
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"FolderTitle");
                  if (value!=NULL) strcpy(histoFolderTitle[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else strcpy(histoFolderTitle[numOfTask][numOfHistos[numOfTask]],"");
                  xmlFree((void*)value);
                  // histo xbins
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"NumberOfBinsX");
                  if (value!=NULL) strcpy(histoXBin[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no number of X bins defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo xmin
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"XMin");
                  if (value!=NULL) strcpy(histoXMin[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no X minimum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo xmax
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"XMax");
                  if (value!=NULL) strcpy(histoXMax[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no X maximum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo ybins
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"NumberOfBinsY");
                  if (value!=NULL) strcpy(histoYBin[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no number of Y bins defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo ymin
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"YMin");
                  if (value!=NULL) strcpy(histoYMin[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Y minimum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo ymax
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"YMax");
                  if (value!=NULL) strcpy(histoYMax[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=50) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Y maximum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo zbins
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"NumberOfBinsZ");
                  if (value!=NULL) strcpy(histoZBin[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no number of Z bins defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo zmin
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"ZMin");
                  if (value!=NULL) strcpy(histoZMin[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Z minimum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  // histo zmax
                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)"ZMax");
                  if (value!=NULL) strcpy(histoZMax[numOfTask][numOfHistos[numOfTask]],(const char*)value);
                  else if (histoType[numOfTask][numOfHistos[numOfTask]][2]>=51) {
                     cout << "Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]] << "' of Task '" << taskName[numOfTask] << "' has no Z maximum defined !" << endl;
                     cout << "Terminating program." << endl;
                     return false;
                  }
                  xmlFree((void*)value);
                  numOfHistos[numOfTask]++;
               }
               if (type == 15 && !strcmp((const char*)name,"Histos")) {
                  break;
               }
            }
         }
      }
      if (type == 15 && !strcmp((const char*)name,"Task")) break;
   }
   numOfTask++;

// write source files
//====================
   char cppFile[500];
   char hFile[500];
   char fFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int lenTot,ll;
   char *pBuffer;
   int bufferLen=0;
   char *pos;
   int fileHandle;
   char format[100];

   if (makeOutput) cout << "\n   Output Files:" << endl;
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
      pos = classDescription[iTask];
      lenTot = strlen(classDescription[iTask]);
      while (pos-classDescription[iTask] < lenTot) {
         if (lenTot+(classDescription[iTask]-pos)<74) i=75;
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
      for (i=0;i<strlen(shortCut);i++) shortcut[i] = tolower(shortCut[i]);
      shortcut[i] = 0;
      char taskname[20];
      for (i=0;i<strlen(taskName[iTask]);i++) taskname[i] = tolower(taskName[iTask][i]);
      taskname[i] = 0;

      bool replaceHeader = true;
      bool replaceBody = true;
      sprintf(cppFile,"%s%sT%s.cpp",outDir,shortCut,taskName[iTask]);
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
            sprintf(buffer,"\n\n#include %c%sT%s.h%c\n",34,shortCut,taskName[iTask],34);

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
// f-File
//--------
      if (fortranFlag[iTask]) {
         sprintf(fFile,"%s%sT%s.f",outDir,shortCut,taskName[iTask]);
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

// h-File
//--------


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

      sprintf(buffer+strlen(buffer),"#include\"%sAnalyzer.h\"\n",shortCut);

// Class
//-------

      sprintf(buffer+strlen(buffer),"\nclass %sT%s : public ROMETask\n",shortCut,taskName[iTask]);
      sprintf(buffer+strlen(buffer),"{\n");

// Fields
//--------

      char format[100];
      sprintf(buffer+strlen(buffer),"protected:\n");

      sprintf(buffer+strlen(buffer),"   %sAnalyzer* fAnalyzer; // Handle to %sAnalyzer class\n\n",shortCut,shortCut);

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
      }

// Methods
//---------

      sprintf(buffer+strlen(buffer),"public:\n");
      // Constructor and Event Methods
      sprintf(buffer+strlen(buffer),"   // Constructor\n");
      sprintf(buffer+strlen(buffer),"   %sT%s(const char *name,const char *title,%sAnalyzer *analyzer):ROMETask(name,title,analyzer)\n",shortCut,taskName[iTask],shortCut);
      sprintf(buffer+strlen(buffer),"   { fAnalyzer = analyzer; fVersion = %s; };\n",version[iTask]);
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
            sprintf(buffer+strlen(buffer),"   %s* Get%sHandleAt(int index) { return (%s*)f%ss->At(index); };\n",histoType[iTask][i],histoName[iTask][i],histoType[iTask][i],histoName[iTask][i]);
         }
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
            sprintf(buffer+strlen(buffer),"   if (!isHistoAccumulation()) f%s->Reset();\n",histoName[iTask][i]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   for (j=0;j<%s;j++) {\n",histoArray[iTask][i]);
            sprintf(buffer+strlen(buffer),"      if (!isHistoAccumulation()) ((%s*)f%ss->At(j))->Reset();\n",histoType[iTask][i],histoName[iTask][i]);
            sprintf(buffer+strlen(buffer),"   }\n");
         }
      }
      sprintf(buffer+strlen(buffer),"}\n\n");

      sprintf(buffer+strlen(buffer),"#endif   // %sT%s_H\n",shortCut,taskName[iTask]);

// Close h-File
//--------------
      sprintf(hFile,"%s%sT%s.h",outDir,shortCut,taskName[iTask]);
      fileHandle = open(hFile,O_RDONLY);
      nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      bool identical = true;
      for (i=0;i<nb||i<strlen(buffer);i++) {
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

//============================================================
//                     Trees
//============================================================

bool ROMEBuilder::XMLToTree(xmlTextReaderPtr reader) {
   const xmlChar *name, *value;
   bool finished = false;
   int type,depth,isub=0;

// read XML file
//===============

   while (xmlTextReaderRead(reader)) {
      type = xmlTextReaderNodeType(reader);
      name = xmlTextReaderConstName(reader);
      depth = xmlTextReaderDepth(reader)-2;
// Trees
      if (type == 1 && !strcmp((const char*)name,"Tree")) {
         numOfTree++;
         numOfBranch[numOfTree] = 0;
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Name");
         if (value!=NULL) strcpy(treeName[numOfTree],(const char*)value);
         xmlFree((void*)value);
         value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Title");
         if (value!=NULL) strcpy(treeTitle[numOfTree],(const char*)value);
         xmlFree((void*)value);
         while (xmlTextReaderRead(reader)) {
            type = xmlTextReaderNodeType(reader);
            name = xmlTextReaderConstName(reader);
            if (type == 1 && !strcmp((const char*)name,"Branch")) {
               value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Name");
               if (value!=NULL) strcpy(branchName[numOfTree][numOfBranch[numOfTree]],(const char*)value);
               xmlFree((void*)value);
               value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Folder");
               if (value!=NULL) strcpy(branchFolder[numOfTree][numOfBranch[numOfTree]],(const char*)value);
               xmlFree((void*)value);
               numOfBranch[numOfTree]++;
            }
            if (type == 15 && !strcmp((const char*)name,"Tree")) {
               break;
            }
         }
      }

      if (type == 15 && !strcmp((const char*)name,"Trees")) break;
   }
   numOfTree++;
   return true;
}

//============================================================
//                     Analyzer & EventLoop
//============================================================

bool ROMEBuilder::WriteAnalyzer() {
   int i;
// write source files
//====================

   char cppFile[500];
   char hFile[500];
   char buffer[bufferLength];
   char fileBuffer[bufferLength];

   int nb,lenTot,j,k,iFold,ll;
   const int bufferSize = 600;
   char str[bufferSize];
   char tmp[bufferSize];
   char *pos;
   int fileHandle;

   if (makeOutput) cout << "\n\nFramework :" << endl;
   if (makeOutput) cout << "\n   Output Files:" << endl;
   char classDescription[500];
   sprintf(classDescription,"Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut,mainProgName);

// Analyzer
//==========

   char format[100];
   int nameLen = -1;
   int typeLen = 12;
   int scl = strlen(shortCut);
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)strlen(folderName[i])+scl) typeLen = strlen(folderName[i])+scl;
      if (nameLen<(int)strlen(folderName[i])) nameLen = strlen(folderName[i]);
   }
// cpp-File
//----------

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

   sprintf(buffer+strlen(buffer),"#include <sys/stat.h>\n");
   sprintf(buffer+strlen(buffer),"#include <libxml/xmlreader.h>\n");
   sprintf(buffer+strlen(buffer),"#include <libxml/xmlwriter.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TROOT.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TObjArray.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMERunTable.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROME.h>\n");
   sprintf(buffer+strlen(buffer),"#include %c%sAnalyzer.h%c\n",34,shortCut,34);
   sprintf(buffer+strlen(buffer),"#include %c%sEventLoop.h%c\n",34,shortCut,34);
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer+strlen(buffer),"#include \"%sT%s.h\"\n",shortCut,taskName[i]);
   }
   sprintf(buffer+strlen(buffer),"#include <Riostream.h>\n");
   sprintf(buffer+strlen(buffer),"#define MY_ENCODING \"ISO-8859-1\"\n");

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

   sprintf(buffer+strlen(buffer),"\nClassImp(%sAnalyzer)\n\n",shortCut);


// Constructor
//-------------

   char parentt[100];
   sprintf(buffer+strlen(buffer),"%sAnalyzer::%sAnalyzer() {\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"// Folder, Task, Tree and Data Base initialisation\n");
   sprintf(buffer+strlen(buffer),"\n");
   // Folder 
   sprintf(buffer+strlen(buffer),"// Folder initialisation\n");
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
         if (!strcmp(folderArray[i],"yes")) {
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


   // Task
   sprintf(buffer+strlen(buffer),"// Task initialisation\n");
   sprintf(buffer+strlen(buffer),"   fMainTask = new %sEventLoop(\"mainTask\",\"Main Task of %s%s\",this);\n",shortCut,shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"   fMainFolder->Add(fMainTask);\n");
   sprintf(buffer+strlen(buffer),"   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   int taskLen=0;
   for (i=0;i<numOfTask;i++) {
      if (taskLen<(int)strlen(taskName[i])) taskLen = strlen(taskName[i]);
   }
   for (i=0;i<numOfTask;i++) {
      sprintf(format,"   TTask* %%sTask%%%ds = new %%sT%%s(\"%%s\",\"%%s\",this);\n",taskLen-strlen(taskName[i]));
      sprintf(buffer+strlen(buffer),format,taskName[i],"",shortCut,taskName[i],taskName[i],"");
   }
   for (i=0;i<numOfTask;i++) {
      if (!strcmp(parentTaskName[i],"GetMainTask()")) strcpy(parentt,parentTaskName[i]);
      else sprintf(parentt,"%sTask",parentTaskName[i]);
      sprintf(buffer+strlen(buffer),"   %s->Add(%sTask);\n",parentt,taskName[i]);
   }
   sprintf(buffer+strlen(buffer),"\n");

   // Tree
   sprintf(buffer+strlen(buffer),"// Tree initialisation\n");
   sprintf(buffer+strlen(buffer),"   TTree *tree;\n\n");
   for (i=0;i<numOfTree;i++) {
      sprintf(buffer+strlen(buffer),"   tree = new TTree(\"%s\",\"%s\");\n",treeName[i],treeTitle[i]);
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (!strcmp(branchFolder[i][j],folderName[k])) iFold = k;
         }
         if (!strcmp(folderArray[iFold],"yes")) {
            sprintf(buffer+strlen(buffer),"   tree->Branch(\"%s\",\"TClonesArray\",&f%sObjects,32000,99);\n",branchName[i][j],branchFolder[i][j]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   tree->Branch(\"%s\",\"%s%s\",&f%sObject,32000,99);\n",branchName[i][j],shortCut,folderName[iFold],branchFolder[i][j]);
         }
      }
      sprintf(buffer+strlen(buffer),"   this->AddTree(tree);\n\n");
    }

   // RunTable
   sprintf(buffer+strlen(buffer),"// RunTable\n");
   sprintf(buffer+strlen(buffer),"   fRunTable = new TList();\n");

   sprintf(buffer+strlen(buffer),"};\n\n");

// Splash Screen
//---------------
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
//----------------

   char prog[100];
   sprintf(prog,"%s%s",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"void %sAnalyzer::consoleStartScreen() {\n",shortCut);
   sprintf(buffer+strlen(buffer),"   int i;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*****************************************\" << endl;\n");   
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*                                       *\" << endl;\n");
   sprintf(buffer+strlen(buffer),"   cout << \"*\";\n");
   int len1 = ((39-(double)strlen(prog))/2+0.5);
   int len2 = ((39-(double)strlen(prog))/2);
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

// Write DataBase Functions
//--------------------------
   int ndb = 0;
   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         ndb++;
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"void %sAnalyzer::Write%sDataBase(ROMETask* task) {\n",shortCut,folderName[i]);
         sprintf(buffer+strlen(buffer),"   int i=0,j=0;\n");
         sprintf(buffer+strlen(buffer),"   xmlTextWriterPtr writer;\n");
         sprintf(buffer+strlen(buffer),"   char filename[gFileNameLength];\n");
         sprintf(buffer+strlen(buffer),"   char runNumber[10],chr[10];\n");
         sprintf(buffer+strlen(buffer),"   GetCurrentRunNumberString(runNumber);\n");

         sprintf(buffer+strlen(buffer),"   sprintf(filename,\"%%sdb%s%%s_%%s_%%d.xml\",GetDataBaseDir(),runNumber,task->GetName(),task->GetVersion());\n",folderName[i]);

         sprintf(buffer+strlen(buffer),"   writer = xmlNewTextWriterFilename(filename, 0);\n");
         sprintf(buffer+strlen(buffer),"   if (writer == NULL) {\n");
         sprintf(buffer+strlen(buffer),"      fprintf(stderr, \"Unable to open %%s\\n\", filename);\n");
         sprintf(buffer+strlen(buffer),"      return;\n");
         sprintf(buffer+strlen(buffer),"   }\n\n");
         sprintf(buffer+strlen(buffer),"   // Header\n");
         sprintf(buffer+strlen(buffer),"   xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);\n");
         sprintf(buffer+strlen(buffer),"   xmlTextWriterWriteFormatComment(writer,\"%%s\",\" edited with the %s%s \");\n",shortCut,mainProgName);
         sprintf(buffer+strlen(buffer),"   // Object\n");
         if (!strcmp(folderArray[i],"yes")) {
            sprintf(buffer+strlen(buffer),"   xmlTextWriterStartElement(writer, BAD_CAST \"%ss\");\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"   for (i=0;i<Get%sObjects()->GetEntries();i++) {\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"      xmlTextWriterStartElement(writer, BAD_CAST \"%s\");\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"      sprintf(chr,\"%%d\",i);\n");
            sprintf(buffer+strlen(buffer),"      xmlTextWriterWriteAttribute(writer, BAD_CAST \"Number\",BAD_CAST chr);\n");
            for (j=0;j<numOfValue[i];j++) {
               char format[10];
               GetFormat(format,valueType[i][j]);
               if (!strcmp(valueType[i][j],"TString")) {
                  sprintf(buffer+strlen(buffer),"      xmlTextWriterWriteFormatElement(writer, BAD_CAST \"%s\", \"%s\", Get%sAt(i)->Get%s().Data());\n",valueName[i][j],format,folderName[i],valueName[i][j]);
               }
               else {
                  sprintf(buffer+strlen(buffer),"      xmlTextWriterWriteFormatElement(writer, BAD_CAST \"%s\", \"%s\", Get%sAt(i)->Get%s());\n",valueName[i][j],format,folderName[i],valueName[i][j]);
               }
            }
            sprintf(buffer+strlen(buffer),"      xmlTextWriterEndElement(writer);\n");
            sprintf(buffer+strlen(buffer),"   }\n");
         }
         else {
            sprintf(buffer+strlen(buffer),"   xmlTextWriterStartElement(writer, BAD_CAST \"%s\");\n",folderName[i]);
            for (j=0;j<numOfValue[i];j++) {
               char format[10];
               GetFormat(format,valueType[i][j]);
               if (!strcmp(valueType[i][j],"TString")) {
                  sprintf(buffer+strlen(buffer),"   xmlTextWriterWriteFormatElement(writer, BAD_CAST \"%s\", \"%s\", Get%sAt(i)->Get%s().Data());\n",valueName[i][j],format,folderName[i],valueName[i][j]);
               }
               else {
                  sprintf(buffer+strlen(buffer),"   xmlTextWriterWriteFormatElement(writer, BAD_CAST \"%s\", \"%s\", Get%sAt(i)->Get%s());\n",valueName[i][j],format,folderName[i],valueName[i][j]);
               }
            }
         }
         sprintf(buffer+strlen(buffer),"   xmlTextWriterEndDocument(writer);\n");
         sprintf(buffer+strlen(buffer),"   xmlFreeTextWriter(writer);\n");
         sprintf(buffer+strlen(buffer),"   xmlCleanupParser();\n");
         sprintf(buffer+strlen(buffer),"   xmlMemoryDump();\n");

         sprintf(buffer+strlen(buffer),"\n");

         sprintf(buffer+strlen(buffer),"   sprintf(filename,\"db%s%%s_%%s_%%d.xml\",runNumber,task->GetName(),task->GetVersion());\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"   int pos = GetCurrentRunTablePos();\n");
         sprintf(buffer+strlen(buffer),"   if (pos<GetRunTable()->GetSize()) {\n");
         sprintf(buffer+strlen(buffer),"      ROMERunTable* runTab = (ROMERunTable*)GetRunTable()->At(pos);\n");
         sprintf(buffer+strlen(buffer),"      int runNum = runTab->GetRunNumber();\n");
         sprintf(buffer+strlen(buffer),"      if (runNum<=GetCurrentRunNumber()) {\n");
         sprintf(buffer+strlen(buffer),"         TString* file = runTab->GetFile();\n");
         sprintf(buffer+strlen(buffer),"         TString *fileNew;\n");
         sprintf(buffer+strlen(buffer),"         int ind = -1;\n");
         sprintf(buffer+strlen(buffer),"         for (i=0;i<runTab->GetNumberOfFiles();i++) {\n");
         sprintf(buffer+strlen(buffer),"            if (!strcmp(file[i].Data(),\"%sFile\")) ind = i;\n",folderName[i]);
         sprintf(buffer+strlen(buffer),"         }\n");
         sprintf(buffer+strlen(buffer),"         if (ind != -1) fileNew = new TString[runTab->GetNumberOfFiles()];\n");
         sprintf(buffer+strlen(buffer),"         else fileNew = new TString[runTab->GetNumberOfFiles()+1];\n");
         sprintf(buffer+strlen(buffer),"         for (i=0;i<runTab->GetNumberOfFiles();i++) {\n");
         sprintf(buffer+strlen(buffer),"            fileNew[i] = file[i];\n");
         sprintf(buffer+strlen(buffer),"         }\n");
         sprintf(buffer+strlen(buffer),"         if (ind != -1) fileNew[ind] = filename;\n");
         sprintf(buffer+strlen(buffer),"         else fileNew[i] = filename;\n");
         sprintf(buffer+strlen(buffer),"         runTab->SetFile(fileNew);\n");
         sprintf(buffer+strlen(buffer),"         return;\n");
         sprintf(buffer+strlen(buffer),"      }\n");
         sprintf(buffer+strlen(buffer),"   }\n");
         sprintf(buffer+strlen(buffer),"   TString *files = new TString[%d];\n",ndb);
         int ndb = 0;
         for (j=0;j<numOfFolder;j++) {
            if (dataBase[j]) {
               if (i==j) {
                  sprintf(buffer+strlen(buffer),"   files[%d] = filename;\n",ndb);
               }
               else {
                  sprintf(buffer+strlen(buffer),"   files[%d] = \"\";\n",ndb);
               }
               ndb++;
            }
         }
         sprintf(buffer+strlen(buffer),"   GetRunTable()->AddAt(new ROMERunTable(Get%sAt(0)->GetTimeStamp(),Get%sAt(0)->GetRunNumber(),Get%sAt(0)->GetRunDescription().Data(),%d,files),pos);\n",folderName[i],folderName[i],folderName[i],ndb);
         sprintf(buffer+strlen(buffer),"   pos++;\n");
         sprintf(buffer+strlen(buffer),"   SetCurrentRunTablePos(pos);\n");
         sprintf(buffer+strlen(buffer),"}\n\n");
      }
   }

// Close cpp-File
//----------------
   sprintf(cppFile,"%s%sAnalyzer.cpp",outDir,shortCut);
   fileHandle = open(cppFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   for (i=0;i<nb||i<strlen(buffer);i++) {
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

// h-File
//--------

   if (overWriteH) {

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

      sprintf(buffer+strlen(buffer),"#ifndef %sAnalyzer_H\n",shortCut);
      sprintf(buffer+strlen(buffer),"#define %sAnalyzer_H\n\n",shortCut);

      sprintf(buffer+strlen(buffer),"#include <TTask.h>\n");
      sprintf(buffer+strlen(buffer),"#include <TTree.h>\n");
      sprintf(buffer+strlen(buffer),"#include <TFolder.h>\n");
      sprintf(buffer+strlen(buffer),"#include <TClonesArray.h>\n");
      sprintf(buffer+strlen(buffer),"#include <ROMETask.h>\n");
      sprintf(buffer+strlen(buffer),"#include <ROMEAnalyzer.h>\n");
      for (i=0;i<numOfFolder;i++) {
         if (numOfValue[i] > 0) {
            sprintf(buffer+strlen(buffer),"#include \"%s%s.h\"\n",shortCut,folderName[i]);
         }
      }

// Class
//-------

      sprintf(buffer+strlen(buffer),"\nclass %sAnalyzer : public ROMEAnalyzer\n",shortCut);
      sprintf(buffer+strlen(buffer),"{\n");

// Fields
//--------

      sprintf(buffer+strlen(buffer),"protected:\n");

      for (i=0;i<numOfFolder;i++) {
         if (numOfValue[i] > 0) {
            if (!strcmp(folderArray[i],"yes")) {
               sprintf(format,"   TClonesArray*%%%ds f%%sObjects;%%%ds // Handle to %%s%%s Objects\n",typeLen-12,nameLen-strlen(folderName[i]));
               sprintf(buffer+strlen(buffer),format,"",folderName[i],"",shortCut,folderName[i]);
            }
            else {
               sprintf(format,"   %%s%%s*%%%ds f%%sObject; %%%ds // Handle to %%s%%s Object\n",typeLen-strlen(folderName[i])-scl,nameLen-strlen(folderName[i]));
               sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",shortCut,folderName[i]);
            }
         }
      }

// Methods
//---------

      sprintf(buffer+strlen(buffer),"public:\n");
      // Constructor
      sprintf(buffer+strlen(buffer),"   %sAnalyzer();\n\n",shortCut);
      // Getters
      for (i=0;i<numOfFolder;i++) {
         if (numOfValue[i] > 0) {
            int lt = typeLen-strlen(folderName[i])-scl+nameLen-strlen(folderName[i]);
            if (!strcmp(folderArray[i],"yes")) {
               sprintf(format,"   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds { return (%%s%%s*)f%%sObjects->At(index);%%%ds };\n",typeLen-strlen(folderName[i])-scl,3+nameLen-strlen(folderName[i]),lt);
               sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",shortCut,folderName[i],folderName[i],"");
               sprintf(format,"   TClonesArray*%%%ds  Get%%sObjects()%%%ds { return f%%sObjects;%%%ds };\n",typeLen-12,7+nameLen-strlen(folderName[i]),14+typeLen+nameLen-strlen(folderName[i]));
               sprintf(buffer+strlen(buffer),format,"",folderName[i],"",folderName[i],"");
               sprintf(format,"   TClonesArray**%%%ds Get%%sObjectsAddress()%%%ds { return &f%%sObjects;%%%ds };\n",typeLen-12,nameLen-strlen(folderName[i]),13+typeLen+nameLen-strlen(folderName[i]));
               sprintf(buffer+strlen(buffer),format,"",folderName[i],"",folderName[i],"");
            }
            else {
               sprintf(format,"   %%s%%s*%%%ds  Get%%sObject()%%%ds { return f%%sObject;%%%ds };\n",typeLen-strlen(folderName[i])-scl,8+nameLen-strlen(folderName[i]),15+typeLen+nameLen-strlen(folderName[i]));
               sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",folderName[i],"");
               sprintf(format,"   %%s%%s**%%%ds Get%%sObjectAddress()%%%ds { return &f%%sObject;%%%ds };\n",typeLen-strlen(folderName[i])-scl,1+nameLen-strlen(folderName[i]),14+typeLen+nameLen-strlen(folderName[i]));
               sprintf(buffer+strlen(buffer),format,shortCut,folderName[i],"",folderName[i],"",folderName[i],"");
            }
         }
      }
      sprintf(buffer+strlen(buffer),"\n");
      for (i=0;i<numOfTree;i++) {
         sprintf(buffer+strlen(buffer),"   ROMETree* Get%sTree() { return (ROMETree*)fTreeObjects->At(%d); };\n",treeName[i],i);
         sprintf(buffer+strlen(buffer),"   Bool_t    is%sFillEvent() { return ((ROMETree*)fTreeObjects->At(%d))->isFillEvent(); };\n",treeName[i],i);
         sprintf(buffer+strlen(buffer),"   void      Set%sFillEvent(Bool_t flag) { ((ROMETree*)fTreeObjects->At(%d))->SetFillEvent(flag); };\n",treeName[i],i);
      }
      sprintf(buffer+strlen(buffer),"\n");

      // Setters
      for (i=0;i<numOfFolder;i++) {
         if (numOfValue[i] > 0) {
            if (!strcmp(folderArray[i],"yes")) {
               sprintf(str,"   void Set%sObject(int index",folderName[i]);
               for (j=0;j<numOfValue[i];j++) {
                  sprintf(tmp,",%s %s=%s",valueType[i][j],valueName[i][j],valueInit[i][j]);
                  strcat(str,tmp);
               }
               strcat(str,")\n");
               strcat(buffer,str);
               sprintf(str,"      { new((*f%sObjects)[index]) %s%s(",folderName[i],shortCut,folderName[i]);
               for (j=0;j<numOfValue[i];j++) {
                  if (j>0) strcat(str,",");
                  sprintf(tmp,"%s",valueName[i][j]);
                  strcat(str,tmp);
               }
               strcat(str,"); };\n");
               strcat(buffer,str);
            }
            else {
               sprintf(str,"   void Set%sObject(",folderName[i]);
               for (j=0;j<numOfValue[i];j++) {
                  if (j>0) strcat(str,",");
                  sprintf(tmp,"%s %s=%s",valueType[i][j],valueName[i][j],valueInit[i][j]);
                  strcat(str,tmp);
               }
               strcat(str,")\n");
               strcat(buffer,str);
               sprintf(str,"      { new(f%sObject) %s%s(",folderName[i],shortCut,folderName[i]);
               for (j=0;j<numOfValue[i];j++) {
                  if (j>0) strcat(str,",");
                  sprintf(tmp,"%s",valueName[i][j]);
                  strcat(str,tmp);
               }
               strcat(str,"); };\n");
               strcat(buffer,str);
            }
         }
      }
// public
      sprintf(buffer+strlen(buffer),"\npublic:\n");
      sprintf(buffer+strlen(buffer),"   void startSplashScreen();\n");
      sprintf(buffer+strlen(buffer),"   void consoleStartScreen();\n");
      for (i=0;i<numOfFolder;i++) {
         if (dataBase[i]) {
            sprintf(buffer+strlen(buffer),"   void Write%sDataBase(ROMETask* task);\n",folderName[i]);
         }
      }

// Footer
//--------

      sprintf(buffer+strlen(buffer),"\n   ClassDef(%sAnalyzer,1)\n",shortCut);
      sprintf(buffer+strlen(buffer),"};\n\n");

      sprintf(buffer+strlen(buffer),"#endif   // %sAnalyzer_H\n",shortCut);

// Close h-File
//--------------

      sprintf(hFile,"%s%sAnalyzer.h",outDir,shortCut);
      fileHandle = open(hFile,O_RDONLY);
      nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
      bool identical = true;
      for (i=0;i<nb||i<strlen(buffer);i++) {
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

// EventLoop
//==========

// cpp-File
//----------

// Description
//-------------
   sprintf(buffer,"//// Author: %s\n",mainAuthor);
   sprintf(buffer+strlen(buffer),"////////////////////////////////////////////////////////////////////////////////\n");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   ll = 74-strlen(shortCut);
   sprintf(format,"// %%s%%-%d.%ds //\n",ll,ll);
   sprintf(buffer+strlen(buffer),format,shortCut,"EventLoop");
   sprintf(buffer+strlen(buffer),"//                                                                            //\n");
   sprintf(buffer+strlen(buffer),"// Derivated form ROMEEventLoop. Implements some DataBase specific methods.   //\n");
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

   sprintf(buffer+strlen(buffer),"#include <libxml/xmlreader.h>\n");
   sprintf(buffer+strlen(buffer),"#include <libxml/xmlwriter.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TBranchElement.h>\n");
   sprintf(buffer+strlen(buffer),"#include <ROMERunTable.h>\n");
   sprintf(buffer+strlen(buffer),"#include \"ROME.h\"\n");
   sprintf(buffer+strlen(buffer),"#include \"%sEventLoop.h\"\n",shortCut);

   sprintf(buffer+strlen(buffer),"#include \"Riostream.h\"\n");

   sprintf(buffer+strlen(buffer),"#define MY_ENCODING \"ISO-8859-1\"\n");

   sprintf(buffer+strlen(buffer),"\nClassImp(%sEventLoop)\n\n",shortCut);

// User Functions
//----------------

   ndb = 0;
   for (i=0;i<numOfFolder;i++) if (dataBase[i]) ndb++;
   if (ndb>0) {
// ReadRunTable
//--------------
      sprintf(buffer+strlen(buffer),"void %sEventLoop::ReadRunTable() {\n",shortCut);

      sprintf(buffer+strlen(buffer),"   TString runDescription;\n");
      sprintf(buffer+strlen(buffer),"   const xmlChar *name,*value;\n");
      sprintf(buffer+strlen(buffer),"   char *cstop;\n");
      sprintf(buffer+strlen(buffer),"   int type,timeStamp,runNumber;\n");
      sprintf(buffer+strlen(buffer),"   xmlTextReaderPtr reader;\n");
      sprintf(buffer+strlen(buffer),"   char filename[gFileNameLength];\n");
      sprintf(buffer+strlen(buffer),"   sprintf(filename,\"%%sRunTable.xml\",fAnalyzer->GetInputDir());\n");
      sprintf(buffer+strlen(buffer),"   reader = xmlReaderForFile(filename, NULL, 0);\n");
      sprintf(buffer+strlen(buffer),"   if (reader != NULL) {\n");
      sprintf(buffer+strlen(buffer),"      while (xmlTextReaderRead(reader)) {\n");
      sprintf(buffer+strlen(buffer),"         type = xmlTextReaderNodeType(reader);\n");
      sprintf(buffer+strlen(buffer),"         name = xmlTextReaderConstName(reader);\n");
      sprintf(buffer+strlen(buffer),"         if (type == 1 && !strcmp((const char*)name,\"Entry\")) {\n");

      sprintf(buffer+strlen(buffer),"            value = xmlTextReaderGetAttribute(reader,(xmlChar*)\"TimeStamp\");\n");
      sprintf(buffer+strlen(buffer),"            if (value!=NULL) timeStamp = strtol((const char*)value,&cstop,10);\n");
      sprintf(buffer+strlen(buffer),"            xmlFree((void*)value);\n");
      sprintf(buffer+strlen(buffer),"            value = xmlTextReaderGetAttribute(reader,(xmlChar*)\"RunNumber\");\n");
      sprintf(buffer+strlen(buffer),"            if (value!=NULL) runNumber = strtol((const char*)value,&cstop,10);\n");
      sprintf(buffer+strlen(buffer),"            xmlFree((void*)value);\n");
      sprintf(buffer+strlen(buffer),"            value = xmlTextReaderGetAttribute(reader,(xmlChar*)\"RunDescription\");\n");
      sprintf(buffer+strlen(buffer),"            if (value!=NULL) runDescription = (char*)value;\n");
      sprintf(buffer+strlen(buffer),"            xmlFree((void*)value);\n");
      sprintf(buffer+strlen(buffer),"            TString *files = new TString[%d];\n",ndb);
      ndb = 0;
      for (i=0;i<numOfFolder;i++) {
         if (dataBase[i]) {
            sprintf(buffer+strlen(buffer),"            value = xmlTextReaderGetAttribute(reader,(xmlChar*)\"%sFile\");\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"            if (value!=NULL) files[%d] = (char*)value;\n",ndb);
            sprintf(buffer+strlen(buffer),"            else files[%d] = \"\";\n",ndb);
            sprintf(buffer+strlen(buffer),"            xmlFree((void*)value);\n");
            ndb++;
         }
      }
      sprintf(buffer+strlen(buffer),"            fAnalyzer->GetRunTable()->AddLast(new ROMERunTable(timeStamp,runNumber,runDescription,%d,files));\n",ndb);

      sprintf(buffer+strlen(buffer),"         }\n");
      sprintf(buffer+strlen(buffer),"         if (type == 15 && !strcmp((const char*)name,\"RunTable\")) break;\n");
      sprintf(buffer+strlen(buffer),"      }\n");
      sprintf(buffer+strlen(buffer),"      xmlFreeTextReader(reader);\n");
      sprintf(buffer+strlen(buffer),"   } else {\n");
      sprintf(buffer+strlen(buffer),"      fprintf(stderr, \"Unable to open %%s\\n\", filename);\n");
      sprintf(buffer+strlen(buffer),"   }\n");

      sprintf(buffer+strlen(buffer),"}\n\n");
// SaveRunTable
//--------------
      sprintf(buffer+strlen(buffer),"void %sEventLoop::SaveRunTable() {\n",shortCut);
      sprintf(buffer+strlen(buffer),"   int i=0;\n");
      sprintf(buffer+strlen(buffer),"   char chr[100];\n");
      sprintf(buffer+strlen(buffer),"   xmlTextWriterPtr writer;\n");
      sprintf(buffer+strlen(buffer),"   char filename[gFileNameLength];\n");
      sprintf(buffer+strlen(buffer),"   sprintf(filename,\"%%sRunTable.xml\",fAnalyzer->GetOutputDir());\n");
      sprintf(buffer+strlen(buffer),"   writer = xmlNewTextWriterFilename(filename, 0);\n");
      sprintf(buffer+strlen(buffer),"   if (writer == NULL) {\n");
      sprintf(buffer+strlen(buffer),"      fprintf(stderr, \"Unable to open %%s\\n\", filename);\n");
      sprintf(buffer+strlen(buffer),"      return;\n");
      sprintf(buffer+strlen(buffer),"   }\n\n");
      sprintf(buffer+strlen(buffer),"   // Header\n");
      sprintf(buffer+strlen(buffer),"   xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);\n");
      sprintf(buffer+strlen(buffer),"   xmlTextWriterWriteFormatComment(writer,\"%%s\",\" edited with the %s%s \");\n",shortCut,mainProgName);
      sprintf(buffer+strlen(buffer),"   // Object\n");
      sprintf(buffer+strlen(buffer),"   xmlTextWriterStartElement(writer, BAD_CAST \"RunTable\");\n");
      sprintf(buffer+strlen(buffer),"   for (i=0;i<fAnalyzer->GetRunTable()->GetSize();i++) {\n");
      sprintf(buffer+strlen(buffer),"      ROMERunTable* runTable = (ROMERunTable*)fAnalyzer->GetRunTable()->At(i);\n");
      sprintf(buffer+strlen(buffer),"      xmlTextWriterStartElement(writer, BAD_CAST \"Entry\");\n");
      sprintf(buffer+strlen(buffer),"      sprintf(chr,\"%%d\",runTable->GetTimeStamp());\n");
      sprintf(buffer+strlen(buffer),"      xmlTextWriterWriteAttribute(writer, BAD_CAST \"TimeStamp\",BAD_CAST chr);\n");
      sprintf(buffer+strlen(buffer),"      sprintf(chr,\"%%d\",runTable->GetRunNumber());\n");
      sprintf(buffer+strlen(buffer),"      xmlTextWriterWriteAttribute(writer, BAD_CAST \"RunNumber\",BAD_CAST chr);\n");
      sprintf(buffer+strlen(buffer),"      sprintf(chr,\"%%s\",runTable->GetRunDescription().Data());\n");
      sprintf(buffer+strlen(buffer),"      xmlTextWriterWriteAttribute(writer, BAD_CAST \"RunDescription\",BAD_CAST chr);\n");
      ndb = 0;
      for (i=0;i<numOfFolder;i++) {
         if (dataBase[i]) {
            sprintf(buffer+strlen(buffer),"      if (strcmp(runTable->GetFile()[%d].Data(),\"\")) {\n",ndb);
            sprintf(buffer+strlen(buffer),"         sprintf(chr,\"%%s\",runTable->GetFile()[%d].Data());\n",ndb);
            sprintf(buffer+strlen(buffer),"         xmlTextWriterWriteAttribute(writer, BAD_CAST \"%sFile\",BAD_CAST chr);\n",folderName[i]);
            sprintf(buffer+strlen(buffer),"      }\n");
            ndb++;
         }
      }
      sprintf(buffer+strlen(buffer),"      xmlTextWriterEndElement(writer);\n");
      sprintf(buffer+strlen(buffer),"   }\n");
      sprintf(buffer+strlen(buffer),"   xmlTextWriterEndDocument(writer);\n");
      sprintf(buffer+strlen(buffer),"   xmlFreeTextWriter(writer);\n");
      sprintf(buffer+strlen(buffer),"   xmlCleanupParser();\n");
      sprintf(buffer+strlen(buffer),"   xmlMemoryDump();\n");
      sprintf(buffer+strlen(buffer),"}\n\n");

// InitDataBase
//--------------
      sprintf(buffer+strlen(buffer),"void %sEventLoop::InitDataBase()\n{\n",shortCut);
      sprintf(buffer+strlen(buffer),"   Update();\n");
      sprintf(buffer+strlen(buffer),"}\n\n");

// UpdateDataBase
//----------------
      sprintf(buffer+strlen(buffer),"void %sEventLoop::UpdateDataBase()\n{\n",shortCut);

      sprintf(buffer+strlen(buffer),"   int pos = fAnalyzer->GetCurrentRunTablePos();\n");
      sprintf(buffer+strlen(buffer),"   if (pos>=fAnalyzer->GetRunTable()->GetSize()) return;\n");
      sprintf(buffer+strlen(buffer),"   int runNum = ((ROMERunTable*)fAnalyzer->GetRunTable()->At(pos))->GetRunNumber();\n");
      sprintf(buffer+strlen(buffer),"   if (runNum>fAnalyzer->GetCurrentRunNumber()) return;\n");
      sprintf(buffer+strlen(buffer),"   for (pos++;runNum<=fAnalyzer->GetCurrentRunNumber() && pos<fAnalyzer->GetRunTable()->GetSize();pos++)\n");
      sprintf(buffer+strlen(buffer),"      runNum = ((ROMERunTable*)fAnalyzer->GetRunTable()->At(pos))->GetRunNumber();\n");
      sprintf(buffer+strlen(buffer),"   pos--;\n");
      sprintf(buffer+strlen(buffer),"   if (pos>=fAnalyzer->GetRunTable()->GetSize()) return;\n");
      sprintf(buffer+strlen(buffer),"   fAnalyzer->SetCurrentRunTablePos(pos);\n");
      sprintf(buffer+strlen(buffer),"   Update();\n");
      sprintf(buffer+strlen(buffer),"}\n\n");
   
// Update
//--------
      sprintf(buffer+strlen(buffer),"void %sEventLoop::Update()\n{\n",shortCut);
      sprintf(buffer+strlen(buffer),"   int i;\n");
      sprintf(buffer+strlen(buffer),"   int pos = fAnalyzer->GetCurrentRunTablePos();\n");
      sprintf(buffer+strlen(buffer),"   ROMERunTable* runTab = (ROMERunTable*)fAnalyzer->GetRunTable()->At(pos);\n");
      sprintf(buffer+strlen(buffer),"   for (i=0;i<runTab->GetNumberOfFiles();i++) {\n");
      sprintf(buffer+strlen(buffer),"      if (!strcmp(runTab->GetFile()[i].Data(),\"\")) continue;\n");
   
      sprintf(buffer+strlen(buffer),"      const xmlChar *name,*value;\n");
      sprintf(buffer+strlen(buffer),"      char *cstop;\n");
      sprintf(buffer+strlen(buffer),"      int type;\n");
      sprintf(buffer+strlen(buffer),"      xmlTextReaderPtr reader;\n");
      sprintf(buffer+strlen(buffer),"      char filename[gFileNameLength];\n");
      sprintf(buffer+strlen(buffer),"      sprintf(filename,\"%%s%%s\",fAnalyzer->GetDataBaseDir(),runTab->GetFile()[i].Data());\n");
      sprintf(buffer+strlen(buffer),"      reader = xmlReaderForFile(filename, NULL, 0);\n");
      sprintf(buffer+strlen(buffer),"      if (reader != NULL) {\n");
      sprintf(buffer+strlen(buffer),"         while (xmlTextReaderRead(reader)) {\n");
      sprintf(buffer+strlen(buffer),"            type = xmlTextReaderNodeType(reader);\n");
      sprintf(buffer+strlen(buffer),"            name = xmlTextReaderConstName(reader);\n");
      ndb = 0;
      for (i=0;i<numOfFolder;i++) {
         if (dataBase[i]) {
            sprintf(buffer+strlen(buffer),"            if (i == %d) {\n",ndb);
            sprintf(buffer+strlen(buffer),"               int number=0;\n",ndb);
            for (j=0;j<numOfValue[i];j++) {
               sprintf(buffer+strlen(buffer),"               %s %s = %s;\n",valueType[i][j],valueName[i][j],valueInit[i][j]);
            }
            if (!strcmp(folderArray[i],"yes")) {
               sprintf(buffer+strlen(buffer),"               if (type == 1 && !strcmp((const char*)name,\"%ss\")) {\n",folderName[i]);
               for (j=0;j<numOfValue[i];j++) {
                  sprintf(buffer+strlen(buffer),"                  value = xmlTextReaderGetAttribute(reader,(xmlChar*)\"%s\");\n",valueName[i][j]);
                  char bufft[1000];
                  setValue(bufft,valueName[i][j],"(const char*)value",valueType[i][j]);
                  sprintf(buffer+strlen(buffer),"                  if (value!=NULL) %s;\n",bufft);
                  sprintf(buffer+strlen(buffer),"                  xmlFree((void*)value);\n");
               }
               sprintf(buffer+strlen(buffer),"                  while (xmlTextReaderRead(reader)) {\n");
               sprintf(buffer+strlen(buffer),"                     type = xmlTextReaderNodeType(reader);\n");
               sprintf(buffer+strlen(buffer),"                     name = xmlTextReaderConstName(reader);\n");
               sprintf(buffer+strlen(buffer),"                     if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",folderName[i]);
               sprintf(buffer+strlen(buffer),"                        value = xmlTextReaderGetAttribute(reader,(xmlChar*)\"Number\");\n");
               sprintf(buffer+strlen(buffer),"                        if (value!=NULL) number = strtol((const char*)value,&cstop,10);\n");
               sprintf(buffer+strlen(buffer),"                        xmlFree((void*)value);\n");
               sprintf(buffer+strlen(buffer),"                     }\n");
   
               for (j=0;j<numOfValue[i];j++) {
                  sprintf(buffer+strlen(buffer),"                     if (type == 1 && !strcmp((const char*)name,\"%s\")) {\n",valueName[i][j]);
                  sprintf(buffer+strlen(buffer),"                        xmlTextReaderRead(reader);\n");
                  sprintf(buffer+strlen(buffer),"                        type = xmlTextReaderNodeType(reader);\n");
                  sprintf(buffer+strlen(buffer),"                        value = xmlTextReaderConstValue(reader);\n");
                  char bufft[1000];
                  setValue(bufft,valueName[i][j],"(const char*)value",valueType[i][j]);
                  sprintf(buffer+strlen(buffer),"                        if (value!=NULL && type==3) %s;\n",bufft);
                  sprintf(buffer+strlen(buffer),"                     }\n");
               }
               sprintf(buffer+strlen(buffer),"                     if (type == 15 && !strcmp((const char*)name,\"%s\")) {\n",folderName[i]);
               sprintf(str,"                        fAnalyzer->Set%sObject(number",folderName[i]);
               for (j=0;j<numOfValue[i];j++) {
                  sprintf(tmp,",%s",valueName[i][j]);
                  strcat(str,tmp);
               }
               strcat(str,");\n");
               strcat(buffer,str);
   
               sprintf(buffer+strlen(buffer),"                     }\n");
               sprintf(buffer+strlen(buffer),"                  }\n");
   
               sprintf(buffer+strlen(buffer),"               }\n");
               sprintf(buffer+strlen(buffer),"               if (type == 15 && !strcmp((const char*)name,\"%ss\")) break;\n",folderName[i]);
            }
            else {
            }
            sprintf(buffer+strlen(buffer),"            }\n",folderName[i]);
            ndb++;
         }
      }
      sprintf(buffer+strlen(buffer),"         }\n");
      sprintf(buffer+strlen(buffer),"         xmlFreeTextReader(reader);\n");
      sprintf(buffer+strlen(buffer),"      } else {\n");
      sprintf(buffer+strlen(buffer),"         fprintf(stderr, \"Unable to open %%s\\n\", filename);\n");
      sprintf(buffer+strlen(buffer),"      }\n");
      sprintf(buffer+strlen(buffer),"   }\n");
   
      sprintf(buffer+strlen(buffer),"}\n");
   }

// ConnectTrees
//--------------
   sprintf(buffer+strlen(buffer),"void %sEventLoop::ConnectTrees()\n{\n",shortCut);
   sprintf(buffer+strlen(buffer),"   TBranchElement *bb;\n");
   for (i=0;i<numOfTree;i++) {
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (!strcmp(branchFolder[i][j],folderName[k])) iFold = k;
         }
         sprintf(buffer+strlen(buffer),"   bb = (TBranchElement*)fAnalyzer->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"%s\");\n",i,branchName[i][j]);
         if (!strcmp(folderArray[iFold],"yes")) {
            sprintf(buffer+strlen(buffer),"   bb->SetAddress(fAnalyzer->Get%sObjectsAddress());\n",folderName[iFold]);
         }
         else {
            sprintf(buffer+strlen(buffer),"   bb->SetAddress(fAnalyzer->Get%sObjectAddress());\n",folderName[iFold]);
         }
      }
   }
   sprintf(buffer+strlen(buffer),"}\n\n");

// Close cpp-File
//----------------

   sprintf(cppFile,"%s%sEventLoop.cpp",outDir,shortCut);
   fileHandle = open(cppFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   identical = true;
   for (i=0;i<nb||i<strlen(buffer);i++) {
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

// h-File
//--------

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

   sprintf(buffer+strlen(buffer),"#ifndef %sEventLoop_H\n",shortCut);
   sprintf(buffer+strlen(buffer),"#define %sEventLoop_H\n\n",shortCut);

   sprintf(buffer+strlen(buffer),"#include<ROMEEventLoop.h>\n");
   sprintf(buffer+strlen(buffer),"#include\"%sAnalyzer.h\"\n",shortCut);

// Class
//-------

   sprintf(buffer+strlen(buffer),"\nclass %sEventLoop : public ROMEEventLoop\n",shortCut);
   sprintf(buffer+strlen(buffer),"{\n");

// Fields
//--------

   sprintf(buffer+strlen(buffer),"protected:\n");

   sprintf(buffer+strlen(buffer),"   %sAnalyzer* fAnalyzer; // Handle to %sAnalyzer class\n\n",shortCut,shortCut);

// Methods
//---------

   sprintf(buffer+strlen(buffer),"\npublic:\n");
   // Constructor and Event Methods
   sprintf(buffer+strlen(buffer),"   %sEventLoop(const char *name,const char *title,%sAnalyzer *analyzer):ROMEEventLoop(name,title,analyzer)\n",shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"   { fAnalyzer = analyzer; fAnalyzer->SetCurrentRunTablePos(0); };\n");
   if (ndb>0) {
      sprintf(buffer+strlen(buffer),"   void ReadRunTable();\n");
      sprintf(buffer+strlen(buffer),"   void SaveRunTable();\n");
      sprintf(buffer+strlen(buffer),"   void InitDataBase();\n");
      sprintf(buffer+strlen(buffer),"   void UpdateDataBase();\n");
      sprintf(buffer+strlen(buffer),"   void Update();\n");
   }
   else {
      sprintf(buffer+strlen(buffer),"   void ReadRunTable() {};\n");
      sprintf(buffer+strlen(buffer),"   void SaveRunTable() {};\n");
      sprintf(buffer+strlen(buffer),"   void InitDataBase() {};\n");
      sprintf(buffer+strlen(buffer),"   void UpdateDataBase() {};\n");
      sprintf(buffer+strlen(buffer),"   void Update() {};\n");
   }
   sprintf(buffer+strlen(buffer),"   void ConnectTrees();\n");

// Footer
//--------
   sprintf(buffer+strlen(buffer),"\n   ClassDef(%sEventLoop,1)\n",shortCut);
   sprintf(buffer+strlen(buffer),"};\n\n");

   sprintf(buffer+strlen(buffer),"#endif   // %sEventLoop_H\n",shortCut);

// Close h-File
//--------------

   sprintf(hFile,"%s%sEventLoop.h",outDir,shortCut);
   fileHandle = open(hFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   identical = true;
   for (i=0;i<nb||i<strlen(buffer);i++) {
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


//  Main
// ======

   sprintf(buffer,"#include <TApplication.h>\n");
   sprintf(buffer+strlen(buffer),"#include <TBrowser.h>\n");
   sprintf(buffer+strlen(buffer),"#include \"%sAnalyzer.h\"\n",shortCut);
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
   sprintf(buffer+strlen(buffer),"   if (!analyzer->Start(argc, argv)) return 1;\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   TBrowser t;\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   theApp.Run();\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"   return 0;\n");
   sprintf(buffer+strlen(buffer),"}\n");

   sprintf(hFile,"%smain.cpp",outDir);
   fileHandle = open(hFile,O_RDONLY);
   nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   identical = true;
   for (i=0;i<nb||i<strlen(buffer);i++) {
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

   const int workDirLen = 1000;
   char workDir[workDirLen];
   getcwd(workDir,workDirLen);

   romeb->makeOutput = false;
   romeb->noLink = false;
   strcpy(romeb->outDir,workDir);
   strcat(romeb->outDir,"/");

   if (argc==1) {
      cout << "  -i       Inputfile" << endl;
      cout << "  -o       Outputfile path" << endl;
      cout << "  -v       Verbose Mode (no Argument)" << endl;
      cout << "  -nl      No Linking (no Argument)" << endl;
      return 0;
   }
   for (int i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-v")) {
         romeb->makeOutput = true;
      }
      else if (!strcmp(argv[i],"-nl")) {
         romeb->noLink = true;
      }
      else if (!strcmp(argv[i],"-i")&&i<argc-1) {
         strcpy(xmlFile,argv[i+1]);
         i++;
      }
      else if (!strcmp(argv[i],"-o")&&i<argc-1) {
         strcpy(romeb->outDir,argv[i+1]);
         strcat(romeb->outDir,"/");
         i++;
      }
      else if (argv[i][0]=='-') {
         cout << "  -i       Inputfile" << endl;
         cout << "  -o       Outputfile path" << endl;
         cout << "  -v       Verbose Mode (no Argument)" << endl;
         cout << "  -nl      No Linking (no Argument)" << endl;
         return 0;
      }
      else {
         strcpy(xmlFile,argv[i]);
      }
   }

   struct stat buf;
   if( stat( xmlFile, &buf )) {
      cout << "Imputfile '" << xmlFile << "' not found." << endl;
      return 1;
   }

   romeb->startBuilder(xmlFile);

   return 0;
}

void ROMEBuilder::startBuilder(char* xmlFile) 
{
   const int bufferSize = 600;
   const xmlChar *name,*value;
   bool finished = false;
   int type,numOfValue=0;


   strcpy(mainAuthor,"");
   strcpy(mainInstitute,"");
   strcpy(mainCollaboration,"");
   strcpy(mainEmail,"");
   strcpy(mainProgName,"");

   xmlTextReaderPtr reader;
   reader = xmlReaderForFile(xmlFile, NULL, 0);
   if (reader != NULL) {
      while (xmlTextReaderRead(reader)&&!finished) {
         type = xmlTextReaderNodeType(reader);
         name = xmlTextReaderConstName(reader);
         if (type == 1) {
            strcpy(shortCut,(const char*)name);
            while (xmlTextReaderRead(reader)&&!finished) {
               type = xmlTextReaderNodeType(reader);
               name = xmlTextReaderConstName(reader);
               if (type == 15 && !strcmp((const char*)name,shortCut)) {
                  finished = true;
                  break;
               }
               if (type == 1) {
                  if (!strcmp((const char*)name,"Author")) {
                     value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Name");
                     if (value!=NULL) strcpy(mainAuthor,(const char*)value);
                     xmlFree((void*)value);
                     value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Institute");
                     if (value!=NULL) strcpy(mainInstitute,(const char*)value);
                     xmlFree((void*)value);
                     value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Collaboration");
                     if (value!=NULL) strcpy(mainCollaboration,(const char*)value);
                     xmlFree((void*)value);
                     value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Email");
                     if (value!=NULL) strcpy(mainEmail,(const char*)value);
                     xmlFree((void*)value);
                  }
                  if (!strcmp((const char*)name,"Programname")) {
                     value = xmlTextReaderGetAttribute(reader,(xmlChar*)"Name");
                     if (value!=NULL) strcpy(mainProgName,(const char*)value);
                     xmlFree((void*)value);
                  }
                  if (!strcmp((const char*)name,"Folder")) {
                     numOfFolder = -1;
                     if (!XMLToFolder(reader)) return;
                  }
                  if (!strcmp((const char*)name,"Task")) {
                     numOfTask = -1;
                     if (!XMLToTask(reader)) return;
                  }
                  if (!strcmp((const char*)name,"Trees")) {
                     numOfTree = -1;
                     if (!XMLToTree(reader)) return;
                  }
               }
            }
         }
      }
      if (!WriteAnalyzer()) return;
      xmlFreeTextReader(reader);
   } 
   else {
      fprintf(stderr, "Unable to open %s\n", xmlFile);
   }
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

#if defined( _MSC_VER )
   sprintf(buffer,"ROOTINC = /I$(ROOTSYS)/include/\n");
   sprintf(buffer+strlen(buffer),"ROOTLIB = $(ROOTSYS)/lib/gdk-1.3.lib $(ROOTSYS)/lib/glib-1.3.lib $(ROOTSYS)/lib/libCint.lib $(ROOTSYS)/lib/libCore.lib $(ROOTSYS)/lib/libEG.lib $(ROOTSYS)/lib/libEGPythia6.lib $(ROOTSYS)/lib/libFumili.lib $(ROOTSYS)/lib/libGeom.lib $(ROOTSYS)/lib/libGeomPainter.lib $(ROOTSYS)/lib/libGpad.lib $(ROOTSYS)/lib/libGraf.lib $(ROOTSYS)/lib/libGraf3d.lib $(ROOTSYS)/lib/libGui.lib $(ROOTSYS)/lib/libHbook.lib $(ROOTSYS)/lib/libHist.lib $(ROOTSYS)/lib/libHistPainter.lib $(ROOTSYS)/lib/libHtml.lib $(ROOTSYS)/lib/libMLP.lib $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libMinuit.lib $(ROOTSYS)/lib/libPhysics.lib $(ROOTSYS)/lib/libPostscript.lib $(ROOTSYS)/lib/libProof.lib $(ROOTSYS)/lib/libProofGui.lib $(ROOTSYS)/lib/libRFIO.lib $(ROOTSYS)/lib/libRGL.lib $(ROOTSYS)/lib/libRint.lib $(ROOTSYS)/lib/libTable.lib $(ROOTSYS)/lib/libTree.lib $(ROOTSYS)/lib/libTreePlayer.lib $(ROOTSYS)/lib/libTreeViewer.lib $(ROOTSYS)/lib/libVMC.lib $(ROOTSYS)/lib/libWin32gdk.lib $(ROOTSYS)/lib/libfreetype.lib\n");
   sprintf(buffer+strlen(buffer),"LIBRARY = gdi32.lib user32.lib kernel32.lib $(ROMESYS)/lib_win/libxml2.lib $(ROMESYS)/lib_win/iconv.lib $(ROMESYS)/lib_win/zlib.lib\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"cfiles = *.cpp\n");
   sprintf(buffer+strlen(buffer),"romecfiles = $(ROMESYS)/src/ROME*.cpp\n");
   sprintf(buffer+strlen(buffer),"hfiles = *.h\n");
   sprintf(buffer+strlen(buffer),"romehfiles = $(ROMESYS)/include/ROME*.h\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"Flags = /GX\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"%s%s.exe: $(cfiles) $(romecfiles)\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"	cl $(Flags) /I$(ROMESYS)/include/ $(ROOTINC) /Fe%s%s.exe $(cfiles) $(romecfiles) $(LIBRARY) $(ROOTLIB)\n",shortCut,mainProgName);
#endif
#if defined ( __linux__ )
   int i;
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
   for (i=0;i<strlen(shortCut);i++) shortcut[i] = tolower(shortCut[i]);
   shortcut[i] = 0;
   for (i=0;i<strlen(mainProgName);i++) mainprogname[i] = tolower(mainProgName[i]);
   mainprogname[i] = 0;
   sprintf(buffer+strlen(buffer),"%s%s: $(cfiles) $(romecfiles) $(hfiles) $(romehfiles)\n",shortcut,mainprogname);
   sprintf(buffer+strlen(buffer),"	g++ -o $@ $(cfiles) $(romecfiles) -I$(ROOTSYS)/include/ -I$(ROMESYS)/include -lxml2 -lz -lpthread -lmidas -lHtml $(ROOTLIBS) $(SYSLIBS)\n");
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
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) sprintf(buffer2+strlen(buffer2),"%s%s.h ",shortCut,folderName[i]);
   }
   for (i=0;i<numOfTask;i++) {
      sprintf(buffer2+strlen(buffer2),"%sT%s.h ",shortCut,taskName[i]);
   }
   sprintf(buffer2+strlen(buffer2),"%sEventLoop.h %sAnalyzer.h ROMEAnalyzer.h ROMEEventLoop.h ROMETask.h ROMETree.h ROMERunTable.h \n",shortCut,shortCut);
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
   sprintf(buffer+strlen(buffer),"The %s%s consists mainly of folders and tasks.\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<a href=\"#folders\">Folders</a> are objects, where you can store data in. Typically you will store the data of one detector (or subdetector) component in it.\n");
   sprintf(buffer+strlen(buffer),"Like disk folders (directories) they are hierarchically arranged.\n");
   sprintf(buffer+strlen(buffer),"Folders may have a data structure (unlike disk folders). The data objects are called fields. Folders without fields can be used to structure the project.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<a href=\"#tasks\">Tasks</a> are objects, which privides actions. They make calculations, store and read data in folders, fill trees and histograms and so on.\n");
   sprintf(buffer+strlen(buffer),"Tasks are also hierarchically arranged. That means that a task may have a subtask, which is executed after the task itself has been executed.\n");
   sprintf(buffer+strlen(buffer),"Task also own histograms, which means that all histograms in this frame work belong to a task. The booking and saving of histograms is made by the frame work.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<H2>Classes of %s%s </H2>\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"In the following all classes of %s%s are discussed. For a quick overview of all classes check out the <A TARGET=_top HREF=\"%s/htmldoc/ClassIndex.html\">class index</A>.\n",shortCut,mainProgName,outDir);
   sprintf(buffer+strlen(buffer),"<p>\n");

// Analyzer
//----------
   sprintf(buffer+strlen(buffer),"<H3>Main Class <A TARGET=_top HREF=\"%s/htmldoc/%sAnalyzer.html\">%sAnalyzer</A></H3>\n",outDir,shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"This is the main class of the frame work. It instantiates the class %sEventLoop which\n",shortCut);
   sprintf(buffer+strlen(buffer),"starts the event loop.<p>\n");
   sprintf(buffer+strlen(buffer),"Further it provides the <a href=\"#getters\">getters</a> and <a href=\"#setters\">setters</a> of the folders\n");
   sprintf(buffer+strlen(buffer),"and methods for the <a href=\"#database\">data base access</a>, <a href=\"#tree\">tree handling</a> and the <a href=\"#steering\">program steering</a>.\n");
   sprintf(buffer+strlen(buffer),"All task can access these methods over their <b>fAnalyzer</b> handle, which every task has by\n");
   sprintf(buffer+strlen(buffer),"default.<p>\n");

// Folders
   sprintf(buffer+strlen(buffer),"<h4><a name=folders><u>Folders</u></a></h4>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"%s%s incorporates the following folders :\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<ul>\n");

   char parentt[100] = "";
   int depthold=0;
   int depth=0;
   for (i=0;i<numOfFolder;i++) {
      depth=0;
      bool found = false;
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
      if (numOfValue[i] > 0) sprintf(buffer+strlen(buffer),"<b>\n");
      if (depth%3==2) sprintf(buffer+strlen(buffer),"<li type=\"square\">%s</li>\n",folderName[i]);
      else if (depth%3==1) sprintf(buffer+strlen(buffer),"<li type=\"circle\">%s</li>\n",folderName[i]);
      else sprintf(buffer+strlen(buffer),"<li type=\"disc\">%s</li>\n",folderName[i]);
      if (numOfValue[i] > 0) sprintf(buffer+strlen(buffer),"</b>\n");
      depthold = depth;
   }
   for (i=0;i<depth;i++) sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"The bold folders are data folders. The others are only used to structure the data.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<a name=getters>The getters of the folders have the following nomenclature : </a><p>\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Folder Name</i>]At([<i>Index</i>])</b></td><td> : for object arrays.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Folder Name</i>]Object()</td><td> : for single objects.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Folder Name</i></b> stands for the name of the folder specified in the xml file (see also list above).</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Index</i></b> stands for the array index of the object.</br>\n");
   sprintf(buffer+strlen(buffer),"These methods return a pointer on the class of this folder.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<a name=setters>The setters of the folders have the following nomenclature : </a><p>\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Set[<i>Folder Name</i>]Object([<i>Index</i>],[<i>Field 1</i>],[<i>Field 2</i>],...)</b></td><td> : for object arrays.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Set[<i>Folder Name</i>]Object([<i>Field 1</i>],[<i>Field 2</i>],...)</b></td><td> : for single objects.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Folder Name</i></b> stands for the name of the folder specified in the xml file (see also list above).</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Index</i></b> stands for the array index of the object.</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Field #</i></b> stands for the fields of the folder as specified in the xml file.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

// trees
   sprintf(buffer+strlen(buffer),"<h4><a name=tree><u>Trees</u></a></h4>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"%s%s incorporates the following trees :\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<ul>\n");
   for (i=0;i<numOfTree;i++) {
      sprintf(buffer+strlen(buffer),"<li type=\"disc\">%s</li>\n",treeName[i]);
   }
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"The tree methods have the following nomenclature : <p>\n");
   sprintf(buffer+strlen(buffer),"<b>Get[<i>Tree Name</i>]Tree()</b>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Tree Name</i></b> stands for the name of the tree specified in the xml file (see also list above).</br>\n");
   sprintf(buffer+strlen(buffer),"These methods return a pointer on a ROMETree object, that contains the tree.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

// Data Base
   sprintf(buffer+strlen(buffer),"<h4><a name=database><u>Data Base</u></a></h4>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The following folders have data base access :\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<ul>\n");
   for (i=0;i<numOfFolder;i++) {
      if (dataBase[i]) {
         sprintf(buffer+strlen(buffer),"<li type=\"disc\">%s</li>\n",folderName[i]);
      }
   }
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"The data base methods have the following nomenclature : <p>\n");
   sprintf(buffer+strlen(buffer),"<b>Write[<i>Folder Name</i>]DataBase(this)</b>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Folder Name</i></b> stands for the name of the folder, which is to be written to the data base (see also list above).</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");

// General Methods
   sprintf(buffer+strlen(buffer),"<h4><a name=steering><u>General methods</u></a></h4>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"%s%s provides some general methods for the user.\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td>GetMidasEvent()</td><td> : returns a pointer on the buffer containing the current event data, that was read from a file or received online.</td></tr>\n");
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
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<p><b>Note</b> : The user may not add code to or alter code in this class. Both the class file (.h) and the methods file (.cpp) are generated by the framework. Manual changes to these two files will allways be overwritten.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");

// EventLoop
//-----------
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<H3>Event Loop Class <A TARGET=_top HREF=\"%s/htmldoc/%sEventLoop.html\">%sEventLoop</A></H3>\n",outDir,shortCut,shortCut);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"This class incorporates the event loop. </br>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"The event loop can be in the following five different stages : </br>\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td>Init</td><td> : at the start of the program.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>BeginOfRun</td><td> : at the beginning of a new run.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>Event</td><td> : every event.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>EndOfRun</td><td> : at the end of a run.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td>Terminate</td><td> : at the end of the program.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<p><b>Note</b> : The user may not add code to or alter code in this class. Both the class file (.h) and the methods file (.cpp) are generated by the framework. Manual changes to these two files will allways be overwritten.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"\n");

// Tasks
//-------
   sprintf(buffer+strlen(buffer),"<hr>\n");
   sprintf(buffer+strlen(buffer),"<H3><a name=tasks>Task Classes </a></H3>\n",shortCut);
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"Tasks provide the user with histogram methods.\n");
   sprintf(buffer+strlen(buffer),"<table border=\"0\">\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Fill[<i>Histogram Name</i>]At([<i>Index</i>],[<i>Value</i>],[<i>Weight</i>])</b></td><td> : to fill histogram arrays.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Fill[<i>Histogram Name</i>]([<i>Value</i>],[<i>Weight</i>])</b></td><td> : to fill single histograms.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Histogram Name</i>]HandleAt([<i>Index</i>])</b></td><td> : to get a pointer on a histogram of a histogram array.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"<tr><td><b>Get[<i>Histogram Name</i>]Handle()</b></td><td> : to get a pointer on a single histogram.</td></tr>\n");
   sprintf(buffer+strlen(buffer),"</table>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Histogram Name</i></b> stands for the name of the histogram specified in the xml file (see also the following task descriptions).</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Index</i></b> stands for the array index of the histogram.</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Value</i></b> is the value to fill into the histogram.</br>\n");
   sprintf(buffer+strlen(buffer),"<b><i>Weight</i></b> is the weight of the value to fill into the histogram.</br>\n");
   sprintf(buffer+strlen(buffer),"<p>\n");
   sprintf(buffer+strlen(buffer),"In the following all tasks of %s%s are discussed.\n",shortCut,mainProgName);
   sprintf(buffer+strlen(buffer),"<p>\n");

   depthold=0;
   depth=0;
   for (i=0;i<numOfTask;i++) {
      depth=0;
      bool found = false;
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
      if (depth%3==2) sprintf(buffer+strlen(buffer),"<li type=\"square\"><h4><A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">%sT%s</A></h4></li>\n",outDir,shortCut,taskName[i],shortCut,taskName[i]);
      else if (depth%3==1) sprintf(buffer+strlen(buffer),"<li type=\"circle\"><h4><A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">%sT%s</A></h4></li>\n",outDir,shortCut,taskName[i],shortCut,taskName[i]);
      else sprintf(buffer+strlen(buffer),"<li type=\"disc\"><h4><A TARGET=_top HREF=\"%s/htmldoc/%sT%s.html\">%sT%s</A></h4></li>\n",outDir,shortCut,taskName[i],shortCut,taskName[i]);
      depthold = depth;
      sprintf(buffer+strlen(buffer),"%s<p>\n",classDescription[i]);
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
      sprintf(buffer+strlen(buffer),"\n");
   }
   for (i=0;i<depth;i++) sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"</ul>\n");
   sprintf(buffer+strlen(buffer),"\n");
   sprintf(buffer+strlen(buffer),"<p><b>Note</b> : The user should write code into the Init(), BeginOfRun(), Event(), EndOfRun() and Terminate() methods of the methods file (.cpp). But the user may not add code to or alter code in the class file (.h). The class file (.h) is generated by the framework. Manual changes to this file will allways be overwritten.\n");
   sprintf(buffer+strlen(buffer),"<p>\n");

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

   sprintf(htmlFile,"%s%s.html",shortCut,mainProgName);
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

void ROMEBuilder::setValue(char *buf,char *destination,char *source,char *type)
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
      sprintf(buf,"%s = strtol(%s,&cstop,10)",destination,source);
   }
   else if (
       !strcmp(type,"bool") ||
       !strcmp(type,"Bool_t")) {
      sprintf(buf,"%s = toBool(strtol(%s,&cstop,10))",destination,source);
   }
   else if (
       !strcmp(type,"char") ||
       !strcmp(type,"unsigned char") ||
       !strcmp(type,"Char_t") ||
       !strcmp(type,"UChar_t") ||

       !strcmp(type,"Option_t") ||
       !strcmp(type,"Text_t")) {
      sprintf(buf,"strcpy(%s,%s)",destination,source);
   }
   else if (
       !strcmp(type,"float") ||
       !strcmp(type,"Float_t") ||

       !strcmp(type,"double") ||
       !strcmp(type,"Double_t") ||

       !strcmp(type,"Stat_t") ||
       !strcmp(type,"Axis_t")) {
      sprintf(buf,"%s = strtod(%s,&cstop)",destination,source);
   }
   else {
      sprintf(buf,"%s = %s",destination,source);
   }
}

