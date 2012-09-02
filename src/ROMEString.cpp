// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEString                                                           //
//                                                                      //
//  Handles character strings.
//  Derived from TString.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <map>
#include <TMath.h>
#include <TVirtualMutex.h>
#include "ROMEiostream.h"
#include "ROMEString.h"

ClassImp(ROMEString)

static TVirtualMutex *fgROMEStringMutex = 0;

//______________________________________________________________________________
ROMEString& ROMEString::AppendFormatted(const char* va_(fmt), ...)
{
   if (!va_(fmt)) {
      return *this;
   }
   va_list ap;
   va_start(ap, va_(fmt));
   Append(Format(va_(fmt), ap));
   va_end(ap);
   return *this;
}

//______________________________________________________________________________
ROMEString& ROMEString::InsertFormatted(Ssiz_t position, const char* va_(fmt), ...)
{
   if (!va_(fmt)) {
      return *this;
   }
   va_list ap;
   va_start(ap, va_(fmt));
   Insert(position, Format(va_(fmt), ap));
   va_end(ap);
   return *this;
}

//______________________________________________________________________________
ROMEString& ROMEString::SetFormatted(const char* va_(fmt), ...)
{
   if (!va_(fmt)) {
      return *this;
   }
   va_list ap;
   va_start(ap, va_(fmt));
   *this = Format(va_(fmt), ap);
   va_end(ap);
   return *this;
}

//______________________________________________________________________________
Int_t ROMEString::NumberOfOccurrence(const char* subString) const
{
   Int_t numberOfOccurrence = 0;
   const char* str = Data();
   str = strstr(str, subString);
   while (str) {
      str++;
      numberOfOccurrence++;
      str = strstr(str, subString);
   }
   return numberOfOccurrence;
}

static map<string, string> fgReadFileCache;
//______________________________________________________________________________
Int_t ROMEString::ReadFile(const char *filename, Bool_t useCache)
{
   if (useCache && fgReadFileCache[filename].length()) {
      *this = fgReadFileCache[filename].data();
      return Length();
   }

   fstream *fileStream = new fstream(filename, ios::in);
   if (fileStream && fileStream->good()) {
      ReadFile(*fileStream);
      fgReadFileCache[filename] = Data();
      delete fileStream;
      return Length();
   } else {
      delete fileStream;
      Resize(0);
      return -1;
   }
}

//______________________________________________________________________________
istream& ROMEString::ReadFile(istream& str)
{
   Resize(0);
   char *buffer;
   int bufferSize = 2000;
   int bufferLength = bufferSize - 1;
   while (bufferLength == bufferSize - 1) {
      bufferSize *= 2;
      buffer = new char[bufferSize];
      str.get(buffer, bufferSize, 0);
      bufferLength = strlen(buffer);
      Append(buffer);
      delete [] buffer;
   }
   return str;
}

//______________________________________________________________________________
const char* ROMEString::ReadCommandOutput(const char* command, Bool_t readError, Bool_t suppressError)
{
   Resize(0);
   TString cmd = command;
   TString tmp;
   if (readError) {
      cmd += " 2>&1";
   }
   if (suppressError) {
      tmp = cmd;
      cmd = "exec 3>&2 2> /dev/null;";
      cmd += tmp;
   }
   FILE *pipe = gSystem->OpenPipe(cmd.Data(), "r");
   if (!pipe) {
      //      SysError("ReadCommandOutput","cannot run command: %s", command);
      return 0;
   }
   while (tmp.Gets(pipe, kFALSE)) {
      Append(tmp);
   }
   gSystem->ClosePipe(pipe);

   if (EndsWith("\n")) {
       Resize(Length() - 1);
   }
   return *this;
}

//______________________________________________________________________________
istream& ROMEString::ReadLine(istream& str)
{
   Resize(0);
   char *buffer;
   int bufferSize = 2000;
   int bufferLength = bufferSize - 1;
   while (bufferLength == bufferSize - 1) {
      bufferSize*=2;
      buffer = new char[bufferSize];
      str.getline(buffer, bufferSize);
      bufferLength = strlen(buffer);
      Append(buffer);
      delete [] buffer;
   }
   return str;
}

//______________________________________________________________________________
Int_t ROMEString::ToInteger() const
{
   char *cstop;
   return strtol(Data(), &cstop, 10);
}

//______________________________________________________________________________
UInt_t ROMEString::ToUnsignedInteger() const
{
   char *cstop;
   return strtoul(Data(), &cstop, 10);
}

//______________________________________________________________________________
Long_t ROMEString::ToLong() const
{
   char *cstop;
   return strtol(Data(), &cstop, 10);
}

//______________________________________________________________________________
ULong_t ROMEString::ToUnsignedLong() const
{
   char *cstop;
   return strtoul(Data(), &cstop, 10);
}

//______________________________________________________________________________
Long64_t ROMEString::ToLong64() const
{
   char *cstop;
#if defined( R__UNIX )
   return strtoll(Data(), &cstop, 10);
#else
   return _strtoi64(Data(), &cstop, 10);
#endif
}

//______________________________________________________________________________
ULong64_t ROMEString::ToUnsignedLong64() const
{
   char *cstop;
#if defined( R__UNIX )
   return strtoull(Data(), &cstop, 10);
#else
   return _strtoui64(Data(), &cstop, 10);
#endif
}

//______________________________________________________________________________
Double_t ROMEString::ToDouble() const
{
   char *cstop;
   return strtod(Data(), &cstop);
}

//______________________________________________________________________________
Float_t ROMEString::ToFloat() const
{
   char *cstop;
   return static_cast<Float_t>(strtod(Data(), &cstop));
}

