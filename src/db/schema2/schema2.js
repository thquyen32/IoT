const mongoose = require('mongoose')

const Schema = mongoose.Schema
const Passlock = new Schema({
    PASS: String
});

module.exports = mongoose.model('passlocks',Passlock)

