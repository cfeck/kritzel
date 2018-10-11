project (kritzel)
cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

set (QT_MIN_VERSION "5.6.0")
set (KF5_MIN_VERSION "5.25.0")

find_package (ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include (KDEInstallDirs)
include (KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include (KDECMakeSettings)

find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
)

# find_package(Enchant)

if (ENCHANT_FOUND)
    include_directories (${ENCHANT_INCLUDE_DIRS})
endif ()

add_subdirectory (src)

