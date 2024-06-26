project('UsenetArchive', 'c', 'cpp', default_options: ['cpp_std=c++17'])

compiler = meson.get_compiler('cpp')
compile_args = compiler.get_supported_arguments('-march=native')
add_project_arguments(compile_args, language: ['c', 'cpp'])


common_src = [
    'common/Filesystem.cpp',
    'common/ICU.cpp',
    'common/KillRe.cpp',
    'common/LexiconTypes.cpp',
    'common/MessageLines.cpp',
    'common/MessageLogic.cpp',
    'common/mmap.cpp',
    'common/ParseDate.cpp',
    'common/StringCompress.cpp',
    'common/System.cpp',
    'common/TaskDispatch.cpp',
    'common/UTF8.cpp',
    'contrib/xxhash/xxhash.c',
]

libuat_src = [
    'libuat/Archive.cpp',
    'libuat/Galaxy.cpp',
    'libuat/PackageAccess.cpp',
    'libuat/PersistentStorage.cpp',
    'libuat/SearchEngine.cpp',
]

zstd_src = [
    'contrib/zstd/common/debug.c',
    'contrib/zstd/common/entropy_common.c',
    'contrib/zstd/common/error_private.c',
    'contrib/zstd/common/fse_decompress.c',
    'contrib/zstd/common/pool.c',
    'contrib/zstd/common/threading.c',
    'contrib/zstd/common/xxhash.c',
    'contrib/zstd/common/zstd_common.c',
    'contrib/zstd/compress/fse_compress.c',
    'contrib/zstd/compress/hist.c',
    'contrib/zstd/compress/huf_compress.c',
    'contrib/zstd/compress/zstd_compress_literals.c',
    'contrib/zstd/compress/zstd_compress_sequences.c',
    'contrib/zstd/compress/zstd_compress_superblock.c',
    'contrib/zstd/compress/zstd_compress.c',
    'contrib/zstd/compress/zstd_double_fast.c',
    'contrib/zstd/compress/zstd_fast.c',
    'contrib/zstd/compress/zstd_lazy.c',
    'contrib/zstd/compress/zstd_ldm.c',
    'contrib/zstd/compress/zstd_opt.c',
    'contrib/zstd/compress/zstdmt_compress.c',
    'contrib/zstd/decompress/huf_decompress.c',
    'contrib/zstd/decompress/huf_decompress_amd64.S',
    'contrib/zstd/decompress/zstd_ddict.c',
    'contrib/zstd/decompress/zstd_decompress_block.c',
    'contrib/zstd/decompress/zstd_decompress.c',
    'contrib/zstd/dictBuilder/cover.c',
    'contrib/zstd/dictBuilder/divsufsort.c',
    'contrib/zstd/dictBuilder/fastcover.c',
    'contrib/zstd/dictBuilder/zdict.c',
]

terminator_src = [
    'contrib/kyotocabinet/kccachedb.cc',
    'contrib/kyotocabinet/kccompare.cc',
    'contrib/kyotocabinet/kccompress.cc',
    'contrib/kyotocabinet/kcdb.cc',
    'contrib/kyotocabinet/kcdbext.cc',
    'contrib/kyotocabinet/kcdirdb.cc',
    'contrib/kyotocabinet/kcfile.cc',
    'contrib/kyotocabinet/kchashdb.cc',
    'contrib/kyotocabinet/kclangc.cc',
    'contrib/kyotocabinet/kcmap.cc',
    'contrib/kyotocabinet/kcplantdb.cc',
    'contrib/kyotocabinet/kcpolydb.cc',
    'contrib/kyotocabinet/kcprotodb.cc',
    'contrib/kyotocabinet/kcregex.cc',
    'contrib/kyotocabinet/kcstashdb.cc',
    'contrib/kyotocabinet/kctextdb.cc',
    'contrib/kyotocabinet/kcthread.cc',
    'contrib/kyotocabinet/kcutil.cc',
    'contrib/terminator/terminator_classifier_base.cc',
    'contrib/terminator/terminator_classifier_bwinnow.cc',
    'contrib/terminator/terminator_classifier_hit.cc',
    'contrib/terminator/terminator_classifier_lr.cc',
    'contrib/terminator/terminator_classifier_nb.cc',
    'contrib/terminator/terminator_classifier_nsnb.cc',
    'contrib/terminator/terminator_classifier_owv.cc',
    'contrib/terminator/terminator_classifier_pa.cc',
    'contrib/terminator/terminator_classifier_pam.cc',
    'contrib/terminator/terminator_classifier_winnow.cc',
    'contrib/terminator/terminator.cc',
]

