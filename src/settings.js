Pebble.addEventListener('ready',
  function(e) {
    console.log('JavaScript app ready and running!');
    console.log('Pebble Watch Token: ' + Pebble.getWatchToken());
  }
);

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://todzero.com/pebble/futconfig.html');
});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    var req = new XMLHttpRequest();
    var activity = JSON.parse(JSON.stringify(e.payload));
    var type, value;
    if (activity['0']) { console.log('Received message: ' + activity['0']); type = "Meditation"; value = activity['0']}
    if (activity['1']) { console.log('Received message: ' + activity['1']); type = "Coding"; value = activity['1']}
    if (activity['2']) { console.log('Received message: ' + activity['2']); type = "Exercise"; value = activity['2']}
    
    //var dataurl = "http://posttestserver.com/post.php?dump&html&dir=debashis&" + type + "=" + value;
    var dataurl = "http://todzero.com/pebble/futsave.php?activity=" + type + "&value=" + value;
    req.open('GET', dataurl, true);
    console.log('Received message: ' + dataurl);
    req.send(null);
  }                     
);