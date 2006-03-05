/********************************************************************
  ROMEConfigParameter.h, M.Schneebeli

  $Id:$

********************************************************************/
#ifndef ROMEConfigParameter_H
#define ROMEConfigParameter_H

#include <TObject.h>
#include <TObjArray.h>
#include <ROMEString.h>

class ROMEConfigParameter : public TObject {
private:
   ROMEString            fName;
   ROMEString            fArraySize;
   ROMEStrArray*         fReadModifiedTrueLines;
   ROMEStrArray*         fSetLines;
   ROMEStrArray*         fWriteLines;
public:
   ROMEConfigParameter::ROMEConfigParameter(ROMEString name, ROMEString arraySize = "1") 
   { 
      fName = name; 
      fArraySize = arraySize;
      fReadModifiedTrueLines = new ROMEStrArray(1);
      fSetLines = new ROMEStrArray(1);
      fWriteLines = new ROMEStrArray(1);
   };
   ROMEString&    GetName() { return fName; };
   ROMEString&    GetArraySize() { return fArraySize; };

   int            GetNumberOfReadModifiedTrueLines() { return fReadModifiedTrueLines->GetEntriesFast(); };
   const char*    GetReadModifiedTrueLineAt(int i) { return fReadModifiedTrueLines->At(i).Data(); };
   void AddReadModifiedTrueLine(const char* line) { fReadModifiedTrueLines->AddLast(line); };

   int            GetNumberOfSetLines() { return fSetLines->GetEntriesFast(); };
   const char*    GetSetLineAt(int i) { return fSetLines->At(i).Data(); };
   void AddSetLine(const char* line,...);

   int            GetNumberOfWriteLines() { return fWriteLines->GetEntriesFast(); };
   const char*    GetWriteLineAt(int i) { return fWriteLines->At(i).Data(); };
   void AddWriteLine(const char* line,...);

};
inline void ROMEConfigParameter::AddSetLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fSetLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}
inline void ROMEConfigParameter::AddWriteLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fWriteLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

class ROMEConfigParameterGroup : public TObject {
private:
   ROMEString         fGroupName;
   ROMEString         fArraySize;
   ROMEString         fGroupIdentifier;
   ROMEString         fNameIdentifier;
   ROMEString         fArrayIdentifier;
   ROMEString         fTagName;
   int                fMultiplicity;
   TObjArray*         fParameters;
   TObjArray*         fSubGroups;
   ROMEStrArray*      fReadGroupArrayInitLines;
   ROMEStrArray*      fWriteStartLines;
   ROMEStrArray*      fWriteEndLines;
   int                fHierarchyLevel;
public:
   ROMEConfigParameterGroup::ROMEConfigParameterGroup(ROMEString groupName,ROMEString arraySize = "1",ROMEString groupIdentifier = "",ROMEString nameIdentifier = "",ROMEString arrayIdentifier = "",ROMEString tagName = "",int multiplicity = 1) 
   { 
      fGroupName = groupName;
      fArraySize = arraySize;
      fGroupIdentifier = groupIdentifier;
      if (nameIdentifier.Length()==0)
         fNameIdentifier = fGroupIdentifier+"Name";
      else
         fNameIdentifier = nameIdentifier;
      if (arrayIdentifier.Length()==0)
         fArrayIdentifier = arrayIdentifier;
      else
         fArrayIdentifier = arrayIdentifier+"=";
      if (tagName.Length()==0)
         fTagName = fGroupName;
      else
         fTagName = tagName;
      fMultiplicity = multiplicity;
      fParameters = new TObjArray(10); 
      fSubGroups = new TObjArray(10); 
      fReadGroupArrayInitLines = new ROMEStrArray(1);
      fWriteStartLines = new ROMEStrArray(1);
      fWriteEndLines = new ROMEStrArray(1);
      fHierarchyLevel = 0;
   };
   ROMEString&                   GetGroupName() { return fGroupName; };
   ROMEString&                   GetArraySize() { return fArraySize; };
   ROMEString&                   GetGroupIdentifier() { return fGroupIdentifier; };
   ROMEString&                   GetNameIdentifier() { return fNameIdentifier; };
   ROMEString&                   GetArrayIdentifier() { return fArrayIdentifier; };
   ROMEString&                   GetTagName() { return fTagName; };
   int                           GetMultiplicity() { return fMultiplicity; };
   int                           GetHierarchyLevel() { return fHierarchyLevel; };

   void SetHierarchyLevel(int level) { fHierarchyLevel = level; };

   int                           GetNumberOfParameters() { return fParameters->GetEntriesFast(); };
   ROMEConfigParameter*          GetParameterAt(int i) { return ((ROMEConfigParameter*)fParameters->At(i)); };
   ROMEConfigParameter*          GetLastParameter() { return ((ROMEConfigParameter*)fParameters->At(fParameters->GetEntriesFast()-1)); };
   void AddParameter(ROMEConfigParameter* parameter) { fParameters->AddLast(parameter); };

   int                           GetNumberOfSubGroups() { return fSubGroups->GetEntriesFast(); };
   ROMEConfigParameterGroup*     GetSubGroupAt(int i) { return ((ROMEConfigParameterGroup*)fSubGroups->At(i)); };
   ROMEConfigParameterGroup*     GetLastSubGroup() { return ((ROMEConfigParameterGroup*)fSubGroups->At(fSubGroups->GetEntriesFast()-1)); };
   void AddSubGroup(ROMEConfigParameterGroup* subGroup) { fSubGroups->AddLast(subGroup); subGroup->SetHierarchyLevel(fHierarchyLevel+1); };

   int            GetNumberOfReadGroupArrayInitLines() { return fReadGroupArrayInitLines->GetEntriesFast(); };
   const char*    GetReadGroupArrayInitLineAt(int i) { return fReadGroupArrayInitLines->At(i).Data(); };
   void AddReadGroupArrayInitLine(const char* line) { fReadGroupArrayInitLines->AddLast(line); };

   int            GetNumberOfWriteStartLines() { return fWriteStartLines->GetEntriesFast(); };
   const char*    GetWriteStartLineAt(int i) { return fWriteStartLines->At(i).Data(); };
   void AddWriteStartLine(const char* line) { fWriteStartLines->AddLast(line); };

   int            GetNumberOfWriteEndLines() { return fWriteEndLines->GetEntriesFast(); };
   const char*    GetWriteEndLineAt(int i) { return fWriteEndLines->At(i).Data(); };
   void AddWriteEndLine(const char* line) { fWriteEndLines->AddLast(line); };
};

#endif   // ROMEConfigParameter_H
