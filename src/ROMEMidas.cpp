// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEMidas
//
//  Interface to the Midas System.
//
//  $Log$
//  Revision 1.6  2005/03/18 16:12:57  schneebeli_m
//  Event request & Histo in romeConfig
//
//  Revision 1.5  2005/02/21 21:29:07  sawada
//
//  Changed OS specifying macros
//  Support for DEC,Ultrix,FreeBSD,Solaris
//
//  Revision 1.4  2005/02/20 19:17:22  sawada
//  Large file support
//
//  Revision 1.3  2005/02/06 01:26:55  sawada
//  moved byte swapping functions to ROMEMidas
//
//  Revision 1.2  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.1  2005/01/24 15:25:09  schneebeli_m
//  Seperated DAQ classes
//
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#include <io.h>
#define O_RDONLY_BINARY O_RDONLY | O_BINARY
#endif
#if defined( R__UNIX )
#include <unistd.h>
#define O_RDONLY_BINARY O_RDONLY
#endif
#include <fcntl.h>
#include <ROMEMidas.h>

#if defined( HAVE_MIDAS )
#include <midas.h>
#define MIDAS_DEBUG // define if you want to run the analyzer in the debugger
void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
// This method is called, when a system message from the online system occurs
}
#endif

ROMEMidas::ROMEMidas() {
}

bool ROMEMidas::Initialize() {
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      // Connect to the Frontend
      int requestId,i;

      gROME->Println("Program is running online.\n");

      // Connect to the experiment
      if (cm_connect_experiment(gROME->GetOnlineHost(), gROME->GetOnlineExperiment(),gROME->GetProgramName(), NULL) != SUCCESS) {
         gROME->Println("\nCannot connect to experiment");
         return false;
      }

      // open the "system" buffer, 1M size
      bm_open_buffer("SYSTEM", EVENT_BUFFER_SIZE, &fMidasOnlineBuffer);

      // set the buffer cache size
      bm_set_cache_size(fMidasOnlineBuffer, 100000, 0);

      // place a request for a specific event id
      if (gROME->GetNumberOfEventRequests()<=0) {
         gROME->Println("\nNo Events Requests for online mode!");
         gROME->Println("\nPlace Events Requests into the ROME configuration file.");
         return false;
      }
      for (i=0;i<gROME->GetNumberOfEventRequests();i++) {
         bm_request_event(fMidasOnlineBuffer, gROME->GetEventRequestID(i), 
            gROME->GetEventRequestMask(i),gROME->GetEventRequestRate(i), &requestId,NULL);
      }

      // place a request for system messages
      cm_msg_register(ProcessMessage);

      // turn off watchdog if in debug mode
#ifdef MIDAS_DEBUG
      cm_set_watchdog_params(TRUE, 0);
#endif

      // Registers a callback function for run transitions.
      if (cm_register_transition(TR_START, NULL ,500) != CM_SUCCESS ||
         cm_register_transition(TR_STOP, NULL, 500) != CM_SUCCESS) {
         gROME->Println("\nCannot connect to experiment");
         return false;
      }

      // Connect to the online database
      if (cm_get_experiment_database(gROME->GetMidasOnlineDataBasePointer(), NULL)!= CM_SUCCESS) {
         gROME->Println("\nCannot connect to the online database");
         return false;
      }

      // Check Run Status
      int state = 0;
      int statesize = sizeof(state);
      if (db_get_value(gROME->GetMidasOnlineDataBase(),0,"/Runinfo/State",&state,&statesize,TID_INT,false)!= CM_SUCCESS) {
         gROME->Println("\nCannot read run status from the online database");
         return false;
      }
      if (state!=3) {
         this->SetBeginOfRun();
         this->SetStopped();
      }

      // Get Runnumber
      int runNumber = 0;
      int size = sizeof(runNumber);
      if (db_get_value(gROME->GetMidasOnlineDataBase(),0,"/Runinfo/Run number",&runNumber,&size,TID_INT,false)!= CM_SUCCESS) {
         gROME->Println("\nCannot read runnumber from the online database");
         return false;
      }
      gROME->SetCurrentRunNumber(runNumber);

      // Initialize the online database
      HNDLE hKey;
      ROMEString str;
      str = "//Trigger/Statistics";
      str.Insert(1,gROME->GetProgramName());
      // Trigger Statistics
      char *triggerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), triggerStatisticsString, TRUE);
      db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTriggerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         gROME->Println("\nCannot open trigger statistics record, probably other analyzer is using it");
         return false;
      }

      // Scaler Statistics
      char *fScalerStatisticsString =  "Events received = DOUBLE : 0\nEvents per sec. = DOUBLE : 0\nEvents written = DOUBLE : 0\n";
      str="//Scaler/Statistics";
      str.Insert(1,gROME->GetProgramName());
      db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), fScalerStatisticsString, TRUE);
      db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
      if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetScalerStatistics(), sizeof(Statistics), MODE_WRITE, NULL, NULL) != DB_SUCCESS) {
         gROME->Println("\nCannot open scaler statistics record, probably other analyzer is using it");
         return false;
      }

      // Tree Switches
      for (i=0;i<gROME->GetTreeObjectEntries();i++) {
         str="//Tree switches/";
         str.Insert(1,gROME->GetProgramName());
         str.Append(gROME->GetTreeObjectAt(i)->GetTree()->GetName());
         db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), gROME->GetTreeObjectAt(i)->GetSwitchesString(), TRUE);
         db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);
         if (db_set_record(gROME->GetMidasOnlineDataBase(),hKey,gROME->GetTreeObjectAt(i)->GetSwitches(),gROME->GetTreeObjectAt(i)->GetSwitchesSize(),0) != DB_SUCCESS) {
            gROME->Println("\nCannot write to tree switches record.");
            return false;
         }
         if (db_open_record(gROME->GetMidasOnlineDataBase(), hKey, gROME->GetTreeObjectAt(i)->GetSwitches(), gROME->GetTreeObjectAt(i)->GetSwitchesSize(), MODE_READ, NULL, NULL) != DB_SUCCESS) {
            gROME->Println("\nCannot open tree switches record, probably other analyzer is using it");
            return false;
         }
      }

      // Experiment dependent ODB initializations
      this->InitODB();

