#include "Msg.h"



Msg::Msg(MSG_TYPE t, std::string d) {
	type = t;
	data = d;
}

Msg::Msg(MSG_TYPE t) {
	type = t;
	data = "";
}

Msg::~Msg() {
}
