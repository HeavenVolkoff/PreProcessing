# - Try to find G3logger
# Once done this will define
#
#  LIBG3LOGGER_FOUND - system has G3logger
#  LIBG3LOGGER_INCLUDE_DIRS - the G3logger include directory
#  LIBG3LOGGER_LIBRARIES - Link these to use G3logger

# Include dir
find_path(LIBG3LOGGER_INCLUDE_DIR
        NAMES g3log.hpp
        PATHS
        /usr/include/g3log
        /usr/local/include/g3log
        /opt/local/include/g3log
        /sw/include/g3log
        )

# Library
find_library(LIBG3LOGGER_LIBRARY
        NAMES g3logger
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        )

find_package(PackageHandleStandardArgs)
find_package_handle_standard_args(LibG3Logger DEFAULT_MSG LIBG3LOGGER_LIBRARY LIBG3LOGGER_INCLUDE_DIR)

if(LIBG3LOGGER_FOUND)
    set(LIBG3LOGGER_LIBRARIES ${LIBG3LOGGER_LIBRARY})
    set(LIBG3LOGGER_INCLUDE_DIRS ${LIBG3LOGGER_INCLUDE_DIR})
endif(LIBG3LOGGER_FOUND)

# show the G3LOGGER_INCLUDE_DIRS and G3LOGGER_LIBRARIES variables only in the advanced view
mark_as_advanced(LIBG3LOGGER_LIBRARY LIBG3LOGGER_LIBRARIES LIBG3LOGGER_INCLUDE_DIR LIBG3LOGGER_INCLUDE_DIRS)