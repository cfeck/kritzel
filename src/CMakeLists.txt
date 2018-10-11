include_directories (cellwriter ink)

set (libcellwriter_SRC
    cellwriter/libcellwriter.c
    cellwriter/averages.c
    cellwriter/preprocess.c
    cellwriter/profile.c
    cellwriter/recognize.c
    cellwriter/stroke.c
    cellwriter/unicode.c
    cellwriter/wordfreq.c
)

set (libink_SRC
    ink/cell.cpp
    ink/ink.cpp
    ink/inkitem.cpp
    ink/recognizer.cpp
)

set (kritzel_SRC
    inputpanel.cpp
    view.cpp
    window.cpp

    main.cpp
)

add_executable (kritzel ${libcellwriter_SRC} ${libink_SRC} ${kritzel_SRC})

target_link_libraries (kritzel
    Qt5::Core
    Qt5::Widgets
)

if (ENCHANT_FOUND)
    target_link_libraries (kritzel ${ENCHANT_LIBRARIES})
endif ()

