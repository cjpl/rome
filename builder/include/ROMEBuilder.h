#ifndef ROMEBuilder_H
#define ROMEBuilder_H


const int numberOfTasks = 200;
const int numberOfFolders = 100;
const int numberOfTrees = 20;
const int numberOfBranches = 10;
const int numberOfBanks = 20;
const int authorNameLength = 50;
const int versionLength = 5;
const int nameLength = 20;
const int descriptionLength = 500;
const int commentLength = 50;
const int maxNumberOfValues = 50;
const int maxNumberOfGetters = 10;
const int maxNumberOfSetters = 10;
const int maxNumberOfInclude = 10;
const int maxNumberOfHistos = 10;
const int maxNumberOfStructFields = 50;
const int bufferLength = 100000;

class ROMEBuilder
{
public:
   char romeVersion[20];

   char outDir[descriptionLength];
   bool makeOutput;
   bool noLink;

private:
   char shortCut[nameLength];

// folders
   int numOfFolder;
   int numOfValue[numberOfFolders];
   int numOfGetters[numberOfFolders];
   int numOfSetters[numberOfFolders];

   char folderName[numberOfFolders][nameLength];
   char parentFolderName[numberOfFolders][nameLength];
   char folderTitle[numberOfFolders][commentLength];
   char folderArray[numberOfFolders][nameLength];
   bool dataBase[numberOfFolders];

   char valueName[numberOfFolders][maxNumberOfValues][nameLength];
   char valueType[numberOfFolders][maxNumberOfValues][nameLength];
   char valueRef[numberOfFolders][maxNumberOfValues][nameLength];
   char valueInit[numberOfFolders][maxNumberOfValues][nameLength];
   char valueComment[numberOfFolders][maxNumberOfValues][commentLength];

   char getter[numberOfFolders][maxNumberOfGetters][nameLength];
   char setter[numberOfFolders][maxNumberOfSetters][nameLength];
   char getterType[numberOfFolders][maxNumberOfGetters][nameLength];
   char setterType[numberOfFolders][maxNumberOfSetters][nameLength];

// task
   int numOfTask;
   int numOfHistos[numberOfTasks];
   char taskName[numberOfTasks][nameLength];
   bool fortranFlag[numberOfTasks];
   char parentTaskName[numberOfTasks][nameLength];
   char histoName[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoType[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoArray[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoTitle[numberOfTasks][maxNumberOfHistos][commentLength];
   char histoFolderName[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoFolderTitle[numberOfTasks][maxNumberOfHistos][commentLength];
   char histoXBin[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoXMin[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoXMax[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoYBin[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoYMin[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoYMax[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoZBin[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoZMin[numberOfTasks][maxNumberOfHistos][nameLength];
   char histoZMax[numberOfTasks][maxNumberOfHistos][nameLength];

// tree
   int numOfTree;
   int numOfBranch[numberOfTrees];
   char treeName[numberOfTrees][nameLength];
   char treeTitle[numberOfTrees][commentLength];
   char branchName[numberOfTrees][numberOfBranches][nameLength];
   char branchFolder[numberOfTrees][numberOfBranches][nameLength];

// banks
   int numOfBank;
   int numOfStructFields[numberOfBanks];
   char bankName[numberOfBanks][nameLength];
   char bankType[numberOfBanks][nameLength];
   char bankStructName[numberOfBanks][nameLength];
   char structFieldName[numberOfBanks][maxNumberOfStructFields][nameLength];
   char structFieldType[numberOfBanks][maxNumberOfStructFields][nameLength];
   char structFieldSize[numberOfBanks][maxNumberOfStructFields][nameLength];

// general
   int numOfInclude[numberOfTasks];
   bool overWriteCpp[numberOfTasks];
   bool overWriteH[numberOfTasks];
   char author[numberOfTasks][authorNameLength];
   char version[numberOfTasks][versionLength];
   char classDescription[numberOfTasks][descriptionLength];
   char include[numberOfTasks][maxNumberOfInclude][nameLength];
   bool localFlag[numberOfTasks][maxNumberOfInclude];

// main
   char mainAuthor[authorNameLength];
   char mainInstitute[descriptionLength];
   char mainCollaboration[descriptionLength];
   char mainEmail[descriptionLength];
   char mainProgName[nameLength];

public:
   ROMEBuilder() {};

   bool XMLToFolder(xmlTextReaderPtr reader);
   bool XMLToTask(xmlTextReaderPtr reader);
   bool XMLToTree(xmlTextReaderPtr reader);
   bool XMLToMidasBanks(xmlTextReaderPtr reader);
   bool WriteAnalyzer();
   void WriteMakefile();
   void WriteHTMLDoku();
   void WriteDictionaryBat(char* buffer1,char* buffer2);
   void startBuilder(char* xmlFile);
   void GetFormat(char *buf,char *type);
   void setValue(char *buf,char *destination,char *source,char *type);
   bool isFloatingType(char *type);
};

#endif   // ROMEBuilder_H
