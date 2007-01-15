// $Id$
// return 0 when stdout is not redirected.
// return 1 when stdout is redirected.
#include <Riostream.h>
#include <unistd.h>

int main(void) {
   ofstream ofile(".check_redirection");
   if(!isatty(fileno(stdout)) || !isatty(fileno(stderr)))
      ofile<<"1"<<endl;
   else
      ofile<<"0"<<endl;
   ofile.close();
   return 0;
}

