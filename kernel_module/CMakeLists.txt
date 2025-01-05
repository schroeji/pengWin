# Find kernel headers
set(TARGET_NAME "kernel_module")
set(LIBRARY_NAME "mem_access")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(KernelHeaders REQUIRED)
include_directories(${KERNELHEADERS_INCLUDE_DIRS})

#Check that KERNEL_DIR is set
if (NOT KERNELHEADERS_DIR)
    message(FATAL_ERROR "Please specify the KERNEL_DIR path to the kernel source")
endif()

# Use the kernel build system to build the kernel module
add_custom_target(
    ${TARGET_NAME} ALL
    COMMAND make -C ${KERNELHEADERS_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} modules
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building kernel module ${LIBRARY_NAME}"
)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_NAME}.ko ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.ko
)

# then add the custom command to copy it
add_custom_command(
  TARGET ${TARGET_NAME}
  POST_BUILD
  COMMAND "make"
  ARGS "clean"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
