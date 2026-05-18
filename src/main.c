#include <stdio.h>
#include <stdlib.h>

#include "network.h"
#include "nmap.h"
#include "parser.h"
#include "dns.h"
#include "output.h"
#include <time.h>
#include <string.h>



int main(void) {
    char subnet[SUBNET_BUFFER_SIZE];

    printf("[*] Detecting local subnet...\n");

    if (create_output_directory() != 0) {
        fprintf(stderr, "Could not create output directory.\n");
        return EXIT_FAILURE;
    }


    if (get_local_subnet(subnet, sizeof(subnet)) != 0) {
        fprintf(stderr, "Could not determine a usable local IPv4 subnet.\n");
        return EXIT_FAILURE;
    }

    printf("[+] Detected subnet: %s\n", subnet);

    printf("[*] Running Nmap scan...\n");

    if (run_nmap(subnet) != 0) {
        fprintf(stderr, "Nmap scan failed.\n");
        return EXIT_FAILURE;
    }

    printf("[*] Parsing Nmap XML results...\n");

    if (parse_nmap_xml(subnet) != 0) {
        fprintf(stderr, "Could not create network report.\n");
        return EXIT_FAILURE;
    }

    /*
     * Example dig calls.
     * You can remove these or make them optional later.
     */
    printf("[*] Running DNS lookup for example.com...\n");

    if (run_dig_lookup("example.com", "DIG_EXAMPLE_FILE") != 0) {
        fprintf(stderr, "dig lookup failed.\n");
    }

    printf("[*] Running reverse DNS lookup for 8.8.8.8...\n");

    if (run_reverse_dig_lookup("8.8.8.8", "DIG_REVERSE_FILE") != 0) {
        fprintf(stderr, "reverse dig lookup failed.\n");
    }

    printf("[+] Report written to: %s\n", REPORT_FILE);
    printf("[+] Raw Nmap XML saved to: %s\n", XML_FILE);
    printf("[+] DNS lookup saved to: %s\n", DIG_EXAMPLE_FILE);
    printf("[+] Reverse DNS lookup saved to: %s\n", DIG_REVERSE_FILE);



    //saves everyhting onto the MEGA
    struct tm *info;
    time_t raw_time = time(NULL);
    info = localtime(&raw_time);

    char command[512];
    char time_string[100];

    // Store the timestamp in time_string
    if (strftime(
            time_string,
            sizeof(time_string),
            "%Y-%m-%d-%H-%M-%S",
            info
        ) == 0) {
        fprintf(stderr, "Failed to format timestamp.\n");
        return EXIT_FAILURE;
    }



snprintf(
    command,
    sizeof(command),
    "python3 MEGA_Upload.py \"%s\" \"$(pwd)/output\"",
    time_string
);

system(command);

    return EXIT_SUCCESS;
}