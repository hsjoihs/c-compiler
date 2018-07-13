#!/bin/bash
cd template/; make; cd ../
make intmap_check
make full_compile001
make full_compile002
make full_compile003
make lexer_check
make lexer_check2
make lexer_check3
make print_assembly_check009
make full_compile003
make full_compile004
make full_compile005
make full_compile006
make full_compile007
make full_compile008
