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

function grabFile(file) {
  var request = getHTTPObject();
  if (request) {
    request.onreadystatechange = function() {
      parseResponse(request);
    };
    request.open("GET", file, true);
    request.send(null);
  }
}

function parseResponse(request) {
  if (request.readyState == 4) {
    if (request.status == 200 || request.status == 304) {

      var data = request.responseXML;
//alert(data);
      var name = data.getElementsByTagName("name")[0].firstChild.nodeValue;
      var website = data.getElementsByTagName("website")[0].firstChild.nodeValue;
      var email = data.getElementsByTagName("email")[0].firstChild.nodeValue;
      var header = document.createElement("h2");
      var mailto = document.createElement("a");
      mailto.setAttribute("href","mailto:"+email);
     // mailto.setAttribute("href",email);
      var text = document.createTextNode(name);
      mailto.appendChild(text);
      header.appendChild(mailto);
      var link = document.createElement("a");
      link.setAttribute("href",website);
      var linktext = document.createTextNode(website);
      link.appendChild(linktext);
      var details = document.getElementById("details");
      while (details.hasChildNodes()) {
        details.removeChild(details.lastChild);
      }
      details.appendChild(header);
      details.appendChild(link);
    }
  }
}
