#ifndef __ARCHIVE_HPP__
#define __ARCHIVE_HPP__

#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include "../common/FileMap.hpp"
#include "../common/HashSearch.hpp"
#include "../common/LexiconTypes.hpp"
#include "../common/MetaView.hpp"
#include "../common/ZMessageView.hpp"

#include "PackageAccess.hpp"
#include "ViewReference.hpp"

struct SearchResult
{
    uint32_t postid;
    float rank;
};

struct SearchData
{
    std::vector<SearchResult> results;
    std::vector<const char*> matched;
};

struct ScoreEntry;
class ExpandingBuffer;

class Archive
{
public:
    enum SearchFlags
    {
        SF_FlagsNone        = 0,
        SF_AdjacentWords    = 1 << 0,   // Calculate words adjacency
        SF_RequireAllWords  = 1 << 1,   // Require all words to be present
        SF_FuzzySearch      = 1 << 2,   // Also search for similar words
    };

    static Archive* Open( const std::string& fn );

    const char* GetMessage( uint32_t idx, ExpandingBuffer& eb );
    const char* GetMessage( const char* msgid, ExpandingBuffer& eb );
    size_t NumberOfMessages() const { return m_mcnt; }

    int GetMessageIndex( const char* msgid ) const;
    int GetMessageIndex( const char* msgid, XXH32_hash_t hash ) const;
    const char* GetMessageId( uint32_t idx ) const;

    ViewReference<uint32_t> GetTopLevel() const;
    size_t NumberOfTopLevel() const { return m_toplevel.DataSize(); }

    int32_t GetParent( uint32_t idx ) const;
    int32_t GetParent( const char* msgid ) const;

    ViewReference<uint32_t> GetChildren( uint32_t idx ) const;
    ViewReference<uint32_t> GetChildren( const char* msgid ) const;

    uint32_t GetTotalChildrenCount( uint32_t idx ) const;
    uint32_t GetTotalChildrenCount( const char* msgid ) const;

    uint32_t GetDate( uint32_t idx ) const;
    uint32_t GetDate( const char* msgid ) const;

    const char* GetFrom( uint32_t idx ) const;
    const char* GetFrom( const char* msgid ) const;

    const char* GetSubject( uint32_t idx ) const;
    const char* GetSubject( const char* msgid ) const;

    const char* GetRealName( uint32_t idx ) const;
    const char* GetRealName( const char* msgid ) const;

    int GetMessageScore( uint32_t idx, const std::vector<ScoreEntry>& scoreList ) const;

    SearchData Search( const char* query, int flags = SF_FlagsNone, int filter = T_All ) const;
    SearchData Search( const std::vector<std::string>& terms, int flags = SF_FlagsNone, int filter = T_All ) const;
    std::map<std::string, uint32_t> TimeChart() const;

    std::pair<const char*, uint64_t> GetShortDescription() const;
    std::pair<const char*, uint64_t> GetLongDescription() const;
    std::pair<const char*, uint64_t> GetArchiveName() const;

private:
    Archive( const std::string& dir );
    Archive( const PackageAccess* pkg );

    std::unique_ptr<const PackageAccess> m_pkg;

    ZMessageView m_mview;
    const size_t m_mcnt;
    const FileMap<uint32_t> m_toplevel;
    const HashSearch m_midhash;
    const MetaView<uint32_t, char> m_middb;
    const MetaView<uint32_t, uint32_t> m_connectivity;
    const MetaView<uint32_t, char> m_strings;
    const FileMap<LexiconMetaPacket> m_lexmeta;
    const FileMap<char> m_lexstr;
    const FileMap<LexiconDataPacket> m_lexdata;
    const FileMap<uint8_t> m_lexhit;
    const HashSearch m_lexhash;
    const FileMap<char> m_descShort;
    const FileMap<char> m_descLong;
    const FileMap<char> m_name;
    std::unique_ptr<MetaView<uint32_t, uint32_t>> m_lexdist;
};

#endif
