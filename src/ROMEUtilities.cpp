// Author: Jan Wouters
//////////////////////////////////////////////////////////////////////////
//
//  ROMEUtilities
//
//  General purpose utilities.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#include <ROMEUtilities.h>

// Byte swapping big endian <-> little endian
void ROMEUtilities::ByteSwap( UShort_t *aValue ) 
{
   Byte_t _tmp;
   _tmp = *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *( ( (Byte_t *)( aValue ) ) + 1 );
   *( ( (Byte_t *)( aValue ) ) + 1 ) = _tmp;
}

void ROMEUtilities::ByteSwap( Short_t *aValue )
{
    ByteSwap( (UShort_t *)aValue );
}

void ROMEUtilities::ByteSwap( UInt_t *aValue ) 
{
   Byte_t _tmp;
   _tmp = *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *( ( (Byte_t *)( aValue ) ) + 3 );
   *( ((Byte_t *)( aValue ) ) + 3) = _tmp;
   
   _tmp = *( ( (Byte_t *)( aValue ) ) + 1);
   *( ( (Byte_t *)( aValue ) ) + 1 ) = *( ( (Byte_t *)( aValue ) ) + 2 ); 
   *( ( (Byte_t *)( aValue ) ) + 2 ) = _tmp;
}

void ROMEUtilities::ByteSwap( Int_t *aValue )
{
    ByteSwap( (UInt_t *)aValue );
}

void ROMEUtilities::ByteSwap( Float_t *aValue )
{
    ByteSwap( (UInt_t *)aValue );
}

void ROMEUtilities::ByteSwap( ULong64_t *aValue ) 
{ 
   Byte_t _tmp;
   _tmp= *( (Byte_t *)( aValue ) );
   *( (Byte_t *)( aValue ) ) = *(((Byte_t *)( aValue ) ) + 7 );
   *( ( (Byte_t *)( aValue ) ) + 7 ) = _tmp;
   
   _tmp = *( ( (Byte_t *)( aValue ) ) + 1 );
   *( ( (Byte_t *)( aValue ) ) + 1 ) = *( ( (Byte_t *)( aValue ) ) + 6 );
   *( ( (Byte_t *)( aValue ) ) + 6 ) = _tmp;
   
   _tmp = *(((Byte_t *)( aValue ))+2);
   *( ( (Byte_t *)( aValue ) ) + 2 ) = *(((Byte_t *)( aValue ) ) + 5 );
   *( ( (Byte_t *)( aValue ) ) + 5 ) = _tmp;
   
   _tmp = *( ( (Byte_t *)( aValue ) ) + 3 );
   *( ( (Byte_t *)( aValue ) ) + 3 ) = *(((Byte_t *)( aValue ) ) + 4 );
   *( ( (Byte_t *)( aValue ) ) + 4 ) = _tmp;
}

void ROMEUtilities::ByteSwap( Double_t *aValue )
{
    ByteSwap( (ULong64_t *)aValue );
}
