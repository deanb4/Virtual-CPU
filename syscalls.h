#ifndef SYSCALLS_H
#define SYSCALLS_H

enum Syscall {
    PRINT_INT = 1,       // Print integer ($a0)
    PRINT_FLOAT = 2,     // Print float ($f12)
    PRINT_DOUBLE = 3,    // Print double ($f12)
    PRINT_STRING = 4,    // Print string ($a0)
    READ_INT = 5,        // Read integer -> $v0
    READ_FLOAT = 6,      // Read float -> $f0
    READ_DOUBLE = 7,     // Read double -> $f0
    READ_STRING = 8,     // Read string ($a0 = buffer, $a1 = length)
    EXIT = 9,            // Exit program
    EXIT_STATUS = 10,    // Exit with status ($a0)
    OPEN_FILE = 13,      // Open file ($a0 = filename, $a1 = flags, $a2 = mode) -> $v0 (fd)
    READ_FILE = 14,      // Read file ($a0 = fd, $a1 = buffer, $a2 = bytes) -> $v0 (bytes read)
    WRITE_FILE = 15,     // Write file ($a0 = fd, $a1 = buffer, $a2 = bytes) -> $v0 (bytes written)
    CLOSE_FILE = 16,     // Close file ($a0 = fd)
    EXIT_WITH_STATUS = 17 // Exit with status ($a0)
};

// flags for files
enum FileAccessFlags {
    RDONLY    = 0,        // Read-only mode
    WRONLY    = 1,        // Write-only mode
    RDWR      = 2,        // Read and write mode
    CREAT     = 64,       // Create the file if it doesn't exist
    EXCL      = 128,      // Fail if the file already exists (exclusive)
    APPEND    = 8,        // Open the file in append mode
    TRUNC     = 512,      // Truncate the file to 0 length if it already exists
    NONBLOCK  = 2048,     // Non-blocking mode (I/O operations won't block)
    SYNC      = 1052672,  // Synchronous I/O, write operations will wait for completion
    DSYNC     = 4096,     // Data synchronization, ensures data is written before returning
    RSYNC     = 1052672,  // Same as O_SYNC, but applies only to read operations
    DIRECTORY = 16384,    // Fail if the file is not a directory
    NOFOLLOW  = 131072,   // Fail if the file is a symbolic link
    CLOEXEC   = 524288,   // Set close-on-exec flag (file descriptor is closed on exec)
    PATH      = 2097152   // Only get file descriptor, no I/O allowed
};




#endif