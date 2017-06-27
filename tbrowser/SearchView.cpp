#include <algorithm>
#include <assert.h>
#include <chrono>
#include <ctype.h>
#include <sstream>
#include <vector>

#include "../libuat/Archive.hpp"
#include "../libuat/PersistentStorage.hpp"
#include "../libuat/SearchEngine.hpp"
#include "../common/ICU.hpp"
#include "../common/MessageLogic.hpp"

#include "BottomBar.hpp"
#include "Browser.hpp"
#include "LevelColors.hpp"
#include "SearchView.hpp"
#include "UTF8.hpp"

SearchView::SearchView( Browser* parent, BottomBar& bar, Archive& archive, PersistentStorage& storage )
    : View( 0, 1, 0, -2 )
    , m_parent( parent )
    , m_bar( bar )
    , m_archive( &archive )
    , m_search( std::make_unique<SearchEngine>( archive ) )
    , m_storage( storage )
    , m_active( false )
    , m_top( 0 )
    , m_bottom( 0 )
    , m_cursor( 0 )
{
}

void SearchView::Entry()
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
        case KEY_EXIT:
        case 27:
        case 'q':
            m_active = false;
            return;
        case 's':
        case '/':
        {
            auto query = m_bar.Query( "Search: ", m_query.c_str() );
            if( !query.empty() )
            {
                std::swap( m_query, query );
                auto start = std::chrono::high_resolution_clock::now();
                m_result = m_search->Search( m_query.c_str(), SearchEngine::SF_AdjacentWords | SearchEngine::SF_FuzzySearch );
                m_queryTime = std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - start ).count() / 1000.f;
                m_preview.clear();
                m_preview.reserve( m_result.results.size() );
                m_top = m_bottom = m_cursor = 0;
            }
            else
            {
                m_bar.Update();
            }
            Draw();
            doupdate();
            break;
        }
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
            MoveCursor( m_bottom - m_top );
            doupdate();
            break;
        case KEY_PPAGE:
            MoveCursor( m_top - m_bottom );
            doupdate();
            break;
        case KEY_ENTER:
        case '\n':
        case 459:   // numpad enter
            if( !m_result.results.empty() )
            {
                m_parent->OpenMessage( m_result.results[m_cursor].postid );
                m_active = false;
                return;
            }
            break;
        case 'a':
            std::sort( m_result.results.begin(), m_result.results.end(), [this]( const auto& l, const auto& r ) { return m_archive->GetDate( l.postid ) < m_archive->GetDate( r.postid ); } );
            m_top = m_bottom = m_cursor = 0;
            m_preview.clear();
            Draw();
            doupdate();
            break;
        case 'd':
            std::sort( m_result.results.begin(), m_result.results.end(), [this]( const auto& l, const auto& r ) { return m_archive->GetDate( l.postid ) > m_archive->GetDate( r.postid ); } );
            m_top = m_bottom = m_cursor = 0;
            m_preview.clear();
            Draw();
            doupdate();
            break;
        case 'r':
            std::sort( m_result.results.begin(), m_result.results.end(), []( const auto& l, const auto& r ) { return l.rank > r.rank; } );
            m_top = m_bottom = m_cursor = 0;
            m_preview.clear();
            Draw();
            doupdate();
            break;
        default:
            break;
        }
        m_bar.Update();
    }
}

void SearchView::Resize()
{
    ResizeView( 0, 1, 0, -2 );
    if( !m_active ) return;
    Draw();
}

