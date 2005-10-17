/********************************************************************
  ROMEUtilities.h, J. Wouters LANL

  $Log$
  Revision 1.2  2005/07/01 08:25:18  sawada
  small bug fix.

  Revision 1.1  2005/07/01 07:14:31  schneebeli_m
  added to cvs

  Revision 1.0  2005/06/27 16:30:00  wouters
  Original.

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
