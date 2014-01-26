#include <stdio.h>	/* perror, puts */
#include <stdlib.h>	/* EXIT_* */

#include "X11/Xlib.h"	/* XOpenDisplay, XCloseDisplay, ... */
#include "X11/xpm.h"	/* XpmReadFileToImage */

/* '1' is a transparent XPM with leak and uninitialized jump.
 * '2' is an opaque XPM that works fine.
 * '3' is a transparent XPM with the same leak as '1', but not with the
 *	uninitialized jump.
 * Note that all images display exactly as expected without any apparent
 * errors. */
static
const char filenum	= '1';

static
void
leak (
	Display *	const display,
	Window		window,
	XImage *	const mask
	)
{
/* PROBLEM 1: This causes the leak:
 *	XCreatePixmap			->
 *	_XNoticeCreateBitmap		->
 *	XcursorNoticeCreateBitmap	->
 *	XcursorSupportsARGB		->
 *	_XcursorGetDisplayInfo		->
 *	XGetDefault			->
 *	???				->
 *	XrmInitialize			->
 *	XrmPermStringToQuark		->
 *	_XrmInternalStringToQuark	->
 *	malloc				-- [Valgrind] Among other; there
 *					are several leaks. */
	Pixmap pixmap	= XCreatePixmap(display, window,
		(unsigned int)mask->width, (unsigned int)mask->height,
		(unsigned int)mask->depth);

	GC gc_pixmap	= XCreateGC	(display, pixmap, 0L, NULL);
	XSetGraphicsExposures		(display, gc_pixmap, False);

/* PROBLEM 2: With `1.xpm`, this causes conditional jumps that depend on
 * uninitialized values (this does not happen with all transparent XPM
 * files):
 *	XPutImage		->
 *	_XNoticePutBitmap	->
 *	XcursorNoticePutBitmap	->
 *	XcursorImageHash	-- [Valgrind] "Jump or move depends on
 *				uninitialized values." */

	XPutImage(display, pixmap, gc_pixmap, mask, 0, 0, 0, 0,
		(unsigned int)mask->width,
		(unsigned int)mask->height);

	XFreeGC		(display, gc_pixmap);
	gc_pixmap	= NULL;
/* PROBLEM 1: This doesn't free pixmap: */
	XFreePixmap	(display, pixmap);
}

/* Get rid of events that may have piled up on the event queue.
 * Returns -1 for some failure, or 0 for success. */
static
int
empty_event_queue (
	Display * const	display
	)
{
	int eventdisc	= 0;
	XEvent * event	= malloc(sizeof(* event));
	if (NULL == event)
	{
		return -1;
	}

	/* Unhandled Expose events can apparently leak, so get rid of
	 * them. */
	while (XPending(display) > 0)
	{
		XNextEvent	(display, event);
		eventdisc++;
	}

	free(event);
	event	= NULL;
	if (printf("%d event(s) discarded.\n", eventdisc) < 0)
	{
		return -1;
	}
	return 0;
}

int
main (
	const int	argc,
	const char * *	argv
	)
{
	const unsigned int w	= (unsigned int)50;
	const unsigned int h	= (unsigned int)50;
	Display *	display	= XOpenDisplay(NULL);
	Window		window	= XCreateSimpleWindow(display,
		XDefaultRootWindow(display), 0, 0, w, h, 0, 0, 0);
	GC		gc	= XCreateGC(display, window, 0l, NULL);

	char *	filename	= malloc(sizeof(* filename) * 6);
	XImage *	img	= NULL;
	XImage *	mask	= NULL;

	XSetGraphicsExposures(display, gc, False);

	XSynchronize(display, True);	/* For debugging. */
	XSelectInput(display, window, ExposureMask);

	if (NULL == filename)
	{
		XFreeGC		(display, gc);
		XCloseDisplay	(display);
		perror		("OOM");
		return EXIT_FAILURE;
	}
	filename[0]	= filenum;
	filename[1]	= '.';
	filename[2]	= 'x';
	filename[3]	= 'p';
	filename[4]	= 'm';
	filename[5]	= '\0';

	if (XpmReadFileToImage(display, filename, & img, & mask, NULL)
		!= 0)
	{
		XFreeGC		(display, gc);
		XCloseDisplay	(display);
		free		(filename);
		perror		("Failed to read file");
		return EXIT_FAILURE;
	}

	if (NULL == img)
	{
		if (NULL != mask)
		{
			XDestroyImage	(mask);
		}
		XFreeGC		(display, gc);
		XCloseDisplay	(display);
		free		(filename);
		filename	= NULL;
		perror		("img is NULL");
		return EXIT_FAILURE;
	}

	if (NULL != mask)
	{
		puts		("Image is transparent. leak()");

		leak		(display, window, mask);

		XDestroyImage	(mask);
		mask		= NULL;
	}
	else
	{
		puts	("Image is opaque (pixmap not loaded).");
	}

	if (argc == argc && NULL != argv) { }	/* -Wunused */
	XDestroyImage	(img);
	img		= NULL;

/* Useless:
	XFreeColormap(display, XDefaultColormap(display,
		XDefaultScreen(display)));
*/

	XFreeGC		(display, gc);
	gc		= NULL;
	{
		const int retval = empty_event_queue(display);
		XCloseDisplay	(display);
		free		(filename);
		filename	= NULL;
		return 0 == retval ? EXIT_SUCCESS : EXIT_FAILURE;
	}
}

