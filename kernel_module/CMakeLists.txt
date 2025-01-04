# Find kernel headers
set(TARGET_NAME "mem_access")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(KernelHeaders REQUIRED)
include_directories(${KERNELHEADERS_INCLUDE_DIRS})

# set(DRIVER_FILE ${TARGET_NAME}.ko)
# set(KBUILD_CMD $(MAKE) -C ${KERNELHEADERS_DIR} modules M=${CMAKE_CURRENT_BINARY_DIR} src=${CMAKE_CURRENT_SOURCE_DIR})

# # Generate the Kbuild file through cmake.
# FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "obj-m := ${TARGET_NAME}.o")

# add_custom_command(OUTPUT ${DRIVER_FILE}
#         COMMAND ${KBUILD_CMD}
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#         DEPENDS ${TARGET_NAME}.c VERBATIM)

# add_custom_target(kernel_module ALL DEPENDS ${DRIVER_FILE})

# Check that KERNEL_DIR is set
if (NOT KERNELHEADERS_DIR)
    message(FATAL_ERROR "Please specify the KERNEL_DIR path to the kernel source")
endif()

# Specify the name of the module and the source file
set(MODULE_NAME mem_access)
set(SRC_FILES mem_access.c)

# Use the kernel build system to build the kernel module
add_custom_target(
    kernel_module
    COMMAND make -C ${KERNELHEADERS_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} modules
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building kernel module mem_access"
)
