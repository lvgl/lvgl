from struct import Struct

# Header
FROGFS_MAGIC            = 0x474F5246 # FROG
FROGFS_VER_MAJOR        = 1
FROGFS_VER_MINOR        = 0

# FrogFS header
# magic, bin_sz, num_ent, ver_majr, ver_minor
head = Struct('<IBBHI')

# Hash table entry
# hash, offs
hash = Struct('<II')

# Offset
# offs
offs = Struct("<I")

# Entry header
# parent, child_count, seg_sz, opts
ent = Struct('<IHBB')

# Directory header
# parent, child_count, seg_sz, opts
dir = Struct('<IHBB')

# File header
# parent, child_count, seg_sz, opts, data_offs, data_sz
file = Struct('<IHBBII')

# Compressed file header
# parent, child_count, seg_sz, opts, data_offs, data_sz, real_sz
comp = Struct('<IHBBIII')

# FrogFS footer
# crc32
foot = Struct('<I')
