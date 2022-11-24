#include <chrono>
#include <iostream>
#include <string>

#define FILE_WATCHER_IMPLEMENTATION
#include "file_watcher.hpp"

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

	file_watcher_watch_async(folderPath, bRecursive, file_watcher_print_file_event, &bWatching, nullptr);

	std::cout << "Press enter to stop watching" << std::endl;
	std::cin.get();

	bWatching = false;

	std::cout << "Watching stopped, press enter to exit" << std::endl;
	std::cin.get();

	return 0;
}