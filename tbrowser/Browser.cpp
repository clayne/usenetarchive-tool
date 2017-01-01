#include <curses.h>

#include "../libuat/Archive.hpp"
#include "../libuat/PersistentStorage.hpp"

#include "Browser.hpp"
#include "Help.hpp"

Browser::Browser( std::unique_ptr<Archive>&& archive, PersistentStorage& storage, const std::string& fn )
    : m_archive( std::move( archive ) )
    , m_storage( storage )
    , m_header( m_archive->GetArchiveName(), m_archive->GetShortDescription().second > 0 ? m_archive->GetShortDescription().first : nullptr, fn.c_str() )
    , m_bottom( this )
    , m_mview( *m_archive, m_storage )
    , m_tview( *m_archive, m_storage, m_mview )
    , m_sview( this, m_bottom, *m_archive, m_storage )
    , m_textview( this )
    , m_fn( fn )
{
    auto& history = m_storage.GetArticleHistory();
    if( m_storage.ReadArticleHistory( m_fn.c_str() ) )
    {
        SwitchToMessage( history.back() );
    }
    m_historyIdx = history.size() - 1;

    m_bottom.Status( "Press '?' for help.", 1 );

    doupdate();
}

bool Browser::MoveOrEnterAction( int move )
{
    auto resizeNeeded = !m_mview.IsActive();
    bool newMessage = m_mview.DisplayedMessage() != m_tview.GetCursor();
    bool ret = m_mview.Display( m_tview.GetCursor(), move );
    if( resizeNeeded )
    {
        m_tview.Resize();
        m_mview.Resize();
    }
    auto cursor = m_tview.GetCursor();
    if( newMessage )
    {
        if( m_tview.CanExpand( cursor ) &&
            !m_tview.IsExpanded( cursor ) &&
            m_archive->GetParent( m_tview.GetCursor() ) == -1 )
        {
            const auto depth = m_tview.Expand( cursor, true );
            if( depth > CondensedDepthThreshold )
            {
                m_tview.MarkTreeCondensed( cursor, depth );
            }
        }
        m_tview.Draw();
        m_tview.FocusOn( cursor );

        auto& history = m_storage.GetArticleHistory();
        auto idx = m_tview.GetCursor();
        if( history.empty() || history.back() != idx )
        {
            m_storage.AddToHistory( idx );
            m_historyIdx = history.size()-1;
        }
    }
    return ret;
}

