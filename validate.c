#include "validate.h"

void validate_bibtex(bibtex_t *b)
{
	int err = 0;

	// Title Validation
	if(b->title.length == 0)
	{
		fprintf(stderr, "Title not present\n");
		err = 1;
	}

	// Authors
	if(b->author.length == 0)
	{
		fprintf(stderr, "No Author present\n");
		err = 1;
	}

	if(b->year == -1)
	{
		fprintf(stderr, "The year of the publication is mandatory\n");
		err = 1;
	}

	switch(b->type)
	{
		case ARTICLE:
			if(b->journal.length == 0)
			{
				fprintf(stderr, "Journal is mandatory for a publication that is an article\n");
				err = 1;
			}
			break;

		case BOOK:
			if(b->publisher.length == 0)
			{
				fprintf(stderr, "Publisher is mandatory for a publication that is an book\n");
				err = 1;
			}
			if(b->address.length == 0)
			{
				fprintf(stderr, "Address of the publisher is mandatory for a publication that is an book\n");
				err = 1;
			}
			break;

		case BOOKLET:
			if(b->howPublished.length == 0)
			{
				fprintf(stderr, "howPublished is mandatory for a publication that is an booklet\n");
				err = 1;
			}
			break;

		case CONFERENCE:
			if(b->bookTitle.length == 0)
			{
				fprintf(stderr, "bookTitle is mandatory for a publication that is an conference\n");
				err = 1;
			}
			break;

		default:{}
	}

	if(err)
		exit(1);
}
