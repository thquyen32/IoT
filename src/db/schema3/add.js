const mongoose = require('mongoose');
const model = require('./schema3');

async function add(ID) {
  try {
    const result = await model.create({ id_card: ID });
    if (result) {
      return true; 
    }
  } catch (error) {
    console.error('Lỗi khi thêm dữ liệu:', error);
    return false; 
  }
}

module.exports = { add };
