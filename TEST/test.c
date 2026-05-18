#include <stdio.h>
#include <time.h>
#include <stdlib.h>


int main(){
    struct tm *info;
    time_t raw_time = time(NULL);
    info = localtime(&raw_time);


    char buf[50];
    char time[50];

    
    strftime(buf, 80, "%Y-%m-%d %H:%M:%S", info);


    sprintf(buf,"python3 MEGA_Upload.py %s $(pwd)/output/scan.xml",time);
    system(buf);
    strcpy(buf, "python3 MEGA_Upload.py %s $(pwd)/output/network_map.txt", time); 
    system(buf);
}