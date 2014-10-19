var HOST = 'the-great-escape.herokuapp.com';
HOST = 'tge.ngrok.com';

Pebble.addEventListener("ready", function(e) {
  var title = "Next Steps";
  var watchId = Pebble.getWatchToken().substring(0, 4);
  var text = "Text the following four characters to +16144291658, they are your AntiSocial Security Number: "+watchId;
  Pebble.showSimpleNotificationOnPebble(title, text);
});

Pebble.addEventListener("appmessage", function(e) {
  var watchId = Pebble.getWatchToken().substring(0, 4);
  var userXhr = new XMLHttpRequest();
  userXhr.open('GET', 'http://'+HOST+'/users/'+watchId, true);
  userXhr.responseType = 'json';

  userXhr.onload = function () {
    var user = this.response;
    var xhr = new XMLHttpRequest();
    xhr.open('POST', 'http://'+HOST+'/escape', true);
    xhr.setRequestHeader('X-To-Phone-Number', user.phone_number);
    xhr.setRequestHeader('Content-type', 'application/json');
    xhr.setRequestHeader('Content-length', 0);
    xhr.setRequestHeader('Connection', 'close');
    xhr.send();
  };
  userXhr.send();
});