//______________________________________________________________________________
Bool_t ROMEString::ToBool() const
{
   if (CompareTo("true", TString::kIgnoreCase)) {
      return true;
   }
   if (CompareTo("false", TString::kIgnoreCase)) {
      return false;
   }
   return ToInteger() != 0;
}

//______________________________________________________________________________
const char* ROMEString::ToLower(ROMEString& destination) const
{
   destination = Data();
   destination.ToLower();
   return destination.Data();
}

//______________________________________________________________________________
const char* ROMEString::ToUpper(ROMEString& destination) const
{
   destination = Data();
   destination.ToUpper();
   return destination.Data();
}

//______________________________________________________________________________
ROMEString& ROMEString::StripSpaces()
{
// Strip space, tab and new line at both sides
   Ssiz_t start = 0;             // Index of first character
   Ssiz_t end = Length();        // One beyond last character
   const char *direct = Data();  // Avoid a dereference w dumb compiler

   while (start < end && direct[start] <= ' ') {
      ++start;
   }

   while (start < end && direct[end] <= ' ') {
      --end;
   }

   if (start == Length()) {
      *this = "";
   } else {
      Remove(end + 1, Length() - end - 1);
      Remove(0, start);
   }
   return *this;
}

//______________________________________________________________________________
ROMEString& ROMEString::ChopSpaces()
{
// Chop space, tab at the end
   Ssiz_t start = 0;         // Index of first character
   Ssiz_t end   = Length()-1;  // The last character
   const char *direct = Data();    // Avoid a dereference w dumb compiler

   while (start < end && (direct[end] == ' ' || direct[end] == '\t' )) {
      --end;
   }

   Remove(end + 1, Length() - end -1);
   return *this;
}

//______________________________________________________________________________
Bool_t ROMEString::ContainsInLongString(const char* str)
{
// Check if 's' is contained by this string.
// Suitable when this string is long.
// If this string is short, use Contains or ContainsFast
//
// Algorithm is copied from
// "C++ Footprint and Performance Optimization" by Rene Alexander and Graham Bensley

   if (!str) {
      return kFALSE;
   }

   Int_t   skips[kMaxChar + 1];
   UChar_t searchLen = strlen(str);

   if (searchLen > Length() || searchLen == 0) {
      return kFALSE;
   }

   Int_t len2 = searchLen + 1;
   Int_t i;

   for (i = 0; i < kMaxChar + 1; i++) {
      skips[i] = len2;
   }
   for (i = 0; i < searchLen; i++) {
      skips[static_cast<UChar_t>(str[i])] = searchLen - i;
   }

   const char *testf = Data();
   const char *end = testf + Length();
   const char *skipindex;
   len2 = searchLen - 1;

   while(1) {
      i = len2;
      while (testf[i] == str[i] && --i >= 0) {;}
      if (i != -1) {
         skipindex = testf + searchLen;
         if (skipindex >= end) {
            return kFALSE;
         }
         testf += skips[static_cast<UChar_t>(*skipindex)];
      } else {
         return kTRUE;
      }
   }
   return kFALSE;
}

// formatting functions copied from TString.cxx
#if defined( R__VISUAL_CPLUSPLUS )
#define vsnprintf _vsnprintf
#endif
const int cb_size  = 4096;
const int fld_size = 2048;

namespace {
   char gFormbuf[cb_size];       // some slob for form overflow
   char *gBfree  = gFormbuf;
   char *gEndbuf = &gFormbuf[cb_size - 1];
}

//______________________________________________________________________________
char* ROMEString::SlowFormat(const char *format, va_list ap, int hint)
{
   // Format a string in a formatting buffer (using a printf style
   // format descriptor).

   static char *slowBuffer  = 0;
   static int   slowBufferSize = 0;

   ROME_LOCKGUARD(fgROMEStringMutex);

   if (hint == -1) {
      hint = fld_size;
   }
   if (hint > slowBufferSize) {
      delete [] slowBuffer;
      slowBufferSize = 2 * hint;
      if (hint < 0 || slowBufferSize < 0) {
         slowBufferSize = 0;
         slowBuffer = 0;
         return 0;
      }
      slowBuffer = new char[slowBufferSize];
   }

   va_list apcopy;
   va_copy(apcopy, ap);
   int n = vsnprintf(slowBuffer, slowBufferSize, format, ap);
   // old vsnprintf's return -1 if string is truncated new ones return
   // total number of characters that would have been written
   if (n == -1 || n >= slowBufferSize) {
      if (n == -1) {
         n = 2 * slowBufferSize;
      }
      if (n == slowBufferSize) {
         n++;
      }
      if (n <= 0) {
         return 0; // int overflow!
      }
      char *c = SlowFormat(format, apcopy, n);
      va_end(apcopy);
      return c;
   }
   va_end(apcopy);

   return slowBuffer;
}

//______________________________________________________________________________
char* ROMEString::Format(const char *format, va_list ap)
{
   // Format a string in a circular formatting buffer (using a printf style
   // format descriptor).

   ROME_LOCKGUARD(fgROMEStringMutex);

   char *buf = gBfree;

   if (buf + fld_size > gEndbuf) {
      buf = gFormbuf;
   }

   va_list apcopy;
   va_copy(apcopy, ap);
   int n = vsnprintf(buf, fld_size, format, ap);
   // old vsnprintf's return -1 if string is truncated new ones return
   // total number of characters that would have been written
   if (n == -1 || n >= fld_size) {
      char *c = SlowFormat(format, apcopy, n);
      va_end(apcopy);
      return c;
   }
   va_end(apcopy);

   gBfree = buf + n + 1;
   return buf;
}
// end of formatting functions copied from TString.cxx
