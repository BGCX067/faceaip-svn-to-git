window.onload = prepareForm;

function getFullPath(obj) {
	if (obj) {
		// ie
		if (window.navigator.userAgent.indexOf("MSIE") >= 1) {
			obj.select();
			return document.selection.createRange().text;
		}
		// firefox
		else if (window.navigator.userAgent.indexOf("Firefox") >= 1) {
			if (obj.files) {
				try {
					netscape.security.PrivilegeManager
							.enablePrivilege('UniversalFileRead')
				} catch (err) {
					// need to set signed.applets.codebase_principal_support to
					// true
				}
				return obj.value;
			}
			return obj.value;
		}
		return obj.value;
	}
}

function prepareForm() {
	if (!document.getElementById) {
		return;
	}

	// if (!document.getElementById("set_ip_prot")) {
	// return;
	// }

	document.getElementById("imp_xml").onsubmit = function() {
		var data = "";
		for (var i = 0; i < this.elements.length; i++) {
			data += this.elements[i].name;
			data += "=";
			data += escape(this.elements[i].value);
			data += "&";
		}
		data = unescape(data);
		var res = imp_xml(data);

		return !res;
	};
	// document.getElementById("set_ip_prot").onsubmit = function() {
	// var data = "";
	// for (var i = 0; i < this.elements.length; i++) {
	// data += this.elements[i].name;
	// data += "=";
	// data += escape(this.elements[i].value);
	// data += "&";
	// }
	// var res = set_ip_port(data);
	//
	// return !res;
	// };
	// document.getElementById("get_conf").onsubmit = function() {
	// var data = "";
	// for (var i = 0; i < this.elements.length; i++) {
	// data += this.elements[i].name;
	// data += "=";
	// data += escape(this.elements[i].value);
	// data += "&";
	// }
	// var res = control_host(data);
	//
	// return !res;
	// };
	//	
	// document.getElementById("get_software").onsubmit = function() {
	// var data = "";
	// for (var i = 0; i < this.elements.length; i++) {
	// data += this.elements[i].name;
	// data += "=";
	// data += escape(this.elements[i].value);
	// data += "&";
	// }
	// var res = control_host(data);
	//
	// return !res;
	// };
}

function imp_xml(data) {
	var request = getHTTPObject();
	if (request) {
		request.onreadystatechange = function() {
			parseResponse(request);
		};
		// alert(data);
		request.open("POST", "../cgi-bin/imp_xml.html", true);
		request.setRequestHeader("Content-Type",
				"application/x-www-form-urlencoded");
		request.send(data);
		return true;
	} else {
		return false;
	}
}

function control_host(data, reload) {
	var request = getHTTPObject();
	if (request) {
		request.onreadystatechange = function() {
			parseResponse(request);
		};
//		alert(data);
		request.open("POST", "../cgi-bin/control_host.html", true);
		request.setRequestHeader("Content-Type",
				"application/x-www-form-urlencoded");
		request.send(data);
		if (reload == 1) {
			window.location.reload();
		}
		return true;
	} else {
		return false;
	}
}

function set_ip_port(data) {
	var request = getHTTPObject();
	if (request) {
		request.onreadystatechange = function() {
			parseResponse(request);
		};
		request.open("POST", "../cgi-bin/set_ip_port.html", true);
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
