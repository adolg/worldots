/* Copyright 2013 Alexander Söderlund.
 *
 * This file is part of HNEFATAFL.
 *
 * HNEFATAFL is free software: you can distribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * HNEFATAFL is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <errno.h>	/* errno */
#include <stdio.h>	/* FILE, stdout, stderr, fputs, ... */
#include <stdlib.h>	/* rand */
#include <string.h>	/* strerror */

#include "config.h"		/* HT_VERSION */
#include "func.h"		/* fr_fail_what */
#include "info.h"		/* HT_PROGNAME */
#include "invoc.h"		/* fail_invoc_what */
#include "iogen.h"
#include "iotextparse.h"	/* ht_line_empty */
#include "lang.h"		/* HT_L_* */
#include "ui.h"			/* ui_def */
#include "uit.h"		/* HT_UIT */

/*
 * If fail, prints to stderr; else prints to stdout.
 *
 * If the program returns EXIT_FAILURE, then fail should be HT_TRUE.
 *
 * If the user requested help to be printed using the "print help"
 * invocation switch, then fail should be HT_FALSE.
 */
enum HT_FR
print_inv_help (
	const HT_BOOL fail
	)
{
	enum HT_UIT ui_d	= ui_def();
	FILE * const stdstr	= fail ? stderr : stdout;
	int rv;

	rv =			fputc(HT_L_INV_S_HELP,		stdstr);
	rv = rv < 0 ? rv : fputc('\t', 				stdstr);
	rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_HELP,		stdstr);
	rv = rv < 0 ? rv : fputc('\n',				stdstr);

	rv = rv < 0 ? rv : fputc(HT_L_INV_S_VERSION,		stdstr);
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_VERSION,	stdstr);
	rv = rv < 0 ? rv : fputc('\n',				stdstr);

	rv = rv < 0 ? rv : fputc(HT_L_INV_S_RUNCOM,		stdstr);
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_RUNCOM,	stdstr);
	rv = rv < 0 ? rv : fputc('\n',				stdstr);

	rv = rv < 0 ? rv : fputc(HT_L_INV_S_UI,			stdstr);
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_UI,		stdstr);
	rv = rv < 0 ? rv : fputc('\n',				stdstr);

#ifdef HT_UI_CLI
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputc(HT_L_INV_S_UI_CLI,		stdstr);
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_UI_CLI,	stdstr);
	if (HT_UIT_CLI == ui_d)
	{
		rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_UI_DEF,stdstr);
	}
	rv = rv < 0 ? rv : fputc('\n',				stdstr);
#endif

#ifdef HT_UI_XLIB
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputc(HT_L_INV_S_UI_XLIB,		stdstr);
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_UI_XLIB,	stdstr);
	if (HT_UIT_XLIB == ui_d)
	{
		rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_UI_DEF,stdstr);
	}
	rv = rv < 0 ? rv : fputc('\n',				stdstr);
#endif

#ifdef HT_UI_MSQU
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputc(HT_L_INV_S_UI_MSQU,		stdstr);
	rv = rv < 0 ? rv : fputc('\t',				stdstr);
	rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_UI_MSQU,	stdstr);
	if (HT_UIT_MSQU == ui_d)
	{
		rv = rv < 0 ? rv : fputs(HT_L_INV_HELP_S_UI_DEF,stdstr);
	}
	rv = rv < 0 ? rv : fputc('\n',				stdstr);
#endif

	return rv < 0 ? HT_FR_FAIL_IO_PRINT : HT_FR_SUCCESS;
}

