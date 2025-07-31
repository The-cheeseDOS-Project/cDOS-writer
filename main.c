/*
 * cheeseDOS Writer -  Write cheeseDOS to a CD, DVD, or USB without compiling cheeseDOS 
 * Copyright (C) 2025  Connor Thomson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define VER "1.1.1"
#define SOURCE "https://api.github.com/repos/The-cheeseDOS-Project/cheeseDOS/releases/latest"
#define TMP "/tmp/cdos.iso"
#define DISK_LISTER "lsblk"
#define BURNER "wodim"

// Translate user input into something the program can understand
void prompt(const char *msg, char *input, size_t size) {
    printf("%s", msg);
    fflush(stdout);
    fgets(input, size, stdin);
    input[strcspn(input, "\n")] = 0;
}

// Main
int main() {
    // Print out version
    printf("cDOS-writer v%s\n\n", VER);
   
    // Check for root
    if (getuid() != 0) {
        fprintf(stderr, "This tool must be run as root.\n");
        return 1;
    }
    
    // Ask user if they want to download the ISO
    char input[32];
    prompt("Do you want to download the latest version of cheeseDOS via GitHub? (y/n): ", input, sizeof(input));
    if (strcmp(input, "y") != 0) {
        printf("Aborted.\n");
        return 0;
    }
    
    // Download the ISO
    char download_cmd[512];
    snprintf(download_cmd, sizeof(download_cmd),
        "curl -L \"$(curl -s %s | grep 'browser_download_url' | cut -d '\"' -f 4)\" -o %s",
        SOURCE, TMP);

    // Print failed message
    int dl = system(download_cmd);
    if (dl != 0) {
        fprintf(stderr, "Download failed.\n");
        return 1;
    }

    // Ask user what media they are using
    prompt("What type of media do you want to write this to? (cd/dvd/other): ", input, sizeof(input));
    char media[16];
    strncpy(media, input, sizeof(media));
    media[sizeof(media) - 1] = '\0';

    // List all disks
    puts("\nAvailable devices:");
    system(DISK_LISTER);

    // Ask user where to write the ISO to
    prompt("\nWhere do you want to write this to? (e.g., sdb, sr0): ", input, sizeof(input));
    char device[32];
    snprintf(device, sizeof(device), "/dev/%s", input);

    // Sanity check
    char confirm[8];
    printf("\nARE YOU SURE YOU WANT TO DO THIS? ALL DATA ON %s WILL BE DESTROYED! (y/n): ", device);
    fgets(confirm, sizeof(confirm), stdin);
    if (confirm[0] != 'y') {
        printf("Aborted.\n");
        return 0;
    }

    // Fail unless the command returns success
    int result = 1;

    // Write to CD or DVD
    if (strcmp(media, "cd") == 0 || strcmp(media, "dvd") == 0) {
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "%s dev=%s %s", BURNER, device, TMP);
        result = system(cmd);
    } 
    
    // Write to "other" device
    else {
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "dd if=%s of=%s bs=4M status=progress && sync", TMP, device);
        result = system(cmd);
    }

    // Print failed message
    if (result != 0) {
        fprintf(stderr, "Failed to write ISO.\n");
        return 1;
    }

    // Show done message
    unlink(TMP);
    printf("\nDone! cheeseDOS has been written to %s\n", device);
    return 0;
}
