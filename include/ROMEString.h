/********************************************************************
  ROMEString.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEString_H
#define ROMEString_H

#include <TString.h>
//#include <Varargs.h>

class ROMEString : public TString
{
public:
   ROMEString() : TString() {};
   ROMEString(Ssiz_t s) : TString(s) {};
   ROMEString(char c) : TString(c) {};
   ROMEString(const char* s) : TString(s) {};
   ROMEString(const TString& s) : TString(s) {};
   ROMEString(const TSubString& s) : TString(s) {};
   ROMEString&  AppendFormatted(const char* format=NULL,...);
   ROMEString&  InsertFormatted(Ssiz_t position,const char* format=NULL,...);
   ROMEString&  SetFormatted(const char* format=NULL,...);
   static char* Format(const char *format, va_list ap);
   static char* SlowFormat(const char *format, va_list ap, int hint);
   Int_t        NumberOfOccurrence(ROMEString& subString);
   Int_t        NumberOfOccurrence(const char* subString);
   void         Write();
   void         WriteLine();
   Int_t        ToInteger();
   Double_t     ToDouble();
   Float_t      ToFloat();
   Bool_t       ToBool();
   void         ToLower();
   const char*  ToLower(ROMEString& destination);
   void         ToUpper();
   const char*  ToUpper(ROMEString& destination);
   istream&     ReadFile(istream& str);
   istream&     ReadLine(istream& str);
   ROMEString&  StripSpaces();
};

#endif   // ROMEString_H
