# include <stdio.h>
# include <idt.h>
# include <files.h>

int main(  )
{
    //replace these two values with the actual argc and argv parameter
    int argc1 = 2;
    //char *cwd = "bin/folder1/";
    char *normalDir = "bin/"; //TODO: Take care of "/"
    if( argc1 == 1 ){
       // funcForLS( cwd );
    }else{
        funcForLS( normalDir );
    }
    return 0;
}
