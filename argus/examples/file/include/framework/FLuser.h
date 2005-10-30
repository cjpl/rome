#ifndef FLuser_H
#define FLuser_H

#include <ROMEDAQSystem.h>

class FLuser : public ROMEDAQSystem
{
protected:
public:
   FLuser();
   int   GetTimeStamp() { return 0; };
   const char* GetName() { return "user"; };

   bool Init();
   bool BeginOfRun();
   bool Event(int event);
   bool EndOfRun();
   bool Terminate();
};

#endif   // FLuser_H
