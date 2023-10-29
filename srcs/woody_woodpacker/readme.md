# 42 Woody-woodpacker

## Objective

Create a x86_64 ELF packer program in c which will obfuscate .text section of any binary passed as argument by encrypted it, thus making it undetectable by an antivirus.

## How does it work

ELF files are composed of three major components:
- ELF Header = contains general information about the binary (architecture type, OS type, header size, number of sections, ...)
    - can be seen using `readelf -h ./binary`
- Sections = all information needed for linking a target object file in order to build a working executable
    - can be seen using `readelf -S ./binary`
    - most common sections:
        - .text = Contains executable code with read write access
        - .rodata (read only data) = Initialized data, with read/write access rights
        - .data = Initialized data, with read access rights only
        - .bss = block starting symbol => includes all uninitialized objects (both variables and constants) declared at file scope (i.e., outside any function) as well as uninitialized static local variables (local variables declared with the static keyword)
        - .plt = Procedure Linkage Table =>  includes external procedure whose address isn't known in the time of linking, and is left to be resolved by the dynamic linker at run time, can contain addresses of Global functions, external imported functions
        - .got = global offset table => includes external data whose address isn't known in the time of linking, and is left to be resolved by the dynamic linker at run time, can contain addresses of Global variables
- Segments =  Program Headers, break down the structure of an ELF binary into suitable chunks to prepare the executable to be loaded into memory
    - can be seen using `readelf -l ./binary`

![Illustation of an ELF structure](../../resources/images/RMV0g.png?raw=true)


### Authorized functions

- open, close, exit
- fpusts, fflush, lseek
- mmap, munmap, mprotect
- perror, strerror
- syscall
- functions of the printf family
- function authorized within your libft

## Sources

https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
https://intezer.com/blog/research/executable-linkable-format-101-part1-sections-segments/
https://en.wikipedia.org/wiki/Data_segment

## To explore further

https://www.akkadia.org/drepper/dsohowto.pdf
https://reverseengineering.stackexchange.com/questions/1992/what-is-plt-got 
https://www.technovelty.org/linux/plt-and-got-the-key-to-code-sharing-and-dynamic-libraries.html