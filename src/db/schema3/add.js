const mongoose = require('mongoose')
const model = require('./schema3')

async function add(ID) {
    await model.create({id_card:ID})
}

module.exports = {add}
