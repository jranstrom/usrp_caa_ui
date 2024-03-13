cmake_minimum_required(VERSION 3.5)

project(caa_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Used :: UHD_4.4.0.0-124-g8a670958
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)
find_package(UHD 4.4.0 REQUIRED)

set(UHD_BOOST_REQUIRED_COMPONENTS
    program_options
    system
    thread
    filesystem
)

set(BOOST_MIN_VERSION 1.65)
include(UHDBoost)

set(PROJECT_SOURCES
        main.cpp
        maingui.cpp
        maingui.h
        maingui.ui
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${UHD_INCLUDE_DIRS}
    ${ARMADILLO_INCLUDE_DIRS}
    "/usr/local/include"
    "third-party"
    "custom"
)

set(serial_SRCS third-party/serial.cc third-party/serial/serial.h
    third-party/serial/v8stdint.h third-party/impl/list_ports/list_ports_linux.cc)

list(APPEND serial_SRCS third-party/impl/unix.cc)

add_library(serial ${serial_SRCS})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(caa_project
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET caa_project APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(caa_project SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(caa_project
            ${PROJECT_SOURCES}
            qcustomplot.h qcustomplot.cpp
            custom/RadioSysConfig.h custom/RadioSysConfig.cpp
            custom/matHF.h
            custom/radiosysobject.h custom/radiosysobject.cpp
            third-party/config_file.h
            circbuffer.h
            custom/uhd_clib.h custom/uhd_clib.cpp
            uiobj.h uiobj.cpp
            custom/TeensyCOM.h custom/TeensyCOM.cpp
            tcom_ui.h tcom_ui.cpp
            mccontrolwidget.h mccontrolwidget.cpp
            custom/labelandfieldwidget.h custom/labelandfieldwidget.cpp
            custom/indicatorbuttonwidget.h custom/indicatorbuttonwidget.cpp
            custom/indicatorgroupwidget.h custom/indicatorgroupwidget.cpp
            custom/sliderandlineedit.h custom/sliderandlineedit.cpp

        )
    endif()
endif()

set(QT_LIB Qt5::PrintSupport)

target_link_libraries(caa_project PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${QT_LIB} "/usr/local/lib/libmatio.so" ${UHD_LIBRARIES} ${Boost_LIBRARIES} "serial")

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.caa_project)
endif()
set_target_properties(caa_project PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE FALSE
    WIN32_EXECUTABLE FALSE
)

include(GNUInstallDirs)
install(TARGETS caa_project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(caa_project)
endif()
