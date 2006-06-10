/********************************************************************
  ROMEConfigParameter.h, M.Schneebeli

  $Id$

********************************************************************/
#ifndef ROMEConfigParameter_H
#define ROMEConfigParameter_H

#include <TObject.h>
#include "ROMEString.h"
#include "ROMEStrArray.h"

class ROMEConfigParameter : public TObject {
private:
   ROMEString            fName;
   ROMEString            fArraySize;
   ROMEString            fWidgetType;
   ROMEStrArray*         fReadModifiedTrueLines;
   ROMEStrArray*         fSetLines;
   ROMEStrArray*         fWriteLines;
   ROMEStrArray*         fAdditionalWriteLines;
   ROMEStrArray*         fComboBoxEntries;
   Bool_t                fWriteLinesAlways;
public:
   ROMEConfigParameter(ROMEString name, ROMEString arraySize = "1", ROMEString widgetType = "EditBox");
   ROMEString&    GetName() { return fName; };
   ROMEString&    GetArraySize() { return fArraySize; };
   ROMEString&    GetWidgetType() { return fWidgetType; };

   Int_t          GetNumberOfReadModifiedTrueLines() { return fReadModifiedTrueLines->GetEntriesFast(); };
   const char*    GetReadModifiedTrueLineAt(Int_t i) { return fReadModifiedTrueLines->At(i).Data(); };
   void AddReadModifiedTrueLine(const char* line) { fReadModifiedTrueLines->AddLast(line); };

   Int_t          GetNumberOfSetLines() { return fSetLines->GetEntriesFast(); };
   const char*    GetSetLineAt(Int_t i) { return fSetLines->At(i).Data(); };
   void AddSetLine(const char* line,...);

   Int_t          GetNumberOfWriteLines() { return fWriteLines->GetEntriesFast(); };
   const char*    GetWriteLineAt(Int_t i) { return fWriteLines->At(i).Data(); };
   void AddWriteLine(const char* line,...);

   Int_t          GetNumberOfAdditionalWriteLines() { return fAdditionalWriteLines->GetEntriesFast(); };
   const char*    GetAdditionalWriteLineAt(Int_t i) { return fAdditionalWriteLines->At(i).Data(); };
   void           AddAdditionalWriteLine(const char* line,...);

   Int_t          GetNumberOfComboBoxEntries() { return fComboBoxEntries->GetEntriesFast(); };
   const char*    GetComboBoxEntryAt(Int_t i) { return fComboBoxEntries->At(i).Data(); };
   void           AddComboBoxEntry(const char* line,...);

   Bool_t         IsWriteLinesAlways() { return fWriteLinesAlways; };
   void           DontWriteLinesAlways() { fWriteLinesAlways = false; };
};


class ROMEConfigParameterGroup : public TObject {
private:
   ROMEString         fGroupName;
   ROMEString         fArraySize;
   ROMEString         fGroupIdentifier;
   ROMEString         fNameIdentifier;
   ROMEString         fArrayIdentifier;
   ROMEString         fTagName;
   Int_t              fMultiplicity;
   TObjArray*         fParameters;
   TObjArray*         fSubGroups;
   ROMEStrArray*      fReadGroupArrayInitLines;
   ROMEStrArray*      fWriteStartLines;
   ROMEStrArray*      fWriteEndLines;
   Int_t              fHierarchyLevel;
   Bool_t             fWriteAlways;

public:
   ROMEConfigParameterGroup(ROMEString groupName,ROMEString arraySize = "1",ROMEString groupIdentifier = "",ROMEString nameIdentifier = "",ROMEString arrayIdentifier = "",ROMEString tagName = "",Int_t multiplicity = 1);
   ROMEString&                GetGroupName() { return fGroupName; };
   ROMEString&                GetArraySize() { return fArraySize; };
   ROMEString&                GetGroupIdentifier() { return fGroupIdentifier; };
   ROMEString&                GetNameIdentifier() { return fNameIdentifier; };
   ROMEString&                GetArrayIdentifier() { return fArrayIdentifier; };
   ROMEString&                GetTagName() { return fTagName; };
   Int_t                      GetMultiplicity() { return fMultiplicity; };
   Int_t                      GetHierarchyLevel() { return fHierarchyLevel; };
   Bool_t                     IsWriteAlways() { return fWriteAlways; };
   void                       SetWriteAlways() { fWriteAlways = true; };

   void                       SetHierarchyLevel(Int_t level) { fHierarchyLevel = level; };

   Int_t                      GetNumberOfParameters() { return fParameters->GetEntriesFast(); };
   ROMEConfigParameter*       GetParameterAt(Int_t i) { return ((ROMEConfigParameter*)fParameters->At(i)); };
   ROMEConfigParameter*       GetLastParameter() { return ((ROMEConfigParameter*)fParameters->At(fParameters->GetEntriesFast()-1)); };
   void                       AddParameter(ROMEConfigParameter* parameter) { fParameters->AddLast(parameter); };

   Int_t                      GetNumberOfSubGroups() { return fSubGroups->GetEntriesFast(); };
   ROMEConfigParameterGroup*  GetSubGroupAt(Int_t i) { return ((ROMEConfigParameterGroup*)fSubGroups->At(i)); };
   ROMEConfigParameterGroup*  GetLastSubGroup() { return ((ROMEConfigParameterGroup*)fSubGroups->At(fSubGroups->GetEntriesFast()-1)); };
   void                       AddSubGroup(ROMEConfigParameterGroup* subGroup) { fSubGroups->AddLast(subGroup); subGroup->SetHierarchyLevel(fHierarchyLevel+1); };

   Int_t                      GetNumberOfReadGroupArrayInitLines() { return fReadGroupArrayInitLines->GetEntriesFast(); };
   const char*                GetReadGroupArrayInitLineAt(Int_t i) { return fReadGroupArrayInitLines->At(i).Data(); };
   void                       AddReadGroupArrayInitLine(const char* line) { fReadGroupArrayInitLines->AddLast(line); };

   Int_t                      GetNumberOfWriteStartLines() { return fWriteStartLines->GetEntriesFast(); };
   const char*                GetWriteStartLineAt(Int_t i) { return fWriteStartLines->At(i).Data(); };
   void                       AddWriteStartLine(const char* line) { fWriteStartLines->AddLast(line); };

   Int_t                      GetNumberOfWriteEndLines() { return fWriteEndLines->GetEntriesFast(); };
   const char*                GetWriteEndLineAt(Int_t i) { return fWriteEndLines->At(i).Data(); };
   void                       AddWriteEndLine(const char* line) { fWriteEndLines->AddLast(line); };
};

#endif   // ROMEConfigParameter_H
