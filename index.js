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
	const bibJSON = bibtex.BibtexParser.parseToJSON(data);

	bibJSON.forEach((value) => {

		value._type = 'researchPublication'

		const authors_and_search = parse_authors(value.author)
		value.authors = authors_and_search[0]
		value.year = parseInt(value.year)
		value.volume = (value.volume == undefined ? undefined : parseInt(value.volume))
		value.number = (value.number == undefined ? undefined : parseInt(value.number))

		// year-Author0FamilyName-(etal|Author1FamilyName)-title

		let auth0 = contributionAuthors.get(authors_and_search[1][0])?.personName.familyName
		let auth1 = contributionAuthors.size > 2 ? 'etal' :
			contributionAuthors.get(authors_and_search[1][1])?.personName.familyName

		value.citationKey = {
			_type: 'slug',
			current: `${value.year}-${auth0}-${auth1}-${value.title}`
		}

		// Values not required
		value.raw = undefined
		value.author = undefined
		value.id = undefined

		STRresearchPublication += JSON.stringify(value) + '\n'
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

		search = givenNames.toString()+familyName;

		if(givenNames?.length <= 0) {
			console.log(`${a} Too Few names for a person`)
			process.exit(1)
		}
		if(givenNames.length > 16) {
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
	if(a === undefined) return undefined

	const t = parseInt(a)
	if(t !== 0/0)
		return t

	let k = a.toLowerCase()
	let d = {
		'january': 1, 'jan': 1, 
		'february': 2, 'feb': 2,
		'march': 3, 'mar': 3,
		'april': 4, 'apr': 4,
		'may': 5, 'may': 5,
		'june': 6, 'jun': 6,
		'july': 7, 'jul': 7,
		'august': 8, 'aug': 8,
		'september': 9, 'sep': 9,
		'october': 10, 'oct': 10,
		'november': 11, 'nov': 11,
		'december': 12, 'dec': 12
	}[k]

	return d === undefined ? a : d
}