void Browser::Entry()
{
    while( auto key = m_tview.GetKey() )
    {
        switch( key )
        {
        case 12:    // ^L
            endwin();
            initscr();
            // fallthrough
        case KEY_RESIZE:
            Resize();
            break;
        case 'e':
        {
            if( m_mview.IsActive() )
            {
                m_mview.Close();
                m_tview.Resize();
            }
            auto idx = m_tview.GetRoot( m_tview.GetCursor() );
            m_tview.SetCursor( idx );
            m_tview.Collapse( idx );
            m_tview.FocusOn( idx );
            doupdate();
            break;
        }
        case 'q':
            if( !m_mview.IsActive() )
            {
                const auto key = m_bottom.KeyQuery( "Are you sure you want to quit? [y/n]" );
                if( key == 'y' || key == 'Y' ) return;
            }
            else
            {
                m_mview.Close();
                m_tview.Resize();
                doupdate();
            }
            break;
        case KEY_ENTER:
        case '\n':
        case 459:   // numpad enter
            MoveOrEnterAction( 1 );
            doupdate();
            break;
        case KEY_BACKSPACE:
        case '\b':
        case 127:
            MoveOrEnterAction( -1 );
            doupdate();
            break;
        case ' ':
            if( MoveOrEnterAction( m_mview.GetHeight() - 2 ) )
            {
                m_tview.MoveCursor( 1 );
                MoveOrEnterAction( 0 );
            }
            doupdate();
            break;
        case KEY_DC:
        case 'b':
            MoveOrEnterAction( -m_mview.GetHeight() - 2 );
            doupdate();
            break;
        case 'd':
            m_storage.MarkVisited( m_archive->GetMessageId( m_tview.GetCursor() ) );
            m_tview.GoNextUnread();
            doupdate();
            break;
        case KEY_UP:
        case 'k':
            m_tview.MoveCursor( -1 );
            doupdate();
            break;
        case KEY_DOWN:
        case 'j':
            m_tview.MoveCursor( 1 );
            doupdate();
            break;
        case KEY_PPAGE:
            m_tview.PageBackward();
            m_tview.Draw();
            doupdate();
            break;
        case KEY_NPAGE:
            m_tview.PageForward();
            m_tview.Draw();
            doupdate();
            break;
        case KEY_HOME:
            m_tview.MoveCursorTop();
            m_tview.Draw();
            doupdate();
            break;
        case KEY_END:
            m_tview.MoveCursorBottom();
            m_tview.Draw();
            doupdate();
            break;
        case 'x':
        {
            auto cursor = m_tview.GetCursor();
            if( m_tview.IsExpanded( cursor ) )
            {
                m_tview.Collapse( cursor );
            }
            else
            {
                const auto depth = m_tview.Expand( cursor, m_archive->GetParent( cursor ) == -1 );
                if( depth > CondensedDepthThreshold )
                {
                    m_tview.MarkTreeCondensed( cursor, depth );
                }
            }
            m_tview.Draw();
            doupdate();
            break;
        }
        case 'p':
        {
            auto parent = m_archive->GetParent( m_tview.GetCursor() );
            if( parent != -1 )
            {
                m_tview.SetCursor( parent );
                m_tview.FocusOn( parent );
                doupdate();
            }
            break;
        }
        case KEY_LEFT:
        case 'h':
        {
            auto cursor = m_tview.GetCursor();
            if( m_tview.CanExpand( cursor ) && m_tview.IsExpanded( cursor ) )
            {
                m_tview.Collapse( cursor );
                m_tview.Draw();
                doupdate();
            }
            else
            {
                auto parent = m_archive->GetParent( cursor );
                if( parent != -1 )
                {
                    m_tview.SetCursor( parent );
                    m_tview.FocusOn( parent );
                    doupdate();
                }
            }
            break;
        }
        case KEY_RIGHT:
        case 'l':
        {
            auto cursor = m_tview.GetCursor();
            if( m_tview.CanExpand( cursor ) && !m_tview.IsExpanded( cursor ) )
            {
                const auto depth = m_tview.Expand( cursor, m_archive->GetParent( cursor ) == -1 );
                if( depth > CondensedDepthThreshold )
                {
                    m_tview.MarkTreeCondensed( cursor, depth );
                }
                m_tview.Draw();
                doupdate();
            }
            else
            {
                m_tview.MoveCursor( 1 );
                doupdate();
            }
            break;
        }
        case 't':
            m_mview.SwitchHeaders();
            if( m_mview.IsActive() )
            {
                doupdate();
            }
            break;
        case 'r':
            m_mview.SwitchROT13();
            if( m_mview.IsActive() )
            {
                doupdate();
            }
            break;
        case ',':
        {
            auto& history = m_storage.GetArticleHistory();
            if( !history.empty() && m_historyIdx > 0 )
            {
                m_historyIdx--;
                SwitchToMessage( history[m_historyIdx] );
                doupdate();
            }
            break;
        }
        case '.':
        {
            auto& history = m_storage.GetArticleHistory();
            if( !history.empty() && m_historyIdx < history.size() - 1 )
            {
                m_historyIdx++;
                SwitchToMessage( history[m_historyIdx] );
                doupdate();
            }
            break;
        }
        case 'g':
        {
            auto msgid = m_bottom.Query( "MsgID: " );
            if( !msgid.empty() )
            {
                auto idx = m_archive->GetMessageIndex( msgid.c_str() );
                if( idx >= 0 )
                {
                    OpenMessage( idx );
                }
                else
                {
                    m_bottom.Status( "No such message." );
                }
                doupdate();
            }
            else
            {
                m_bottom.Update();
                doupdate();
            }
            break;
        }
        case 's':
            m_bottom.SetHelp( HelpSet::Search );
            m_sview.Entry();
            m_bottom.SetHelp( HelpSet::Default );
            RestoreDefaultView();
            doupdate();
            break;
        case '?':
            m_bottom.SetHelp( HelpSet::Text );
            m_textview.Entry( HelpContents );
            m_bottom.SetHelp( HelpSet::Default );
            RestoreDefaultView();
            doupdate();
            break;
        case 'c':
        {
            auto charter = m_archive->GetLongDescription();
            if( charter.second == 0 )
            {
                m_bottom.Status( "Group charter is not available." );
            }
            else
            {
                m_bottom.SetHelp( HelpSet::Text );
                m_textview.Entry( charter.first, charter.second );
                m_bottom.SetHelp( HelpSet::Default );
                RestoreDefaultView();
            }
            doupdate();
            break;
        }
        case 'o':
        {
            auto fn = m_bottom.Query( "Open archive: ", m_fn.c_str(), true );
            if( !fn.empty() )
            {
                if( !Exists( fn ) )
                {
                    m_bottom.Status( "No such archive!" );
                }
                else
                {
                    OpenArchive( std::move( fn ) );
                }
            }
            break;
        }
        default:
            break;
        }

        m_bottom.Update();
    }
}

void Browser::Resize()
{
    resize_term( 0, 0 );
    m_header.Resize();
    m_mview.Resize();
    m_tview.Resize();
    m_sview.Resize();
    m_textview.Resize();
    m_bottom.Resize();
    doupdate();
}

void Browser::OpenMessage( int msgidx )
{
    SwitchToMessage( msgidx );
    MoveOrEnterAction( 0 );
}

void Browser::SwitchToMessage( int msgidx )
{
    auto root = m_tview.GetRoot( msgidx );
    if( msgidx != root && m_tview.CanExpand( root ) && !m_tview.IsExpanded( root ) )
    {
        const auto depth = m_tview.Expand( root, true );
        if( depth > CondensedDepthThreshold )
        {
            m_tview.MarkTreeCondensed( root, depth );
        }
        m_tview.Draw();
    }
    m_tview.SetCursor( msgidx );
    m_tview.FocusOn( msgidx );
}

void Browser::RestoreDefaultView()
{
    m_tview.Draw();
    if( m_mview.IsActive() )
    {
        m_mview.Draw();
    }
}

void Browser::OpenArchive( std::string&& fn )
{
    std::unique_ptr<Archive> archive( Archive::Open( fn ) );
    if( !archive )
    {
        m_bottom.Status( "Cannot open archive!" );
        return;
    }

    m_storage.WriteArticleHistory( m_fn.c_str() );

    std::swap( fn, m_fn );
    std::swap( archive, m_archive );

    m_header.Change( m_archive->GetArchiveName(), m_archive->GetShortDescription().second > 0 ? m_archive->GetShortDescription().first : nullptr, m_fn.c_str() );
    m_mview.Reset( *m_archive );
    m_sview.Reset( *m_archive );
    m_tview.Reset( *m_archive );

    if( m_storage.ReadArticleHistory( m_fn.c_str() ) )
    {
        SwitchToMessage( m_storage.GetArticleHistory().back() );
    }

    m_tview.Resize();

    m_bottom.Status( "Archive opened." );

    doupdate();
}
