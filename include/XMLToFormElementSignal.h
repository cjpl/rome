// $Id: XMLToFormElementSignal.h 1041 2006-05-04 18:20:33Z sawada $
#ifndef XMLToFormElementSignal_H
#define XMLToFormElementSignal_H

#include <TObjArray.h>
#include <TArrayI.h>
#include "ROMEStr2DArray.h"

class XMLToFormElementSignal : public TObject
{
private:
   ROMEString   fSignal;  //!
   ROMEString   fSenderType;  //!
   ROMEStrArray fReceiverPath;  //!
   ROMEStrArray fReceiverType;  //!
   int fNumberOfReceivers;  //!
public:
   XMLToFormElementSignal(const char* signal,const char* senderType) { 
      fSignal = signal;
      fSenderType = senderType;
      fNumberOfReceivers = 0; 
   };
   void AddReceiver(const char* receiverPath,const char* receiverType) {
      fReceiverPath.Add(receiverPath);
      fReceiverType.Add(receiverType);
      fNumberOfReceivers++;
   };
   int GetNumberOfReceivers() { return fNumberOfReceivers; };
   TString GetSignal() { return fSignal; };
   TString GetSenderType() { return fSenderType; };
   TString GetReceiverPathAt(int i) { return fReceiverPath[i]; };
   TString GetReceiverTypeAt(int i) { return fReceiverType[i]; };

   ClassDef(XMLToFormElementSignal, 0)
};
#endif   // XMLToFormElementSignal_H

