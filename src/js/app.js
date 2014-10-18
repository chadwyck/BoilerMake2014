var Accel = require("ui").Accel;

Accel.on("tap", function(e) {
  var xhr = new XMLHttpRequest();
  xhr.open('POST', 'http://the-great-escape.herokuapp.com', true);
  var params = 'phone=+6149158886';

  xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
  xhr.setRequestHeader('Content-length', params.length);
  xhr.setRequestHeader('Connection', 'close');
  xhr.send(params);
});
