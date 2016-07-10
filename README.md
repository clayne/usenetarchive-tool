# Usenet Archive Toolkit

Usenet Archive Toolkit project aims to provide a set of tools to process various sources of usenet messages into a coherent, searchable archive.

### Import Formats

Usenet messages may be retrieved from a number of different sources. Currently we support:

- import-source-slrnpull --- Import from a directory where each file is a separate message (slrnpull was chosen because of extra-simple setup required to get it working).
- import-source-slrnpull-7z --- Import from a slrnpull directory compressed into a single 7z compressed file.
- import-source-mbox --- [Archive.org](https://archive.org/details/usenet) keeps its collection of usenet messages in a mbox format, in which all posts are merged into a single file.

Imported messages are stored in a per-message LZ4 compressed meta+payload database.

### Data Processing

Raw imported messages have to be processed to be of any use. We provide the following utilities:

- kill-duplicates --- Removes duplicate messages. It is relatively rare, but data sets from even a single NNTP server may contain the same message twice.
- extract-msgid --- Extracts unique identifier of each message and builds reference table for fast access to any message through its ID.
- merge-raw --- Merges two imported data sets into one. Does not duplicate messages.
- utf8ize --- Converts messages to a common character encoding, UTF-8.
- repack-zstd --- Builds a common dictionary for all messages and recompresses them to a zstd meta+payload+dict database.

### Data Access

- query-raw --- Implements queries on LZ4 database. Supports:
    * Message count.
    * Listing of message identifiers.
    * Query message by identifier.
    * Query message by database record number.

### License

    Usenet Archive
    Copyright (C) 2016  Bartosz Taudul <wolf.pld@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.