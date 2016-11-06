#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "../libuat/Archive.hpp"

#include "Browser.hpp"

int main( int argc, char** argv )
{
    if( argc != 2 )
    {
        fprintf( stderr, "Usage: %s archive-name.usenet\n", argv[0] );
        return 1;
    }

    std::unique_ptr<Archive> archive( Archive::Open( argv[1] ) );
    if( !archive )
    {
        fprintf( stderr, "%s is not an archive!\n", argv[1] );
        return 1;
    }

    initscr();
    if( !has_colors() )
    {
        endwin();
        fprintf( stderr, "Terminal doesn't support colors.\n" );
        exit( 1 );
    }

    start_color();
    cbreak();
    noecho();
    keypad( stdscr, TRUE );

    init_pair( 1, COLOR_WHITE, COLOR_BLUE );

    Browser browser( std::move( archive ) );
    browser.Entry();

    endwin();

    return 0;
}
