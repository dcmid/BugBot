var socket = io();

function send_it(){
  socket.emit('send_it');
  console.log('sending it');
}

socket.on('update_dist', function(msg){
  console.log(msg);
  document.getElementById("dist").innerHTML = msg;
});

document.getElementById("sendbtn").addEventListener("click",send_it);
