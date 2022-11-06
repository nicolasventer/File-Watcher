# Load dll filewatcher.dll
import ctypes
import os
import time

# Get the path of the dll file
dll_path = os.path.join(os.path.dirname(
    os.path.abspath(__file__)), 'filewatcher.dll')
# Load the dll file
dll = ctypes.cdll.LoadLibrary(dll_path)

# Define the callback function

# Callback function type: const char *file, file_watcher_event_type event, bool is_directory, void *data


def callback(file, event, is_directory, data):
    print('Callback: file = {}, event = {}, is_directory = {}, data = {}'.format(
        file, event, is_directory, data))


# Define the callback function type
callback_type = ctypes.CFUNCTYPE(
    None, ctypes.c_char_p, ctypes.c_int, ctypes.c_bool, ctypes.c_void_p)

# Define the callback function
callback_func = callback_type(callback)

print('Start watching, press Ctrl+C to stop...')

# file_watcher_watch_async (const char* folder_path,
# 		bool b_recursive,
# 		file_watcher_callback callback,
# 		bool* bWatching = nullptr,
# 		void* data = nullptr);

# Call the function
folder_path = b'.'
b_recursive = True
bWatching = ctypes.c_bool()
dll.file_watcher_watch_async(folder_path, b_recursive,
                             callback_func, ctypes.byref(bWatching), None)

# Wait for Ctrl+C
try:
    while True:
        time.sleep(10)
        print('Watching...')
except KeyboardInterrupt:
    print('Stop watching...')
