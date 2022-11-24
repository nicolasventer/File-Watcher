# File Watcher

# Description

FileWatcher is a **c++ library** that allows you to **watch a folder** and get notified of any file changes inside.

# Features

- watch a file or a folder
- execute the callback for all files
- exported c functions for use in other languages (see example: [test_file_watcher.py](test_file_watcher.py))

# Installation

*(Windows only)*

### Header only

Include the [`file_watcher.hpp`](file_watcher.hpp) anywhere you want to use it.  
And above only one include, define `FILE_WATCHER_IMPLEMENTATION` like this:

```cpp
#define FILE_WATCHER_IMPLEMENTATION
#include "file_watcher.hpp"
```

### Header and source

Include the [`file_watcher.hpp`](file_watcher.hpp) anywhere you want to use it.    
Compile with the [`file_watcher.cpp`](file_watcher.cpp) file or with the built dll available [here](https://github.com/nicolasventer/File-Watcher/releases).

### Build the dll

```bash
g++ -shared -O3 -fPIC -static -o file_watcher.dll file_watcher.cpp
```

Note: the `-static` flag is required.

### Requirements

c++17 or later required for compilation.  
No external dependencies.

# Example

Content of [main.cpp](main.cpp)
```cpp
#include <chrono>
#include <iostream>
#include <string>

#include "file_watcher.hpp"

int usage()
{
	std::cout << "Usage: File_Watcher.exe [folderPath]" << std::endl;
	return 1;
}

int main(int argc, const char* argv[])
{
	if (argc != 2) return usage();

	const char* folderPath = argv[1];
	bool bRecursive = true;
	bool bWatching = true;

	file_watcher_watch_async(folderPath, bRecursive, file_watcher_print_file_event, &bWatching, nullptr);

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
D:\Projets\C++\File-Watcher\file_watcher.h removed
D:\Projets\C++\File-Watcher\file_watcher.h added
D:\Projets\C++\File-Watcher\file_watcher.h renamed_old
D:\Projets\C++\File-Watcher\file_watcher.hh renamed_new
D:\Projets\C++\File-Watcher\file_watcher.hh renamed_old
D:\Projets\C++\File-Watcher\file_watcher.h renamed_new

Watching stopped, press enter to exit
```

# Usage

```cpp
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
```

# Licence

MIT Licence. See [LICENSE file](LICENSE).
Please refer me with:

	Copyright (c) Nicolas VENTER All rights reserved.
