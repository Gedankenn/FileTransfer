#ifndef __FILE_TRANSFER_PROTOCOL__
#define __FILE_TRANSFER_PROTOCOL__

#include <stdint.h>
#include <stddef.h>

#define HELLO_MESSAGE "O Pai Ta On"

/*
 * Wire format (all integers big-endian):
 *   header: "FTP1" | u32 entry_count | u64 total_size
 *   entry : u8 type (0 = file, 1 = directory) | u16 path_len | path bytes
 *           (relative, '/'-separated, no terminator) | u64 size
 *           followed by <size> bytes of content for files.
 * Entries are sent parents first, so a directory always arrives before
 * anything inside it.
 */
#define FTP_MAGIC "FTP1"

enum file_transfer_errors
{
    OK = 0,
    WRONG_SIZE = -1,
    ERROR_WRITE_TO_SOCKET = -2,
    CANT_CREATE_DIR = -3,
    CANT_CREATE_FILE = -4,
    CANT_CONNECT = -5,
    CANT_RECEIVE_BIN_DATA = -6,
    BAD_HEADER = -7,
    BAD_PATH = -8,
    CANT_READ_FILE = -9,
};

int send_all(int sfd, const void* data, size_t size);
int recv_all(int sfd, void* data, size_t size);
int send_u64(int sfd, uint64_t v);
int recv_u64(int sfd, uint64_t* v);
int send_header(int sfd, uint32_t entry_count, uint64_t total_size);
int recv_header(int sfd, uint32_t* entry_count, uint64_t* total_size);
int send_entry(int sfd, int is_dir, const char* rel_path, const char* src_path, uint64_t size);
long long recv_entry(int sfd, const char* dest_dir);

#endif
