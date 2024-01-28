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
		validate_bibtex(&b);
		generate_json(&b);
	}

	return 0;
}
