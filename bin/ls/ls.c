# include <stdio.h>
# include <stdlib.h>
# include <utilities.h>
# include <idt.h>
# include <files.h>

int main(int argc, char *argv[])
{
    //replace these two values with the actual argc and argv parameter
    //char *cwd = "bin/folder1/";
    if( argc == 1 ){
        funcForLS(pwd());
     }else{
        char *dir = dirnameWithEndSlash(getAbsoluteFilePath(pwd(), argv[1]));
        funcForLS(dir);
    }
    return 0;
}
