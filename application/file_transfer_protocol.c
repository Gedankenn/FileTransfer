#include "file_transfer_protocol.h"
#include "file_handler.h"
#include "color.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define CHUNK 65536

int send_all(int sfd, const void* data, size_t size)
{
    const unsigned char* p = data;
    while (size > 0)
    {
        ssize_t n = send(sfd, p, size, 0);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            return ERROR_WRITE_TO_SOCKET;
        p += n;
        size -= (size_t)n;
    }
    return OK;
}

int recv_all(int sfd, void* data, size_t size)
{
    unsigned char* p = data;
    while (size > 0)
    {
        ssize_t n = recv(sfd, p, size, 0);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            return CANT_RECEIVE_BIN_DATA;
        p += n;
        size -= (size_t)n;
    }
    return OK;
}

int send_u64(int sfd, uint64_t v)
{
    unsigned char b[8];
    for (int i = 7; i >= 0; i--) { b[i] = v & 0xff; v >>= 8; }
    return send_all(sfd, b, 8);
}

int recv_u64(int sfd, uint64_t* v)
{
    unsigned char b[8];
    if (recv_all(sfd, b, 8) != OK)
        return CANT_RECEIVE_BIN_DATA;
    *v = 0;
    for (int i = 0; i < 8; i++) *v = (*v << 8) | b[i];
    return OK;
}

int send_header(int sfd, uint32_t entry_count, uint64_t total_size)
{
    uint32_t n = htonl(entry_count);
    if (send_all(sfd, FTP_MAGIC, 4) != OK || send_all(sfd, &n, 4) != OK)
        return ERROR_WRITE_TO_SOCKET;
    return send_u64(sfd, total_size);
}

int recv_header(int sfd, uint32_t* entry_count, uint64_t* total_size)
{
    char magic[4];
    uint32_t n;
    if (recv_all(sfd, magic, 4) != OK || memcmp(magic, FTP_MAGIC, 4) != 0)
        return BAD_HEADER;
    if (recv_all(sfd, &n, 4) != OK)
        return BAD_HEADER;
    *entry_count = ntohl(n);
    return recv_u64(sfd, total_size);
}

/* Send one entry; for files, stream the content from src_path. */
int send_entry(int sfd, int is_dir, const char* rel_path, const char* src_path, uint64_t size)
{
    size_t len = strlen(rel_path);
    if (len == 0 || len >= FILE_PATH_SIZE)
        return BAD_PATH;
    unsigned char type = is_dir ? 1 : 0;
    uint16_t plen = htons((uint16_t)len);
    if (send_all(sfd, &type, 1) != OK || send_all(sfd, &plen, 2) != OK ||
        send_all(sfd, rel_path, len) != OK || send_u64(sfd, is_dir ? 0 : size) != OK)
        return ERROR_WRITE_TO_SOCKET;
    if (is_dir)
        return OK;

    FILE* fp = fopen(src_path, "rb");
    if (fp == NULL)
    {
        perror(src_path);
        return CANT_READ_FILE;
    }
    unsigned char* buf = malloc(CHUNK);
    if (buf == NULL) { fclose(fp); return CANT_READ_FILE; }
    uint64_t left = size;
    int ret = OK;
    while (left > 0)
    {
        size_t want = left > CHUNK ? CHUNK : (size_t)left;
        size_t got = fread(buf, 1, want, fp);
        if (got != want) { ret = CANT_READ_FILE; break; }  /* file changed while sending */
        if (send_all(sfd, buf, got) != OK) { ret = ERROR_WRITE_TO_SOCKET; break; }
        left -= got;
    }
    free(buf);
    fclose(fp);
    return ret;
}

/* Reject absolute paths and any ".." component coming from the network. */
static int safe_rel_path(const char* p)
{
    if (p[0] == '/' || p[0] == '\\')
        return 0;
    const char* s = p;
    while (*s)
    {
        const char* e = s;
        while (*e && *e != '/' && *e != '\\') e++;
        if (e - s == 2 && s[0] == '.' && s[1] == '.')
            return 0;
        s = *e ? e + 1 : e;
    }
    return 1;
}

/* Receive one entry into dest_dir. Returns the bytes of file content
 * received (0 for directories) or a negative error. */
long long recv_entry(int sfd, const char* dest_dir)
{
    unsigned char type;
    uint16_t plen;
    uint64_t size;
    char rel[FILE_PATH_SIZE];
    char out[FILE_PATH_SIZE * 2];

    if (recv_all(sfd, &type, 1) != OK || recv_all(sfd, &plen, 2) != OK)
        return CANT_RECEIVE_BIN_DATA;
    plen = ntohs(plen);
    if (plen == 0 || plen >= FILE_PATH_SIZE)
        return BAD_PATH;
    if (recv_all(sfd, rel, plen) != OK)
        return CANT_RECEIVE_BIN_DATA;
    rel[plen] = '\0';
    if (recv_u64(sfd, &size) != OK)
        return CANT_RECEIVE_BIN_DATA;
    if (!safe_rel_path(rel))
    {
        printf("%sRefusing unsafe path: %s%s\n", ANSI_RED, rel, ANSI_RESET);
        return BAD_PATH;
    }
    snprintf(out, sizeof out, "%s/%s", dest_dir, rel);

    if (type == 1)
    {
        printf("directory: %s\n", rel);
        return create_dir(out) ? 0 : CANT_CREATE_DIR;
    }

    printf("file: %s (%llu bytes)\n", rel, (unsigned long long)size);
    FILE* fp = fopen(out, "wb");
    if (fp == NULL)
    {
        perror(out);
        return CANT_CREATE_FILE;
    }
    unsigned char* buf = malloc(CHUNK);
    if (buf == NULL) { fclose(fp); return CANT_CREATE_FILE; }
    uint64_t left = size;
    long long ret = (long long)size;
    while (left > 0)
    {
        size_t want = left > CHUNK ? CHUNK : (size_t)left;
        if (recv_all(sfd, buf, want) != OK) { ret = CANT_RECEIVE_BIN_DATA; break; }
        if (fwrite(buf, 1, want, fp) != want) { ret = CANT_CREATE_FILE; break; }
        left -= want;
    }
    free(buf);
    if (fclose(fp) != 0 && ret >= 0)
        ret = CANT_CREATE_FILE;
    return ret;
}
