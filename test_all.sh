#!/bin/bash
# cd template/; make; cd ../
make format
make supplement
make intmap_check
make typeparse_check
make assembly_sandbox
make full_compile
make check_error
