# File Watcher

# Description

FileWatcher is a **c library** that allows you to **watch a folder** and get notified of any file changes inside.

# Features

- watch a file or a folder
- execute the callback for all files
- exported c functions for use in other languages (careful not in Javascript, neihter in Python since callback cannot be called outside of the main thread).

# Installation

Get the files [here](https://github.com/nicolasventer/File-Watcher/releases).

Include the [`filewatcher.h`](filewatcher.h) where you want to use the FileWatcher.
Compile with the [`filewatcher.cpp`](filewatcher.cpp) file or with the built dll in the release.

c++17 or later compilation required for cpp compilation.  
No external dependencies.

# Example

Content of [main.cpp](main.cpp)
```cpp
int usage()
{
	std::cout << "Usage: FileWatcher.exe [folderPath]" << std::endl;
	return 1;
}

int main(int argc, const char* argv[])
{
	if (argc != 2) return usage();

	const char* folderPath = argv[1];
	bool bRecursive = true;
	bool bWatching = true;

	cWatch(folderPath, bRecursive, cPrintFileEvent, &bWatching);

	std::cout << "Press enter to stop watching" << std::endl;
	std::cin.get();

	bWatching = false;

	std::cout << "Watching stopped, press enter to exit" << std::endl;
	std::cin.get();

	return 0;
}
```

Possible output:
```
Press enter to stop watching
D:\Projets\C++\File-Watcher\main.cpp modified
D:\Projets\C++\File-Watcher\filewatcher.h removed
D:\Projets\C++\File-Watcher\filewatcher.h added
D:\Projets\C++\File-Watcher\filewatcher.h renamed_old
D:\Projets\C++\File-Watcher\filewatcher.hh renamed_new
D:\Projets\C++\File-Watcher\filewatcher.hh renamed_old
D:\Projets\C++\File-Watcher\filewatcher.h renamed_new

Watching stopped, press enter to exit
```

# Usage

Most important:
```cpp
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
```

# Licence

MIT Licence. See [LICENSE file](LICENSE).
Please refer me with:

	Copyright (c) Nicolas VENTER All rights reserved.
