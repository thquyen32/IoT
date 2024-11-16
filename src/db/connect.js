const mongoose = require('mongoose')

async function connect() {
    try {
        await mongoose.connect('mongodb://localhost:27017/PRJ2')
        console.log("Connected")
    } catch (error) {
      console.log('Failed Bitch')  
    }
}

module.exports = {connect}
