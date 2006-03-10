/********************************************************************

  ROMEBuildReadXML.cpp, M. Schneebeli PSI

  $Id:$

********************************************************************/
#include "ROMEBuilder.h"
#include "Riostream.h"

Bool_t ROMEBuilder::ReadXMLDefinitionFile() 
{
   int i,j;
   char* name;
   bool finished = false;
   bool inputok = false;
   int type;

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
   numOfTaskHierarchy = -1;
   numOfTree = -1;
   numOfEvent = -1;
   numOfDAQ = -1;
   numOfDB = -1;
   numOfIncludeDirectories = -1;
   numOfTab = -1;
   numOfNetFolder = -1;
   numOfMFDictHeaders = 0;
   numOfMFDictIncDirs = 0;
   numOfMFWinLibs = 0;
   numOfMFUnixLibs = 0;
   numOfMFIncDirs = 0;
   numOfMFPreDefs = 0;
   numOfMFSources = 0;
   xml = new ROMEXML();
   affiliationList.RemoveAll();

   if (!xml->OpenFileForRead(xmlFile.Data())) return false;
   // read in configuration file
   while (xml->NextLine()&&!finished) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"ROMEFrameworkDefinition")) {
         inputok = true;
         while (xml->NextLine()&&!finished) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 15 && !strcmp((const char*)name,"ROMEFrameworkDefinition")) {
               if (!readExperiment) {
                  cout << "Experiment tree missing!" << endl;
                  cout << "Terminating program." << endl;
                  return false;
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
                           return false;
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
                  hasFolderUserCode = false;
                  hasFolderGenerated = false;
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
                        if (!ReadXMLFolder()) return false;
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
                           return false;
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
                        if (!ReadXMLFolder()) return false;
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
                           return false;
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
                  hasTaskUserCode = false;
                  hasTaskGenerated = false;
                  // output
                  if (makeOutput) cout << "\n\nTasks:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // task
                     if (type == 1 && !strcmp((const char*)name,"Task")) {
                        recursiveDepth = 0;
                        if (!ReadXMLTask()) return false;
                     }
                     // tasks end
                     if (type == 15 && !strcmp((const char*)name,"Tasks")) {
                        break;
                     }
                  }
                  // count tasks
                  numOfTask++;
                  // check input
                  for (i=0;i<numOfTask;i++) {
                     for (j=i+1;j<numOfTask;j++) {
                        if (taskName[i] == taskName[j]) {
                           cout << "\nTask '" << taskName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return false;
                        }
                     }
                  }
                  // default task hierarchy
                  for (i=0;i<numOfTask;i++) {
                     taskHierarchyName[i] = taskName[i];
                     taskHierarchyParentIndex[i] = -1;
                     taskHierarchyClassIndex[i] = i;
                     taskHierarchyMultiplicity[i] = 1;
                     taskHierarchyLevel[i] = 0;
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
                           return false;
                        }
                        taskHierarchyMultiplicity[numOfTaskHierarchy] = 1;
                        for (i=0;i<numOfTaskHierarchy;i++) {
                           if (taskHierarchyName[i]==taskHierarchyName[numOfTaskHierarchy] && 
                               taskHierarchyParentIndex[i]==taskHierarchyParentIndex[numOfTaskHierarchy])
                              taskHierarchyMultiplicity[numOfTaskHierarchy]++;
                        }
                        if (!taskUsed[taskHierarchyClassIndex[numOfTaskHierarchy]])
                           numOfTaskHierarchy--;
                     }
                     if (type == 1 && !strcmp((const char*)name,"Task")) {
                        depth++;
                        numOfTaskHierarchy++;
                        parentIndex[depth] = numOfTaskHierarchy;
                        taskHierarchyName[numOfTaskHierarchy] = "";
                        taskHierarchyParentIndex[numOfTaskHierarchy] = parentIndex[depth-1];
                        taskHierarchyLevel[numOfTaskHierarchy] = depth;
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
               if (!strcmp((const char*)name,"Tabs")) {
                  if (numOfTaskHierarchy==-1)
                     numOfTaskHierarchy++;
                  // initialization
                  numOfTab = -1;
                  // output
                  if (makeOutput)
                     cout << "\n\nTabs:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // tab
                     if (type == 1 && !strcmp(name, "Tab")) {
                        tabParentIndex[numOfTab + 1] = taskHierarchyParentIndex[numOfTab + 1] = -1;
                        recursiveTabDepth = 0;
                        if (!ReadXMLTab())
                           return false;
                     }
                     // tabs end
                     if (type == 15 && !strcmp(name, "Tabs")) {
                        break;
                     }
                  }
                  // count tabs
                  numOfTab++;
                  // check input
                  for (i = 0; i < numOfTab; i++) {
                     for (j = i + 1; j < numOfTab; j++) {
                        if (tabName[i] == tabName[j]) {
                           cout << "\nTab '" << tabName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return false;
                        }
                        if (tabName[i] == tabHeredity[j]) {
                           tabHeredityIndex[j] = i;
                        }
                     }
                  }
               }

               if (!strcmp((const char*)name,"NetFolders")) {
                  // initialization
                  numOfNetFolder = -1;
                  // output
                  if (makeOutput)
                     cout << "\n\nNetFolders:" << endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // net object
                     if (type == 1 && !strcmp(name, "NetFolder")) {
                        if (!ReadXMLNetFolder())
                           return false;
                     }
                     // net folders end
                     if (type == 15 && !strcmp(name, "NetFolders")) {
                        break;
                     }
                  }
                  // check input
                  for (i = 0; i < numOfNetFolder; i++) {
                     for (j = i + 1; j < numOfNetFolder; j++) {
                        if (netFolderName[i] == netFolderName[j]) {
                           cout << "\nNet folder '" << netFolderName[i].Data() << "' is defined twice !" << endl;
                           cout << "Terminating program." << endl;
                           return false;
                        }
                     }
                  }
                  // count net object
                  numOfNetFolder++;
               }

               if (!strcmp((const char*)name,"Trees")) {
                  numOfTree = -1;
                  if (!ReadXMLTree()) return false;
               }
               if (!strcmp((const char*)name,"UserDAQSystems")) {
                  numOfDAQ = -1;
                  if (!ReadXMLDAQ()) return false;
               }
               if (!strcmp((const char*)name,"UserDatabases")) {
                  numOfDB = -1;
                  if (!ReadXMLDB()) return false;
               }
               if (!strcmp((const char*)name,"MidasBanks")) {
                  numOfEvent = -1;
                  if (!ReadXMLMidasBanks()) return false;
               }
               if (!strcmp((const char*)name,"GlobalSteeringParameters")) {
                  if (numOfTaskHierarchy==-1)
                     numOfTaskHierarchy++;
                  readGlobalSteeringParameters = true;
                  // output
                  if (makeOutput) cout << "\n\nGlobal Steering Parameters:" << endl;
                  // initialisation
                  steerName[numOfTaskHierarchy][0] = "GlobalSteering";
                  actualSteerIndex = 0;
                  recursiveSteerDepth = 0;
                  steerParent[numOfTaskHierarchy][0] = -1;
                  numOfSteering[numOfTaskHierarchy] = -1;
                  if (!ReadXMLSteering(numOfTaskHierarchy)) return false;
                  numOfSteering[numOfTaskHierarchy]++;
               }
               if (!strcmp((const char*)name,"UserMakeFile")) {
                  if (!ReadXMLUserMakefile()) return false;
               }
            }
         }
      }
   }
   if (!inputok) {
      cout << "\nCould not find the <ROMEFrameworkDefinition> tag in the inputfile '"<< xmlFile.Data() << "'." << endl;
      cout << "Terminating the ROMEBuilder." << endl;
      return false;
   }
   delete xml;
   return true;
}
Bool_t ROMEBuilder::ReadXMLFolder()
{
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
   folderNet[numOfFolder] = false;
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
         hasFolderUserCode = true;
         xml->GetValue(tmp,"false");
         if (tmp == "true")
            folderUserCode[numOfFolder] = true;
      }
      else {
         hasFolderGenerated = true;
      }
      // folder version
      if (type == 1 && !strcmp((const char*)name,"FolderVersion")) {
         xml->GetValue(folderVersion[numOfFolder],folderVersion[numOfFolder]);
         ParseSVNKeyword(folderVersion[numOfFolder]);
      }
      // folder description
      if (type == 1 && !strcmp((const char*)name,"FolderDescription"))
         xml->GetValue(folderDescription[numOfFolder],folderDescription[numOfFolder]);
      // Net Folder
      if (type == 1 && !strcmp((const char*)name,"NetFolder")) {
         xml->GetValue(tmp,"false");
         if (tmp == "true")
            folderNet[numOfFolder] = true;
      }
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
         valueNoBoundChech[numOfFolder][numOfValue[numOfFolder]] = false;
         for (iDm=0;iDm<maxNumberOfValueDimension;iDm++)
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
               if (valueType[numOfFolder][numOfValue[numOfFolder]] == "TString" || valueType[numOfFolder][numOfValue[numOfFolder]] == "ROMEString")
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
            if (type == 1 && !strcmp((const char*)name,"ArraySize")) {
               tmp = "1";
               xml->GetValue(tmp,tmp);
               istart = iend = 0;
               valueDimension[numOfFolder][numOfValue[numOfFolder]]++;
               while ((iend=tmp.Index(":",1,istart,TString::kExact))!=-1) {
                  valueDimension[numOfFolder][numOfValue[numOfFolder]]++;
                  if (valueDimension[numOfFolder][numOfValue[numOfFolder]] > maxNumberOfValueDimension) {
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
            // no boundcheck
            if (type == 1 && !strcmp((const char*)name,"NoBoundCheck")) {
               xml->GetValue(tmp,"false");
               if (tmp == "true")
                  valueNoBoundChech[numOfFolder][numOfValue[numOfFolder]] = true;
            }
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
         if (valueDimension[numOfFolder][numOfValue[numOfFolder]]>1) {
            for (iDm=1;iDm<3;iDm++) {
               if (valueArray[numOfFolder][numOfValue[numOfFolder]][iDm]=="variable") {
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
         if ((isNumber(valueType[numOfFolder][numOfValue[numOfFolder]].Data()) || isBoolType(valueType[numOfFolder][numOfValue[numOfFolder]].Data())) && valueArray[numOfFolder][numOfValue[numOfFolder]][0]=="variable" && tmp[0]!='!') {
            for (i=0;i<numOfValue[numOfFolder];i++) {
               tmp.SetFormatted("[%s]",valueName[numOfFolder][i].Data());
               if (valueComment[numOfFolder][numOfValue[numOfFolder]].Contains(tmp) && isIntType(valueType[numOfFolder][i].Data())) {
                  break;
               }
            }
            if (i==numOfValue[numOfFolder]) {
               cout << "\nError: Variable length array member '"<< valueName[numOfFolder][numOfValue[numOfFolder]].Data()<<"' should have '!' or '[length]' at the top of comment."<<endl;
               cout << "       If '!', field will not be stored in tree."<<endl;
               cout << "       If '[length]', field will be stored in tree with the length. 'length' needs to be an integer field name of the folder. It must be defined ahead of its use."<<endl;
               cout << "Terminating program." << endl;
               return false;
            }
         }
         if (isTArrayType(valueType[numOfFolder][numOfValue[numOfFolder]])) {
            if (valueDimension[numOfFolder][numOfValue[numOfFolder]]>2) {
               cout << "Array of TArray is not supported. ( '" << valueName[numOfFolder][numOfValue[numOfFolder]] << "' )"<<endl;
               cout << "Terminating program." << endl;
               return false;
            }
            if (valueDBName[numOfFolder][numOfValue[numOfFolder]].Length()
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

Bool_t ROMEBuilder::ReadXMLNetFolder()
{
   // read the net folder definitions out of the xml file
   Int_t type;
   char *name;
   ROMEString currentNetFolderName = "";

   // count netFolders
   numOfNetFolder++;
   if (numOfNetFolder >= maxNumberOfNetFolders) {
      cout << "Maximal number of net folders reached : " << maxNumberOfNetFolders << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   // initialization
   netFolderName[numOfNetFolder] = "";
   netFolderTitle[numOfNetFolder] = "";
   netFolderHost[numOfNetFolder] = "localhost";
   netFolderPort[numOfNetFolder] = "9090";
   netFolderRoot[numOfNetFolder] = shortCut;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // end net object
      if (type == 15 && !strcmp(name, "NetFolder")) {
         // check input
         if (netFolderName[numOfNetFolder] == "") {
            cout << "The " << (numOfNetFolder + 1) << ". NetFolder has no name !" << endl;
            cout << "Terminating program." << endl;
            return kFALSE;
         }
         return kTRUE;
      }
      // net object name
      if (type == 1 && !strcmp(name, "NetFolderName")) {
         xml->GetValue(netFolderName[numOfNetFolder], netFolderName[numOfNetFolder]);
         // output
         if (makeOutput)
            netFolderName[numOfNetFolder].WriteLine();
      }
      // net object title
      if (type == 1 && !strcmp(name, "NetFolderTitle"))
         xml->GetValue(netFolderTitle[numOfNetFolder], netFolderTitle[numOfNetFolder]);

      // net object host
      if (type == 1 && !strcmp(name, "NetFolderHost"))
         xml->GetValue(netFolderHost[numOfNetFolder], netFolderHost[numOfNetFolder]);

      // net object port
      if (type == 1 && !strcmp(name, "NetFolderPort"))
         xml->GetValue(netFolderPort[numOfNetFolder], netFolderPort[numOfNetFolder]);

      // net object Root
      if (type == 1 && !strcmp(name, "NetFolderRoot"))
         xml->GetValue(netFolderRoot[numOfNetFolder], netFolderRoot[numOfNetFolder]);
   }
   return kTRUE;
}


Bool_t ROMEBuilder::ReadXMLTask()
{
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
   numOfTaskAffiliations[numOfTask] = 0;
   taskUsed[numOfTask] = true;
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
         if (affiliations.GetEntriesFast()>0) {
            taskUsed[numOfTask] = false;
            for (i=0;i<affiliations.GetEntriesFast() && !taskUsed[numOfTask];i++) {
               for (j=0;j<numOfTaskAffiliations[numOfTask];j++) {
                  if (affiliations.At(i)==taskAffiliation[numOfTask][j]) {
                     taskUsed[numOfTask] = true;
                     break;
                  }
               }
            }
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
      // task affiliation
      if (type == 1 && !strcmp((const char*)name,"Affiliation")) {
         taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]] = "";
         xml->GetValue(taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]],taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]]);
         for (i=0;i<affiliationList.GetEntries();i++) {
            if (affiliationList.At(i,0) == taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]])
               break;
         }
         affiliationList.SetAt(taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]], i, 0);
         affiliationList.SetAt("Task", i, affiliationList.GetEntriesAt(i));
         affiliationList.SetAt(taskName[numOfTask], i, affiliationList.GetEntriesAt(i));
         numOfTaskAffiliations[numOfTask]++;
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
         hasTaskUserCode = true;
         xml->GetValue(tmp,"false");
         if (tmp == "true")
            taskUserCode[numOfTask] = true;
      }
      else {
         hasTaskGenerated = true;
      }
      // task author
      if (type == 1 && !strcmp((const char*)name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp((const char*)name,"AuthorName"))
               xml->GetValue(taskAuthor[numOfTask],taskAuthor[numOfTask]);
            if (type == 1 && !strcmp((const char*)name,"AuthorInstitute"))
               xml->GetValue(taskAuthorInstitute[numOfTask],taskAuthorInstitute[numOfTask]);
            if (type == 1 && !strcmp((const char*)name,"AuthorCollaboration"))
               xml->GetValue(taskAuthorCollaboration[numOfTask],taskAuthorCollaboration[numOfTask]);
            if (type == 1 && !strcmp((const char*)name,"AuthorEmail"))
               xml->GetValue(taskAuthorEmail[numOfTask],taskAuthorEmail[numOfTask]);
            if (type == 15 && !strcmp((const char*)name,"Author"))
               break;
         }
      }
      // task version
      if (type == 1 && !strcmp((const char*)name,"TaskVersion")) {
         xml->GetValue(taskVersion[numOfTask],taskVersion[numOfTask]);
         ParseSVNKeyword(taskVersion[numOfTask]);
      }
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
         numOfHistoTabs[numOfTask][numOfHistos[numOfTask]] = 0;
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
            // argus
            if (type == 1 && !strcmp((const char*)name,"Argus")) {
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // tab
                  if (type == 1 && !strcmp((const char*)name,"Tab")) {
                     if (numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]>=maxNumberOfHistoTabs) {
                        cout << "Maximal number of Tabs in the Argus section of Histogram '" << histoName[numOfTask][numOfHistos[numOfTask]].Data() << "' reached : " << maxNumberOfHistoTabs << " !" << endl;
                        cout << "Terminating program." << endl;
                        return false;
                     }
                     histoTabName[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]] = "";
                     histoTabIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]] = "0";
                     histoTabArrayIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]] = "0";
                     while (xml->NextLine()) {
                        type = xml->GetType();
                        name = xml->GetName();
                        // tab name
                        if (type == 1 && !strcmp((const char*)name,"TabName"))
                           xml->GetValue(histoTabName[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]],histoTabName[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]]);
                        // index
                        if (type == 1 && !strcmp((const char*)name,"Index"))
                           xml->GetValue(histoTabIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]],histoTabIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]]);
                        // array index
                        if (type == 1 && !strcmp((const char*)name,"HistArrayIndex"))
                           xml->GetValue(histoTabArrayIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]],histoTabArrayIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]]);
                        // tab end
                        if (type == 15 && !strcmp((const char*)name,"Tab")) {
                           numOfHistoTabs[numOfTask][numOfHistos[numOfTask]]++;
                           break;
                        }
                     }
                  }
                  // argus end
                  if (type == 15 && !strcmp((const char*)name,"Argus"))
                     break;
               }
            }
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

