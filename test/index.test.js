var postal = require('../');
var sets = require('simplesets');
var assert = require('assert');

describe('expand', function() {
    var expansions;
    var expansions2;
    describe('expand_address', function() {
        it('should correctly expand a US address', function() {
            expansions = new sets.Set(postal.expand.expand_address('781 Franklin Ave Crown Hts Brooklyn NY'));
            assert(expansions.has('781 franklin avenue crown heights brooklyn new york'));
        });
        it('should correctly dedupe equivalent addresses', function() {
            expansions = new sets.Set(postal.expand.expand_address('30 West Twenty-sixth Street Floor Number 7', {languages: ['en']}));
            expansions2 = new sets.Set(postal.expand.expand_address('Thirty W 26th St Fl #7', {languages: ['en']}));
            assert(expansions.intersection(expansions2).size() > 0);
        });
        it('should handle non-ASCII addresses correctly from Node', function() {
            expansions = new sets.Set(postal.expand.expand_address('Friedrichstraße 128, Berlin, Germany'));
            assert(expansions.has('friedrich strasse 128 berlin germany'));
        });

    })
})

describe('parser', function() {
    var parsed = [];

    function has_components(parsed, components) {
        var expected = Object.keys(components).length;
        var got = 0;

        for (var i = 0; i < parsed.length; i++) {
            var el = parsed[i];
            if (components[el.component] === el.value) {
                got++;
            }
        }

        return expected === got;
    }

    describe('parse_address', function() {
        it('should parse a US address', function() {
            parsed = postal.parser.parse_address('781 Franklin Ave Crown Heights Brooklyn NYC NY 11216 USA');
            assert(has_components(parsed, {
                house_number: '781',
                road: 'franklin ave',
                suburb: 'crown heights',
                city_district: 'brooklyn',
                city: 'nyc',
                state: 'ny',
                postcode: '11216',
                country: 'usa'
            }));
        });
        it('should parse a venue and state', function() {
            parsed = postal.parser.parse_address('whole foods ny');
            assert(has_components(parsed, {
                house: 'whole foods',
                state: 'ny'
            }));
        });
        it('should parse house/apt style house_number', function() {
            parsed = postal.parser.parse_address('1917/2 Pike Drive');
            assert(has_components(parsed, {
                house_number: '1917 / 2',
                road: 'pike drive'
            }));
        });
        it('should parse different comma styles correctly', function() {
            parsed = postal.parser.parse_address('3437 warwickshire rd,pa');
            assert(has_components(parsed, {
                house_number: '3437',
                road: 'warwickshire rd',
                state: 'pa'
            }));
            parsed = postal.parser.parse_address('3437 warwickshire rd, pa');
            assert(has_components(parsed, {
                house_number: '3437',
                road: 'warwickshire rd',
                state: 'pa'
            }));
            parsed = postal.parser.parse_address('3437 warwickshire rd pa');
            assert(has_components(parsed, {
                house_number: '3437',
                road: 'warwickshire rd',
                state: 'pa'
            }));

        });

    })
})
