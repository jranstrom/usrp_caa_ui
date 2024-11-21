cmake_minimum_required(VERSION 3.5)

project(caa_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(MATLAB_ROOT "/usr/local/MATLAB/R2024b")

# Used :: UHD_4.4.0.0-124-g8a670958
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)
find_package(UHD 4.4.0 REQUIRED)
find_package(Armadillo REQUIRED)

find_package(CURL REQUIRED)

find_package(Threads)

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)

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
    PkgConfig::FFTW
    "/usr/local/include"
    "third-party"
    "custom"
    "/usr/local/MATLAB/R2024b/extern/include"
)

set(serial_SRCS third-party/serial.cc third-party/serial/serial.h
    third-party/serial/v8stdint.h third-party/impl/list_ports/list_ports_linux.cc)

list(APPEND serial_SRCS third-party/impl/unix.cc)

add_library(serial ${serial_SRCS})

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
    custom/labelandspinboxwidget.h custom/labelandspinboxwidget.cpp
    custom/labelandcheckboxwidget.h custom/labelandcheckboxwidget.cpp
    custom/customfftwobject.h custom/customfftwobject.cpp
    custom/cradioobject.h custom/cradioobject.cpp
    custom/radiocontrolwidget.h custom/radiocontrolwidget.cpp
    custom/cradiousrp2.h custom/cradiousrp2.cpp
    custom/cradiob200.h custom/cradiob200.cpp
    custom/cradioproperty.h custom/cradioproperty.cpp
    custom/continousreceptionradiocontrol.h custom/continousreceptionradiocontrol.cpp




)

set(QT_LIB Qt6::PrintSupport)

target_link_libraries(caa_project
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    ${QT_LIB}
    "/usr/lib/x86_64-linux-gnu/libmatio.so"
    ${UHD_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    "/usr/local/MATLAB/R2024b/bin/glnxa64/libmx.so"
    "/usr/local/MATLAB/R2024b/bin/glnxa64/libeng.so"
    "/usr/local/MATLAB/R2024b/extern/bin/glnxa64/libMatlabEngine.so"
    "/usr/local/MATLAB/R2024b/extern/bin/glnxa64/libMatlabDataArray.so"
    ${CMAKE_THREAD_LIBS_INIT}
    "serial"
    ${ARMADILLO_LIBRARIES}
    PkgConfig::FFTW)

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
