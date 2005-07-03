/********************************************************************

  main.cpp, Ryu Sawada

  $Log$
  Revision 1.2  2005/07/03 17:31:34  sawada
  Support folder.
  Multiple dimension fields in folders.

  Revision 1.1  2005/05/26 14:26:55  sawada
  Lots of changes.
  Made ArgusBuilder an inheriting class of ROMEBuilder.
  Remove ROMEFolder and added NetFolers.
  Added ArgusWindow class.
  and so on.


*******************************************************************/
#include "ArgusBuilder.h"

int main(int argc, char *argv[])
{
   struct stat buf;
   Char_t* argussys = getenv("ARGUSSYS");
   if (argussys==NULL) {
      cout << "Please set the environment variable ARGUSSYS to the Argus root-directory." << endl;
      return 1;
   }
   Char_t* romesys = getenv("ROMESYS");
   if (romesys==NULL) {
      cout << "Please set the environment variable ROMESYS to the ROME root-directory." << endl;
      return 1;
   }
   Char_t* rootsys = getenv("ROOTSYS");
   if (rootsys==NULL) {
      cout << "Please set the environment variable ROOTSYS to the ROOT root-directory." << endl;
      return 1;
   }
   ArgusBuilder* argusb = new ArgusBuilder();
   argusb->argusVersion = ARGUS_RELEASE;
   ROMEString xmlFile = "";
   const Int_t workDirLen = 1000;
   Char_t workDir[workDirLen];
   getcwd(workDir,workDirLen);
   argusb->makeOutput = kFALSE;
   argusb->noLink = kFALSE;
   argusb->midas = kFALSE;
   argusb->sql = kFALSE;
   argusb->mysql = kFALSE;
   argusb->pgsql = kFALSE;
   argusb->sqlite = kFALSE;
   argusb->sqlite3 = kFALSE;
   argusb->outDir = workDir;
   argusb->outDir.Append("/");
   if (argc==1) {
      argusb->usage();
      return 0;
   }
   for (Int_t i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-v")) {
         argusb->makeOutput = kTRUE;
      }
      else if (!strcmp(argv[i],"-nl")) {
         argusb->noLink = kTRUE;
      }
      else if (!strcmp(argv[i],"-nosql")) {
         cout<<"-nosql is obsolete. SQL support is off by default."<<endl;
      }
      else if (!strcmp(argv[i],"-mysql")) {
         argusb->mysql = kTRUE;
      }
      else if (!strcmp(argv[i],"-pgsql")) {
         argusb->pgsql = kTRUE;
      }
      else if (!strcmp(argv[i],"-sqlite")) {
         argusb->sqlite = kTRUE;
      }
      else if (!strcmp(argv[i],"-sqlite3")) {
         argusb->sqlite3 = kTRUE;
      }
      else if (!strcmp(argv[i],"-midas")) {
         argusb->midas = kTRUE;
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
         if(xmlFile(xmlFile.Length()-4,xmlFile.Length()) != ".xml")
            xmlFile+=".xml";
         i++;
      }
      else if (!strcmp(argv[i],"-o")&&i<argc-1) {
         argusb->outDir = argv[i+1];
         if (argusb->outDir[argusb->outDir.Length()-1]!='/' && argusb->outDir[argusb->outDir.Length()-1]!='\\')
            argusb->outDir.Append("/");
         i++;
      }
      else if (argv[i][0]=='-') {
         argusb->usage();
         return 0;
      }
      else {
         xmlFile = argv[i];
         if(xmlFile(xmlFile.Length()-4,xmlFile.Length()) != ".xml")
            xmlFile+=".xml";
      }
   }
   argusb->sql = (argusb->mysql ||argusb->pgsql || argusb->sqlite || argusb->sqlite3 );
   if( stat( xmlFile.Data(), &buf )) {
      if ( xmlFile == "")
         cout << "No inputfile specified." << endl;
      else
         cout << "Inputfile '" << xmlFile.Data() << "' not found." << endl;
      return 1;
   }
   ROMEString path;
   path = argusb->outDir;
   path.Remove(path.Length()-1);
   if (stat( path, &buf )) {
      cout << "Outputpath '" << argusb->outDir.Data() << "' not found." << endl;
      return 1;
   }
#if defined( R__VISUAL_CPLUSPLUS )
   path.SetFormatted("%s/src",argusb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/src/tabs",argusb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/src/framework",argusb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/include/",argusb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/include/tabs",argusb->outDir.Data());
   mkdir(path);
   path.SetFormatted("%s/include/framework",argusb->outDir.Data());
   mkdir(path);
#endif
#if defined( R__UNIX )
   path.SetFormatted("%s/src",argusb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/src/tabs",argusb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/src/framework",argusb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/include/",argusb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/include/tabs",argusb->outDir.Data());
   mkdir(path,0711);
   path.SetFormatted("%s/include/framework",argusb->outDir.Data());
   mkdir(path,0711);
#endif
   argusb->startBuilder((Char_t*)xmlFile.Data());
   return 0;
}
