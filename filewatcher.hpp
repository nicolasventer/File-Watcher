// Copyright (c) Nicolas VENTER All rights reserved.

#pragma once

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
	static void file_watcher_print_file_event(FILE_WATCHER_FILE_EVENT_PARAM);

	// blocking function
	static void file_watcher_watch_sync(
		const char* folder_path, bool b_recursive, file_watcher_callback callback, void* data = nullptr);

	// non blocking function, change the value of bWatching to stop the thread
	static void file_watcher_watch_async(const char* folder_path,
		bool b_recursive,
		file_watcher_callback callback,
		bool* bWatching = nullptr,
		void* data = nullptr);
}

#include "Windows.h"

#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <thread>

void file_watcher_print_file_event(FILE_WATCHER_FILE_EVENT_PARAM)
{
	std::cout << (is_directory ? "[DIR] " : "[FILE] ") << std::filesystem::path(file).string() << " "
			  << file_watcher_event_type_str[event] << std::endl;
}

class EventFilter
{
public:
	bool operator()(const std::string& formattedPath, const file_watcher_event_type filewatchEventType)
	{
		auto it = toIgnoreSet.find(formattedPath);
		switch (filewatchEventType)
		{
		case file_watcher_event_type::removed:
		case file_watcher_event_type::renamed_old:
			if (it != toIgnoreSet.end()) toIgnoreSet.erase(it);
			return true;
		case file_watcher_event_type::renamed_new:
			return true;
		case file_watcher_event_type::modified: // pattern: modified, modified
		case file_watcher_event_type::added:	// pattern: added, modified
			if (it == toIgnoreSet.end())
			{
				toIgnoreSet.insert(formattedPath);
				return true;
			}
			else
			{
				toIgnoreSet.erase(it);
				return false;
			}
		default:
			return true;
		}
	}

private:
	std::set<std::string> toIgnoreSet;
};

static void file_watcher_watch_sync_p(
	const char* folder_path, bool b_recursive, file_watcher_callback callback, bool* bWatching, void* data)
{
	if (!bWatching) bWatching = new bool(true);
	*bWatching = true;
	static const DWORD timeout = 100'000; // 100 seconds timeout
	std::string formattedFolderPath = std::filesystem::absolute(folder_path).string();
	HANDLE hDirectory = CreateFileA(formattedFolderPath.c_str(),
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	char buffer[4096];
	OVERLAPPED overlapped;
	overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);
	EventFilter eventFilter;
	while (*bWatching)
	{
		ReadDirectoryChangesW(hDirectory,
			buffer,
			sizeof(buffer),
			b_recursive,
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE
				| FILE_NOTIFY_CHANGE_CREATION,
			NULL,
			&overlapped,
			NULL);

		if (!WaitForSingleObject(overlapped.hEvent, timeout) && *bWatching)
		{
			DWORD bytesReturned;
			GetOverlappedResult(hDirectory, &overlapped, &bytesReturned, FALSE);
			FILE_NOTIFY_INFORMATION* info;
			DWORD offset = 0;
			do
			{
				info = (FILE_NOTIFY_INFORMATION*)(buffer + offset);
				std::wstring fileName(info->FileName, info->FileNameLength / sizeof(wchar_t));
				std::string formattedPath = formattedFolderPath + "\\" + std::string(fileName.begin(), fileName.end());
				bool is_directory = std::filesystem::is_directory(formattedPath);
				if (is_directory || eventFilter(formattedPath, static_cast<file_watcher_event_type>(info->Action)))
					callback(formattedPath.c_str(), static_cast<file_watcher_event_type>(info->Action), is_directory, data);
				offset += info->NextEntryOffset;
			} while (info->NextEntryOffset);
		}
	}
}

void file_watcher_watch_sync(const char* folder_path, bool b_recursive, file_watcher_callback callback, void* data)
{
	bool bWatching = true;
	file_watcher_watch_sync_p(folder_path, b_recursive, callback, &bWatching, data);
}

void file_watcher_watch_async(
	const char* folder_path, bool b_recursive, file_watcher_callback callback, bool* bWatching, void* data)
{
	std::thread([folder_path, b_recursive, callback, &bWatching, data]()
		{ file_watcher_watch_sync_p(folder_path, b_recursive, callback, bWatching, data); })
		.detach();
}
