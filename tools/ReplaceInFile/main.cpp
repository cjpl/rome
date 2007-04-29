/********************************************************************

  main.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <fstream>
#include <iostream>
using namespace std;

#define MAX_FILE_LENGTH 500
#define MAX_STR_LENGTH 500
#define MAX_BUFFER_SIZE 60000
#define MAX_WILD_CARDS 10
#define MAX_VETO_CHAR 100

//______________________________________________________________________________
void usage() {
   cout << "  -f        File" << endl;
   cout << "  -s        String to replace" << endl;
   cout << "  -r        Replacement" << endl;
   cout << "  -v        Veto Characters in Wildcards" << endl;
}

//______________________________________________________________________________
int main(int argc, char *argv[])
{
   int i,j,k;
   char file[MAX_FILE_LENGTH];
   char str[MAX_STR_LENGTH];
   char rep[MAX_STR_LENGTH];
   char buffer[MAX_BUFFER_SIZE];
   char newBuffer[MAX_BUFFER_SIZE] = "";
   char *pb,*p;

   bool wfound;
   int nstrw,nrepw;
   char strw[MAX_WILD_CARDS][MAX_STR_LENGTH];
   char repw[MAX_WILD_CARDS][MAX_STR_LENGTH];
   char *pw[MAX_WILD_CARDS];

   char veto[MAX_VETO_CHAR];

   if (argc==1) {
      usage();
   }
   for (i=1;i<argc;i++) {
      if (!strcmp(argv[i],"-f")&&i<argc-1) {
         i++;
         strncpy(file,argv[i],MAX_FILE_LENGTH);
      }
      else if (!strcmp(argv[i],"-s")&&i<argc-1) {
         i++;
         strncpy(str,argv[i],MAX_STR_LENGTH);
      }
      else if (!strcmp(argv[i],"-r")&&i<argc-1) {
         i++;
         strncpy(rep,argv[i],MAX_STR_LENGTH);
      }
      else if (!strcmp(argv[i],"-v")&&i<argc-1) {
         i++;
         strncpy(veto,argv[i],MAX_VETO_CHAR);
      }
      else {
         usage();
         return 1;
      }
   }
   // Wildcards
   nstrw = 0;
   strncpy(strw[nstrw],str,strlen(str));
   pb = str;
   while((p = strstr(pb,"*"))!=NULL) {
      strncpy(strw[nstrw],pb,p-pb);
      strw[nstrw][p-pb] = 0;
      nstrw++;
      if (nstrw>=MAX_WILD_CARDS) {
         cout << "Error : Max. Wild-Cards exceeded (" << MAX_WILD_CARDS << ")." << endl;
         return 1;
      }
      pb = ++p;
   }
   strncpy(strw[nstrw],pb,strlen(pb));
   strw[nstrw][strlen(pb)] = 0;

   nrepw = 0;
   strncpy(repw[nrepw],rep,strlen(rep));
   pb = rep;
   while((p = strstr(pb,"*"))!=NULL) {
      strncpy(repw[nrepw],pb,p-pb);
      repw[nrepw][p-pb] = 0;
      nrepw++;
      if (nrepw>=MAX_WILD_CARDS) {
         cout << "Error : Max. Wild-Cards exceeded (" << MAX_WILD_CARDS << ")." << endl;
         return 1;
      }
      pb = ++p;
   }
   strncpy(repw[nrepw],pb,strlen(pb));
   repw[nrepw][strlen(pb)] = 0;

   if (nrepw!=nstrw) {
      cout << "Error : Number of Wild-Cards different for '-s' and '-r'." << endl;
      return 2;
   }

   // Read file
   fstream *fileStream;
   if((fileStream = new fstream(file,ios::in))){
      fileStream->get(buffer,MAX_BUFFER_SIZE,0);
      delete fileStream;
   }
   // Replace
   pb = buffer;
   while((pw[0] = strstr(pb,strw[0]))!=NULL) {
      wfound = true;
      for (i=1;i<=nstrw && wfound;i++) {
         if ((pw[i] = strstr(pw[i-1],strw[i]))==NULL) {
            wfound = false;
            break;
         }
         for (j=0;j<strlen(veto);j++) {
            for (k=0;k<pw[i]-(pw[i-1]+strlen(strw[i-1]));k++){
               if (pw[i-1][strlen(strw[i-1])+k]==veto[j]) {
                  wfound = false;
                  break;
               }
            }
         }
      }
      if (!wfound) {
         strncat(newBuffer,pb,pw[0]+1-pb);
         pb = pw[0]+1;
         continue;
      }
      for (i=0;i<=nstrw;i++) {
         strncat(newBuffer,pb,pw[i]-pb);
         strncat(newBuffer,repw[i],strlen(repw[i]));
         pb = pw[i]+strlen(strw[i]);
      }
   }
   strncat(newBuffer,pb,strlen(pb));
   // Write file
   if(!(fileStream = new fstream(file,ios::out | ios::trunc))) {
      cout << "Error : Could not open Output File" << endl;
      return 3;
   }
   *fileStream<<newBuffer;
   delete fileStream;

   return 0;
}
