#ifndef _TOKENIZE_H_
#define _TOKENIZE_H_

#include "b2j.h"

typedef enum
{
	ARTICLE = 0,
	BOOK,
	BOOKLET,
	CONFERENCE,
	INBOOK,
	INCOLLECTION,
	INPROCEEDINGS,
	MANUAL,
	MASTERSTHESIS,
	MISC,
	PHDTHESIS,
	PROCEEDINGS,
	TECHREPORT,
	UNPUBLISHED
} BibType;

typedef struct
{
	BibType type;	// type of bib
	string_t tag;	// identifier of the bib

	// Creating the types that we are interested in.
	string_t title;
	string_t author;	// this is a bit complicated, we will deal with this later.
	int year;
	string_t month;
	int volume;
	int number;
	string_t pages;
	string_t editor;
	string_t series;
	string_t organisation;
	string_t journal;
	string_t publisher;
	string_t address;
	string_t howPublished;
	string_t bookTitle;
	string_t notes;
} bibtex_t;

bibtex_t bibtex_tokenize(string_t source, size_t *current, size_t *line);

#endif
