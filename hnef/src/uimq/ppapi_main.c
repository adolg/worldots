#ifdef HT_UI_MSQU

#include "ppapi_main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <pthread.h>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"
#include "nacl_io/nacl_io.h"

#include "queue.h"
#include "func.h"		/* fr_* */
#include "game.h"		/* game, alloc_game, free_game, ... */
#include "invoc.h"		/* invoc_read */
#include "iogen.h"		/* print_err, print_inv */
#include "iotextparse.h"	/* alloc_ht_line, free_ht_line */
#include "rreader.h"		/* alloc_game_read */
#include "rvalid.h"		/* ruleset_valid */
#include "uimq.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static pthread_t g_handle_message_thread;
static PP_Instance g_instance = 0;
static PPB_GetInterface get_browser_interface = NULL;
static PPB_Messaging* ppb_messaging_interface = NULL;
static PPB_Var* ppb_var_interface = NULL;

static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[]) {
  g_instance = instance;
  nacl_io_init_ppapi(instance, get_browser_interface);

  // By default, nacl_io mounts / to pass through to the original NaCl
  // filesystem (which doesn't do much). Let's remount it to a memfs
  // filesystem.
  umount("/");
  mount("", "/", "memfs", 0, "");

  mount("",                                       /* source */
        "/persistent",                            /* target */
        "html5fs",                                /* filesystemtype */
        0,                                        /* mountflags */
        "type=PERSISTENT,expected_size=1048576"); /* data */

  mount("",       /* source. Use relative URL */
        "/http",  /* target */
        "httpfs", /* filesystemtype */
        0,        /* mountflags */
        "");      /* data */

  fprintf(stdout,"logmsg: %s\n","InitializeMessageQueue()");
  InitializeMessageQueue();
  pthread_create(&g_handle_message_thread, NULL, &game_main, NULL);
  // TODO setup rules, etc
  // TODO game_main() will block somewhere inside uimq_play_new(), trying to dequeue
  // a message. We need to initialize game object and wait for commands, instead -
  // so game_main() should return immediately after reading the rules!
  //game_main();
  return PP_TRUE;
}

static void Instance_DidDestroy(PP_Instance instance) {
	// send quit message to game thread, so it frees its resources!
}

static void Instance_DidChangeView(PP_Instance instance,
                                   PP_Resource view_resource) {}

static void Instance_DidChangeFocus(PP_Instance instance, PP_Bool has_focus) {}

static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance,
                                           PP_Resource url_loader) {
  /* NaCl modules do not need to handle the document load function. */
  return PP_FALSE;
}

static void Messaging_HandleMessage(PP_Instance instance,
                                    struct PP_Var message) {
  char buffer[1024];
  VarToCStr(message, &buffer[0], 1024);
  if (!EnqueueMessage(strdup(buffer))) {
    struct PP_Var var;
    var = PrintfToVar(
        "Warning: dropped message \"%s\" because the queue was full.", message);
    ppb_messaging_interface->PostMessage(g_instance, var);
  }
}

void PostMessage(char * msgType, char * message) {
    struct PP_Var var;
    var = PrintfToVar(
        "%s:%s", msgType, message);

    ppb_messaging_interface->PostMessage(g_instance, var);
}

PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id,
                                       PPB_GetInterface get_browser) {
  get_browser_interface = get_browser;
  ppb_messaging_interface =
      (PPB_Messaging*)(get_browser(PPB_MESSAGING_INTERFACE));
  ppb_var_interface = (PPB_Var*)(get_browser(PPB_VAR_INTERFACE));
  return PP_OK;
}

PP_EXPORT const void* PPP_GetInterface(const char* interface_name) {
  if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
    static PPP_Instance instance_interface = {
      &Instance_DidCreate,
      &Instance_DidDestroy,
      &Instance_DidChangeView,
      &Instance_DidChangeFocus,
      &Instance_HandleDocumentLoad,
    };
    return &instance_interface;
  } else if (strcmp(interface_name, PPP_MESSAGING_INTERFACE) == 0) {
    static PPP_Messaging messaging_interface = {
      &Messaging_HandleMessage,
    };
    return &messaging_interface;
  }
  return NULL;
}

