#ifdef HT_UI_MSQU

#ifndef HT_UIMQ_CONSOLE
#define HT_UIMQ_CONSOLE

#include <stdio.h>
//char* DequeueMessage();
void startStdinThread();

FILE * DequeueMessageIntoTmpFile(FILE * fp);

#endif

#endif