void SearchView::Draw()
{
    werase( m_win );
    if( m_result.results.empty() )
    {
        if( m_query.empty() )
        {
            mvwprintw( m_win, 2, 4, "Nothing to show." );
        }
        else
        {
            mvwprintw( m_win, 2, 4, "No results for: %s", m_query.c_str() );
        }
        mvwprintw( m_win, 4, 4, "Press '/' to enter search query." );

        wattron( m_win, COLOR_PAIR( 8 ) | A_BOLD );
        mvwprintw( m_win, 6, 4, "Hint: quote words to disable fuzzy search." );
        wattroff( m_win, COLOR_PAIR( 8 ) | A_BOLD );
    }
    else
    {
        wattron( m_win, COLOR_PAIR( 2 ) | A_BOLD );
        wprintw( m_win, " %i", m_result.results.size() );
        wattroff( m_win, COLOR_PAIR( 2 ) | A_BOLD );
        wprintw( m_win, " results for query: " );
        wattron( m_win, A_BOLD );
        wprintw( m_win, "%s", m_query.c_str() );
        wattron( m_win, COLOR_PAIR( 8 ) );
        wprintw( m_win, " (%.3f ms elapsed)", m_queryTime );
        wattroff( m_win, COLOR_PAIR( 8 ) | A_BOLD );

        const int h = getmaxy( m_win ) - 1;
        const int w = getmaxx( m_win );
        int cnt = m_top;
        int line = 0;
        while( line < h && cnt < m_result.results.size() )
        {
            const auto& res = m_result.results[cnt];
            wmove( m_win, line + 1, 0 );
            wattron( m_win, COLOR_PAIR(1) );
            wprintw( m_win, "%5i ", cnt+1 );
            wattron( m_win, COLOR_PAIR(15) );
            wprintw( m_win, "(%5.1f%%) ", res.rank * 100.f );

            if( m_cursor == cnt )
            {
                wmove( m_win, line + 1, 0 );
                wattron( m_win, COLOR_PAIR(14) | A_BOLD );
                wprintw( m_win, "->" );
                wattroff( m_win, COLOR_PAIR(14) | A_BOLD );
                wmove( m_win, line + 1, 15 );
            }

            wattron( m_win, COLOR_PAIR(1) );
            if( m_storage.WasVisited( m_archive->GetMessageId( res.postid ) ) )
            {
                waddch( m_win, 'R' );
            }
            else
            {
                waddch( m_win, '-' );
            }

            wprintw( m_win, " [" );
            auto realname = m_archive->GetRealName( res.postid );
            wattron( m_win, COLOR_PAIR(13) | A_BOLD );
            int len = 16;
            auto end = utfendl( realname, len );
            utfprint( m_win, realname, end );
            while( len++ < 16 )
            {
                waddch( m_win, ' ' );
            }
            wattroff( m_win, COLOR_PAIR(3) | A_BOLD );
            wattron( m_win, COLOR_PAIR(1) );
            wprintw( m_win, "] " );

            time_t date = m_archive->GetDate( res.postid );
            auto lt = localtime( &date );
            char buf[64];
            auto dlen = strftime( buf, 64, "%F %R", lt );

            auto subject = m_archive->GetSubject( res.postid );
            len = w - 38 - dlen;
            end = utfendl( subject, len );
            wattron( m_win, A_BOLD );
            utfprint( m_win, subject, end );
            wattroff( m_win, A_BOLD );

            while( len++ < w - 38 - dlen )
            {
                waddch( m_win, ' ' );
            }

            wmove( m_win, line+1, w-dlen-2 );
            waddch( m_win, '[' );
            wattron( m_win, COLOR_PAIR(14) | A_BOLD );
            wprintw( m_win, "%s", buf );
            wattroff( m_win, COLOR_PAIR(14) | A_BOLD );
            wattron( m_win, COLOR_PAIR(1) );
            waddch( m_win, m_cursor == cnt ? '<' : ']' );
            wattroff( m_win, COLOR_PAIR(1) );
            line++;

            int frameCol = COLOR_PAIR(6);
            assert( cnt <= m_preview.size() );
            if( m_preview.size() == cnt )
            {
                FillPreview( cnt );
            }

            auto preview = m_preview[cnt];
            auto it = preview.begin();
            while( it != preview.end() )
            {
                if( line == h ) break;

                wmove( m_win, line+1, 0 );
                wattron( m_win, frameCol );
                waddch( m_win, ACS_VLINE );
                wattroff( m_win, frameCol );
                int len = w - 3;

                bool newline;
                do
                {
                    int l = len;
                    auto end = utfendl( it->text.c_str(), l );
                    len -= l;

                    wattron( m_win, it->color );
                    utfprint( m_win, it->text.c_str(), end );
                    wattroff( m_win, it->color );

                    newline = it->newline;
                    ++it;
                }
                while( !newline );

                wmove( m_win, line+1, w-1 );
                wattron( m_win, frameCol );
                waddch( m_win, ACS_VLINE );
                wattroff( m_win, frameCol );
                line++;
            }
            if( line < h )
            {
                wattron( m_win, frameCol );
                waddch( m_win, ACS_LLCORNER );
                for( int i=0; i<w-2; i++ )
                {
                    waddch( m_win, ACS_HLINE );
                }
                waddch( m_win, ACS_LRCORNER );
                wattroff( m_win, frameCol );
                line++;
            }

            cnt++;
        }
        m_bottom = cnt;
    }
    wnoutrefresh( m_win );
}