lzma_src = [
    'contrib/lzma/7zBuf.c',
    'contrib/lzma/7zBuf2.c',
    'contrib/lzma/7zCrc.c',
    'contrib/lzma/7zCrcOpt.c',
    'contrib/lzma/7zDec.c',
    'contrib/lzma/7zFile.c',
    'contrib/lzma/7zIn.c',
    'contrib/lzma/7zStream.c',
    'contrib/lzma/Bcj2.c',
    'contrib/lzma/Bra.c',
    'contrib/lzma/Bra86.c',
    'contrib/lzma/CpuArch.c',
    'contrib/lzma/Lzma2Dec.c',
    'contrib/lzma/LzmaDec.c',
    'contrib/lzma/Ppmd7.c',
    'contrib/lzma/Ppmd7Dec.c',
]

ini_src = [
    'contrib/ini/ini.c',
]

mongoose_src = [
    'contrib/mongoose/mongoose.c',
]

inn_src = [
    'contrib/inn/date.cpp',
]


common_lib = static_library('common', common_src)
uat_lib = static_library('uat', libuat_src)
zstd_lib = static_library('zstd', zstd_src)
terminator_lib = static_library('terminator', terminator_src, include_directories: 'contrib/kyotocabinet')
lzma_lib = static_library('lzma', lzma_src)
ini_lib = static_library('ini', ini_src)
mongoose_lib = static_library('mongoose', mongoose_src)
inn_lib = static_library('inn', inn_src, include_directories: 'contrib/inn')


curl_dep = dependency('libcurl')
openssl_dep = dependency('openssl')
icu_dep = dependency('icu-uc')
ncurses_dep = dependency('ncursesw')
gmime_dep = dependency('gmime-3.0')
lz4_dep = dependency('liblz4')


