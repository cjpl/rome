#ifndef ROMEString_H
#define ROMEString_H

#include<TString.h>

class ROMEString : public TString
{
public:
   ROMEString() : TString() {};
   ROMEString(int s) : TString(s) {};
   ROMEString(char c) : TString(c) {};
   ROMEString(const char* s) : TString(s) {};
   ROMEString(const TString& s) : TString(s) {};
   ROMEString(const TSubString& s) : TString(s) {};
   bool AppendFormated(char* format,...);
   bool InsertFormated(int position,char* format,...);
   bool SetFormated(char* format,...);
   int  NumberOfOccurrence(ROMEString& subString);
   int  NumberOfOccurrence(char* subString);
   void Write();
   void WriteLine();
private:
   bool FormateString(ROMEString* string,char* format,va_list parameters);
   int  SearchFormatType(char* str);
};

#endif   // ROMEString_H
