var sp = require('serialport')

sp.SerialPort.list()
.then((data) => console.log(data))
.catch(err => console.log(err));