void SearchView::Reset( Archive& archive )
{
    m_archive = &archive;
    m_search = std::make_unique<SearchEngine>( archive );
    m_result.results.clear();
    m_query.clear();
    m_top = m_bottom = m_cursor = 0;
}

void SearchView::FillPreview( int idx )
{
    static const char terminator[] = { '\0' };

    const auto& res = m_result.results[idx];
    const auto& words = m_result.matched;

    auto msg = std::string( m_archive->GetMessage( res.postid, m_eb ) );
    auto content = msg.c_str();
    // skip headers
    for(;;)
    {
        if( *content == '\n' ) break;
        while( *content++ != '\n' ) {}
    }
    content++;

    m_preview.emplace_back();
    auto& preview = m_preview.back();

    std::vector<std::string> wordbuf;
    for( int i=0; i<res.hitnum; i++ )
    {
        const auto htype = LexiconDecodeType( res.hits[i] );
        if( htype == T_Signature || htype == T_Header ) continue;
        const auto hpos = LexiconHitPos( res.hits[i] );
        uint8_t max = LexiconHitPosMask[htype];
        if( hpos == max ) continue;

        int basePos = 0;
        auto line = content;
        for(;;)
        {
            auto end = line;
            while( *end != '\n' && *end != '\0' ) end++;
            if( end - line == 4 && strncmp( line, "-- ", 3 ) == 0 ) break;  // start of signature
            auto ptr = line;
            const auto quotLevel = QuotationLevel( ptr, end );
            if( LexiconTypeFromQuotLevel( quotLevel ) == htype )
            {
                SplitLine( ptr, end, wordbuf, false );
                if( basePos + wordbuf.size() > hpos )
                {
                    const auto& word = wordbuf[hpos - basePos];
                    auto wptr = ptr;
                    while( strncmp( wptr, word.c_str(), word.size() ) != 0 ) wptr++;

                    if( !preview.empty() )
                    {
                        preview.emplace_back( PreviewData { std::string( " -*-" ), 0, true  } );
                    }

                    if( wptr != line )
                    {
                        preview.emplace_back( PreviewData { std::string( line, wptr ), QuoteFlags[htype-2], false } );
                    }
                    preview.emplace_back( PreviewData { word, COLOR_PAIR( 16 ) | A_BOLD, false } );
                    preview.emplace_back( PreviewData { std::string( wptr+word.size(), end ), QuoteFlags[htype-2], true } );

                    for( int i=0; i<2; i++ )
                    {
                        if( *end == '\0' ) break;
                        line = end + 1;
                        end = line;
                        while( *end != '\n' && *end != '\0' ) end++;
                        if( end == line ) break;
                        if( end - line == 4 && strncmp( line, "-- ", 3 ) == 0 ) break;
                        auto ptr = line;
                        const auto quotLevel = QuotationLevel( ptr, end );
                        preview.emplace_back( PreviewData { std::string( line, end ), QuoteFlags[LexiconTypeFromQuotLevel( quotLevel )-2], true } );
                    }
                    break;
                }
                else
                {
                    basePos += wordbuf.size();
                }
            }
            if( *end == '\0' ) break;
            line = end + 1;
        }
    }
}

void SearchView::MoveCursor( int offset )
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
        if( m_cursor == m_result.results.size() - 1 ) break;
        m_cursor++;
        if( m_cursor >= m_bottom - 1 )
        {
            m_top++;
            m_bottom++;
        }
        offset--;
    }
    Draw();
}
