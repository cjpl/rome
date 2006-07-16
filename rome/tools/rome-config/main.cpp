/********************************************************************

  main.cpp, Ryu Sawada

  $Id$

********************************************************************/
#include <stdlib.h>
#include <Riostream.h>
#include "ROMEVersion.h"

void help()
{
   cout<<" Usage: rome-config [options]"<<endl;
   cout<<"  --version                  Print the ROME version"<<endl;
   cout<<"  --revision                 Print the ROME revision"<<endl;
   cout<<"  --revision-greater number  Print if revision is greater than number [yes/no]"<<endl;
   cout<<"  --revision-less number     Print if revision is less than number [yes/no]"<<endl;
   cout<<"  --revision-equal number    Print if revision is equal to number [yes/no]"<<endl;
   cout<<"  --release                  Print if release version [yes/no]"<<endl;
   cout<<"  --help                     Print this message"<<endl;
}

int main(int argc, char *argv[])
{
   char* cstop = 0;
   if (argc == 1) {
      help();
      return 0;
   }

   if (!strcmp(argv[1], "--version")) {
      cout<<ROME_RELEASE<<endl;
      return 0;
   }

   if (!strcmp(argv[1], "--revision")) {
      cout<<ROME_REVISION_CODE<<endl;
      return 0;
   }

   if (!strcmp(argv[1], "--revision-greater")) {
      if (argc < 3 ){
         help();
         return 0;
      }
      if (ROME_REVISION_CODE > strtol(argv[2], &cstop, 10))
         cout<<"yes"<<endl;
      else
         cout<<"no"<<endl;
      return 0;
   }

   if (!strcmp(argv[1], "--revision-less")) {
      if (argc < 3 ){
         help();
         return 0;
      }
      if (ROME_REVISION_CODE < strtol(argv[2], &cstop, 10))
         cout<<"yes"<<endl;
      else
         cout<<"no"<<endl;
      return 0;
   }

   if (!strcmp(argv[1], "--revision-equal")) {
      if (argc < 3 ){
         help();
         return 0;
      }
      if (ROME_REVISION_CODE == strtol(argv[2], &cstop, 10))
         cout<<"yes"<<endl;
      else
         cout<<"no"<<endl;
      return 0;
   }

   if (!strcmp(argv[1], "--release")) {
      if (ROME_STABLE)
         cout<<"yes"<<endl;
      else
         cout<<"no"<<endl;
      return 0;
   }

   help();
   return 0;
}
