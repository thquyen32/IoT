const express = require('express');
const morgan = require('morgan');
const Websocket = require('ws');
const http = require('http');
const mongoose = require('mongoose')
const app = express();
const port = 8483;
const db = require('./db/connect');
const check = require('./db/schema1/check')
const {checkpass} = require('./db/schema2/checkpass')
const {checkID} = require('./db/schema3/checkID')
const {add} = require('./db/schema3/add')
const model2 = require('./db/schema2/schema2')
app.use(morgan('combined'));
app.use(express.json())
app.use(express.urlencoded({
  extended: true
}))

const server = http.createServer(app); // Tạo một server HTTP và kết hợp với Express
const wss = new Websocket.Server({ server }); // Sử dụng server HTTP với WebSocket


db.connect()
// Websocket server
wss.on('connection', function connection(ws) {
  ws.on('error', console.error);
  console.log('A new connection');
  
  ws.on('message', function message(data) {
    console.log(data.toString())
  });
});
// dùng để check tài khoản mật khẩu
app.post('/',async (req,res) => {
  const SSID = req.body.SSID
  const PASSWORD = req.body.PASS
  const result = await check.check(SSID,PASSWORD)
  if(result == true)
  {
    res.send('Accept')
  }
  else
  {
    res.send('Your SSID or PASSWORD is invalid')
}
})
// Dùng để check mật khẩu khóa
app.post('/pass', async (req,res) => {
  const PASS = req.body.PASS
  const result = await checkpass(PASS)
  console.log(result)
  if(result == true)
  {
    res.send('Door is open')
  }
})
// Dùng để đổi mật khẩu
app.post('/changepass',async (req,res) => 
{
  try {
    const PASSWORD = req.body.PASS
    const result = await model2.updateOne(
      { _id: new mongoose.Types.ObjectId("673748552bdbcb11b9fe7be6") },
      { $set: { PASS: PASSWORD } }
    )
    if(result.modifiedCount)
      res.send("Update Success")
    else
    res.send('Failed')
  } catch (error) {
    console.error("Interupt Server",error)
  }
})
// Dùng để check thẻ quét
app.post('/card', async (req,res) => 
{
  const ID = req.body.ID
  const result = await checkID(ID)
  console.log(result)
  if(result == true)
  {
    res.send('Accept')
  }
})
app.post('/add', async (req,res) => 
{
 await add(ID);
})

server.listen(port, '0.0.0.0', () => {
  console.log(`Server is listening on port ${port}`);
});
