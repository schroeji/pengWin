file(GLOB pengWin_srcs "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/misc/*" "${CMAKE_CURRENT_SOURCE_DIR}/hacks/*"
     "${CMAKE_CURRENT_SOURCE_DIR}/vpk_parsing/*")

file(GLOB dumper_srcs "${CMAKE_CURRENT_SOURCE_DIR}/offset_dumper/*"
     "${CMAKE_CURRENT_SOURCE_DIR}/misc/*")

set(pengwin "pengWin")
set(offsetDumper "offsetDumper")
add_executable(${pengwin} ${pengWin_srcs})
target_include_directories(
  ${pengwin} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(
  Boost
  COMPONENTS thread
  REQUIRED)
find_package(
  Boost
  COMPONENTS system
  REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(${pengwin} PRIVATE Boost::thread Boost::system X11
                                         Threads::Threads)
set(CMAKE_BUILD_RPATH "$ORIGIN/bin/linux64")
set(CMAKE_INSTALL_RPATH "$ORIGIN/bin/linux64")

add_executable("offsetDumper" ${dumper_srcs})
target_link_libraries(${offsetDumper} PRIVATE Boost::thread Boost::system X11
                                              Threads::Threads)
target_include_directories(
  ${offsetDumper} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>)
