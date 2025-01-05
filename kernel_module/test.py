import binascii

# addr = 0x71A8ACF75A40
# health_offset = 1212
addr = 0xFFFF
kernel_module = open("/proc/read_access", "rb")
kernel_module.seek(addr, 0)
buff = kernel_module.read(0x20000)
kernel_module.close()
print(buff.decode("utf-8"))
