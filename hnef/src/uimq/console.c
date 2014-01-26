#include "console.h"

#include <pthread.h>
#include <stdlib.h>
#include "queue.h"

/** A handle to the thread that handles messages. */
//static pthread_t g_handle_message_thread;

/** A worker thread that handles messages from JavaScript.
 * Currently it reads them from stdin and puts into tmp file.
 * @param[in] user_data Unused.
 * @return unused. */
 /*
void* HandleMessageThread(void* user_data) {
while (1) {
   char * str = malloc(60);

   if( fgets (str, 60, stdin)!=NULL ) {
      EnqueueMessage(str);
   }
}
}*/

FILE * DequeueMessageIntoTmpFile(FILE * fp)
{
   char * msg = DequeueMessage(); 
   fputs(msg, fp);
   fseek(fp, 0, 0);
   puts(msg);
   free(msg);
   return fp;
}

void startStdinThread() {
	//pthread_create(&g_handle_message_thread, NULL, &HandleMessageThread, NULL);
}
