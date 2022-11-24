# Load dll filewatcher.dll
import ctypes
import os
import time

# Get the path of the dll file
dll_path = os.path.join(os.path.dirname(
    os.path.abspath(__file__)), 'filewatcher.dll')
# Load the dll file
dll = ctypes.cdll.LoadLibrary(dll_path)

# void (*file_watcher_callback) (const char *file, file_watcher_event_type event, bool is_directory, void *data)

# Define the callback function type
callback_type = ctypes.CFUNCTYPE(
    None, ctypes.c_char_p, ctypes.c_int, ctypes.c_bool, ctypes.c_void_p)

# void file_watcher_watch_async (const char* folder_path,
# 		bool b_recursive,
# 		file_watcher_callback callback,
# 		bool* bWatching,
# 		void* data);

# Define the function
dll.file_watcher_watch_async.argtypes = [
    ctypes.c_char_p, ctypes.c_bool, callback_type, ctypes.POINTER(ctypes.c_bool), ctypes.c_void_p]
dll.file_watcher_watch_async.restype = None


def callback(file, event, is_directory, data):
    print('Callback: file = {}, event = {}, is_directory = {}, data = {}'.format(
        file, event, is_directory, data))


print('Start watching, press Ctrl+C to stop...')

# Call the function
folder_path = b'.'
b_recursive = True
bWatching = ctypes.c_bool()
callback_func = callback_type(callback)
dll.file_watcher_watch_async(folder_path, b_recursive,
                             callback_func, ctypes.byref(bWatching), None)

# Wait for Ctrl+C
try:
    while True:
        time.sleep(10)
        print('Watching...')
except KeyboardInterrupt:
    print('Stop watching...')
