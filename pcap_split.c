/*
 * pcap_splitter.c
 *
 * Splits a pcap file into multiple smaller files, each starting with the
 * original global header. The chunk size can be specified using a human-readable
 * format (e.g., "100M", "500K", "2G", or "100B"). The output prefix may include a full path.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define GLOBAL_HEADER_SIZE 24
#define PACKET_HEADER_SIZE 16
#define MAX_FILENAME 512

/* Set maximum warnings to print to avoid flooding output */
#define MAX_WARNINGS 10

static int warning_count = 0;

void print_help(const char *progname) {
    printf("Usage: %s input.pcap output_prefix chunk_limit\n", progname);
    printf("\n");
    printf("Splits the pcap file (input.pcap) into smaller chunks. Each chunk\n");
    printf("will start with the original pcap global header. The output files will\n");
    printf("be named as output_prefix_000.pcap, output_prefix_001.pcap, etc. The\n");
    printf("output_prefix may include a path (e.g., /tmp/output/myfile).\n");
    printf("\n");
    printf("The chunk_limit should be specified in a human-readable format, e.g.:\n");
    printf("  100B      (100 bytes)\n");
    printf("  500K      (500 kilobytes)\n");
    printf("  100M      (100 megabytes)\n");
    printf("  2G        (2 gigabytes)\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s file.cap /tmp/output/myfile 100.32M\n", progname);
}

/*
 * parse_size:
 *   Converts a string like "100B", "100K", "100M", or "1G" to a number of bytes.
 *   Accepts an optional decimal point and suffixes B, K, M, or G.
 */
uint64_t parse_size(const char *str) {
    char *end;
    double value = strtod(str, &end);

    if (end == str) {
        fprintf(stderr, "Error: Invalid numeric value '%s'\n", str);
        exit(EXIT_FAILURE);
    }

    if (*end == '\0' || tolower(*end) == 'b') {
        return (uint64_t)value;  // Treat as raw bytes (default or 'B' suffix)
    }

    switch (tolower(*end)) {
        case 'k': value *= 1024; break;
        case 'm': value *= 1024 * 1024; break;
        case 'g': value *= 1024 * 1024 * 1024; break;
        default:
            fprintf(stderr, "Error: Unknown size suffix '%c' in '%s'\n", *end, str);
            exit(EXIT_FAILURE);
    }
    return (uint64_t)value;
}

/*
 * safe_read:
 *   Reads 'size' bytes from 'stream' into 'ptr'. Exits on error.
 */
