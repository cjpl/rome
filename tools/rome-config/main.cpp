/********************************************************************

  main.cpp, Ryu Sawada

  $Id$

********************************************************************/
#include <Riostream.h>
#include "ROMEVersion.h"

void help()
{
   cout<<" Usage: rome-config [options]"<<endl;
   cout<<"  --version             Print the ROME version"<<endl;
   cout<<"  --revision            Print the ROME revision"<<endl;
   cout<<"  --release             Print if release version [yes/no]"<<endl;
   cout<<"  --help                Print this message"<<endl;
}

int main(int argc, char *argv[])
{
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