#else
      gROME->Println("Need Midas support for Online Mode !!");
      gROME->Println("Please link the midas library into this project.");
      return false;
#endif
   }
   else if (gROME->isOffline()) {
      gROME->Println("Program is running offline.\n");
   }
   return true;
}
bool ROMEMidas::Connect() {
   if (gROME->isOffline()) {
      // Open Midas File
      ROMEString runNumberString;
      gROME->GetCurrentRunNumberString(runNumberString);
      ROMEString filename;
      filename.SetFormatted("%srun%s.mid",gROME->GetInputDir(),runNumberString.Data());
#if defined( R__SEEK64 )
      fMidasFileHandle = open64(filename.Data(),O_RDONLY_BINARY);
#else
      fMidasFileHandle = open(filename.Data(),O_RDONLY_BINARY);
#endif
      if (fMidasFileHandle==-1) {
         gROME->Print("Inputfile '");
         gROME->Print(filename.Data());
         gROME->Println("' not found.");
         return false;
      }
      gROME->Print("Reading Midas-File run");
      gROME->Print(runNumberString.Data());
      gROME->Println(".mid");
   }
   return true;
}
bool ROMEMidas::ReadEvent(int event) {
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      int runNumber,trans;
      if (cm_query_transition(&trans, &runNumber, NULL)) {
         if (trans == TR_START) {
            gROME->SetCurrentRunNumber(runNumber);
            this->SetBeginOfRun();
            this->SetRunning();
            return true;
         }
         if (trans == TR_STOP) {
            this->SetEndOfRun();
            this->SetStopped();
            return true;
         }
      }
      int status = cm_yield(100);
      if (status == RPC_SHUTDOWN || status == SS_ABORT) {
         this->SetTerminate();
         return false;
      }
      if (this->isStopped()) {
         this->SetContinue();
         return true;
      }
      int size = gROME->GetRawDataEventSize();
      void* mEvent = gROME->GetRawDataEvent();
      status = bm_receive_event(fMidasOnlineBuffer, mEvent, &size, ASYNC);
      if (status != BM_SUCCESS) {
         this->SetContinue();
         return true;
      }

      gROME->SetCurrentEventNumber(((EVENT_HEADER*)mEvent)->serial_number);
      gROME->SetEventID(((EVENT_HEADER*)mEvent)->event_id);
      fTimeStamp = ((EVENT_HEADER*)mEvent)->time_stamp;
#ifndef R__BYTESWAP
      //byte swapping
      if(((EVENT_HEADER*)mEvent)->event_id != EVENTID_BOR &&
              ((EVENT_HEADER*)mEvent)->event_id != EVENTID_EOR &&
              ((EVENT_HEADER*)mEvent)->event_id != EVENTID_MESSAGE)
              bk_swap((EVENT_HEADER*)mEvent + 1, 0);
#endif
      gROME->InitMidasBanks();

#endif
   }
   else if (gROME->isOffline()) {
      // read event header
      EVENT_HEADER *pevent = (EVENT_HEADER*)gROME->GetRawDataEvent();
      bool readError = false;

      // read event
      int n = read(fMidasFileHandle,pevent, sizeof(EVENT_HEADER));
      
      if (n < (int)sizeof(EVENT_HEADER)) readError = true;
      else {
#ifndef R__BYTESWAP
         //byte swapping
         ByteSwap((UShort_t *)&pevent->event_id);
         ByteSwap((UShort_t *)&pevent->trigger_mask);
         ByteSwap((UInt_t   *)&pevent->serial_number);
         ByteSwap((UInt_t   *)&pevent->time_stamp);
         ByteSwap((UInt_t   *)&pevent->data_size);
#endif
         n = 0;
         if (pevent->data_size <= 0) readError = true;
         else {
            n = read(fMidasFileHandle, pevent+1, pevent->data_size);
            if (n != (int) pevent->data_size) readError = true;
//            if ((int) ((BANK_HEADER*)(pevent+1))->data_size <= 0) readError = true;
         }
      }
      // check input
      if (readError) {
         if (n > 0) gROME->Println("Unexpected end of file");
         this->SetEndOfRun();
         return true;
      }
      if (pevent->event_id < 0) {
         this->SetContinue();
         return true;
      }
      if (pevent->event_id == EVENTID_EOR) {
         this->SetEndOfRun();
         return true;
      }
#ifndef R__BYTESWAP
      //byte swapping
      if(pevent->event_id != EVENTID_BOR &&
         pevent->event_id != EVENTID_EOR &&
         pevent->event_id != EVENTID_MESSAGE)
         bk_swap(pevent + 1, 0);
#endif
      if (pevent->data_size<((BANK_HEADER*)(pevent+1))->data_size) {
         this->SetContinue();
         return true;
      }

      // initalize event
      gROME->SetEventID(pevent->event_id);
      gROME->SetCurrentEventNumber(pevent->serial_number);
      fTimeStamp = pevent->time_stamp;

      gROME->InitMidasBanks();
   }
   return true;
}
bool ROMEMidas::Disconnect() {
   if (gROME->isOffline()) {
      close(fMidasFileHandle);
   }
   return true;
}
bool ROMEMidas::Termination() {
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      cm_disconnect_experiment();
#endif
   }
   return true;
}