static
enum HT_FR
print_inv_nine (void)
/*@globals fileSystem, stdout, errno, internalState@*/
/*@modifies fileSystem, stdout, errno, internalState@*/
{
	int r		= 0;
	int random	= 1 + (int)(9.0 * (rand() / (RAND_MAX + 1.0)));
	if (1 == random)
	{
		r = r < 0 ? r : puts("Þar munu eftir");
		r = r < 0 ? r : puts("undrsamligar");
		r = r < 0 ? r : puts("gullnar tǫflur");
		r = r < 0 ? r : puts("í grasi finnask,");
		r = r < 0 ? r : puts("þærs í árdaga");
		r = r < 0 ? r : puts("áttar hǫfðu.");
		/*
		 * Där skola åter
		 * de underbara
		 * guldbrädspelsbrickorna
		 * i gräset hittas,
		 * som i tidens morgon
		 * dem tillhört hade.
		 */
	}
	else if (2 == random)
	{
		r = r < 0 ? r : puts("Tefldu í túni,");
		r = r < 0 ? r : puts("teitir váru,");
		r = r < 0 ? r : puts("var þeim vettergis");
		r = r < 0 ? r : puts("vant ór gulli,");
		r = r < 0 ? r : puts("uns þrjár kvámu");
		r = r < 0 ? r : puts("þursa meyjar");
		r = r < 0 ? r : puts("ámáttkar mjǫk");
		r = r < 0 ? r : puts("ór Jǫtunheimum.");
		/*
		 * På gården med brädspel
		 * de glada lekte,
		 * armod på guld
		 * fanns ingalunda,
		 * tills tursamöar
		 * trenne kommo,
		 * mycket mäktiga
		 * mör, från jättevärlden.
		 */
	}
	else if (3 == random)
	{
		r = r < 0 ? r : puts("Tafl emk ǫrr at efla,");
		r = r < 0 ? r : puts("íþróttir kank níu,");
		r = r < 0 ? r : puts("týnik trauðla rúnum,");
		r = r < 0 ? r : puts("tíð er bók ok smíðir,");
		r = r < 0 ? r : puts("skríða kank á skíðum,");
		r = r < 0 ? r : puts("skýtk ok rœk, svát nýtir;");
		r = r < 0 ? r : puts("hvártveggja kank hyggja:");
		r = r < 0 ? r : puts("harpslótt ok bragþóttu.");
		/*
		 * I can play at tafl,
		 * nine skills I know
		 * rarely forget I the runes,
		 * I know of books and smithing,
		 * I know how to slide on skis,
		 * shoot and row, well enough;
		 * each of two arts I know,
		 * harp-playing and speaking poetry.
		 */
	}
	else if (4 == random)
	{
		r = r < 0 ? r : puts("Burr var inn ellsti,");
		r = r < 0 ? r : puts("en Barn annat,");
		r = r < 0 ? r : puts("Jóð ok Aðal,");
		r = r < 0 ? r : puts("Arfi, Mǫgr,");
		r = r < 0 ? r : puts("Niðr ok Niðjungr,");
		r = r < 0 ? r : puts("- námu leika, -");
		r = r < 0 ? r : puts("Sonr ok Sveinn,");
		r = r < 0 ? r : puts("- sund ok tafl, -");
		r = r < 0 ? r : puts("Kundr hét enn,");
		r = r < 0 ? r : puts("Konr var inn yngsti.");
		/*
		 * Bur var den äldste,
		 * och Barn den andre,
		 * Jod och Adal,
		 * Arve, Mog;
		 * lekar Nid
		 * och Nidjung lärde
		 * samt Son och Sven
		 * simning och brädspel;
		 * Kund hette en,
		 * Kon var den yngste.
		 */
	}
	else if (5 == random)
	{
		r = r < 0 ? r : puts("Mjǫk eru reifðir");
		r = r < 0 ? r : puts("rógbirtingar,");
		r = r < 0 ? r : puts("þeirs í Haralds túni");
		r = r < 0 ? r : puts("húnum verpa;");
		r = r < 0 ? r : puts("féi eru þeir gœddir");
		r = r < 0 ? r : puts("ok fǫgrum mækum,");
		r = r < 0 ? r : puts("málmi húnlenzkum");
		r = r < 0 ? r : puts("ok mani austrœnu.");
		/*
		 * I höj grad begaves
		 * de krigere,
		 * som i Haralds gård
		 * leger med brikker;
		 * med gods de begaves
		 * og med blanke sværd,
		 * med hunnisk malm
		 * og østlandske trælkvinder.
		 */
	}
	else if (6 == random)
	{
		r = r < 0 ? r : puts("Gestumblindi mælti:\n");
		r = r < 0 ? r : puts("\"Hverjar eru þær snótir,");
		r = r < 0 ? r : puts("er um sinn dróttin");
		r = r < 0 ? r : puts("vápnlausar vega,");
		r = r < 0 ? r : puts("inar jǫrpu hlífa");
		r = r < 0 ? r : puts("um alla daga,");
		r = r < 0 ? r : puts("en inar fegri frýja?");
		r = r < 0 ? r : puts("Heiðrekr konungr,");
		r = r < 0 ? r : puts("hygg þú at gátu.\"\n");
		r = r < 0 ? r : puts("\"Góð er gáta þín,");
		r = r < 0 ? r : puts("Gestumblindi,");
		r = r < 0 ? r : puts("getit er þeirar:\n");
		r = r < 0 ? r : fputs(
		"Þat er hneftafl. Tǫflur drepast vápnlausar ",	stdout);
		r = r < 0 ? r : puts(
			"um hnefann, ok fylgja honum inar rauðu.\"");
		/*
		 * Da sagde Gest:
		 *
		 * Hvilke ere de Møer,
		 * som vaabenløse
		 * kæmpe om sin Herre?
		 * mod de hvides
		 * vilde Angreb
		 * de sorte ham stedse skjerme.
		 * Raad den Gaade
		 * rigtig, Kong Heidrek.
		 *
		 * Heidrek svarede:
		 *
		 * God er din Gaade,
		 * Gest hin blinde!
		 * gjettet er den:
		 * i damspil de sorte
		 * sin Drot forsvare
		 * mod Anfald af de hvide.
		 */
	}
	else if (7 == random)
	{
		r = r < 0 ? r : puts("Gestumblindi mælti:\n");
		r = r < 0 ? r : puts("\"Hvat er þat dýra,");
		r = r < 0 ? r : puts("er drepr fé manna");
		r = r < 0 ? r : puts("ok er járni kringt útan,");
		r = r < 0 ? r : puts("horn hefir átta,");
		r = r < 0 ? r : puts("en hǫfuð ekki");
		r = r < 0 ? r : puts("ok rennr, sem renna má?");
		r = r < 0 ? r : puts("Heiðrekr konungr,");
		r = r < 0 ? r : puts("hygg þú at gátu.\"\n");
		r = r < 0 ? r : puts("\"Gód er gáta þín,");
		r = r < 0 ? r : puts("Gestumblindi,");
		r = r < 0 ? r : puts("getit er þeirar:\n");
		r = r < 0 ? r : fputs(
		"Þat er húnn í hneftafli; hann heitir som ",	stdout);
		r = r < 0 ? r : puts(
			"bjǫrn; hann rennr, þegar honum er kastat.\"");
		/*
		 * Da sagde Gest:
		 *
		 * Hvilket er det Dyr
		 * som dræber Folks Hjorde,
		 * helt af Jern forhudet?
		 * Otte Horn det har,
		 * men Hoved ikke,
		 * og tæller et talrigt Følge
		 * Raad den Gaade
		 * rigtig, Kong Heidrek.
		 *
		 * Heidrek svarade:
		 *
		 * God er din Gaade,
		 * Gest hin blinde!
		 * gjettet er den:
		 * det er Ræven,
		 * som rovbegjerlig
		 * i Damspil Faarene dræber.
		 */
	}
	else if (8 == random)
	{
		r = r < 0 ? r :
		fputs("Jafnan skemmtu þau Helga sér at tafli ok ",
			stdout);
		r = r < 0 ? r :
		fputs("Gunnlaugr. Lagði hvárt þeira góðan þokka til ",
			stdout);
		r = r < 0 ? r :
		fputs("annars bráðliga, sem raunir bar á síðan. Þau ",
			stdout);
		r = r < 0 ? r : fputs("váru mjǫk jafnaldrar.\n\n",
			stdout);
		r = r < 0 ? r :
		fputs("Helga var svá fǫgr, at þat er sǫgn fróðra ",
			stdout);
		r = r < 0 ? r :
		fputs("manna, at hon hafi fegrst kona verit á Íslandi.",
			stdout);
		r = r < 0 ? r :
		fputs(" Hár hennar var svá mikit, at þat mátti hylja ",
			stdout);
		r = r < 0 ? r :
		fputs("hana alla, ok svá fagrt sem gull barit, ok ",
			stdout);
		r = r < 0 ? r :
		fputs("engi kostr þótti þá þvílíkr sem Helga in fagra ",
			stdout);
		r = r < 0 ? r :
		fputs("í ǫllum Borgarfirði ok víðara annars staðar.\n",
			stdout);
		/*
		 * Gunnlög och Helga muntrade sig tidt och ofta med att
		 * spela tafvel. Mycket snart fattade de godt tycke till
		 * hvarandra; hvilket tiden sedan skulle visa. De voro
		 * nästan jemnåriga. Helga var så fager, att sagokunnige
		 * män sagt, att hon varit Islands skönaste qvinna.
		 * Hennes hår var så ymnigt, att hon dermed kunde hölja
		 * sig helt och hållet, och det var vänt som smidt gull.
		 * Och så godt gifte som Helga den fagra fans ej i hela
		 * Borgarfjärden och vida om kring.
		 */
	}
	else /* if (9 == random) */
	{
		r = r < 0 ? r :
		fputs("En konungs synir sǫfnuðu liði ok fengu lítit ",
			stdout);
		r = r < 0 ? r :
		fputs("lið ok sendu orð Friðþjófi ok báðu hann liðs, ",
			stdout);
		r = r < 0 ? r :
		fputs("en hann sat at tafli, er sendimenn kómu til "
			"hans.\n\n", stdout);
		r = r < 0 ? r :
		fputs("Þeir mæltu: \"Konungar várar sendu þér kvedju ",
			stdout);
		r = r < 0 ? r :
		fputs("ok vildu þiggja liðsinni þitt til orrostu með "
			"sér.\"\n\n", stdout);
		r = r < 0 ? r :
		fputs("Hann svarade engu ok mælti við Bjǫrn: \"Leita ",
			stdout);
		r = r < 0 ? r :
		fputs("ráðs fyrir at tefla, því at tvíkostr er þér á ",
			stdout);
		r = r < 0 ? r :
		fputs("tvá vega,\" sagði hann.\n", stdout);
		/*
		 * Björn och Fritiof sutto båda
		 * vid ett schackbord, skönt att skåda.
		 * Silver var varannan ruta,
		 * och varannan var av guld.
		 *
		 * Då steg Hilding in. "Sitt neder!
		 * Upp i högbänk jag dig leder,
		 * töm ditt horn, och låt mig sluta
		 * spelet, fosterfader huld!"
		 *
		 * Hilding kvad: "Från Beles söner
		 * kommer jag till dig med böner.
		 * Tidningarna äro onde,
		 * och till dig står landets hopp."
		 *
		 * Fritiof kvar: "Tag dig till vara,
		 * Björn, ty nu är kung i fara.
		 * Frälsas kan han med en bonde:
		 * den är gjord att offras upp."
		 */
	}

	return HT_FR_SUCCESS;
}

