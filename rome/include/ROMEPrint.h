/********************************************************************
  ROMEPrint.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEPrint_H
#define ROMEPrint_H

#include <RConfig.h>
#include <Rtypes.h>
#include <Varargs.h>
#include "ROMEAnalyzer.h"
#include "ROME.h"

namespace ROMEPrint
{
   // Verbose level
   enum {
      kMute    = 0,
      kError   = 1,
      kWarning = 2,
      kNormal  = 3,
      kVerbose = 4,
      kDebug   = 5
   };
   const char* const kReportLevelLetter = "MEWIVD";

   extern Int_t gVerboseLevel;   // verbose level
   extern Int_t gReportMaxCount; // max number of printing for a report

   // Output
   void  PrintAlways(const char *msgfmt, ...) G_GNUC_PRINTF(1, 2);
   void  Debug(const char *msgfmt, ...)       G_GNUC_PRINTF(1, 2);
   void  Print(const char *msgfmt, ...)       G_GNUC_PRINTF(1, 2);
   void  Info(const char *msgfmt, ...)        G_GNUC_PRINTF(1, 2);
   void  Warning(const char *msgfmt, ...)     G_GNUC_PRINTF(1, 2);
   void  Error(const char *msgfmt, ...)       G_GNUC_PRINTF(1, 2);

   void  Report(const Int_t verboseLevel, const char* fileName, const char *funcName, Int_t lineNumber,
                const Long64_t run, const Long64_t event, const Bool_t printHeader, const char* va_(fmt),...) G_GNUC_PRINTF(8, 9);
   void  ReportSummary();

   inline Int_t GetVerboseLevel() { return gVerboseLevel; }
   inline void  SetVerboseLevel(Int_t level = kNormal) { gVerboseLevel = level; }

   inline Int_t GetReportMaxCount() { return gReportMaxCount; }
   inline void  SetReportMaxCount(Int_t max = 20) { gReportMaxCount = max; }
};

// Report styles
//  R_ERROR   : Printed when gVerboseLevel >= kError
//  R_WARNING : Printed when gVerboseLevel >= kWarning
//  R_PLAIN   : Printed when gVerboseLevel >= kNormal, without line header (run number, event number, filename and line number)
//  R_INFO    : Printed when gVerboseLevel >= kNormal
//  R_VERBOSE : Printed when gVerboseLevel >= kVerbose
//  R_DEBUG   : Printed when gVerboseLevel >= kDebug

#if defined(R__UNIX)
#define R_ERROR   ROMEPrint::kError,   __FILE__, __func__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_WARNING ROMEPrint::kWarning, __FILE__, __func__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_PLAIN   ROMEPrint::kNormal,  __FILE__, __func__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kFALSE
#define R_INFO    ROMEPrint::kNormal,  __FILE__, __func__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_VERBOSE ROMEPrint::kVerbose, __FILE__, __func__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_DEBUG   ROMEPrint::kDebug,   __FILE__, __func__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#else
#define R_ERROR   ROMEPrint::kError,   __FILE__, __FUNCTION__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_WARNING ROMEPrint::kWarning, __FILE__, __FUNCTION__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_PLAIN   ROMEPrint::kNormal,  __FILE__, __FUNCTION__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kFALSE
#define R_INFO    ROMEPrint::kNormal,  __FILE__, __FUNCTION__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_VERBOSE ROMEPrint::kVerbose, __FILE__, __FUNCTION__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE
#define R_DEBUG   ROMEPrint::kDebug,   __FILE__, __FUNCTION__, __LINE__, gAnalyzer->GetCurrentRunNumber(), gAnalyzer->GetCurrentEventNumber(),kTRUE

#endif

using namespace ROMEPrint;

#endif   // ROMEPrint_H
