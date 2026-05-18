#ifndef OUTPUT_H
#define OUTPUT_H

#define OUTPUT_DIR "output"

#define XML_FILE OUTPUT_DIR "/scan.xml"
#define REPORT_FILE OUTPUT_DIR "/network_map.txt"

#define DIG_EXAMPLE_FILE OUTPUT_DIR "/dig_example.txt"
#define DIG_REVERSE_FILE OUTPUT_DIR "/dig_reverse.txt"

int create_output_directory(void);

#endif