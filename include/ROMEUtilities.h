/********************************************************************
  ROMEUtilities.h, J. Wouters LANL

  $Id$

********************************************************************/
#ifndef ROMEUtilities_H
#define ROMEUtilities_H

#include <TObject.h>

class ROMEString;
class ROMEStrArray;

class ROMEUtilities : public TObject
{
public:
   ROMEUtilities() {};
   virtual ~ROMEUtilities() {};
   static void ByteSwap( UShort_t *aValue );
   static void ByteSwap( Short_t *aValue );
   static void ByteSwap( UInt_t *aValue );
   static void ByteSwap( Int_t *aValue );
   static void ByteSwap( Float_t *aValue );
   static void ByteSwap( ULong64_t *aValue );
   static void ByteSwap( Long64_t *aValue );
   static void ByteSwap( Double_t *aValue );
   static void GetMidasTID(ROMEString *buf,Char_t *type);
   static void SearchXMLFiles(ROMEStrArray& files, const char* filepath, const char* xmlpath);

   ClassDef(ROMEUtilities, 0) // Utilities for ROME
};

#endif // ROMEUtilities_H
