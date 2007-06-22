// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//
// ROMEPrint
//
//  Message system in ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <map>
#include <math.h>
#include <TArrayI.h>
#if defined( HAVE_MIDAS )
#   include "midas.h"
#endif
#include "Riostream.h"
#include "ROMEPrint.h"
#include "ROMEString.h"
#include "ROMEStrArray.h"

Int_t ROMEPrint::gVerboseLevel = kNormal;
Int_t ROMEPrint::gReportMaxCount = 20;

namespace {

map<string, Int_t> fgReportMap;              // index of reports
TArrayI            fgReportLine;             // line number of reports
TArrayI            fgReportPrintCount;       // print count of reports
TArrayI            fgReportLevel;            // verbose level of reports
ROMEStrArray       fgReportFile;             // function name of reports
ROMEStrArray       fgReportFunction;         // function name of reports
ROMEStrArray       fgReportMessage;          // function name of reports
Int_t              fgReportHeaderLength;     // length of line header

//______________________________________________________________________________
void PrintSummary(Int_t level, Int_t reportIndex, Int_t funcFileLineLength, Int_t countLength)
{
   ROMEString funcFileLine;
   ROMEString report;
   Int_t      messageLength;

   if (fgReportLevel.At(reportIndex) == level || level < 0) {
      funcFileLine.SetFormatted("%s:%d:%s", gSystem->BaseName(fgReportFile.At(reportIndex).Data()),
                                fgReportLine.At(reportIndex), fgReportFunction.At(reportIndex).Data());
      report = fgReportMessage.At(reportIndex);
      report.ReplaceAll("\n", " ");
      messageLength = 76 - countLength - funcFileLineLength - 4;
      if (messageLength > 0) {
         if (report.Length() > messageLength + 1) {
            report.Resize(messageLength + 1);
            report[messageLength - 2] = report[messageLength - 1] = report[messageLength] = '.';
         }
         if (!report.EndsWith("\n")) {
            report += '\n';
         }
      } else {
         report.Resize(0);
      }
      cout<<setw(countLength)<<fgReportPrintCount.At(reportIndex)<<" "
          <<kReportLevelLetter[fgReportLevel.At(reportIndex)]<<" "
          <<left<<setw(funcFileLineLength)<<funcFileLine<<" "
          <<report<<right<<flush;
   }
}

} // namespace

