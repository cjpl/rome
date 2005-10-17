/********************************************************************
  ROMEUtilities.h, J. Wouters LANL

  $Id:$

********************************************************************/
#ifndef ROMEUtilities_H
#define ROMEUtilities_H

#include <Rtypes.h>

void ByteSwap( UShort_t *aValue );
void ByteSwap( Short_t *aValue );
void ByteSwap( UInt_t *aValue );
void ByteSwap( Int_t *aValue );
void ByteSwap( Float_t *aValue );
void ByteSwap( ULong64_t *aValue );
void ByteSwap( Long64_t *aValue );
void ByteSwap( Double_t *aValue );

#endif
