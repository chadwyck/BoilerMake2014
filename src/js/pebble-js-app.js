Pebble.addEventListener("ready", function(e) {
  var title = "Next Steps";
  var text = "Use this Pebble ID to sign up on greatesc.com: "+Pebble.getWatchToken();
  Pebble.showSimpleNotificationOnPebble(title, text);
});

Pebble.addEventListener("appmessage", function(e) {
  var xhr = new XMLHttpRequest();
  xhr.open('POST', 'http://the-great-escape.herokuapp.com', true);
  var params = 'phone_number=+6149158886';

  xhr.setRequestHeader('Content-type', 'application/json');
  xhr.setRequestHeader('Content-length', params.length);
  xhr.setRequestHeader('Connection', 'close');
  xhr.send(params);
});
