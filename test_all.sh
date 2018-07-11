#!/bin/bash
cd template/; make; cd ../
make test_task001
make test_task002
make lexer_check
make print_assembly_check
make print_assembly_check2
make test_task003
make test_task004
make test_task005
