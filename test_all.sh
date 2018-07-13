#!/bin/bash
cd template/; make; cd ../
make format
make intmap_check
make assembly_sandbox
make full_compile
