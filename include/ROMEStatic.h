#ifndef ROMEStatic_H
#define ROMEStatic_H
#include <TArrayI.h>
//#include <Riostream.h>
#include <strstream>
#include <stdio.h>

#if defined( _MSC_VER )
#include <conio.h>
#endif
#if defined ( __linux__ )
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#endif

#define ALIGN8(x)  (((x)+7) & ~7)

#include "ROME.h"

class ROMEStatic
{
public:
/********************************************************************\
    Routine: decodeRunNumbers
\********************************************************************/
   static TArrayI decodeRunNumbers(char *str)
   {
      char cminus='-';
      char ccomma=',';
      char csemi =';';
      char *pstr = str;
      int num;
      int na=0;
      int nat=1;
      int arraySize = 1000;
      TArrayI arr(arraySize);
      while (pstr<str+strlen(str)) {
         if (na>=arraySize*nat) {
            nat++;
            arr.Set(arraySize*nat);
         }
         num = strtol(pstr,&pstr,10);
         if (pstr[0]==cminus) {
            arr.AddAt(-num,na);
            na++;
            pstr++;
         }
         else if (pstr[0]==ccomma||pstr[0]==csemi) {
            arr.AddAt(num,na);
            na++;
            pstr++;
         }
         else {
            arr.AddAt(num,na);
            arr.Set(na+1);
            return arr;
         }
      }
      arr.Set(na);
      return arr;
   }

/********************************************************************\
    Routine: ss_kbhit
\********************************************************************/
   static Bool_t ss_kbhit()
   {
#if defined( _MSC_VER )
      return toBool(kbhit());
#endif
#if defined ( __linux__ )
      int n;
      ioctl(0, FIONREAD, &n);
      return (n > 0);
#endif
   }
/********************************************************************\
   bk_locate
/********************************************************************/

   typedef struct {
      unsigned long int data_size;                    /**< Size in bytes */
      unsigned long int flags;                        /**< internal flag */
   } BANK_HEADER;
   static bool bk_is32(void *event)
   {
      return ((((BANK_HEADER *) event)->flags & (1<<4)) > 0);
   }
   static int bk_locate(void *event, const char *name, void *pdata)
   {
      typedef struct {
         char name[4];                       /**< - */
         unsigned short int type;                          /**< - */
         unsigned short int data_size;                     /**< - */
      } BANK;

      typedef struct {
         char name[4];                       /**< - */
         unsigned long int type;                         /**< - */
         unsigned long int data_size;                    /**< - */
      } BANK32;
      int tid_size[] = {0,1,1,1,2,2,4,4,4,4,8,1,0,0,0,0,0};
      BANK *pbk;
      BANK32 *pbk32;
      unsigned long int dname;
   
      if (bk_is32(event)) {
         pbk32 = (BANK32 *) (((BANK_HEADER *) event) + 1);
         strncpy((char *) &dname, name, 4);
         do {
            if (*((unsigned long int *) pbk32->name) == dname) {
               *((void **) pdata) = pbk32 + 1;
               if (tid_size[pbk32->type & 0xFF] == 0)
                  return pbk32->data_size;
               return pbk32->data_size / tid_size[pbk32->type & 0xFF];
            }
            pbk32 = (BANK32 *) ((char *) (pbk32 + 1) + ALIGN8(pbk32->data_size));
         } while ((unsigned long int) pbk32 - (unsigned long int) event <
                  ((BANK_HEADER *) event)->data_size + sizeof(BANK_HEADER));
      } else {
         pbk = (BANK *) (((BANK_HEADER *) event) + 1);
         strncpy((char *) &dname, name, 4);
         do {
            if (*((unsigned long int *) pbk->name) == dname) {
               *((void **) pdata) = pbk + 1;
               if (tid_size[pbk->type & 0xFF] == 0)
                  return pbk->data_size;
               return pbk->data_size / tid_size[pbk->type & 0xFF];
            }
            pbk = (BANK *) ((char *) (pbk + 1) + ALIGN8(pbk->data_size));
         } while ((unsigned long int) pbk - (unsigned long int) event <
                  ((BANK_HEADER *) event)->data_size + sizeof(BANK_HEADER));
      }
   
      /* bank not found */
      *((void **) pdata) = NULL;
      return 0;
   }
};


#endif   // ROMEStatic_H
