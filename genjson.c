#include "genjson.h"

// FIXME(NimComPoo): although this is not that concerning still 
// make this variable non be global.
extern string_t Types[];

void generate_json(bibtex_t *b)
{
	printf("{\n");
	printf("\t_type: \"researchPublication\"\n");
	printf("\t, _id: \"%.*s\"\n", (int)b->tag.length, b->tag.value);
	printf("\t, publicationType: \"%.*s\"\n", (int)Types[b->type].length, Types[b->type].value);
	printf("\t, title: \"%.*s\"\n", (int)b->title.length, b->title.value);
	printf("\t, authors: \"%.*s\"\n", (int)b->author.length, b->author.value);

	printf("\t, year: %d\n", b->year);

	if(b->month != 0)
		printf("\t, month: %d\n", b->month);

	if(b->volume != 0)
	        printf("\t, volume: %d\n", b->volume);

	if(b->pages.length != 0)
	        printf("\t, pages: \"%.*s\"\n", (int)b->pages.length, b->pages.value);

	if(b->editor.length != 0)
	        printf("\t, editor: \"%.*s\"\n", (int)b->editor.length, b->editor.value);

	if(b->series.length != 0)
	        printf("\t, series: \"%.*s\"\n", (int)b->series.length, b->series.value);

	if(b->organisation.length != 0)
	        printf("\t, organisation: \"%.*s\"\n", (int)b->organisation.length, b->organisation.value);

	if(b->journal.length != 0)
	        printf("\t, journal: \"%.*s\"\n", (int)b->journal.length, b->journal.value);

	if(b->publisher.length != 0)
	        printf("\t, publisher: \"%.*s\"\n", (int)b->publisher.length, b->publisher.value);

	if(b->address.length != 0)
	        printf("\t, address: \"%.*s\"\n", (int)b->address.length, b->address.value);

	if(b->howPublished.length != 0)
	        printf("\t, howPublished: \"%.*s\"\n", (int)b->howPublished.length, b->howPublished.value);

	if(b->bookTitle.length != 0)
	        printf("\t, bookTitle: \"%.*s\"\n", (int)b->bookTitle.length, b->bookTitle.value);

	if(b->notes.length != 0)
		printf("\t, notes: \"%.*s\"\n", (int)b->notes.length, b->notes.value);

	printf(" }\n");
}
