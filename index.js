
module.exports = {
    expand: require('bindings')('expand'),
    parser: require('bindings')('parser'),
    language_classifier: require('bindings')('language_classifier').language_classifier
}