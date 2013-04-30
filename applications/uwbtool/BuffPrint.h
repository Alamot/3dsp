#ifndef BUSFPRINT_H
  #define BUSFPRINT_H

/************************************************************************** 
 * FILENAME:     BuffPrint.h          
 * CREATE DATE:  2008/09/05 
 * PURPOSE:      format print db key ! 
 *                ... 
 *  
 * AUTHORS:      3dsp <ritow.qi@3dsp.com.cn> 
 * 
 * NOTES:        description of constraints when using functions of this file 
 * 
 **************************************************************************/

#define VOID void
#define PUCHAR unsigned char *
#define ULONG unsigned long
#define DebugPrint printf

VOID BtPrintBuffer(PUCHAR buffer, ULONG len)
{
  ULONG i, j, k;

  i = len / 16;
  j = len % 16;
  k = 0;

  DebugPrint("Print Buffer = 0x%x  len = %ld:\n", (unsigned int)buffer, len);

  for (k = 0; k < i; k++)
  {
    DebugPrint
      ("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x\n",
       *(buffer + k * 16), *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
       *(buffer + k * 16 + 3), *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
       *(buffer + k * 16 + 6), *(buffer + k * 16 + 7), *(buffer + k * 16 + 8),
       *(buffer + k * 16 + 9), *(buffer + k * 16 + 10), *(buffer + k * 16 + 11),
       *(buffer + k * 16 + 12), *(buffer + k * 16 + 13),
       *(buffer + k * 16 + 14), *(buffer + k * 16 + 15));
  }

  switch (j)
  {
  case (0):
    break;
  case (1):
    DebugPrint("%02x\n", *(buffer + k * 16));
    break;
  case (2):
    DebugPrint("%02x %02x\n", *(buffer + k * 16), *(buffer + k * 16 + 1));
    break;
  case (3):
    DebugPrint("%02x %02x %02x\n", *(buffer + k * 16), *(buffer + k * 16 + 1),
               *(buffer + k * 16 + 2));
    break;
  case (4):
    DebugPrint("%02x %02x %02x %02x\n", *(buffer + k * 16),
               *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
               *(buffer + k * 16 + 3));
    break;
  case (5):
    DebugPrint("%02x %02x %02x %02x %02x\n", *(buffer + k * 16),
               *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
               *(buffer + k * 16 + 3), *(buffer + k * 16 + 4));
    break;
  case (6):
    DebugPrint("%02x %02x %02x %02x %02x %02x\n", *(buffer + k * 16),
               *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
               *(buffer + k * 16 + 3), *(buffer + k * 16 + 4),
               *(buffer + k * 16 + 5));
    break;
  case (7):
    DebugPrint("%02x %02x %02x %02x %02x %02x %02x \n", *(buffer + k * 16),
               *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
               *(buffer + k * 16 + 3), *(buffer + k * 16 + 4),
               *(buffer + k * 16 + 5), *(buffer + k * 16 + 6));
    break;
  case (8):
    DebugPrint("%02x %02x %02x %02x %02x %02x %02x %02x\n", *(buffer + k * 16),
               *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
               *(buffer + k * 16 + 3), *(buffer + k * 16 + 4),
               *(buffer + k * 16 + 5), *(buffer + k * 16 + 6),
               *(buffer + k * 16 + 7));
    break;
  case (9):
    DebugPrint("%02x %02x %02x %02x %02x %02x %02x %02x   %02x\n",
               *(buffer + k * 16), *(buffer + k * 16 + 1),
               *(buffer + k * 16 + 2), *(buffer + k * 16 + 3),
               *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
               *(buffer + k * 16 + 6), *(buffer + k * 16 + 7),
               *(buffer + k * 16 + 8));
    break;
  case (10):
    DebugPrint("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x\n",
               *(buffer + k * 16), *(buffer + k * 16 + 1),
               *(buffer + k * 16 + 2), *(buffer + k * 16 + 3),
               *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
               *(buffer + k * 16 + 6), *(buffer + k * 16 + 7),
               *(buffer + k * 16 + 8), *(buffer + k * 16 + 9));
    break;
  case (11):
    DebugPrint("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x\n",
               *(buffer + k * 16), *(buffer + k * 16 + 1),
               *(buffer + k * 16 + 2), *(buffer + k * 16 + 3),
               *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
               *(buffer + k * 16 + 6), *(buffer + k * 16 + 7),
               *(buffer + k * 16 + 8), *(buffer + k * 16 + 9),
               *(buffer + k * 16 + 10));
    break;
  case (12):
    DebugPrint
      ("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x\n",
       *(buffer + k * 16), *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
       *(buffer + k * 16 + 3), *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
       *(buffer + k * 16 + 6), *(buffer + k * 16 + 7), *(buffer + k * 16 + 8),
       *(buffer + k * 16 + 9), *(buffer + k * 16 + 10),
       *(buffer + k * 16 + 11));
    break;
  case (13):
    DebugPrint
      ("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x\n",
       *(buffer + k * 16), *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
       *(buffer + k * 16 + 3), *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
       *(buffer + k * 16 + 6), *(buffer + k * 16 + 7), *(buffer + k * 16 + 8),
       *(buffer + k * 16 + 9), *(buffer + k * 16 + 10), *(buffer + k * 16 + 11),
       *(buffer + k * 16 + 12));
    break;
  case (14):
    DebugPrint
      ("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x\n",
       *(buffer + k * 16), *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
       *(buffer + k * 16 + 3), *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
       *(buffer + k * 16 + 6), *(buffer + k * 16 + 7), *(buffer + k * 16 + 8),
       *(buffer + k * 16 + 9), *(buffer + k * 16 + 10), *(buffer + k * 16 + 11),
       *(buffer + k * 16 + 12), *(buffer + k * 16 + 13));
    break;
  default:
    DebugPrint
      ("%02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x\n",
       *(buffer + k * 16), *(buffer + k * 16 + 1), *(buffer + k * 16 + 2),
       *(buffer + k * 16 + 3), *(buffer + k * 16 + 4), *(buffer + k * 16 + 5),
       *(buffer + k * 16 + 6), *(buffer + k * 16 + 7), *(buffer + k * 16 + 8),
       *(buffer + k * 16 + 9), *(buffer + k * 16 + 10), *(buffer + k * 16 + 11),
       *(buffer + k * 16 + 12), *(buffer + k * 16 + 13),
       *(buffer + k * 16 + 14));
  }
}

#endif