size_t safe_read(void *ptr, size_t size, FILE *stream) {
    size_t ret = fread(ptr, 1, size, stream);
    if (ret != size && ferror(stream)) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    return ret;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: Too few arguments.\n");
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    /* Check if help is requested */
    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc < 4) {
        fprintf(stderr, "Error: Missing required arguments.\n\n");
        print_help(argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_filename = argv[1];
    const char *output_prefix = argv[2];
    uint64_t chunk_limit = parse_size(argv[3]);

    printf("Processing file: %s\n", input_filename);

    FILE *infile = fopen(input_filename, "rb");
    if (!infile) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    /* Read and store the 24-byte global header */
    unsigned char global_header[GLOBAL_HEADER_SIZE];
    if (safe_read(global_header, GLOBAL_HEADER_SIZE, infile) != GLOBAL_HEADER_SIZE) {
        fprintf(stderr, "Error reading global header\n");
        fclose(infile);
        return EXIT_FAILURE;
    }

    int chunk_count = 0;
    char out_filename[512];
    snprintf(out_filename, sizeof(out_filename), "%s_%03d.pcap", output_prefix, chunk_count);
    printf("Processing: %s\n", out_filename);
    FILE *outfile = fopen(out_filename, "wb");
    if (!outfile) {
        perror("Error opening output file");
        fclose(infile);
        return EXIT_FAILURE;
    }

    /* Write the global header to the first chunk */
    if (fwrite(global_header, 1, GLOBAL_HEADER_SIZE, outfile) != GLOBAL_HEADER_SIZE) {
        fprintf(stderr, "Error writing global header to output file\n");
        fclose(outfile);
        fclose(infile);
        return EXIT_FAILURE;
    }
    uint64_t current_chunk_size = GLOBAL_HEADER_SIZE;

    /* Process packets one by one */
    while (1) {
        unsigned char packet_header[PACKET_HEADER_SIZE];
        size_t bytes_read = fread(packet_header, 1, PACKET_HEADER_SIZE, infile);
        if (bytes_read == 0) { /* End-of-file reached */
            break;
        }
        if (bytes_read < PACKET_HEADER_SIZE) {
            if (warning_count < MAX_WARNINGS)
                fprintf(stderr, "Warning: Incomplete packet header encountered; ignoring remainder of file.\n");
            warning_count++;
            break;
        }

        /* Extract the captured length (incl_len) from bytes 8-11 */
        uint32_t incl_len;
        memcpy(&incl_len, packet_header + 8, sizeof(uint32_t));

        /* Validate incl_len: if it's zero or unreasonably large, skip this packet */
        if (incl_len == 0 || incl_len > 65535) {
            if (warning_count < MAX_WARNINGS)
                fprintf(stderr, "Warning: Invalid packet length (%u). Skipping packet.\n", incl_len);
            warning_count++;
            /* Skip over this packet's data */
            fseek(infile, incl_len, SEEK_CUR);
            continue;
        }

        /* Allocate memory for the packet data */
        unsigned char *packet_data = malloc(incl_len);
        if (!packet_data) {
            fprintf(stderr, "Memory allocation failed for packet data\n");
            fclose(outfile);
            fclose(infile);
            return EXIT_FAILURE;
        }

        size_t data_read = safe_read(packet_data, incl_len, infile);
        if (data_read != incl_len) {
            if (warning_count < MAX_WARNINGS)
                fprintf(stderr, "Warning: Incomplete packet data encountered (expected %u bytes, got %zu). Skipping this packet.\n", incl_len, data_read);
            warning_count++;
            free(packet_data);
            continue;  /* Skip this packet and continue processing */
        }

        /* Check if adding this packet would exceed the chunk limit.
         * If yes, close the current file and open a new one.
         */
        if (current_chunk_size + PACKET_HEADER_SIZE + incl_len > chunk_limit) {
            fclose(outfile);
            chunk_count++;
            snprintf(out_filename, sizeof(out_filename), "%s_%03d.pcap", output_prefix, chunk_count);
            printf("Processing: %s\n", out_filename);
            outfile = fopen(out_filename, "wb");
            if (!outfile) {
                perror("Error opening new output file");
                free(packet_data);
                fclose(infile);
                return EXIT_FAILURE;
            }
            if (fwrite(global_header, 1, GLOBAL_HEADER_SIZE, outfile) != GLOBAL_HEADER_SIZE) {
                fprintf(stderr, "Error writing global header to new output file\n");
                free(packet_data);
                fclose(outfile);
                fclose(infile);
                return EXIT_FAILURE;
            }
            current_chunk_size = GLOBAL_HEADER_SIZE;
        }

        /* Write the packet header and data to the current chunk */
        if (fwrite(packet_header, 1, PACKET_HEADER_SIZE, outfile) != PACKET_HEADER_SIZE ||
            fwrite(packet_data, 1, incl_len, outfile) != incl_len) {
            fprintf(stderr, "Error writing packet data to output file\n");
            free(packet_data);
            fclose(outfile);
            fclose(infile);
            return EXIT_FAILURE;
        }
        current_chunk_size += PACKET_HEADER_SIZE + incl_len;
        free(packet_data);
    }

    fclose(outfile);
    fclose(infile);
    return EXIT_SUCCESS;
}