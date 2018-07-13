#!/bin/bash
cd template/; make; cd ../
make intmap_check
make print_assembly_check009
make full_compile
