const mongoose = require('mongoose')
const Mymodel = require('./schema2')

async function checkID(ID) 
{
    const Instance = await Mymodel.findOne()  
    
    if(Instance)
    {
        if(Instance.id_card == ID)
            return true
        else
        return false
    }
    else 
    return false
}

module.exports = {checkID}
