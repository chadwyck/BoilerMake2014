

  
  
  // Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
   var xhr = new XMLHttpRequest();
  xhr.open('POST', 'http://the-great-escape.herokuapp.com', true);
  var params = 'phone=+6149158886';

  xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
  xhr.setRequestHeader('Content-length', params.length);
  xhr.setRequestHeader('Connection', 'close');
  xhr.send(params);    
  }                     
);