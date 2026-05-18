#ifndef DNS_H
#define DNS_H

int run_dig_lookup(const char *target, const char *output_file);
int run_reverse_dig_lookup(const char *ip, const char *output_file);

#endif