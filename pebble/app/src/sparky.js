var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

var parseStatus = function(responseText) {
  // responseText contains a JSON object with weather info
  var status = JSON.parse(responseText);

  console.log('Lamp status: ' + JSON.stringify(status));

  // Assemble dictionary using our keys
  var dictionary = {
    'KEY_POWER': status.power
  };

  // Send to Pebble
  Pebble.sendAppMessage(dictionary,
                        function(e) {
                          console.log('Status info sent to Pebble successfully!');
                        },
                        function(e) {
                          console.log('Error sending status info to Pebble!');
                        }
                       );
};

function getStatus() {
  // Construct URL
  var url = 'http://jarknet.noip.me:3001/status';
  
  xhrRequest(url, 'GET', parseStatus);
}

function togglePower() {
  var url = 'http://jarknet.noip.me:3001/power';
  
  xhrRequest(url, 'PUT', parseStatus);
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    
    getStatus();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    
    togglePower();
  }                     
);

//Listen for configuration request
Pebble.addEventListener('showConfiguration', function(e) {
  console.log('Opening configuration page...');
  Pebble.openURL('https://dl.dropboxusercontent.com/u/11347975/sparky-config/index.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));

  console.log('Configuration page returned: ' + JSON.stringify(configData));

//   if (configData.backgroundColor) {
//     Pebble.sendAppMessage({
//       backgroundColor: parseInt(configData.backgroundColor, 16),
//       twentyFourHourFormat: configData.twentyFourHourFormat
//     }, function() {
//       console.log('Send successful!');
//     }, function() {
//       console.log('Send failed!');
//     });
//  }
});
