/********************************************************************

  ROMEBuilder.cpp, M. Schneebeli PSI

  $Log$
  Revision 1.111  2005/03/17 17:46:48  sawada
  small bug fix.

  Revision 1.110  2005/03/17 15:44:02  schneebeli_m
  Makefile for windows

  Revision 1.109  2005/03/11 17:56:12  sawada
  Added XXXGlobalSteering.h to dependence of XXXAnalyzer.obj.

  Revision 1.108  2005/03/08 15:23:27  sawada
  Added UserClassHeaders and task dependences in Makefile for UNIX.

  Revision 1.107  2005/03/08 11:00:36  sawada
  Improvement Makefile for UNIX.

  Revision 1.106  2005/02/25 14:40:19  sawada
  fixed a bug at InitMidasBanks when there are no midas banks.

  Revision 1.105  2005/02/21 21:29:06  sawada

  Changed OS specifying macros
  Support for DEC,Ultrix,FreeBSD,Solaris

  Revision 1.104  2005/02/07 18:47:35  schneebeli_m
  GSP sdt::string

  Revision 1.103  2005/02/07 18:12:53  schneebeli_m
  suppress fortran analyzer

  Revision 1.102  2005/01/27 16:21:05  schneebeli_m
  print method & no gROME in path

  Revision 1.101  2005/01/25 16:36:22  schneebeli_m
  histo label

  Revision 1.100  2005/01/25 14:30:20  schneebeli_m
  small changes

  Revision 1.99  2005/01/24 16:29:40  schneebeli_m
  last bank access

  Revision 1.98  2005/01/24 15:20:32  schneebeli_m
  Seperated DAQ classes

  Revision 1.97  2005/01/20 10:32:23  sawada
  Makefile for linux.

  Revision 1.96  2005/01/20 09:56:14  sawada
  Makefile for linux. Added -DOS_UNIX

  Revision 1.95  2005/01/20 09:25:37  sawada
  Makefile for linux.

  Revision 1.94  2005/01/18 15:29:16  schneebeli_m
  std::string init

  Revision 1.93  2005/01/18 15:12:43  schneebeli_m
  include <string>

  Revision 1.92  2005/01/17 08:54:36  schneebeli_m
  String SP

  Revision 1.91  2005/01/14 19:17:05  sawada
  Makefile for UNIX
   (i)   removed -g. Debug and optimization flags can be specified in Makefile.usr.
   (ii)  removed -I$(ROMESYS)/include/mysql/.
   (iii) small changes.

  Revision 1.90  2005/01/14 17:13:52  schneebeli_m
  HistoArrayStartIndex

  Revision 1.89  2005/01/14 12:44:19  schneebeli_m
  midas path in makefile

  Revision 1.88  2005/01/13 09:11:28  schneebeli_m
  Description text overflow

  Revision 1.87  2005/01/11 08:04:16  schneebeli_m
  UserHTML

  Revision 1.86  2005/01/11 08:00:43  schneebeli_m
  UserHTML

  Revision 1.85  2005/01/10 16:50:47  schneebeli_m
  TaskDiscription in HTML

  Revision 1.84  2005/01/07 11:37:32  schneebeli_m
  Bank Arrays, Folder Array Getter

  Revision 1.83  2005/01/06 10:33:42  schneebeli_m
  Added Makefile.usr

  Revision 1.82  2005/01/05 10:37:05  schneebeli_m
  Bank enumeration

  Revision 1.81  2004/12/21 08:49:54  schneebeli_m
  Config SP read error

  Revision 1.80  2004/12/09 08:22:19  schneebeli_m
  lg2c

  Revision 1.79  2004/12/07 11:43:39  schneebeli_m
  error when no database folder

  Revision 1.78  2004/12/06 16:03:02  sawada
  code cleanup (tab -> space)

  Revision 1.77  2004/12/06 09:04:34  schneebeli_m
  minor changes

  Revision 1.76  2004/12/03 14:42:08  schneebeli_m
  some minor changes

  Revision 1.75  2004/12/02 17:46:43  sawada
  Macintosh port

  Revision 1.74  2004/11/23 08:54:25  schneebeli_m
  steering parameter hierarchy error removed

  Revision 1.73  2004/11/19 13:29:55  schneebeli_m
  added stuff for sample

  Revision 1.72  2004/11/18 13:35:24  schneebeli_m
  Steering hierarchy error removed

  Revision 1.71  2004/11/17 16:13:49  schneebeli_m
  config structure changed

  Revision 1.70  2004/11/16 16:20:59  schneebeli_m
  detail

  Revision 1.69  2004/11/16 16:14:00  schneebeli_m
  implemented task hierarchy

  Revision 1.68  2004/11/12 17:35:17  schneebeli_m
  fast xml database

  Revision 1.67  2004/11/11 14:07:15  schneebeli_m
  ROMEStrArray and ROMEStr2DArray change

  Revision 1.66  2004/11/11 13:17:37  schneebeli_m
  detail

  Revision 1.65  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.64  2004/10/25 08:03:41  schneebeli_m
  html docu and tree switches

  Revision 1.63  2004/10/21 13:03:51  schneebeli_m
  *** empty log message ***

  Revision 1.62  2004/10/20 09:22:16  schneebeli_m
  bugs removed

  Revision 1.61  2004/10/19 21:10:08  pierre
  add lutil for forkpty

  Revision 1.60  2004/10/15 11:51:28  schneebeli_m
  bugs removed

  Revision 1.59  2004/10/14 15:09:12  schneebeli_m
  changed Get[Histo]Handle() -> Get[Histo]

  Revision 1.58  2004/10/14 10:53:17  schneebeli_m
  bug fixed

  Revision 1.57  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

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

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#include <direct.h>
#endif
#if defined( R__UNIX )
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#endif
#ifndef R__MACOSX
#if defined( R__VISUAL_CPLUSPLUS )
#include <io.h>
#endif
#if defined( R__UNIX )
#include <sys/io.h>
#endif
#endif // R__MACOSX
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
         if (parent[recursiveDepth].Length()==0)
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

      // Methods

      buffer.AppendFormatted("public:\n");
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

      // Getters
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
         histoType[numOfTask][numOfHistos[numOfTask]] = "";
         histoArray[numOfTask][numOfHistos[numOfTask]] = "1";
         histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]] = "1";
         histoXLabel[numOfTask][numOfHistos[numOfTask]] = "\"\"";
         histoYLabel[numOfTask][numOfHistos[numOfTask]] = "\"\"";
         histoZLabel[numOfTask][numOfHistos[numOfTask]] = "\"\"";
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
            // histo array start index
            if (type == 1 && !strcmp((const char*)name,"HistArrayStartIndex"))
               xml->GetValue(histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]],histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]]);
            // histo label of x axis
            if (type == 1 && !strcmp((const char*)name,"HistXLabel"))
               xml->GetValue(histoXLabel[numOfTask][numOfHistos[numOfTask]],histoXLabel[numOfTask][numOfHistos[numOfTask]]);
            // histo label of y axis
            if (type == 1 && !strcmp((const char*)name,"HistYLabel"))
               xml->GetValue(histoYLabel[numOfTask][numOfHistos[numOfTask]],histoYLabel[numOfTask][numOfHistos[numOfTask]]);
            // histo label of z axis
            if (type == 1 && !strcmp((const char*)name,"HistZLabel"))
               xml->GetValue(histoZLabel[numOfTask][numOfHistos[numOfTask]],histoZLabel[numOfTask][numOfHistos[numOfTask]]);
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
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int lenTot,ll,i,j;
   char *pBuffer=NULL;
   int bufferLen=0;
   char *pos;
   int fileHandle;
   ROMEString format;
   ROMEString discript;
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
         buffer.AppendFormatted("// %-74.74s   \n",pos);
         pos = pos+i+1;
      }
      buffer.AppendFormatted("// \n");
      buffer.AppendFormatted("// \n");
      buffer.AppendFormatted("// This header has been generated by the ROMEBuilder.\n");
      if (!taskFortran[iTask]) {
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
            buffer.AppendFormatted("// Get<Histogram Name>()\n");
            buffer.AppendFormatted("//\n");
            buffer.AppendFormatted("// For histogram arrays use :\n");
            buffer.AppendFormatted("//\n");
            buffer.AppendFormatted("// Get<Histogram Name>At(int index)\n");
         }
      }
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("/////////////////////////////////////----///////////////////////////////////////");

      // Write file
      bool replaceHeader = true;
      bool replaceBody = true;
      struct stat buf;
      int nb=0;
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
#endif
         bool writeFile = false;
         if( !stat( cppFile.Data(), &buf )) {
            fileHandle = open(cppFile.Data(),O_RDONLY);
            nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
            close(fileHandle);
            if (buffer.Length()!=nb) {
               writeFile = true;
            }
            else {
               for (i=0;i<nb&&i<(int)buffer.Length();i++) {
                  if (buffer[i] != fileBuffer[i]) {
                     writeFile = true;
                     break;
                  }
               }
            }
         }
         else {
            writeFile = true;
         }
         if (writeFile) {
            fileHandle = open(cppFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
            close(fileHandle);
            fileHandle = open(cppFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
            nb = write(fileHandle,buffer.Data(), buffer.Length());
            if (makeOutput) cout << "yy      " << cppFile.Data() << endl;
         }
         return true;
      }
      // c++ task
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
            buffer.AppendFormatted("void %sT%s::Init()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
            buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
            buffer.AppendFormatted("void %sT%s::Event()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
            buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
            buffer.AppendFormatted("void %sT%s::Terminate()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());

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
         fFile.SetFormatted("%s/src/tasks/%sTF%s.f",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

         ROMEString shortcut(shortCut);
         shortcut.ToLower();
         ROMEString taskname(taskName[iTask]);
         taskname.ToLower();

         if( stat( fFile.Data(), &buf )) {
            fileHandle = open(fFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
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
      if (taskUserCode[iTask])
         hFile.SetFormatted("%s/include/tasks/%sT%s_Base.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      else
         hFile.SetFormatted("%s/include/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
      buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
      buffer.AppendFormatted("//                                                                            //\n");
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

      // Header
      if (taskUserCode[iTask]) {
         buffer.AppendFormatted("#ifndef %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_Base_H\n\n",shortCut.Data(),taskName[iTask].Data());
      }
      else {
         buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());
      }

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
      if (taskUserCode[iTask])
         buffer.AppendFormatted("   %sT%s_Base(const char *name,const char *title):ROMETask(name,title)\n",shortCut.Data(),taskName[iTask].Data());
      else 
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
         buffer.AppendFormatted("   Bool_t is%sAccumulation() { return f%sAccumulation; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sAccumulation(Bool_t flag) { f%sAccumulation = flag; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
      }


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
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline void %sT%s_Base::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      else
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
            buffer.AppendFormatted("   f%s->GetXaxis()->SetTitle(%s);\n",histoName[iTask][i].Data(),histoXLabel[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetYaxis()->SetTitle(%s);\n",histoName[iTask][i].Data(),histoYLabel[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetZaxis()->SetTitle(%s);\n",histoName[iTask][i].Data(),histoZLabel[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   %s *hist%d;\n",histoType[iTask][i].Data(),i);
            buffer.AppendFormatted("   f%ss = new TObjArray(%s);\n",histoName[iTask][i].Data(),histoArray[iTask][i].Data());
            if (!sameFolder&&!homeFolder)
               buffer.AppendFormatted("   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",\"%s\");\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data(),histoFolderTitle[iTask][i].Data());
            buffer.AppendFormatted("   for (j=0;j<%s;j++) {\n",histoArray[iTask][i].Data());
            buffer.AppendFormatted("      name.SetFormatted(\"%%0*d\",3,j+%s);\n",histoArrayStartIndex[iTask][i].Data());
            buffer.AppendFormatted("      name.Insert(0,\"%s_\");\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      title.SetFormatted(\"%%0*d\",3,j+%s);\n",histoArrayStartIndex[iTask][i].Data());
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
               buffer.AppendFormatted("      %sFolder->Add(f%ss->At(j));\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
            else
               buffer.AppendFormatted("      GetHistoFolder()->Add(f%ss->At(j));\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      hist%d->GetXaxis()->SetTitle(%s);\n",i,histoXLabel[iTask][i].Data());
            buffer.AppendFormatted("      hist%d->GetYaxis()->SetTitle(%s);\n",i,histoYLabel[iTask][i].Data());
            buffer.AppendFormatted("      hist%d->GetZaxis()->SetTitle(%s);\n",i,histoZLabel[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline void %sT%s_Base::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      else
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

      if (taskUserCode[iTask])
         buffer.AppendFormatted("#endif   // %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

      // Write File
      fileHandle = open(hFile.Data(),O_RDONLY);
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
         fileHandle = open(hFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
         close(fileHandle);
         fileHandle = open(hFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
         if (makeOutput) cout << "      " << hFile.Data() << endl;
         nb = write(fileHandle,buffer.Data(), buffer.Length());
         close(fileHandle);
      }

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
         buffer.AppendFormatted("   %sT%s(const char *name,const char *title):%sT%s_Base(name,title) {};\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
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
   ROMEString tmp;
   char *cstop;

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
         bankArraySize[numOfBank] = 0;
         bankArrayStart[numOfBank] = 0;
         bankArrayDigit[numOfBank] = 0;
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
            // bank array
            if (type == 1 && !strcmp((const char*)name,"BankArray")) {
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // lower boundary
                  if (type == 1 && !strcmp((const char*)name,"BASize")) {
                     xml->GetValue(tmp,"");
                     bankArraySize[numOfBank] = strtol(tmp,&cstop,10);
                  }
                  // upper boundary
                  if (type == 1 && !strcmp((const char*)name,"BAStartIndex")) {
                     xml->GetValue(tmp,"");
                     bankArrayStart[numOfBank] = strtol(tmp,&cstop,10);
                  }
                  // number of digits
                  if (type == 1 && !strcmp((const char*)name,"BANumberOfDigits")) {
                     xml->GetValue(tmp,"");
                     bankArrayDigit[numOfBank] = strtol(tmp,&cstop,10);
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
            // bank array
            if (type == 1 && !strcmp((const char*)name,"BankArray")) {
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // lower boundary
                  if (type == 1 && !strcmp((const char*)name,"BASize")) {
                     xml->GetValue(tmp,"");
                     bankArraySize[numOfBank] = strtol(tmp,&cstop,10);
                  }
                  // upper boundary
                  if (type == 1 && !strcmp((const char*)name,"BAStartIndex")) {
                     xml->GetValue(tmp,"");
                     bankArrayStart[numOfBank] = strtol(tmp,&cstop,10);
                  }
                  // number of digits
                  if (type == 1 && !strcmp((const char*)name,"BANumberOfDigits")) {
                     xml->GetValue(tmp,"");
                     bankArrayDigit[numOfBank] = strtol(tmp,&cstop,10);
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
   if (numOfSteering[iTask]>0)
      steerName[iTask][numOfSteering[iTask]] = "";
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
      // steering parameter field
      if (type == 1 && !strcmp((const char*)name,"SteeringParameterField")) {
         // include initialisation
         bool readName = false;
         bool readType = false;
         steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
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
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "' '";
               else if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "TRef")
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "NULL";
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
//               xml->GetValue(steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
            }
            // steering parameter field comment
            if (type == 1 && !strcmp((const char*)name,"SPFieldComment")) {
               xml->GetValue(steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               if (steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][0]!='/') {
                  steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].Insert(0,"// ");
               }
            }
            // steering parameter field end
            if (type == 15 && !strcmp((const char*)name,"SteeringParameterField"))
               break;
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

bool ROMEBuilder::WriteSteering(int iTask) {
   int i;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   hFile.SetFormatted("%s/include/framework/%sGlobalSteering.h",outDir.Data(),shortCut.Data());

   if (numOfSteering[numOfTaskHierarchy]==-1) {
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
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sGlobalSteering_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sGlobalSteering_H\n\n",shortCut.Data());
   buffer.AppendFormatted("#include <string>\n\n");

   WriteSteeringClass(buffer,0,iTask,0);

   buffer.AppendFormatted("#endif   // %sGlobalSteering_H\n",shortCut.Data());

   int fileHandle = open(hFile.Data(),O_RDONLY);
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
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Analyzer");
   format.Resize(0);
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
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
   buffer.AppendFormatted("#include <direct.h>\n");
   buffer.AppendFormatted("#endif\n");

   buffer.AppendFormatted("#include <sys/stat.h>\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
   buffer.AppendFormatted("#include <TObjString.h>\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#if defined( HAVE_SQL )\n");
   buffer.AppendFormatted("#include <ROMESQLDataBase.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#include <ROMEStr2DArray.h>\n");
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
   buffer.AppendFormatted("%sAnalyzer::%sAnalyzer(TRint *app):ROMEAnalyzer(app) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("// Folder and Task initialisation\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gPassToROME = (void*)this; // Pass the handle to the framework\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fConfiguration = new %sConfig();\n",shortCut.Data());
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
      if (folderParentName[i]=="GetMainFolder()") parentt = folderParentName[i];
      else parentt.SetFormatted("%sFolder",folderParentName[i].Data());
      format.SetFormatted("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-folderName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),folderName[i].Data(),"",parentt.Data(),folderName[i].Data(),folderTitle[i].Data());
   }
   for (i=0;i<numOfFolder;i++) {
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
   buffer.AppendFormatted("\n");

   // Task
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   fMainTask = new %sEventLoop(\"mainTask\",\"Main Task of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   fMainFolder->Add(fMainTask);\n");
   buffer.AppendFormatted("   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   int taskLen=0;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (taskLen<(int)taskHierarchyName[i].Length()) taskLen = taskHierarchyName[i].Length();
   }
   for (i=0;i<numOfTaskHierarchy;i++) {
      format.SetFormatted("   f%%s%%03dTask%%%ds = new %%sT%%s(\"%%s\",\"%%s\");\n",taskLen-taskHierarchyName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),taskHierarchyName[i].Data(),i,"",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),"");
      buffer.AppendFormatted("   ((%sT%s*)f%s%03dTask)->SetActive(false);\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),i);
      if (taskHierarchyParentIndex[i]==-1) 
         parentt = "GetMainTask()";
      else
         parentt.SetFormatted("f%s%03dTask",taskHierarchyName[taskHierarchyParentIndex[i]].Data(),taskHierarchyParentIndex[i]);
      buffer.AppendFormatted("   %s->Add(f%s%03dTask);\n",parentt.Data(),taskHierarchyName[i].Data(),i);
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("}\n\n");


   // Midas Bank Initialisation
   buffer.AppendFormatted("// Midas Bank Initialisation\n");
   buffer.AppendFormatted("void %sAnalyzer::InitMidasBanks() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfBank;i++) {
      // Bank Array
      if (bankArrayDigit[i]>0) {
         buffer.AppendFormatted("   for (i=%d;i<%d;i++)\n",bankArrayStart[i],bankArraySize[i]+bankArrayStart[i]);
         buffer.AppendFormatted("      this->Init%sBank(i);\n",bankName[i].Data());
      }
      // Single Bank
      else {
         buffer.AppendFormatted("   this->Init%sBank();\n",bankName[i].Data());
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Midas Bank Methodes
   buffer.AppendFormatted("\n// Midas Bank Getters\n");
   for (i=0;i<numOfBank;i++) {
      // Bank Array
      if (bankArrayDigit[i]>0) {
         // Functions
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            buffer.AppendFormatted("%sStruct* %sAnalyzer::Get%sBankAt(int bankIndex,int index) {\n",bankName[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i].Data());
            buffer.AppendFormatted("      return f%sBankPointer[bankIndex]+index;\n",bankName[i].Data());
            buffer.AppendFormatted("   return NULL;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("%sStruct* %sAnalyzer::GetLast%sBankAt(int bankIndex,int index) {\n",bankName[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i].Data());
            buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex]+index;\n",bankName[i].Data());
            buffer.AppendFormatted("   return NULL;\n");
            buffer.AppendFormatted("}\n");
         }
         else {
            buffer.AppendFormatted("%s %sAnalyzer::Get%sBankAt(int bankIndex,int index) {\n",bankType[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i].Data());
            buffer.AppendFormatted("      return *(f%sBankPointer[bankIndex]+index);\n",bankName[i].Data());
            buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("%s %sAnalyzer::GetLast%sBankAt(int bankIndex,int index) {\n",bankType[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i].Data());
            buffer.AppendFormatted("      return *(fLast%sBankPointer[bankIndex]+index);\n",bankName[i].Data());
            buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i].Data());
            buffer.AppendFormatted("}\n");
         }
         buffer.AppendFormatted("void %sAnalyzer::Init%sBank(int bankIndex) {\n",shortCut.Data(),bankName[i].Data());
         buffer.AppendFormatted("   fLast%sBankExists[bankIndex] = f%sBankExists[bankIndex];\n",bankName[i].Data(),bankName[i].Data());
         buffer.AppendFormatted("   fLast%sBankPointer[bankIndex] = f%sBankPointer[bankIndex];\n",bankName[i].Data(),bankName[i].Data());
         buffer.AppendFormatted("   fLast%sBankLength[bankIndex] = f%sBankLength[bankIndex];\n",bankName[i].Data(),bankName[i].Data());
         buffer.AppendFormatted("   unsigned long bktype;\n");
         buffer.AppendFormatted("   EVENT_HEADER *pevent = (EVENT_HEADER*)this->GetRawDataEvent();\n");
         buffer.AppendFormatted("   pevent++;\n");
         buffer.AppendFormatted("   ROMEString bankname;\n");
         buffer.AppendFormatted("   bankname.SetFormatted(\"%s%%0%dd\",bankIndex);\n",bankName[i].Data(),bankArrayDigit[i]);
         buffer.AppendFormatted("   if (bk_find((BANK_HEADER*)pevent, bankname.Data(), (unsigned long*)&f%sBankLength[bankIndex], &bktype, (void**)&f%sBankPointer[bankIndex])) {\n",bankName[i].Data(),bankName[i].Data());
         buffer.AppendFormatted("      f%sBankExists[bankIndex] = true;\n",bankName[i].Data());
         buffer.AppendFormatted("      return;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   f%sBankExists[bankIndex] = false;\n",bankName[i].Data());
         buffer.AppendFormatted("   f%sBankPointer[bankIndex] = NULL;\n",bankName[i].Data());
         buffer.AppendFormatted("   f%sBankLength[bankIndex] = 0;\n",bankName[i].Data());
         buffer.AppendFormatted("   return;\n");
         buffer.AppendFormatted("}\n");

         buffer.AppendFormatted("int %sAnalyzer::Get%sBankEntries(int bankIndex) {\n",shortCut.Data(),bankName[i].Data());
         buffer.AppendFormatted("   return f%sBankLength[bankIndex];\n",bankName[i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("int %sAnalyzer::GetLast%sBankEntries(int bankIndex) {\n",shortCut.Data(),bankName[i].Data());
         buffer.AppendFormatted("   return fLast%sBankLength[bankIndex];\n",bankName[i].Data());
         buffer.AppendFormatted("}\n\n");
      }
      // Single Bank
      else {
         if (bankType[i]=="structure"||bankType[i]=="struct") {
            buffer.AppendFormatted("%sStruct* %sAnalyzer::Get%sBankAt(int index) {\n",bankName[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i].Data());
            buffer.AppendFormatted("      return f%sBankPointer+index;\n",bankName[i].Data());
            buffer.AppendFormatted("   return NULL;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("%sStruct* %sAnalyzer::GetLast%sBankAt(int index) {\n",bankName[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i].Data());
            buffer.AppendFormatted("      return fLast%sBankPointer+index;\n",bankName[i].Data());
            buffer.AppendFormatted("   return NULL;\n");
            buffer.AppendFormatted("}\n");
         }
         else {
            buffer.AppendFormatted("%s %sAnalyzer::Get%sBankAt(int index) {\n",bankType[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i].Data());
            buffer.AppendFormatted("      return *(f%sBankPointer+index);\n",bankName[i].Data());
            buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("%s %sAnalyzer::GetLast%sBankAt(int index) {\n",bankType[i].Data(),shortCut.Data(),bankName[i].Data());
            buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i].Data());
            buffer.AppendFormatted("      return *(fLast%sBankPointer+index);\n",bankName[i].Data());
            buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i].Data());
            buffer.AppendFormatted("}\n");
         }
         buffer.AppendFormatted("void %sAnalyzer::Init%sBank() {\n",shortCut.Data(),bankName[i].Data());
         buffer.AppendFormatted("   fLast%sBankExists = f%sBankExists;\n",bankName[i].Data(),bankName[i].Data());
         buffer.AppendFormatted("   fLast%sBankPointer = f%sBankPointer;\n",bankName[i].Data(),bankName[i].Data());
         buffer.AppendFormatted("   fLast%sBankLength = f%sBankLength;\n",bankName[i].Data(),bankName[i].Data());
         buffer.AppendFormatted("   unsigned long bktype;\n");
         buffer.AppendFormatted("   EVENT_HEADER *pevent = (EVENT_HEADER*)this->GetRawDataEvent();\n");
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
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("int %sAnalyzer::GetLast%sBankEntries() {\n",shortCut.Data(),bankName[i].Data());
         buffer.AppendFormatted("   return fLast%sBankLength;\n",bankName[i].Data());
         buffer.AppendFormatted("}\n\n");
      }
   }



   int ndb = 0;
   for (i=0;i<numOfFolder;i++) if (folderDataBase[i]) ndb++;

   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("bool %sAnalyzer::ReadSingleDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      buffer.AppendFormatted("   ROMEString path;\n");
      buffer.AppendFormatted("   int i;\n");
      buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
      buffer.AppendFormatted("   char *cstop;\n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (folderArray[i]=="1") {
                  buffer.AppendFormatted("   values->RemoveAll();\n");
                  buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path,gAnalyzer->GetCurrentRunNumber()))\n");
                  buffer.AppendFormatted("      return false;\n");
                  if (valueArray[i][j]=="1") {
                     buffer.AppendFormatted("   if (values->At(0,0).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(0,0).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("      f%sFolder->Set%s((%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("   else\n");
                     buffer.AppendFormatted("      f%sFolder->Set%s(%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  }
                  else {
                     buffer.AppendFormatted("   for (i=0;i<%s;i++) {\n",valueArray[i][j].Data());
                     buffer.AppendFormatted("      if (values->At(0,i).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(0,i).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(i,(%s)%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("      else\n");
                     buffer.AppendFormatted("         f%sFolder->Set%sAt(i,%s);\n",folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                     buffer.AppendFormatted("   }\n");
                  }
               }
            }
         }
      }
      buffer.AppendFormatted("   values->RemoveAll();\n");
      buffer.AppendFormatted("   delete values;\n");
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // ReadArrayDataBaseFolders
   buffer.AppendFormatted("bool %sAnalyzer::ReadArrayDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      buffer.AppendFormatted("   ROMEString path;\n");
      buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
      buffer.AppendFormatted("   char *cstop;\n");
      buffer.AppendFormatted("   int i,j;\n");
      for (i=0;i<numOfFolder;i++) {
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (folderArray[i]!="1") {
                  buffer.AppendFormatted("   values->RemoveAll();\n");
                  buffer.AppendFormatted("   path.SetFormatted(%s);\n",valueDataBasePath[i][j].Data());
                  buffer.AppendFormatted("   if (!this->GetDataBase()->Read(values,path,gAnalyzer->GetCurrentRunNumber()))\n");
                  buffer.AppendFormatted("      return false;\n");
                  buffer.AppendFormatted("   for (i=0;i<f%sFolders->GetEntries();i++) {\n",folderName[i].Data());
                  if (valueArray[i][j]=="1") {
                     buffer.AppendFormatted("      if (values->At(i,0).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(i,0).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("         ((%s%s*)f%sFolders->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("      else\n");
                     buffer.AppendFormatted("         ((%s%s*)f%sFolders->At(i))->Set%s(%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                  }
                  else {
                     buffer.AppendFormatted("      for (j=0;j<%s;j++) {\n",valueArray[i][j].Data());
                     buffer.AppendFormatted("         if (values->At(i,j).Length()!=0)\n");
                     setValue(&buf,(char*)valueName[i][j].Data(),"values->At(i,j).Data()",(char*)valueType[i][j].Data(),1);
                     buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%sAt(j,(%s)%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueType[i][j].Data(),buf.Data());
                     buffer.AppendFormatted("         else\n");
                     buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%sAt(j,%s);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueInit[i][j].Data());
                     buffer.AppendFormatted("      }\n");
                  }
                  buffer.AppendFormatted("   }\n");
               }
            }
         }
      }
      buffer.AppendFormatted("   values->RemoveAll();\n");
      buffer.AppendFormatted("   delete values;\n");
   }
   buffer.AppendFormatted("   return true;\n");
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
                  buffer.AppendFormatted("      value.SetFormatted(\"%s\",f%sFolder->Get%s().Data());\n",format.Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("      value.SetFormatted(\"%s\",f%sFolder->Get%s());\n",format.Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               buffer.AppendFormatted("      xml->WriteElement(\"%s\",value.Data());\n",valueName[i][j].Data());
            }
            buffer.AppendFormatted("      xml->EndElement();\n");
         }
         else {
            buffer.AppendFormatted("      for (i=0;i<f%sFolders->GetEntries();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("         xml->StartElement(\"%s\");\n",folderName[i].Data());
            buffer.AppendFormatted("         value.SetFormatted(\"%%d\",i);\n");
            buffer.AppendFormatted("         xml->WriteAttribute(\"Number\",value.Data());\n");
            for (j=0;j<numOfValue[i];j++) {
               GetFormat(&format,(char*)valueType[i][j].Data());
               if (valueType[i][j]=="TString") {
                  buffer.AppendFormatted("         value.SetFormatted(\"%s\",((%s%s*)f%sFolders->At(i))->Get%s().Data());\n",format.Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("         value.SetFormatted(\"%s\",((%s%s*)f%sFolders->At(i))->Get%s());\n",format.Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
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

   bool haveFortranTask = false;
   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         haveFortranTask = true;
         break;
      }
   }

   if (haveFortranTask) {
      buffer.AppendFormatted("// Interface to Fortran Tasks\n");
      for (i=0;i<numOfFolder;i++) {
         for (j=0;j<numOfValue[i];j++) {
            if (!isNumber(valueType[i][j]))
                continue;
            if (folderArray[i]=="1") {
               if (valueArray[i][j]=="1") {
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
               if (valueArray[i][j]=="1") {
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

   // Close cpp-File
   fileHandle = open(cppFile.Data(),O_RDONLY);
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
   ROMEString bankname;
   ROMEString format;
   // max folder name length
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
      if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
   }
   // max task name length
   int taskLen=0;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (taskLen<(int)taskHierarchyName[i].Length()) taskLen = taskHierarchyName[i].Length();
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
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
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
   buffer.AppendFormatted("#include <include/framework/%sConfig.h>\n",shortCut.Data());

   // include
   if (numOfSteering[numOfTaskHierarchy]>0) {
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
   for (i=0;i<numOfTaskHierarchy;i++) {
      int index = taskHierarchyParentIndex[i];
      switchString = taskHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      format.SetFormatted("   int %%s;%%%ds  //! %%s Task\n",switchLen-switchString.Length());
      buffer.AppendFormatted((char*)format.Data(),switchString.Data(),"",switchString.Data());
//      buffer.AppendFormatted("   int %s;   //! %s Task\n",switchString.Data(),switchString.Data());
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
            format.SetFormatted("   %%s%%s*%%%ds f%%sFolder; %%%ds // Handle to %%s%%s Folder\n",typeLen-folderName[i].Length()-scl,nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
         else {
            format.SetFormatted("   TClonesArray*%%%ds f%%sFolders;%%%ds // Handle to %%s%%s Folders\n",typeLen-12,nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task Fields
   buffer.AppendFormatted("   // Task Fields\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      format.SetFormatted("   ROMETask* f%%s%%03dTask;%%%ds  // Handle to %%s Task\n",taskLen-taskHierarchyName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),taskHierarchyName[i].Data(),i,"",taskHierarchyName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Steering Fields
   if (numOfSteering[numOfTaskHierarchy]>0) {
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
         if (bankNameLen<(int)bankName[i].Length()+bankArrayDigit[i]) bankNameLen = bankName[i].Length()+bankArrayDigit[i];
      }
      buffer.AppendFormatted("   // Bank Fields\n");
      for (i=0;i<numOfBank;i++) {
         // Bank Array
         if (bankArrayDigit[i]>0) {
            if (bankType[i]=="structure"||bankType[i]=="struct") {
               format.SetFormatted("   %%sStruct*%%%ds f%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankName[i].Length()-6,bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankName[i].Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
               format.SetFormatted("   %%sStruct*%%%ds fLast%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankName[i].Length()-6,bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankName[i].Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
            }
            else {
               format.SetFormatted("   %%s*%%%ds f%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i].Length(),bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankType[i].Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
               format.SetFormatted("   %%s*%%%ds fLast%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankType[i].Length(),bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankType[i].Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
            }
            format.SetFormatted("   int%%%ds f%%sBankLength[%%d];  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-2,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
            format.SetFormatted("   int%%%ds fLast%%sBankLength[%%d];  %%%ds //! Length  of the %%s Bank of the last event\n",bankTypeLen-2,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
            format.SetFormatted("   bool%%%ds f%%sBankExists[%%d];  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
            format.SetFormatted("   bool%%%ds fLast%%sBankExists[%%d];  %%%ds //! Exist Flags of the %%s Bank of the last event\n",bankTypeLen-3,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),bankArraySize[i]+bankArrayStart[i],"",bankName[i].Data());
         }
         // Single Bank
         else {
            if (bankType[i]=="structure"||bankType[i]=="struct") {
               format.SetFormatted("   %%sStruct*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankName[i].Length()-6,bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankName[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
               format.SetFormatted("   %%sStruct*%%%ds fLast%%sBankPointer; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankName[i].Length()-6,bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankName[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
            }
            else {
               format.SetFormatted("   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i].Length(),bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankType[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
               format.SetFormatted("   %%s*%%%ds fLast%%sBankPointer; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankType[i].Length(),bankNameLen-bankName[i].Length());
               buffer.AppendFormatted((char*)format.Data(),bankType[i].Data(),"",bankName[i].Data(),"",bankName[i].Data());
            }
            format.SetFormatted("   int%%%ds f%%sBankLength;  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-2,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
            format.SetFormatted("   int%%%ds fLast%%sBankLength;  %%%ds //! Length  of the %%s Bank of the last event\n",bankTypeLen-2,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
            format.SetFormatted("   bool%%%ds f%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-3,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
            format.SetFormatted("   bool%%%ds fLast%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank of the last event\n",bankTypeLen-3,bankNameLen-bankName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",bankName[i].Data(),"",bankName[i].Data());
         }
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
            format.SetFormatted("   %%s%%s*%%%ds  Get%%s()%%%ds { return f%%sFolder;%%%ds };\n",typeLen-folderName[i].Length()-scl,11+nameLen-folderName[i].Length(),15+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   %%s%%s**%%%ds Get%%sAddress()%%%ds { return &f%%sFolder;%%%ds };\n",typeLen-folderName[i].Length()-scl,4+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
         else if (folderArray[i]=="variable") {
            format.SetFormatted("   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds\n",typeLen-folderName[i].Length()-scl,0+nameLen-folderName[i].Length(),lt);
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),"");
            buffer.AppendFormatted("   { if (f%sFolders->GetEntries()<=index)\n",folderName[i].Data());
            buffer.AppendFormatted("        for (int i=f%sFolders->GetEntries();i<=index;i++)\n",folderName[i].Data());
            buffer.AppendFormatted("           new((*f%sFolders)[i]) %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            buffer.AppendFormatted("     return (%s%s*)f%sFolders->At(index); };\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            format.SetFormatted("   TClonesArray*%%%ds  Get%%ss()%%%ds { return f%%sFolders;%%%ds };\n",typeLen-12,10+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray**%%%ds Get%%sAddress()%%%ds { return &f%%sFolders;%%%ds };\n",typeLen-12,4+nameLen-folderName[i].Length(),13+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
         else {
            format.SetFormatted("   %%s%%s*%%%ds  Get%%sAt(int index)%%%ds { return (%%s%%s*)f%%sFolders->At(index);%%%ds };\n",typeLen-folderName[i].Length()-scl,0+nameLen-folderName[i].Length(),lt);
            buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray*%%%ds  Get%%ss()%%%ds { return f%%sFolders;%%%ds };\n",typeLen-12,10+nameLen-folderName[i].Length(),14+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
            format.SetFormatted("   TClonesArray**%%%ds Get%%sAddress()%%%ds { return &f%%sFolders;%%%ds };\n",typeLen-12,4+nameLen-folderName[i].Length(),13+typeLen+nameLen-folderName[i].Length());
            buffer.AppendFormatted((char*)format.Data(),"",folderName[i].Data(),"",folderName[i].Data(),"");
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task Getters
   buffer.AppendFormatted("   // Tasks\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      format.SetFormatted("   ROMETask* Get%%s%%03dTask()%%%ds { return f%%s%%03dTask;%%%ds };\n",taskLen-taskHierarchyName[i].Length(),taskLen-taskHierarchyName[i].Length());
      buffer.AppendFormatted((char*)format.Data(),taskHierarchyName[i].Data(),i,"",taskHierarchyName[i].Data(),i,"");
   }

   // Banks
   buffer.AppendFormatted("   // Bank Methodes\n");
   buffer.AppendFormatted("   void InitMidasBanks();\n");
   if (numOfBank>0) {
      for (i=0;i<numOfBank;i++) {
         // Bank Array
         if (bankArrayDigit[i]>0) {
            if (bankType[i]=="structure"||bankType[i]=="struct") {
               buffer.AppendFormatted("   %sStruct* Get%sBankAt(int bankIndex,int index);\n",bankName[i].Data(),bankName[i].Data());
               buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(int bankIndex,int index);\n",bankName[i].Data(),bankName[i].Data());
            }
            else {
               buffer.AppendFormatted("   %s Get%sBankAt(int bankIndex,int index);\n",bankType[i].Data(),bankName[i].Data());
               buffer.AppendFormatted("   %s GetLast%sBankAt(int bankIndex,int index);\n",bankType[i].Data(),bankName[i].Data());
            }
            buffer.AppendFormatted("   int Get%sBankEntries(int bankIndex);\n",bankName[i].Data());
            buffer.AppendFormatted("   int GetLast%sBankEntries(int bankIndex);\n",bankName[i].Data());
            buffer.AppendFormatted("   void Init%sBank(int bankIndex);\n",bankName[i].Data());
         }
         // Single Bank
         else {
            if (bankType[i]=="structure"||bankType[i]=="struct") {
               buffer.AppendFormatted("   %sStruct* Get%sBankAt(int index);\n",bankName[i].Data(),bankName[i].Data());
               buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(int index);\n",bankName[i].Data(),bankName[i].Data());
            }
            else {
               buffer.AppendFormatted("   %s Get%sBankAt(int index);\n",bankType[i].Data(),bankName[i].Data());
               buffer.AppendFormatted("   %s GetLast%sBankAt(int index);\n",bankType[i].Data(),bankName[i].Data());
            }
            buffer.AppendFormatted("   int Get%sBankEntries();\n",bankName[i].Data());
            buffer.AppendFormatted("   int GetLast%sBankEntries();\n",bankName[i].Data());
            buffer.AppendFormatted("   void Init%sBank();\n",bankName[i].Data());
         }
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

   // Steering
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   void startSplashScreen();\n");
   buffer.AppendFormatted("   void consoleStartScreen();\n");
   buffer.AppendFormatted("   void redirectOutput();\n");
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

bool ROMEBuilder::WriteAnalyzerF() {
   int i,j;

   ROMEString fFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

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
      for (j=0;j<numOfValue[i];j++) {
         if (!isNumber(valueType[i][j]))
            continue;
         if (folderArray[i]=="1") {
            if (valueArray[i][j]=="1") {
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
            if (valueArray[i][j]=="1") {
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
   buffer.AppendFormatted("\n");

   // Write File
   fileHandle = open(fFile.Data(),O_RDONLY);
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
      fileHandle = open(fFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(fFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << fFile.Data() << endl;
      nb = write(fileHandle,buffer.Data(), buffer.Length());
      close(fileHandle);
   }

   return true;
}

bool ROMEBuilder::WriteConfigCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int lenTot,j,ll;
   char *pos;
   int fileHandle;
   ROMEString format;

   ROMEString classDescription;
   classDescription.SetFormatted("This class handles the framework configuration file (romeConfig.xml).");

   // File name
   cppFile.SetFormatted("%s/src/framework/%sConfig.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Config");
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
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include <ROMEString.h>\n");
   buffer.AppendFormatted("#include <ROMEXMLDataBase.h>\n");
   buffer.AppendFormatted("#if defined( HAVE_SQL )\n");
   buffer.AppendFormatted("#include <ROMESQLDataBase.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sConfig.h>\n",shortCut.Data());
   for (i=0;i<numOfTask;i++)
      buffer.AppendFormatted("#include <include/tasks/%sT%s.h>\n",shortCut.Data(),taskName[i].Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sConfig::%sConfig() {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   fConfigData = new ConfigData*[1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = 0;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Configuration File
   buffer.AppendFormatted("\n// Read Configuration File\n");
   buffer.AppendFormatted("bool %sConfig::ReadConfigurationFile(char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   xml->OpenFileForPath(fXMLFile);\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = xml->NumberOfOccurrenceOfPath(\"//Configuration/RunConfiguration\");\n");
   buffer.AppendFormatted("   delete [] fConfigData;\n");
   buffer.AppendFormatted("   fConfigData = new ConfigData*[fNumberOfRunConfigs+1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   ROMEString path = \"//Configuration/MainConfiguration\";\n");
   buffer.AppendFormatted("   ReadConfiguration(xml,path,0);\n");
   buffer.AppendFormatted("   if (!SetConfiguration(0,0))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   for (int i=0;i<fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      fConfigData[i+1] = new ConfigData();\n");
   buffer.AppendFormatted("      path.SetFormatted(\"//Configuration/child::RunConfiguration[position()=%%d]\",i+1);\n");
   buffer.AppendFormatted("      ReadConfiguration(xml,path,i+1);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Configuration
   buffer.AppendFormatted("\n// Read Configuration\n");
   buffer.AppendFormatted("bool %sConfig::ReadConfiguration(ROMEXML *xml,ROMEString& path,int index) {\n",shortCut.Data());
   // RunNumbers
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/RunNumbers\",fConfigData[index]->fRunNumbers,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fRunNumbers==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fRunNumbersModified = false;\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      fConfigData[index]->fRunNumbersModified = true;\n");
   buffer.AppendFormatted("      fConfigData[index]->fLastRunNumberIndex = 0;\n");
   buffer.AppendFormatted("      fConfigData[index]->fRunNumberArray = gAnalyzer->decodeRunNumbers(fConfigData[index]->fRunNumbers);\n");
   buffer.AppendFormatted("   }\n");
   // EventNumbers
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/EventNumbers\",fConfigData[index]->fEventNumbers,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fEventNumbers==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fEventNumbersModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fEventNumbersModified = true;\n");
   // modes
   buffer.AppendFormatted("   // modes\n");
   buffer.AppendFormatted("   fConfigData[index]->fModes = new ConfigData::Modes();\n");
   // AnalyzingMode
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/AnalyzingMode\",fConfigData[index]->fModes->fAnalyzingMode,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fModes->fAnalyzingMode.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fAnalyzingMode==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fAnalyzingModeModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fAnalyzingModeModified = true;\n");
   // InputDataFormat
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/Modes/InputDataFormat\",fConfigData[index]->fModes->fInputDataFormat,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fModes->fInputDataFormat.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fInputDataFormat==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fInputDataFormatModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModes->fInputDataFormatModified = true;\n");
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
   // all
   buffer.AppendFormatted("   if (fConfigData[index]->fModes->fAnalyzingModeModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fModes->fInputDataFormatModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fModes->fBatchModeModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fModes->fShowSplashScreenModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fModesModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fModesModified = false;\n");
   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   fConfigData[index]->fDataBase = new ConfigData::DataBase();\n");
   // Type
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/DataBase/Type\",fConfigData[index]->fDataBase->fType,\"\");\n");
   buffer.AppendFormatted("   fConfigData[index]->fDataBase->fType.ToLower();\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fDataBase->fType==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBase->fTypeModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBase->fTypeModified = true;\n");
   // Connection
   buffer.AppendFormatted("   xml->GetPathValue(path+\"/DataBase/Connection\",fConfigData[index]->fDataBase->fConnection,\"\");\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fDataBase->fConnection==\"\")\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBase->fConnectionModified = false;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBase->fConnectionModified = true;\n");
   // all
   buffer.AppendFormatted("   if (fConfigData[index]->fDataBase->fTypeModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fDataBase->fConnectionModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBaseModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fDataBaseModified = false;\n");
   // online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   fConfigData[index]->fOnline = new ConfigData::Online();\n");
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
   // all
   buffer.AppendFormatted("   if (fConfigData[index]->fOnline->fHostModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnlineModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fOnlineModified = false;\n");
   // socket interface
   buffer.AppendFormatted("   // socket interface\n");
   buffer.AppendFormatted("   fConfigData[index]->fSocketInterface = new ConfigData::SocketInterface();\n");
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
   // all
   buffer.AppendFormatted("   if (fConfigData[index]->fSocketInterface->fPortNumberModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fSocketInterface->fAvailableOfflineModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterfaceModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fSocketInterfaceModified = false;\n");
   // paths
   buffer.AppendFormatted("   // paths\n");
   buffer.AppendFormatted("   fConfigData[index]->fPaths = new ConfigData::Paths();\n");
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
   // all
   buffer.AppendFormatted("   if (fConfigData[index]->fPaths->fInputFilePathModified ||\n");
   buffer.AppendFormatted("       fConfigData[index]->fPaths->fOutputFilePathModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fPathsModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fPathsModified = false;\n");
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
         path.InsertFormatted(0,"/child::Task[child::TaskName='%s']",taskHierarchyName[index].Data());
         classname.InsertFormatted(0,"::%sTask",taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      buffer.AppendFormatted("   fConfigData[index]%s = new ConfigData%s();\n",pointer.Data(),classname.Data());
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
         buffer.AppendFormatted("   fConfigData[index]%s->f%sHisto = new ConfigData%s::%sHisto();\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),classname.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // Accumulate
         buffer.AppendFormatted("   xml->GetPathValue(path+\"/Tasks%s/child::Histogram[child::HistName='%s']/Accumulate\",fConfigData[index]%s->f%sHisto->fAccumulate,\"\");\n",path.Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fAccumulate==\"\")\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fAccumulateModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHisto->fAccumulateModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         // all
         buffer.AppendFormatted("   if (fConfigData[index]%s->f%sHisto->fAccumulateModified) {\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      fConfigData[index]%s->fHistogramsModified = true;\n",pointer.Data());
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHistoModified = true;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      fConfigData[index]%s->f%sHistoModified = false;\n",pointer.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
      }
      // Steering parameter
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("   // steering parameters\n");
         buffer.AppendFormatted("   fConfigData[index]%s->fSteering = new ConfigData%s::Steering();\n",pointer.Data(),classname.Data());
         ROMEString pathT;
         ROMEString pointerT;
         ROMEString classT;
         pathT.SetFormatted("path+\"/Tasks%s",path.Data());
         pointerT.SetFormatted("fConfigData[index]%s->fSteering",pointer.Data());
         classT.SetFormatted("ConfigData%s::Steering",classname.Data());
         WriteSteeringConfigRead(buffer,0,taskHierarchyClassIndex[i],xml,pathT,pointerT,classT);
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
   // all
   buffer.AppendFormatted("   if (fConfigData[index]->fTreeAccumulateModified)\n");
   buffer.AppendFormatted("      fConfigData[index]->fTreesModified = true;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fConfigData[index]->fTreesModified = false;\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   // %s Tree\n",treeName[i].Data());
      buffer.AppendFormatted("   fConfigData[index]->f%sTree = new ConfigData::%sTree();\n",treeName[i].Data(),treeName[i].Data());
      // Read
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/child::Tree[child::TreeName='%s']/Read\",fConfigData[index]->f%sTree->fRead,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fRead==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fReadModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fReadModified = true;\n",treeName[i].Data());
      // Write
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/child::Tree[child::TreeName='%s']/Write\",fConfigData[index]->f%sTree->fWrite,\"\");\n",treeName[i].Data(),treeName[i].Data());
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
      buffer.AppendFormatted("      xml->GetPathValue(path+\"/Trees/child::Tree[child::TreeName='%s']/Fill\",fConfigData[index]->f%sTree->fFill,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("      if (fConfigData[index]->f%sTree->fFill==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("         fConfigData[index]->f%sTree->fFillModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         fConfigData[index]->f%sTree->fFillModified = true;\n",treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      // CompressionLevel
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/child::Tree[child::TreeName='%s']/CompressionLevel\",fConfigData[index]->f%sTree->fCompressionLevel,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fCompressionLevel==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fCompressionLevelModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fCompressionLevelModified = true;\n",treeName[i].Data());
      // MaxNumberOfEntries
      buffer.AppendFormatted("   xml->GetPathValue(path+\"/Trees/child::Tree[child::TreeName='%s']/MaxNumberOfEntries\",fConfigData[index]->f%sTree->fMaxNumberOfEntries,\"\");\n",treeName[i].Data(),treeName[i].Data());
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fMaxNumberOfEntries==\"\")\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified = false;\n",treeName[i].Data());
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified = true;\n",treeName[i].Data());
      // all
      buffer.AppendFormatted("   if (fConfigData[index]->f%sTree->fReadModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fWriteModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fFillModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fCompressionLevelModified ||\n",treeName[i].Data());
      buffer.AppendFormatted("       fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      fConfigData[index]->fTreesModified = true;\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTreeModified = true;\n",treeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      fConfigData[index]->f%sTreeModified = false;\n",treeName[i].Data());
   }
   // Global Steering Parameters
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted("   // global steering parameters\n");
      buffer.AppendFormatted("   fConfigData[index]->fGlobalSteering = new ConfigData::GlobalSteering();\n");
      ROMEString pathT;
      ROMEString pointerT;
      ROMEString classT;
      pathT.SetFormatted("path+\"/GlobalSteeringParameters");
      pointerT.SetFormatted("fConfigData[index]->fGlobalSteering");
      classT.SetFormatted("ConfigData::GlobalSteering");
      WriteSteeringConfigRead(buffer,0,numOfTaskHierarchy,xml,pathT,pointerT,classT);
   }
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
   buffer.AppendFormatted("               else if (fConfigData[i]->fRunNumberArray.At(i)<runNumber) {\n");
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


   // Set Configuration
   buffer.AppendFormatted("\n// Set Configuration\n");
   buffer.AppendFormatted("bool %sConfig::SetConfiguration(int modIndex,int index) {\n",shortCut.Data());
   buffer.AppendFormatted("   char* cstop;\n");
   buffer.AppendFormatted("   fActiveConfiguration = index;\n");
   // Run Numbers
   buffer.AppendFormatted("   // run numbers\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fRunNumbersModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetRunNumbers(fConfigData[index]->fRunNumbers);\n");
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
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fModes->fInputDataFormatModified) {\n");
   buffer.AppendFormatted("      gAnalyzer->SetDAQ(fConfigData[index]->fModes->fInputDataFormat);\n");
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

   buffer.AppendFormatted("   if (fConfigData[modIndex]->fDataBase->fConnectionModified) {\n");
   buffer.AppendFormatted("      gAnalyzer->SetDataBaseConnection(fConfigData[index]->fDataBase->fConnection);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fDataBase->fTypeModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fDataBase->fType==\"sql\") {\n");
   buffer.AppendFormatted("#if defined( HAVE_SQL )\n");
   buffer.AppendFormatted("         delete gAnalyzer->GetDataBase();\n");
   buffer.AppendFormatted("         gAnalyzer->SetDataBase(new ROMESQLDataBase());\n");
   buffer.AppendFormatted("         if (!gAnalyzer->GetDataBase()->Init(\"\",gAnalyzer->GetDataBaseConnection()))\n");
   buffer.AppendFormatted("            return false;\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fDataBase->fType==\"xml\") {\n");
   buffer.AppendFormatted("         delete gAnalyzer->GetDataBase();\n");
   buffer.AppendFormatted("         gAnalyzer->SetDataBase(new ROMEXMLDataBase());\n");
   buffer.AppendFormatted("         ROMEString str = gAnalyzer->GetDataBaseConnection();\n");
   buffer.AppendFormatted("         int index;\n");
   buffer.AppendFormatted("         if ((index=str.Index(\";\",1,0,TString::kExact))==-1) {\n");
   buffer.AppendFormatted("            gAnalyzer->Println(\"Invalid database connection\");\n");
   buffer.AppendFormatted("            return false;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         ROMEString path = str(0,index);\n");
   buffer.AppendFormatted("         if (path[path.Length()-1]!='/' && path[path.Length()-1]!='\\\\')\n");
   buffer.AppendFormatted("            path += \"/\";\n");
   buffer.AppendFormatted("         gAnalyzer->SetDataBaseDir((char*)path.Data());\n");
   buffer.AppendFormatted("         if (!gAnalyzer->GetDataBase()->Init(gAnalyzer->GetDataBaseDir(),((TString)str(index+1,str.Length()-index-1)).Data()))\n");
   buffer.AppendFormatted("            return false;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   // Online
   buffer.AppendFormatted("   // online\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fOnline->fHostModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetOnlineHost((char*)fConfigData[index]->fOnline->fHost.Data());\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetOnlineExperiment((char*)fConfigData[index]->fOnline->fExperiment.Data());\n");
   // Socket Interface
   buffer.AppendFormatted("   // socket interface\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fSocketInterface->fPortNumberModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetPortNumber((char*)fConfigData[index]->fSocketInterface->fPortNumber.Data());\n");
   buffer.AppendFormatted("   if (fConfigData[modIndex]->fSocketInterface->fAvailableOfflineModified) {\n");
   buffer.AppendFormatted("      if (fConfigData[index]->fSocketInterface->fAvailableOffline==\"true\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetSocketOffline(true);\n");
   buffer.AppendFormatted("      else\n");
   buffer.AppendFormatted("         gAnalyzer->SetSocketOffline(false);\n");
   buffer.AppendFormatted("   }\n");
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
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gAnalyzer->Get%s%03dTask()->SetActive(false);\n",taskHierarchyName[i].Data(),i);
      buffer.AppendFormatted("   }\n");
      // Histogram
      if (numOfHistos[i]>0)
         buffer.AppendFormatted("   // histograms\n");
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
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
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->Get%s%03dTask())->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i);
         WriteSteeringConfigSet(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT);
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
      buffer.AppendFormatted("      if (fConfigData[index]->f%sTree->fCompressionLevel==\"1\")\n",treeName[i].Data());
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetCompressionLevel(1);\n",i);
      buffer.AppendFormatted("      else\n");
      buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->SetCompressionLevel(0);\n",i);
      buffer.AppendFormatted("   }\n");
      // MaxNumberOfEntries
      buffer.AppendFormatted("   if (fConfigData[modIndex]->f%sTree->fMaxNumberOfEntriesModified) {\n",treeName[i].Data());
      buffer.AppendFormatted("      int maxNumOfEntries = strtol(fConfigData[index]->f%sTree->fMaxNumberOfEntries.Data(),&cstop,10);\n",treeName[i].Data());
      buffer.AppendFormatted("      gAnalyzer->GetTreeObjectAt(%d)->SetMaxEntries(maxNumOfEntries);\n",i);
      buffer.AppendFormatted("   }\n");
   }
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTaskHierarchy]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
      WriteSteeringConfigSet(buffer,0,numOfTaskHierarchy,pointerT,steerPointerT);
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
   buffer.AppendFormatted("   xml->WriteAttribute(\"xsi:noNamespaceSchemaLocation\",\"c:/rome/romeConfig.xsd\");\n");
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
   // run numbers
   buffer.AppendFormatted("   if (index==0)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"RunNumbers\",gAnalyzer->GetRunNumberStringOriginal());\n");
   buffer.AppendFormatted("   else if (fConfigData[index]->fRunNumbersModified)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"RunNumbers\",(char*)fConfigData[index]->fRunNumbers.Data());\n");
   // event numbers
   buffer.AppendFormatted("   if (index==0)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"EventNumbers\",gAnalyzer->GetEventNumberStringOriginal());\n");
   buffer.AppendFormatted("   else if (fConfigData[index]->fRunNumbersModified)\n");
   buffer.AppendFormatted("      xml->WriteElement(\"EventNumbers\",(char*)fConfigData[index]->fEventNumbers.Data());\n");
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
   buffer.AppendFormatted("         xml->WriteElement(\"AnalyzingMode\",(char*)fConfigData[index]->fModes->fAnalyzingMode.Data());\n");
   // InputDataFormat
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputDataFormat\",gAnalyzer->GetDAQ().Data());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fInputDataFormatModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputDataFormat\",(char*)fConfigData[index]->fModes->fInputDataFormat.Data());\n");
   // BatchMode
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isBatchMode())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"BatchMode\",\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"BatchMode\",\"false\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fBatchModeModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"BatchMode\",(char*)fConfigData[index]->fModes->fBatchMode.Data());\n");
   // ShowSplashScreen
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isSplashScreen())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"ShowSplashScreen\",\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"ShowSplashScreen\",\"false\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fModes->fShowSplashScreenModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"ShowSplashScreen\",(char*)fConfigData[index]->fModes->fShowSplashScreen.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // DataBase
   buffer.AppendFormatted("   // database\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fDataBaseModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"DataBase\");\n");
   // Type
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Type\",gAnalyzer->GetDataBase()->GetType());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fDataBase->fTypeModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Type\",(char*)fConfigData[index]->fDataBase->fType.Data());\n");
   // Connection
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Connection\",gAnalyzer->GetDataBaseConnection());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fDataBase->fConnectionModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Connection\",(char*)fConfigData[index]->fDataBase->fConnection.Data());\n");
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
   buffer.AppendFormatted("         xml->WriteElement(\"Host\",(char*)fConfigData[index]->fOnline->fHost.Data());\n");
   // Experiment
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",gAnalyzer->GetOnlineExperiment());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fOnline->fExperimentModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"Experiment\",(char*)fConfigData[index]->fOnline->fExperiment.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // SocketInterface
   buffer.AppendFormatted("   // socket interface\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fSocketInterfaceModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"SocketInterface\");\n");
   // PortNumber
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         str.SetFormatted(\"%%d\",gAnalyzer->GetPortNumber());\n");
   buffer.AppendFormatted("         xml->WriteElement(\"PortNumber\",(char*)str.Data());\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fSocketInterface->fPortNumberModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"PortNumber\",(char*)fConfigData[index]->fSocketInterface->fPortNumber.Data());\n");
   // AvailableOffline
   buffer.AppendFormatted("      if (index==0) {\n");
   buffer.AppendFormatted("         if (gAnalyzer->isSocketOffline())\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AvailableOffline\",\"true\");\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            xml->WriteElement(\"AvailableOffline\",\"false\");\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fSocketInterface->fAvailableOfflineModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"AvailableOffline\",(char*)fConfigData[index]->fSocketInterface->fAvailableOffline.Data());\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   // Paths
   buffer.AppendFormatted("   // paths\n");
   buffer.AppendFormatted("   if (fConfigData[index]->fPathsModified || index==0) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"Paths\");\n");
   // InputFilePath
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputFilePath\",(char*)gAnalyzer->GetInputDir());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fPaths->fInputFilePathModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"InputFilePath\",(char*)fConfigData[index]->fPaths->fInputFilePath.Data());\n");
   // OutputFilePath
   buffer.AppendFormatted("      if (index==0)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"OutputFilePath\",(char*)gAnalyzer->GetOutputDir());\n");
   buffer.AppendFormatted("      else if (fConfigData[index]->fPaths->fOutputFilePathModified)\n");
   buffer.AppendFormatted("         xml->WriteElement(\"OutputFilePath\",(char*)fConfigData[index]->fPaths->fOutputFilePath.Data());\n");
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
      buffer.AppendFormatted("         xml->WriteElement(\"Accumulate\",(char*)fConfigData[index]->fTreeAccumulate.Data());\n");
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
         buffer.AppendFormatted("            xml->WriteElement(\"Read\",(char*)fConfigData[index]->f%sTree->fRead.Data());\n",treeName[i].Data());
         // write
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            if (gAnalyzer->GetTreeObjectAt(%d)->isWrite())\n",i);
         buffer.AppendFormatted("               xml->WriteElement(\"Write\",\"true\");\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               xml->WriteElement(\"Write\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fWriteModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"Write\",(char*)fConfigData[index]->f%sTree->fWrite.Data());\n",treeName[i].Data());
         // fill
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            if (gAnalyzer->GetTreeObjectAt(%d)->isFill())\n",i);
         buffer.AppendFormatted("               xml->WriteElement(\"Fill\",\"true\");\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               xml->WriteElement(\"Fill\",\"false\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fFillModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"Fill\",(char*)fConfigData[index]->f%sTree->fFill.Data());\n",treeName[i].Data());
         // compression level
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            if (gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel()==1)\n",i);
         buffer.AppendFormatted("               xml->WriteElement(\"CompressionLevel\",\"1\");\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               xml->WriteElement(\"CompressionLevel\",\"0\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fCompressionLevelModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"CompressionLevel\",(char*)fConfigData[index]->f%sTree->fCompressionLevel.Data());\n",treeName[i].Data());
         // MaxNumberOfEntries
         buffer.AppendFormatted("         if (index==0) {\n");
         buffer.AppendFormatted("            str.SetFormatted(\"%%d\",gAnalyzer->GetTreeObjectAt(%d)->GetMaxEntries());\n",i);
         buffer.AppendFormatted("            xml->WriteElement(\"MaxNumberOfEntries\",str.Data());\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else if (fConfigData[index]->f%sTree->fMaxNumberOfEntriesModified)\n",treeName[i].Data());
         buffer.AppendFormatted("            xml->WriteElement(\"MaxNumberOfEntries\",(char*)fConfigData[index]->f%sTree->fMaxNumberOfEntries.Data());\n",treeName[i].Data());
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
      pointerT.SetFormatted("fConfigData[index]->fGlobalSteering");
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
      WriteSteeringConfigWrite(buffer,0,numOfTaskHierarchy,pointerT,steerPointerT,1);
   }
   if (numOfSteering[numOfTaskHierarchy]>0) {
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
bool ROMEBuilder::WriteConfigH() {
   int i;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   // File name
   hFile.SetFormatted("%s/include/framework/%sConfig.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sConfig_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sConfig_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#ifndef __CINT__\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#endif\n");
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
   buffer.AppendFormatted("      ROMEString  fRunNumbers;\n");
   buffer.AppendFormatted("      bool        fRunNumbersModified;\n");
   buffer.AppendFormatted("      int         fLastRunNumberIndex;\n");
   buffer.AppendFormatted("      TArrayI     fRunNumberArray;\n");
   buffer.AppendFormatted("      ROMEString  fEventNumbers;\n");
   buffer.AppendFormatted("      bool        fEventNumbersModified;\n");
   // modes
   buffer.AppendFormatted("      // modes;\n");
   buffer.AppendFormatted("      class Modes {\n");
   buffer.AppendFormatted("      public:\n");
   buffer.AppendFormatted("         ROMEString  fAnalyzingMode;\n");
   buffer.AppendFormatted("         bool        fAnalyzingModeModified;\n");
   buffer.AppendFormatted("         ROMEString  fInputDataFormat;\n");
   buffer.AppendFormatted("         bool        fInputDataFormatModified;\n");
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
   buffer.AppendFormatted("   public:\n");
   // Constructor
   buffer.AppendFormatted("      ConfigData() {\n");
   buffer.AppendFormatted("         fRunNumbersModified = false;\n");
   buffer.AppendFormatted("         fEventNumbersModified = false;\n");
   buffer.AppendFormatted("         fModesModified = false;\n");
   buffer.AppendFormatted("         fDataBaseModified = false;\n");
   buffer.AppendFormatted("         fOnlineModified = false;\n");
   buffer.AppendFormatted("         fSocketInterfaceModified = false;\n");
   buffer.AppendFormatted("         fPathsModified = false;\n");
   buffer.AppendFormatted("         fTasksModified = false;\n");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("         f%sTaskModified = false;\n",taskName[i].Data());
      buffer.AppendFormatted("         f%sTask = new %sTask();\n",taskName[i].Data(),taskName[i].Data());
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
   buffer.AppendFormatted("   bool WriteConfigurationFile(char *file);\n");
   buffer.AppendFormatted("   bool ReadConfigurationFile(char *file);\n");
   buffer.AppendFormatted("   bool CheckConfiguration(int runNumber);\n",shortCut.Data());
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
bool ROMEBuilder::WriteMidasCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int lenTot,ll;
   char *pos;
   int fileHandle;
   ROMEString format;

   ROMEString classDescription;
   classDescription.SetFormatted("This class implements the midas odb access.");

   // File name
   cppFile.SetFormatted("%s/src/framework/%sMidas.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Midas");
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
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include <ROMEEventLoop.h>\n");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sMidas.h>\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sMidas::%sMidas() { }\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");

   // Task switches call back
   buffer.AppendFormatted("// Task switches call back\n");
   buffer.AppendFormatted("bool ROMEEventLoop::fTaskSwitchesChanged = false;\n");
   buffer.AppendFormatted("void TaskSwitchesChanged(int hDB,int hKey,void *info) {\n");
   buffer.AppendFormatted("   ROMEEventLoop::fTaskSwitchesChanged = true;\n");
   buffer.AppendFormatted("}\n\n");

   // Initialize ODB
   buffer.AppendFormatted("// InitODB\n");
   buffer.AppendFormatted("bool %sMidas::InitODB() {\n",shortCut.Data());
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
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

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
bool ROMEBuilder::WriteMidasH() {
   int i;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   // File name
   hFile.SetFormatted("%s/include/framework/%sMidas.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sMidas_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sMidas_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#include <ROMEMidas.h>\n");

   // Class
   buffer.AppendFormatted("\nclass %sMidas : public ROMEMidas\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sMidas();\n",shortCut.Data());

   // methods
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   bool InitODB();\n");

   buffer.AppendFormatted("\n");
   // Footer

   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sMidas_H\n",shortCut.Data());

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
bool ROMEBuilder::WriteRootCpp() {
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int lenTot,ll;
   char *pos;
   int fileHandle;
   ROMEString format;

   ROMEString classDescription;
   classDescription.SetFormatted("This class implements rome's root input file access.");

   // File name
   cppFile.SetFormatted("%s/src/framework/%sRoot.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   ll = 74-shortCut.Length();
   format.SetFormatted("// %%s%%-%d.%ds //\n",ll,ll);
   buffer.AppendFormatted((char*)format.Data(),shortCut.Data(),"Root");
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
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sRoot.h>\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sRoot::%sRoot() { }\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");

   // Connect Trees
   int iFold=0,j,k;
   buffer.AppendFormatted("// Connect Trees\n");
   buffer.AppendFormatted("void %sRoot::ConnectTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TBranchElement *bb;\n");
   for (i=0;i<numOfTree;i++) {
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k]) 
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
bool ROMEBuilder::WriteRootH() {
   int i;

   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];

   int nb;
   int fileHandle;

   // File name
   hFile.SetFormatted("%s/include/framework/%sRoot.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#ifndef %sRoot_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sRoot_H\n\n",shortCut.Data());

   buffer.AppendFormatted("#include <ROMERoot.h>\n");

   // Class
   buffer.AppendFormatted("\nclass %sRoot : public ROMERoot\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sRoot();\n",shortCut.Data());

   // methods
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   void ConnectTrees();\n");

   buffer.AppendFormatted("\n");
   // Footer

   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sRoot_H\n",shortCut.Data());

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

   buffer.AppendFormatted("%sprotected:\n",blank.Data());

   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      WriteSteeringClass(buffer,steerChildren[numTask][numSteer][i],numTask,tab+1);
   }

   // Fields
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      format.SetFormatted("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldComment[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      format.SetFormatted("%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
   }
   buffer.AppendFormatted("\n%spublic:\n",blank.Data());
   // Constructor
   buffer.AppendFormatted("%s   %s%s() { ",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      buffer.AppendFormatted("f%s = %s; ",steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      buffer.AppendFormatted("f%s = new %s(); ",steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
   }
   buffer.AppendFormatted("};\n");
   // Getters
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      format.SetFormatted("%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      format.SetFormatted("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
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
bool ROMEBuilder::WriteSteeringConfigClass(ROMEString &buffer,int numSteer,int numTask,int tab) {
   ROMEString format;
   ROMEString sc;
   ROMEString blank="";
   int i,k;
   for (i=0;i<tab;i++)
      blank.Append("   ");   

   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   ROMEString  f%s;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("%s   bool        f%sModified;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
   }
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   class %s {\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("%s   public:\n",blank.Data());
      WriteSteeringConfigClass(buffer,steerChildren[numTask][numSteer][k],numTask,tab+1);
      buffer.AppendFormatted("%s   };\n",blank.Data());
      buffer.AppendFormatted("%s   %s *f%s;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("%s   bool f%sModified;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
   }
   buffer.AppendFormatted("%spublic:\n",blank.Data());
   buffer.AppendFormatted("%s   %s() {\n",blank.Data(),steerName[numTask][numSteer].Data());
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s      f%sModified = false;\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
   }
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s      f%s = new %s;\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
   }
   buffer.AppendFormatted("%s   }\n",blank.Data());
   return true;
}
bool ROMEBuilder::WriteSteeringConfigRead(ROMEString &buffer,int numSteer,int numTask,ROMEXML *xml,ROMEString& path,ROMEString& pointer,ROMEString& classPath) {
   int k;
   ROMEString pathT;
   ROMEString pointerT;
   ROMEString classPathT;
   buffer.AppendFormatted("   %sModified = false;\n",pointer.Data());
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("   xml->GetPathValue(%s/child::SteeringParameterField[child::SPName='%s']/SPValue\",%s->f%s,\"\");\n",path.Data(),steerFieldName[numTask][numSteer][k].Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("   if (%s->f%s==\"\")\n",pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("      %s->f%sModified = false;\n",pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("   else {\n");
      buffer.AppendFormatted("      %s->f%sModified = true;\n",pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("      %sModified = true;\n",pointer.Data());
      buffer.AppendFormatted("   }\n");
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("   %s->f%s = new %s::%s();\n",pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data(),classPath.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pathT = path;
      pointerT = pointer;
      classPathT = classPath;
      path.AppendFormatted("/child::SteeringParameterGroup[child::SPGroupName='%s']",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      classPath.AppendFormatted("::%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringConfigRead(buffer,steerChildren[numTask][numSteer][k],numTask,xml,path,pointer,classPath);
      path = pathT;
      pointer = pointerT;
      classPath = classPathT;
      buffer.AppendFormatted("   if (%s->f%sModified)\n",pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("      %sModified = true;\n",pointer.Data());
   }
   return true;
}

bool ROMEBuilder::WriteSteeringConfigSet(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer) {
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("   if (fConfigData[modIndex]%s->f%sModified) {\n",pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      value.SetFormatted("fConfigData[index]%s->f%s",pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      setValue(&decodedValue,"",(char*)value.Data(),(char*)steerFieldType[numTask][numSteer][k].Data(),1);
      if (steerFieldType[numTask][numSteer][k]=="std::string")
         buffer.AppendFormatted("      %s->Set%s(%s.Data());\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),value.Data());
      else
         buffer.AppendFormatted("      %s->Set%s((%s)%s);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
      buffer.AppendFormatted("   }\n");
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringConfigSet(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}
bool ROMEBuilder::WriteSteeringConfigWrite(ROMEString &buffer,int numSteer,int numTask,ROMEString& pointer,ROMEString& steerPointer,int tab) {
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
      buffer.AppendFormatted("%s   if (%s->f%sModified || index==0) {\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterField\");\n",blank.Data());
      buffer.AppendFormatted("%s      xml->WriteElement(\"SPName\",\"%s\");\n",blank.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("%s      if (index==0) {\n",blank.Data());
      GetFormat(&value,(char*)steerFieldType[numTask][numSteer][k].Data());
      if (steerFieldType[numTask][numSteer][k]=="TString")
         buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s().Data());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      else if (steerFieldType[numTask][numSteer][k]=="std::string")
         buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s().c_str());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      else
         buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"SPValue\",(char*)value.Data());\n",blank.Data());
      buffer.AppendFormatted("%s      }\n",blank.Data());
      buffer.AppendFormatted("%s      else if (%s->f%sModified)\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"SPValue\",(char*)%s->f%s.Data());\n",blank.Data(),pointer.Data(),steerFieldName[numTask][numSteer][k].Data());
      buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
      buffer.AppendFormatted("%s   }\n",blank.Data());
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Group\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("%s   if (%s->f%sModified || index==0) {\n",blank.Data(),pointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterGroup\");\n",blank.Data());
      buffer.AppendFormatted("%s      xml->WriteElement(\"SPGroupName\",\"%s\");\n",blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringConfigWrite(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer,tab+1);
      pointer = pointerT;
      steerPointer = steerPointerT;
      buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
      buffer.AppendFormatted("%s   }\n",blank.Data());
   }
   return true;
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
      buffer.AppendFormatted("%s            xml->WriteElement(\"Active\",(char*)fConfigData[index]%s->fActive.Data());\n",blank.Data(),pointerI.Data());
      if (numOfHistos[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("%s         // histograms\n",blank.Data());
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("%s         if (fConfigData[index]%s->f%sHistoModified || index==0) {\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s            xml->StartElement(\"Histogram\");\n",blank.Data());
         buffer.AppendFormatted("%s            xml->WriteElement(\"HistName\",\"%s\");\n",blank.Data(),histoName[i][j].Data());
         buffer.AppendFormatted("%s            if (index==0) {\n",blank.Data());
         buffer.AppendFormatted("%s               if (((%sT%s*)gAnalyzer->Get%s%03dTask())->is%sAccumulation())\n",blank.Data(),shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s                  xml->WriteElement(\"Accumulate\",\"true\");\n",blank.Data());
         buffer.AppendFormatted("%s               else\n",blank.Data());
         buffer.AppendFormatted("%s                  xml->WriteElement(\"Accumulate\",\"false\");\n",blank.Data());
         buffer.AppendFormatted("%s            }\n",blank.Data());
         buffer.AppendFormatted("%s            else if (fConfigData[index]%s->f%sHisto->fAccumulateModified)\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("%s               xml->WriteElement(\"Accumulate\",(char*)fConfigData[index]%s->f%sHisto->fAccumulate.Data());\n",blank.Data(),pointerI.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
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
         pointerT.SetFormatted("fConfigData[index]%s->fSteering",pointerI.Data());
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->Get%s%03dTask())->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyName[i].Data(),i);
         WriteSteeringConfigWrite(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT,3+tab);
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
         buffer.AppendFormatted("%s            ROMEString  fAccumulate;\n",blank.Data());
         buffer.AppendFormatted("%s            bool        fAccumulateModified;\n",blank.Data());
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
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");

   // Header
   buffer.AppendFormatted("#if defined( R__VISUAL_CPLUSPLUS )\n");
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
   buffer.AppendFormatted("#if defined( HAVE_SQL )\n");
   buffer.AppendFormatted("#include <ROMESQL.h>\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("#include <ROMEXML.h>\n");
   buffer.AppendFormatted("#include <ROMEEventLoop.h>\n");
   buffer.AppendFormatted("#include <ROMETree.h>\n");
   buffer.AppendFormatted("#include <ROMETreeInfo.h>\n");
   buffer.AppendFormatted("#include <ROME.h>\n");
   buffer.AppendFormatted("#include <include/framework/%sAnalyzer.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sEventLoop.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sMidas.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <include/framework/%sRoot.h>\n",shortCut.Data());
   buffer.AppendFormatted("#include <Riostream.h>\n");
   buffer.AppendFormatted("\n");
//   buffer.AppendFormatted("ClassImp(%sEventLoop);\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // Constructor
   buffer.AppendFormatted("%sEventLoop::%sEventLoop(const char *name,const char *title):ROMEEventLoop(name,title) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   fMidas = new %sMidas();\n",shortCut.Data());
   buffer.AppendFormatted("   fRoot = new %sRoot();\n",shortCut.Data());
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
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"%s%s\",gAnalyzer->Get%sAddress(),32000,99);\n",branchName[i][j].Data(),shortCut.Data(),folderName[iFold].Data(),branchFolder[i][j].Data());
         }
         else {
            buffer.AppendFormatted("   tree->Branch(\"%s\",\"TClonesArray\",gAnalyzer->Get%sAddress(),32000,99);\n",branchName[i][j].Data(),branchFolder[i][j].Data());
         }
      }
      buffer.AppendFormatted("   gAnalyzer->AddTree(tree);\n");
      buffer.AppendFormatted("   treeFolder->Add(tree);\n\n");
   }
   buffer.AppendFormatted("}\n\n");

   // clean up folders
   buffer.AppendFormatted("// Delete Unused Folders\n");
   buffer.AppendFormatted("void %sEventLoop::CleanUpFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
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
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i]>0 && !folderDataBase[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   gAnalyzer->Get%s()->Reset();\n",folderName[i].Data());
         }
         else if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=0;i<gAnalyzer->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->Reset();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<gAnalyzer->Get%ss()->GetEntriesFast();i++) {\n",folderName[i].Data());
            buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->SetModified(false);\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      if (bankHasHeader) {
         if (folderName[i]==bankHeaderFolder) {
            if (bankHeaderEventID!="")
               buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)gAnalyzer->GetRawDataEvent())->event_id);\n",folderName[i].Data(),bankHeaderEventID.Data());
            if (bankHeaderTriggerMask!="")
               buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)gAnalyzer->GetRawDataEvent())->trigger_mask);\n",folderName[i].Data(),bankHeaderTriggerMask.Data());
            if (bankHeaderSerialNumber!="")
               buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)gAnalyzer->GetRawDataEvent())->serial_number);\n",folderName[i].Data(),bankHeaderSerialNumber.Data());
            if (bankHeaderTimeStamp!="")
               buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)gAnalyzer->GetRawDataEvent())->time_stamp);\n",folderName[i].Data(),bankHeaderTimeStamp.Data());
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Initialize Single Folders
   buffer.AppendFormatted("void %sEventLoop::InitSingleFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   new(gAnalyzer->Get%s()) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="1")
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
            buffer.AppendFormatted("     new((*gAnalyzer->Get%ss())[i]) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (valueArray[i][j]=="1")
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
      buffer.AppendFormatted("   if (romeTree->isFill()) {\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (folderName[k]==branchFolder[i][j]) {
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

   // Initialize Task Switches
   buffer.AppendFormatted("void %sEventLoop::InitTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTaskHierarchy;i++) {
      buffer.AppendFormatted("   gAnalyzer->GetTaskSwitches()->%s = gAnalyzer->Get%s%03dTask()->IsActive();\n",taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),i);
   }
   buffer.AppendFormatted("};\n\n");

   // Update Task Switches
   buffer.AppendFormatted("void %sEventLoop::UpdateTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTaskHierarchy;i++) {
      buffer.AppendFormatted("   if (gAnalyzer->GetTaskSwitches()->%s)\n",taskHierarchyName[i].Data());
      buffer.AppendFormatted("      gAnalyzer->Get%s%03dTask()->SetActive(true);\n",taskHierarchyName[i].Data(),i);
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      gAnalyzer->Get%s%03dTask()->SetActive(false);\n",taskHierarchyName[i].Data(),i);
   }
   buffer.AppendFormatted("};\n\n");

   // Close cpp-File
   fileHandle = open(cppFile.Data(),O_RDONLY);
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

   // File name
   hFile.SetFormatted("%s/include/framework/%sEventLoop.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ROMEBuilder.                           //\n");
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

   char* romesys = getenv("ROMESYS");
   if (romesys==NULL) {
      cout << "Please set the environment variable ROMESYS to the ROME root-directory." << endl;
      return 1;
   }
   char* rootsys = getenv("ROOTSYS");
   if (rootsys==NULL) {
      cout << "Please set the environment variable ROOTSYS to the ROOT root-directory." << endl;
      return 1;
   }

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
         romeb->noLink = false;
         romeb->outDir = "C:/Data/analysis/MEG/ROME .NET/MEGFrameWork/";
         xmlFile = "C:/Data/analysis/MEG/ROME .NET/MEGFrameWork/MEGFrameWork.xml";
      }
      else if (!strcmp(argv[i],"-lp")) {
         romeb->makeOutput = true;
         romeb->midas = true;
         romeb->noLink = true;
         romeb->outDir = "C:/lpframework/";
         xmlFile = "C:/lpframework/lpframework.xml";
      }
      else if (!strcmp(argv[i],"-sample")) {
         romeb->makeOutput = true;
         romeb->midas = false;
         romeb->noLink = false;
         romeb->outDir = "C:/rome/examples/sample/";
         xmlFile = "C:/rome/examples/sample/sample.xml";
      }
      else if (!strcmp(argv[i],"-dance")) {
         romeb->makeOutput = true;
         romeb->midas = false;
         romeb->noLink = true;
         romeb->outDir = "C:/Data/Testprojects/dance2/test/";
         xmlFile = "C:/Data/Testprojects/dance2/test/dance.xml";
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
#if defined( R__VISUAL_CPLUSPLUS )
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

#if defined( R__UNIX )
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
   
   numOfFolder = -1;
   numOfTask = -1;
   numOfTree = -1;
   numOfBank = -1;

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
                  // default task hierarchy
                  for (i=0;i<numOfTask;i++) {
                     taskHierarchyName[i] = taskName[i];
                     taskHierarchyParentIndex[i] = -1;
                     taskHierarchyClassIndex[i] = i;
                  }
                  numOfTaskHierarchy = numOfTask;
                  // write task classes
                  if (!WriteTaskCpp()) return;
                  if (!WriteTaskF()) return;
                  if (!WriteTaskH()) return;
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
               if (!strcmp((const char*)name,"MidasBanks")) {
                  numOfBank = -1;
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
                  if (!WriteSteering(numOfTaskHierarchy)) return;
               }
            }
         }
      }
   }
   if (makeOutput) cout << "\n\nFramework :" << endl;
   if (!WriteAnalyzerCpp()) return;
   if (!WriteAnalyzerH()) return;
   if (!WriteAnalyzerF()) return;
   if (!WriteConfigCpp()) return;
   if (!WriteConfigH()) return;
   if (!WriteMidasCpp()) return;
   if (!WriteMidasH()) return;
   if (!WriteRootCpp()) return;
   if (!WriteRootH()) return;
   if (!WriteEventLoopCpp()) return;
   if (!WriteEventLoopH()) return;
   if (!WriteMain()) return;
   delete xml;

   ROMEString buffer;
   chdir(outDir.Data());

// Linking
   if (makeOutput && !noLink) cout << "\nLinking " << shortCut.Data() << " Project." << endl;
   WriteMakefile();
   if (!noLink) {
#if defined( R__UNIX )
      system("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
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
char* ROMEBuilder::EqualSign() {
#if defined( R__VISUAL_CPLUSPLUS )
   return "=";
#endif
#if defined( R__UNIX )
   return ":=";
#endif
   return "";
}

void ROMEBuilder::WriteMakefile() {
   // write a Makefile
   ROMEString buffer;
   ROMEString tempBuffer;
   ROMEString compileFormatFrame,compileFormatFramF,compileFormatTasks,compileFormatTaskF,compileFormatBlank,compileFormatROME;
   int i;
   bool haveFortranTask = false;
   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         haveFortranTask = true;
         break;
      }
   }

   ROMEString shortcut(shortCut);
   shortcut.ToLower();
   ROMEString mainprogname(mainProgName);
   mainprogname.ToLower();

   buffer.Resize(0);
// Libraries, Flags and Includes
// -----------------------------
#if defined( R__VISUAL_CPLUSPLUS )
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
   // fortran flags
   buffer.AppendFormatted("FortranFlags = $(%suserflags)\n",shortcut.Data());
   // includes
   buffer.AppendFormatted("Includes = /I$(ROMESYS)/include/ /I$(ROOTSYS)/include/ /I. /Iinclude/ /Iinclude/tasks/ /Iinclude/framework/ ");
   if (this->midas) 
      buffer.AppendFormatted(" /I$(MIDASSYS)/include/");
   if (this->sql) 
      buffer.AppendFormatted(" /I$(ROMESYS)/include/mysql/");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("rootlibs := $(shell root-config --libs)\n");
   buffer.AppendFormatted("rootglibs := $(shell root-config --glibs)\n");
   buffer.AppendFormatted("rootcflags := $(shell root-config --cflags)\n");
   buffer.AppendFormatted("rootthreadlibs := -lThread\n");
   buffer.AppendFormatted("xmllibs :=  $(shell xml2-config --libs)\n");
   buffer.AppendFormatted("xmlcflags :=  $(shell xml2-config --cflags)\n");
   if (this->sql){
      buffer.AppendFormatted("sqllibs := $(shell mysql_config --libs)\n");
      buffer.AppendFormatted("sqlcflags := $(shell mysql_config --cflags) -DHAVE_SQL\n");
   }
   else{
      buffer.AppendFormatted("sqllibs := \n");
      buffer.AppendFormatted("sqlcflags := \n");
   }
#if defined( R__ALPHA )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lc -lbsd\n");
#elif defined ( R__SGI )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
#elif defined ( R__FBSD )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lbsd -lcompat\n");
#elif defined ( R__MACOSX )
   buffer.AppendFormatted("FINK_DIR := $(shell which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\")\n");
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function  $(shell [ -d $(FINK_DIR)/include ] && echo -I$(FINK_DIR)/include)\n");
   buffer.AppendFormatted("oslibs := -lpthread -multiply_defined suppress $(shell [ -d $(FINK_DIR)/lib ] && echo -L$(FINK_DIR)/lib)\n");
#elif defined ( R__LINUX )
   buffer.AppendFormatted("oscflags := -fPIC -Wno-unused-function\n");
   buffer.AppendFormatted("oslibs := -lutil -lpthread\n");
#elif defined ( R__SOLARIS )
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs := -lsocket -lnsl\n");
#else
   buffer.AppendFormatted("oscflags :=\n");
   buffer.AppendFormatted("oslibs :=\n");
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
   buffer.AppendFormatted("clibs :=-lHtml $(SYSLIBS)");
   if (haveFortranTask)
      buffer.AppendFormatted(" -lg2c");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   // libs
   buffer.AppendFormatted("Libraries := $(oslibs) $(rootlibs) $(rootglibs) $(rootthreadlibs) $(xmllibs) $(clibs) $(sqllibs) $(midaslibs)\n");
   // flags
   buffer.AppendFormatted("Flags := $(%suserflags) $(oscflags) $(rootcflags) $(xmlcflags) $(sqlcflags) $(midascflags)\n",shortcut.Data());
   // includes
   buffer.AppendFormatted("Includes := -I$(ROMESYS)/include/ -I. -Iinclude/ -Iinclude/tasks/ -Iinclude/framework/\n");
   buffer.AppendFormatted("\n");
#endif

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

// user classes
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("UserClassHeaders %s",EqualSign());

// task dependences
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTask;i++) {
      buffer.AppendFormatted("%sT%sDep %s\n",shortCut.Data(),taskName[i].Data(),EqualSign());
   }
   buffer.AppendFormatted("\n");

// Objects
// -------
   buffer.AppendFormatted("objects %s",EqualSign());
   for (i=0;i<numOfFolder;i++) {
      if (!folderUserCode[i]) continue;
      buffer.AppendFormatted(" obj/%s%s.obj",shortCut.Data(),folderName[i].Data());
   }
   buffer.AppendFormatted(" $(TaskObjects)");
   buffer.AppendFormatted(" obj/%sAnalyzer.obj obj/%sEventLoop.obj obj/%sConfig.obj obj/%sMidas.obj obj/%sRoot.obj obj/main.obj",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   if (haveFortranTask)
      buffer.AppendFormatted(" obj/%sFAnalyzer.obj",shortCut.Data());
   if (this->sql)
      buffer.AppendFormatted(" obj/ROMESQL.obj obj/ROMESQLDataBase.obj");
   buffer.AppendFormatted(" obj/ROMEAnalyzer.obj obj/ROMEEventLoop.obj obj/ROMETask.obj obj/ROMESplashScreen.obj obj/ROMEXML.obj obj/ROMEString.obj obj/ROMEStrArray.obj obj/ROMEStr2DArray.obj obj/ROMEPath.obj obj/ROMEXMLDataBase.obj obj/ROMEMidas.obj obj/ROMERoot.obj");
   buffer.AppendFormatted(" obj/%sDict.obj",shortCut.Data());
   buffer.AppendFormatted("\n\n");
// all
   buffer.AppendFormatted("all:obj %s%s.exe\n",shortcut.Data(),mainprogname.Data());
   buffer.AppendFormatted("\n");
// user makefile
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("!INCLUDE Makefile.usr\n");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("-include Makefile.usr\n");
#endif
   buffer.AppendFormatted("\n");
// make obj
   buffer.AppendFormatted("obj:\n");
   buffer.AppendFormatted("\t@if [ ! -d  obj ] ; then \\\n");
   buffer.AppendFormatted("\t\techo \"Making directory obj\" ; \\\n");
   buffer.AppendFormatted("\t\tmkdir obj; \\\n");
   buffer.AppendFormatted("\tfi;\n");

// Link Statement
// --------------
   buffer.AppendFormatted("%s%s.exe: $(objects)\n",shortcut.Data(),mainprogname.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   if (haveFortranTask)
      buffer.AppendFormatted("	cl /Fe%s%s.exe $(objects) $(Libraries) /link /nodefaultlib:\"libcmtd.lib\" /FORCE:MULTIPLE\n\n",shortCut.Data(),mainProgName.Data());
   else
      buffer.AppendFormatted("	cl /Fe%s%s.exe $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("	g++ $(Flags) -o $@ $(objects) $(Libraries)\n\n",shortCut.Data(),mainProgName.Data());
#endif

// Compile Statements
// ------------------
#if defined( R__UNIX )
   compileFormatFrame.SetFormatted("	g++ -c $(Flags) $(Includes) src/framework/%s%%s.cpp -o obj/%s%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatFramF.SetFormatted("	@echo fortran tasks not implemented on unix\n");
   compileFormatTasks.SetFormatted("	g++ -c $(Flags) $(Includes) src/tasks/%sT%%s.cpp -o obj/%sT%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTaskF.SetFormatted("	@echo fortran tasks not implemented on unix\n");
   compileFormatBlank.SetFormatted("	g++ -c $(Flags) $(Includes) %%s.cpp -o obj/%%s.obj\n");
   compileFormatROME.SetFormatted ("	g++ -c $(Flags) $(Includes) $(ROMESYS)/src/ROME%%s.cpp -o obj/ROME%%s.obj\n");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   compileFormatFrame.SetFormatted("	cl /c $(Flags) $(Includes) src/framework/%s%%s.cpp /Foobj/%s%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatFramF.SetFormatted("	df $(FortranFlags) /compile_only src\\framework\\%sF%%s.f /object:obj\\%sF%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTasks.SetFormatted("	cl /c $(Flags) $(Includes) src/tasks/%sT%%s.cpp /Foobj/%sT%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatTaskF.SetFormatted("	df $(FortranFlags) /compile_only src\\tasks\\%sTF%%s.f /object:obj\\%sTF%%s.obj\n",shortCut.Data(),shortCut.Data());
   compileFormatBlank.SetFormatted("	cl /c $(Flags) $(Includes) %%s.cpp /Foobj/%%s.obj\n");
   compileFormatROME.SetFormatted ("	cl /c $(Flags) $(Includes) $(ROMESYS)/src/ROME%%s.cpp /Foobj/ROME%%s.obj\n");
#endif
   for (i=0;i<numOfFolder;i++) {
      if (folderUserCode[i]) {
         buffer.AppendFormatted("obj/%s%s.obj: src/framework/%s%s.cpp include/framework/%s%s.h",shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
	      buffer.AppendFormatted(" include/framework/%s%s_Base.h",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted((char*)compileFormatFrame.Data(),folderName[i].Data(),folderName[i].Data());
      }
   }
   for (i=0;i<numOfTask;i++) {
      if (taskFortran[i]) {
         buffer.AppendFormatted("obj/%sTF%s.obj: src/tasks/%sTF%s.f include/tasks/%sT%s.h $(%sT%sDep)\n",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
         buffer.AppendFormatted((char*)compileFormatTaskF.Data(),taskName[i].Data(),taskName[i].Data());
      }
      buffer.AppendFormatted("obj/%sT%s.obj: src/tasks/%sT%s.cpp include/tasks/%sT%s.h $(%sT%sDep)",shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data(),shortCut.Data(),taskName[i].Data());
      if (taskUserCode[i])
         buffer.AppendFormatted(" include/tasks/%sT%s_Base.h",shortCut.Data(),taskName[i].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted((char*)compileFormatTasks.Data(),taskName[i].Data(),taskName[i].Data());
   }

   buffer.AppendFormatted("obj/%sFAnalyzer.obj: src/framework/%sFAnalyzer.f src/framework/%sAnalyzer.cpp\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted((char*)compileFormatFramF.Data(),"Analyzer","Analyzer");
   buffer.AppendFormatted("obj/%sAnalyzer.obj: src/framework/%sAnalyzer.cpp include/framework/%sAnalyzer.h include/framework/%sEventLoop.h",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(TaskIncludes)");
   buffer.AppendFormatted(" $(BaseTaskIncludes)");
   buffer.AppendFormatted(" $(FolderIncludes)");
   buffer.AppendFormatted(" $(BaseFolderIncludes)");
   if (numOfSteering[numOfTaskHierarchy]>0) {
      buffer.AppendFormatted(" include/framework/%sGlobalSteering.h",shortCut.Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted((char*)compileFormatFrame.Data(),"Analyzer","Analyzer");
   buffer.AppendFormatted("obj/%sEventLoop.obj: src/framework/%sEventLoop.cpp include/framework/%sEventLoop.h include/framework/%sAnalyzer.h include/framework/%sMidas.h include/framework/%sRoot.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted((char*)compileFormatFrame.Data(),"EventLoop","EventLoop");
   buffer.AppendFormatted("obj/%sConfig.obj: src/framework/%sConfig.cpp include/framework/%sConfig.h include/framework/%sAnalyzer.h",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(" $(TaskIncludes)");
   buffer.AppendFormatted(" $(BaseTaskIncludes)");
   buffer.AppendFormatted("\n");   
   buffer.AppendFormatted((char*)compileFormatFrame.Data(),"Config","Config");
   buffer.AppendFormatted("obj/%sMidas.obj: src/framework/%sMidas.cpp include/framework/%sMidas.h include/framework/%sAnalyzer.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted((char*)compileFormatFrame.Data(),"Midas","Midas");
   buffer.AppendFormatted("obj/%sRoot.obj: src/framework/%sRoot.cpp include/framework/%sRoot.h include/framework/%sAnalyzer.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted((char*)compileFormatFrame.Data(),"Root","Root");
   buffer.AppendFormatted("obj/main.obj: src/framework/main.cpp include/framework/%sAnalyzer.h\n",shortCut.Data());
   buffer.AppendFormatted((char*)compileFormatBlank.Data(),"src/framework/main","main");

   buffer.AppendFormatted("obj/ROMEAnalyzer.obj: $(ROMESYS)/src/ROMEAnalyzer.cpp  $(ROMESYS)/include/ROMEAnalyzer.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"Analyzer","Analyzer");
   buffer.AppendFormatted("obj/ROMEEventLoop.obj: $(ROMESYS)/src/ROMEEventLoop.cpp $(ROMESYS)/include/ROMEEventLoop.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"EventLoop","EventLoop");
   buffer.AppendFormatted("obj/ROMETask.obj: $(ROMESYS)/src/ROMETask.cpp $(ROMESYS)/include/ROMETask.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"Task","Task");
   buffer.AppendFormatted("obj/ROMESplashScreen.obj: $(ROMESYS)/src/ROMESplashScreen.cpp\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"SplashScreen","SplashScreen");
   buffer.AppendFormatted("obj/ROMEMidas.obj: $(ROMESYS)/src/ROMEMidas.cpp $(ROMESYS)/include/ROMEMidas.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"Midas","Midas");
   buffer.AppendFormatted("obj/ROMERoot.obj: $(ROMESYS)/src/ROMERoot.cpp $(ROMESYS)/include/ROMERoot.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"Root","Root");
   buffer.AppendFormatted("obj/ROMEString.obj: $(ROMESYS)/src/ROMEString.cpp $(ROMESYS)/include/ROMEString.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"String","String");
   buffer.AppendFormatted("obj/ROMEStrArray.obj: $(ROMESYS)/src/ROMEStrArray.cpp $(ROMESYS)/include/ROMEStrArray.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"StrArray","StrArray");
   buffer.AppendFormatted("obj/ROMEStr2DArray.obj: $(ROMESYS)/src/ROMEStr2DArray.cpp $(ROMESYS)/include/ROMEStr2DArray.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"Str2DArray","Str2DArray");
   buffer.AppendFormatted("obj/ROMEPath.obj: $(ROMESYS)/src/ROMEPath.cpp $(ROMESYS)/include/ROMEPath.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"Path","Path");
   buffer.AppendFormatted("obj/ROMEXML.obj: $(ROMESYS)/src/ROMEXML.cpp $(ROMESYS)/include/ROMEXML.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"XML","XML");
   buffer.AppendFormatted("obj/ROMEXMLDataBase.obj: $(ROMESYS)/src/ROMEXMLDataBase.cpp $(ROMESYS)/include/ROMEXMLDataBase.h\n");
   buffer.AppendFormatted((char*)compileFormatROME.Data(),"XMLDataBase","XMLDataBase");
   if (this->sql) {
      buffer.AppendFormatted("obj/ROMESQLDataBase.obj: $(ROMESYS)/src/ROMESQLDataBase.cpp $(ROMESYS)/include/ROMESQLDataBase.h\n");
      buffer.AppendFormatted((char*)compileFormatROME.Data(),"SQLDataBase","SQLDataBase");
      buffer.AppendFormatted("obj/ROMESQL.obj: $(ROMESYS)/src/ROMESQL.cpp $(ROMESYS)/include/ROMESQL.h\n");
      buffer.AppendFormatted((char*)compileFormatROME.Data(),"SQL","SQL");
   }
   buffer.AppendFormatted("obj/%sDict.obj: %sDict.cpp %sDict.h\n",shortCut.Data(),shortCut.Data(),shortCut.Data());
   tempBuffer.SetFormatted("%sDict",shortCut.Data());
   buffer.AppendFormatted((char*)compileFormatBlank.Data(),tempBuffer.Data(),tempBuffer.Data());
   buffer.AppendFormatted("\n");
   ROMEString dictionarybat;
   WriteDictionaryBat(dictionarybat);
   dictionarybat.ReplaceAll("$ROOTSYS","$(ROOTSYS)");
   dictionarybat.ReplaceAll("$ROMESYS","$(ROMESYS)");
   buffer.AppendFormatted("%sDict.cpp: ",shortCut.Data());
   buffer.AppendFormatted(" $(TaskIncludes)");
   buffer.AppendFormatted(" $(BaseTaskIncludes)");
   buffer.AppendFormatted(" $(FolderIncludes)");
   buffer.AppendFormatted(" $(BaseFolderIncludes)");
   buffer.AppendFormatted(" $(ROMESYS)/include/ROMETask.h $(ROMESYS)/include/ROMETreeInfo.h $(ROMESYS)/include/ROMEAnalyzer.h include/framework/%sAnalyzer.h $(UserClassHeaders)\n",shortCut.Data());
   dictionarybat.Remove(dictionarybat.Length()-1);
   buffer.AppendFormatted("	%s $(UserClassHeaders)\n",dictionarybat.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("clean:\n");
   buffer.AppendFormatted("	rm -f obj/*.obj %sDict.cpp %sDict.h\n",shortCut.Data(),shortCut.Data());

   ROMEString makeFile;
#if defined( R__UNIX )
   makeFile = "Makefile";
#endif
#if defined ( R__VISUAL_CPLUSPLUS )
   makeFile = "Makefile.win";
#endif
   int fileHandle = open(makeFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(makeFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer.Data(), buffer.Length());
   close(fileHandle);
   // Write Makefile.usr
   struct stat buf;
   makeFile = "Makefile.usr";
   ROMEString usrBuffer;
   if( stat( makeFile.Data(), &buf )) {
      fileHandle = open(makeFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      usrBuffer.SetFormatted("# User editable Makefile for the %s%s\n",shortcut.Data(),mainprogname.Data());
      usrBuffer.AppendFormatted("#\n");
      usrBuffer.AppendFormatted("# Description:\n");
      usrBuffer.AppendFormatted("# 1) Add compile(link) options to Flags(Libraries), e.g. Flags += -g -O2 -Wall\n");
      usrBuffer.AppendFormatted("# 2) Add mySource.obj to the list of objects, e.g. objects += mySource.obj\n");
      usrBuffer.AppendFormatted("# 3) Add compile statment, e.g.\n");
      usrBuffer.AppendFormatted("#       obj/mySource.obj: mySource.cpp\n");
      usrBuffer.AppendFormatted("#          g++ -c $(Flags) $(Includes) mySource.cpp -o obj/mySource.obj\n");
      usrBuffer.AppendFormatted("#\n");
      write(fileHandle,usrBuffer.Data(), usrBuffer.Length());
      close(fileHandle);
   }
}

void ROMEBuilder::WriteDictionaryBat(ROMEString& buffer) 
{
   // writes a script file that executes rootcint
   int i;

   buffer.Resize(0);
   buffer.AppendFormatted("rootcint -f %sDict.cpp -c ",shortCut.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("-I%%ROMESYS%%/include ");
   buffer.AppendFormatted("-I%%ROOTSYS%%/include ");
#endif
#if defined( R__UNIX )
   buffer.AppendFormatted("-I$ROMESYS/include ");
   buffer.AppendFormatted("-I$ROOTSYS/include ");
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
      if (taskUserCode[i])
         buffer.AppendFormatted("include/tasks/%sT%s_Base.h ",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("ROMETask.h ROMETreeInfo.h ROMEAnalyzer.h include/framework/%sAnalyzer.h\n",shortCut.Data());
   buffer.Append("\0");

#if defined( R__VISUAL_CPLUSPLUS )
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
   // Objects
   buffer.AppendFormatted("<H2><a name=objects>Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("All <a href=\"#taskobjects\">Tasks</a>, <a href=\"#folderobjects\">Folders</a>, <a href=\"#treeobjects\">Trees</a> and <a href=\"#midasbankobjects\">Midas Banks</a> are described here.\n");
   // Tasks
   buffer.AppendFormatted("<h3><a name=taskobjects>Tasks</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s has the following task hierarchy :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

   int index;
   for (i=0;i<numOfTaskHierarchy;i++) {
      index = i;
      depth=0;
      while (index!=-1) {
         depth++;
         index = taskHierarchyParentIndex[index];
      }
      if (depth<depthold) buffer.AppendFormatted("</ul>\n");
      if (depth>depthold) buffer.AppendFormatted("<ul>\n");
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
      buffer.AppendFormatted("<tr><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         ROMEString comment = valueComment[i][j];
         if (valueComment[i][j].Length()>3) {
            if (valueComment[i][j][0]=='/') {
               comment = valueComment[i][j](3,valueComment[i][j].Length()-3);
            }
         }
         buffer.AppendFormatted("<tr><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",valueName[i][j].Data(),valueType[i][j].Data(),comment.Data());
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
   htmlFile.SetFormatted("%s%s%s.html",outDir.Data(),shortCut.Data(),mainProgName.Data());
   fileHandle = open(htmlFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
   close(fileHandle);
   fileHandle = open(htmlFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
   write(fileHandle,buffer.Data(), buffer.Length());
   close(fileHandle);
   if (makeOutput) htmlFile.WriteLine();

   // Write UserHTML
   struct stat buf;
   htmlFile.SetFormatted("%s%sUserHTML.html",outDir.Data(),shortCut.Data(),mainProgName.Data());
   if( stat( htmlFile.Data(), &buf )) {
      fileHandle = open(htmlFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
   }
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
bool ROMEBuilder::isNumber(ROMEString& string)
{
   if (string!="float"&&string!="Float_t"&&
       string!="double"&&string!="Double_t"&&string!="Double32_t"&&
       string!="int"&&string!="Int_t"&&
       string!="unsigned int"&&string!="UInt_t"&&
       string!="long"&&string!="Long_t"&&
       string!="unsigned long"&&string!="ULong_t"&&
       string!="short"&&string!="Short_t"&&
       string!="unsigned short"&&string!="UShort_t"&&
       string!="Long64_t"&&string!="ULong64_t"&&
       string!="long long"&&string!="unsigned long long")
      return false;
   return true;
}

bool ROMEBuilder::isFloatingType(char *type)
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
