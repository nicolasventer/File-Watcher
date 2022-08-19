// Copyright (c) Nicolas VENTER All rights reserved.

#pragma once

typedef enum
{
	added = 1,
	removed,
	modified,
	renamed_old,
	renamed_new,
} Event;

static const char* eventToStringArray[] = {"", "added", "removed", "modified", "renamed_old", "renamed_new"};

#define C_FILE_PARAM  const char *file, Event event, void *data
#define C_FILE_LAMBDA [](C_FILE_PARAM) // no capture --> use data
typedef void (*CFileCallback)(C_FILE_PARAM);

// example callback function
extern "C" void cPrintFileEvent(C_FILE_PARAM);

// non blocking function, change the value of bWatching to stop the thread
extern "C" void cWatch(
	const char* folderPath, bool bRecursive, CFileCallback cFileCallback, bool* bWatching = nullptr, void* data = nullptr);
