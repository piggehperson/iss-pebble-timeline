/******************************* timeline lib *********************************/

// The timeline public URL root
var API_URL_ROOT = 'https://timeline-api.rebble.io/';

/**
 * Send a request to the Pebble public web timeline API.
 * @param pin The JSON pin to insert. Must contain 'id' field.
 * @param type The type of request, either PUT or DELETE.
 * @param topics Array of topics if a shared pin, 'null' otherwise.
 * @param apiKey Timeline API key for this app, available from dev-portal.getpebble.com
 * @param callback The callback to receive the responseText after the request has completed.
 */
function timelineRequest(pin, type, topics, apiKey, callback) {
  // User or shared?
  var url = API_URL_ROOT + 'v1/' + ((topics != null) ? 'shared/' : 'user/') + 'pins/' + pin.id;

  // Create XHR
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    console.log('timeline: response received: ' + this.responseText);
    callback(this.responseText);
  };
  xhr.open(type, url);

  // Set headers
  xhr.setRequestHeader('Content-Type', 'application/json');
  if(topics != null) {
    xhr.setRequestHeader('X-Pin-Topics', '' + topics.join(','));
    xhr.setRequestHeader('X-API-Key', '' + apiKey);
  }

  // Get token
  Pebble.getTimelineToken(function(token) {
    // Add headers
    xhr.setRequestHeader('X-User-Token', '' + token);

    // Send
    xhr.send(JSON.stringify(pin));
    console.log('timeline: request sent.');
  }, function(error) { console.log('timeline: error getting timeline token: ' + error); });
}

/**
 * Insert a pin into the timeline for this user.
 * @param pin The JSON pin to insert.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertUserPin(pin, callback) {
  timelineRequest(pin, 'PUT', null, null, callback);
}

/**
 * Delete a pin from the timeline for this user.
 * @param pin The JSON pin to delete.
 * @param callback The callback to receive the responseText after the request has completed.
 */
function deleteUserPin(pin, callback) {
  timelineRequest(pin, 'DELETE', null, null, callback);
}

/**
 * Insert a pin into the timeline for these topics.
 * @param pin The JSON pin to insert.
 * @param topics Array of topics to insert pin to.
 * @param apiKey Timeline API key for this app, available from dev-portal.getpebble.com
 * @param callback The callback to receive the responseText after the request has completed.
 */
function insertSharedPin(pin, topics, apiKey, callback) {
  timelineRequest(pin, 'PUT', topics, apiKey, callback);
}

/**
 * Delete a pin from the timeline for these topics.
 * @param pin The JSON pin to delete.
 * @param topics Array of topics to delete pin from.
 * @param apiKey Timeline API key for this app, available from dev-portal.getpebble.com
 * @param callback The callback to receive the responseText after the request has completed.
 */
function deleteSharedPin(pin, topics, apiKey, callback) {
  timelineRequest(pin, 'DELETE', topics, apiKey, callback);
}

/************************************* App ************************************/

var lat;
var lon;

var issApi;
// var timeline = require('./timeline');

// Create the request
var apiRequest;

Pebble.addEventListener('ready', function() {
  // PebbleKit JS is ready!
  console.log('PebbleKit JS ready in index!');
  
  doSearch();
});

function doSearch() {
  // Choose options about the data returned
  var options = {
    enableHighAccuracy: true,
    maximumAge: 10000,
    timeout: 10000
  };

  // Request current position
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, options);
}


function locationSuccess(pos) {
  lat = pos.coords.latitude;
  lon = pos.coords.longitude;

  issApi = 'http://api.open-notify.org/iss-pass.json?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude;
  console.log('lat= ' + pos.coords.latitude + ' lon= ' + pos.coords.longitude);
  console.log(issApi);

  // Send the request
  apiRequest = new XMLHttpRequest();
  // receive ISS data and iterate through it
  apiRequest.onload = function() {
    try {
      // Transform in to JSON
      var json = JSON.parse(this.responseText);
  
      // Read data
      var responseArray = json.response;
  
      var date;
      var pin;
  
      for (let i = 0; i < responseArray.length; i++){
          date = new Date(responseArray[i].risetime * 1000);
          // Create the pin
          pin = {
              "id": "lavender-iss-flyover-" + i,
              "time": date.toISOString(),
              "duration": responseArray[i].duration.toFixed(0) / 10,
              "layout": {
                  "type":"genericPin",
                  "title":"ISS Flyover",
                  "subtitle":"Visible for " + (responseArray[i].duration.toFixed(0) / 10) + " minutes",
                  "body":"Via Open-Notify",
                  "tinyIcon":"app://images/SATELLITE"
              }     
          };
  
          console.log('Inserting pin in the future: ' + "lavender-iss-flyover-" + responseArray[i].risetime);
  
          // Push the pin
          insertUserPin(pin, function(responseText) {
              console.log('Result: ' + responseText);
          });
      };
  
      Pebble.sendAppMessage({'searchResult': 1 }, function() { // Success
        console.log('Message sent successfully: ' + JSON.stringify(dict));
      }, function(e) {
        console.log('Message failed: ' + JSON.stringify(e));
      });
  } catch(err) {
    console.log('Error in API onLoad! ' + err);
  }
  };

  apiRequest.open('GET', issApi);
  apiRequest.send();
}

function locationError(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}