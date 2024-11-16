const mongoose = require('mongoose')
const Mymodel = require('./schema1')

async function check( SSID, PASS) 
{
    const Instance = await Mymodel.findOne({SSID: SSID})  
    
    if(Instance)
    {   
        if(Instance.PASSWORD.trim() === PASS.trim())
        {
            return true
            
        }
        else    
        {
        return false
    }
}
    else 
    {
    return false
}
}

module.exports = {check}
