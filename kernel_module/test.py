import binascii

# addr = 0x71A8ACF75A40
# health_offset = 1212
addr = 0x725D54400000 + 58549504
kernel_module = open("/proc/read_access", "rb")
kernel_module.seek(addr, 0)
buff = kernel_module.read(8)
kernel_module.close()
print(binascii.hexlify(buff))
# print(buff.decode("utf-8"))
