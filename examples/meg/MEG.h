#ifndef MEG_H
#define MEG_H

// Definitions
#define TOP     200
#define BOTTOM  201
#define FRONT   202
#define BACK    203
#define RIGHT   204
#define LEFT    205
#define NOFACE  206

#define INVALID -1000.f
#define SUCCESS 1
#define ERROR   0

// Photo Multiplier
#define gNumberOfPMT             257
#define gNumberOfFTDC             64
#define gNumberOfCTDC             80
#define gNumberOfVTDC            128
#define gNumberOfLXePMT          228
#define gPedestalThreshold       500.
#define gPedestalSpreadThreshold   1.
#define gNumberOfMapHistos        10

// Midas
#define gMaxMidasBankSize 10000

// midas bank structures
typedef struct {
   unsigned short int data     : 12;
   unsigned short int q1       : 4;
   unsigned short int channel  : 6;
   unsigned short int q2       : 1;
   unsigned short int range    : 1;
   unsigned short int event    : 3;
   unsigned short int geo_addr : 5;
} LRS1875_DATA;

typedef struct {
   unsigned short int data     : 12;
   unsigned short int overflow : 1;
   unsigned short int underflow: 1;
   unsigned short int valid    : 1;
   unsigned short int q1       : 1;
   unsigned short int channel  : 6;
   unsigned short int q2       : 2;
   unsigned short int tag      : 3;
   unsigned short int geo_addr : 5;
} CAEN775_DATA;

#endif   // MEG_H
