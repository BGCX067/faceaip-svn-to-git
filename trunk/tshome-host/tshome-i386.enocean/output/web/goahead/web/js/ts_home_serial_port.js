window.onload = prepareForm;

function prepareForm() {
	if (!document.getElementById) {
		return;
	}

	if (!document.getElementById("set_serial_info")) {
		return;
	}

	document.getElementById("set_serial_info").onsubmit = function() {
		var data = "";
		for (var i = 0; i < this.elements.length; i++) {
			data += this.elements[i].name;
			data += "=";
			data += escape(this.elements[i].value);
			data += "&";
		}
		var res = set_serial_info(data);
		// var res = login_validate(data);
		return !res;
	};
}

function set_serial_info(data) {
	var request = getHTTPObject();
	if (request) {
		request.onreadystatechange = function() {
			parseResponse(request);
		};
		request.open("POST", "../cgi-bin/home_serial_port.html", true);
		request.setRequestHeader("Content-Type",
				"application/x-www-form-urlencoded");
		request.send(data);
		return true;
	} else {
		return false;
	}
}

function parseResponse(request) {
	if (request.readyState == 4) {
		if (request.status == 200 || request.status == 304) {
			var container = document.getElementById("change_set_ip");
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
