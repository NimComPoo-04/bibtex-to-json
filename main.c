#include "b2j.h"
#include "tokenize.h"
#include "validate.h"
#include "genjson.h"

static string_t read_file_source_code(const char *name)
{
	string_t s = {0};
	FILE *f = fopen(name, "rb");

	if(!f) return s;

	fseek(f, 0, SEEK_END);
	s.length = ftell(f);
	fseek(f, 0, SEEK_SET);

	s.value = calloc(sizeof(char), s.length + 1);
	fread(s.value, sizeof(char), s.length, f);
	s.value[s.length++] = 0;

	fclose(f);
	return s;
}

int main(int argc, char *argv[])
{
	if(argc < 0)
	{
		fprintf(stderr, "Usage: ./%s [filename]\n", argv[0]);
		return 64;
	}

	string_t source = read_file_source_code(argv[1]);
	// fprintf(stderr, "Source Dump: %.*s\n", (int)source.length, source.value);

	size_t current = 0, line = 0;

	while(current < source.length)
	{
		bibtex_t b = bibtex_tokenize(source, &current, &line);

		/*
		   fprintf(stderr, "Type: %d\n", b.type);
		   fprintf(stderr, "Tag: %.*s\n", (int)b.tag.length, b.tag.value);

		   fprintf(stderr, "Title: %.*s\n", (int)b.title.length, b.title.value);
		   fprintf(stderr, "Author: %.*s\n", (int)b.author.length, b.author.value);
		   fprintf(stderr, "Year: %d\n", b.year);
		   fprintf(stderr, "Month: %d\n", b.month);
		   fprintf(stderr, "Volume: %d\n", b.volume);
		   fprintf(stderr, "Number: %d\n", b.number);
		   fprintf(stderr, "Pages: %.*s\n", (int)b.pages.length, b.pages.value);
		   fprintf(stderr, "Editor: %.*s\n", (int)b.editor.length, b.editor.value);
		   fprintf(stderr, "Series: %.*s\n", (int)b.series.length, b.series.value);
		   fprintf(stderr, "Organisation: %.*s\n", (int)b.organisation.length, b.organisation.value);
		   fprintf(stderr, "Journal: %.*s\n", (int)b.journal.length, b.journal.value);
		   fprintf(stderr, "Publisher: %.*s\n", (int)b.publisher.length, b.publisher.value);
		   fprintf(stderr, "Address: %.*s\n", (int)b.address.length, b.address.value);
		   fprintf(stderr, "HowPublished: %.*s\n", (int)b.howPublished.length, b.howPublished.value);
		   fprintf(stderr, "BookTitle: %.*s\n", (int)b.bookTitle.length, b.bookTitle.value);
		   fprintf(stderr, "Notes: %.*s\n", (int)b.notes.length, b.notes.value);

*/
		validate_bibtex(&b);
		generate_json(&b);
	}

	return 0;
}
