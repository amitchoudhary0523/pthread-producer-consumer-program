#!/bin/bash

set -x

rm -f pthread_producer_consumer_program.out

gcc -Wall -Werror -Wextra -Wundef -Wunreachable-code -Winit-self -Wparentheses -Wconversion -Wsign-conversion -Wsign-compare -Werror-implicit-function-declaration -Wmissing-prototypes -Wmissing-declarations -Wformat-security pthread_producer_consumer_program.c -o pthread_producer_consumer_program.out -lpthread

