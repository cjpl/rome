// Author: Ryu Sawada

/******************************************************************************
 *         ***  This file will be overwritten by the ROMEBuilder  ***         *
 *          ***      Don't make manual changes to this file      ***          *
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FOPMTData                                                                  //
//                                                                            //
// PMT data.                                                                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "generated/FOPMTData.h"
ClassImp(FOPMTData)
FOPMTData::FOPMTData( Short_t ADC_value,Short_t TDC_value )
{
   FOPMTData::Class()->IgnoreTObjectStreamer();
   ADC = ADC_value;
   TDC = TDC_value;
   fModified = false;
};

FOPMTData::~FOPMTData()
{
};


Bool_t FOPMTData::isModified()
{
   if ( fModified ) return true;
   return false;
}



void FOPMTData::ResetModified()
{
   if (isModified()) {
      SetModified(false);
   }
};

void FOPMTData::SetAll( Short_t ADC_value,Short_t TDC_value )
{
   ADC = ADC_value;
   TDC = TDC_value;
   SetModified(true);
};

void FOPMTData::Reset()
{
   if ( !isModified() ) return;
      ADC = (Short_t)0;
      TDC = (Short_t)0;
   fModified = false;
};