Bool_t ROMEBuilder::ReadXMLTab()
{
   // read the tab definitions out of the xml file
   char *name;
   Int_t type, i, j;
   ROMEString currentTabName = "";
   Int_t currentNumberOfTabs = 0;

   // count tabs
   numOfTab++;
   currentNumberOfTabs = numOfTab;
   if (currentNumberOfTabs >= maxNumberOfTabs) {
      cout << "Maximal number of tabs reached : " << maxNumberOfTabs << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   // initialisation
   tabName[currentNumberOfTabs] = "";
   tabTitle[currentNumberOfTabs] = "";
   numOfTabAffiliations[numOfTab] = 0;
   tabUsed[numOfTab] = true;
   tabAuthor[currentNumberOfTabs] = mainAuthor;
   tabVersion[currentNumberOfTabs] = "1";
   tabDescription[currentNumberOfTabs] = "";
   tabHeredity[currentNumberOfTabs] = "";
   tabHeredityIndex[currentNumberOfTabs] = 0;
   numOfSteering[currentNumberOfTabs+numOfTaskHierarchy+1] = -1;
   numOfMenu[currentNumberOfTabs] = -1;
   tabNumOfChildren[currentNumberOfTabs] = 0;
   numOfThreadFunctions[currentNumberOfTabs] = 0;
   numOfTabHistos[currentNumberOfTabs] = 0;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();

      // subtab
      if (type == 1 && !strcmp(name, "Tab")) {
         // set tab as parent for subsequent tabs
         recursiveTabDepth++;
         tabParentIndex[numOfTab + 1] = currentNumberOfTabs;
         tabNumOfChildren[currentNumberOfTabs]++;

         // read subtab
         if (!ReadXMLTab())
            return kFALSE;
         continue;
      }
      // end tab
      if (type == 15 && !strcmp(name, "Tab")) {
         // check input
         if (currentTabName == "") {
            cout << "The " << (currentNumberOfTabs + 1) << ". Tab has no name !" << endl;
            cout << "Terminating program." << endl;
            return kFALSE;
         }
         if (affiliations.GetEntriesFast()>0) {
            tabUsed[numOfTab] = false;
            for (i=0;i<affiliations.GetEntriesFast() && !tabUsed[numOfTab];i++) {
               for (j=0;j<numOfTabAffiliations[numOfTab];j++) {
                  if (affiliations.At(i)==tabAffiliation[numOfTab][j]) {
                     tabUsed[numOfTab] = true;
                     break;
                  }
               }
            }
         }
         recursiveTabDepth--;
         return kTRUE;
      }
      // tab name
      if (type == 1 && !strcmp(name, "TabName")) {
         xml->GetValue(tabName[currentNumberOfTabs], tabName[currentNumberOfTabs]);
         currentTabName = tabName[currentNumberOfTabs];
         // output
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth; i++)
               cout << "   ";
         if (makeOutput)
            tabName[currentNumberOfTabs].WriteLine();
      }
      // tab title
      if (type == 1 && !strcmp(name, "TabTitle"))
         xml->GetValue(tabTitle[currentNumberOfTabs], tabTitle[currentNumberOfTabs]);
      // tab affiliation
      if (type == 1 && !strcmp((const char*)name,"Affiliation")) {
         tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]] = "";
         xml->GetValue(tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]],tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]]);
         for (i=0;i<affiliationList.GetEntries();i++) {
            if (affiliationList.At(i,0) == tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]])
               break;
         }
         affiliationList.SetAt(tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]], i, 0);
         affiliationList.SetAt("Tab", i, affiliationList.GetEntriesAt(i));
         affiliationList.SetAt(tabName[numOfTab], i, affiliationList.GetEntriesAt(i));
         numOfTabAffiliations[numOfTab]++;
      }

      // tab author
      if (type == 1 && !strcmp((const char*)name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp((const char*)name,"AuthorName"))
               xml->GetValue(tabAuthor[currentNumberOfTabs],tabAuthor[currentNumberOfTabs]);
            if (type == 1 && !strcmp((const char*)name,"AuthorInstitute"))
               xml->GetValue(tabAuthorInstitute[currentNumberOfTabs],tabAuthorInstitute[currentNumberOfTabs]);
            if (type == 1 && !strcmp((const char*)name,"AuthorCollaboration"))
               xml->GetValue(tabAuthorCollaboration[currentNumberOfTabs],tabAuthorCollaboration[currentNumberOfTabs]);
            if (type == 1 && !strcmp((const char*)name,"AuthorEmail"))
               xml->GetValue(tabAuthorEmail[currentNumberOfTabs],tabAuthorEmail[currentNumberOfTabs]);
            if (type == 15 && !strcmp((const char*)name,"Author"))
               break;
         }
      }

      // tab version
      if (type == 1 && !strcmp(name, "TabVersion")) {
         xml->GetValue(tabVersion[currentNumberOfTabs], tabVersion[currentNumberOfTabs]);
         ParseSVNKeyword(tabVersion[currentNumberOfTabs]);
      }

      // tab description
      if (type == 1 && !strcmp(name, "TabDescription"))
         xml->GetValue(tabDescription[currentNumberOfTabs], tabDescription[currentNumberOfTabs]);
      // tab heredity
      if (type == 1 && !strcmp(name, "InheritedFrom"))
         xml->GetValue(tabHeredity[currentNumberOfTabs], tabHeredity[currentNumberOfTabs]);

      // tab steering parameters
      if (type == 1 && !strcmp(name, "SteeringParameters")) {
         // read steering parameter
         steerName[currentNumberOfTabs+numOfTaskHierarchy+1][0] = "Steering";
         steerParent[currentNumberOfTabs+numOfTaskHierarchy+1][0] = -1;
         actualSteerIndex = 0;
         recursiveSteerDepth = 0;
         if (!ReadXMLSteering(currentNumberOfTabs+numOfTaskHierarchy+1))
            return kFALSE;
         numOfSteering[currentNumberOfTabs+numOfTaskHierarchy+1]++;
      }
      // tab threadFunctions
      if (type == 1 && !strcmp(name, "ThreadFunctions")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout << "   ";
         if (makeOutput)
            cout << "ThreadFunctions:" << endl;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // end
            if (type == 15 && !strcmp(name, "ThreadFunctions"))
               break;

            // thread function
            if (type == 1 && !strcmp(name, "ThreadFunction")) {
               threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] = "";
               numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  if (type == 1 && !strcmp(name, "FunctionName"))
                     xml->GetValue(threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]], threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]);
                  if (type == 1 && !strcmp(name, "FunctionArgument")) {
                     xml->GetValue(threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]], threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]]);
                     numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]++;
                  }
                  // end
                  if (type == 15 && !strcmp(name, "ThreadFunction")) {
                     // output
                     if (makeOutput)
                        for (i = 0; i < recursiveTabDepth + 2; i++)
                           cout << "   ";
                     if (makeOutput)
                        threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]].WriteLine();
                     // check input
                     if (threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] == "") {
                        cout << "A thread function of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Name !" << endl;
                        cout << "Terminating program." << endl;
                        return kFALSE;
                     }
                     for (j = 0; j < numOfThreadFunctions[currentNumberOfTabs]; j++) {
                        if (threadFunctionName[currentNumberOfTabs][j] == threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]) {
                           cout << "Two thread functions of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Name !" << endl;
                           cout << "Terminating program." << endl;
                           return kFALSE;
                        }
                     }
                     // count thread functions
                     numOfThreadFunctions[currentNumberOfTabs]++;
                     if (numOfThreadFunctions[currentNumberOfTabs] >= maxNumberOfThreadFunctions) {
                        cout << "Maximal number of thread functions reached : " << maxNumberOfThreadFunctions << " !" << endl;
                        cout << "Terminating program." << endl;
                        return kFALSE;
                     }
                     break;
                  }
               }
            }
         }
      }
      // tab menu
      if (type == 1 && !strcmp(name, "Menus")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout << "   ";
         if (makeOutput)
            cout << "Menus:" << endl;
         recursiveMenuDepth = 0;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();

            // end
            if (type == 15 && !strcmp(name, "Menus")) {
               numOfMenu[currentNumberOfTabs]++;
               break;
            }
            // menu
            if (type == 1 && !strcmp(name, "Menu")) {
               recursiveMenuDepth++;
               numOfMenu[currentNumberOfTabs]++;
               if (!ReadXMLMenu(currentNumberOfTabs))
                  return kFALSE;
               recursiveMenuDepth--;
            }
         }
      }

   }

   return kTRUE;
}

