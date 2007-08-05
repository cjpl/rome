// $Id$
#ifndef XMLToFormElementSignal_H
#define XMLToFormElementSignal_H

#include <TObject.h>
#include <TString.h>
#include "ROMEString.h"
#include "ROMEStrArray.h"

class XMLToFormElementSignal : public TObject
{
private:
   ROMEString   fSignal;            //!
   ROMEString   fSenderType;        //!
   ROMEStrArray fReceiverPath;      //!
   ROMEStrArray fReceiverType;      //!
   Int_t        fNumberOfReceivers; //!

public:
   XMLToFormElementSignal(const char* signal,const char* senderType)
   :TObject()
   ,fSignal(signal)
   ,fSenderType(senderType)
   ,fReceiverPath()
   ,fReceiverType()
   ,fNumberOfReceivers(0)
   {
   }
   virtual ~XMLToFormElementSignal() {}

   void AddReceiver(const char* receiverPath,const char* receiverType) {
      fReceiverPath.Add(receiverPath);
      fReceiverType.Add(receiverType);
      fNumberOfReceivers++;
   }
   Int_t GetNumberOfReceivers() const { return fNumberOfReceivers; }
   TString GetSignal() const { return fSignal; }
   TString GetSenderType() const { return fSenderType; }
   TString GetReceiverPathAt(Int_t i) const { return fReceiverPath[i]; }
   TString GetReceiverTypeAt(Int_t i) const { return fReceiverType[i]; }

   ClassDef(XMLToFormElementSignal, 0)
};
#endif   // XMLToFormElementSignal_H

