var app = require('http').createServer(handler)
var io = require('socket.io')(app);
var fs = require('fs');

app.listen(1234);

function handler (req, res) {
  fs.readFile(__dirname + '/index.html',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Error loading index.html');
    }

    res.writeHead(200);
    res.end(data);
  });
}
function sendTime() {
    io.sockets.emit('atime', { time: new Date().toJSON() });
}
io.on('connection', function (socket) {
	console.log("Connected");
  socket.emit('welcome', { message: 'Connected !!!!' });
  socket.on('connection', function (data) {
    console.log(data);   });
  socket.on('atime', function (data) {
	  sendTime();
    console.log(data);
  });
    socket.on('arduino', function (data) {
	  io.sockets.emit('arduino', { message: 'R0' });
    console.log(data);
  });
});