Bool_t ROMEBuilder::ReadXMLMenu(Int_t currentNumberOfTabs)
{
   Int_t type, i, j;
   char *name;

   Int_t currentNumberOfMenus = numOfMenu[currentNumberOfTabs];
   menuDepth[currentNumberOfTabs][currentNumberOfMenus] = recursiveMenuDepth;
   // count menus
   numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] = -1;
   if (currentNumberOfMenus >= maxNumberOfMenus) {
      cout << "Maximal number of menus reached : " << maxNumberOfMenus << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   menuTitle[currentNumberOfTabs][currentNumberOfMenus] = "";

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();

      // end
      if (type == 15 && !strcmp(name, "Menu")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 2; i++)
               cout << "   ";
         if (makeOutput)
            menuTitle[currentNumberOfTabs][currentNumberOfMenus].WriteLine();
         numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
         for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
            if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE) {
               if (makeOutput)
                  for (i = 0; i < recursiveTabDepth + 3; i++)
                     cout << "   ";
               if (makeOutput)
                  menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j].WriteLine();
            }
         }
         break;
      }
      if (type == 1 && !strcmp(name, "MenuTitle"))
         xml->GetValue(menuTitle[currentNumberOfTabs][currentNumberOfMenus], menuTitle[currentNumberOfTabs][currentNumberOfMenus]);

      // tab menu items
      if (type == 1 && !strcmp(name, "MenuItems")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();

            // end
            if (type == 15 && !strcmp(name, "MenuItems"))
               break;

            // menu
            if (type == 1 && !strcmp(name, "Menu")) {
               recursiveMenuDepth++;
               numOfMenu[currentNumberOfTabs]++;
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = numOfMenu[currentNumberOfTabs];
               if (!ReadXMLMenu(currentNumberOfTabs))
                  return kFALSE;
               recursiveMenuDepth--;
            }
            // Splitter
            if (type == 1 && !strcmp(name, "Line")) {
               // count menu items
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = LINE_TITLE;
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // end
                  if (type == 15 && !strcmp(name, "Line"))
                     break;
               }
            }
            // menu item
            if (type == 1 && !strcmp(name, "MenuItem")) {
               // count menu items
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout << "Maximal number of menu items reached : " << maxNumberOfMenuItems << " !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = "";
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  // end
                  if (type == 15 && !strcmp(name, "MenuItem"))
                     break;
                  if (type == 1 && !strcmp(name, "MenuItemTitle"))
                     xml->GetValue(menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]], menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                  if (type == 1 && !strcmp(name, "MenuItemEnumName")) {
                     xml->GetValue(menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]], menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                  }
               }

               // check input
               if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]] == "") {
                  cout << currentNumberOfTabs << endl;
                  cout << currentNumberOfMenus << endl;
                  cout << numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] << endl;
                  cout << "A menu item of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
                  cout << "Terminating program." << endl;
                  return kFALSE;
               }
               for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
                  if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE && menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j]
                      == menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]) {
                     cout << "Two menu items of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
                     cout << "Terminating program." << endl;
                     return kFALSE;
                  }
               }
            }
         }
      }
   }

   // check input
   if (menuTitle[currentNumberOfTabs][currentNumberOfMenus] == "") {
      cout << "A menu of tab '" << tabName[currentNumberOfTabs].Data() << "' has no Title !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }
   for (j = 0; j < currentNumberOfMenus; j++) {
      if (menuTitle[currentNumberOfTabs][j] == menuTitle[currentNumberOfTabs][currentNumberOfMenus]) {
         cout << "Two menus of tab '" << tabName[currentNumberOfTabs].Data() << "' have the same Title !" << endl;
         cout << "Terminating program." << endl;
         return kFALSE;
      }
   }

   return kTRUE;
}

