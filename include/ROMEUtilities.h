/********************************************************************
  ROMEUtilities.h, J. Wouters LANL

  $Id$

********************************************************************/
#ifndef ROMEUtilities_H
#define ROMEUtilities_H

#include <Rtypes.h>

class ROMEUtilities {
public:
   ROMEUtilities() {};
   static void ByteSwap( UShort_t *aValue );
   static void ByteSwap( Short_t *aValue );
   static void ByteSwap( UInt_t *aValue );
   static void ByteSwap( Int_t *aValue );
   static void ByteSwap( Float_t *aValue );
   static void ByteSwap( ULong64_t *aValue );
   static void ByteSwap( Long64_t *aValue );
   static void ByteSwap( Double_t *aValue );
};

#endif // ROMEUtilities_H
