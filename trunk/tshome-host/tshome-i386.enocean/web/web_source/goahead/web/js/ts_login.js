window.onload = prepareForm;

function clickHandler() {
	if (window.ActiveXObject) {
		xmlDoc = new ActiveXObject("MSXML.DOMDocument");
		if (xmlDoc == null) {
			window.alert("MSXML.DOMDocument isn't installed.");
		} else {
			xmlDoc.async = false;
			xmlDoc.load("../cgi-bin/log_xml/web_log.xml");
			document
					.write(xmlDoc.getElementsByTagName("body")[0].firstChild.nodeValue);
		}
	}
	// code for Mozilla, Firefox, etc.
	else if (document.implementation && document.implementation.createDocument) {
		xmlDoc = document.implementation.createDocument("", "", null)
		// xmlDoc.load("note.xml");
		xmlDoc.load("../cgi-bin/log_xml/web_log.xml");
		alert(3);
//		xmlDoc.onload = function()// anonymous function
//		{
			alert(xmlDoc);
			alert(xmlDoc.getElementsByTagName("details")[0]);
			alert(xmlDoc.getElementsByTagName("log")[0]);
			alert(xmlDoc.getElementsByTagName("details")[0].firstChild.nodeValue);
			// document
			// .write(xmlDoc.getElementsByTagName("details")[0].firstChild.nodeValue);
//		}
	}
}

function prepareForm() {
	if (!document.getElementById) {
		return;
	}

	if (!document.getElementById("login_validate")) {
		return;
	}

	document.getElementById("login_validate").onsubmit = function() {
		var data = "";
		for (var i = 0; i < this.elements.length; i++) {
			data += this.elements[i].name;
			data += "=";
			data += escape(this.elements[i].value);
			data += "&";
		}

		clickHandler();
		// var res = login_validate(data);

		return !res;
	};
}

function login_validate(data) {
	var request = getHTTPObject();
	if (request) {
		request.onreadystatechange = function() {
			parseResponse(request);
		};

		request.open("POST", "../cgi-bin/login_validate.html", true);
		request.setRequestHeader("Content-Type",
				"application/x-www-form-urlencoded");
		request.send(data);
//		alert(data);

		// alert(1);
		// var xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
		// alert(2);
		// xmlDoc.async = false;
		// alert(3);
		// xmlDoc.load("../cgi-bin/log_xml/web_log.xml");
		// alert(xmlDoc);
		// var items = xmlDoc.getElementsByTagName("detail");
		// alert(items);
		// var name = items[0].childNodes[0].nodeValue;
		// alert(name);

		return true;
	} else {
		return false;
	}
}

function parseResponse(request) {
	if (request.readyState == 4) {
		if (request.status == 200 || request.status == 304) {
			// alert(1);
			// var data = request.responseXML;
			// alert(data);
			// var name =
			// data.getElementsByTagName("log")[0].firstChild.nodeValue;
			// alert(name);
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
		} catch (e) {
			try {
				xhr = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {
				xhr = false;
			}
		}
	}
	return xhr;
}
