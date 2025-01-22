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
    })
})

describe('language_classifier', function() {
    it('should classify phrases', function() {
        const cases = [
            {
                text: 'street',
                expected: [{ language: 'en', probability: 0.9975550392228959 }]
            },
            {
                text: 'calle',
                expected: [{ language: 'es', probability: 0.9948278315613933 }]
            },
            {
                text: '200 santa monica pier santa monica coahuila 90401 usa',
                expected: [{ language: 'es', probability: 0.9889375382113144 }]
            },
            {
                text: '200 santa monica pier santa monica compania 90401 usa',
                expected: [
                    { language: 'es', probability: 0.6896583016342134 },
                    { language: 'it', probability: 0.12355268595569942 },
                    { language: 'en', probability: 0.09448842175870104 },
                    { language: 'pt', probability: 0.09152280207660235 }
                ]
            },
            {
                text: '200 santa monica pier santa monica compania anonima 90401 usa',
                expected: [
                    { language: 'en', probability: 0.39249744852100377 },
                    { language: 'pt', probability: 0.27690798537637573 },
                    { language: 'es', probability: 0.2631088374468901 },
                    { language: 'it', probability: 0.06642801798298495 }
                ]
            },
            {
                text: '200 santa monica pier santa monica calle 90401 usa',
                expected: [
                    { language: 'es', probability: 0.9332794851572307 },
                    { language: 'it', probability: 0.0653229950496308 }
                ]
            },
            {
                text: '200 santa monica pier santa monica ca 90401 usa',
                expected: [
                    { language: 'es', probability: 0.6860753090491215 },
                    { language: 'it', probability: 0.16420520901155986 },
                    { language: 'en', probability: 0.08797427320205269 },
                    { language: 'pt', probability: 0.061694606284459816 }
                ]
            },
            {
                text: '200 santa monica pier santa monica casa 90401 usa',
                expected: [
                    { language: 'it', probability: 0.7099331948983176 },
                    { language: 'pt', probability: 0.1484441794525817 },
                    { language: 'en', probability: 0.09171194632862785 }
                ]
            },
            {
                text: '200 santa monica pier santa monica cagliari 90401 usa',
                expected: [{ language: 'it', probability: 0.9409304710772296 }]
            },
            {
                text: '200 santa monica pier santa monica california 90401 usa',
                expected: [
                    { language: 'en', probability: 0.529208078531959 },
                    { language: 'pt', probability: 0.26387306487348855 },
                    { language: 'es', probability: 0.20130465470054235 }
                ]
            },
            {
                text: '200 santa monica pier santa monica companhia anonima 90401 usa',
                expected: [
                    { language: 'pt', probability: 0.8631314862441015 },
                    { language: 'en', probability: 0.1090210811124842 }
                ]
            }
        ];
        for (const c of cases) {
            assert.deepEqual(postal.language_classifier(c.text), c.expected, c.text);
        }
    });
})