PP_EXPORT void PPP_ShutdownModule() {}

/**
 * Printf to a new PP_Var.
 * @param[in] format A print format string.
 * @param[in] ... The printf arguments.
 * @return A new PP_Var. */
struct PP_Var PrintfToVar(const char* format, ...) {
  if (ppb_var_interface != NULL) {
    char* string;
    va_list args;
    struct PP_Var var;

    va_start(args, format);
    string = VprintfToNewString(format, args);
    va_end(args);

    var = ppb_var_interface->VarFromUtf8(string, strlen(string));
    free(string);

    return var;
  }

  return PP_MakeUndefined();
}

/**
 * Convert a PP_Var to a C string, given a buffer.
 * @param[in] var The PP_Var to convert.
 * @param[out] buffer The buffer to write to.
 * @param[in] length The length of |buffer|.
 * @return The number of characters written. */
uint32_t VarToCStr(struct PP_Var var, char* buffer, uint32_t length) {
  if (ppb_var_interface != NULL) {
    uint32_t var_length;
    const char* str = ppb_var_interface->VarToUtf8(var, &var_length);
    /* str is NOT NULL-terminated. Copy using memcpy. */
    uint32_t min_length = MIN(var_length, length - 1);
    memcpy(buffer, str, min_length);
    buffer[min_length] = 0;

    return min_length;
  }

  return 0;
}

/**
 * Create a new PP_Var from a C string.
 * @param[in] str The string to convert.
 * @return A new PP_Var with the contents of |str|. */
struct PP_Var CStrToVar(const char* str) {
  if (ppb_var_interface != NULL) {
    return ppb_var_interface->VarFromUtf8(str, strlen(str));
  }
  return PP_MakeUndefined();
}

/**
 * Printf to a newly allocated C string.
 * @param[in] format A printf format string.
 * @param[in] args The printf arguments.
 * @return The newly constructed string. Caller takes ownership. */
char* VprintfToNewString(const char* format, va_list args) {
  va_list args_copy;
  int length;
  char* buffer;
  int result;

  va_copy(args_copy, args);
  length = vsnprintf(NULL, 0, format, args);
  buffer = (char*)malloc(length + 1); /* +1 for NULL-terminator. */
  result = vsnprintf(&buffer[0], length + 1, format, args_copy);
  if (result != length) {
    assert(0);
    return NULL;
  }
  return buffer;
}

/**
 * Printf to a newly allocated C string.
 * @param[in] format A print format string.
 * @param[in] ... The printf arguments.
 * @return The newly constructed string. Caller takes ownership. */
char* PrintfToNewString(const char* format, ...) {
  va_list args;
  char* result;
  va_start(args, format);
  result = VprintfToNewString(format, args);
  va_end(args);
  return result;
}

