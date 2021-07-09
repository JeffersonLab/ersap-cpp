message(STATUS "Downloading Doxygen tags from cppreference.com...")
set(tagfile_url http://upload.cppreference.com/mwiki/images/f/f8/${TAGFILE})
file(DOWNLOAD ${tagfile_url} ${DESTDIR}/${TAGFILE})
message(STATUS "Doxygen tags downloaded.")