/*
 * Prints, separated by tabulators:
 * HT_PROGNAME
 * HT_VERSION
 * HT_PREPROC
 */
static
enum HT_FR
print_inv_version (void)
/*@globals fileSystem, stdout, errno@*/
/*@modifies fileSystem, stdout, errno@*/
{
	int rv;

	rv =			fputs(HT_PROGNAME,	stdout);
	rv = rv < 0 ? rv : fputc('\t',			stdout);
	rv = rv < 0 ? rv : printf("%d\t\t%s\n", HT_VERSION, HT_PREPROC);
	return rv < 0 ? HT_FR_FAIL_IO_PRINT : HT_FR_SUCCESS;
}

/*
 * nplay_print is the user's invocation request to print something.
 *
 * This function delegates the appropriate printing function.
 */
enum HT_FR
print_inv (
	const enum HT_INV_PRINT nplay_print
	)
{
	switch (nplay_print)
	{
		case HT_INV_PRINT_HELP:
			return print_inv_help(HT_FALSE);
		case HT_INV_PRINT_NINE:
			return print_inv_nine();
		case HT_INV_PRINT_VERSION:
			return print_inv_version();
		default:
			return HT_FR_FAIL_ENUM_UNK;
	}
}

/*
 * Prints errno and returns fprintf, or returns 0 if errno is 0.
 */
