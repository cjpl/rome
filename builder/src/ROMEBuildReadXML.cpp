/********************************************************************

  ROMEBuildReadXML.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <Riostream.h>
#include "ROMEBuilder.h"
#include "ROMEXML.h"

//______________________________________________________________________________
Bool_t ROMEBuilder::AllocateMemorySpace()
{
   xml = new ROMEXML();
   xml->OpenFileForPath(xmlFile.Data());
   xml->GetPathAttribute("ROMEFrameworkDefinition", "xsi:noNamespaceSchemaLocation",xsdFile,"rome.xsd");

   Int_t nfound;
   Int_t nfound2;
   Int_t tmp;
   Int_t tmp2;
   ROMEString path;
   ROMEString path2;

   // Folders
   maxNumberOfFolders = 0;
   path = "ROMEFrameworkDefinition/Folders";
   while(1) {
      path.AppendFormatted("/Folder");
      nfound = xml->NumberOfOccurrenceOfPath(path);
      if (nfound) {
         maxNumberOfFolders += nfound;
      } else {
         break;
      }
   }
   path = "ROMEFrameworkDefinition/SupportFolders";
   while(1) {
      path.AppendFormatted("/SupportFolder");
      nfound = xml->NumberOfOccurrenceOfPath(path);
      if (nfound) {
         maxNumberOfFolders += nfound;
      } else {
         break;
      }
   }
   maxNumberOfFolders++; // for configuration folder

   // Tasks
   tmp = 0;
   path = "ROMEFrameworkDefinition/Tasks";
   while(1) {
      path.AppendFormatted("/Task");
      nfound = xml->NumberOfOccurrenceOfPath(path);
      if (nfound) {
         tmp += nfound;
      } else {
         break;
      }
   }
   tmp2 = 0;
   path = "ROMEFrameworkDefinition/TaskHierarchy";
   while(1) {
      path.AppendFormatted("/Task");
      nfound = xml->NumberOfOccurrenceOfPath(path);
      if (nfound) {
         tmp2 += nfound;
      } else {
         break;
      }
   }
   maxNumberOfTasks = TMath::Max(tmp, tmp2);

   // Tab
   maxNumberOfTabs = 0;
   path = "ROMEFrameworkDefinition/Tabs";
   while(1) {
      path.AppendFormatted("/Tab");
      nfound = xml->NumberOfOccurrenceOfPath(path);
      if (nfound) {
         maxNumberOfTabs += nfound;
      } else {
         break;
      }
   }
   path = "ROMEFrameworkDefinition/Tasks";
   while(1) {
      path.AppendFormatted("/Task");
      path2 = path;
      path2.AppendFormatted("/Histogram/Argus/Tab");
      nfound = xml->NumberOfOccurrenceOfPath(path);
      nfound2 = xml->NumberOfOccurrenceOfPath(path2);
      if (nfound) {
         if (nfound2)
            maxNumberOfTabs += nfound2;
      } else {
         break;
      }
   }

   // Net folders
   path = "ROMEFrameworkDefinition/NetFolders/NetFolder";
   maxNumberOfNetFolders = xml->NumberOfOccurrenceOfPath(path);

   // Trees
   path = "ROMEFrameworkDefinition/Trees/Tree";
   maxNumberOfTrees = xml->NumberOfOccurrenceOfPath(path);

   // User DAQ system
   path = "ROMEFrameworkDefinition/UserDAQSystems/UserDAQSystem";
   maxNumberOfDAQ = xml->NumberOfOccurrenceOfPath(path);

   // Use database
   path = "ROMEFrameworkDefinition/UserDataBasess/UserDataBase";
   maxNumberOfDB = xml->NumberOfOccurrenceOfPath(path);

   // Midas banks
   path = "ROMEFrameworkDefinition/MidasBanks/EventDefinition";
   maxNumberOfEvents = xml->NumberOfOccurrenceOfPath(path);

   // Root DAQ
   path = "ROMEFrameworkDefinition/RootDAQ/RootTree";
   maxNumberOfRootTrees = xml->NumberOfOccurrenceOfPath(path);

   // User makefile
   {
      // Dict headers
      path = "ROMEFrameworkDefinition/UserMakeFile/DictionaryHeaders/Header";
      maxNumberOfMFDictHeaders = xml->NumberOfOccurrenceOfPath(path);
      // Dict include directories
      path = "ROMEFrameworkDefinition/UserMakeFile/DictionaryIncludesDirectories/IncludeDirectory";
      maxNumberOfMFDictIncDirs = xml->NumberOfOccurrenceOfPath(path);
      // Windows libraries
      path = "ROMEFrameworkDefinition/UserMakeFile/WindowsLibraries/Library";
      maxNumberOfMFWinLibs = xml->NumberOfOccurrenceOfPath(path);
      // UNIX libraries
      path = "ROMEFrameworkDefinition/UserMakeFile/UnixLibraries/Library";
      maxNumberOfMFUnixLibs = xml->NumberOfOccurrenceOfPath(path);
      // Include directories
      path = "ROMEFrameworkDefinition/UserMakeFile/IncludeDirectories/IncludeDirectory";
      maxNumberOfMFIncDirs = xml->NumberOfOccurrenceOfPath(path);
      // Preprocessor
      path = "ROMEFrameworkDefinition/UserMakeFile/PreprocessorDefinition/Name";
      maxNumberOfMFPreDefs = xml->NumberOfOccurrenceOfPath(path);
      // Source
      path = "ROMEFrameworkDefinition/UserMakeFile/AdditionalFiles/File";
      maxNumberOfMFSources = xml->NumberOfOccurrenceOfPath(path);
   }

   // Allocate memory
   parent = static_cast<ROMEString*>(AllocateROMEString(TMath::Max(maxNumberOfTasks,maxNumberOfFolders) + 1));

   numOfValue = static_cast<Int_t*>(AllocateInt(maxNumberOfFolders));
   numOfFolderInclude = static_cast<Int_t*>(AllocateInt(maxNumberOfFolders));

   numOfFolderAffiliations = static_cast<Int_t*>(AllocateInt(maxNumberOfFolders));
   folderAffiliation = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfAffiliations));
   folderUsed = static_cast<Bool_t*>(AllocateBool(maxNumberOfFolders));
   folderName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderShortDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderParentName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderInheritName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderTitle = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderArray = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderAuthor = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderVersion = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfFolders));
   folderInclude = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfInclude));
   folderLocalFlag = static_cast<Bool_t**>(AllocateBool(maxNumberOfFolders,maxNumberOfInclude));
   folderDataBase = static_cast<Bool_t*>(AllocateBool(maxNumberOfFolders));
   folderUserCode = static_cast<Bool_t*>(AllocateBool(maxNumberOfFolders));
   folderSupport = static_cast<Bool_t*>(AllocateBool(maxNumberOfFolders));
   folderNoReset = static_cast<Bool_t*>(AllocateBool(maxNumberOfFolders));
   folderNoResetModified = static_cast<Bool_t*>(AllocateBool(maxNumberOfFolders));
   folderHasClassField = static_cast<Bool_t*>(AllocateBool(maxNumberOfFolders));

   valueName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueType = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueInit = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueComment = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueDimension = static_cast<Int_t**>(AllocateInt(maxNumberOfFolders,maxNumberOfValues));
   valueArray = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues,
                                                              maxNumberOfValueDimension));
   valueArraySpecifier = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueDBName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueDBPath = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueDBIf = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfFolders,maxNumberOfValues));
   valueNoBoundChech = static_cast<Bool_t**>(AllocateBool(maxNumberOfFolders,maxNumberOfValues));
   valueIsTObject = static_cast<Bool_t**>(AllocateBool(maxNumberOfFolders,maxNumberOfValues));

   // net folder
   netFolderName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfNetFolders));
   netFolderTitle = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfNetFolders));
   netFolderHost = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfNetFolders));
   netFolderPort = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfNetFolders));
   netFolderRoot = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfNetFolders));

   // task
   numOfTaskInclude = static_cast<Int_t*>(AllocateInt(maxNumberOfTasks));
   taskName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   numOfTaskAffiliations = static_cast<Int_t*>(AllocateInt(maxNumberOfTasks));
   taskAffiliation = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfAffiliations));
   taskUsed = static_cast<Bool_t*>(AllocateBool(maxNumberOfTasks));
   taskEventID = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskShortDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskUsage = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskStatus = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskToDo = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskKnownProblems = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskAuthor = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskAuthorInstitute = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskAuthorCollaboration = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskAuthorEmail = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskVersion = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskDependence = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTasks));
   taskInclude = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfInclude));
   taskLocalFlag = static_cast<Bool_t**>(AllocateBool(maxNumberOfTasks,maxNumberOfInclude));
   numOfTaskAccessedFolder = static_cast<Int_t*>(AllocateInt(maxNumberOfTasks));
   taskAccessedFolder = static_cast<Int_t**>(AllocateInt(maxNumberOfTasks,maxNumberOfFolders));
   numOfHistos = static_cast<Int_t*>(AllocateInt(maxNumberOfTasks));
   histoName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoTitle = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoFolderName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoFolderTitle = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoType = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoArraySize = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoArrayStartIndex = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoXLabel = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoYLabel = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoZLabel = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoXNbins = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoXmin = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoXmax = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoYNbins = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoYmin = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoYmax = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoZNbins = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoZmin = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   histoZmax = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos));
   numOfHistoSingleObjectTabs = static_cast<Int_t**>(AllocateInt(maxNumberOfTasks,maxNumberOfHistos));
   histoSingleObjectTabName = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos,
                                                                            maxNumberOfHistoSingleObjectTabs));
   histoSingleObjectTabIndex = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos,
                                                                             maxNumberOfHistoSingleObjectTabs));
   histoSingleObjectTabArrayIndex = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks,maxNumberOfHistos,
                                                                                  maxNumberOfHistoSingleObjectTabs));
   numOfGraphs = static_cast<Int_t*>(AllocateInt(maxNumberOfTasks));
   graphName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphTitle = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphFolderName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphFolderTitle = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphType = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphArraySize = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphArrayStartIndex = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphXLabel = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphYLabel = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphZLabel = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphXmin = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphXmax = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphYmin = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphYmax = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphZmin = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   graphZmax = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs));
   numOfGraphSingleObjectTabs = static_cast<Int_t**>(AllocateInt(maxNumberOfTasks,maxNumberOfGraphs));
   graphSingleObjectTabName = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs,
                                                                            maxNumberOfGraphSingleObjectTabs));
   graphSingleObjectTabIndex = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs,
                                                                             maxNumberOfGraphSingleObjectTabs));
   graphSingleObjectTabArrayIndex = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks,maxNumberOfGraphs,
                                                                                  maxNumberOfGraphSingleObjectTabs));

   // task hierarchy
   taskHierarchyName = static_cast<ROMEString*>(AllocateROMEString(2*maxNumberOfTasks));
   taskHierarchyParentIndex = static_cast<Int_t*>(AllocateInt(2*maxNumberOfTasks));
   taskHierarchyClassIndex = static_cast<Int_t*>(AllocateInt(2*maxNumberOfTasks));
   taskHierarchyMultiplicity = static_cast<Int_t*>(AllocateInt(2*maxNumberOfTasks));
   taskHierarchyLevel = static_cast<Int_t*>(AllocateInt(2*maxNumberOfTasks));
   taskHierarchyObjectIndex = static_cast<Int_t*>(AllocateInt(2*maxNumberOfTasks));
   taskHierarchySuffix = static_cast<ROMEString*>(AllocateROMEString(2*maxNumberOfTasks));

   // steering
   numOfSteering = static_cast<Int_t*>(AllocateInt(maxNumberOfTasks+maxNumberOfTabs + 1));
   numOfSteerFields = static_cast<Int_t**>(AllocateInt(maxNumberOfTasks+maxNumberOfTabs + 1,maxNumberOfSteering + 1));
   numOfSteerChildren = static_cast<Int_t**>(AllocateInt(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                         maxNumberOfSteering + 1));
   steerName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,maxNumberOfSteering));
   steerArraySize = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                 maxNumberOfSteering));
   steerParent = static_cast<Int_t**>(AllocateInt(maxNumberOfTasks+maxNumberOfTabs + 1,maxNumberOfSteering));
   steerChildren = static_cast<Int_t***>(AllocateInt(maxNumberOfTasks+maxNumberOfTabs + 1,maxNumberOfSteering,
                                                     maxNumberOfSteering));
   numOfSteerAffiliations = static_cast<Int_t**>(AllocateInt(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                             maxNumberOfSteering));
   steerAffiliation = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                    maxNumberOfSteering,maxNumberOfAffiliations));
   steerUsed = static_cast<Bool_t**>(AllocateBool(maxNumberOfTasks+maxNumberOfTabs + 1,maxNumberOfSteering));
   steerFieldName = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                  maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldType = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                  maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldFormat = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                    maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldArraySize = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                       maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldInit = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                  maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldComment = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                     maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldShortDescription = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                              maxNumberOfSteering,
                                                                              maxNumberOfSteeringField));
   steerFieldCLOption = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                      maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldCLDescription = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                           maxNumberOfSteering,
                                                                           maxNumberOfSteeringField));
   numOfSteerFieldAffiliations = static_cast<Int_t***>(AllocateInt(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                   maxNumberOfSteering,maxNumberOfSteeringField));
   steerFieldAffiliation = static_cast<ROMEString****>(AllocateROMEString(maxNumberOfTasks+maxNumberOfTabs + 1,
                                                                          maxNumberOfSteering,maxNumberOfSteeringField,
                                                                          maxNumberOfAffiliations));
   steerFieldUsed = static_cast<Bool_t***>(AllocateBool(maxNumberOfTasks+maxNumberOfTabs + 1,maxNumberOfSteering,
                                                        maxNumberOfSteeringField));
   steerFieldHotLink = static_cast<Bool_t***>(AllocateBool(maxNumberOfTasks+maxNumberOfTabs + 1,maxNumberOfSteering,
                                                           maxNumberOfSteeringField));
   gspInclude = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfInclude));
   gspLocalFlag = static_cast<Bool_t*>(AllocateBool(maxNumberOfInclude));

   // tab
   tabName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabSuffix = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabTitle = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabObjectDisplay = static_cast<Bool_t*>(AllocateBool(maxNumberOfTabs));
   numOfTabAffiliations = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   tabAffiliation = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,maxNumberOfAffiliations));
   tabUsed = static_cast<Bool_t*>(AllocateBool(maxNumberOfTabs));
   tabDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabShortDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabUsage = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabStatus = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabToDo = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabKnownProblems = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabAuthor = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabAuthorInstitute = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabAuthorCollaboration = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabAuthorEmail = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabVersion = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabDependence = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabHeredity = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTabs));
   tabHeredityIndex = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   tabParentIndex = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   tabNumOfChildren = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   numOfMenu = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   numOfMenuItem = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfMenus));
   menuTitle = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,maxNumberOfMenus));
   menuDepth = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfMenus));
   menuItemChildMenuIndex = static_cast<Int_t***>(AllocateInt(maxNumberOfTabs,maxNumberOfMenus,maxNumberOfMenuItems));
   menuItemEnumName = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTabs,maxNumberOfMenus,
                                                                    maxNumberOfMenuItems));
   menuItemTitle = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTabs,maxNumberOfMenus,
                                                                 maxNumberOfMenuItems));
   numOfTabSingleObjects = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   tabSingleObjectName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectIndex = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectArrayIndexStart = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectArrayIndexEnd = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectTaskHierarchyIndex = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectTaskIndex = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectObjectIndex = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectType = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,maxNumberOfTabSingleObjects));
   tabSingleObjectIndexMax = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   numOfTabObjectDisplays = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   tabObjectDisplayName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,maxNumberOfTabObjectDisplays));
   tabObjectDisplayTitle = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,maxNumberOfTabObjectDisplays));
   numOfTabObjectDisplayObjects = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs
                                                                   ,maxNumberOfTabObjectDisplays));
   tabObjectDisplayObject = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTabs
                                                                         ,maxNumberOfTabObjectDisplays
                                                                         ,maxNumberOfTabObjectDisplayObjects));
   tabObjectDisplayType = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTabs
                                                                         ,maxNumberOfTabObjectDisplays
                                                                         ,maxNumberOfTabObjectDisplayObjects));
   tabObjectDisplayTaskHierarchyIndex = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,
                                                                         maxNumberOfTabObjectDisplays));
   tabObjectDisplayTaskIndex = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfTabObjectDisplays));
   tabObjectDisplayObjectIndex = static_cast<Int_t***>(AllocateInt(maxNumberOfTabs,maxNumberOfTabObjectDisplays,maxNumberOfTabObjectDisplayObjects));
   tabObjectDisplayTaskHierarchyNumber = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,
                                                                          maxNumberOfTabObjectDisplays));
   tabObjectDisplayObjectTypeIndex = static_cast<Int_t***>(AllocateInt(maxNumberOfTabs,maxNumberOfTabObjectDisplays,maxNumberOfTabObjectDisplayObjects));
   numOfTabObjectDisplayObjectTypes = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   tabObjectDisplayObjectType = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,
                                                                             maxNumberOfTabObjectDisplayObjectTypes));

   // tree
   numOfBranch = static_cast<Int_t*>(AllocateInt(maxNumberOfTrees));
   numOfRunHeader = static_cast<Int_t*>(AllocateInt(maxNumberOfTrees));
   treeName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTrees));
   treeTitle = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTrees));
   treeFileName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTrees));
   treeDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfTrees));
   branchName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTrees,maxNumberOfBranches));
   branchFolder = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTrees,maxNumberOfBranches));
   branchBufferSize = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTrees,maxNumberOfBranches));
   branchSplitLevel = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTrees,maxNumberOfBranches));
   branchSplitLevelSpecified = static_cast<Bool_t**>(AllocateBool(maxNumberOfTrees,maxNumberOfBranches));
   runHeaderName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTrees,maxNumberOfRunHeaders));
   runHeaderFolder = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTrees,maxNumberOfRunHeaders));
   runHeaderFolderIndex = static_cast<Int_t**>(AllocateInt(maxNumberOfTrees,maxNumberOfRunHeaders));

   // thread functions
   numOfThreadFunctions = static_cast<Int_t*>(AllocateInt(maxNumberOfTabs));
   numOfThreadFunctionArguments = static_cast<Int_t**>(AllocateInt(maxNumberOfTabs,maxNumberOfThreadFunctions));
   threadFunctionName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfTabs,maxNumberOfThreadFunctions));
   threadFunctionArgument = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfTabs,maxNumberOfThreadFunctions,
                                                                          maxNumberOfThreadFunctionArguments));

   // daq
   daqName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfDAQ));
   numOfDAQAffiliations = static_cast<Int_t*>(AllocateInt(maxNumberOfDAQ));
   daqAffiliation = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfDAQ,maxNumberOfAffiliations));
   daqUsed = static_cast<Bool_t*>(AllocateBool(maxNumberOfDAQ));

   // database
   dbName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfDB));
   dbDescription = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfDB));

   // midas
   numOfBank = static_cast<Int_t*>(AllocateInt(maxNumberOfEvents));
   numOfStructFields = static_cast<Int_t**>(AllocateInt(maxNumberOfEvents,maxNumberOfBanks));
   eventName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfEvents));
   eventID = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfEvents));
   eventTriggerMask = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfEvents));
   eventSamplingRate = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfEvents));
   bankName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfEvents,maxNumberOfBanks));
   bankType = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfEvents,maxNumberOfBanks));
   bankArraySize = static_cast<Int_t**>(AllocateInt(maxNumberOfEvents,maxNumberOfBanks));
   bankArrayStart = static_cast<Int_t**>(AllocateInt(maxNumberOfEvents,maxNumberOfBanks));
   bankArrayDigit = static_cast<Int_t**>(AllocateInt(maxNumberOfEvents,maxNumberOfBanks));
   structFieldName = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfEvents,maxNumberOfBanks,
                                                                   maxNumberOfStructFields));
   structFieldType = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfEvents,maxNumberOfBanks,
                                                                   maxNumberOfStructFields));
   structFieldSize = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfEvents,maxNumberOfBanks,
                                                                   maxNumberOfStructFields));
   bankFieldArraySize = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfEvents,maxNumberOfBanks,
                                                                      maxNumberOfStructFields));

   // root DAQ
   numOfRootBranch = static_cast<Int_t*>(AllocateInt(maxNumberOfRootTrees));
   numOfRootBranchField = static_cast<Int_t**>(AllocateInt(maxNumberOfRootTrees,maxNumberOfRootBranches));
   rootTreeName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfRootTrees));
   rootBranchName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfRootTrees,maxNumberOfRootBranches));
   rootBranchType = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfRootTrees,maxNumberOfRootBranches));
   rootBranchArraySize = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfRootTrees,maxNumberOfRootBranches));
   rootBranchClassName = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfRootTrees,maxNumberOfRootBranches));
   rootBranchClassVersion = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfRootTrees,
                                                                         maxNumberOfRootBranches));
   rootBranchFieldName = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfRootTrees,maxNumberOfRootBranches,
                                                                       maxNumberOfRootBranchFields));
   rootBranchFieldType = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfRootTrees,maxNumberOfRootBranches,
                                                                       maxNumberOfRootBranchFields));
   rootBranchFieldArraySize = static_cast<ROMEString***>(AllocateROMEString(maxNumberOfRootTrees,
                                                                            maxNumberOfRootBranches,
                                                                            maxNumberOfRootBranchFields));

   // user makefile
   mfDictHeaderName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFDictHeaders));
   mfDictHeaderLinkDefSuffix = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFDictHeaders));
   numOfMFDictHeaderAffiliations = static_cast<Int_t*>(AllocateInt(maxNumberOfMFDictHeaders));
   mfDictHeaderAffiliation = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfMFDictHeaders,
                                                                          maxNumberOfAffiliations));
   mfDictHeaderUsed = static_cast<Bool_t*>(AllocateBool(maxNumberOfMFDictHeaders));

   mfDictIncDir = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFDictIncDirs));

   mfWinLibName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFWinLibs));
   numOfMFWinLibFlags = static_cast<Int_t*>(AllocateInt(maxNumberOfMFWinLibs));
   mfWinLibFlag = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfMFWinLibs,maxNumberOfMFWinLibFlags));

   mfUnixLibName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFUnixLibs));
   numOfMFUnixLibFlags = static_cast<Int_t*>(AllocateInt(maxNumberOfMFUnixLibs));
   mfUnixLibFlag = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfMFUnixLibs,maxNumberOfMFUnixLibFlags));

   mfIncDir = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFIncDirs));

   mfPreDefName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFPreDefs));

   mfSourceFileName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFSources));
   mfSourceFilePath = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFSources));
   mfSourceFileObjPath = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFSources));
   mfHeaderFileName = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFSources));
   mfHeaderFilePath = static_cast<ROMEString*>(AllocateROMEString(maxNumberOfMFSources));
   numOfMFSourceFlags = static_cast<Int_t*>(AllocateInt(maxNumberOfMFSources));
   mfSourceFileFlag = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfMFSources,maxNumberOfMFSourceFlags));
   numOfMFSourceAffiliations = static_cast<Int_t*>(AllocateInt(maxNumberOfMFSources));
   mfSourceFileAffiliation = static_cast<ROMEString**>(AllocateROMEString(maxNumberOfMFSources,
                                                                          maxNumberOfAffiliations));
   mfSourceFileUsed = static_cast<Bool_t*>(AllocateBool(maxNumberOfMFSources));

   // precompiled headers;
   precompiledHeaders = new ROMEStrArray(1);
   precompiledIncludeHeaders = new ROMEStrArray(3);

   delete xml;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLDefinitionFile()
{
   int i,j;
   const char* name;
   bool finished = false;
   bool inputok = false;
   int type;
   ROMEString tmp;

   experimentName = "";
   styleSheet = "";
   baseURL = "htmldoc/";
   shortCut = "";
   mainProgName = "";
   mainDescription = "";
   mainAuthor = "";
   mainInstitute = "";
   mainCollaboration = "";
   mainDefinitionVersion = "1";
   dictionaryType = 1;
   mainEmail = "";
   usedCLO = ROMECommandLineOptions;

   readExperiment = false;
   readGlobalSteeringParameters = false;
   bankHasHeader = false;

   numOfFolder = -1;
   numOfTask = -1;
   numOfTaskHierarchy = -1;
   numOfTree = -1;
   numOfEvent = -1;
   numOfRootTree = -1;
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
      if (type == 1 && !strcmp(name,"ROMEFrameworkDefinition")) {
         inputok = true;
         while (xml->NextLine()&&!finished) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 15 && !strcmp(name,"ROMEFrameworkDefinition")) {
               if (!readExperiment) {
                  cout<<"Experiment tree missing!"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (numOfFolder < 0) numOfFolder = 0;
               if (numOfTask < 0) numOfTask = 0;
               if (numOfTaskHierarchy < 0) numOfTaskHierarchy = 0;
               if (numOfTree < 0) numOfTree = 0;
               if (numOfEvent < 0) numOfEvent = 0;
               if (numOfDAQ < 0) numOfDAQ = 0;
               if (numOfDB < 0) numOfDB = 0;
               if (numOfIncludeDirectories < 0) numOfIncludeDirectories = 0;
               if (numOfTab < 0) numOfTab = 0;
               if (numOfNetFolder <0) numOfNetFolder = 0;
               if (numOfMFDictHeaders < 0) numOfMFDictHeaders = 0;
               if (numOfMFDictIncDirs < 0) numOfMFDictIncDirs = 0;
               if (numOfMFWinLibs < 0) numOfMFWinLibs = 0;
               if (numOfMFUnixLibs < 0) numOfMFUnixLibs = 0;
               if (numOfMFIncDirs < 0) numOfMFIncDirs = 0;
               if (numOfMFPreDefs < 0) numOfMFPreDefs = 0;
               if (numOfMFSources < 0) numOfMFSources = 0;
               finished = true;
               break;
            }
            if (type == 1) {
               if (!strcmp(name,"Experiment")) {
                  readExperiment = true;
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp(name,"ExperimentName")) {
                        xml->GetValue(experimentName,experimentName);
                        FormatText(experimentName, kTRUE);
                     }
                     if (type == 1 && !strcmp(name,"ExperimentShortCut")) {
                        xml->GetValue(shortCut,shortCut);
                        FormatText(shortCut, kTRUE, " ");
                     }
                     if (type == 1 && !strcmp(name,"ProgramName")) {
                        xml->GetValue(mainProgName,mainProgName);
                        FormatText(mainProgName, kTRUE, " ");
                     }
                     if (type == 1 && !strcmp(name,"DictionaryType")) {
                        xml->GetValue(tmp,tmp);
                        if (tmp.ToInteger() == 0) {
                           dictionaryType = 0;
                        }
                     }
                     if (type == 1 && !strcmp(name,"ProgramDefinitionVersion")) {
                        xml->GetValue(mainDefinitionVersion,mainDefinitionVersion);
                        FormatText(mainDefinitionVersion, kTRUE);
                        ParseSVNKeyword(mainDefinitionVersion);
                     }
                     if (type == 1 && !strcmp(name,"FrameworkDescription")) {
                        xml->GetValue(mainDescription,mainDescription);
                        FormatText(mainDescription, kFALSE);
                     }
                     if (type == 1 && !strcmp(name,"DocumentStyleSheet")) {
                        xml->GetValue(styleSheet,styleSheet);
                        FormatText(styleSheet, kTRUE);
                     }
                     if (type == 1 && !strcmp(name,"DocumentBaseURL")) {
                        xml->GetValue(baseURL,baseURL);
                        FormatText(baseURL, kTRUE);
                     }
                     if (type == 15 && !strcmp(name,"Experiment")) {
                        if (shortCut == "") {
                           cout<<"Experiment must have a shortcut!"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                        break;
                     }
                  }
                  continue;
               }
               if (!strcmp(name,"Author")) {
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp(name,"AuthorName")) {
                        xml->GetValue(mainAuthor,mainAuthor);
                        FormatText(mainAuthor, kTRUE);
                     }
                     if (type == 1 && !strcmp(name,"AuthorInstitute")) {
                        xml->GetValue(mainInstitute,mainInstitute);
                        FormatText(mainInstitute, kTRUE);
                     }
                     if (type == 1 && !strcmp(name,"AuthorCollaboration")) {
                        xml->GetValue(mainCollaboration,mainCollaboration);
                        FormatText(mainCollaboration, kTRUE);
                     }
                     if (type == 1 && !strcmp(name,"AuthorEmail")) {
                        xml->GetValue(mainEmail,mainEmail);
                        FormatText(mainEmail, kTRUE);
                     }
                     if (type == 15 && !strcmp(name,"Author"))
                        break;
                  }
                  continue;
               }
               if (!strcmp(name,"Folders")) {
                  // initialization
                  numOfFolder = -1;
                  hasFolderUserCode = false;
                  hasFolderGenerated = false;
                  hasSupportFolderGenerated = false;
                  parent[0] = "GetMainFolder()";
                  // output
                  if (makeOutput) cout<<"Folders:"<<endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // folder
                     if (type == 1 && !strcmp(name,"Folder")) {
                        recursiveDepth = 0;
                        folderSupport[numOfFolder + 1] = false;
                        if (!ReadXMLFolder()) return false;
                     }
                     // folders end
                     if (type == 15 && !strcmp(name,"Folders")) {
                        break;
                     }
                  }
                  // check input
                  for (i = 0; i < numOfFolder; i++) {
                     for (j = i + 1; j < numOfFolder; j++) {
                        if (folderName[i] == folderName[j]) {
                           cout<<"\nFolder '"<<folderName[i].Data()<<"' is defined twice !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                     }
                  }
                  // count folders
                  numOfFolder++;
                  // check if a folder has class fields
                  for (i = 0; i < numOfFolder; i++) {
                     for (j = 0; j < numOfValue[i]; j++) {
                        if (valueIsTObject[i][j] || isFolder(valueName[i][j])) {
                           tmp = valueComment[i][j];
                           tmp.StripSpaces();
                           if (!tmp.BeginsWith("!")) {
                              folderHasClassField[i] = kTRUE;
                              break;
                           }
                        }
                     }
                  }
               }
               if (!strcmp(name,"SupportFolders")) {
                  if (numOfFolder == -1)
                     continue;
                  numOfFolder--;
                  // output
                  if (makeOutput) cout<<"SupportFolders:"<<endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // folder
                     if (type == 1 && !strcmp(name,"SupportFolder")) {
                        recursiveDepth = 0;
                        folderSupport[numOfFolder + 1] = true;
                        if (!ReadXMLFolder()) return false;
                     }
                     // folders end
                     if (type == 15 && !strcmp(name,"SupportFolders")) {
                        break;
                     }
                  }
                  // check input
                  for (i = 0; i < numOfFolder; i++) {
                     for (j = i + 1; j < numOfFolder; j++) {
                        if (folderName[i] == folderName[j]) {
                           cout<<"\nFolder '"<<folderName[i].Data()<<"' is defined twice !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                     }
                  }
                  // count folders
                  numOfFolder++;
                  // check if a folder has class fields
                  for (i = 0; i < numOfFolder; i++) {
                     for (j = 0; j < numOfValue[i]; j++) {
                        if (valueIsTObject[i][j] || isFolder(valueName[i][j])) {
                           tmp = valueComment[i][j];
                           tmp.StripSpaces();
                           if (!tmp.BeginsWith("!")) {
                              folderHasClassField[i] = kTRUE;
                              break;
                           }
                        }
                     }
                  }
               }
               if (!strcmp(name,"Tasks")) {
                  // initialization
                  numOfTask = -1;
                  hasTaskGenerated = false;
                  parent[0] = "GetMainTask()";
                  // output
                  if (makeOutput) cout<<"\n\nTasks:"<<endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // task
                     if (type == 1 && !strcmp(name,"Task")) {
                        recursiveDepth = 0;
                        if (!ReadXMLTask()) return false;
                     }
                     // tasks end
                     if (type == 15 && !strcmp(name,"Tasks")) {
                        break;
                     }
                  }
                  // count tasks
                  numOfTask++;
                  // check input
                  for (i = 0; i < numOfTask; i++) {
                     for (j = i + 1; j < numOfTask; j++) {
                        if (taskName[i] == taskName[j]) {
                           cout<<"\nTask '"<<taskName[i].Data()<<"' is defined twice !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                     }
                  }
                  // default task hierarchy
                  for (i = 0; i < numOfTask; i++) {
                     taskHierarchyName[i] = taskName[i];
                     taskHierarchyParentIndex[i] = -1;
                     taskHierarchyClassIndex[i] = i;
                     taskHierarchyMultiplicity[i] = 1;
                     taskHierarchyLevel[i] = 1;
                  }
                  numOfTaskHierarchy = numOfTask;
               }
               if (!strcmp(name,"TaskHierarchy")) {
                  int depth = 0;
                  int *parentIndex = new int[2*maxNumberOfTasks];
                  parentIndex[0] = -1;
                  numOfTaskHierarchy = -1;
                  while (xml->NextLine()&&!finished) {
                     type = xml->GetType();
                     name = xml->GetName();
                     if (type == 1 && !strcmp(name,"TaskName")) {
                        xml->GetValue(taskHierarchyName[numOfTaskHierarchy],taskHierarchyName[numOfTaskHierarchy]);
                        FormatText(taskHierarchyName[numOfTaskHierarchy], kTRUE);
                        taskHierarchyClassIndex[numOfTaskHierarchy] = -1;
                        for (i = 0; i < numOfTask; i++) {
                           if (taskName[i] == taskHierarchyName[numOfTaskHierarchy])
                              taskHierarchyClassIndex[numOfTaskHierarchy] = i;
                        }
                        if (taskHierarchyClassIndex[numOfTaskHierarchy] == -1) {
                           cout<<"The task '"<<taskHierarchyName[numOfTaskHierarchy].Data()
                               <<"' used in the task hierarchy is not defined !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                        taskHierarchyMultiplicity[numOfTaskHierarchy] = 1;
                        for (i = 0; i < numOfTaskHierarchy; i++) {
                           if (taskHierarchyName[i] == taskHierarchyName[numOfTaskHierarchy] &&
                               taskHierarchyParentIndex[i] == taskHierarchyParentIndex[numOfTaskHierarchy])
                              taskHierarchyMultiplicity[numOfTaskHierarchy]++;
                        }
                        if (!taskUsed[taskHierarchyClassIndex[numOfTaskHierarchy]])
                           numOfTaskHierarchy--;
                     }
                     if (type == 1 && !strcmp(name,"Task")) {
                        depth++;
                        numOfTaskHierarchy++;
                        parentIndex[depth] = numOfTaskHierarchy;
                        taskHierarchyName[numOfTaskHierarchy] = "";
                        taskHierarchyParentIndex[numOfTaskHierarchy] = parentIndex[depth-1];
                        taskHierarchyLevel[numOfTaskHierarchy] = depth;
                     }
                     if (type == 15 && !strcmp(name,"Task")) {
                        depth--;
                     }
                     if (type == 15 && !strcmp(name,"TaskHierarchy"))
                        break;
                  }
                  numOfTaskHierarchy++;
                  delete [] parentIndex;
                  continue;
               }
               if (!strcmp(name,"Tabs")) {
                  if (numOfTask == -1)
                     numOfTask++;
                  // initialization
                  numOfTab = -1;
                  // output
                  if (makeOutput)
                     cout<<"\n\nTabs:"<<endl;
                  while (xml->NextLine()) {
                     type = xml->GetType();
                     name = xml->GetName();
                     // tab
                     if (type == 1 && !strcmp(name, "Tab")) {
                        tabParentIndex[numOfTab + 1] = -1;
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
                           cout<<"\nTab '"<<tabName[i].Data()<<"' is defined twice !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                        if (tabName[i] == tabHeredity[j]) {
                           tabHeredityIndex[j] = i;
                        }
                     }
                  }
               }

               if (!strcmp(name,"NetFolders")) {
                  // initialization
                  numOfNetFolder = -1;
                  // output
                  if (makeOutput)
                     cout<<"\n\nNetFolders:"<<endl;
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
                           cout<<"\nNet folder '"<<netFolderName[i].Data()<<"' is defined twice !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                     }
                  }
                  // count net object
                  numOfNetFolder++;
               }

               if (!strcmp(name,"Trees")) {
                  numOfTree = -1;
                  if (!ReadXMLTree()) return false;
               }
               if (!strcmp(name,"UserDAQSystems")) {
                  numOfDAQ = -1;
                  if (!ReadXMLDAQ()) return false;
               }
               if (!strcmp(name,"UserDatabases")) {
                  numOfDB = -1;
                  if (!ReadXMLDB()) return false;
               }
               if (!strcmp(name,"MidasBanks")) {
                  numOfEvent = -1;
                  if (!ReadXMLMidasBanks()) return false;
               }
               if (!strcmp(name,"RootDAQ")) {
                  numOfRootTree = -1;
                  if (!ReadXMLRootDAQ()) return false;
               }
               if (!strcmp(name,"GlobalSteeringParameters")) {
                  if (numOfTask == -1)
                     numOfTask++;
                  readGlobalSteeringParameters = true;
                  // output
                  if (makeOutput) cout<<"\n\nGlobal Steering Parameters:"<<endl;
                  // initialisation
                  steerName[numOfTask][0] = "GlobalSteering";
                  actualSteerIndex = 0;
                  recursiveSteerDepth = 0;
                  steerParent[numOfTask][0] = -1;
                  numOfSteering[numOfTask] = -1;
                  if (!ReadXMLSteering(numOfTask,true)) return false;
                  numOfSteering[numOfTask]++;
               }
               if (!strcmp(name,"UserMakeFile")) {
                  if (!ReadXMLUserMakefile()) return false;
               }
            }
         }
      }
   }
   if (!inputok) {
      cout<<"\nCould not find the <ROMEFrameworkDefinition> tag in the inputfile '"<< xmlFile.Data()<<"'."<<endl;
      cout<<"Terminating the ROMEBuilder."<<endl;
      return false;
   }
   delete xml;
   return true;
}

//______________________________________________________________________________
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
   if (numOfFolder >= maxNumberOfFolders) {
      cout<<"Maximal number of folders reached : "<<maxNumberOfFolders<<" !"<<endl;
      cout<<"Terminating program."<<endl;
      return false;
   }
   // initialisation
   folderName[numOfFolder] = "";
   numOfFolderAffiliations[numOfFolder] = 0;
   folderUsed[numOfFolder] = true;
   folderTitle[numOfFolder] = "";
   folderArray[numOfFolder] = "1";
   folderDataBase[numOfFolder] = false;
   folderUserCode[numOfFolder] = false;
   folderVersion[numOfFolder] = "1";
   folderDescription[numOfFolder] = "";
   folderShortDescription[numOfFolder] = "";
   folderInheritName[numOfFolder] = "";
   folderAuthor[numOfFolder] = mainAuthor;
   numOfFolderInclude[numOfFolder] = 0;
   numOfValue[numOfFolder] = 0;
   folderNoReset[numOfFolder] = false;
   folderNoResetModified[numOfFolder] = false;
   folderHasClassField[numOfFolder] = false;

   // set parent
   folderParentName[numOfFolder] = parent[recursiveDepth];
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subfolder
      if (type == 1 && !strcmp(name,"Folder")) {
         // set folder as parent for subsequent folders
         recursiveDepth++;
         folderSupport[numOfFolder + 1] = false;
         if (parent[recursiveDepth].Length() == 0)
            parent[recursiveDepth] = folderName[numOfFolder].Data();
         // read subfolder
         if (!ReadXMLFolder())
            return false;
         continue;
      }
      // end folder
      if (type == 15 && (!strcmp(name,"Folder") || !strcmp(name,"SupportFolder"))) {
         // check input
         if (currentFolderName == "") {
            cout<<"The "<<(currentNumberOfFolders + 1)<<". Folder has no name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (affiliations.GetEntriesFast() > 0) {
            folderUsed[currentNumberOfFolders] = false;
            for (i = 0; i < affiliations.GetEntriesFast() && !folderUsed[currentNumberOfFolders]; i++) {
               for (j = 0; j < numOfFolderAffiliations[currentNumberOfFolders]; j++) {
                  if (affiliations.At(i) == folderAffiliation[currentNumberOfFolders][j]) {
                     folderUsed[currentNumberOfFolders] = true;
                     break;
                  }
               }
            }
         }
         if (folderUsed[numOfFolder]) {
            if (folderUserCode[numOfFolder]) {
               hasFolderUserCode = true;
            } else {
               if (folderSupport[numOfFolder]) {
                  hasSupportFolderGenerated = true;
               } else {
                  hasFolderGenerated = true;
               }
            }
         }
         // description
         if (folderDescription[currentNumberOfFolders].Length()
             && !folderShortDescription[currentNumberOfFolders].Length()
             && !folderDescription[currentNumberOfFolders].Contains("\n"))
            folderShortDescription[currentNumberOfFolders] = folderDescription[currentNumberOfFolders];
         parent[recursiveDepth + 1] = "";
         recursiveDepth--;
         return true;
      }
      // folder name
      if (type == 1 && !strcmp(name,"FolderName")) {
         xml->GetValue(folderName[numOfFolder],folderName[numOfFolder]);
         FormatText(folderName[numOfFolder], kTRUE);
         currentFolderName = folderName[numOfFolder];
         // output
         if (makeOutput) for (i = 0; i < recursiveDepth; i++) cout<<"   ";
         if (makeOutput) folderName[numOfFolder].WriteLine();
      }
      // folder affiliation
      if (type == 1 && !strcmp(name,"Affiliation")) {
         folderAffiliation[numOfFolder][numOfFolderAffiliations[numOfFolder]] = "";
         xml->GetValue(folderAffiliation[numOfFolder][numOfFolderAffiliations[numOfFolder]]
                       ,folderAffiliation[numOfFolder][numOfFolderAffiliations[numOfFolder]]);
         FormatText(folderAffiliation[numOfFolder][numOfFolderAffiliations[numOfFolder]], kTRUE, " ");
         for (i = 0; i < affiliationList.GetEntries(); i++) {
            if (affiliationList.At(i,0) == folderAffiliation[numOfFolder][numOfFolderAffiliations[numOfFolder]])
               break;
         }
         affiliationList.SetAt(folderAffiliation[numOfFolder][numOfFolderAffiliations[numOfFolder]], i, 0);
         affiliationList.SetAt("Folder", i, affiliationList.GetEntriesAt(i));
         affiliationList.SetAt(folderName[numOfFolder], i, affiliationList.GetEntriesAt(i));
         numOfFolderAffiliations[numOfFolder]++;
      }
      // folder title
      if (type == 1 && !strcmp(name,"FolderTitle")) {
         xml->GetValue(folderTitle[numOfFolder],folderTitle[numOfFolder]);
         FormatText(folderTitle[numOfFolder], kFALSE);
      }
      // folder array size
      if (type == 1 && !strcmp(name,"ArraySize")) {
         xml->GetValue(folderArray[numOfFolder],folderArray[numOfFolder]);
         FormatText(folderArray[numOfFolder], kTRUE);
      }
      // folder data base access
      if (type == 1 && !strcmp(name,"DataBaseAccess")) {
         if (folderSupport[numOfFolder]) {
            cout<<"Support folders can not have database access!!"<<endl;
            continue;
         }
         xml->GetValue(tmp,"false");
         FormatText(tmp, kTRUE);
         if (tmp == "true") {
            folderDataBase[numOfFolder] = true;
            folderNoReset[numOfFolder] = true;
         }
      }
      // folder with changeble class file
      if (type == 1 && !strcmp(name,"ChangeableClassFile")) {
         xml->GetValue(tmp,"false");
         FormatText(tmp, kTRUE);
         if (tmp == "true")
            folderUserCode[numOfFolder] = true;
      }
      // folder version
      if (type == 1 && !strcmp(name,"FolderVersion")) {
         xml->GetValue(folderVersion[numOfFolder],folderVersion[numOfFolder]);
         FormatText(folderVersion[numOfFolder], kTRUE);
         ParseSVNKeyword(folderVersion[numOfFolder]);
      }
      // folder description
      if (type == 1 && !strcmp(name,"FolderDescription")) {
         xml->GetValue(folderDescription[numOfFolder],folderDescription[numOfFolder]);
         FormatText(folderDescription[numOfFolder], kFALSE);
      }
      // folder short description
      if (type == 1 && !strcmp(name,"FolderShortDescription")) {
         xml->GetValue(folderShortDescription[numOfFolder],folderShortDescription[numOfFolder]);
         FormatText(folderShortDescription[numOfFolder], kTRUE);
      }
      // folder inherit name
      if (type == 1 && !strcmp(name,"Inherit")) {
         xml->GetValue(folderInheritName[numOfFolder],folderInheritName[numOfFolder]);
         FormatText(folderInheritName[numOfFolder], kTRUE);
      }
      // folder author
      if (type == 1 && !strcmp(name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // author name
            if (type == 1 && !strcmp(name,"AuthorName")) {
               xml->GetValue(folderAuthor[numOfFolder],folderAuthor[numOfFolder]);
               FormatText(folderAuthor[numOfFolder], kTRUE);
            }
            if (type == 15 && !strcmp(name,"Author"))
               break;
         }
         continue;
      }
      // folder include
      if (type == 1 && !strcmp(name,"Include")) {
         // include initialisation
         folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]] = "";
         folderLocalFlag[numOfFolder][numOfFolderInclude[numOfFolder]] = false;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include name
            if (type == 1 && !strcmp(name,"IncludeName")) {
               xml->GetValue(folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]]
                             ,folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]]);
               FormatText(folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]], kTRUE);
            }
            // include type
            if (type == 1 && !strcmp(name,"IncludeType")) {
               xml->GetValue(tmp,"false");
               FormatText(tmp, kTRUE);
               if (tmp == "local")
                  folderLocalFlag[numOfFolder][numOfFolderInclude[numOfFolder]] = true;
            }
            // include end
            if (type == 15 && !strcmp(name,"Include"))
               break;
         }
         // check input
         if (folderInclude[numOfFolder][numOfFolderInclude[numOfFolder]] == "") {
            cout<<"An Include of Folder '"<<folderName[numOfFolder].Data()<<"' has no Name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // count includes
         numOfFolderInclude[numOfFolder]++;
         if (numOfFolderInclude[numOfFolder] >= maxNumberOfInclude) {
            cout<<"Maximal number of includes in folder '"<<folderName[numOfFolder].Data()<<"' reached : "
                <<maxNumberOfInclude<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         continue;
      }
      // folder no reset by framework
      if (type == 1 && !strcmp(name,"NoResetByFramework")) {
         xml->GetValue(tmp,"false");
         FormatText(tmp, kTRUE);
         if (tmp == "true")
            folderNoReset[numOfFolder] = true;
      }
      // folder no reset modified by framework
      if (type == 1 && !strcmp(name,"NoResetModifiedByFramework")) {
         xml->GetValue(tmp,"false");
         FormatText(tmp, kTRUE);
         if (tmp == "true")
            folderNoResetModified[numOfFolder] = true;
      }
      // folder field
      if (type == 1 && !strcmp(name,"Field")) {
         // field initialisation
         bool readName = false;
         bool readType = false;
         valueName[numOfFolder][numOfValue[numOfFolder]] = "";
         valueType[numOfFolder][numOfValue[numOfFolder]] = "";
         valueComment[numOfFolder][numOfValue[numOfFolder]] = "";
         valueDimension[numOfFolder][numOfValue[numOfFolder]] = 0;
         valueNoBoundChech[numOfFolder][numOfValue[numOfFolder]] = false;
         valueIsTObject[numOfFolder][numOfValue[numOfFolder]] = false;
         for (iDm = 0; iDm < maxNumberOfValueDimension; iDm++)
            valueArray[numOfFolder][numOfValue[numOfFolder]][iDm] = "1";
         valueArraySpecifier[numOfFolder][numOfValue[numOfFolder]] = "";
         valueDBName[numOfFolder][numOfValue[numOfFolder]] = "";
         valueDBPath[numOfFolder][numOfValue[numOfFolder]] = "";
         valueDBIf[numOfFolder][numOfValue[numOfFolder]] = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // field name
            if (type == 1 && !strcmp(name,"FieldName")) {
               readName = true;
               xml->GetValue(valueName[numOfFolder][numOfValue[numOfFolder]],
                             valueName[numOfFolder][numOfValue[numOfFolder]]);
               FormatText(valueName[numOfFolder][numOfValue[numOfFolder]], kTRUE, " ");
            }
            // field type
            if (type == 1 && !strcmp(name,"FieldType")) {
               readType = true;
               xml->GetValue(valueType[numOfFolder][numOfValue[numOfFolder]],
                             valueType[numOfFolder][numOfValue[numOfFolder]]);
               FormatText(valueType[numOfFolder][numOfValue[numOfFolder]], kTRUE);
#if 0 // better to use "InheritTObject" flag
               if (valueType[numOfFolder][numOfValue[numOfFolder]].BeginsWith("T")) {
                  valueIsTObject[numOfFolder][numOfValue[numOfFolder]] = true;
               }
#endif
               // set initial value
               if (valueType[numOfFolder][numOfValue[numOfFolder]] == "TString" ||
                   valueType[numOfFolder][numOfValue[numOfFolder]] == "ROMEString") {
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "' '";
               } else if (valueType[numOfFolder][numOfValue[numOfFolder]] == "TRef") {
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "NULL";
               } else if (isTArrayType(valueType[numOfFolder][numOfValue[numOfFolder]])) {
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "0";
                  valueDimension[numOfFolder][numOfValue[numOfFolder]] = 1;
                  valueArray[numOfFolder][numOfValue[numOfFolder]][0] = "0";
               } else if (isPointerType(valueType[numOfFolder][numOfValue[numOfFolder]])) {
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "NULL";
               } else {
                  valueInit[numOfFolder][numOfValue[numOfFolder]] = "0";
               }
            }
            // field initialization
            if (type == 1 && !strcmp(name,"FieldInitialization")) {
               if (!readName) {
                  cout<<"Please specify a field name befor the initial value in the "<<(numOfValue[numOfFolder] + 1)
                      <<".field in folder '"<<folderName[numOfFolder].Data()<<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (!readType) {
                  cout<<"Please specify a field type befor the initial value in field '"
                      <<valueName[numOfFolder][numOfValue[numOfFolder]].Data()<<"' in folder  '"
                      <<folderName[numOfFolder].Data()<<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               xml->GetValue(valueInit[numOfFolder][numOfValue[numOfFolder]],
                             valueInit[numOfFolder][numOfValue[numOfFolder]]);
               FormatText(valueInit[numOfFolder][numOfValue[numOfFolder]], kTRUE);
            }
            // field comment
            if (type == 1 && !strcmp(name,"FieldComment")) {
               xml->GetValue(valueComment[numOfFolder][numOfValue[numOfFolder]],
                             valueComment[numOfFolder][numOfValue[numOfFolder]]);
               FormatText(valueComment[numOfFolder][numOfValue[numOfFolder]], kFALSE);
            }
            // field array size
            if (type == 1 && !strcmp(name,"ArraySize")) {
               tmp = "1";
               xml->GetValue(tmp,tmp);
               FormatText(tmp, kTRUE);
               istart = iend = 0;
               valueDimension[numOfFolder][numOfValue[numOfFolder]]++;
               while ((iend=tmp.Index(":",1,istart,TString::kExact)) != -1) {
                  valueDimension[numOfFolder][numOfValue[numOfFolder]]++;
                  if (valueDimension[numOfFolder][numOfValue[numOfFolder]] > maxNumberOfValueDimension) {
                     cout<<"Error: dimension of "<<valueName[numOfFolder][numOfValue[numOfFolder]]
                         <<" is larger than "<<maxNumberOfValueDimension<<" in "<<folderName[numOfFolder]<<"."<<endl;
                     cout<<"Terminating program."<<endl;
                     return false;

                  }
                  valueArray[numOfFolder][numOfValue[numOfFolder]][valueDimension[numOfFolder][numOfValue[numOfFolder]]-2] =
                        tmp(istart,iend-istart);
                  istart = iend + 1;
               }
               valueArray[numOfFolder][numOfValue[numOfFolder]][valueDimension[numOfFolder][numOfValue[numOfFolder]]-1] = tmp(istart,tmp.Length()-istart);
            }
            // database name
            if (type == 1 && !strcmp(name,"DataBaseName")) {
               xml->GetValue(valueDBName[numOfFolder][numOfValue[numOfFolder]],
                             valueDBName[numOfFolder][numOfValue[numOfFolder]]);
               FormatText(valueDBName[numOfFolder][numOfValue[numOfFolder]], kTRUE);
            }
            // database path
            if (type == 1 && !strcmp(name,"DataBasePath")) {
               xml->GetValue(valueDBPath[numOfFolder][numOfValue[numOfFolder]],
                             valueDBPath[numOfFolder][numOfValue[numOfFolder]]);
               FormatText(valueDBPath[numOfFolder][numOfValue[numOfFolder]], kTRUE);
            }
            // database if
            if (type == 1 && !strcmp(name,"DataBaseIfStatement")) {
               xml->GetValue(valueDBIf[numOfFolder][numOfValue[numOfFolder]],
                             valueDBIf[numOfFolder][numOfValue[numOfFolder]]);
               FormatText(valueDBIf[numOfFolder][numOfValue[numOfFolder]], kTRUE);
            }
            // no boundcheck
            if (type == 1 && !strcmp(name,"NoBoundCheck")) {
               xml->GetValue(tmp,"false");
               FormatText(tmp, kTRUE);
               if (tmp == "true")
                  valueNoBoundChech[numOfFolder][numOfValue[numOfFolder]] = true;
            }
            // ROOT object
            if (type == 1 && !strcmp(name,"InheritTObject")) {
               xml->GetValue(tmp,"false");
               FormatText(tmp, kTRUE);
               if (tmp == "true") {
                  valueIsTObject[numOfFolder][numOfValue[numOfFolder]] = true;
               } else {
                  valueIsTObject[numOfFolder][numOfValue[numOfFolder]] = false;
               }
            }
            // field end
            if (type == 15 && !strcmp(name,"Field"))
               break;
         }
         // check input
         if (valueName[numOfFolder][numOfValue[numOfFolder]] == "") {
            cout<<"A Field of Folder '"<<folderName[numOfFolder].Data()<<"' has no Name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (valueType[numOfFolder][numOfValue[numOfFolder]] == "") {
            cout<<"Field '"<<valueName[numOfFolder][numOfValue[numOfFolder]].Data()<<"' of Folder '"
                <<folderName[numOfFolder].Data()<<"' has no Type !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (valueDimension[numOfFolder][numOfValue[numOfFolder]]>1 &&
             (valueDBName[numOfFolder][numOfValue[numOfFolder]].Length()
              || valueDBPath[numOfFolder][numOfValue[numOfFolder]].Length())
            ) {
            cout<<"Multiple dimension field '"<<valueName[numOfFolder][numOfValue[numOfFolder]]
                <<"' can not have database connection"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (valueDimension[numOfFolder][numOfValue[numOfFolder]]>1) {
            for (iDm = 1; iDm < 3; iDm++) {
               if (valueArray[numOfFolder][numOfValue[numOfFolder]][iDm] == "variable") {
                  cout<<"Multiple dimension field '"<<valueName[numOfFolder][numOfValue[numOfFolder]]
                      <<"' can not have variable length."<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
         }
         if (valueArray[numOfFolder][numOfValue[numOfFolder]][0] == "variable"
             && ( valueDBName[numOfFolder][numOfValue[numOfFolder]].Length()
                  || valueDBPath[numOfFolder][numOfValue[numOfFolder]].Length())) {
            cout<<"Variable length array field '"<<valueName[numOfFolder][numOfValue[numOfFolder]]
                <<"' can not have database connection"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         tmp = valueComment[numOfFolder][numOfValue[numOfFolder]];
         tmp.ReplaceAll("//","");
         tmp.ReplaceAll(" ","");
         tmp.ReplaceAll("\t","");
         if ((isNumber(valueType[numOfFolder][numOfValue[numOfFolder]].Data()) ||
              isBoolType(valueType[numOfFolder][numOfValue[numOfFolder]].Data()) ||
              valueType[numOfFolder][numOfValue[numOfFolder]] == "TString") &&
             valueArray[numOfFolder][numOfValue[numOfFolder]][0] == "variable" && tmp[0] != '!') {
            for (i = 0; i < numOfValue[numOfFolder]; i++) {
               tmp.SetFormatted("[%s]",valueName[numOfFolder][i].Data());
               if (valueComment[numOfFolder][numOfValue[numOfFolder]].Contains(tmp) &&
                   isIntType(valueType[numOfFolder][i].Data())) {
                  valueArraySpecifier[numOfFolder][numOfValue[numOfFolder]] = valueName[numOfFolder][i];
                  break;
               }
            }
            if (i == numOfValue[numOfFolder]) {
               cout<<"\nError: Variable length array member '"
                   << valueName[numOfFolder][numOfValue[numOfFolder]].Data()
                   <<"' should have '!' or '[length]' at the top of comment."<<endl;
               cout<<"       If '!', field will not be stored in tree."<<endl;
               cout<<"       If '[length]', field will be stored in tree with the length. 'length' needs to be an integer field name of the folder. It must be defined ahead of its use."
                   <<endl;
               cout<<"Terminating program."<<endl;
               return false;
            }
         }
         if (isTArrayType(valueType[numOfFolder][numOfValue[numOfFolder]])) {
            if (valueDimension[numOfFolder][numOfValue[numOfFolder]]>2) {
               cout<<"Array of TArray is not supported. ( '"<<valueName[numOfFolder][numOfValue[numOfFolder]]
                   <<"' )"<<endl;
               cout<<"Terminating program."<<endl;
               return false;
            }
            if (valueDBName[numOfFolder][numOfValue[numOfFolder]].Length()
               || valueDBPath[numOfFolder][numOfValue[numOfFolder]].Length()) {
               cout<<"TArray type field '"<<valueName[numOfFolder][numOfValue[numOfFolder]]
                   <<"' can not have database connection"<<endl;
               cout<<"Terminating program."<<endl;
               return false;
            }
         }
         for (i = 0; i < numOfValue[numOfFolder]; i++) {
            for (j = i + 1; j < numOfValue[numOfFolder]; j++) {
               if (valueName[numOfFolder][i] == valueName[numOfFolder][j]) {
                  cout<<"\nFolder '"<<folderName[numOfFolder].Data()<<"' has two fields with the name '"
                      <<valueName[numOfFolder][i].Data()<<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
         }
         // count fields
         numOfValue[numOfFolder]++;
         if (numOfValue[numOfFolder] >= maxNumberOfValues) {
            cout<<"Maximal number of fields in folder '"<<folderName[numOfFolder].Data()<<"' reached : "
                <<maxNumberOfValues<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         continue;
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLNetFolder()
{
   // read the net folder definitions out of the xml file
   Int_t type;
   const char *name;
   ROMEString currentNetFolderName = "";

   // count netFolders
   numOfNetFolder++;
   if (numOfNetFolder >= maxNumberOfNetFolders) {
      cout<<"Maximal number of net folders reached : "<<maxNumberOfNetFolders<<" !"<<endl;
      cout<<"Terminating program."<<endl;
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
            cout<<"The "<<(numOfNetFolder + 1)<<". NetFolder has no name !"<<endl;
            cout<<"Terminating program."<<endl;
            return kFALSE;
         }
         return kTRUE;
      }
      // net object name
      if (type == 1 && !strcmp(name, "NetFolderName")) {
         xml->GetValue(netFolderName[numOfNetFolder], netFolderName[numOfNetFolder]);
         FormatText(netFolderName[numOfNetFolder], kTRUE);
         // output
         if (makeOutput)
            netFolderName[numOfNetFolder].WriteLine();
      }
      // net object title
      if (type == 1 && !strcmp(name, "NetFolderTitle")) {
         xml->GetValue(netFolderTitle[numOfNetFolder], netFolderTitle[numOfNetFolder]);
         FormatText(netFolderTitle[numOfNetFolder], kFALSE);
      }

      // net object host
      if (type == 1 && !strcmp(name, "NetFolderHost")) {
         xml->GetValue(netFolderHost[numOfNetFolder], netFolderHost[numOfNetFolder]);
         FormatText(netFolderHost[numOfNetFolder], kTRUE);
      }

      // net object port
      if (type == 1 && !strcmp(name, "NetFolderPort")) {
         xml->GetValue(netFolderPort[numOfNetFolder], netFolderPort[numOfNetFolder]);
         FormatText(netFolderPort[numOfNetFolder], kTRUE);
      }

      // net object Root
      if (type == 1 && !strcmp(name, "NetFolderRoot")) {
         xml->GetValue(netFolderRoot[numOfNetFolder], netFolderRoot[numOfNetFolder]);
         FormatText(netFolderRoot[numOfNetFolder], kTRUE);
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
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
   if (numOfTask >= maxNumberOfTasks) {
      cout<<"Maximal number of tasks reached : "<<maxNumberOfTasks<<" !"<<endl;
      cout<<"Terminating program."<<endl;
      return false;
   }
   // initialisation
   taskName[numOfTask] = "";
   numOfTaskAffiliations[numOfTask] = 0;
   taskUsed[numOfTask] = true;
   taskEventID[numOfTask] = "-1";
   taskAuthor[numOfTask] = mainAuthor;
   taskVersion[numOfTask] = "0";
   taskDependence[numOfTask] = "";
   taskDescription[numOfTask] = "";
   taskShortDescription[numOfTask] = "";
   taskUsage[numOfTask] = "";
   taskStatus[numOfTask] = "";
   taskToDo[numOfTask] = "";
   taskKnownProblems[numOfTask] = "";
   numOfHistos[numOfTask] = 0;
   numOfGraphs[numOfTask] = 0;
   numOfTaskInclude[numOfTask] = 0;
   numOfSteering[numOfTask] = -1;
   numOfSteerFields[numOfTask][0] = 0;
   numOfSteerChildren[numOfTask][0] = 0;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // subtask
      if (type == 1 && !strcmp(name,"Task")) {
         // set task as parent for subsequent tasks
         recursiveDepth++;
         parent[recursiveDepth] = taskName[numOfTask];
         // read subtask
         if (!ReadXMLTask())
            return false;
         continue;
      }
      // end task
      if (type == 15 && !strcmp(name,"Task")) {
         // check input
         if (currentTaskName == "") {
            cout<<"The "<<(currentNumberOfTasks + 1)<<". Task has no name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (affiliations.GetEntriesFast() > 0) {
            taskUsed[numOfTask] = false;
            for (i = 0; i < affiliations.GetEntriesFast() && !taskUsed[numOfTask]; i++) {
               for (j = 0; j < numOfTaskAffiliations[numOfTask]; j++) {
                  if (affiliations.At(i) == taskAffiliation[numOfTask][j]) {
                     taskUsed[numOfTask] = true;
                     break;
                  }
               }
            }
         }
         if (taskUsed[numOfTask]) {
            hasTaskGenerated = true;
         }
         // description
         if (taskDescription[numOfTask].Length()
             && !taskShortDescription[numOfTask].Length()
             && !taskDescription[numOfTask].Contains("\n"))
            taskShortDescription[numOfTask] = taskDescription[numOfTask];
         recursiveDepth--;
         return true;
      }
      // task name
      if (type == 1 && !strcmp(name,"TaskName")) {
         xml->GetValue(taskName[numOfTask],taskName[numOfTask]);
         FormatText(taskName[numOfTask], kTRUE, " .");
         currentTaskName = taskName[numOfTask];
         // output
         if (makeOutput) for (i = 0; i < recursiveDepth; i++) cout<<"   ";
         if (makeOutput) taskName[numOfTask].WriteLine();
      }
      // task affiliation
      if (type == 1 && !strcmp(name,"Affiliation")) {
         taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]] = "";
         xml->GetValue(taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]],
                       taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]]);
         FormatText(taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]], kTRUE, " ");
         for (i = 0; i < affiliationList.GetEntries(); i++) {
            if (affiliationList.At(i,0) == taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]])
               break;
         }
         affiliationList.SetAt(taskAffiliation[numOfTask][numOfTaskAffiliations[numOfTask]], i, 0);
         affiliationList.SetAt("Task", i, affiliationList.GetEntriesAt(i));
         affiliationList.SetAt(taskName[numOfTask], i, affiliationList.GetEntriesAt(i));
         numOfTaskAffiliations[numOfTask]++;
      }
      // task event id
      if (type == 1 && !strcmp(name,"TaskEventId")) {
         xml->GetValue(taskEventID[numOfTask],taskEventID[numOfTask]);
         FormatText(taskEventID[numOfTask], kTRUE);
      }
      // task language
      if (type == 1 && !strcmp(name,"Language")) {
         xml->GetValue(tmp,"c++");
         FormatText(tmp, kTRUE);
      }
      // task author
      if (type == 1 && !strcmp(name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp(name,"AuthorName")) {
               xml->GetValue(taskAuthor[numOfTask],taskAuthor[numOfTask]);
               FormatText(taskAuthor[numOfTask], kTRUE);
            }
            if (type == 1 && !strcmp(name,"AuthorInstitute")) {
               xml->GetValue(taskAuthorInstitute[numOfTask],taskAuthorInstitute[numOfTask]);
               FormatText(taskAuthorInstitute[numOfTask], kTRUE);
            }
            if (type == 1 && !strcmp(name,"AuthorCollaboration")) {
               xml->GetValue(taskAuthorCollaboration[numOfTask],taskAuthorCollaboration[numOfTask]);
               FormatText(taskAuthorCollaboration[numOfTask], kTRUE);
            }
            if (type == 1 && !strcmp(name,"AuthorEmail")) {
               xml->GetValue(taskAuthorEmail[numOfTask],taskAuthorEmail[numOfTask]);
               FormatText(taskAuthorEmail[numOfTask], kTRUE);
            }
            if (type == 15 && !strcmp(name,"Author"))
               break;
         }
      }
      // task version
      if (type == 1 && !strcmp(name,"TaskVersion")) {
         xml->GetValue(taskVersion[numOfTask],taskVersion[numOfTask]);
         FormatText(taskVersion[numOfTask], kTRUE);
         ParseSVNKeyword(taskVersion[numOfTask]);
      }
      // task dependence
      if (type == 1 && !strcmp(name,"Dependence")) {
         xml->GetValue(taskDependence[numOfTask],taskDependence[numOfTask]);
         FormatText(taskDependence[numOfTask], kTRUE);
         hasDependenceCheck = kTRUE;
      }
      // task description
      if (type == 1 && !strcmp(name,"TaskDescription")) {
         xml->GetValue(taskDescription[numOfTask],taskDescription[numOfTask]);
         FormatText(taskDescription[numOfTask], kFALSE);
      }
      // task short description
      if (type == 1 && !strcmp(name,"TaskShortDescription")) {
         xml->GetValue(taskShortDescription[numOfTask],taskShortDescription[numOfTask]);
         FormatText(taskShortDescription[numOfTask], kTRUE);
      }
      // task usage
      if (type == 1 && !strcmp(name,"TaskUsage")) {
         xml->GetValue(taskUsage[numOfTask],taskUsage[numOfTask]);
         FormatText(taskUsage[numOfTask], kFALSE);
      }
      // task status
      if (type == 1 && !strcmp(name,"TaskStatus")) {
         xml->GetValue(taskStatus[numOfTask],taskStatus[numOfTask]);
         FormatText(taskStatus[numOfTask], kFALSE);
      }
      // task to do
      if (type == 1 && !strcmp(name,"TaskToDo")) {
         xml->GetValue(taskToDo[numOfTask],taskToDo[numOfTask]);
         FormatText(taskToDo[numOfTask], kFALSE);
      }
      // task known problems
      if (type == 1 && !strcmp(name,"TaskKnownProblems")) {
         xml->GetValue(taskKnownProblems[numOfTask],taskKnownProblems[numOfTask]);
         FormatText(taskKnownProblems[numOfTask], kFALSE);
      }
      // task include
      if (type == 1 && !strcmp(name,"Include")) {
         // include initialisation
         taskInclude[numOfTask][numOfTaskInclude[numOfTask]] = "";
         taskLocalFlag[numOfTask][numOfTaskInclude[numOfTask]] = false;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include name
            if (type == 1 && !strcmp(name,"IncludeName")) {
               xml->GetValue(taskInclude[numOfTask][numOfTaskInclude[numOfTask]],
                             taskInclude[numOfTask][numOfTaskInclude[numOfTask]]);
               FormatText(taskInclude[numOfTask][numOfTaskInclude[numOfTask]], kTRUE);
            }
            // include type
            if (type == 1 && !strcmp(name,"IncludeType")) {
               xml->GetValue(tmp,"false");
               FormatText(tmp, kTRUE);
               if (tmp == "local")
                  taskLocalFlag[numOfTask][numOfTaskInclude[numOfTask]] = true;
            }
            // include end
            if (type == 15 && !strcmp(name,"Include"))
               break;
         }
         // check input
         if (taskInclude[numOfTask][numOfTaskInclude[numOfTask]] == "") {
            cout<<"An Include of Task '"<<taskName[numOfTask].Data()<<"' has no Name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // count includes
         numOfTaskInclude[numOfTask]++;
         if (numOfTaskInclude[numOfTask] >= maxNumberOfInclude) {
            cout<<"Maximal number of includes in Task '"<<taskName[numOfTask].Data()<<"' reached : "
                <<numOfTaskInclude<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         continue;
      }
      // task histogram
      if (type == 1 && !strcmp(name,"Histogram")) {
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
         numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]] = 0;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // histo name
            if (type == 1 && !strcmp(name,"HistName")) {
               xml->GetValue(histoName[numOfTask][numOfHistos[numOfTask]],
                             histoName[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoName[numOfTask][numOfHistos[numOfTask]], kTRUE, " ");
            }
            // histo title
            if (type == 1 && !strcmp(name,"HistTitle")) {
               xml->GetValue(histoTitle[numOfTask][numOfHistos[numOfTask]],
                             histoTitle[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoTitle[numOfTask][numOfHistos[numOfTask]], kFALSE);
            }
            // histo folder name
            if (type == 1 && !strcmp(name,"HistFolderName")) {
               xml->GetValue(histoFolderName[numOfTask][numOfHistos[numOfTask]],
                             histoFolderName[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoFolderName[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo folder title
            if (type == 1 && !strcmp(name,"HistFolderTitle")) {
               xml->GetValue(histoFolderTitle[numOfTask][numOfHistos[numOfTask]],
                             histoFolderTitle[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoFolderTitle[numOfTask][numOfHistos[numOfTask]], kFALSE);
            }
            // histo type
            if (type == 1 && !strcmp(name,"HistType")) {
               xml->GetValue(histoType[numOfTask][numOfHistos[numOfTask]],
                             histoType[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoType[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo array size
            if (type == 1 && !strcmp(name,"HistArraySize")) {
               xml->GetValue(histoArraySize[numOfTask][numOfHistos[numOfTask]],
                             histoArraySize[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoArraySize[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo array start index
            if (type == 1 && !strcmp(name,"HistArrayStartIndex")) {
               xml->GetValue(histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]],
                             histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoArrayStartIndex[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo x label
            if (type == 1 && !strcmp(name,"HistXLabel")) {
               xml->GetValue(histoXLabel[numOfTask][numOfHistos[numOfTask]],
                             histoXLabel[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoXLabel[numOfTask][numOfHistos[numOfTask]], kFALSE);
            }
            // histo y label
            if (type == 1 && !strcmp(name,"HistYLabel")) {
               xml->GetValue(histoYLabel[numOfTask][numOfHistos[numOfTask]],
                             histoYLabel[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoYLabel[numOfTask][numOfHistos[numOfTask]], kFALSE);
            }
            // histo z label
            if (type == 1 && !strcmp(name,"HistZLabel")) {
               xml->GetValue(histoZLabel[numOfTask][numOfHistos[numOfTask]],
                             histoZLabel[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoZLabel[numOfTask][numOfHistos[numOfTask]], kFALSE);
            }
            // histo number of bins in x
            if (type == 1 && !strcmp(name,"HistXNbins")) {
               xml->GetValue(histoXNbins[numOfTask][numOfHistos[numOfTask]],
                             histoXNbins[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoXNbins[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo min value in x
            if (type == 1 && !strcmp(name,"HistXmin")) {
               xml->GetValue(histoXmin[numOfTask][numOfHistos[numOfTask]],
                             histoXmin[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoXmin[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo max value in x
            if (type == 1 && !strcmp(name,"HistXmax")) {
               xml->GetValue(histoXmax[numOfTask][numOfHistos[numOfTask]],
                             histoXmax[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoXmax[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo number of bins in y
            if (type == 1 && !strcmp(name,"HistYNbins")) {
               xml->GetValue(histoYNbins[numOfTask][numOfHistos[numOfTask]],
                             histoYNbins[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoYNbins[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo min value in y
            if (type == 1 && !strcmp(name,"HistYmin")) {
               xml->GetValue(histoYmin[numOfTask][numOfHistos[numOfTask]],
                             histoYmin[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoYmin[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo max value in y
            if (type == 1 && !strcmp(name,"HistYmax")) {
               xml->GetValue(histoYmax[numOfTask][numOfHistos[numOfTask]],
                             histoYmax[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoYmax[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo number of bins in z
            if (type == 1 && !strcmp(name,"HistZNbins")) {
               xml->GetValue(histoZNbins[numOfTask][numOfHistos[numOfTask]],
                             histoZNbins[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoZNbins[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo min value in z
            if (type == 1 && !strcmp(name,"HistZmin")) {
               xml->GetValue(histoZmin[numOfTask][numOfHistos[numOfTask]],
                             histoZmin[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoZmin[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // histo max value in z
            if (type == 1 && !strcmp(name,"HistZmax")) {
               xml->GetValue(histoZmax[numOfTask][numOfHistos[numOfTask]],
                             histoZmax[numOfTask][numOfHistos[numOfTask]]);
               FormatText(histoZmax[numOfTask][numOfHistos[numOfTask]], kTRUE);
            }
            // argus
            if (type == 1 && !strcmp(name,"Argus")) {
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // tab
                  if (type == 1 && !strcmp(name,"Tab")) {
                     if (numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]] >=
                         maxNumberOfHistoSingleObjectTabs) {
                        cout<<"Maximal number of Tabs in the Argus section of Histogram '"
                            <<histoName[numOfTask][numOfHistos[numOfTask]].Data()<<"' reached : "
                            <<maxNumberOfHistoSingleObjectTabs<<" !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                     histoSingleObjectTabName[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]] = "";
                     histoSingleObjectTabIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]] = "0";
                     histoSingleObjectTabArrayIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]] = "0";
                     while (xml->NextLine()) {
                        type = xml->GetType();
                        name = xml->GetName();
                        // tab name
                        if (type == 1 && !strcmp(name,"TabName")) {
                           xml->GetValue(histoSingleObjectTabName[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]]
                                         ,histoSingleObjectTabName[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]]);
                           FormatText(histoSingleObjectTabName[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]], kTRUE, " ");
                        }
                        // index
                        if (type == 1 && !strcmp(name,"Index")) {
                           xml->GetValue(histoSingleObjectTabIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]]
                                         ,histoSingleObjectTabIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]]);
                           FormatText(histoSingleObjectTabIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]], kTRUE);
                        }
                        // array index
                        if (type == 1 && !strcmp(name,"HistArrayIndex")) {
                           xml->GetValue(histoSingleObjectTabArrayIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]],
                                         histoSingleObjectTabArrayIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]]);
                           FormatText(histoSingleObjectTabArrayIndex[numOfTask][numOfHistos[numOfTask]][numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]],
                                      kTRUE);
                        }
                        // tab end
                        if (type == 15 && !strcmp(name,"Tab")) {
                           numOfHistoSingleObjectTabs[numOfTask][numOfHistos[numOfTask]]++;
                           break;
                        }
                     }
                  }
                  // argus end
                  if (type == 15 && !strcmp(name,"Argus"))
                     break;
               }
            }
            // histo end
            if (type == 15 && !strcmp(name,"Histogram"))
               break;
         }
         // check input
         if (histoName[numOfTask][numOfHistos[numOfTask]] == "") {
            cout<<"A Histo of Task '"<<taskName[numOfTask].Data()<<"' has no Name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (histoType[numOfTask][numOfHistos[numOfTask]] == "") {
            cout<<"Histogram '"<<histoName[numOfTask][numOfHistos[numOfTask]].Data()<<"' of Task '"
                <<taskName[numOfTask].Data()<<"' has no type defined !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         for (i = 0; i < numOfHistos[numOfTask]; i++) {
            for (j = i + 1; j < numOfHistos[numOfTask]; j++) {
               if (histoName[numOfTask][i] == histoName[numOfTask][j]) {
                  cout<<"\nTask '"<<taskName[numOfTask].Data()<<"' has two histos with the name '"
                      <<histoName[numOfTask][i].Data()<<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
         }
         // count histos
         numOfHistos[numOfTask]++;
         if (numOfHistos[numOfTask] >= maxNumberOfHistos) {
            cout<<"Maximal number of histos in task '"<<taskName[numOfTask].Data()<<"' reached : "
                <<maxNumberOfHistos<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         continue;
      }
      // task graph
      if (type == 1 && !strcmp(name,"Graph")) {
         // graph initialisation
         graphName[numOfTask][numOfGraphs[numOfTask]] = "";
         graphTitle[numOfTask][numOfGraphs[numOfTask]] = "";
         graphFolderName[numOfTask][numOfGraphs[numOfTask]] = "";
         graphFolderTitle[numOfTask][numOfGraphs[numOfTask]] = "";
         graphType[numOfTask][numOfGraphs[numOfTask]] = "ROMETGraph";
         graphArraySize[numOfTask][numOfGraphs[numOfTask]] = "1";
         graphArrayStartIndex[numOfTask][numOfGraphs[numOfTask]] = "0";
         graphXLabel[numOfTask][numOfGraphs[numOfTask]] = "X";
         graphYLabel[numOfTask][numOfGraphs[numOfTask]] = "Y";
         graphZLabel[numOfTask][numOfGraphs[numOfTask]] = "Z";
         graphXmin[numOfTask][numOfGraphs[numOfTask]] = "0";
         graphXmax[numOfTask][numOfGraphs[numOfTask]] = "1";
         graphYmin[numOfTask][numOfGraphs[numOfTask]] = "0";
         graphYmax[numOfTask][numOfGraphs[numOfTask]] = "1";
         graphZmin[numOfTask][numOfGraphs[numOfTask]] = "0";
         graphZmax[numOfTask][numOfGraphs[numOfTask]] = "1";
         numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]] = 0;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // histo name
            if (type == 1 && !strcmp(name,"GraphName")) {
               xml->GetValue(graphName[numOfTask][numOfGraphs[numOfTask]],
                             graphName[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphName[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph title
            if (type == 1 && !strcmp(name,"GraphTitle")) {
               xml->GetValue(graphTitle[numOfTask][numOfGraphs[numOfTask]],
                             graphTitle[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphTitle[numOfTask][numOfGraphs[numOfTask]], kFALSE);
            }
            // graph folder name
            if (type == 1 && !strcmp(name,"GraphFolderName")) {
               xml->GetValue(graphFolderName[numOfTask][numOfGraphs[numOfTask]],
                             graphFolderName[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphFolderName[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph folder title
            if (type == 1 && !strcmp(name,"GraphFolderTitle")) {
               xml->GetValue(graphFolderTitle[numOfTask][numOfGraphs[numOfTask]],
                             graphFolderTitle[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphFolderTitle[numOfTask][numOfGraphs[numOfTask]], kFALSE);
            }
            // graph type
            if (type == 1 && !strcmp(name,"GraphType")) {
               xml->GetValue(graphType[numOfTask][numOfGraphs[numOfTask]],
                             graphType[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphType[numOfTask][numOfGraphs[numOfTask]], kTRUE);
               if (graphType[numOfTask][numOfGraphs[numOfTask]] == "TGraph")
                  graphType[numOfTask][numOfGraphs[numOfTask]]="ROMETGraph";
               if (graphType[numOfTask][numOfGraphs[numOfTask]] == "TCutG")
                  graphType[numOfTask][numOfGraphs[numOfTask]]="ROMETCutG";
            }
            // graph array size
            if (type == 1 && !strcmp(name,"GraphArraySize")) {
               xml->GetValue(graphArraySize[numOfTask][numOfGraphs[numOfTask]],
                             graphArraySize[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphArraySize[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph array start index
            if (type == 1 && !strcmp(name,"GraphArrayStartIndex")) {
               xml->GetValue(graphArrayStartIndex[numOfTask][numOfGraphs[numOfTask]],
                             graphArrayStartIndex[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphArrayStartIndex[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph x label
            if (type == 1 && !strcmp(name,"GraphXLabel")) {
               xml->GetValue(graphXLabel[numOfTask][numOfGraphs[numOfTask]],
                             graphXLabel[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphXLabel[numOfTask][numOfGraphs[numOfTask]], kFALSE);
            }
            // graph y label
            if (type == 1 && !strcmp(name,"GraphYLabel")) {
               xml->GetValue(graphYLabel[numOfTask][numOfGraphs[numOfTask]],
                             graphYLabel[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphYLabel[numOfTask][numOfGraphs[numOfTask]], kFALSE);
            }
            // graph z label
            if (type == 1 && !strcmp(name,"GraphZLabel")) {
               xml->GetValue(graphZLabel[numOfTask][numOfGraphs[numOfTask]],
                             graphZLabel[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphZLabel[numOfTask][numOfGraphs[numOfTask]], kFALSE);
            }
            // graph xmin
            if (type == 1 && !strcmp(name,"GraphXmin")) {
               xml->GetValue(graphXmin[numOfTask][numOfGraphs[numOfTask]],
                             graphXmin[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphXmin[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph xmax
            if (type == 1 && !strcmp(name,"GraphXmax")) {
               xml->GetValue(graphXmax[numOfTask][numOfGraphs[numOfTask]],
                             graphXmax[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphXmax[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph ymin
            if (type == 1 && !strcmp(name,"GraphYmin")) {
               xml->GetValue(graphYmin[numOfTask][numOfGraphs[numOfTask]],
                             graphYmin[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphYmin[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph ymax
            if (type == 1 && !strcmp(name,"GraphYmax")) {
               xml->GetValue(graphYmax[numOfTask][numOfGraphs[numOfTask]],
                             graphYmax[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphYmax[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph zmin
            if (type == 1 && !strcmp(name,"GraphZmin")) {
               xml->GetValue(graphZmin[numOfTask][numOfGraphs[numOfTask]],
                             graphZmin[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphZmin[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // graph zmax
            if (type == 1 && !strcmp(name,"GraphZmax")) {
               xml->GetValue(graphZmax[numOfTask][numOfGraphs[numOfTask]],
                             graphZmax[numOfTask][numOfGraphs[numOfTask]]);
               FormatText(graphZmax[numOfTask][numOfGraphs[numOfTask]], kTRUE);
            }
            // argus
            if (type == 1 && !strcmp(name,"Argus")) {
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // tab
                  if (type == 1 && !strcmp(name,"Tab")) {
                     if (numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]] >=
                         maxNumberOfGraphSingleObjectTabs) {
                        cout<<"Maximal number of Tabs in the Argus section of Graph '"
                            <<graphName[numOfTask][numOfGraphs[numOfTask]].Data()<<"' reached : "
                            <<maxNumberOfGraphSingleObjectTabs<<" !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                     graphSingleObjectTabName[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]] = "";
                     graphSingleObjectTabIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]] = "0";
                     graphSingleObjectTabArrayIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]] = "0";
                     while (xml->NextLine()) {
                        type = xml->GetType();
                        name = xml->GetName();
                        // tab name
                        if (type == 1 && !strcmp(name,"TabName")) {
                           xml->GetValue(graphSingleObjectTabName[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]],
                                         graphSingleObjectTabName[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]]);
                           FormatText(graphSingleObjectTabName[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]],
                                      kTRUE, " ");
                        }
                        // index
                        if (type == 1 && !strcmp(name,"Index")) {
                           xml->GetValue(graphSingleObjectTabIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]],
                                         graphSingleObjectTabIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]]);
                           FormatText(graphSingleObjectTabIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]],
                                      kTRUE);
                        }
                        // array index
                        if (type == 1 && !strcmp(name,"HistArrayIndex")) {
                           xml->GetValue(graphSingleObjectTabArrayIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]],
                                         graphSingleObjectTabArrayIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]]);
                           FormatText(graphSingleObjectTabArrayIndex[numOfTask][numOfGraphs[numOfTask]][numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]],
                                      kTRUE);
                        }
                        // tab end
                        if (type == 15 && !strcmp(name,"Tab")) {
                           numOfGraphSingleObjectTabs[numOfTask][numOfGraphs[numOfTask]]++;
                           break;
                        }
                     }
                  }
                  // argus end
                  if (type == 15 && !strcmp(name,"Argus"))
                     break;
               }
            }
            // graph end
            if (type == 15 && !strcmp(name,"Graph"))
               break;
         }
         // check input
         if (graphName[numOfTask][numOfGraphs[numOfTask]] == "") {
            cout<<"A Graph of Task '"<<taskName[numOfTask].Data()<<"' has no Name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (graphType[numOfTask][numOfGraphs[numOfTask]] == "") {
            cout<<"Graph '"<<graphName[numOfTask][numOfGraphs[numOfTask]].Data()<<"' of Task '"
                <<taskName[numOfTask].Data()<<"' has no type defined !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         for (i = 0; i < numOfGraphs[numOfTask]; i++) {
            for (j = i + 1;j < numOfGraphs[numOfTask]; j++) {
               if (graphName[numOfTask][i] == graphName[numOfTask][j]) {
                  cout<<"\nTask '"<<taskName[numOfTask].Data()<<"' has two graphs with the name '"
                      <<graphName[numOfTask][i].Data()<<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
         }
         // count graphs
         numOfGraphs[numOfTask]++;
         if (numOfGraphs[numOfTask] >= maxNumberOfGraphs) {
            cout<<"Maximal number of graphs in task '"<<taskName[numOfTask].Data()<<"' reached : "
                <<maxNumberOfGraphs<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         continue;
      }
      // task steering parameters
      if (type == 1 && !strcmp(name,"SteeringParameters")) {
         // read steering parameter
         steerName[numOfTask][0] = "Steering";
         steerArraySize[numOfTask][0] = "1";
         steerParent[numOfTask][0] = -1;
         actualSteerIndex = 0;
         recursiveSteerDepth = 0;
         if (!ReadXMLSteering(numOfTask,false))
            return false;
         numOfSteering[numOfTask]++;
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLTab()
{
   // read the tab definitions out of the xml file
   const char *name;
   Int_t type, i, j, k,l;
   ROMEString currentTabName = "";
   ROMEString tmp;
   Int_t currentNumberOfTabs = 0;
   bool found;
   bool objectFound[maxNumberOfTabObjectDisplayObjects];
   bool typeFound;

   // count tabs
   numOfTab++;
   currentNumberOfTabs = numOfTab;
   if (currentNumberOfTabs >= maxNumberOfTabs) {
      cout<<"Maximal number of tabs reached : "<<maxNumberOfTabs<<" !"<<endl;
      cout<<"Terminating program."<<endl;
      return kFALSE;
   }
   // initialisation
   tabName[currentNumberOfTabs] = "";
   tabTitle[currentNumberOfTabs] = "";
   tabObjectDisplay[currentNumberOfTabs] = false;
   numOfTabAffiliations[numOfTab] = 0;
   tabUsed[numOfTab] = true;
   tabAuthor[currentNumberOfTabs] = mainAuthor;
   tabVersion[currentNumberOfTabs] = "0";
   tabDependence[currentNumberOfTabs] = "";
   tabDescription[currentNumberOfTabs] = "";
   tabShortDescription[currentNumberOfTabs] = "";
   tabUsage[currentNumberOfTabs] = "";
   tabStatus[currentNumberOfTabs] = "";
   tabToDo[currentNumberOfTabs] = "";
   tabKnownProblems[currentNumberOfTabs] = "";
   tabHeredity[currentNumberOfTabs] = "";
   tabHeredityIndex[currentNumberOfTabs] = 0;
   numOfSteering[currentNumberOfTabs+numOfTask + 1] = -1;
   numOfMenu[currentNumberOfTabs] = -1;
   tabNumOfChildren[currentNumberOfTabs] = 0;
   numOfThreadFunctions[currentNumberOfTabs] = 0;
   numOfTabSingleObjects[currentNumberOfTabs] = 0;
   numOfSteering[currentNumberOfTabs+numOfTask + 1] = 0;
   numOfSteerFields[currentNumberOfTabs+numOfTask + 1][0] = 0;
   numOfSteerChildren[currentNumberOfTabs+numOfTask + 1][0] = 0;
   numOfTabObjectDisplays[currentNumberOfTabs] = 0;
   numOfTabObjectDisplayObjectTypes[currentNumberOfTabs] = 0;
   memset(numOfTabObjectDisplayObjects[currentNumberOfTabs], 0, sizeof(Int_t) * maxNumberOfTabObjectDisplays);

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
            cout<<"The "<<(currentNumberOfTabs + 1)<<". Tab has no name !"<<endl;
            cout<<"Terminating program."<<endl;
            return kFALSE;
         }
         if (affiliations.GetEntriesFast() > 0) {
            tabUsed[currentNumberOfTabs] = false;
            for (i = 0; i < affiliations.GetEntriesFast() && !tabUsed[currentNumberOfTabs]; i++) {
               for (j = 0; j < numOfTabAffiliations[currentNumberOfTabs]; j++) {
                  if (affiliations.At(i) == tabAffiliation[currentNumberOfTabs][j]) {
                     tabUsed[currentNumberOfTabs] = true;
                     break;
                  }
               }
            }
         }
         // Check Tab Objects
         for (i = 0; i < numOfTabObjectDisplays[currentNumberOfTabs]; i++) {
            for (l = 0; l < numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]; l++) {
               if (tabObjectDisplayType[currentNumberOfTabs][i][l] == "none") {
                  cout<<"Object "<<l<<" of tab object display "<<tabObjectDisplayName[currentNumberOfTabs][i].Data()<<" of tab "<<
                     tabName[currentNumberOfTabs].Data()<<" not found!"<<endl;
                  cout<<"Terminating program."<<endl;
                  return kFALSE;
               }
            }
         }
         if (numOfTabObjectDisplays[currentNumberOfTabs] == 0 && tabObjectDisplay[currentNumberOfTabs])
            tabObjectDisplay[currentNumberOfTabs] = false;

         // description
         if (tabDescription[currentNumberOfTabs].Length()
             && !tabShortDescription[currentNumberOfTabs].Length()
             && !tabDescription[currentNumberOfTabs].Contains("\n"))
            tabShortDescription[currentNumberOfTabs] = tabDescription[currentNumberOfTabs];
         recursiveTabDepth--;
         return kTRUE;
      }
      // tab name
      if (type == 1 && !strcmp(name, "TabName")) {
         xml->GetValue(tabName[currentNumberOfTabs], tabName[currentNumberOfTabs]);
         FormatText(tabName[currentNumberOfTabs], kTRUE);
         currentTabName = tabName[currentNumberOfTabs];
         // output
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth; i++)
               cout<<"   ";
         if (makeOutput)
            tabName[currentNumberOfTabs].WriteLine();
      }
      // tab title
      if (type == 1 && !strcmp(name, "TabTitle")) {
         xml->GetValue(tabTitle[currentNumberOfTabs], tabTitle[currentNumberOfTabs]);
         FormatText(tabTitle[currentNumberOfTabs], kFALSE);
      }
      // tab histo display
      if (type == 1 && !strcmp(name, "HistogramDisplay")) {
         cout<<"The tag HistogramDisplay has been renamed to TabObjectDisplay. ( "<<tabName[currentNumberOfTabs]<<" )"
             <<endl;
         cout<<"Please see the ROME homepage under 'TabObjectDisplay'."<<endl;
         cout<<"Terminating program."<<endl;
         return kFALSE;
      }
      if (type == 1 && !strcmp(name, "TabObjectDisplay")) {
         xml->GetValue(tmp,"false");
         FormatText(tmp, kTRUE);
         if (tmp == "true")
            tabObjectDisplay[currentNumberOfTabs] = true;
      }
      // tab affiliation
      if (type == 1 && !strcmp(name,"Affiliation")) {
         tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]] = "";
         xml->GetValue(tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]],
                       tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]]);
         FormatText(tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]], kTRUE, " ");
         for (i = 0; i < affiliationList.GetEntries(); i++) {
            if (affiliationList.At(i,0) == tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]])
               break;
         }
         affiliationList.SetAt(tabAffiliation[numOfTab][numOfTabAffiliations[numOfTab]], i, 0);
         affiliationList.SetAt("Tab", i, affiliationList.GetEntriesAt(i));
         affiliationList.SetAt(tabName[numOfTab], i, affiliationList.GetEntriesAt(i));
         numOfTabAffiliations[numOfTab]++;
      }

      // tab author
      if (type == 1 && !strcmp(name,"Author")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp(name,"AuthorName")) {
               xml->GetValue(tabAuthor[currentNumberOfTabs],tabAuthor[currentNumberOfTabs]);
               FormatText(tabAuthor[currentNumberOfTabs], kTRUE);
            }
            if (type == 1 && !strcmp(name,"AuthorInstitute")) {
               xml->GetValue(tabAuthorInstitute[currentNumberOfTabs],tabAuthorInstitute[currentNumberOfTabs]);
               FormatText(tabAuthorInstitute[currentNumberOfTabs], kTRUE);
            }
            if (type == 1 && !strcmp(name,"AuthorCollaboration")) {
               xml->GetValue(tabAuthorCollaboration[currentNumberOfTabs],tabAuthorCollaboration[currentNumberOfTabs]);
               FormatText(tabAuthorCollaboration[currentNumberOfTabs], kTRUE);
            }
            if (type == 1 && !strcmp(name,"AuthorEmail")) {
               xml->GetValue(tabAuthorEmail[currentNumberOfTabs],tabAuthorEmail[currentNumberOfTabs]);
               FormatText(tabAuthorEmail[currentNumberOfTabs], kTRUE);
            }
            if (type == 15 && !strcmp(name,"Author"))
               break;
         }
      }
      // tab version
      if (type == 1 && !strcmp(name, "TabVersion")) {
         xml->GetValue(tabVersion[currentNumberOfTabs], tabVersion[currentNumberOfTabs]);
         FormatText(tabVersion[currentNumberOfTabs], kTRUE);
         ParseSVNKeyword(tabVersion[currentNumberOfTabs]);
      }
      // tab dependence
      if (type == 1 && !strcmp(name, "Dependence")) {
         xml->GetValue(tabDependence[currentNumberOfTabs], tabDependence[currentNumberOfTabs]);
         FormatText(tabDependence[currentNumberOfTabs], kTRUE);
         hasDependenceCheck = kTRUE;
      }
      // tab description
      if (type == 1 && !strcmp(name, "TabDescription")) {
         xml->GetValue(tabDescription[currentNumberOfTabs], tabDescription[currentNumberOfTabs]);
         FormatText(tabDescription[currentNumberOfTabs], kFALSE);
      }
      // tab short description
      if (type == 1 && !strcmp(name, "TabShortDescription")) {
         xml->GetValue(tabShortDescription[currentNumberOfTabs], tabShortDescription[currentNumberOfTabs]);
         FormatText(tabShortDescription[currentNumberOfTabs], kTRUE);
      }
      // tab usage
      if (type == 1 && !strcmp(name,"TabUsage")) {
         xml->GetValue(tabUsage[currentNumberOfTabs],tabUsage[currentNumberOfTabs]);
         FormatText(tabUsage[currentNumberOfTabs], kFALSE);
      }
      // tab status
      if (type == 1 && !strcmp(name,"TabStatus")) {
         xml->GetValue(tabStatus[currentNumberOfTabs],tabStatus[currentNumberOfTabs]);
         FormatText(tabStatus[currentNumberOfTabs], kFALSE);
      }
      // tab to do
      if (type == 1 && !strcmp(name,"TabToDo")) {
         xml->GetValue(tabToDo[currentNumberOfTabs],tabToDo[currentNumberOfTabs]);
         FormatText(tabToDo[currentNumberOfTabs], kFALSE);
      }
      // tab known problems
      if (type == 1 && !strcmp(name,"TabKnownProblems")) {
         xml->GetValue(tabKnownProblems[currentNumberOfTabs],tabKnownProblems[currentNumberOfTabs]);
         FormatText(tabKnownProblems[currentNumberOfTabs], kFALSE);
      }
      // tab heredity
      if (type == 1 && !strcmp(name, "InheritedFrom")) {
         xml->GetValue(tabHeredity[currentNumberOfTabs], tabHeredity[currentNumberOfTabs]);
         FormatText(tabHeredity[currentNumberOfTabs], kTRUE);
      }

      // tab steering parameters
      if (type == 1 && !strcmp(name, "SteeringParameters")) {
         // read steering parameter
         steerName[currentNumberOfTabs+numOfTask + 1][0] = "Steering";
         steerParent[currentNumberOfTabs+numOfTask + 1][0] = -1;
         actualSteerIndex = 0;
         recursiveSteerDepth = 0;
         if (!ReadXMLSteering(currentNumberOfTabs+numOfTask + 1,false))
            return kFALSE;
         numOfSteering[currentNumberOfTabs+numOfTask + 1]++;
      }
      // tab threadFunctions
      if (type == 1 && !strcmp(name, "ThreadFunctions")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout<<"   ";
         if (makeOutput)
            cout<<"ThreadFunctions:"<<endl;
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

                  if (type == 1 && !strcmp(name, "FunctionName")) {
                     xml->GetValue(threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]],
                                   threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]);
                     FormatText(threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]],
                                kTRUE);
                  }
                  if (type == 1 && !strcmp(name, "FunctionArgument")) {
                     xml->GetValue(threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]],
                                   threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]]);
                     FormatText(threadFunctionArgument[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]][numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]],
                                kTRUE);
                     numOfThreadFunctionArguments[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]++;
                  }
                  // end
                  if (type == 15 && !strcmp(name, "ThreadFunction")) {
                     // output
                     if (makeOutput)
                        for (i = 0; i < recursiveTabDepth + 2; i++)
                           cout<<"   ";
                     if (makeOutput)
                        threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]].WriteLine();
                     // check input
                     if (threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]] == "") {
                        cout<<"A thread function of tab '"<<tabName[currentNumberOfTabs].Data()<<"' has no Name !"
                            <<endl;
                        cout<<"Terminating program."<<endl;
                        return kFALSE;
                     }
                     for (j = 0; j < numOfThreadFunctions[currentNumberOfTabs]; j++) {
                        if (threadFunctionName[currentNumberOfTabs][j] ==
                            threadFunctionName[currentNumberOfTabs][numOfThreadFunctions[currentNumberOfTabs]]) {
                           cout<<"Two thread functions of tab '"<<tabName[currentNumberOfTabs].Data()
                               <<"' have the same Name !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return kFALSE;
                        }
                     }
                     // count thread functions
                     numOfThreadFunctions[currentNumberOfTabs]++;
                     if (numOfThreadFunctions[currentNumberOfTabs] >= maxNumberOfThreadFunctions) {
                        cout<<"Maximal number of thread functions reached : "<<maxNumberOfThreadFunctions<<" !"<<endl;
                        cout<<"Terminating program."<<endl;
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
               cout<<"   ";
         if (makeOutput)
            cout<<"Menus:"<<endl;
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
      // tab display objects
      if (type == 1 && !strcmp(name, "DisplayObjects")) {
         cout<<"The tag DisplayObjects and all subtags have been renamed. ( "<<tabName[currentNumberOfTabs]<<" )"
             <<endl;
         cout<<"Please see the ROME homepage under 'ObjectDisplays'."<<endl;
         cout<<"Terminating program."<<endl;
         return kFALSE;
      }
      if (type == 1 && !strcmp(name, "ObjectDisplays")) {
         if (makeOutput)
            for (i = 0; i < recursiveTabDepth + 1; i++)
               cout<<"   ";
         if (makeOutput)
            cout<<"Object Displays :"<<endl;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();

            // end
            if (type == 15 && !strcmp(name, "ObjectDisplays")) {
               break;
            }
            // display object
            if (type == 1 && !strcmp(name, "ObjectDisplay")) {
               numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]] = 0;
               tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]] = "";
               tabObjectDisplayTitle[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]] = "";
               tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]] = "";
               tabObjectDisplayType[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]] = "";
               tabObjectDisplayTaskHierarchyNumber[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                     = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  // object name
                  if (type == 1 && !strcmp(name, "ObjectDisplayName")) {
                     xml->GetValue(tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]],
                                   tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]);
                     FormatText(tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]],
                                kTRUE);
                     if (makeOutput) {
                        for (i = 0; i < recursiveTabDepth + 2; i++)
                           cout<<"   ";
                        tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]].WriteLine();
                     }
                  }
                  // object title
                  if (type == 1 && !strcmp(name, "ObjectDisplayTitle")) {
                     xml->GetValue(tabObjectDisplayTitle[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]],
                                   tabObjectDisplayTitle[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]);
                     FormatText(tabObjectDisplayTitle[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]],
                                kFALSE);
                  }
                  // object
                  if (type == 1 && !strcmp(name, "ObjectDisplayObject")) {
                     xml->GetValue(tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]],
                                   tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]]);
                     FormatText(tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]],
                                kTRUE);
                     if (tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]]
                         == "TGraph")
                        tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]]
                              = "ROMETGraph";
                     if (tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]]
                         == "TCutG")
                        tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]]
                              = "ROMETCutG";
                     numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]++;
                  }
                  // object task hierarchy index
                  if (type == 1 && !strcmp(name, "ObjectDisplayTaskHierarchyIndex")) {
                     xml->GetValue(tmp,"0");
                     FormatText(tmp, kTRUE);
                     tabObjectDisplayTaskHierarchyNumber[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                           = tmp.ToInteger();
                  }
                  // end
                  if (type == 15 && !strcmp(name, "ObjectDisplay")) {
                     // check input
                     if (tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]] == "") {
                        cout<<"A object display of tab '"<<tabName[currentNumberOfTabs].Data()<<"' has no Name !"
                            <<endl;
                        cout<<"Terminating program."<<endl;
                        return kFALSE;
                     }
                     if (tabObjectDisplayTitle[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]].Length() == 0)
                        tabObjectDisplayTitle[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                              = tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]];

                     for (j = 0; j < numOfTabObjectDisplays[currentNumberOfTabs]; j++) {
                        if (tabObjectDisplayName[currentNumberOfTabs][j]
                            == tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]) {
                           cout<<"Two object display of tab '"<<tabName[currentNumberOfTabs].Data()
                               <<"' have the same Name !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return kFALSE;
                        }
                     }
                     for (l = 0; l < numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]; l++) {
                        objectFound[l] = false;
                        for (i = 0; i < tabObjectDisplaySupportedObjects.GetEntriesFast() && !objectFound[l]; i++) {
                           for (k = 0; k < tabObjectDisplaySupportedObjects.GetEntriesFast(); k++) {
                              if (tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l] 
                                  == "ROMETGraph" ||
                                  tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                  == "ROMETCutG" ||
                                  tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                  == tabObjectDisplaySupportedObjects.At(k)) {
                                 objectFound[l] = true;
                                 tabObjectDisplayType[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                       = tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l];
                                 tabObjectDisplayTaskIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                                       = -1;
                                 tabObjectDisplayTaskHierarchyIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                                       = -1;
                                 tabObjectDisplayObjectIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                       = -1;
                                 break;
                              }
                           }
                        }
                        int num = 0;
                        for (i = 0; i < numOfTaskHierarchy && !objectFound[l]; i++) {
                           for (j = 0; j < numOfHistos[taskHierarchyClassIndex[i]] && !objectFound[l]; j++) {
                              if (tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                  == histoName[taskHierarchyClassIndex[i]][j]) {
                                 if (num
                                     == tabObjectDisplayTaskHierarchyNumber[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]) {
                                    objectFound[l] = true;
                                    tabObjectDisplayType[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                          = histoType[taskHierarchyClassIndex[i]][j];
                                    tabObjectDisplayTaskIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                                          = taskHierarchyClassIndex[i];
                                    tabObjectDisplayTaskHierarchyIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                                          = i;
                                    tabObjectDisplayObjectIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                          = j;
                                    typeFound = false;
                                    for (k = 0; k < tabObjectDisplaySupportedObjects.GetEntriesFast() &&
                                               !typeFound; k++) {
                                       if (histoType[taskHierarchyClassIndex[i]][j]
                                           == tabObjectDisplaySupportedObjects.At(k))
                                          typeFound = true;
                                    }
                                    if (!typeFound) {
                                       cout<<histoType[taskHierarchyClassIndex[i]][j]
                                           <<" histograms are not yet supported for object displays. ("
                                           <<tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]].Data()
                                           <<" of tab "<<tabName[currentNumberOfTabs].Data()<<")"<<endl;
                                       cout<<"Terminating program."<<endl;
                                       return kFALSE;
                                    }
                                 }
                                 num++;
                              }
                           }
                           for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]] && !objectFound[l]; j++) {
                              if (tabObjectDisplayObject[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                  == graphName[taskHierarchyClassIndex[i]][j]) {
                                 if (num
                                     == tabObjectDisplayTaskHierarchyNumber[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]) {
                                    objectFound[l] = true;
                                    tabObjectDisplayType[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                          = graphType[taskHierarchyClassIndex[i]][j];
                                    tabObjectDisplayTaskIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                                          = taskHierarchyClassIndex[i];
                                    tabObjectDisplayTaskHierarchyIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]
                                          = i;
                                    tabObjectDisplayObjectIndex[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l]
                                          = j;
                                    typeFound = false;
                                    for (k = 0; k < tabObjectDisplaySupportedObjects.GetEntriesFast() &&
                                               !typeFound; k++) {
                                       if (graphType[taskHierarchyClassIndex[i]][j]
                                           == tabObjectDisplaySupportedObjects.At(k))
                                          typeFound = true;
                                    }
                                    if (!typeFound) {
                                       cout<<graphType[taskHierarchyClassIndex[i]][j]
                                           <<" graphs are not yet supported for object displays. ("
                                           <<tabObjectDisplayName[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]].Data()
                                           <<" of tab "<<tabName[currentNumberOfTabs].Data()<<")"<<endl;
                                       cout<<"Terminating program."<<endl;
                                       return kFALSE;
                                    }
                                 }
                                 num++;
                              }
                           }
                        }
                     }
                     for (l = 0; l < numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]; l++) {
                        if (!objectFound[l]) {
                           tabObjectDisplayType[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]][l] = "none";
                        }
                     }
                     for (l = 0; l < numOfTabObjectDisplayObjects[currentNumberOfTabs][numOfTabObjectDisplays[currentNumberOfTabs]]; l++) {
                        for (i = 0; i < numOfTabObjectDisplays[currentNumberOfTabs]+1; i++) {
                           found = false;
                           for (j = 0; j < numOfTabObjectDisplayObjectTypes[currentNumberOfTabs] && !found; j++) {
                              if (tabObjectDisplayType[currentNumberOfTabs][i][l]
                                  == tabObjectDisplayObjectType[currentNumberOfTabs][j]) {
                                 found = true;
                                 tabObjectDisplayObjectTypeIndex[currentNumberOfTabs][i][l] = j;
                                 break;
                              }
                           }
                           if (!found) {
                              tabObjectDisplayObjectType[currentNumberOfTabs][numOfTabObjectDisplayObjectTypes[currentNumberOfTabs]]
                                    = tabObjectDisplayType[currentNumberOfTabs][i][l];
                              tabObjectDisplayObjectTypeIndex[currentNumberOfTabs][i][l]
                                    = numOfTabObjectDisplayObjectTypes[currentNumberOfTabs];
                              numOfTabObjectDisplayObjectTypes[currentNumberOfTabs]++;
                           }
                        }
                     }
                     // count object displays
                     numOfTabObjectDisplays[currentNumberOfTabs]++;
                     if (numOfTabObjectDisplays[currentNumberOfTabs] >= maxNumberOfTabObjectDisplays) {
                        cout<<"Maximal number of object displays reached : "<<maxNumberOfTabObjectDisplays<<" !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return kFALSE;
                     }
                     break;
                  }
               }
            }
         }
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLMenu(Int_t currentNumberOfTabs)
{
   Int_t type, i, j;
   const char *name;

   Int_t currentNumberOfMenus = numOfMenu[currentNumberOfTabs];
   menuDepth[currentNumberOfTabs][currentNumberOfMenus] = recursiveMenuDepth;
   // count menus
   numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] = -1;
   if (currentNumberOfMenus >= maxNumberOfMenus) {
      cout<<"Maximal number of menus reached : "<<maxNumberOfMenus<<" !"<<endl;
      cout<<"Terminating program."<<endl;
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
               cout<<"   ";
         if (makeOutput)
            menuTitle[currentNumberOfTabs][currentNumberOfMenus].WriteLine();
         numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
         for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
            if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE) {
               if (makeOutput)
                  for (i = 0; i < recursiveTabDepth + 3; i++)
                     cout<<"   ";
               if (makeOutput)
                  menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j].WriteLine();
            }
         }
         break;
      }
      if (type == 1 && !strcmp(name, "MenuTitle")) {
         xml->GetValue(menuTitle[currentNumberOfTabs][currentNumberOfMenus],
                       menuTitle[currentNumberOfTabs][currentNumberOfMenus]);
         FormatText(menuTitle[currentNumberOfTabs][currentNumberOfMenus], kTRUE);
      }

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
                  cout<<"Maximal number of menu items reached : "<<maxNumberOfMenuItems<<" !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return kFALSE;
               }
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]
                     = numOfMenu[currentNumberOfTabs];
               if (!ReadXMLMenu(currentNumberOfTabs))
                  return kFALSE;
               recursiveMenuDepth--;
            }
            // Splitter
            if (type == 1 && !strcmp(name, "Line")) {
               // count menu items
               numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]++;
               if (numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus] >= maxNumberOfMenuItems) {
                  cout<<"Maximal number of menu items reached : "<<maxNumberOfMenuItems<<" !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]
                     = LINE_TITLE;
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]
                     = "";
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
                  cout<<"Maximal number of menu items reached : "<<maxNumberOfMenuItems<<" !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return kFALSE;
               }
               menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]
                     = "";
               menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]
                     = "";
               menuItemChildMenuIndex[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]
                     = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();

                  // end
                  if (type == 15 && !strcmp(name, "MenuItem"))
                     break;
                  if (type == 1 && !strcmp(name, "MenuItemTitle")) {
                     xml->GetValue(menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]],
                                   menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                     FormatText(menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]],
                                kTRUE);
                  }
                  if (type == 1 && !strcmp(name, "MenuItemEnumName")) {
                     xml->GetValue(menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]],
                                   menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]);
                     FormatText(menuItemEnumName[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]],
                                kTRUE);
                  }
               }

               // check input
               if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]
                   == "") {
                  cout<<currentNumberOfTabs<<endl;
                  cout<<currentNumberOfMenus<<endl;
                  cout<<numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]<<endl;
                  cout<<"A menu item of tab '"<<tabName[currentNumberOfTabs].Data()<<"' has no Title !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return kFALSE;
               }
               for (j = 0; j < numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]; j++) {
                  if (menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j] != LINE_TITLE && menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][j]
                      == menuItemTitle[currentNumberOfTabs][currentNumberOfMenus][numOfMenuItem[currentNumberOfTabs][currentNumberOfMenus]]) {
                     cout<<"Two menu items of tab '"<<tabName[currentNumberOfTabs].Data()<<"' have the same Title !"<<endl;
                     cout<<"Terminating program."<<endl;
                     return kFALSE;
                  }
               }
            }
         }
      }
   }

   // check input
   if (menuTitle[currentNumberOfTabs][currentNumberOfMenus] == "") {
      cout<<"A menu of tab '"<<tabName[currentNumberOfTabs].Data()<<"' has no Title !"<<endl;
      cout<<"Terminating program."<<endl;
      return kFALSE;
   }
   for (j = 0; j < currentNumberOfMenus; j++) {
      if (menuTitle[currentNumberOfTabs][j] == menuTitle[currentNumberOfTabs][currentNumberOfMenus]) {
         cout<<"Two menus of tab '"<<tabName[currentNumberOfTabs].Data()<<"' have the same Title !"<<endl;
         cout<<"Terminating program."<<endl;
         return kFALSE;
      }
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLTree()
{
   const char *name;
   int type,i,j;

   // output
   if (makeOutput) cout<<"\n\nTrees:"<<endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp(name,"Tree")) {
         // count trees
         numOfTree++;
         if (numOfTree >= maxNumberOfTrees) {
            cout<<"Maximal number of trees reached : "<<maxNumberOfTrees<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // tree initialisation
         numOfBranch[numOfTree] = 0;
         numOfRunHeader[numOfTree] = 0;
         treeName[numOfTree] = "";
         treeTitle[numOfTree] = "";
         treeFileName[numOfTree] = "";
         treeDescription[numOfTree] = "";

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // tree name
            if (type == 1 && !strcmp(name,"TreeName")) {
               xml->GetValue(treeName[numOfTree],treeName[numOfTree]);
               FormatText(treeName[numOfTree], kTRUE);
               // output
               if (makeOutput) cout<<"   "<<treeName[numOfTree].Data()<<endl;
            }
            // tree title
            if (type == 1 && !strcmp(name,"TreeTitle")) {
               xml->GetValue(treeTitle[numOfTree],treeTitle[numOfTree]);
               FormatText(treeTitle[numOfTree], kFALSE);
            }
            // tree description
            if (type == 1 && !strcmp(name,"TreeDescription")) {
               xml->GetValue(treeDescription[numOfTree],treeDescription[numOfTree]);
               FormatText(treeDescription[numOfTree], kFALSE);
            }
            // tree file name
            if (type == 1 && !strcmp(name,"TreeOutputFileName")) {
               xml->GetValue(treeFileName[numOfTree],treeFileName[numOfTree]);
               FormatText(treeFileName[numOfTree], kTRUE);
            }
            // run header
            if (type == 1 && !strcmp(name,"RunHeader")) {
               // run header initialisation
               runHeaderName[numOfTree][numOfRunHeader[numOfTree]] = "";
               runHeaderFolder[numOfTree][numOfRunHeader[numOfTree]] = "";
               runHeaderFolderIndex[numOfTree][numOfRunHeader[numOfTree]] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // run header name
                  if (type == 1 && !strcmp(name,"RunHeaderName")) {
                     xml->GetValue(runHeaderName[numOfTree][numOfRunHeader[numOfTree]],
                                   runHeaderName[numOfTree][numOfRunHeader[numOfTree]]);
                     FormatText(runHeaderName[numOfTree][numOfRunHeader[numOfTree]], kTRUE);
                  }
                  // run header folder
                  if (type == 1 && !strcmp(name,"RelatedFolder")) {
                     xml->GetValue(runHeaderFolder[numOfTree][numOfRunHeader[numOfTree]],
                                   runHeaderFolder[numOfTree][numOfRunHeader[numOfTree]]);
                     FormatText(runHeaderFolder[numOfTree][numOfRunHeader[numOfTree]], kTRUE);
                  }
                  // runHeader
                  if (type == 15 && !strcmp(name,"RunHeader"))
                     break;
               }
               bool found = false;
               for (i = 0; i < numOfFolder; i++) {
                  if (!folderSupport[i] && folderName[i] == runHeaderFolder[numOfTree][numOfRunHeader[numOfTree]]) {
                     found = true;
                     runHeaderFolderIndex[numOfTree][numOfRunHeader[numOfTree]] = i;
                  }
               }
               if (!found) {
                  cout<<"Folder of RunHeader '"<<runHeaderFolder[numOfTree][numOfRunHeader[numOfTree]].Data()
                      <<"' of Tree '"<<treeName[numOfTree].Data()<<"' not existing !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (runHeaderName[numOfTree][numOfRunHeader[numOfTree]] == "") {
                  runHeaderName[numOfTree][numOfRunHeader[numOfTree]]
                        = runHeaderFolder[numOfTree][numOfRunHeader[numOfTree]];
               }
               // count run headeres
               numOfRunHeader[numOfTree]++;
               if (numOfRunHeader[numOfTree] >= maxNumberOfRunHeaders) {
                  cout<<"Maximal number of run headers in tree '"<<treeName[numOfTree].Data()<<"' reached : "
                      <<maxNumberOfRunHeaders<<" !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
            if (type == 1 && !strcmp(name,"Branch")) {
               // branch initialisation
               branchName[numOfTree][numOfBranch[numOfTree]] = "";
               branchFolder[numOfTree][numOfBranch[numOfTree]] = "";
               branchBufferSize[numOfTree][numOfBranch[numOfTree]] = "32000";
               branchSplitLevel[numOfTree][numOfBranch[numOfTree]] = "99";
               branchSplitLevelSpecified[numOfTree][numOfBranch[numOfTree]] = kFALSE;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // branch name
                  if (type == 1 && !strcmp(name,"BranchName")) {
                     xml->GetValue(branchName[numOfTree][numOfBranch[numOfTree]],
                                   branchName[numOfTree][numOfBranch[numOfTree]]);
                     FormatText(branchName[numOfTree][numOfBranch[numOfTree]], kTRUE);
                     // output
                     if (makeOutput) cout<<"      "<<branchName[numOfTree][numOfBranch[numOfTree]].Data()<<endl;
                  }
                  // branch folder
                  if (type == 1 && !strcmp(name,"RelatedFolder")) {
                     xml->GetValue(branchFolder[numOfTree][numOfBranch[numOfTree]],
                                   branchFolder[numOfTree][numOfBranch[numOfTree]]);
                     FormatText(branchFolder[numOfTree][numOfBranch[numOfTree]], kTRUE);
                  }
                  // branch buffer size
                  if (type == 1 && !strcmp(name,"BufferSize")) {
                     xml->GetValue(branchBufferSize[numOfTree][numOfBranch[numOfTree]],
                                   branchBufferSize[numOfTree][numOfBranch[numOfTree]]);
                     FormatText(branchBufferSize[numOfTree][numOfBranch[numOfTree]], kTRUE);
                  }
                  // branch split level
                  if (type == 1 && !strcmp(name,"SplitLevel")) {
                     xml->GetValue(branchSplitLevel[numOfTree][numOfBranch[numOfTree]],
                                   branchSplitLevel[numOfTree][numOfBranch[numOfTree]]);
                     FormatText(branchSplitLevel[numOfTree][numOfBranch[numOfTree]], kTRUE);
                     branchSplitLevelSpecified[numOfTree][numOfBranch[numOfTree]] = kTRUE;
                  }
                  // branch end
                  if (type == 15 && !strcmp(name,"Branch"))
                     break;
               }
               // input chaeck
               if (branchName[numOfTree][numOfBranch[numOfTree]] == "") {
                  cout<<"Branch without a name in Tree '"<<treeName[numOfTree].Data()<<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (branchFolder[numOfTree][numOfBranch[numOfTree]] == "") {
                  cout<<"Branch '"<<branchName[numOfTree][numOfBranch[numOfTree]].Data()<<"' of Tree '"
                      <<treeName[numOfTree].Data()<<"' has no Folder specified!"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               bool found = false;
               for (i = 0; i < numOfFolder; i++) {
                  if (!folderSupport[i] && folderName[i] == branchFolder[numOfTree][numOfBranch[numOfTree]]) {
                     found = true;
                     // set split level to be zero when branch folder has a class as a field
                     if (folderHasClassField[i] && !branchSplitLevelSpecified[numOfTree][numOfBranch[numOfTree]]) {
#if 0 // disable automatic setting until safety of split level 0 is checked.
                        branchSplitLevel[numOfTree][numOfBranch[numOfTree]] = "0";
#endif
                     }
                  }
               }
               if (!found) {
                  cout<<"Folder of Branch '"<<branchName[numOfTree][numOfBranch[numOfTree]].Data()<<"' of Tree '"
                      <<treeName[numOfTree].Data()<<"' not existing !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               // count branches
               numOfBranch[numOfTree]++;
               if (numOfBranch[numOfTree] >= maxNumberOfBranches) {
                  cout<<"Maximal number of branches in tree '"<<treeName[numOfTree].Data()<<"' reached : "
                      <<maxNumberOfBranches<<" !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
            if (type == 15 && !strcmp(name,"Tree")) {
               // input check
               if (treeName[numOfTree] == "") {
                  cout<<"Tree without a name !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (numOfBranch[numOfTree] == 0) {
                  cout<<"Tree '"<<treeName[numOfTree].Data()<<"' has no Branch !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               for (i = 0; i < numOfBranch[numOfTree]; i++) {
                  for (j = i + 1; j < numOfBranch[numOfTree]; j++) {
                     if (branchName[numOfTree][i] == branchName[numOfTree][j]) {
                        cout<<"\nTree '"<<treeName[numOfTree].Data()<<"' has two branches with the name '"
                            <<branchName[numOfTree][i].Data()<<"' !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                  }
               }
               break;
            }
         }
      }

      if (type == 15 && !strcmp(name,"Trees")) {
         // input check
         for (i = 0; i < numOfTree; i++) {
            for (j = i + 1; j < numOfTree; j++) {
               if (treeName[i] == treeName[j]) {
                  cout<<"\nTree '"<<treeName[i].Data()<<"' is defined twice !"<<endl;
                  cout<<"Terminating program."<<endl;
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

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLDAQ()
{
   const char *name;
   int type,i,j;

   // output
   if (makeOutput) cout<<"\n\nUser DAQ Systems:"<<endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp(name,"UserDAQSystem")) {
         // count trees
         numOfDAQ++;
         if (numOfDAQ >= maxNumberOfDAQ) {
            cout<<"Maximal number of user DAQ systems reached : "<<maxNumberOfDAQ<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // daq initialisation
         daqName[numOfDAQ] = "";
         numOfDAQAffiliations[numOfDAQ] = 0;
         daqUsed[numOfDAQ] = true;

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // daq name
            if (type == 1 && !strcmp(name,"DAQSystemName")) {
               xml->GetValue(daqName[numOfDAQ],daqName[numOfDAQ]);
               FormatText(daqName[numOfDAQ], kTRUE, " ");
               // output
               if (makeOutput) cout<<"   "<<daqName[numOfDAQ].Data()<<endl;
            }
            // daq affiliation
            if (type == 1 && !strcmp(name,"Affiliation")) {
               daqAffiliation[numOfDAQ][numOfDAQAffiliations[numOfDAQ]] = "";
               xml->GetValue(daqAffiliation[numOfDAQ][numOfDAQAffiliations[numOfDAQ]],
                             daqAffiliation[numOfDAQ][numOfDAQAffiliations[numOfDAQ]]);
               FormatText(daqAffiliation[numOfDAQ][numOfDAQAffiliations[numOfDAQ]], kTRUE, " ");
               for (i = 0; i < affiliationList.GetEntries(); i++) {
                  if (affiliationList.At(i,0) == daqAffiliation[numOfDAQ][numOfDAQAffiliations[numOfDAQ]])
                     break;
               }
               affiliationList.SetAt(daqAffiliation[numOfDAQ][numOfDAQAffiliations[numOfDAQ]], i, 0);
               affiliationList.SetAt("DAQ", i, affiliationList.GetEntriesAt(i));
               affiliationList.SetAt(daqName[numOfDAQ], i, affiliationList.GetEntriesAt(i));
               numOfDAQAffiliations[numOfDAQ]++;
            }
            if (type == 15 && !strcmp(name,"UserDAQSystem")) {
               // input check
               if (daqName[numOfDAQ] == "") {
                  cout<<"User DAQ system without a name !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (affiliations.GetEntriesFast() > 0) {
                  daqUsed[numOfDAQ] = false;
                  for (i = 0; i < affiliations.GetEntriesFast() && !daqUsed[numOfDAQ]; i++) {
                     for (j = 0; j < numOfDAQAffiliations[numOfDAQ]; j++) {
                        if (affiliations.At(i) == daqAffiliation[numOfDAQ][j]) {
                           daqUsed[numOfDAQ] = true;
                           break;
                        }
                     }
                  }
               }
               break;
            }
         }
      }

      if (type == 15 && !strcmp(name,"UserDAQSystems")) {
         // input check
         for (i = 0; i < numOfDAQ; i++) {
            for (j = i + 1; j < numOfDAQ; j++) {
               if (daqName[i] == daqName[j]) {
                  cout<<"\nDAQ system '"<<daqName[i].Data()<<"' is defined twice !"<<endl;
                  cout<<"Terminating program."<<endl;
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

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLDB()
{
   const char *name;
   int type,i,j;

   // output
   if (makeOutput) cout<<"\n\nUser Database Interfaces:"<<endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp(name,"UserDatabase")) {
         // count trees
         numOfDB++;
         if (numOfDB >= maxNumberOfDB) {
            cout<<"Maximal number of user database interfaces reached : "<<maxNumberOfDB<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // database initialisation
         dbName[numOfDB] = "";

         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // database name
            if (type == 1 && !strcmp(name,"DatabaseName")) {
               xml->GetValue(dbName[numOfDB],dbName[numOfDB]);
               FormatText(dbName[numOfDB], kTRUE, " ");
               // output
               if (makeOutput) cout<<"   "<<dbName[numOfDB].Data()<<endl;
            }
            // database description
            if (type == 1 && !strcmp(name,"DatabaseDescription")) {
               xml->GetValue(dbDescription[numOfDB],dbDescription[numOfDB]);
               FormatText(dbDescription[numOfDB], kFALSE);
            }
            if (type == 15 && !strcmp(name,"UserDatabase")) {
               // input check
               if (dbName[numOfDB] == "") {
                  cout<<"User database interface without a name !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               break;
            }
         }
      }

      if (type == 15 && !strcmp(name,"UserDatabases")) {
         // input check
         for (i = 0; i < numOfDB; i++) {
            for (j = i + 1; j < numOfDB; j++) {
               if (dbName[i] == dbName[j]) {
                  cout<<"\nDatabase '"<<dbName[i].Data()<<"' is defined twice !"<<endl;
                  cout<<"Terminating program."<<endl;
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

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLMidasBanks()
{
   const char *name;
   int type,i,j,k,kk;
   ROMEString tmp;
   char *cstop=NULL;

   // output
   if (makeOutput) cout<<"\n\nBanks:"<<endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp("EventHeader",name)) {
         // output
         if (makeOutput) cout<<"   Header"<<endl;
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
            if (type == 1 && !strcmp(name,"Folder")) {
               xml->GetValue(bankHeaderFolder,bankHeaderFolder);
               FormatText(bankHeaderFolder, kTRUE);
            }
            // header EventID
            if (type == 1 && !strcmp(name,"EventID")) {
               xml->GetValue(bankHeaderEventID,bankHeaderEventID);
               FormatText(bankHeaderEventID, kTRUE);
            }
            // header TriggerMask
            if (type == 1 && !strcmp(name,"TriggerMask")) {
               xml->GetValue(bankHeaderTriggerMask,bankHeaderTriggerMask);
               FormatText(bankHeaderTriggerMask, kTRUE);
            }
            // header SerialNumber
            if (type == 1 && !strcmp(name,"SerialNumber")) {
               xml->GetValue(bankHeaderSerialNumber,bankHeaderSerialNumber);
               FormatText(bankHeaderSerialNumber, kTRUE);
            }
            // header TimeStamp
            if (type == 1 && !strcmp(name,"TimeStamp")) {
               xml->GetValue(bankHeaderTimeStamp,bankHeaderTimeStamp);
               FormatText(bankHeaderTimeStamp, kTRUE);
            }
            // header end
            if (type == 15 && !strcmp(name,"EventHeader"))
               break;
         }
         // input check
         if (bankHeaderFolder == "") {
            cout<<"Midas event header has no folder !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         int iFold = -1;
         for (i = 0; i < numOfFolder; i++) {
            if (!folderSupport[i] && folderName[i] == bankHeaderFolder) {
               iFold = i;
               break;
            }
         }
         if (iFold == -1) {
            cout<<"Midas event header : folder '"<<bankHeaderFolder.Data()<<"' does not exist !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (folderArray[iFold] != "1") {
            cout<<"Midas event header : folder '"<<bankHeaderFolder.Data()<<"' is an array !"<<endl;
            cout<<"Terminating program."<<endl;
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
         if (bankHeaderSerialNumber == "")
            foundNum = true;
         if (bankHeaderTimeStamp == "")
            foundTime = true;
         for (i = 0; i < numOfValue[iFold]; i++) {
            if (valueName[iFold][i] == bankHeaderEventID) {
               foundID = true;
            }
            if (valueName[iFold][i] == bankHeaderTriggerMask) {
               foundMask = true;
            }
            if (valueName[iFold][i] == bankHeaderSerialNumber) {
               foundNum = true;
            }
            if (valueName[iFold][i] == bankHeaderTimeStamp) {
               foundTime = true;
            }
         }
         if (!foundID) {
            cout<<"Midas event header : event id field '"<<bankHeaderEventID.Data()<<"' does not exist in folder '"
                <<bankHeaderFolder<<"'!"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (!foundMask) {
            cout<<"Midas event header : trigger mask field '"<<bankHeaderTriggerMask.Data()
                <<"' does not exist in folder '"<<bankHeaderFolder<<"'!"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (!foundNum) {
            cout<<"Midas event header : serial number field '"<<bankHeaderSerialNumber.Data()<<"' does not exist '"
                <<bankHeaderFolder<<"'!"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (!foundTime) {
            cout<<"Midas event header : time stamp field '"<<bankHeaderTimeStamp.Data()<<"' does not exist '"
                <<bankHeaderFolder<<"'!"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
      } else if (type == 1 && !strcmp(name,"EventDefinition")) {
         numOfEvent++;
         if (numOfEvent >= maxNumberOfEvents) {
            cout<<"Maximal number of midas events reached : "<<maxNumberOfEvents<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // event initialisation
         eventName[numOfEvent] = "";
         eventID[numOfEvent] = "-1";
         eventTriggerMask[numOfEvent] = "-1";
         eventSamplingRate[numOfEvent] = "2";
         numOfBank[numOfEvent] = -1;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // event name
            if (type == 1 && !strcmp(name,"EventName")) {
               xml->GetValue(eventName[numOfEvent],eventName[numOfEvent]);
               FormatText(eventName[numOfEvent], kTRUE);
               // output
               if (makeOutput) cout<<"   "<<eventName[numOfEvent].Data()<<endl;
            }
            // event id
            if (type == 1 && !strcmp(name,"EventID")) {
               xml->GetValue(eventID[numOfEvent],eventID[numOfEvent]);
               FormatText(eventID[numOfEvent], kTRUE);
            }
            // event trigger mask
            if (type == 1 && !strcmp(name,"EventTriggerMask")) {
               xml->GetValue(eventTriggerMask[numOfEvent],eventTriggerMask[numOfEvent]);
               FormatText(eventTriggerMask[numOfEvent], kTRUE);
            }
            // event sampling rate
            if (type == 1 && !strcmp(name,"EventSamplingRate")) {
               xml->GetValue(eventSamplingRate[numOfEvent],eventSamplingRate[numOfEvent]);
               FormatText(eventSamplingRate[numOfEvent], kTRUE);
            }
            if (type == 1 && !strcmp(name,"Banks")) {
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  if (type == 1 && !strcmp(name,"Bank")) {
                     // count banks
                     numOfBank[numOfEvent]++;
                     if (numOfBank[numOfEvent] >= maxNumberOfBanks) {
                        cout<<"Maximal number of banks reached : "<<maxNumberOfBanks<<" !"<<endl;
                        cout<<"Terminating program."<<endl;
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
                        if (type == 1 && !strcmp(name,"BankName")) {
                           xml->GetValue(bankName[numOfEvent][numOfBank[numOfEvent]],
                                         bankName[numOfEvent][numOfBank[numOfEvent]]);
                           FormatText(bankName[numOfEvent][numOfBank[numOfEvent]], kTRUE);
                           // output
                           if (makeOutput) cout<<"      "<<bankName[numOfEvent][numOfBank[numOfEvent]].Data()<<endl;
                        }
                        // bank type
                        if (type == 1 && !strcmp(name,"BankType")) {
                           xml->GetValue(bankType[numOfEvent][numOfBank[numOfEvent]],
                                         bankType[numOfEvent][numOfBank[numOfEvent]]);
                           FormatText(bankType[numOfEvent][numOfBank[numOfEvent]], kTRUE);
                        }
                        // bank array
                        if (type == 1 && !strcmp(name,"BankArray")) {
                           while (xml->NextLine()) {
                              type = xml->GetType();
                              name = xml->GetName();
                              // lower boundary
                              if (type == 1 && !strcmp(name,"BASize")) {
                                 xml->GetValue(tmp,"");
                                 FormatText(tmp, kTRUE);
                                 bankArraySize[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // upper boundary
                              if (type == 1 && !strcmp(name,"BAStartIndex")) {
                                 xml->GetValue(tmp,"");
                                 FormatText(tmp, kTRUE);
                                 bankArrayStart[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // number of digits
                              if (type == 1 && !strcmp(name,"BANumberOfDigits")) {
                                 xml->GetValue(tmp,"");
                                 FormatText(tmp, kTRUE);
                                 bankArrayDigit[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              if (type == 15 && !strcmp(name,"BankArray"))
                                 break;
                           }
                        }
                        // bank end
                        if (type == 15 && !strcmp(name,"Bank"))
                           break;
                     }
                     // input check
                     if (bankName[numOfEvent][numOfBank[numOfEvent]] == "") {
                        cout<<"A Midas Bank has no Name !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                     if (bankType[numOfEvent][numOfBank[numOfEvent]] == "") {
                        cout<<"Bank '"<<bankName[numOfEvent][numOfBank[numOfEvent]].Data()<<"' has no type !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                  } else if (type == 1 && !strcmp(name,"StructuredBank")) {
                     // count banks
                     numOfBank[numOfEvent]++;
                     if (numOfBank[numOfEvent] >= maxNumberOfBanks) {
                        cout<<"Maximal number of banks reached : "<<maxNumberOfBanks<<" !"<<endl;
                        cout<<"Terminating program."<<endl;
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
                        if (type == 1 && !strcmp(name,"BankName")) {
                           xml->GetValue(bankName[numOfEvent][numOfBank[numOfEvent]],
                                         bankName[numOfEvent][numOfBank[numOfEvent]]);
                           FormatText(bankName[numOfEvent][numOfBank[numOfEvent]], kTRUE);
                           // output
                           if (makeOutput) cout<<"      "<<bankName[numOfEvent][numOfBank[numOfEvent]].Data()<<endl;
                        }
                        // structured bank field
                        if (type == 1 && !strcmp(name,"BankField")) {
                           // structured bank field initialisation
                           structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]
                                 = "";
                           structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]
                                 = "";
                           structFieldSize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]
                                 = "";
                           bankFieldArraySize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]
                                 = "1";
                           while (xml->NextLine()) {
                              type = xml->GetType();
                              name = xml->GetName();
                              // field name
                              if (type == 1 && !strcmp(name,"BankFieldName")) {
                                 xml->GetValue(structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                               structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                                 FormatText(structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                            kTRUE);
                                 // output
                                 if (makeOutput) cout<<"         "
                                                     <<structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]].Data()
                                                     <<endl;
                              }
                              // field type
                              if (type == 1 && !strcmp(name,"BankFieldType")) {
                                 xml->GetValue(structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                               structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                                 FormatText(structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                            kTRUE);
                              }
                              // field size
                              if (type == 1 && !strcmp(name,"BankFieldSize")) {
                                 xml->GetValue(structFieldSize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                               structFieldSize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                                 FormatText(structFieldSize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                            kTRUE);
                              }
                              // bank field array size
                              if (type == 1 && !strcmp(name,"BankFieldArraySize")) {
                                 xml->GetValue(bankFieldArraySize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                               bankFieldArraySize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]);
                                 FormatText(bankFieldArraySize[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]],
                                            kTRUE);
                              }
                              // field end
                              if (type == 15 && !strcmp(name,"BankField"))
                                 break;
                           }
                           // input check
                           if (structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]
                               == "") {
                              cout<<"A field of the midas bank '"<<bankName[numOfEvent][numOfBank[numOfEvent]].Data()
                                  <<"' has no name !"<<endl;
                              cout<<"Terminating program."<<endl;
                              return false;
                           }
                           if (structFieldType[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]]
                               == "") {
                              cout<<"Structure field '"
                                  <<structFieldName[numOfEvent][numOfBank[numOfEvent]][numOfStructFields[numOfEvent][numOfBank[numOfEvent]]].Data()
                                  <<"' of bank '"<<bankName[numOfEvent][numOfBank[numOfEvent]].Data()
                                  <<"' has no type !"<<endl;
                              cout<<"Terminating program."<<endl;
                              return false;
                           }
                           for (i = 0; i < numOfStructFields[numOfEvent][numOfBank[numOfEvent]]; i++) {
                              for (j = i + 1; j < numOfStructFields[numOfEvent][numOfBank[numOfEvent]]; j++) {
                                 if (structFieldName[numOfEvent][numOfBank[numOfEvent]][i]
                                     == structFieldName[numOfEvent][numOfBank[numOfEvent]][j]) {
                                    cout<<"\nStructure of bank '"<<bankName[numOfEvent][numOfBank[numOfEvent]].Data()
                                        <<"' has two fields with the name '"
                                        <<structFieldName[numOfEvent][numOfBank[numOfEvent]][i].Data()<<"' !"<<endl;
                                    cout<<"Terminating program."<<endl;
                                    return false;
                                 }
                              }
                           }
                           // count structured bank fields
                           numOfStructFields[numOfEvent][numOfBank[numOfEvent]]++;
                           if (numOfStructFields[numOfEvent][numOfBank[numOfEvent]] >= maxNumberOfStructFields) {
                              cout<<"Maximal number of fields in bank '"<<bankName[numOfEvent][numOfBank[numOfEvent]].Data()<<"' reached : "<<maxNumberOfStructFields<<" !"<<endl;
                              cout<<"Terminating program."<<endl;
                              return false;
                           }
                        }
                        // bank array
                        if (type == 1 && !strcmp(name,"BankArray")) {
                           while (xml->NextLine()) {
                              type = xml->GetType();
                              name = xml->GetName();
                              // lower boundary
                              if (type == 1 && !strcmp(name,"BASize")) {
                                 xml->GetValue(tmp,"");
                                 FormatText(tmp, kTRUE);
                                 bankArraySize[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // upper boundary
                              if (type == 1 && !strcmp(name,"BAStartIndex")) {
                                 xml->GetValue(tmp,"");
                                 FormatText(tmp, kTRUE);
                                 bankArrayStart[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              // number of digits
                              if (type == 1 && !strcmp(name,"BANumberOfDigits")) {
                                 xml->GetValue(tmp,"");
                                 FormatText(tmp, kTRUE);
                                 bankArrayDigit[numOfEvent][numOfBank[numOfEvent]] = strtol(tmp,&cstop,10);
                              }
                              if (type == 15 && !strcmp(name,"BankArray"))
                                 break;
                           }
                        }
                        // structured bank end
                        if (type == 15 && !strcmp(name,"StructuredBank"))
                           break;
                     }
                     // input check
                     if (bankName[numOfEvent][numOfBank[numOfEvent]] == "") {
                        cout<<"A structured midas bank has no Name !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                  }
                  // banks end
                  if (type == 15 && !strcmp(name,"Banks"))
                     break;
               }
            }
            // event end
            if (type == 15 && !strcmp(name,"EventDefinition")) {
               numOfBank[numOfEvent]++;
               if (eventName[numOfEvent] == "") {
                  cout<<"Midas event definition without a name !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               break;
            }
         }
      }
      // midas bank end
      if (type == 15 && !strcmp(name,"MidasBanks")) {
         numOfEvent++;
         break;
      }
   }
   for (i = 0; i < numOfEvent; i++) {
      for (j = 0; j < numOfBank[i]; j++) {
         for (k = 0; k < numOfEvent; k++) {
            for (kk = 0; kk< numOfBank[k]; kk++) {
               if (bankName[i][j] == bankName[k][kk] && (i != k || j != kk)) {
                  cout<<"\nMidas bank '"<<bankName[i][j].Data()<<"' is defined twice !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
         }
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLRootDAQ()
{
   const char *name;
   int type,i,k;
   ROMEString tmp;

   // output
   if (makeOutput) cout<<"\n\nRootDAQ:"<<endl;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp(name,"RootTree")) {
         numOfRootTree++;
         if (numOfRootTree >= maxNumberOfRootTrees) {
            cout<<"Maximal number of root trees in the RootDAQ reached : "<<maxNumberOfRootTrees<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // tree initialisation
         rootTreeName[numOfRootTree] = "";
         numOfRootBranch[numOfRootTree] = -1;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // tree name
            if (type == 1 && !strcmp(name,"RootTreeName")) {
               xml->GetValue(rootTreeName[numOfRootTree],rootTreeName[numOfRootTree]);
               FormatText(rootTreeName[numOfRootTree], kTRUE, " ");
               // output
               if (makeOutput) cout<<"   "<<rootTreeName[numOfRootTree].Data()<<endl;
            }
            if (type == 1 && !strcmp(name,"RootBranch")) {
               // count branches
               numOfRootBranch[numOfRootTree]++;
               if (numOfRootBranch[numOfRootTree] >= maxNumberOfRootBranches) {
                  cout<<"Maximal number of branches in a root tree of a RootDAQ reached : "
                      <<maxNumberOfRootBranches<<" !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               // branch initialisation
               numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]] = -1;
               rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]] = "";
               rootBranchType[numOfRootTree][numOfRootBranch[numOfRootTree]] = "";
               rootBranchArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]] = "";
               rootBranchClassName[numOfRootTree][numOfRootBranch[numOfRootTree]] = "";
               rootBranchClassVersion[numOfRootTree][numOfRootBranch[numOfRootTree]] = "0";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // branch name
                  if (type == 1 && !strcmp(name,"RootBranchName")) {
                     xml->GetValue(rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]]
                                   ,rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]]);
                     FormatText(rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]], kTRUE, " ");
                     // output
                     if (makeOutput) cout<<"      "
                                         <<rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]].Data()<<endl;
                  }
                  // branch type
                  if (type == 1 && !strcmp(name,"RootBranchType")) {
                     xml->GetValue(rootBranchType[numOfRootTree][numOfRootBranch[numOfRootTree]]
                                   ,rootBranchType[numOfRootTree][numOfRootBranch[numOfRootTree]]);
                     FormatText(rootBranchType[numOfRootTree][numOfRootBranch[numOfRootTree]], kTRUE);
                  }
                  // branch array size
                  if (type == 1 && !strcmp(name,"RootBranchArraySize")) {
                     xml->GetValue(rootBranchArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]]
                                   ,rootBranchArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]]);
                     FormatText(rootBranchArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]], kTRUE);
                     if (rootBranchArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]].ToInteger() <= 1)
                        rootBranchArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]] = "";
                  }
                  // branch class name
                  if (type == 1 && !strcmp(name,"RootBranchClassName")) {
                     xml->GetValue(rootBranchClassName[numOfRootTree][numOfRootBranch[numOfRootTree]]
                                   ,rootBranchClassName[numOfRootTree][numOfRootBranch[numOfRootTree]]);
                     FormatText(rootBranchClassName[numOfRootTree][numOfRootBranch[numOfRootTree]], kTRUE);
                  }
                  // branch class version
                  if (type == 1 && !strcmp(name,"RootBranchClassVersion")) {
                     xml->GetValue(rootBranchClassVersion[numOfRootTree][numOfRootBranch[numOfRootTree]]
                                   ,rootBranchClassVersion[numOfRootTree][numOfRootBranch[numOfRootTree]]);
                     FormatText(rootBranchClassVersion[numOfRootTree][numOfRootBranch[numOfRootTree]], kTRUE);
                  }
                  // branch class field
                  if (type == 1 && !strcmp(name,"RootBranchClassField")) {
                     // count branch fields
                     numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]++;
                     if (numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]] >=
                         maxNumberOfRootBranchFields) {
                        cout<<"Maximal number of fields in a class of a branch in a root tree of a RootDAQ reached : "
                            <<maxNumberOfRootBranchFields<<" !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                     // branch field initialisation
                     rootBranchFieldName[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]
                           = "";
                     rootBranchFieldType[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]
                           = "";
                     rootBranchFieldArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]
                           = "";
                     while (xml->NextLine()) {
                        type = xml->GetType();
                        name = xml->GetName();
                        // branch field name
                        if (type == 1 && !strcmp(name,"RootBranchClassFieldName")) {
                           xml->GetValue(rootBranchFieldName[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]],
                                         rootBranchFieldName[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]);
                           FormatText(rootBranchFieldName[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]],
                                      kTRUE);
                           // output
                           if (makeOutput) cout<<"      "
                                               <<rootBranchFieldName[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]].Data()
                                               <<endl;
                        }
                        // branch field type
                        if (type == 1 && !strcmp(name,"RootBranchClassFieldType")) {
                           xml->GetValue(rootBranchFieldType[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]],
                                         rootBranchFieldType[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]);
                           FormatText(rootBranchFieldType[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]],
                                      kTRUE);
                        }
                        // branch field array size
                        if (type == 1 && !strcmp(name,"RootBranchClassFieldArraySize")) {
                           xml->GetValue(rootBranchFieldArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]],
                                         rootBranchFieldArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]);
                           FormatText(rootBranchFieldArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]],
                                      kTRUE);
                           if (rootBranchFieldArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]].ToInteger() <= 1)
                              rootBranchFieldArraySize[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]
                                    = "";
                        }
                        // branch end
                        if (type == 15 && !strcmp(name,"RootBranchClassField"))
                           break;
                     }
                     // input check
                     if (rootBranchFieldName[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]
                         == "") {
                        cout<<"A field of a class of a branch of the Root DAQ has no Name !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                     if (rootBranchFieldType[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]]
                         == "") {
                        cout<<"Field '"
                            <<rootBranchFieldName[numOfRootTree][numOfRootBranch[numOfRootTree]][numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]].Data()
                            <<"' of branch '"<<rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]].Data()
                            <<"' of the Root DAQ has no type !"<<endl;
                        cout<<"Terminating program."<<endl;
                        return false;
                     }
                  }
                  // branch end
                  if (type == 15 && !strcmp(name,"RootBranch")) {
                     if (!rootBranchType[numOfRootTree][numOfRootBranch[numOfRootTree]].CompareTo("Class",
                                                                                                  TString::kIgnoreCase)) {
                        if (rootBranchClassName[numOfRootTree][numOfRootBranch[numOfRootTree]].Length() == 0) {
                           cout<<"Class name of Branch '"
                               <<rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]].Data()
                               <<"' not specified !"<<endl;
                           cout<<"Terminating program."<<endl;
                           return false;
                        }
                     }
                     numOfRootBranchField[numOfRootTree][numOfRootBranch[numOfRootTree]]++;
                     break;
                  }
               }
               // input check
               if (rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]] == "") {
                  cout<<"A branch of the Root DAQ has no Name !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (rootBranchType[numOfRootTree][numOfRootBranch[numOfRootTree]] == "") {
                  cout<<"Branch '"<<rootBranchName[numOfRootTree][numOfRootBranch[numOfRootTree]].Data()
                      <<"' of the Root DAQ has no type !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
            }
            // tree end
            if (type == 15 && !strcmp(name,"RootTree")) {
               numOfRootBranch[numOfRootTree]++;
               if (rootTreeName[numOfRootTree] == "") {
                  cout<<"Tree of the Root DAQ without a name !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               break;
            }
         }
      }
      // root DAQ end
      if (type == 15 && !strcmp(name,"RootDAQ")) {
         numOfRootTree++;
         break;
      }
   }
   for (i = 0; i < numOfRootTree; i++) {
      for (k = i + 1; k < numOfRootTree; k++) {
         if (rootTreeName[i] == rootTreeName[k]) {
            cout<<"\nTree '"<<rootTreeName[k].Data()<<"' of the Root DAQ is defined twice !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLSteering(Int_t iTask,Bool_t gsp)
{
   // read the steering parameter definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i,j;
   ROMEString currentSteeringName = "";
   int currentNumberOfSteerings;
   bool readFieldAffiliation;

   // count steering parameters
   numOfSteering[iTask]++;
   currentNumberOfSteerings = numOfSteering[iTask];
   if (numOfSteering[iTask] >= maxNumberOfSteering) {
      cout<<"Maximal number of steering parameters reached : "<<maxNumberOfSteering<<" !"<<endl;
      cout<<"Terminating program."<<endl;
      return false;
   }
   // initialisation
   if (numOfSteering[iTask] > 0) {
      steerName[iTask][numOfSteering[iTask]] = "";
      steerArraySize[iTask][numOfSteering[iTask]] = "1";
   }
   numOfSteerFields[iTask][numOfSteering[iTask]] = 0;
   numOfSteerChildren[iTask][numOfSteering[iTask]] = 0;
   numOfSteerAffiliations[iTask][numOfSteering[iTask]] = 0;
   if (gsp)
      numOfGSPInclude = 0;
   steerUsed[iTask][numOfSteering[iTask]] = true;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // end
      if (type == 15 && !strcmp(name,"SteeringParameters"))
         return true;
      if (type == 15 && !strcmp(name,"GlobalSteeringParameters"))
         return true;
      // gsp include
      if (gsp) {
         if (type == 1 && !strcmp(name,"Include")) { // there is no check if under GSP. We should add the check.
            // include initialisation
            gspInclude[numOfGSPInclude] = "";
            gspLocalFlag[numOfGSPInclude] = false;
            while (xml->NextLine()) {
               type = xml->GetType();
               name = xml->GetName();
               // include name
               if (type == 1 && !strcmp(name,"IncludeName")) {
                  xml->GetValue(gspInclude[numOfGSPInclude],gspInclude[numOfGSPInclude]);
                  FormatText(gspInclude[numOfGSPInclude], kTRUE);
               }
               // include type
               if (type == 1 && !strcmp(name,"IncludeType")) {
                  xml->GetValue(tmp,"false");
                  FormatText(tmp, kTRUE);
                  if (tmp == "local")
                     gspLocalFlag[numOfGSPInclude] = true;
               }
               // include end
               if (type == 15 && !strcmp(name,"Include")) {
                  break;
               }
            }
            // check input
            if (gspInclude[numOfGSPInclude] == "") {
               cout<<"An Include of global steering parameter  has no Name !"<<endl;
               cout<<"Terminating program."<<endl;
               return false;
            }
            // count includes
            numOfGSPInclude++;
            if (numOfGSPInclude >= maxNumberOfInclude) {
               cout<<"Maximal number of includes in global steering parameter reached : "<<maxNumberOfInclude<<" !"
                   <<endl;
               cout<<"Terminating program."<<endl;
               return false;
            }
            continue;
         }
      }
      // subgroup
      if (type == 1 && !strcmp(name,"SteeringParameterGroup")) {
         // set steering parameter group as parent for subsequent groups
         steerParent[iTask][numOfSteering[iTask] + 1] = actualSteerIndex;
         actualSteerIndex = numOfSteering[iTask] + 1;
         recursiveSteerDepth++;
         // read subgroup
         if (!ReadXMLSteering(iTask,false))
            return false;
         continue;
      }
      // end group
      if (type == 15 && !strcmp(name,"SteeringParameterGroup")) {
         // check input
         if (currentSteeringName == "") {
            cout<<"The "<<(currentNumberOfSteerings + 1)<<". Steering Parameter Group has no name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         if (affiliations.GetEntriesFast() > 0) {
            steerUsed[iTask][actualSteerIndex] = false;
            for (i = 0; i < affiliations.GetEntriesFast() && !steerUsed[iTask][actualSteerIndex]; i++) {
               for (j = 0; j < numOfSteerAffiliations[iTask][actualSteerIndex]; j++) {
                  if (affiliations.At(i) == steerAffiliation[iTask][actualSteerIndex][j]) {
                     steerUsed[iTask][actualSteerIndex] = true;
                     break;
                  }
               }
            }
         }
         actualSteerIndex = steerParent[iTask][actualSteerIndex];
         recursiveSteerDepth--;
         return true;
      }
      // group name
      if (type == 1 && !strcmp(name,"SPGroupName")) {
         xml->GetValue(steerName[iTask][actualSteerIndex],steerName[iTask][actualSteerIndex]);
         FormatText(steerName[iTask][actualSteerIndex], kTRUE);
         currentSteeringName = steerName[iTask][actualSteerIndex];
         if (steerParent[iTask][numOfSteering[iTask]] !=  -1) {
            steerChildren[iTask][steerParent[iTask][actualSteerIndex]][numOfSteerChildren[iTask][steerParent[iTask][actualSteerIndex]]]
                  = actualSteerIndex;
            numOfSteerChildren[iTask][steerParent[iTask][actualSteerIndex]]++;
         }
         // output
         if (makeOutput) for (i = 0; i < recursiveSteerDepth; i++) cout<<"   ";
         if (makeOutput) steerName[iTask][actualSteerIndex].WriteLine();
      }
      // group affiliation
      if (type == 1 && !strcmp(name,"Affiliation")) {
         steerAffiliation[iTask][actualSteerIndex][numOfSteerAffiliations[iTask][actualSteerIndex]] = "";
         xml->GetValue(steerAffiliation[iTask][actualSteerIndex][numOfSteerAffiliations[iTask][actualSteerIndex]]
                       ,steerAffiliation[iTask][actualSteerIndex][numOfSteerAffiliations[iTask][actualSteerIndex]]);
         FormatText(steerAffiliation[iTask][actualSteerIndex][numOfSteerAffiliations[iTask][actualSteerIndex]], kTRUE,
                    " ");
         for (i = 0; i < affiliationList.GetEntries(); i++) {
            if (affiliationList.At(i,0)
                == steerAffiliation[iTask][actualSteerIndex][numOfSteerAffiliations[iTask][actualSteerIndex]])
               break;
         }
         affiliationList.SetAt(steerAffiliation[iTask][actualSteerIndex][numOfSteerAffiliations[iTask][actualSteerIndex]],
                               i, 0);
         affiliationList.SetAt("SteerGroup", i, affiliationList.GetEntriesAt(i));
         affiliationList.SetAt(steerName[iTask][actualSteerIndex], i, affiliationList.GetEntriesAt(i));
         numOfSteerAffiliations[iTask][actualSteerIndex]++;
      }
      // group array size
      if (type == 1 && !strcmp(name,"SPGroupArraySize")) {
         xml->GetValue(steerArraySize[iTask][actualSteerIndex],steerArraySize[iTask][actualSteerIndex]);
         FormatText(steerArraySize[iTask][actualSteerIndex], kTRUE);
      }
      // steering parameter field
      if (type == 1 && !strcmp(name,"SteeringParameterField")) {
         // include initialisation
         bool readName = false;
         bool readType = false;
         readFieldAffiliation = false;
         steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldFormat[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldArraySize[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "1";
         steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldShortDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         steerFieldCLDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "";
         numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = 0;
         steerFieldUsed[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = true;
         steerFieldHotLink[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = false;
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // steering parameter field name
            if (type == 1 && !strcmp(name,"SPFieldName")) {
               readName = true;
               xml->GetValue(steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                             ,steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]], kTRUE);
               // output
               if (makeOutput) for (i = 0; i < recursiveSteerDepth + 1; i++) cout<<"   ";
               if (makeOutput) steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].WriteLine();
            }
            // steering parameter field affiliation
            if (type == 1 && !strcmp(name,"Affiliation")) {
               readFieldAffiliation = true;
               steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]]
                     = "";
               xml->GetValue(steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]],
                             steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]]);
               FormatText(steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]],
                          kTRUE, " ");
               for (i = 0; i < affiliationList.GetEntries(); i++) {
                  if (affiliationList.At(i,0)
                      == steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]])
                     break;
               }
               affiliationList.SetAt(steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]],
                                     i, 0);
               affiliationList.SetAt("SteerField", i, affiliationList.GetEntriesAt(i));
               affiliationList.SetAt(steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                                     i, affiliationList.GetEntriesAt(i));
               numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]++;
            }

            // steering parameter field type
            if (type == 1 && !strcmp(name,"SPFieldType")) {
               readType = true;
               xml->GetValue(steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                             ,steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]], kTRUE);
               if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "TString" ||
                   steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                   == "ROMEString") {
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "' '";
               } else if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                          == "std::string") {
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "\" \"";
               } else if (steerFieldType[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                          == "TRef") {
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "NULL";
               } else {
                  steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = "0";
               }
            }
            // steering parameter field format
            if (type == 1 && !strcmp(name,"SPFieldFormat")) {
               xml->GetValue(steerFieldFormat[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                             ,steerFieldFormat[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldFormat[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]], kTRUE);
            }
            // steering parameter field array size
            if (type == 1 && !strcmp(name,"SPFieldArraySize")) {
               xml->GetValue(steerFieldArraySize[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                             ,steerFieldArraySize[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldArraySize[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                          kTRUE);
            }
            // steering parameter field initialization
            if (type == 1 && !strcmp(name,"SPFieldInitialization")) {
               if (!readName) {
                  cout<<"Please specify a steering parameter field name befor the initial value in the "
                      <<(numOfSteering[iTask] + 1)<<". steering parameter field in task '"<<taskName[iTask].Data()
                      <<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (!readType) {
                  cout<<"Please specify a steering parameter field type befor the initial value in steering parameter field '"
                      <<steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]].Data()
                      <<"' in task  '"<<taskName[iTask].Data()<<"' !"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               xml->GetValue(steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                             ,steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldInit[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]], kTRUE);
            }
            // steering parameter field comment
            if (type == 1 && !strcmp(name,"SPFieldComment")) {
               xml->GetValue(steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                             ,steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                          kFALSE);
            }
            // steering parameter field comment
            if (type == 1 && !strcmp(name,"SPFieldShortDescription")) {
               xml->GetValue(steerFieldShortDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                             steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldShortDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                          kTRUE);
            }
            // steering parameter command line option
            if (type == 1 && !strcmp(name,"SPFieldCommandLineOption")) {
               xml->GetValue(steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                             ,steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                          kTRUE);
               ROMEString cloption = cloSeparator +
                     steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] +
                     cloSeparator;
               if ( usedCLO.Contains(cloption) ) {
                  cout<<"Command line option \""
                      <<steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                      <<"\" is reserved by ROME or already used. Please use other word." <<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               usedCLO += cloption;
            }
            // steering parameter command line option description
            if (type == 1 && !strcmp(name,"SPFieldCommandLineDescription")) {
               xml->GetValue(steerFieldCLDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                             steerFieldCLDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]);
               FormatText(steerFieldCLDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]],
                          kFALSE);
            }
            // steering parameter hot link
            if (type == 1 && !strcmp(name,"SPFieldHotLink")) {
               xml->GetValue(tmp,"false");
               FormatText(tmp, kTRUE);
               if (tmp == "true") {
                  steerFieldHotLink[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = true;
                  haveSteerFieldHotLinks = true;
               }
            }
            // steering parameter field end
            if (type == 15 && !strcmp(name,"SteeringParameterField")) {
               // description
               if (steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].Length() &&
                   !steerFieldShortDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].Length())
                  steerFieldShortDescription[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                        = steerFieldComment[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]];
               if (steerArraySize[iTask][actualSteerIndex].Length() && steerArraySize[iTask][actualSteerIndex]
                   != "1" &&
                   steerFieldCLOption[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]].Length()) {
                  cout<<"Error. Command line option of field '"
                      << steerFieldName[iTask][numOfSteering[iTask]][numOfSteerFields[iTask][numOfSteering[iTask]]]
                      <<"' in arrayed steering parameter group '"<<steerName[iTask][actualSteerIndex]
                      <<"' is not supported!"<<endl;
                  cout<<"Terminating program."<<endl;
                  return false;
               }
               if (!readFieldAffiliation) {
                  numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]]
                     = numOfSteerAffiliations[iTask][numOfSteering[iTask]];
                  for (i = 0;
                       i < numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]];
                       i++) {
                     steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][i]
                        = steerAffiliation[iTask][actualSteerIndex][i];
                  }
               }
               if (affiliations.GetEntriesFast() > 0) {
                  steerFieldUsed[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = false;
                  for (i = 0; i < affiliations.GetEntriesFast() &&
                             !steerFieldUsed[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]];
                       i++) {
                     for (j = 0;
                          j < numOfSteerFieldAffiliations[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]];
                          j++) {
                        if (affiliations.At(i)
                            == steerFieldAffiliation[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]][j]) {
                           steerFieldUsed[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] = true;
                           break;
                        }
                     }
                  }
               }
               break;
            }
         }
         // check input
         if (steerFieldName[iTask][actualSteerIndex][numOfSteerFields[iTask][actualSteerIndex]] == "") {
            cout<<"A steering parameter field of task '"<<taskName[iTask].Data()<<"' has no Name !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         // count includes
         numOfSteerFields[iTask][actualSteerIndex]++;
         if (numOfSteerFields[iTask][actualSteerIndex] >= maxNumberOfSteeringField) {
            cout<<"Maximal number of steering parameter fields in task '"<<taskName[iTask].Data()<<"' reached : "
                <<maxNumberOfSteeringField<<" !"<<endl;
            cout<<"Terminating program."<<endl;
            return false;
         }
         continue;
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::ReadXMLUserMakefile()
{
   int i,j;
   const char *name;
   int type;
   ROMEString temp;
   ROMEString ext;
   bool found;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      if (type == 1 && !strcmp(name,"DictionaryHeaders")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // header
            if (type == 1 && !strcmp(name,"Header")) {
               numOfMFDictHeaderAffiliations[numOfMFDictHeaders] = 0;
               mfDictHeaderName[numOfMFDictHeaders] = "";
               mfDictHeaderUsed[numOfMFDictHeaders] = true;
               mfDictHeaderLinkDefSuffix[numOfMFDictHeaders] = "+";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // header name
                  if (type == 1 && !strcmp(name,"HeaderName")) {
                     xml->GetValue(mfDictHeaderName[numOfMFDictHeaders],mfDictHeaderName[numOfMFDictHeaders]);
                     FormatText(mfDictHeaderName[numOfMFDictHeaders], kTRUE);
                     if (mfDictHeaderName[numOfMFDictHeaders].Length() > 0) {
                        if (mfDictHeaderName[numOfMFDictHeaders].Index(".") == -1)
                           mfDictHeaderName[numOfMFDictHeaders].Append(".h");
                     }
                  }
                  // affiliation
                  if (type == 1 && !strcmp(name,"Affiliation")) {
                     mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]]
                           = "";
                     xml->GetValue(mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]],
                                   mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]]);
                     FormatText(mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]],
                                kTRUE, " ");

                     for (i = 0; i < affiliationList.GetEntries(); i++) {
                        if (affiliationList.At(i,0)
                            == mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]])
                           break;
                     }
                     affiliationList.SetAt(mfDictHeaderAffiliation[numOfMFDictHeaders][numOfMFDictHeaderAffiliations[numOfMFDictHeaders]],
                                           i, 0);
                     affiliationList.SetAt("Class", i, affiliationList.GetEntriesAt(i));
                     affiliationList.SetAt(mfDictHeaderName[numOfMFDictHeaders], i, affiliationList.GetEntriesAt(i));
                     numOfMFDictHeaderAffiliations[numOfMFDictHeaders]++;
                  }
                  // linkDef suffix
                  if (type == 1 && !strcmp(name,"LinkDefSuffix")) {
                     xml->GetValue(mfDictHeaderLinkDefSuffix[numOfMFDictHeaders],
                                   mfDictHeaderLinkDefSuffix[numOfMFDictHeaders]);
                     FormatText(mfDictHeaderLinkDefSuffix[numOfMFDictHeaders], kTRUE);
                  }
                  if (type == 15 && !strcmp(name,"Header")) {
                     if (affiliations.GetEntriesFast() > 0) {
                        mfDictHeaderUsed[numOfMFDictHeaders] = false;
                        for (i = 0; i < affiliations.GetEntriesFast() && !mfDictHeaderUsed[numOfMFDictHeaders]; i++) {
                           for (j = 0; j < numOfMFDictHeaderAffiliations[numOfMFDictHeaders]; j++) {
                              if (affiliations.At(i) == mfDictHeaderAffiliation[numOfMFDictHeaders][j]) {
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
            if (type == 15 && !strcmp(name,"DictionaryHeaders"))
               break;
         }
      }
      if (type == 1 && !strcmp(name,"DictionaryIncludesDirectories")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include directory
            if (type == 1 && !strcmp(name,"IncludeDirectory")) {
               mfDictIncDir[numOfMFDictIncDirs] = "";
               xml->GetValue(mfDictIncDir[numOfMFDictIncDirs],mfDictIncDir[numOfMFDictIncDirs]);
               FormatText(mfDictIncDir[numOfMFDictIncDirs], kTRUE);
               if (mfDictIncDir[numOfMFDictIncDirs].Length() > 0) {
                  if (mfDictIncDir[numOfMFDictIncDirs][mfDictIncDir[numOfMFDictIncDirs].Length()-1] != '/' &&
                     mfDictIncDir[numOfMFDictIncDirs][mfDictIncDir[numOfMFDictIncDirs].Length()-1] != '\\')
                     mfDictIncDir[numOfMFDictIncDirs].Append("/");
                  numOfMFDictIncDirs++;
               }
            }
            if (type == 15 && !strcmp(name,"DictionaryIncludesDirectories"))
               break;
         }
      }
      if (type == 1 && !strcmp(name,"WindowsLibraries")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp(name,"Library")) {
               mfWinLibName[numOfMFWinLibs] = "";
               numOfMFWinLibFlags[numOfMFWinLibs] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // library name
                  if (type == 1 && !strcmp(name,"LibraryName")) {
                     xml->GetValue(mfWinLibName[numOfMFWinLibs],mfWinLibName[numOfMFWinLibs]);
                     FormatText(mfWinLibName[numOfMFWinLibs], kTRUE);
                     if (mfWinLibName[numOfMFWinLibs].Length() > 0) {
                        if (mfWinLibName[numOfMFWinLibs].Index(".") == -1)
                           mfWinLibName[numOfMFWinLibs].Append(".lib");
                     }
                  }
                  // flags
                  if (type == 1 && !strcmp(name,"NeededFlag")) {
                     mfWinLibFlag[numOfMFWinLibs][numOfMFWinLibFlags[numOfMFWinLibs]] = "";
                     xml->GetValue(mfWinLibFlag[numOfMFWinLibs][numOfMFWinLibFlags[numOfMFWinLibs]]
                                   ,mfWinLibFlag[numOfMFWinLibs][numOfMFWinLibFlags[numOfMFWinLibs]]);
                     FormatText(mfWinLibFlag[numOfMFWinLibs][numOfMFWinLibFlags[numOfMFWinLibs]], kTRUE);
                     numOfMFWinLibFlags[numOfMFWinLibs]++;
                  }
                  if (type == 15 && !strcmp(name,"Library")) {
                     numOfMFWinLibs++;
                     break;
                  }
               }
            }
            if (type == 15 && !strcmp(name,"WindowsLibraries"))
               break;
         }
      }
      if (type == 1 && !strcmp(name,"UnixLibraries")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            if (type == 1 && !strcmp(name,"Library")) {
               mfUnixLibName[numOfMFUnixLibs] = "";
               numOfMFUnixLibFlags[numOfMFUnixLibs] = 0;
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // library name
                  if (type == 1 && !strcmp(name,"LibraryName")) {
                     xml->GetValue(mfUnixLibName[numOfMFUnixLibs],mfUnixLibName[numOfMFUnixLibs]);
                     FormatText(mfUnixLibName[numOfMFUnixLibs], kTRUE);
                  }
                  // flags
                  if (type == 1 && !strcmp(name,"NeededFlag")) {
                     mfUnixLibFlag[numOfMFUnixLibs][numOfMFUnixLibFlags[numOfMFUnixLibs]] = "";
                     xml->GetValue(mfUnixLibFlag[numOfMFUnixLibs][numOfMFUnixLibFlags[numOfMFUnixLibs]]
                                   ,mfUnixLibFlag[numOfMFUnixLibs][numOfMFUnixLibFlags[numOfMFUnixLibs]]);
                     FormatText(mfUnixLibFlag[numOfMFUnixLibs][numOfMFUnixLibFlags[numOfMFUnixLibs]], kTRUE);
                     numOfMFUnixLibFlags[numOfMFUnixLibs]++;
                  }
                  if (type == 15 && !strcmp(name,"Library")) {
                     numOfMFUnixLibs++;
                     break;
                  }
               }
            }
            if (type == 15 && !strcmp(name,"UnixLibraries"))
               break;
         }
      }
      if (type == 1 && !strcmp(name,"IncludeDirectories")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // include directory
            if (type == 1 && !strcmp(name,"IncludeDirectory")) {
               mfIncDir[numOfMFIncDirs] = "";
               xml->GetValue(mfIncDir[numOfMFIncDirs],mfIncDir[numOfMFIncDirs]);
               FormatText(mfIncDir[numOfMFIncDirs], kTRUE);
               if (mfIncDir[numOfMFIncDirs].Length() > 0) {
                  if (mfIncDir[numOfMFIncDirs][mfIncDir[numOfMFIncDirs].Length()-1] != '/' &&
                     mfIncDir[numOfMFIncDirs][mfIncDir[numOfMFIncDirs].Length()-1] != '\\')
                     mfIncDir[numOfMFIncDirs].Append("/");
                  numOfMFIncDirs++;
               }
            }
            if (type == 15 && !strcmp(name,"IncludeDirectories"))
               break;
         }
      }
      if (type == 1 && !strcmp(name,"PreprocessorDefinition")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // name
            if (type == 1 && !strcmp(name,"Name")) {
               mfPreDefName[numOfMFPreDefs] = "";
               xml->GetValue(mfPreDefName[numOfMFPreDefs],mfPreDefName[numOfMFPreDefs]);
               FormatText(mfPreDefName[numOfMFPreDefs], kTRUE, " ");
               if (mfPreDefName[numOfMFPreDefs].Length() > 0)
                  numOfMFPreDefs++;
            }
            if (type == 15 && !strcmp(name,"PreprocessorDefinition"))
               break;
         }
      }
      if (type == 1 && !strcmp(name,"AdditionalFiles")) {
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // source file
            if (type == 1 && !strcmp(name,"File")) {
               numOfMFSourceFlags[numOfMFSources] = 0;
               numOfMFSourceAffiliations[numOfMFSources] = 0;
               mfSourceFileUsed[numOfMFSources] = true;
               mfSourceFileName[numOfMFSources] = "";
               mfSourceFilePath[numOfMFSources] = "";
               mfSourceFileObjPath[numOfMFSources] = "obj";
               mfHeaderFileName[numOfMFSources] = "";
               mfHeaderFilePath[numOfMFSources] = "";
               while (xml->NextLine()) {
                  type = xml->GetType();
                  name = xml->GetName();
                  // source file
                  if (type == 1 && !strcmp(name,"SourceFile")) {
                     xml->GetValue(temp,"");
                     FormatText(temp, kTRUE);
                     AnalyzeFileName(temp.Data(),mfSourceFilePath[numOfMFSources],
                                     mfSourceFileName[numOfMFSources],ext);
                     if (ext.Length() == 0) {
                        mfSourceFileName[numOfMFSources].Append(".cpp");
                     } else {
                        mfSourceFileName[numOfMFSources].AppendFormatted(".%s",ext.Data());
                     }
                     if (mfSourceFilePath[numOfMFSources].Length() > 0) {
                        if (mfSourceFilePath[numOfMFSources][mfSourceFilePath[numOfMFSources].Length()-1] != '/' &&
                            mfSourceFilePath[numOfMFSources][mfSourceFilePath[numOfMFSources].Length()-1] != '\\')
                           mfSourceFilePath[numOfMFSources].Append("/");
                     }
                  }
                  // header file
                  if (type == 1 && !strcmp(name,"HeaderFile")) {
                     xml->GetValue(temp,"");
                     FormatText(temp, kTRUE);
                     AnalyzeFileName(temp.Data(),mfHeaderFilePath[numOfMFSources],
                                     mfHeaderFileName[numOfMFSources],ext);
                     if (ext.Length() == 0) {
                        mfHeaderFileName[numOfMFSources].Append(".h");
                     } else {
                        mfHeaderFileName[numOfMFSources].AppendFormatted(".%s",ext.Data());
                     }
                     if (mfHeaderFilePath[numOfMFSources].Length() > 0) {
                        if (mfHeaderFilePath[numOfMFSources][mfHeaderFilePath[numOfMFSources].Length()-1] != '/' ||
                           mfHeaderFilePath[numOfMFSources][mfHeaderFilePath[numOfMFSources].Length()-1] != '\\')
                           mfHeaderFilePath[numOfMFSources].Append("/");
                     }
                  }
#if defined( R__UNIX )
                  // object directory
                  if (type == 1 && !strcmp(name,"ObjectDirectory")) {
                     xml->GetValue(mfSourceFileObjPath[numOfMFSources],"");
                     FormatText(mfSourceFileObjPath[numOfMFSources], kTRUE);
                  }
#endif
                  // flags
                  if (type == 1 && !strcmp(name,"NeededFlag")) {
                     mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]] = "";
                     xml->GetValue(mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]]
                                   ,mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]]);
                     FormatText(mfSourceFileFlag[numOfMFSources][numOfMFSourceFlags[numOfMFSources]], kTRUE, " ");
                     numOfMFSourceFlags[numOfMFSources]++;
                  }
                  // affiliation
                  if (type == 1 && !strcmp(name,"Affiliation")) {
                     mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]] = "";
                     xml->GetValue(mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]],
                                   mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]]);
                     FormatText(mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]],
                                kTRUE, " ");
                     for (i = 0 ;i < affiliationList.GetEntries(); i++) {
                        if (affiliationList.At(i,0)
                            == mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]])
                           break;
                     }
                     affiliationList.SetAt(mfSourceFileAffiliation[numOfMFSources][numOfMFSourceAffiliations[numOfMFSources]],
                                           i, 0);
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
                  if (type == 15 && !strcmp(name,"File")) {
                     if (affiliations.GetEntriesFast() > 0) {
                        mfSourceFileUsed[numOfMFSources] = false;
                        for (i = 0; i < affiliations.GetEntriesFast() && !mfSourceFileUsed[numOfMFSources]; i++) {
                           for (j = 0; j < numOfMFSourceAffiliations[numOfMFSources]; j++) {
                              if (affiliations.At(i) == mfSourceFileAffiliation[numOfMFSources][j]) {
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
            if (type == 15 && !strcmp(name,"AdditionalFiles")) {
               for (i = 0; i < numOfMFSources; i++) {
                  if (!mfSourceFileUsed[i])
                     continue;
                  found = false;
                  for (j = 0; j < objDirList.GetEntries(); j++) {
                     if (objDirList.At(j) == mfSourceFileObjPath[i].Data()) {
                        found = true;
                     }
                  }
                  if (!found &&
                      mfSourceFileObjPath[i] != "obj" &&
                      mfSourceFileObjPath[i] != "obj/" &&
                      mfSourceFileObjPath[i] != "./obj" &&
                      mfSourceFileObjPath[i] != "./obj/") {
                     objDirList.AddLast(mfSourceFileObjPath[i]);
                  }
               }
               break;
            }
         }
      }

      if (type == 15 && !strcmp(name,"UserMakeFile")) {
         break;
      }
   }
   return true;
}

//______________________________________________________________________________
void ROMEBuilder::ParseSVNKeyword(ROMEString& str)
{
   // extract Subversion revision number from string.
   if (!str.BeginsWith("$") || !str.EndsWith(" $")) {// This isn't Subversion keyword
      return;
   }

   if (str.BeginsWith("$Rev: ")) {
      str.Remove(0, 6);
   } else if (str.BeginsWith("$Revision: ")) {
      str.Remove(0, 11);
   } else if (str.BeginsWith("$LastChangedRevision: ")){
      str.Remove(0, 22);
   } else{
      cout<<"Warning: ROMEBuilder does not support keyword '"<<str<<"'"<<endl;
      return;
   }
   str.Remove(str.Length()-2, 2);
   return;
}

//______________________________________________________________________________
void ROMEBuilder::FormatText(ROMEString& str, Bool_t stripSpace, const char* invalidCharacters)
{
   if (stripSpace)
      str.StripSpaces();

   if (invalidCharacters) {
      Int_t n = strlen(invalidCharacters);
      Int_t i;
      char t[2];
      for (i = 0; i < n; i++) {
         t[0] = invalidCharacters[i];
         t[1] = '\0';
         str.ReplaceAll(t, "");
      }
   }
}
