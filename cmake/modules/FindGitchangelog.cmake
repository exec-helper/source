find_program(GITCHANGELOG NAMES gitchangelog PATHS /bin /usr/bin /usr/local/bin)

mark_as_advanced(GITCHANGELOG)

if(GITCHANGELOG-NOTFOUND)
    MESSAGE(STATUS "Did not find gitchangelog")
    SET(GITCHANGELOG_FOUND OFF)
else()
    MESSAGE(STATUS "Found gitchangelog: " ${GITCHANGELOG})
    SET(GITCHANGELOG_FOUND ON)
endif()
