/* buggy.c
 * --------
 * A program jam-packed with memory errors.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reporter.h"

/* This function is designed to make a variety of memory
 * errors so you can observe their results under gdb/valgrind.
 * Give the program a command-line arg which identifies whic
 * of the following errors you want to observe.
 */
void MakeMemoryError(int which)
{
    int *ptr;
    char *str, stackarr[4];

    switch (which) {

        case 1:
            *ptr = 123; // error #1: dereference uninitialized ptr
            break;

        case 2 :
            str = malloc(8); // error #2: allocation doesn't include space for null char
            strcpy(str, "too long");
            free(str);
            break;

       case 3:
            strncpy(stackarr, "cs", 2);
            printf("%s", stackarr); // error #3: reads past end of string (missing null terminator)
            break;

        case 4:
            stackarr[11] = 'Y'; // error #4: out of bounds on stack array
            break;

        case 5:
            str = malloc(4);
            free(str);
            *str = 'A'; // error #5: write to freed memory
            break;

        case 6:
            str = malloc(4);
            free(str);
            free(str); // error #6: free pointer that was previously freed
            break;

        case 7:
            str = "washington";
            str[0] = 'W'; // error #7: write to a string constant (read only)
            break;
    }
}


int main(int argc, const char *argv[])
{
    InitReporter();
    if (argc != 2) {
        printf("Usage: %s <num>\n", argv[0]);
        printf("<num> is a value from 1 to 7 identifying which error to execute.\n");
        exit(1);
    }
    MakeMemoryError(atoi(argv[1]));
    return 0;
}
