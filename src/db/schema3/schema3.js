const mongoose = require('mongoose')

const Schema = mongoose.Schema
const Card = new Schema({
    ID: String
});

module.exports = mongoose.model('cards',Card)