void game_do_main()
{
	enum HT_FR		fr		= HT_FR_NONE;
	//enum HT_FAIL_INVOC	fail_inv	= HT_FAIL_INVOC_NONE;
	enum HT_FAIL_RREADER	fail_read	= HT_FAIL_RREADER_NONE;
	struct game *		g		= NULL;
	struct invoc *		inv		= NULL;
	struct ht_line *	line		= NULL;

	// TODO initialize inv!
	inv	= malloc(sizeof(* inv));
	inv->play = HT_TRUE;
	inv->info.play_ui = HT_UIT_MSQU;
	inv->file_rc = "/http/rules/tablut";
	/*
	 * Used in alloc_game_read, to get bad command on failure.
	 */
	line	= alloc_ht_line((size_t)3, (size_t)18);
	if (NULL == line)
	{
		free			(inv);
		(void) print_err_fr	(stderr, HT_FR_FAIL_ALLOC);
		return;
	}
	FILE* fp = fopen("/hneferr.tmp" , "w+");
	g = alloc_game_read(inv->file_rc, line, & fr, & fail_read);
	if (HT_FR_SUCCESS != fr)
	{
		(void) print_err_fr_rreader(fp, fr, fail_read, line);
		fseek(fp, 0, 0);
		char * str = malloc(60);
		   if( fgets (str, 60, fp)!=NULL ) {
			    struct PP_Var var;
			    var = PrintfToVar(
			        "log:from hnef: %s.", str);

			    ppb_messaging_interface->PostMessage(g_instance, var);
		   }
		free(str);

#ifndef HT_UNSAFE
		if (NULL != g)	/* Guaranteed to never happen. */
		{
			free_game(g);
		}
#endif
		free_ht_line	(line);
		free		(inv);
		//flose(fp);
		return;
	}
	free_ht_line	(line);

#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
	if (NULL == g)		/* Guaranteed to never happen. */
	{
		(void) print_err_fr	(fp,HT_FR_FAIL_NULLPTR);
		free			(inv);
		//flose(fp);
		return;
	}
#endif

	game_initopt(g);

	/*
	 * This is only needed for ruleset_valid(), since all interfaces
	 * must call this when ui#_play_new is called. At this point no
	 * computer player is set, meaning no hash maps are allocated,
	 * and thus this is a very cheap operation anyway.
	 */
	fr = game_board_reset(g);
	if (HT_FR_SUCCESS != fr)
	{
		(void) print_err_fr	(fp,fr);
		free_game		(g);
		free			(inv);
		//flose(fp);
		return;
	}
#ifndef HT_UNSAFE
	fr	= HT_FR_NONE;
#endif

	{
		const char * const rvalid_why	= ruleset_valid(g);
		if (NULL != rvalid_why)
		{
			(void) fputs	(rvalid_why,	stderr);
			(void) fputc	('\n',		stderr);
			free_game	(g);
			free		(inv);
			//flose(fp);
			return;
		}
	}

#ifdef HT_UI_MSQU
	if (HT_UIT_MSQU == inv->info.play_ui)
	{
		fr	= play_uimq(g);
		if (HT_FR_SUCCESS != fr)
		{
			(void) print_err_fr	(fp,fr);
			free_game		(g);
			free			(inv);
			//flose(fp);
			return;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	}
	else
#endif /* HT_UI_MSQU */
	{
		/*
		 * This should be detected when reading invocation
		 * parameters.
		 */
		(void) print_err_fr	(fp,HT_FR_FAIL_UI_NONE);
		free_game		(g);
		free			(inv);
		//flose(fp);
		return;
	}

	free_game	(g);
	free		(inv);
	//flose(fp);
	return;

}
void*
game_main (void* user_data)
{
	while (1) {
		game_do_main();
		DequeueMessage();
	}
}

enum HT_FR
play_uimq (
/*@in@*/
/*@notnull@*/
	struct game * const	g
	)
/*@globals internalState, fileSystem, errno, stdout, stdin@*/
/*@modifies internalState, fileSystem, errno, stdout, stdin, * g@*/
{
	enum HT_FR		fr	= HT_FR_NONE;
	HT_BOOL			again	= HT_FALSE;
	struct ui		interf;
	interf.type			= HT_UIT_MSQU;
	interf.data.ui_mq		= alloc_uimq(g);
	if (NULL == interf.data.ui_mq)
	{
		return	HT_FR_FAIL_ALLOC;
	}

	fr	= uimq_init(g, interf.data.ui_mq);
	if (HT_FR_SUCCESS != fr)
	{
		free_uimq	(interf.data.ui_mq);
		return		fr;
	}

	do
	{
		fr	= uimq_play_new(g, & interf);
		if (HT_FR_SUCCESS != fr)
		{
			free_uimq	(interf.data.ui_mq);
			return		fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif

		fr	= uimq_play_again(g, interf.data.ui_mq, & again);
		if (HT_FR_SUCCESS != fr)
		{
			free_uimq	(interf.data.ui_mq);
			return		fr;
		}
#ifndef HT_UNSAFE
		fr	= HT_FR_NONE;
#endif
	} while (again);

	free_uimq	(interf.data.ui_mq);
	return		HT_FR_SUCCESS;
}
#endif
