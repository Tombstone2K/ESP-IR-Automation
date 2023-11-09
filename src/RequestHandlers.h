// RequestHandlers.h

#ifndef RequestHandlers_H
#define RequestHandlers_H

#include <ESPAsyncWebSrv.h>

void handleLogsPage(AsyncWebServerRequest *request);
void handleTestPing(AsyncWebServerRequest *request);
void handleSec(AsyncWebServerRequest *request);
void handleMain(AsyncWebServerRequest *request);
void handleDataRetreival(AsyncWebServerRequest *request);


void handleUploadRequest(AsyncWebServerRequest *request);
void handleJSON(AsyncWebServerRequest *request);





#endif
