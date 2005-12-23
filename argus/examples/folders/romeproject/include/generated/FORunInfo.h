//////////////////////////////////////////////////////////////
// *** This file will be overwritten by the ROMEBuilder *** //
// ***      Don't make manual changes to this file      *** //
//////////////////////////////////////////////////////////////

#ifndef FORunInfo_H
#define FORunInfo_H

#include "TObject.h"
#include "TClass.h"
#include "TClonesArray.h"
#include "TString.h"

class FORunInfo : public TObject
{
protected:
   Short_t       RunMode;   // Trigger mode written by mlogger
   TString       Comment;   // Comment written by mlogger
   Int_t         StartTime; // Run start time written by mlogger
   Int_t         StopTime;  // Run stop time written by mlogger
   Bool_t        fModified; //! Modified Folder Flag

public:
   FORunInfo( Short_t RunMode_value=0,TString Comment_value="none",Int_t StartTime_value=0,Int_t StopTime_value=0 )
   {
      FORunInfo::Class()->IgnoreTObjectStreamer();
      RunMode = RunMode_value;
      Comment = Comment_value;
      StartTime = StartTime_value;
      StopTime = StopTime_value;
      fModified = false;
   };

   virtual ~FORunInfo()
   {
   };

   Short_t        GetRunMode()   { return RunMode;   };
   TString        GetComment()   { return Comment;   };
   Int_t          GetStartTime() { return StartTime; };
   Int_t          GetStopTime()  { return StopTime;  };

   Bool_t         isModified()   {
      if( fModified ) return true;
      return false;
   }

   void SetRunMode  (Short_t       RunMode_value  ) { RunMode   = RunMode_value;   SetModified(true); };
   void SetComment  (TString       Comment_value  ) { Comment   = Comment_value;   SetModified(true); };
   void SetStartTime(Int_t         StartTime_value) { StartTime = StartTime_value; SetModified(true); };
   void SetStopTime (Int_t         StopTime_value ) { StopTime  = StopTime_value;  SetModified(true); };

   void AddRunMode  (Short_t       RunMode_value  ) { RunMode   += RunMode_value;   SetModified(true); };
   void AddComment  (TString       Comment_value  ) { Comment   += Comment_value;   SetModified(true); };
   void AddStartTime(Int_t         StartTime_value) { StartTime += StartTime_value; SetModified(true); };
   void AddStopTime (Int_t         StopTime_value ) { StopTime  += StopTime_value;  SetModified(true); };

   void SetModified (Bool_t        modified ) { fModified  = modified; };

   void ResetModified() {
      int i=0;
      if(isModified()){
         SetModified(false);
      }
   };

   void SetAll( Short_t RunMode_value=0,TString Comment_value="none",Int_t StartTime_value=0,Int_t StopTime_value=0 )
   { RunMode = RunMode_value; Comment = Comment_value; StartTime = StartTime_value; StopTime = StopTime_value; SetModified(true); };

   void Reset() {
      if( !isModified() ) return;
      int i=0;
      RunMode = (Short_t)0;
      Comment = (TString)"none";
      StartTime = (Int_t)0;
      StopTime = (Int_t)0;
      fModified = false;
   };

private:

   ClassDef(FORunInfo,1)
};

#endif   // FORunInfo_H
