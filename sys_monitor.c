#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

void get_cpu_usage() {
    long double a[4], b[4], loadavg;
    FILE *fp;

    fp = fopen("/proc/stat","r");
    fscanf(fp, "cpu %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat","r");
    fscanf(fp, "cpu %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
    fclose(fp);

    loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));
    printf("The current CPU utilization is : %Lf%%\n", loadavg*100);
}

void get_mem_usage() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if(fp == NULL) {
        perror("Unable to open /proc/meminfo");
        exit(1);
    }

    int total_memory = 0;
    int free_memory = 0;
    char buffer[256];

    while(fgets(buffer, sizeof(buffer), fp)) {
        if(strncmp(buffer, "MemTotal:", 9) == 0) {
            sscanf(buffer, "MemTotal: %d kB", &total_memory);
        }
        if(strncmp(buffer, "MemFree:", 8) == 0) {
            sscanf(buffer, "MemFree: %d kB", &free_memory);
        }
    }

    fclose(fp);
    printf("The current memory utilization is : %d%%\n", 100 * (total_memory - free_memory) / total_memory);
}

int main(int argc, char **argv) {
    int opt;
    float interval = 1.0;

    static struct option long_options[] = {
        {"interval", required_argument, 0, 'i'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "i:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'i':
                interval = atof(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s --interval <seconds>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (interval <= 0) {
        fprintf(stderr, "Interval must be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    printf("Monitoring CPU and memory every %f seconds.\n", interval);

    while (1) {
        get_cpu_usage();
        get_mem_usage();
        usleep(interval * 1000000); // microseconds
    }
    return 0;
}
