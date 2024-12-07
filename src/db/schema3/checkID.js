const mongoose = require('mongoose')
const Mymodel = require('./schema3')

async function checkID(ID) 
{
    const Instance = await Mymodel.findOne({id_card:ID})  
    if(Instance)
    return true
    else 
    return false
}

module.exports = {checkID}