Bool_t ROMEBuilder::ReadXMLTree()
{
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
         treeDescription[numOfTree] = "";

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
            // tree description
            if (type == 1 && !strcmp((const char*)name,"TreeDescription"))
               xml->GetValue(treeDescription[numOfTree],treeDescription[numOfTree]);
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

Bool_t ROMEBuilder::ReadXMLDAQ()
{
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

Bool_t ROMEBuilder::ReadXMLDB()
{
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

Bool_t ROMEBuilder::ReadXMLMidasBanks()
{
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

Bool_t ROMEBuilder::ReadXMLSteering(Int_t iTask)
{
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
               if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "TString" || steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "ROMEString")
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
               if (steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]=="FileName")
                  cout << endl;
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
               if ( usedCLO.Contains(cloption) ) {
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
            if (type == 15 && !strcmp((const char*)name,"SteeringParameterField")) {
               if (steerArraySize[iTask][actualSteerIndex].Length() && steerArraySize[iTask][actualSteerIndex]!="1" && steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].Length()) {
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

Bool_t ROMEBuilder::ReadXMLUserMakefile()
{
   int i,j;
   char *name;
   int type;
   ROMEString temp;
   ROMEString ext;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp((const char*)name,"DictionaryHeaders")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // header
            if (type == 1 && !strcmp((const char*)name,"Header")) {
               numOfMFDictHeaderAffiliations[numOfMFDictHeaders] = 0;
               mfDictHeaderName[numOfMFDictHeaders] = "";
               mfDictHeaderUsed[numOfMFDictHeaders] = true;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // header name
                  if (type == 1 && !strcmp((const char*)name,"HeaderName")) {
                     xml->GetValue(mfDictHeaderName[numOfMFDictHeaders],mfDictHeaderName[numOfMFDictHeaders]);
                     if (mfDictHeaderName[numOfMFDictHeaders].Length()>0) {
                        if (mfDictHeaderName[numOfMFDictHeaders].Index(".")==-1)
                           mfDictHeaderName[numOfMFDictHeaders].Append(".h");
                     }
                  }
                  // affiliation
                  if (type == 1 && !strcmp((const char*)name,"Affiliation")) {
                     mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]] = "";
                     xml->GetValue(mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]],mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]]);
                     for (i=0;i<affiliationList.GetEntries();i++) {
                        if (affiliationList.At(i,0) == mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]])
                           break;
                     }
                     affiliationList.SetAt(mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]], i, 0);
                     affiliationList.SetAt("Class", i, affiliationList.GetEntriesAt(i));
                     affiliationList.SetAt(mfDictHeaderName[numOfMFDictHeaders], i, affiliationList.GetEntriesAt(i));
                     numOfMFDictHeaderAffiliations[numOfMFDictHeaders]++;
                  }
                  if (type == 15 && !strcmp((const char*)name,"Header")) {
                     if (affiliations.GetEntriesFast()>0) {
                        mfDictHeaderUsed[numOfMFDictHeaders] = false;
                        for (i=0;i<affiliations.GetEntriesFast() && !mfDictHeaderUsed[numOfMFDictHeaders];i++) {
                           for (j=0;j<numOfMFDictHeaderAffiliations[numOfMFDictHeaders];j++) {
                              if (affiliations.At(i)==mfDictHeaderAffiliation[numOfMFDictHeaders][j]) {
                                 mfDictHeaderUsed[numOfMFDictHeaders] = true;
                                 break;
                              }
                           }
                        }
                     }
                     numOfMFDictHeaders++;
                     break;
                  }
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
            if (type == 1 && !strcmp((const char*)name,"Library")) {
               mfWinLibName[numOfMFWinLibs] = "";
               numOfMFWinLibFlags[numOfMFWinLibs] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // library name
                  if (type == 1 && !strcmp((const char*)name,"LibraryName")) {
                     xml->GetValue(mfWinLibName[numOfMFWinLibs],mfWinLibName[numOfMFWinLibs]);
                     if (mfWinLibName[numOfMFWinLibs].Length()>0) {
                        if (mfWinLibName[numOfMFWinLibs].Index(".")==-1)
                           mfWinLibName[numOfMFWinLibs].Append(".lib");
                     }
                  }
                  // flags
                  if (type == 1 && !strcmp((const char*)name,"NeededFlag")) {
                     mfWinLibFlag[numOfMFWinLibs][numOfMFWinLibFlags[numOfMFWinLibs]] = "";
                     xml->GetValue(mfWinLibFlag[numOfMFWinLibs][numOfMFWinLibFlags[numOfMFWinLibs]],mfWinLibFlag[numOfMFWinLibs][numOfMFWinLibFlags[numOfMFWinLibs]]);
                     numOfMFWinLibFlags[numOfMFWinLibs]++;
                  }   
                  if (type == 15 && !strcmp((const char*)name,"Library")) {
                     numOfMFWinLibs++;
                     break;
                  }
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
            if (type == 1 && !strcmp((const char*)name,"Library")) {
               mfUnixLibName[numOfMFUnixLibs] = "";
               numOfMFUnixLibFlags[numOfMFUnixLibs] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // library name
                  if (type == 1 && !strcmp((const char*)name,"LibraryName")) {
                     xml->GetValue(mfUnixLibName[numOfMFUnixLibs],mfUnixLibName[numOfMFUnixLibs]);
                  }
                  // flags
                  if (type == 1 && !strcmp((const char*)name,"NeededFlag")) {
                     mfUnixLibFlag[numOfMFUnixLibs][numOfMFUnixLibFlags[numOfMFUnixLibs]] = "";
                     xml->GetValue(mfUnixLibFlag[numOfMFUnixLibs][numOfMFUnixLibFlags[numOfMFUnixLibs]],mfUnixLibFlag[numOfMFUnixLibs][numOfMFUnixLibFlags[numOfMFUnixLibs]]);
                     numOfMFUnixLibFlags[numOfMFUnixLibs]++;
                  }   
                  if (type == 15 && !strcmp((const char*)name,"Library")) {
                     numOfMFUnixLibs++;
                     break;
                  }
               }
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
      if (type == 1 && !strcmp((const char*)name,"AdditionalFiles")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // source file
            if (type == 1 && !strcmp((const char*)name,"File")) {
               numOfMFSourceFlags[numOfMFSources] = 0;
               numOfMFSourceAffiliations[numOfMFSources] = 0;
               mfSourceFileUsed[numOfMFSources] = true;
               mfSourceFileName[numOfMFSources] = "";
               mfSourceFilePath[numOfMFSources] = "";
               mfHeaderFileName[numOfMFSources] = "";
               mfHeaderFilePath[numOfMFSources] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // source file
                  if (type == 1 && !strcmp((const char*)name,"SourceFile")) {
                     xml->GetValue(temp,"");
                     AnalyzeFileName(temp.Data(),mfSourceFilePath[numOfMFSources],mfSourceFileName[numOfMFSources],ext);
                     if (ext.Length()==0)
                        mfSourceFileName[numOfMFSources].Append(".cpp");
                     else
                        mfSourceFileName[numOfMFSources].AppendFormatted(".%s",ext.Data());
                     if (mfSourceFilePath[numOfMFSources].Length()>0) {
                        if (mfSourceFilePath[numOfMFSources][mfSourceFilePath[numOfMFSources].Length()-1]!='/' ||
                           mfSourceFilePath[numOfMFSources][mfSourceFilePath[numOfMFSources].Length()-1]!='\\')
                           mfSourceFilePath[numOfMFSources].Append("/");
                     }
                  }
                  // header file
                  if (type == 1 && !strcmp((const char*)name,"HeaderFile")) {
                     xml->GetValue(temp,"");
                     AnalyzeFileName(temp.Data(),mfHeaderFilePath[numOfMFSources],mfHeaderFileName[numOfMFSources],ext);
                     if (ext.Length()==0)
                        mfHeaderFileName[numOfMFSources].Append(".h");
                     else
                        mfHeaderFileName[numOfMFSources].AppendFormatted(".%s",ext.Data());
                     if (mfHeaderFilePath[numOfMFSources].Length()>0) {
                        if (mfHeaderFilePath[numOfMFSources][mfHeaderFilePath[numOfMFSources].Length()-1]!='/' ||
                           mfHeaderFilePath[numOfMFSources][mfHeaderFilePath[numOfMFSources].Length()-1]!='\\')
                           mfHeaderFilePath[numOfMFSources].Append("/");
                     }
                  }
                  // flags
                  if (type == 1 && !strcmp((const char*)name,"NeededFlag")) {
                     mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]] = "";
                     xml->GetValue(mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]],mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]]);
                     numOfMFSourceFlags[numOfMFSources]++;
                  }
                  // affiliation
                  if (type == 1 && !strcmp((const char*)name,"Affiliation")) {
                     mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]] = "";
                     xml->GetValue(mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]],mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]]);
                     for (i=0;i<affiliationList.GetEntries();i++) {
                        if (affiliationList.At(i,0) == mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]])
                           break;
                     }
                     affiliationList.SetAt(mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]], i, 0);
                     if (mfSourceFileName[numOfMFSources].Length()) {
                        affiliationList.SetAt("Source", i, affiliationList.GetEntriesAt(i));
                        affiliationList.SetAt(mfSourceFileName[numOfMFSources], i, affiliationList.GetEntriesAt(i));
                     }
                     if (mfHeaderFileName[numOfMFSources].Length()) {
                        affiliationList.SetAt("Header", i, affiliationList.GetEntriesAt(i));
                        affiliationList.SetAt(mfHeaderFileName[numOfMFSources], i, affiliationList.GetEntriesAt(i));
                     }
                     numOfMFSourceAffiliations[numOfMFSources]++;
                  }
                  if (type == 15 && !strcmp((const char*)name,"File")) {
                     if (affiliations.GetEntriesFast()>0) {
                        mfSourceFileUsed[numOfMFSources] = false;
                        for (i=0;i<affiliations.GetEntriesFast() && !mfSourceFileUsed[numOfMFSources];i++) {
                           for (j=0;j<numOfMFSourceAffiliations[numOfMFSources];j++) {
                              if (affiliations.At(i)==mfSourceFileAffiliation[numOfMFSources][j]) {
                                 mfSourceFileUsed[numOfMFSources] = true;
                                 break;
                              }
                           }
                        }
                     }
                     numOfMFSources++;
                     break;
                  }
               }
            }
            if (type == 15 && !strcmp((const char*)name,"AdditionalFiles"))
               break;
         }
      }

      if (type == 15 && !strcmp((const char*)name,"UserMakeFile")) {
         break;
      }
   }
   return true;
}


void ROMEBuilder::ParseSVNKeyword(ROMEString& str)
{
   // extract Subversion revision number from string.
   if (!str.BeginsWith("$") || !str.EndsWith(" $")) // This isn't Subversion keyword
      return;

   if (str.BeginsWith("$Rev: "))                      str.Remove(0, 6);
   else if (str.BeginsWith("$Revision: "))            str.Remove(0, 11);
   else if (str.BeginsWith("$LastChangedRevision: ")) str.Remove(0, 22);
   else{
      cout << "Warning: ROMEBuilder does not support keyword '"<<str<<"'"<<endl;
      return;
   }
   str.Remove(str.Length()-2, 2);
   return;
}
