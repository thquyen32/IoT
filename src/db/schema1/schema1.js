const mongoose = require('mongoose')

const Schema = mongoose.Schema
const Lockdoor = new Schema({
    SSID: String,
    PASSWORD: String
});

module.exports = mongoose.model('lockdoors',Lockdoor)
