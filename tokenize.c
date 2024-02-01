#include <ctype.h>
#include <string.h>

#include "tokenize.h"

_Noreturn void bibtex_error(const char *msg, size_t current, size_t line)
{
	fprintf(stderr, "Error: %s line: %d character: %d\n", msg, (int)line, (int)current);
	exit(1);
}

static void skip_space(string_t source, size_t *current, size_t *line)
{
	while(*current < source.length && isspace(source.value[*current]))
	{
		if(source.value[*current] == '\n')
			++*line;
		++*current;
	}

	if(*current >= source.length)
		bibtex_error("Error in formatting", *current, *line);
}

static string_t read_group(string_t source, size_t *current, size_t *line, int(*fn)(int))
{
	skip_space(source, current, line);

	string_t s = {source.value + *current, 0};
	while(*current < source.length && fn(source.value[*current]))
	{
		s.length++;
		++*current;
	}

	if(*current >= source.length)
		bibtex_error("Error in formatting", *current, *line);

	if(s.length == 0)
		bibtex_error("No Word present", *current, *line);

	return s;
}

static string_t read_word(string_t source, size_t *current, size_t *line)
{
	return read_group(source, current, line, isalnum);
}

static int read_number(string_t source, size_t *current, size_t *line)
{
	skip_space(source, current, line);

	if(source.value[*current] == '{')
		++*current;

	string_t s = read_group(source, current, line, isdigit);

	if(source.value[*current] == '}')
			++*current;


	char *d = s.value + s.length;
	return strtol(s.value, &d, 10);
}

static char search_till(string_t source, size_t *current, size_t *line, char *a)
{
	char *t = 0;
	while(*current < source.length && (t = strchr(a, source.value[*current])) == 0)
	{
		if(source.value[*current] == '\n')
			++*line;
		++*current;
	}

	if(*current >= source.length)
		bibtex_error("Error in formatting.", *current, *line);

	++*current;

	return *t;
}

static string_t read_string(string_t source, size_t *current, size_t *line)
{
	char c = search_till(source, current, line, "{\"");

	string_t s = {source.value + *current, 0};

	if(c == '"')
	{
		search_till(source, current, line, "\"");
		s.length = *current - 1 + source.value - s.value;
	}
	else
	{
		int count = 1;
		while(*current < source.length && count)
		{
			if(search_till(source, current, line, "{}") == '{')
				count++;
			else
				count--;
		}	

		s.length = *current - 1 + source.value - s.value;
	}

	if(*current >= source.length)
		bibtex_error("Error in formatting", *current, *line);

	if(s.length == 0)
		bibtex_error("No Word present", *current, *line);

	return s;
}

int nocasestrncmp(const char *this, const char *other, size_t len)
{
	for(size_t i = 0; i < len; i++)
	{
		if(tolower(this[i]) != tolower(other[i]))
			return tolower(this[i]) - tolower(other[i]);
	}
	return 0;
}

// Checks for the correct
// Probably fine since there will only be n times we do check and most
string_t Types[] =
{{"article", 7},
	{"book", 4},
	{"booklet", 7},
	{"conference", 10},
	{"inbook", 7},
	{"incollection", 12},
	{"inproceedings", 13},
	{"manual", 6},
	{"mastersthesis", 13},
	{"misc", 4},
	{"phdthesis", 9},
	{"proceedings", 11},
	{"techreport", 10},
	{"unpublished", 11}};
static int document_type_checker(string_t str)
{

	for(int i = 0; i < (int)(sizeof(Types)/sizeof(Types[0])); i++)
		if(Types[i].length == str.length &&
				nocasestrncmp(str.value, Types[i].value, str.length) == 0)
			return i;

	return -1;
}

// helper functions
static void load_string(string_t source, size_t *current, size_t *line, string_t *data)
{
	*data = read_string(source, current, line);
}

static void load_number(string_t source, size_t *current, size_t *line, int *data)
{
	*data = read_number(source, current, line);
}

bibtex_t bibtex_tokenize(string_t source, size_t *current, size_t *line)
{
	bibtex_t b = {0};
	b.year = -1;

	search_till(source, current, line, "@");

	// Getting the Type of the bib entry
	string_t type = read_word(source, current, line);
	int k = document_type_checker(type);

	if(k == -1) bibtex_error("Invalid Type", *current, *line);

	b.type = k;

	search_till(source, current, line, "{");

	b.tag = read_word(source, current, line);

	while((k = search_till(source, current, line, ",}")) == ',')
	{
		skip_space(source, current, line);
		if(source.value[*current] == '}')
			break;

		string_t key = read_word(source, current, line);	 // word
		search_till(source, current, line, "=");

	//	printf("%.*s\n", key.length, key.value);

		// FIXME(NimComPoo): Not a good way to do this, just something for now 
		// does not account for errors if they go beyond the first three
		// characters. Get a good hashing algorithm.
		// So, author and autger is a hash collision.
		unsigned int hash = (tolower(key.value[0]) << 16)
			| (tolower(key.value[1]) << 8)
			| (tolower(key.value[2]));

		// Since there are limited number of cases we can just hardcode them
		// for now.
		switch(hash)
		{
			// All of these are working with strings
			case ('t' << 16) | ('i' << 8) | 't': load_string(source, current, line, &b.title); break;
			case ('a' << 16) | ('u' << 8) | 't': load_string(source, current, line, &b.author); break;
			case ('e' << 16) | ('d' << 8) | 'i': load_string(source, current, line, &b.editor); break;
			case ('s' << 16) | ('e' << 8) | 'r': load_string(source, current, line, &b.series); break;
			case ('o' << 16) | ('r' << 8) | 'g': load_string(source, current, line, &b.organisation); break;
			case ('j' << 16) | ('o' << 8) | 'u': load_string(source, current, line, &b.journal); break;
			case ('p' << 16) | ('u' << 8) | 'b': load_string(source, current, line, &b.publisher); break;
			case ('a' << 16) | ('d' << 8) | 'd': load_string(source, current, line, &b.address); break;
			case ('h' << 16) | ('o' << 8) | 'w': load_string(source, current, line, &b.howPublished); break;
			case ('b' << 16) | ('o' << 8) | 'o': load_string(source, current, line, &b.bookTitle); break;
			case ('m' << 16) | ('o' << 8) | 'n': load_string(source, current, line, &b.month); break;

			// For the time being considering abstract to be note
			case ('a' << 16) | ('b' << 8) | 's': 
			case ('n' << 16) | ('o' << 8) | 't': load_string(source, current, line, &b.notes); break;
			case ('p' << 16) | ('a' << 8) | 'g': load_string(source, current, line, &b.pages); break;

			case ('y' << 16) | ('e' << 8) | 'a': load_number(source, current, line, &b.year); break;
			case ('v' << 16) | ('o' << 8) | 'l': load_number(source, current, line, &b.volume); break;
			case ('n' << 16) | ('u' << 8) | 'm': load_number(source, current, line, &b.number); break;

			default:
			     {
				     fprintf(stderr, "Unrecognized Key: %.*s\n", (int)key.length, key.value);
				     string_t value = {0};
				     load_string(source, current, line, &value);
			     }
			     continue;
		}
	}

	return b;
}

