window.onload = prepareForm;

function prepareForm() {
  if(!document.getElementById) {
    return;
  }
  if(!document.getElementById("contactform")) {
    return;
  }
  document.getElementById("contactform").onsubmit = function() {
    var data = "";
    for (var i=0; i<this.elements.length; i++) {
      data+= this.elements[i].name;
      data+= "=";
      data+= escape(this.elements[i].value);
      data+= "&";
    }
    return !sendData(data);
  };
}

function sendData(data) {
  var request = getHTTPObject();
  if (request) {
    request.onreadystatechange = function() {
      parseResponse(request);
    };
    request.open( "POST", "formlogic.php", true );
    request.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
    request.send(data);
    return true;
  } else {
    return false;
  }
}

function parseResponse(request) {
  if (request.readyState == 4) {
    if (request.status == 200 || request.status == 304) {
      var container = document.getElementById("container");
      container.innerHTML = request.responseText;
      prepareForm();
    }
  }
}

function getHTTPObject() {
  var xhr = false;
  if (window.XMLHttpRequest) {
    xhr = new XMLHttpRequest();
  } else if (window.ActiveXObject) {
    try {
      xhr = new ActiveXObject("Msxml2.XMLHTTP");
    } catch(e) {
      try {
        xhr = new ActiveXObject("Microsoft.XMLHTTP");
      } catch(e) {
        xhr = false;
      }
    }
  }
  return xhr;
}