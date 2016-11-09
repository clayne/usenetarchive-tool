#ifndef __THREADVIEW_HPP__
#define __THREADVIEW_HPP__

#include <stdint.h>
#include <vector>

#include "View.hpp"

class Archive;
class BottomBar;

struct ThreadData
{
    unsigned int expanded   : 1;
    unsigned int valid      : 1;
    unsigned int msgid      : 30;
    int parent;
};

static_assert( sizeof( ThreadData ) == sizeof( uint32_t ) * 2, "Wrong size of ThreadData" );

class ThreadView : public View
{
public:
    ThreadView( const Archive& archive, BottomBar& bottomBar );
    ~ThreadView();

    void Resize();
    void Draw();

    void Up();
    void Down();
    void Expand( int cursor, bool recursive );
    void Collapse( int cursor );
    bool IsExpanded( int cursor ) const { return m_data[cursor].expanded; }

    int GetCursor() const { return m_cursor; }
    int GetMessageIndex() const { return m_data[m_cursor].msgid; }

private:
    void Fill( int index, int msgid, int parent );
    void DrawLine( int idx );
    void MoveCursor( int offset );

    const Archive& m_archive;
    BottomBar& m_bottomBar;
    std::vector<ThreadData> m_data;
    int m_top;
    int m_cursor;
};

#endif