static
int
print_errno (FILE* outlog)
/*@globals fileSystem, stderr, errno@*/
/*@modifies fileSystem, stderr@*/
{
	if (0 != errno)
	{
		return fprintf(outlog, "(errno: %s)\n",
			strerror(errno));
	}
	else
	{
		return 0;
	}
}

/*
 * Prints the following to stderr:
 *
 * 1.	HT_FAIL_INVOC fail code.
 *
 * 2.	Localized fail message retrieved from fail_invoc_what.
 *
 * 3.	print_errno.
 */
static
enum HT_FR
print_err_fail_invoc (
	const enum HT_FAIL_INVOC code
	)
/*@globals fileSystem, stderr, errno@*/
/*@modifies fileSystem, stderr@*/
{
	const char * msg = fail_invoc_what(code);

	int rv		= fprintf(stderr, "[%d:%d]: %s\n",
			(int)HT_FR_FAIL_INVOC, (int)code, msg);
	rv		= rv < 0 ? rv : print_errno(stderr);

	return rv < 0 ? HT_FR_FAIL_IO_PRINT : HT_FR_SUCCESS;
}

/*
 * Prints the following to stderr:
 *
 * 1.	HT_FAIL_RREADER fail code.
 *
 * 2.	Localized fail message retrieved from fail_rreader_what.
 *
 * 3.	Contents of line (unless it's NULL or empty).
 *
 * 4.	print_errno.
 */
