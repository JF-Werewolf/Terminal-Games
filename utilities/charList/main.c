#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <string.h>
CONSOLE_FONT_INFOEX cfi;



void printChars(){
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    for(int x=0; x<256; x++){
        printf("%d: %c \n", x, x);
    }
}


int main()
{
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = 20;
    cfi.dwFontSize.Y = cfi.dwFontSize.X*1.04;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    printChars();

    char c;

    for(;;){
        c = getch();
        printf("%d : %c \n", c, c);
    }

    return 0;
}
