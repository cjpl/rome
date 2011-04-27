/********************************************************************
  ROMEString.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEString_H
#define ROMEString_H

#include <TString.h>
#include <Varargs.h>
#include "ROME.h"

class ROMEString : public TString
{
public:
   ROMEString();
   ROMEString(Ssiz_t s);
   ROMEString(char c);
   ROMEString(const char* s);
   ROMEString(const TString& s);
   ROMEString(const TSubString& s);
   virtual ~ROMEString() {}

#ifndef __MAKECINT__
   ROMEString&  AppendFormatted(const char* format=NULL,...) G_GNUC_PRINTF(2, 3);
   ROMEString&  InsertFormatted(Ssiz_t position,const char* format=NULL,...) G_GNUC_PRINTF(3, 4);
   ROMEString&  SetFormatted(const char* format=NULL,...) G_GNUC_PRINTF(2, 3);
   static char* Format(const char *format, va_list ap) G_GNUC_PRINTF(1, 0);
   static char* SlowFormat(const char *format, va_list ap, int hint) G_GNUC_PRINTF(1, 0);
#endif
   Int_t        NumberOfOccurrence(ROMEString& subString) const { return NumberOfOccurrence(subString.Data()); }
   Int_t        NumberOfOccurrence(const char* subString) const;
   void         Write() const { cout << Data(); }
   void         WriteLine() const { cout << Data() << endl; }
   Int_t        ToInteger() const;
   UInt_t       ToUnsignedInteger() const;
   Long_t       ToLong() const;
   ULong_t      ToUnsignedLong() const;
   Long64_t     ToLong64() const;
   ULong64_t    ToUnsignedLong64() const;
   Double_t     ToDouble() const;
   Float_t      ToFloat() const;
   Bool_t       ToBool() const;
   void         ToLower() { static_cast<TString*>(this)->ToLower(); }
   const char*  ToLower(ROMEString& destination) const;
   void         ToUpper() { static_cast<TString*>(this)->ToUpper(); }
   const char*  ToUpper(ROMEString& destination) const;
   Int_t        ReadFile(const char *filename, Bool_t useCache = kFALSE);
   istream&     ReadFile(istream& str);
   istream&     ReadLine(istream& str);
   const char*  ReadCommandOutput(const char* command, Bool_t readError = kTRUE);
   ROMEString&  StripSpaces();
   ROMEString&  ChopSpaces();
   Bool_t       ContainsFast(const char* s) const { return strstr(Data(), s) != 0; }
   Bool_t       ContainsInLongString(const char* str);
#ifndef __CINT__
   static void  SetLongMode(Int_t length = (1<<15));
#endif

   ClassDef(ROMEString, 1) // Functional TString for ROME
};

#endif   // ROMEString_H
