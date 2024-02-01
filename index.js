// TODO: remove all the random single letter variables

let bibtex = require('bibtex-js-parser')
let fs = require('node:fs')

let data = ''

if(process.argv.length <= 2) {
	console.log(`Usage: node ${process.argv[1]} [filename]`)
	process.exit(1)
}

let r = fs.createReadStream(process.argv[2])

r.on('data', (chunk) => {
	data += chunk
})

let output_json = []

let contributionAuthors = new Map()

let STRresearchPublication = ''
let STRcontributionAuthors = ''

r.on('end', () => {

	console.log(data)

	const bibJSON = bibtex.BibtexParser.parseToJSON(data);

	bibJSON.forEach((value) => {

		value._type = 'researchPublication'

		const authors_and_search = parse_authors(value.author)
		let auth0 = contributionAuthors.get(authors_and_search[1][0])?.personName.familyName
		let auth1 = contributionAuthors.size > 2 ? 'etal' :
			contributionAuthors.get(authors_and_search[1][1])?.personName.familyName

		let obj = {
			_type: 'researchPublication',
			citationKey: {
				_type: 'slug',
				current: `${value.year}-${auth0}-${auth1}-${value.title}`
			},
			title: value.title,
			authors: authors_and_search[0],
			year: parseInt(value.year),
			school: value.school,
			publicationType: value.type,
			month: to_month(value.month),
			volume: (value.volume == undefined ? undefined : parseInt(value.volume)),
			number: (value.number == undefined ? undefined : parseInt(value.number)),
			pages: value.pages,
			editor: value.editor,
			edition: value.edition,
			series: value.series,
			organisation: value.organization || value.organisation,
			institution: value.institution,
			journal: value.journal,
			publisher: value.publisher,
			address: value.address,
			howPublished: value.howPublished || value.howpublished,
			bookTitle: value.booktitle || value.bookTitle,
			notes: value.notes
		}

		// year-Author0FamilyName-(etal|Author1FamilyName)-title

		// Values not required
		STRresearchPublication += JSON.stringify(obj) + '\n'
	})

	contributionAuthors.forEach((a)=>{
		STRcontributionAuthors += JSON.stringify(a) + '\n'
	})

	r.close()

	let w1 = fs.createWriteStream('researchPublication.ndjson')
	w1.write(STRresearchPublication, ()=>{w1.close()})
	let w2 = fs.createWriteStream('contributionAuthors.ndjson')
	w2.write(STRcontributionAuthors, ()=>{w2.close()})
})

function parse_authors(a) {
	if(typeof(a) !== typeof('a'))
		throw Error('Author type is not string')

	let x = []
	let dup = []

	a.split('and').forEach((a) => {
		let id = 0
		let search = a.trim()
		let [familyName, restOfName] = search.split(',')
		let givenNames = restOfName?.split(' ').filter(a => a)

		search = givenNames?.toString()+familyName;

		if(givenNames?.length <= 0) {
			console.log(`${a} Too Few names for a person`)
			process.exit(1)
		}
		if(givenNames?.length > 16) {
			console.log(`${a} Too many names for a single person`)
			process.exit(1)
		}

		if(dup.length < 2)
			dup.push(search)

		let ca = contributionAuthors.get(search)
		if(ca === undefined) {
			id = 'authors-' + contributionAuthors.size;
			contributionAuthors.set(search, {
				_id: id,
				_type: 'contributionAuthor',
				//FIXME: This is probably not a good assumption
				//But we have know way to knowing?
				authorType: 'person',		
				personName: {
					_type: 'personName',
					familyName: familyName,
					givenNames: givenNames,
					pronouns: 'they/them'
				}
			})
		} else {
			id = ca._id
		}

		x.push({
			_type: 'reference',
			_ref: id
		})
	})

	return [x, dup]
}

// Converts Some data to month form
function to_month(a) {
	if(parseInt(a))
		return parseInt(a)
	else
	{
		switch(a?.toLowerCase()?.substring(0, 3))
		{
			case 'jan': return 1;
			case 'feb': return 2;
			case 'mar': return 3;
			case 'apr': return 4;
			case 'may': return 5;
			case 'jun': return 6;
			case 'jul': return 7;
			case 'aug': return 8;
			case 'sep': return 9;
			case 'oct': return 10;
			case 'nov': return 11;
			case 'dec': return 12;
			default:
				return undefined;
		}
	}
}
