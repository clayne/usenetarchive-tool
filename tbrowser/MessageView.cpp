#include <algorithm>

#include "../common/String.hpp"
#include "../common/MessageLogic.hpp"
#include "../libuat/Archive.hpp"

#include "MessageView.hpp"
#include "UTF8.hpp"

MessageView::MessageView( Archive& archive )
    : View( 0, 0, 1, 1 )
    , m_archive( archive )
    , m_idx( -1 )
    , m_active( false )
{
}

MessageView::~MessageView()
{
}

void MessageView::Resize()
{
    if( !m_active ) return;
    int sw = getmaxx( stdscr );
    m_vertical = sw > 160;
    if( m_vertical )
    {
        ResizeView( sw / 2, 1, (sw+1) / 2, -2 );
    }
    else
    {
        int sh = getmaxy( stdscr ) - 2;
        ResizeView( 0, 1 + sh * 20 / 100, 0, sh - ( sh * 20 / 100 ) );
    }
    Draw();
}

void MessageView::Display( uint32_t idx )
{
    if( idx != m_idx )
    {
        m_idx = idx;
        m_text = m_archive.GetMessage( idx );
        PrepareLines();
    }
    // If view is not active, drawing will be performed during resize.
    if( m_active )
    {
        Draw();
    }
    m_active = true;
}

void MessageView::Close()
{
    m_active = false;
}

void MessageView::Draw()
{
    int h = getmaxy( m_win );
    werase( m_win );
    int limit = std::min<int>( h, m_lines.size() );
    for( int i=0; i<limit; i++ )
    {
        auto start = m_text + m_lines[i].offset;
        auto end = utfendcrlf( start, 79 );
        if( m_lines[i].flags == L_Header )
        {
            auto hend = start;
            while( *hend != ' ' ) hend++;
            wattron( m_win, COLOR_PAIR( 2 ) | A_BOLD );
            wprintw( m_win, "%.*s", hend - start, start );
            wattroff( m_win, COLOR_PAIR( 2 ) );
            wattron( m_win, COLOR_PAIR( 7 ) );
            wprintw( m_win, "%.*s\n", end - hend, hend );
            wattroff( m_win, COLOR_PAIR( 7 ) | A_BOLD );
        }
        else
        {
            switch( m_lines[i].flags )
            {
            case L_Signature:
                wattron( m_win, COLOR_PAIR( 8 ) | A_BOLD );
                break;
            case L_Quote1:
                wattron( m_win, COLOR_PAIR( 5 ) );
                break;
            case L_Quote2:
                wattron( m_win, COLOR_PAIR( 3 ) );
                break;
            case L_Quote3:
                wattron( m_win, COLOR_PAIR( 6 ) | A_BOLD );
                break;
            case L_Quote4:
                wattron( m_win, COLOR_PAIR( 2 ) );
                break;
            default:
                break;
            }
            wprintw( m_win, "%.*s\n", end - start, start );
            switch( m_lines[i].flags )
            {
            case L_Signature:
                wattroff( m_win, COLOR_PAIR( 8 ) | A_BOLD );
                break;
            case L_Quote1:
                wattroff( m_win, COLOR_PAIR( 5 ) );
                break;
            case L_Quote2:
                wattroff( m_win, COLOR_PAIR( 3 ) );
                break;
            case L_Quote3:
                wattroff( m_win, COLOR_PAIR( 6 ) | A_BOLD );
                break;
            case L_Quote4:
                wattroff( m_win, COLOR_PAIR( 2 ) );
                break;
            default:
                break;
            }
        }
    }
    wattron( m_win, COLOR_PAIR( 6 ) );
    for( int i=limit; i<h; i++ )
    {
        wprintw( m_win, "~\n" );
    }
    wattroff( m_win, COLOR_PAIR( 6 ) );
    wnoutrefresh( m_win );
}

void MessageView::PrepareLines()
{
    m_lines.clear();
    auto txt = m_text;
    bool headers = true;
    bool sig = false;
    for(;;)
    {
        auto end = txt;
        while( *end != '\n' && *end != '\0' ) end++;
        if( headers )
        {
            if( end-txt == 0 )
            {
                m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Quote0 } );
                headers = false;
                while( *end == '\n' ) end++;
                end--;
            }
            else
            {
                if( strnicmpl( txt, "from: ", 6 ) == 0 ||
                    strnicmpl( txt, "newsgroups: ", 12 ) == 0 ||
                    strnicmpl( txt, "subject: ", 9 ) == 0 ||
                    strnicmpl( txt, "date: ", 6 ) == 0 )
                {
                    m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Header } );
                }
            }
        }
        else
        {
            if( strncmp( "-- \n", txt, 4 ) == 0 )
            {
                sig = true;
            }
            if( sig )
            {
                m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Signature } );
            }
            else
            {
                auto test = txt;
                int level = QuotationLevel( test, end );
                switch( level )
                {
                case 0:
                    m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Quote0 } );
                    break;
                case 1:
                    m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Quote1 } );
                    break;
                case 2:
                    m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Quote2 } );
                    break;
                case 3:
                    m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Quote3 } );
                    break;
                default:
                    m_lines.emplace_back( Line { uint32_t( txt - m_text ), L_Quote4 } );
                    break;
                }
            }
        }
        if( *end == '\0' ) break;
        txt = end + 1;
    }
}
