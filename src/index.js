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
const wss = new Websocket.Server({ server}); // Sử dụng server HTTP với WebSocket
const clients = new Map()

let clientID

db.connect()
// Websocket server
wss.on('connection', function connection(ws) {
  console.log(`New Connection`);
  ws.send("HELLO")
  let clientID
  ws.on('message', function message(data) {
  const json = JSON.parse(data)
  if(json.type == 'Register')
  {
   clientID = json.ID
  if(clients.has(clientID))
  {
    console.log('da co id nay r')
  }
  else
  {
  clients.set(clientID,ws)
  console.log("Dang ky thanh cong %s",clientID)
  }
  }
})
ws.on('close', (code, reason) => {
  if (clientID) {
      clients.delete(clientID);
      console.log('Đã xóa client:', clientID);
  }
  console.log(`WebSocket đóng với mã: ${code}, lý do: ${reason}`);
});
  ws.on('error', ()=>{
    // clients.delete(clientID)
    // console.log('Đã xóa client: %s',clientID)
  });
})
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
app.post('/android', async (req,res) => {
  const pass_input = req.body.PASS
  console.log(pass_input)
  const result = await checkpass(pass_input)
  console.log(result)
  if(result == true)
  {
    res.send('Door is open')
    sendToClient("1", "DOOR IS OPEN")
  }
  else
  res.send("CUT") 
})
// Dùng để check mật khẩu khóa
app.post('/pass', async (req,res) => {
  const pass_input = req.body.PASS
  console.log(pass_input)
  const result = await checkpass(pass_input)
  console.log(result)
  if(result == true)
  {
    res.send('Door is open')
    sendToClient("2", "DOOR IS OPEN")
  }
  else
  res.send("CUT") 
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
  if(result == true)
  {
    res.send('Accept')
    sendToClient("2", "DOOR IS OPEN")
  }
  else 
  console.log("Loi o day")
})
app.post('/add', async (req,res) => 
{
  ID = req.body.ID
 if(add(ID))
 {
  res.send("Success")
 }
 else
 res.send("FAILED")
})

server.listen(port, '0.0.0.0', () => {
  console.log(`Server is listening on port ${port}`);
});

function sendToClient(clientId, message) {
  const client = clients.get(clientId);
  if (client && client.readyState === WebSocket.OPEN) {
      client.send(message);
  } else {
      console.log(`Client ${clientId} không tồn tại hoặc không kết nối.`);
  }
}