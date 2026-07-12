#pragma once

namespace FNLauncher {

void setBackendPort(int port);
void startBackend();
void stopBackend();
bool checkBackend();
bool backendOnline();

}
