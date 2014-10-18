var dotenv = require('dotenv');
dotenv.load();

var client = require('twilio')(process.env.ACCOUNT_SID, process.env.AUTH_TOKEN);
var express = require('express');
var app = express();

app.post('/escape', function (req, res) {
  client.calls.create({
    to: req.params.phone,
    from: process.env.FROM_PHONE_NUMBER,
    applicationSid: process.env.APPLICATION_SID,
    method: "GET",
    fallbackMethod: "GET",
    statusCallbackMethod: "GET",
    record: "false"
  }, function(err, call) {
    console.log(call.sid);
  });
});
