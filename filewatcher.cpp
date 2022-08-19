// Copyright (c) Nicolas VENTER All rights reserved.

#include "filewatcher.h"

#include "Windows.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

// pub
void printFileEvent(const std::filesystem::path& file, Event event)
{
	std::cout << file.string() << " " << eventToStringArray[event] << std::endl;
}

void cPrintFileEvent(C_FILE_PARAM) { printFileEvent(std::filesystem::path(file), event); }

class EventFilter
{
public:
	bool operator()(const std::string& formattedPath, const Event filewatchEventType);

private:
	std::unordered_map<std::string, uint32_t> countMap;
};

bool EventFilter::operator()(const std::string& formattedPath, const Event filewatchEventType)
{
	auto it = countMap.find(formattedPath);
	switch (filewatchEventType)
	{
	case Event::removed:
	case Event::renamed_old:
		if (it != countMap.end()) countMap.erase(it);
		return true;
	case Event::renamed_new: // pattern: renamed_new, modified
	case Event::added:		 // pattern: added, modified, modified
	case Event::modified:	 // pattern: modified, modified, modified
		if (it == countMap.end())
		{
			countMap[formattedPath] = 1;
			return true;
		}
		else
		{
			uint32_t removeValue = filewatchEventType == Event::renamed_new ? 2 : 3;
			if (++it->second == removeValue) countMap.erase(it);
			return false;
		}
	default:
		return true;
	}
}

void cWatch(const char* folderPath, bool bRecursive, CFileCallback cFileCallback, bool* bWatching, void* data)
{
	std::thread(
		[folderPath, bRecursive, cFileCallback, &bWatching, data]()
		{
			if (!bWatching) bWatching = new bool(true);
			*bWatching = true;
			static const DWORD timeout = 100'000; // 100 seconds timeout
			std::string formattedFolderPath = std::filesystem::absolute(folderPath).string();
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
					bRecursive,
					FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE
						| FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_CREATION
						| FILE_NOTIFY_CHANGE_SECURITY,
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
						if (eventFilter(formattedPath, static_cast<Event>(info->Action)))
							cFileCallback(formattedPath.c_str(), static_cast<Event>(info->Action), data);
						offset += info->NextEntryOffset;
					} while (info->NextEntryOffset);
				}
			}
		})
		.detach();
}
