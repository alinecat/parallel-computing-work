#ifndef UTIL_H
#define UTIL_H

struct keyPair {
    int firstKey;
    int secondKey;
};

void read_file_part_a(const char *file_name, int **array, int *size);

void read_file_part_b(const char *file_name, keyPair **kpArray, int *size);



#endif
