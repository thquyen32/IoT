const mongoose = require('mongoose')

const Schema = mongoose.Schema
const Card = new Schema({
    id_card: String
});

module.exports = mongoose.model('cards',Card)
