// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEMidasDAQ
//
//  Interface to the Midas System.
//
//  $Id: ROMEMidasDAQ.cpp 689 2005-11-04 10:23:49Z schneebeli_m $
//
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#include <stdlib.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   include <io.h>
#endif
#if defined( R__UNIX )
#   include <unistd.h>
#endif
#include <fcntl.h>
#include <ROMEMidasDAQ.h>
#include <ROMEUtilities.h>

#if defined( R__VISUAL_CPLUSPLUS )
#   define O_RDONLY_BINARY O_RDONLY | O_BINARY
#else
#   if defined( R__SEEK64 )
#      define O_RDONLY_BINARY O_RDONLY | O_LARGEFILE
#   else
#      define O_RDONLY_BINARY O_RDONLY
#   endif
#endif

#if defined( HAVE_MIDAS )
#   include <midas.h>
#   define MIDAS_DEBUG // define if you want to run the analyzer in the debugger
void ProcessMessage(int hBuf, int id, EVENT_HEADER * pheader, void *message)
{
// This method is called, when a system message from the online system occurs
}
#endif

ROMEMidasDAQ::ROMEMidasDAQ() {
   fStopRequest = false;
   fCurrentRawDataEvent = 0;
}

bool ROMEMidasDAQ::Init() {
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      // Connect to the Frontend
      int requestId,i;

      gROME->Println("Program is running online.\n");

      // Connect to the experiment
      if (cm_connect_experiment((char*)gROME->GetOnlineHost(), (char*)gROME->GetOnlineExperiment(),(char*)gROME->GetProgramName(), NULL) != SUCCESS) {
         gROME->Println("\nCannot connect to experiment");
         return false;
      }

      // regesters a disconnection to be executed when the program terminates normally.
      atexit((void (*)(void))cm_disconnect_experiment);

      // open the "system" buffer, 1M size
      bm_open_buffer("SYSTEM", EVENT_BUFFER_SIZE, &fMidasOnlineBuffer);

      // set the buffer cache size
      bm_set_cache_size(fMidasOnlineBuffer, 100000, 0);

      // place a request for a specific event id
      if (this->GetNumberOfEventRequests()<=0) {
         gROME->Println("\nNo Events Requests for online mode!");
         gROME->Println("\nPlace Events Requests into the ROME configuration file.");
         return false;
      }
      for (i=0;i<this->GetNumberOfEventRequests();i++) {
         bm_request_event(fMidasOnlineBuffer, this->GetEventRequestID(i),
            this->GetEventRequestMask(i),this->GetEventRequestRate(i), &requestId,NULL);
      }

      // place a request for system messages
      cm_msg_register(ProcessMessage);

      // turn off watchdog if in debug mode
#if defined( MIDAS_DEBUG )
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
         db_check_record(gROME->GetMidasOnlineDataBase(), 0, (char*)str.Data(), (char*)gROME->GetTreeObjectAt(i)->GetSwitchesString(), TRUE);
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
bool ROMEMidasDAQ::BeginOfRun() {
   if (gROME->isOffline()) {
      ROMEString filename;
      ROMEString gzfilename;
      ROMEString fileExtension = ".mid";
      ROMEString gzfileExtension = ".mid.gz";
      ROMEString runNumberString;

      this->SetAnalyze();
      this->SetRunning();
      // Open Midas File
      if (gROME->IsRunNumberBasedIO()) {
         gROME->GetCurrentRunNumberString(runNumberString);
         fGZippedMidasFile = false;
         filename.SetFormatted("%srun%s%s",gROME->GetInputDir(),runNumberString.Data(),fileExtension.Data());
         gzfilename.SetFormatted("%srun%s%s",gROME->GetInputDir(),runNumberString.Data(),gzfileExtension.Data());
         fMidasFileHandle = open(filename.Data(),O_RDONLY_BINARY);
         if (fMidasFileHandle == -1) {
            fMidasGzFileHandle = gzopen(gzfilename.Data(),"rb");
            if (fMidasGzFileHandle==NULL) {
               gROME->Print("Failed to open input file '");
               gROME->Print(filename.Data());
               gROME->Println("[.gz]'.");
               return false;
            }
            fGZippedMidasFile = true;
         }
      }
      else {
         filename.SetFormatted("%s%s",gROME->GetInputDir(),gROME->GetCurrentInputFileName().Data());
         if (filename.Index(".gz")!=-1) {
            gzfilename = filename;
            fGZippedMidasFile = true;
            fMidasGzFileHandle = gzopen(gzfilename.Data(),"rb");
            if (fMidasGzFileHandle==NULL) {
               gROME->Print("Failed to open input file '");
               gROME->Print(gzfilename.Data());
               gROME->Println("'.");
               return false;
            }
         }
         else {
            fGZippedMidasFile = false;
            fMidasFileHandle = open(filename.Data(),O_RDONLY_BINARY);
            if (fMidasFileHandle == -1) {
               gROME->Print("Failed to open input file '");
               gROME->Print(filename.Data());
               gROME->Println("'.");
               return false;
            }
         }
      }

      gROME->Print("Reading Midas-File ");
      if(!fGZippedMidasFile){
         gROME->Println(filename.Data());
      }
      else
         gROME->Println(gzfilename.Data());

      while (!isBeginOfRun())
         if (!Event(0))
            return false;
      SetAnalyze();
   }
   return true;
}
bool ROMEMidasDAQ::Event(int event) {
   // Switch Raw Data Buffer
   this->SwitchRawDataBuffer();

   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      int runNumber,trans;
      if (cm_query_transition(&trans, &runNumber, NULL) || fStopRequest) {
         if (trans == TR_START) {
            gROME->SetCurrentRunNumber(runNumber);
            this->SetBeginOfRun();
            this->SetRunning();
            return true;
         }
         if (trans == TR_STOP || fStopRequest) {
            fStopRequest = true;
            int numberOfBytes;
            bm_get_buffer_level(fMidasOnlineBuffer, &numberOfBytes);
            if (numberOfBytes <= 0) {
               this->SetEndOfRun();
               this->SetStopped();
               fStopRequest = false;
               return true;
            }
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
      int size = this->GetRawDataEventSize();
      void* mEvent = this->GetRawDataEvent();
      status = bm_receive_event(fMidasOnlineBuffer, mEvent, &size, ASYNC);
      if (status != BM_SUCCESS) {
         this->SetContinue();
         return true;
      }

      gROME->SetCurrentEventNumber(((EVENT_HEADER*)mEvent)->serial_number);
      gROME->SetEventID(((EVENT_HEADER*)mEvent)->event_id);
      fTimeStamp = ((EVENT_HEADER*)mEvent)->time_stamp;
#if !defined( R__BYTESWAP )
      //byte swapping
      if(((EVENT_HEADER*)mEvent)->event_id != EVENTID_BOR &&
         ((EVENT_HEADER*)mEvent)->event_id != EVENTID_EOR &&
         ((EVENT_HEADER*)mEvent)->event_id != EVENTID_MESSAGE)
         if(IsActiveEventID(((EVENT_HEADER*)mEvent)->event_id ))
            bk_swap((EVENT_HEADER*)mEvent + 1, 0);
#endif
      this->InitMidasBanks();
#endif
   }
   else if (gROME->isOffline()) {
      // read event header
      EVENT_HEADER *pevent = (EVENT_HEADER*)this->GetRawDataEvent();
      bool readError = false;

      // read event
      int n;
      if(!fGZippedMidasFile)
         n = read(fMidasFileHandle,pevent, sizeof(EVENT_HEADER));
      else
         n = gzread(fMidasGzFileHandle,pevent, sizeof(EVENT_HEADER));
      if (n < (int)sizeof(EVENT_HEADER)) readError = true;
      else {
#if !defined( R__BYTESWAP )
         //byte swapping
         ROMEUtilities::ByteSwap((UShort_t *)&pevent->event_id);
         ROMEUtilities::ByteSwap((UShort_t *)&pevent->trigger_mask);
         ROMEUtilities::ByteSwap((UInt_t   *)&pevent->serial_number);
         ROMEUtilities::ByteSwap((UInt_t   *)&pevent->time_stamp);
         ROMEUtilities::ByteSwap((UInt_t   *)&pevent->data_size);
#endif
         n = 0;
         if (pevent->data_size <= 0) readError = true;
         else {
            if(!fGZippedMidasFile)
               n = read(fMidasFileHandle,pevent+1,pevent->data_size);
            else
               n = gzread(fMidasGzFileHandle,pevent+1,pevent->data_size);
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
      // Get Handle to ODB header
      if (pevent->event_id == EVENTID_BOR) {
         if (gROME->isDataBaseActive("odb"))
            ((ROMEODBOfflineDataBase*)gROME->GetDataBase("ODB"))->SetBuffer((char*)(pevent+1));
         this->SetBeginOfRun();
         if (!gROME->IsRunNumberBasedIO()) {
            gROME->SetCurrentRunNumber(pevent->serial_number);
         }
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
#if !defined( R__BYTESWAP )
      //byte swapping
      if(pevent->event_id != EVENTID_BOR &&
         pevent->event_id != EVENTID_EOR &&
         pevent->event_id != EVENTID_MESSAGE)
         if(IsActiveEventID( pevent->event_id ))
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

      this->InitMidasBanks();
   }
   this->InitHeader();
   return true;
}
bool ROMEMidasDAQ::EndOfRun() {
   if (gROME->isOffline()) {
      if(!fGZippedMidasFile)
         close(fMidasFileHandle);
      else
         gzclose(fMidasGzFileHandle);
   }
   return true;
}
bool ROMEMidasDAQ::Terminate() {
   if (gROME->isOnline()) {
#if defined( HAVE_MIDAS )
      cm_disconnect_experiment();
#endif
   }
   return true;
}


#if !defined( R__BYTESWAP )
#   if !defined( HAVE_MIDAS )
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
void ROMEMidasDAQ::bk_swap(void *event, bool force)
{
   BANK_HEADER  *pbh;
   BANK         *pbk;
   BANK32       *pbk32;
   char         name[ 4 ];
   void         *pdata;
   UShort_t     type;
   bool         b32;

   pbh = (BANK_HEADER *) event;

   // only swap if flags in high 16-bit
   if (pbh->flags < 0x10000 && !force)
      return;

   // swap bank header
   ROMEUtilities::ByteSwap((UInt_t   *)&pbh->data_size);
   ROMEUtilities::ByteSwap((UInt_t   *)&pbh->flags);

   // check for 32bit banks
   b32 = ((pbh->flags & (1<<4)) > 0);

   pbk = (BANK *) (pbh + 1);
   pbk32 = (BANK32 *) pbk;

   // scan event
   while ((Seek_t) pbk - (Seek_t) pbh < (Int_t) pbh->data_size + (Int_t) sizeof(BANK_HEADER)) {
      // swap bank header
      if (b32) {
         ROMEUtilities::ByteSwap((UInt_t *)&pbk32->type);
         ROMEUtilities::ByteSwap((UInt_t *)&pbk32->data_size);
         pdata = pbk32 + 1;
         type = (UShort_t) pbk32->type;
         for ( long i = 0; i < 4; i++ )
            name[ i ] = pbk32->name[ i ];
      } else {
         ROMEUtilities::ByteSwap((UShort_t *)&pbk->type);
         ROMEUtilities::ByteSwap((UShort_t *)&pbk->data_size);
         pdata = pbk + 1;
         type = pbk->type;
         for ( long i = 0; i < 4; i++ )
            name[ i ] = pbk->name[ i ];
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
               ROMEUtilities::ByteSwap((UShort_t*)pdata);
               pdata = (void *) (((UShort_t *) pdata) + 1);
            }
            break;

         case TID_DWORD:
         case TID_INT:
         case TID_BOOL:
         case TID_FLOAT:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ROMEUtilities::ByteSwap((UInt_t*)pdata);
               pdata = (void *) (((UInt_t *) pdata) + 1);
            }
            break;

         case TID_DOUBLE:
            while ((Seek_t) pdata < (Seek_t) pbk) {
               ROMEUtilities::ByteSwap((ULong64_t*)pdata);
               pdata = (void *) (((ULong64_t *) pdata) + 1);
            }
            break;

         case TID_STRUCT:
            while ( (Seek_t) pdata < (Seek_t) pbk ) {
                pdata = ByteSwapStruct( &name[ 0 ], pdata );
            }
            break;
      }
   }
   return;
}
#   endif

#endif
#if !defined( HAVE_MIDAS )
bool ROMEMidasDAQ::bk_is32(void *event)
{
   return ((((BANK_HEADER *) event)->flags & (1<<4)) > 0);
}

int ROMEMidasDAQ::bk_find(void* pbkh, const char *name, unsigned long * bklen, unsigned long * bktype,void *pdata)
{
   int tid_size[] = {0,1,1,1,2,2,4,4,4,4,8,1,0,0,0,0,0};
   BANK *pbk;
   BANK32 *pbk32;
   unsigned long dname;

   if (bk_is32(pbkh)) {
      pbk32 = (BANK32 *) (((BANK_HEADER *)pbkh) + 1);
      strncpy((char *) &dname, name, 4);
      do {
         if (*((unsigned long *) pbk32->name) == dname) {
            *((void **) pdata) = pbk32 + 1;
            if (tid_size[pbk32->type & 0xFF] == 0)
               *bklen = pbk32->data_size;
            else
               *bklen = pbk32->data_size / tid_size[pbk32->type & 0xFF];

            *bktype = pbk32->type;
            return 1;
         }
         pbk32 = (BANK32 *) ((char *) (pbk32 + 1) + ALIGN8(pbk32->data_size));
      } while ((unsigned long) pbk32 - (unsigned long) pbkh < ((BANK_HEADER *) pbkh)->data_size + sizeof(BANK_HEADER));
   } else {
      pbk = (BANK *) (((BANK_HEADER *)pbkh) + 1);
      strncpy((char *) &dname, name, 4);
      do {
         if (*((unsigned long *) pbk->name) == dname) {
            *((void **) pdata) = pbk + 1;
            if (tid_size[pbk->type & 0xFF] == 0)
               *bklen = pbk->data_size;
            else
               *bklen = pbk->data_size / tid_size[pbk->type & 0xFF];

            *bktype = pbk->type;
            return 1;
         }
         pbk = (BANK *) ((char *) (pbk + 1) + ALIGN8(pbk->data_size));
      } while ((unsigned long) pbk - (unsigned long) pbkh < ((BANK_HEADER *) pbkh)->data_size + sizeof(BANK_HEADER));
   }
   *((void **) pdata) = NULL;
   return 0;
}
#endif