//______________________________________________________________________________
void ROMEPrint::PrintAlways(const char* va_(fmt),...)
{
// Print always independently of verbose level
// STDOUT
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap) <<flush;
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Debug(const char* va_(fmt),...)
{
// Print when verbose level >= kDebug
// STDOUT
   if (gVerboseLevel < kDebug)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Print(const char* va_(fmt),...)
{
// Print when verbose level >= kNormal
// STDOUT
   if (gVerboseLevel < kNormal)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Info(const char* va_(fmt),...)
{
// Print when verbose level >= kNormal
// STDOUT and midas message
   if (gVerboseLevel < kNormal)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MINFO, "ROMEPrint::Info", text.Data());
   cout<<text.Data()<<flush;
#else
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Warning(const char* va_(fmt),...)
{
// Print when verbose level >= kWarning
// STDERR and midas message
   if (gVerboseLevel < kWarning)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MINFO, "ROMEPrint::Warning", text.Data());
   cerr<<text.Data()<<flush;
#else
   cerr<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Error(const char* va_(fmt),...)
{
// Print when verbose level >= kError
// STDERR and midas message
   if (gVerboseLevel < kError)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MERROR, "ROMEPrint::Error", text.Data());
   cerr<<text.Data()<<flush;
#else
   cerr<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Report(const Int_t verboseLevel, const char* fileName, const char *funcName, Int_t lineNumber,
                       const Long64_t run, const Long64_t event, const Bool_t printHeader, const char* va_(fmt),...)
{
   // Messaging system of ROME. This function is called from user code.
   // ROME framework or generated code does not call this.
   //
   // Example.
   //
   // #include "ROMEPrint.h"
   //
   // void function() {
   //    Int_t status = otherFunction();
   //    if (!status) {
   //       Report(R_WARNING, "Status is %d", status);
   //    }
   // }
   //
   // Available verbose levels are R_ERROR, R_WARNING, R_INFO, R_VERBOSE and R_DEBUG.

   if (gVerboseLevel < verboseLevel) {
      return;
   }

   if (va_(fmt) == 0) {
      return;
   }

   if (!printHeader) {
      // Just print and return
      va_list ap;
      va_start(ap,va_(fmt));
      ROMEString report = ROMEString::Format(va_(fmt), ap);
      if (!report.EndsWith("\n")) {
         report += '\n';
      }
      if (verboseLevel <= kWarning) {
         cerr<<report<<flush;
      } else {
         cout<<report<<flush;
      }
#if defined( HAVE_MIDAS )
      if (verboseLevel <= kWarning) {
         cm_msg(MERROR, "ROMEPrint::Report", report.Data());
      } else {
         cm_msg(MINFO, "ROMEPrint::Report", report.Data());
      }
#endif
      va_end(ap);
      return;
   }

   ROMEString fileLine;
   ROMEString lineHeader;

   fileLine.SetFormatted("%s:%d", gSystem->BaseName(fileName), lineNumber);

   Int_t indx = fgReportMap[fileLine.Data()];

   if (!indx) { // this report is not registered
      fgReportMap[fileLine.Data()] = indx = fgReportMap.size();
      fgReportLine.Set(indx + 1);
      fgReportPrintCount.Set(indx + 1);
      fgReportLevel.Set(indx + 1);
      fgReportLine.AddAt(lineNumber, indx);
      fgReportPrintCount.AddAt(0, indx);
      fgReportLevel.AddAt(verboseLevel, indx);
      fgReportFile.AddAtAndExpand(fileName, indx);
      fgReportFunction.AddAtAndExpand(funcName, indx);
      fgReportMessage.AddAtAndExpand(va_(fmt), indx);
   }

   fgReportPrintCount.AddAt(fgReportPrintCount.At(indx) + 1, indx); // increment

   if (gReportMaxCount <= 0 || fgReportPrintCount.At(indx) <= gReportMaxCount) {
      // print
      ROMEString text;
      ROMEString tmp;
#if defined(R__UNIX)
      switch (event) {
      case kEventNumberInit:
         lineHeader.SetFormatted("[%c,%lld-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'I', fileLine.Data());
         break;
      case kEventNumberBeginOfRun:
         lineHeader.SetFormatted("[%c,%lld-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'B', fileLine.Data());
         break;
      case kEventNumberEndOfRun:
         lineHeader.SetFormatted("[%c,%lld-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'E', fileLine.Data());
         break;
      case kEventNumberTerminate:
         lineHeader.SetFormatted("[%c,%lld-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'T', fileLine.Data());
         break;
      default:
         lineHeader.SetFormatted("[%c,%lld-%lld,%s] ", kReportLevelLetter[verboseLevel], run, event, fileLine.Data());
      }
#else
      switch (event) {
      case kEventNumberInit:
         lineHeader.SetFormatted("[%c,%I64d-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'I', fileLine.Data());
         break;
      case kEventNumberBeginOfRun:
         lineHeader.SetFormatted("[%c,%I64d-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'B', fileLine.Data());
         break;
      case kEventNumberEndOfRun:
         lineHeader.SetFormatted("[%c,%I64d-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'E', fileLine.Data());
         break;
      case kEventNumberTerminate:
         lineHeader.SetFormatted("[%c,%I64d-%c,%s] ", kReportLevelLetter[verboseLevel], run, 'T', fileLine.Data());
         break;
      default:
         lineHeader.SetFormatted("[%c,%I64d-%I64d,%s] ", kReportLevelLetter[verboseLevel], run, event, fileLine.Data());
      }
#endif
      if (fgReportHeaderLength < lineHeader.Length()) {
         fgReportHeaderLength = lineHeader.Length();
      }
      text.SetFormatted("%-*s", fgReportHeaderLength, lineHeader.Data());

      if (fgReportPrintCount.At(indx) == gReportMaxCount) {
         text.Insert(0, "The following message will be suppressed\n");
      }

      va_list ap;
      va_start(ap,va_(fmt));
      ROMEString report = ROMEString::Format(va_(fmt), ap);
      if (!report.EndsWith("\n")) {
         report += '\n';
      }
      tmp.SetFormatted("\n%*s", fgReportHeaderLength, "");
      report.ReplaceAll("\n", tmp.Data());
      report.Resize(report.Length() -  fgReportHeaderLength);
      va_end(ap);

      text += report;
      if (verboseLevel <= kWarning) {
         cerr<<text<<flush;
      } else {
         cout<<text<<flush;
      }
#if defined( HAVE_MIDAS )
      if (verboseLevel <= kWarning) {
         cm_msg(MERROR, "ROMEPrint::Report", text.Data());
      } else {
         cm_msg(MINFO, "ROMEPrint::Report", text.Data());
      }
#endif
   }
}

//______________________________________________________________________________
void ROMEPrint::ReportSummary()
{
   Int_t i;
   Int_t n = fgReportMap.size();

   if (!n) {
      // probably, the user is not using 'Report'.
      return;
   }

   ROMEString    fileLine;
   Int_t         maxFuncFileLineLength = 0;
   Int_t         funcFileLineLength;
   Int_t         maxCountLength = 0;
   Int_t         countLength;
   ROMEStrArray  strArraySort;
   Int_t        *sortIndex = new Int_t[n + 1];

   strArraySort.AddAtAndExpand("", 0); // dummy

   for (i = 1; i <= n; i++) {
      funcFileLineLength = strlen(gSystem->BaseName(fgReportFile.At(i).Data())) + 4 /* line number */ +
            fgReportFunction.At(i).Length() + 2/* : : */;
      if (maxFuncFileLineLength < funcFileLineLength) {
         maxFuncFileLineLength = funcFileLineLength;
      }
      countLength = static_cast<Int_t>(log10(static_cast<Double_t>(fgReportPrintCount.At(i)))) + 1;
      if (maxCountLength < countLength) {
         maxCountLength = countLength;
      }
      fileLine = gSystem->BaseName(fgReportFile.At(i));
      fileLine += fgReportLine.At(i);
      fileLine.ToLower();
      strArraySort.AddAtAndExpand(fileLine.Data(), i);
   }

   strArraySort.Sort(sortIndex, kFALSE);

   cout<<"*************************** Report Message Summary ***************************"<<endl<<endl;
#if 0
   // error
   for (i = 1; i <= n; i++) {
      PrintSummary(kError, sortIndex[i], maxFuncFileLineLength, maxCountLength);
   }
   // warning
   for (i = 1; i <= n; i++) {
      PrintSummary(kWarning, sortIndex[i], maxFuncFileLineLength, maxCountLength);
   }
   // info
   for (i = 1; i <= n; i++) {
      PrintSummary(kNormal, sortIndex[i], maxFuncFileLineLength, maxCountLength);
   }
   // verbose
   for (i = 1; i <= n; i++) {
      PrintSummary(kVerbose, sortIndex[i], maxFuncFileLineLength, maxCountLength);
   }
   // debug
   for (i = 1; i <= n; i++) {
      PrintSummary(kDebug, sortIndex[i], maxFuncFileLineLength, maxCountLength);
   }
#else
   for (i = 1; i <= n; i++) {
      PrintSummary(-1, sortIndex[i], maxFuncFileLineLength, maxCountLength);
   }
#endif
   cout<<endl<<"******************************************************************************"<<endl;

   delete [] sortIndex;
}