static
enum HT_FR
print_err_fail_rreader (
	FILE * outlog,
	const enum HT_FAIL_RREADER	code,
/*@in@*/
/*@notnull@*/
	const struct ht_line * const	line
	)
/*@globals fileSystem, stderr, errno@*/
/*@modifies fileSystem, stderr, errno@*/
{
	const char *	msg = fail_rreader_what(code);

	int rv		= fprintf(outlog, "[%d:%d]: %s\n",
			(int)HT_FR_FAIL_INVOC, (int)code, msg);
	if (NULL != line && !ht_line_empty(line))
	{
		size_t i;
		for (i = 0; i < line->wordc; i++)
		{
			const char * const wrd = line->words[i].chars;
			if (strlen(wrd) < (size_t)1)
			{
				continue;
			}
			rv = rv < 0 ? rv : fprintf(outlog, "[%s]", wrd);
			if (i + 1 < line->wordc)
			{
				rv = rv < 0 ? rv : fputc(' ', outlog);
			}
		}
		rv		= rv < 0 ? rv : fputc('\n', outlog);
	}
	rv			= rv < 0 ? rv : print_errno(outlog);

	return rv < 0 ? HT_FR_FAIL_IO_PRINT : HT_FR_SUCCESS;
}

/*
 * Prints the following to stderr:
 *
 * 1.	HT_FR fail code.
 *
 * 2.	Localized fail message retrieved from fr_fail_what.
 *
 * 3.	print_errno.
 */
enum HT_FR
print_err_fr (
	FILE * outlog,
	const enum HT_FR code
	)
{
	const char * msg = fr_fail_what(code);

	int rv		= fprintf(outlog, "[%d]: %s\n", (int)code, msg);
	rv		= rv < 0 ? rv : print_errno(outlog);

	return rv < 0 ? HT_FR_FAIL_IO_PRINT : HT_FR_SUCCESS;
}

/*
 * Delegates either (depending on code):
 *
 * *	print_err_fail_invoc
 *
 * *	print_err_fr
 */
enum HT_FR
print_err_fr_invoc (
	const enum HT_FR		code,
	const enum HT_FAIL_INVOC	code_inv
	)
{
	if (fr_fail_invoc(code))
	{
		return print_err_fail_invoc	(code_inv);
	}
	else
	{
		return print_err_fr		(stderr,code);
	}
}

/*
 * Delegates either (depending on code):
 *
 * *	print_err_fail_rreader
 *
 * *	print_err_fr
 */
enum HT_FR
print_err_fr_rreader (
	FILE * outlog,
	const enum HT_FR		code,
	const enum HT_FAIL_RREADER	code_read,
	const struct ht_line * const	line
	)
{
	if (fr_fail_rreader(code))
	{
		return print_err_fail_rreader	(outlog, code_read, line);
	}
	else
	{
		return print_err_fr		(outlog, code);
	}
}

