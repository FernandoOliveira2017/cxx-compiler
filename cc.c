#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cc.h"

#define USAGE   "C Compiler - Copyright (c) 2017\n"			\
                "Usage: cc [options] input\n"				\
                "	-o output	set output file\n"			\
                "	-h			display this information\n"

FILE * object;

static void parse_options(int argc, char * argv[])
{
    char * filename = NULL;
    
    for (int i = 1; i < argc; i++)
        if (argv[i][0] == '-')
            if (strcmp(argv[i], "-o") == 0)
                if (object != NULL)
                    error("multiples output files");
                else if (++i < argc) {
                    object = fopen(argv[i], "w+");
                    if (object == NULL)
                        error("%s: unable to create file", argv[i]);
                } else
                    error("missing output file");
            else if (strcmp(argv[i], "-h") == 0) {
                printf(USAGE);
                exit(EXIT_SUCCESS);
            } else
                error("unrecognized option `%s'", argv[i]);
        else {
            int len = strlen(argv[i]);
            if (strcmp(argv[i]+len-2, ".c")) {
				printf("%s: ", argv[i]);
                error("invalid file type");
			} else
				program(filename = argv[i]);
        }
    if (filename == NULL)
        error("no input files");
}

int main(int argc, char * argv[])
{
    if (argc > 1)
        parse_options(argc, argv);
	else
        printf(USAGE);
    
    return 0;
}
