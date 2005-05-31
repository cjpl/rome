/********************************************************************

  main.cpp, M. Schneebeli PSI

  $Log$
  Revision 1.2  2005/05/31 06:47:49  sawada
  added unistd.h

  Revision 1.1  2005/05/17 18:45:38  sawada
  Separated main from ROMEBuilder.
  Reading function for every database folders.


********************************************************************/
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   include <direct.h>
#endif
#if defined( R__UNIX )
#   include <unistd.h>
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
#include "ROMEBuilder.h"
#include <Riostream.h>

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
   romeb->sql = false;
   romeb->mysql = false;
   romeb->pgsql = false;
   romeb->sqlite = false;
   romeb->sqlite3 = false;

   romeb->outDir = workDir;
   romeb->outDir.Append("/");

   if (argc==1) {
      romeb->usage();
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
         romeb->noLink = true;
         romeb->sql = true;
         romeb->mysql = true;
         romeb->sqlite = false;
//         romeb->sqlite3 = true;
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
         cout<<"-nosql is obsolete. SQL support is off by default."<<endl;
      }
      else if (!strcmp(argv[i],"-mysql")) {
         romeb->mysql = true;
      }
      else if (!strcmp(argv[i],"-pgsql")) {
         romeb->pgsql = true;
      }
      else if (!strcmp(argv[i],"-sqlite")) {
         romeb->sqlite = true;
      }
      else if (!strcmp(argv[i],"-sqlite3")) {
         romeb->sqlite3 = true;
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
         romeb->usage();
         return 0;
      }
      else {
         xmlFile = argv[i];
      }
   }

   romeb->sql = (romeb->mysql || romeb->pgsql || romeb->sqlite || romeb->sqlite3 );

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

   romeb->startBuilder(xmlFile.Data());

   return 0;
}