#ifndef R__BYTESWAP
#ifndef HAVE_MIDAS
/**
Swaps bytes from little endian to big endian or vice versa for a whole event.

An event contains a flag which is set by bk_init() to identify
the endian format of an event. If force is FALSE, this flag is evaluated and
the event is only swapped if it is in the "wrong" format for this system.
An event can be swapped to the "wrong" format on purpose for example by a
front-end which wants to produce events in a "right" format for a back-end
analyzer which has different byte ordering.
@param event pointer to data area of event
@param force If TRUE, the event is always swapped, if FALSE, the event
is only swapped if it is in the wrong format.
@return 1==event has been swap, 0==event has not been swapped.
*/
void ROMEMidas::bk_swap(void *event, bool force)
{
   BANK_HEADER *pbh;
   BANK *pbk;
   BANK32 *pbk32;
   void *pdata;
   UShort_t type;
   bool b32;
   
   pbh = (BANK_HEADER *) event;
   
   // only swap if flags in high 16-bit
   if (pbh->flags < 0x10000 && !force)
      return;
   
   // swap bank header 
   ByteSwap((UInt_t   *)&pbh->data_size);
   ByteSwap((UInt_t   *)&pbh->flags);   
   
   // check for 32bit banks
   b32 = ((pbh->flags & (1<<4)) > 0);
   
   pbk = (BANK *) (pbh + 1);
   pbk32 = (BANK32 *) pbk;
   
   // scan event
   while ((Seek_t) pbk - (Seek_t) pbh < (Int_t) pbh->data_size + (Int_t) sizeof(BANK_HEADER)) {
      // swap bank header
      if (b32) {
         ByteSwap((UInt_t *)&pbk32->type);
         ByteSwap((UInt_t *)&pbk32->data_size);
         pdata = pbk32 + 1;
         type = (UShort_t) pbk32->type;
      } else {
         ByteSwap((UShort_t *)&pbk->type);
         ByteSwap((UShort_t *)&pbk->data_size);
         pdata = pbk + 1;
         type = pbk->type;
      }
      
      // pbk points to next bank
      if (b32) {
         pbk32 = (BANK32 *) ((char *) (pbk32 + 1) + ALIGN8(pbk32->data_size));
         pbk = (BANK *) pbk32;
      } else {
         pbk = (BANK *) ((char *) (pbk + 1) + ALIGN8(pbk->data_size));
         pbk32 = (BANK32 *) pbk;
      }
      
      switch (type) {
         case TID_WORD:
         case TID_SHORT:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ByteSwap((UShort_t*)pdata);
               pdata = (void *) (((UShort_t *) pdata) + 1);
            }
            break;
            
         case TID_DWORD:
         case TID_INT:
         case TID_BOOL:
         case TID_FLOAT:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ByteSwap((UInt_t*)pdata);
               pdata = (void *) (((UInt_t *) pdata) + 1);
            }
            break;
            
         case TID_DOUBLE:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ByteSwap((ULong64_t*)pdata);
               pdata = (void *) (((ULong64_t *) pdata) + 1);
            }
            break;
      }
   }   
   return;
}
#endif

