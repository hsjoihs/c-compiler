#!/bin/bash
cd template/; make; cd ../
make intmap_check
make test_task001
make test_task002
make lexer_check
make print_assembly_check
make print_assembly_check2
make print_assembly_check3
make print_assembly_check4
make print_assembly_check5
make test_task003
make test_task004
make test_task005
make test_task006
make test_task007
make test_task008
