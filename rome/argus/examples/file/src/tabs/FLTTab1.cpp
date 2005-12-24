// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FLTTab1                                                                    //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ArgusBuilder. Manual changes above the       *
 * following line will be lost next time ArgusBuilder is executed.            */
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/FLTTab1.h"
#include "include/generated/FLWindow.h"

ClassImp(FLTTab1)

void FLTTab1::Init()
{
   fRunNumber = gMonitor->GetCurrentRunNumber();

   // Create "run number" text entry 
   fTextRun = new TGTextEntry(this, "     ");
   fTextRun->SetMaxLength(5);
   fTextRun->Associate(this);

   // Create "event number" text entry 
   fTextEvent = new TGTextEntry(this, "     ");
   fTextEvent->SetMaxLength(5);
   fTextEvent->Associate(this);

   // Create "value number" text entry 
   fTextValue = new TGTextEntry(this, "     ");
   fTextValue->SetMaxLength(5);
   fTextValue->Associate(this);

   // Create "Update" button
   fBUpdate = new TGTextButton(this, "Update", B_UPDATE);
   fBUpdate->Associate(this);

   fLabelRun = new TGLabel(this, "Run Number");
   fLabelEvent = new TGLabel(this, "Event Number");
   fLabelValue = new TGLabel(this, "Value");

   Char_t str[1024];
   sprintf(str, "%d", gMonitor->GetCurrentRunNumber());
   fTextRun->SetText(str);
   sprintf(str, "%d", gMonitor->GetCurrentEventNumber());
   fTextEvent->SetText(str);
   sprintf(str, "%d", gMonitor->GetTest()->Getvalue());
   fTextValue->SetText(str);

   AddFrame(fLabelRun, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
   AddFrame(fTextRun, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
   AddFrame(fLabelEvent, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
   AddFrame(fTextEvent, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
   AddFrame(fLabelValue, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
   AddFrame(fTextValue, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
   AddFrame(fBUpdate, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
}

void FLTTab1::MenuClicked(TGPopupMenu *menu, Long_t param)
{
}

void FLTTab1::TabSelected()
{
}
void FLTTab1::TabUnSelected()
{
}

void FLTTab1::Update()
{
   Int_t run, event, value;
   Char_t str[1024];

   // read run and event number from tab
   run = atoi(fTextRun->GetText());
   // currently, only sequensial read from event-0 is supported. Random access will be supported soon.
   // event = atoi(fTextEvent->GetText());
   event = gMonitor->GetCurrentEventNumber();

   // change reading file when run number is changed
   if (run != fRunNumber) {
      gMonitor->GetActiveDAQ()->EndOfRun();
      gMonitor->SetCurrentRunNumber(run);
      if (!gMonitor->GetActiveDAQ()->BeginOfRun()) {
         fTextValue->SetText("error");
         // currently, only sequensial read from event-0 is supported. Random access will be supported soon.
         gMonitor->SetCurrentEventNumber(0);
         return;
      }
      // currently, only sequensial read from event-0 is supported. Random access will be supported soon.
      event = 0;
      fRunNumber = run;
   }
   // read data from file
   gMonitor->GetActiveDAQ()->Event(event);
   sprintf(str, "%d", gMonitor->GetTest()->Getvalue());
   fTextValue->SetText(str);

   // increment event number
   gMonitor->SetCurrentEventNumber(event + 1);
   sprintf(str, "%d", gMonitor->GetCurrentEventNumber());
   fTextEvent->SetText(str);

   return;
}

bool FLTTab1::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)
{
   // Process messages coming from widgets associated with the dialog.  
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1) {
         case B_UPDATE:
            Update();
            break;
         }
         break;
      }
      break;
   }
   return true;
}
