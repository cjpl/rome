#ifndef ROME_H
#define ROME_H

#if defined( _MSC_VER )
#define OS_WINNT
#endif

#if defined(OS_WINNT) && defined(ROMEDLL_EXPORTS)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

/**
toBool */
#ifndef toBool
#define toBool(a)            (a != 0)
#endif

// midas
#define EVENTID_BOR      ((short) 0x8000)  /**< Begin-of-run      */
#define EVENTID_EOR      ((short) 0x8001)  /**< End-of-run        */
#define EVENTID_MESSAGE  ((short) 0x8002)  /**< Message events    */
typedef struct {
   short int event_id;
   short int trigger_mask;
   unsigned long int serial_number;
   unsigned long int time_stamp;
   unsigned long int data_size;
} EVENT_HEADER;

// General
#define gDebugLevel 0

#define gShowTime 1
#define gPrintProgress 0

#define gFileNameLength 300

#define gMidasEventSize 1000000

#define gTaskInit       "init"
#define gTaskBeginOfRun "bof"
#define gTaskEvent      "evt"
#define gTaskEndOfRun   "eof"
#define gTaskTerminate  "term"

#endif   // ROME_H
