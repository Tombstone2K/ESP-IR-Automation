#ifndef FileHandlingSD_H
#define FileHandlingSD_H

#include "FS.h"

void appendFile(fs::FS &fs, const char * path, const char * message);
void writeFile(fs::FS &fs, const char * path, const char * message);
void initializeDataTextFile();
void initializeSD();
void initializeLogsFile();


#endif