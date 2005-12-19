//////////////////////////////////////////////////////////////
// *** This file will be overwritten by the ROMEBuilder *** //
// ***      Don't make manual changes to this file      *** //
//////////////////////////////////////////////////////////////

#ifndef FOPMTData_H
#define FOPMTData_H

#include "TObject.h"
#include "TClass.h"
#include "TClonesArray.h"

class FOPMTData : public TObject
{
protected:
   Short_t       ADC;      // ADC value
   Short_t       TDC;      // TDC value
   Bool_t        fModified;  //! Modified Folder Flag

public:
   FOPMTData( Short_t ADC_value=0,Short_t TDC_value=0 )
   {
      FOPMTData::Class()->IgnoreTObjectStreamer();
      ADC = ADC_value;
      TDC = TDC_value;
      fModified = false;
   };

   virtual ~FOPMTData()
   {
   };

   Short_t        GetADC()      { return ADC;      };
   Short_t        GetTDC()      { return TDC;      };

   Bool_t         isModified()  {
      if( fModified ) return true;
      return false;
   }

   void SetADC     (Short_t       ADC_value     ) { ADC      = ADC_value;      SetModified(true); };
   void SetTDC     (Short_t       TDC_value     ) { TDC      = TDC_value;      SetModified(true); };

   void AddADC     (Short_t       ADC_value     ) { ADC      += ADC_value;      SetModified(true); };
   void AddTDC     (Short_t       TDC_value     ) { TDC      += TDC_value;      SetModified(true); };

   void SetModified(Bool_t        modified) { fModified = modified;};

   void ResetModified() {
      int i=0;
      if(isModified()){
         SetModified(false);
      }
   };

   void SetAll( Short_t ADC_value=0,Short_t TDC_value=0 )
   { ADC = ADC_value; TDC = TDC_value; SetModified(true); };

   void Reset() {
      if( !isModified() ) return;
      int i=0;
      ADC = (Short_t)0;
      TDC = (Short_t)0;
      fModified = false;
   };

private:

   ClassDef(FOPMTData,1)
};

#endif   // FOPMTData_H
