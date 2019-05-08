#pragma once

#define CRASHDUMP_ENABLED true

#pragma comment (lib, "dbghelp.lib")

namespace ErrorDump {

	void setup();
	void close();
}