// Byte swapping big endian <-> little endian
void ROMEMidas::ByteSwap(UShort_t *x) 
{
   Byte_t _tmp;
   _tmp= *((Byte_t *)(x));
   *((Byte_t *)(x)) = *(((Byte_t *)(x))+1);
   *(((Byte_t *)(x))+1) = _tmp;
}

void ROMEMidas::ByteSwap(UInt_t *x) 
{
   Byte_t _tmp;
   _tmp= *((Byte_t *)(x));
   *((Byte_t *)(x)) = *(((Byte_t *)(x))+3);
   *(((Byte_t *)(x))+3) = _tmp;
   _tmp= *(((Byte_t *)(x))+1);
   *(((Byte_t *)(x))+1) = *(((Byte_t *)(x))+2); 
   *(((Byte_t *)(x))+2) = _tmp;
}

void ROMEMidas::ByteSwap(ULong64_t *x) { 
   Byte_t _tmp;
   _tmp= *((Byte_t *)(x));
   *((Byte_t *)(x)) = *(((Byte_t *)(x))+7);
   *(((Byte_t *)(x))+7) = _tmp;
   _tmp= *(((Byte_t *)(x))+1);
   *(((Byte_t *)(x))+1) = *(((Byte_t *)(x))+6);
   *(((Byte_t *)(x))+6) = _tmp;
   _tmp= *(((Byte_t *)(x))+2);
   *(((Byte_t *)(x))+2) = *(((Byte_t *)(x))+5);
   *(((Byte_t *)(x))+5) = _tmp;
   _tmp= *(((Byte_t *)(x))+3);
   *(((Byte_t *)(x))+3) = *(((Byte_t *)(x))+4);
   *(((Byte_t *)(x))+4) = _tmp;
}
#endif
