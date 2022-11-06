// Copyright (c) Nicolas VENTER All rights reserved.

#pragma once

#define EXPORT __declspec(dllexport)

typedef enum
{
	added = 1,
	removed,
	modified,
	renamed_old,
	renamed_new,
} file_watcher_event_type;

static const char* file_watcher_event_type_str[] = {"", "added", "removed", "modified", "renamed_old", "renamed_new"};

#define FILE_WATCHER_FILE_EVENT_PARAM const char *file, file_watcher_event_type event, bool is_directory, void *data
#define FILE_WATCHER_FILE_LAMBDA	  [](FILE_WATCHER_FILE_EVENT_PARAM) // no capture --> use data
typedef void (*file_watcher_callback)(FILE_WATCHER_FILE_EVENT_PARAM);

extern "C"
{
	// example callback function
	EXPORT void file_watcher_print_file_event(FILE_WATCHER_FILE_EVENT_PARAM);

	// blocking function
	EXPORT void file_watcher_watch_sync(
		const char* folder_path, bool b_recursive, file_watcher_callback callback, void* data = nullptr);

	// non blocking function, change the value of bWatching to stop the thread
	EXPORT void file_watcher_watch_async(const char* folder_path,
		bool b_recursive,
		file_watcher_callback callback,
		bool* bWatching = nullptr,
		void* data = nullptr);
}
