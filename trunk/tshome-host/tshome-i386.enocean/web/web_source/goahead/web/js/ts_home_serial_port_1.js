#include <stdio.h>
#include <cgic.h>
#include <string.h>
#include <stdlib.h>

int cgiMain() {

	cgiHeaderContentType("text/html");
	fprintf(cgiOut,"window.onload = prepareForm;\n");
	fprintf(cgiOut,"\n");
	fprintf(cgiOut,"function prepareForm() {\n");
	fprintf(cgiOut,"	if (!document.getElementById) {\n");
	fprintf(cgiOut,"		return;\n");
	fprintf(cgiOut,"	}\n");
	fprintf(cgiOut,"\n");
	fprintf(cgiOut,"	if (!document.getElementById(\"set_serial_info\")) {\n");
	fprintf(cgiOut,"		return;\n");
	fprintf(cgiOut,"	}\n");
	fprintf(cgiOut,"\n");
	fprintf(cgiOut,"	document.getElementById(\"set_serial_info\").onsubmit = function() {\n");
	fprintf(cgiOut,"		var data = \"\";\n");
	fprintf(cgiOut,"		for (var i = 0; i < this.elements.length; i++) {\n");
	fprintf(cgiOut,"			data += this.elements[i].name;\n");
	fprintf(cgiOut,"			data += \"=\";\n");
	fprintf(cgiOut,"			data += escape(this.elements[i].value);\n");
	fprintf(cgiOut,"			data += \"&\";\n");
	fprintf(cgiOut,"		}\n");
	fprintf(cgiOut,"		var res = set_serial_info(data);\n");
	fprintf(cgiOut,"		// var res = login_validate(data);\n");
	fprintf(cgiOut,"		return !res;\n");
	fprintf(cgiOut,"	};\n");
	fprintf(cgiOut,"}\n");
	fprintf(cgiOut,"\n");

	return EXIT_SUCCESS;
}