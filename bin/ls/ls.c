# include <stdio.h>
# include <idt.h>
# include <stdlib.h>
# include <files.h>

int main(int argc, char *argv[])
{

    //replace these two values with the actual argc and argv parameter
    //char *cwd = "bin/folder1/";
    if( argc == 1 ){
        funcForLS(pwd());
    }else{
        funcForLS(argv[1]);
    }
    return 0;
}
