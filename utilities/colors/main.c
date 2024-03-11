#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void textcolor (int color)                                      /// -|
{                                                               /// -|
    static int __BACKGROUND;                                    /// -|
                                                                /// -|
    HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );              /// -|
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;                        /// -|
                                                                /// -2-
                                                                /// -|
    GetConsoleScreenBufferInfo(h, &csbiInfo);                   /// -|
                                                                /// -|
    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE),  /// -|
                             color + (__BACKGROUND << 4));      /// -|
}

int main()
{
    for(int x=0; x<256; x++){
        textcolor(x);
        printf("%d: Hello world!\n", x);

    }
    return 0;
}
