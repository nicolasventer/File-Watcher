#include <chrono>
#include <iostream>
#include <string>

#include "filewatcher.h"

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