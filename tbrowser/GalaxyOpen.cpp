#include <algorithm>
#include <assert.h>
#include <chrono>
#include <ctype.h>
#include <sstream>
#include <vector>

#include "../libuat/Galaxy.hpp"
#include "../libuat/PersistentStorage.hpp"

#include "BottomBar.hpp"
#include "Browser.hpp"
#include "GalaxyOpen.hpp"
#include "UTF8.hpp"

GalaxyOpen::GalaxyOpen( Browser* parent, BottomBar& bar, Galaxy& galaxy, PersistentStorage& storage )
    : View( 0, 1, 0, -2 )
    , m_parent( parent )
    , m_bar( bar )
    , m_storage( storage )
    , m_galaxy( galaxy )
    , m_active( false )
    , m_top( 0 )
    , m_bottom( 0 )
    , m_cursor( 0 )
{
}

void GalaxyOpen::Entry()
{
    m_active = true;
    Draw();
    doupdate();

    while( auto key = GetKey() )
    {
        switch( key )
        {
        case KEY_RESIZE:
            m_parent->Resize();
            break;
        case 'q':
            m_active = false;
            return;
        case KEY_DOWN:
        case 'j':
            MoveCursor( 1 );
            doupdate();
            break;
        case KEY_UP:
        case 'k':
            MoveCursor( -1 );
            doupdate();
            break;
        case KEY_NPAGE:
            MoveCursor( m_bottom - m_top - 1 );
            doupdate();
            break;
        case KEY_PPAGE:
            MoveCursor( m_top - m_bottom + 1 );
            doupdate();
            break;
        case KEY_ENTER:
        case '\n':
        case 459:   // numpad enter

            break;
        default:
            break;
        }
        m_bar.Update();
    }
}

void GalaxyOpen::Resize()
{
    ResizeView( 0, 1, 0, -2 );
    if( !m_active ) return;
    Draw();
}

void GalaxyOpen::Draw()
{
    const int num = m_galaxy.GetNumberOfArchives();
    int w, h;
    getmaxyx( m_win, h, w );

    werase( m_win );
    mvwprintw( m_win, 1, 2, "Select archive to open: (%i/%i)", m_cursor+1, num );

    const int lenBase = w-2;

    wattron( m_win, A_BOLD );
    int line = m_top;
    for( int i=0; i<h-3; i++ )
    {
        if( line > num ) break;

        if( m_cursor == line )
        {
            wmove( m_win, 3+i, 0 );
            wattron( m_win, COLOR_PAIR( 2 ) );
            wprintw( m_win, "->" );
        }
        else
        {
            wmove( m_win, 3+i, 2 );
        }

        auto name = m_galaxy.GetArchiveName( line );
        auto desc = m_galaxy.GetArchiveDescription( line );

        int len = lenBase;
        auto end = utfendl( name, len );
        wattroff( m_win, COLOR_PAIR( 2 ) );
        wprintw( m_win, "%.*s", end - name, name );

        len = lenBase - len - 2;
        if( len > 0 )
        {
            end = utfendcrlfl( desc, len );
            wattron( m_win, COLOR_PAIR( 2 ) );
            mvwprintw( m_win, 3+i, w-len-1, "%.*s", end - desc, desc );
        }
        if( m_cursor == line )
        {
            wattron( m_win, COLOR_PAIR( 2 ) );
            waddch( m_win, '<' );
        }
        line++;
    }
    wattroff( m_win, COLOR_PAIR( 2 ) | A_BOLD );

    m_bottom = line;

    wnoutrefresh( m_win );
}

void GalaxyOpen::MoveCursor( int offset )
{
    while( offset < 0 )
    {
        if( m_cursor == 0 ) break;
        m_cursor--;
        if( m_cursor < m_top )
        {
            m_top--;
            m_bottom--;
        }
        offset++;
    }
    while( offset > 0 )
    {
        if( m_cursor == m_galaxy.GetNumberOfArchives() - 1 ) break;
        m_cursor++;
        if( m_cursor >= m_bottom )
        {
            m_top++;
            m_bottom++;
        }
        offset--;
    }
    Draw();
}