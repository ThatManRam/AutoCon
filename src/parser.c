#include "parser.h"
#include "nmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#define TEXT_BUFFER_SIZE 256

static void parse_host(xmlNode *host_node, FILE *report, int *host_count) {
    char ip[TEXT_BUFFER_SIZE] = "Unknown";
    char mac[TEXT_BUFFER_SIZE] = "Unknown";
    char vendor[TEXT_BUFFER_SIZE] = "Unknown";
    char hostname[TEXT_BUFFER_SIZE] = "Unknown";

    int is_up = 0;

    for (xmlNode *child = host_node->children;
         child != NULL;
         child = child->next) {

        if (child->type != XML_ELEMENT_NODE) {
            continue;
        }

        if (xmlStrcmp(child->name, (const xmlChar *)"status") == 0) {
            xmlChar *state =
                xmlGetProp(child, (const xmlChar *)"state");

            if (state &&
                xmlStrcmp(state, (const xmlChar *)"up") == 0) {
                is_up = 1;
            }

            if (state) {
                xmlFree(state);
            }
        }

        else if (xmlStrcmp(child->name, (const xmlChar *)"address") == 0) {
            xmlChar *addr =
                xmlGetProp(child, (const xmlChar *)"addr");

            xmlChar *addrtype =
                xmlGetProp(child, (const xmlChar *)"addrtype");

            xmlChar *vendor_prop =
                xmlGetProp(child, (const xmlChar *)"vendor");

            if (addr && addrtype) {
                if (xmlStrcmp(addrtype, (const xmlChar *)"ipv4") == 0) {
                    snprintf(ip, sizeof(ip), "%s", (char *)addr);
                }
                else if (xmlStrcmp(addrtype, (const xmlChar *)"mac") == 0) {
                    snprintf(mac, sizeof(mac), "%s", (char *)addr);

                    if (vendor_prop) {
                        snprintf(
                            vendor,
                            sizeof(vendor),
                            "%s",
                            (char *)vendor_prop
                        );
                    }
                }
            }

            if (addr) {
                xmlFree(addr);
            }

            if (addrtype) {
                xmlFree(addrtype);
            }

            if (vendor_prop) {
                xmlFree(vendor_prop);
            }
        }

        else if (xmlStrcmp(child->name, (const xmlChar *)"hostnames") == 0) {
            for (xmlNode *hn = child->children;
                 hn != NULL;
                 hn = hn->next) {

                if (hn->type != XML_ELEMENT_NODE) {
                    continue;
                }

                if (xmlStrcmp(hn->name, (const xmlChar *)"hostname") == 0) {
                    xmlChar *name =
                        xmlGetProp(hn, (const xmlChar *)"name");

                    if (name) {
                        snprintf(
                            hostname,
                            sizeof(hostname),
                            "%s",
                            (char *)name
                        );

                        xmlFree(name);
                    }
                }
            }
        }
    }

    if (is_up) {
        (*host_count)++;

        fprintf(report, "Host %d\n", *host_count);
        fprintf(report, "------------------------------\n");
        fprintf(report, "IP Address : %s\n", ip);
        fprintf(report, "MAC Address: %s\n", mac);
        fprintf(report, "Vendor     : %s\n", vendor);
        fprintf(report, "Hostname   : %s\n", hostname);
        fprintf(report, "\n");
    }
}

int parse_nmap_xml(const char *subnet) {
    xmlDoc *doc = xmlReadFile(XML_FILE, NULL, 0);

    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", XML_FILE);
        return -1;
    }

    xmlNode *root = xmlDocGetRootElement(doc);

    if (root == NULL) {
        fprintf(stderr, "XML file has no root element.\n");
        xmlFreeDoc(doc);
        return -1;
    }

    FILE *report = fopen(REPORT_FILE, "w");

    if (!report) {
        perror("fopen failed");
        xmlFreeDoc(doc);
        return -1;
    }

    fprintf(report, "Network Mapping Report\n");
    fprintf(report, "======================\n");
    fprintf(report, "Detected Subnet: %s\n\n", subnet);

    int host_count = 0;

    for (xmlNode *node = root->children;
         node != NULL;
         node = node->next) {

        if (node->type == XML_ELEMENT_NODE &&
            xmlStrcmp(node->name, (const xmlChar *)"host") == 0) {
            parse_host(node, report, &host_count);
        }
    }

    fprintf(report, "======================\n");
    fprintf(report, "Total Active Hosts: %d\n", host_count);

    fclose(report);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}