connectivity = executable(
    'connectivity',
    'connectivity/connectivity.cpp',
    link_with: [common_lib, inn_lib],
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

export_messages = executable(
    'export-messages',
    'export-messages/export-messages.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

extract_msgid = executable(
    'extract-msgid',
    'extract-msgid/extract-msgid.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

extract_msgmeta = executable(
    'extract-msgmeta',
    'extract-msgmeta/extract-msgmeta.cpp',
    'extract-msgmeta/tin.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

filter_newsgroups = executable(
    'filter-newsgroups',
    'filter-newsgroups/filter-newsgroups.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

filter_spam = executable(
    'filter-spam',
    'filter-spam/filter-spam.cpp',
    link_with: [common_lib, terminator_lib, zstd_lib],
    include_directories: 'contrib/kyotocabinet',
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

galaxy_util = executable(
    'galaxy-util',
    'galaxy-util/galaxy-util.cpp',
    link_with: [common_lib, uat_lib, zstd_lib],
    install: true,
    install_dir: 'lib/uat'
)

google_groups = executable(
    'google-groups',
    'google-groups/google-groups.cpp',
    link_with: common_lib,
    dependencies: [curl_dep, openssl_dep],
    install: true,
    install_dir: 'lib/uat'
)

import_source_maildir = executable(
    'import-source-maildir',
    'import-source-maildir/import-source-maildir.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

import_source_maildir_7z = executable(
    'import-source-maildir-7z',
    'import-source-maildir-7z/import-source-maildir-7z.cpp',
    link_with: [common_lib, lzma_lib],
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

import_source_mbox = executable(
    'import-source-mbox',
    'import-source-mbox/import-source-mbox.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

kill_duplicates = executable(
    'kill-duplicates',
    'kill-duplicates/kill-duplicates.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

lexdist = executable(
    'lexdist',
    'lexdist/lexdist.cpp',
    link_with: common_lib,
    install: true,
    install_dir: 'lib/uat'
)

lexicon = executable(
    'lexicon',
    'lexicon/lexicon.cpp',
    link_with: common_lib,
    dependencies: [icu_dep, lz4_dep],
    install: true,
    install_dir: 'lib/uat'
)

lexsort = executable(
    'lexsort',
    'lexsort/lexsort.cpp',
    link_with: common_lib,
    install: true,
    install_dir: 'lib/uat'
)

lexstats = executable(
    'lexstats',
    'lexstats/lexstats.cpp',
    link_with: common_lib,
    install: true,
    install_dir: 'lib/uat'
)

merge_raw = executable(
    'merge-raw',
    'merge-raw/merge-raw.cpp',
    link_with: common_lib,
    install: true,
    install_dir: 'lib/uat'
)

nntp_get = executable(
    'nntp-get',
    'nntp-get/nntp-get.cpp',
    'nntp-get/Socket.cpp',
    link_with: [common_lib, inn_lib],
    install: true,
    install_dir: 'lib/uat'
)

package = executable(
    'package',
    'package/package.cpp',
    link_with: common_lib,
    install: true,
    install_dir: 'lib/uat'
)

query = executable(
    'query',
    'query/query.cpp',
    link_with: [common_lib, zstd_lib, uat_lib],
    install: true,
    install_dir: 'lib/uat'
)

query_raw = executable(
    'query-raw',
    'query-raw/query-raw.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

relative_complement = executable(
    'relative-complement',
    'relative-complement/relative-complement.cpp',
    link_with: common_lib,
    install: true,
    install_dir: 'lib/uat'
)

repack_lz4 = executable(
    'repack-lz4',
    'repack-lz4/repack-lz4.cpp',
    link_with: [common_lib, zstd_lib],
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

repack_zstd = executable(
    'repack-zstd',
    'repack-zstd/repack-zstd.cpp',
    cpp_args: ['-DZDICT_STATIC_LINKING_ONLY', '-DZSTD_MULTITHREAD'],
    link_with: [common_lib, zstd_lib],
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

sort = executable(
    'sort',
    'sort/sort.cpp',
    link_with: common_lib,
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

tbrowser = executable(
    'tbrowser',
    'tbrowser/BitSet.cpp',
    'tbrowser/BottomBar.cpp',
    'tbrowser/Browser.cpp',
    'tbrowser/ChartView.cpp',
    'tbrowser/GalaxyOpen.cpp',
    'tbrowser/GalaxyWarp.cpp',
    'tbrowser/HeaderBar.cpp',
    'tbrowser/Help.cpp',
    'tbrowser/MessageView.cpp',
    'tbrowser/SearchView.cpp',
    'tbrowser/tbrowser.cpp',
    'tbrowser/TextView.cpp',
    'tbrowser/ThreadTree.cpp',
    'tbrowser/ThreadView.cpp',
    'tbrowser/Utf8Print.cpp',
    'tbrowser/View.cpp',
    link_with: [common_lib, zstd_lib, uat_lib],
    dependencies: [icu_dep, ncurses_dep],
    install: true,
    install_dir: 'lib/uat'
)

threadify = executable(
    'threadify',
    'threadify/threadify.cpp',
    link_with: [common_lib, zstd_lib, uat_lib],
    dependencies: icu_dep,
    install: true,
    install_dir: 'lib/uat'
)

uat = executable(
    'uat',
    'uat/uat.cpp',
    install: true
)

update_zstd = executable(
    'update-zstd',
    'update-zstd/update-zstd.cpp',
    link_with: [common_lib, zstd_lib],
    cpp_args: '-DZDICT_STATIC_LINKING_ONLY',
    dependencies: lz4_dep,
    install: true,
    install_dir: 'lib/uat'
)

utf8ize = executable(
    'utf8ize',
    'utf8ize/utf8ize.cpp',
    link_with: common_lib,
    dependencies: [gmime_dep, lz4_dep],
    install: true,
    install_dir: 'lib/uat'
)

verify = executable(
    'verify',
    'verify/verify.cpp',
    link_with: [common_lib, zstd_lib, uat_lib],
    install: true,
    install_dir: 'lib/uat'
)

web = executable(
    'web',
    'web/web.cpp',
    link_with: [common_lib, zstd_lib, uat_lib, ini_lib, mongoose_lib],
    dependencies: icu_dep,
    install: true,
    install_dir: 'lib/uat'
)

install_man([
    'man/uat-connectivity.1',
    'man/uat-export-messages.1',
    'man/uat-extract-msgid.1',
    'man/uat-extract-msgmeta.1',
    'man/uat-filter-newsgroups.1',
    'man/uat-filter-spam.1',
    'man/uat-galaxy-util.1',
    'man/uat-google-groups.1',
    'man/uat-import-source-maildir-7z.1',
    'man/uat-import-source-maildir.1',
    'man/uat-import-source-mbox.1',
    'man/uat-kill-duplicates.1',
    'man/uat-lexdist.1',
    'man/uat-lexicon.1',
    'man/uat-lexsort.1',
    'man/uat-lexstats.1',
    'man/uat-libuat.1',
    'man/uat-merge-raw.1',
    'man/uat-nntp-get.1',
    'man/uat-package.1',
    'man/uat-query-raw.1',
    'man/uat-query.1',
    'man/uat-relative-complement.1',
    'man/uat-repack-lz4.1',
    'man/uat-repack-zstd.1',
    'man/uat-sort.1',
    'man/uat-tbrowser.1',
    'man/uat-threadify.1',
    'man/uat-update-zstd.1',
    'man/uat-utf8ize.1',
    'man/uat-verify.1',
    'man/uat.1',
])
