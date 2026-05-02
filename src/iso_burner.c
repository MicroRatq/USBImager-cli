#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#endif
#include "stream_minimal.h"
#include "disks.h"

// Globals used by platform-specific disks_*.c
char *device_names[DISKS_MAX];
int num_devices = 0;
int verbose = 0;
int baud = 115200;

void main_addToCombobox(char *text) {
    if (num_devices < DISKS_MAX) {
        device_names[num_devices] = strdup(text);
        num_devices++;
    }
}

void main_getErrorMessage(void) {
#ifdef _WIN32
    fprintf(stderr, "Disk Error: %lu\n", GetLastError());
#else
    fprintf(stderr, "Disk Error: %s\n", strerror(errno));
#endif
}

void main_onProgress(void *data) {
    // Unused
}

// Helper to list devices as JSON
void list_devices_json() {
    num_devices = 0;
    disks_refreshlist();
    printf("[\n");
    for (int i = 0; i < num_devices; i++) {
        // Find disk id from disks_targets array
        int disk_id = disks_targets[i];
        uint64_t cap = disks_capacity[i];
        printf("  {\"id\": %d, \"name\": \"%s\", \"capacity\": %" PRIu64 "}%s\n",
               disk_id, device_names[i], cap,
               (i == num_devices - 1) ? "" : ",");
    }
    printf("]\n");
}

typedef void (*progress_callback_t)(int percent);

void console_progress(int percent) {
    printf("\rProgress: %d%%", percent);
    fflush(stdout);
}

int burn_iso_image(const char *iso_file, int disk_id, int verify, progress_callback_t callback) {
    stream_t ctx;
    if (!stream_open(&ctx, iso_file, 1)) {
        fprintf(stderr, "Failed to open ISO file: %s\n", iso_file);
        return 0;
    }

    // Refresh list so disks_targets are mapped correctly
    num_devices = 0;
    disks_refreshlist();

    // Find index of the disk_id
    int target_idx = -1;
    for (int i = 0; i < num_devices; i++) {
        if (disks_targets[i] == disk_id) {
            target_idx = i;
            break;
        }
    }

    if (target_idx == -1) {
        fprintf(stderr, "Invalid disk ID: %d\n", disk_id);
        stream_close(&ctx);
        return 0;
    }

    void *hDrive = disks_open(target_idx, ctx.fileSize);
    if (!hDrive || hDrive == (void*)-1 || hDrive == (void*)-2 || hDrive == (void*)-3) {
        fprintf(stderr, "Failed to open drive for writing.\n");
        stream_close(&ctx);
        return 0;
    }

    int buf_size = 1024 * 1024;
    char *buf;
#ifdef _WIN32
    buf = (char *)VirtualAlloc(NULL, buf_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    buf = (char *)malloc(buf_size);
#endif

    if (!buf) {
        fprintf(stderr, "Out of memory.\n");
        disks_close(hDrive);
        stream_close(&ctx);
        return 0;
    }

    printf("Starting burn process...\n");
    int read_bytes = 0;
    while ((read_bytes = stream_read(&ctx, buf, buf_size)) > 0) {
        int write_bytes = read_bytes;
#ifdef _WIN32
        // Pad to 4096-byte sector boundary for raw disk write on Windows (required for NO_BUFFERING)
        if (write_bytes % 4096 != 0) {
            int pad = 4096 - (write_bytes % 4096);
            memset(buf + write_bytes, 0, pad);
            write_bytes += pad;
        }

        DWORD written;
        if (!WriteFile((HANDLE)hDrive, buf, write_bytes, &written, NULL) || written != (DWORD)write_bytes) {
            fprintf(stderr, "\nWrite failed at offset %" PRIu64 " (Error: %lu)\n", ctx.readSize - read_bytes, GetLastError());
            VirtualFree(buf, 0, MEM_RELEASE);
            disks_close(hDrive);
            stream_close(&ctx);
            return 0;
        }
#else
        if (write((int)(intptr_t)hDrive, buf, write_bytes) != (ssize_t)write_bytes) {
            fprintf(stderr, "\nWrite failed at offset %" PRIu64 " (Error: %s)\n", ctx.readSize - read_bytes, strerror(errno));
            free(buf);
            disks_close(hDrive);
            stream_close(&ctx);
            return 0;
        }
#endif
        if (callback) {
            callback(stream_status(&ctx));
        }
    }

    if (callback) {
        callback(100);
        printf("\n");
    }

    printf("Syncing disks...\n");
#ifdef _WIN32
    VirtualFree(buf, 0, MEM_RELEASE);
#else
    free(buf);
#endif
    disks_close(hDrive);
    stream_close(&ctx);
    return 1;
}

void print_help() {
    printf("Usage:\n");
    printf("  usbimager-cli --list\n");
    printf("  usbimager-cli --write <iso_file> <disk_id>\n");
}

int main(int argc, char **argv) {
    extern int disks_maxsize;
    disks_maxsize = 0; // Don't limit by size
    
    if (argc < 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help();
        return (argc < 2) ? 1 : 0;
    }

    if (strcmp(argv[1], "--list") == 0) {
        list_devices_json();
        return 0;
    }

    if (strcmp(argv[1], "--write") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Missing arguments for --write.\n");
            print_help();
            return 1;
        }
        const char *iso_file = argv[2];
        int disk_id = atoi(argv[3]);
        
        if (burn_iso_image(iso_file, disk_id, 0, console_progress)) {
            printf("Burn completed successfully.\n");
            return 0;
        } else {
            fprintf(stderr, "Burn failed.\n");
            return 1;
        }
    }

    print_help();
    return 1;
}
