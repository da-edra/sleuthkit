#pragma once

#include <stdint.h>

#include "tsk_apfs.h"

#ifdef __cplusplus
extern "C" {
#else
#define static_assert(x, y)  // static assertions are not valid in C
#endif

// All structures are defined as they exist on disk, so we need to disable
// padding
#pragma pack(push, 1)

typedef struct TSK_FS_INFO TSK_FS_INFO;
typedef struct TSK_FS_FILE TSK_FS_FILE;

#define APFS_ROOT_INODE_NUM 2

typedef enum {
  APFS_JOBJTYPE_SNAP_METADATA = 1,
  APFS_JOBJTYPE_PHYS_EXTENT,
  APFS_JOBJTYPE_INODE,
  APFS_JOBJTYPE_XATTR,
  APFS_JOBJTYPE_SIBLING_LINK,
  APFS_JOBJTYPE_DSTREAM_ID,
  APFS_JOBJTYPE_CRYPTO_STATE,
  APFS_JOBJTYPE_FILE_EXTENT,
  APFS_JOBJTYPE_DIR_RECORD,
  APFS_JOBJTYPE_DIR_STATS,
  APFS_JOBJTYPE_SNAP_NAME,
  APFS_JOBJTYPE_SIBLING_MAP,
} APFS_JOBJTYPE;

typedef enum {
  APFS_ITEM_TYPE_FIFO = 1,
  APFS_ITEM_TYPE_CHAR_DEVICE = 2,
  APFS_ITEM_TYPE_DIRECTORY = 4,
  APFS_ITEM_TYPE_BLOCK_DEVICE = 6,
  APFS_ITEM_TYPE_REGULAR = 8,
  APFS_ITEM_TYPE_SYMBOLIC_LINK = 10,
  APFS_ITEM_TYPE_SOCKET = 12,
  APFS_ITEM_TYPE_WHITEOUT = 14,
} APFS_ITEM_TYPE;

typedef enum {
  APFS_XATTR_FLAG_NONRES = 0x0001,
  APFS_XATTR_FLAG_INLINE = 0x0002,
  APFS_XATTR_FLAG_SYMLINK = 0x0004,
} APFS_XATTR_FLAGS;

typedef enum {
  APFS_BSD_FLAG_UF_NODUMP = 0x00000001,
  APFS_BSD_FLAG_UF_IMMUTABLE = 0x00000002,
  APFS_BSD_FLAG_UF_APPEND = 0x00000004,
  APFS_BSD_FLAG_UF_OPAQUE = 0x00000008,
  APFS_BSD_FLAG_UF_COMPRESSED = 0x00000020,
  APFS_BSD_FLAG_UF_TRACKED = 0x00000040,
  APFS_BSD_FLAG_UF_DATAVAULT = 0x00000080,
  APFS_BSD_FLAG_UF_HIDDEN = 0x00008000,
  APFS_BSD_FLAG_SF_ARCHIVED = 0x00010000,
  APFS_BSD_FLAG_SF_IMMUTABLE = 0x00020000,
  APFS_BSD_FLAG_SF_APPEND = 0x00040000,
  APFS_BSD_FLAG_SF_RESTRICTED = 0x00080000,
  APFS_BSD_FLAG_SF_NOUNLINK = 0x00100000,
} APFS_BSD_FLAGS;

/* special xattrs */
#define APFS_XATTR_NAME_DECOMPFS "com.apple.decmpfs"
#define APFS_XATTR_NAME_FINDERINFO "com.apple.FinderInfo"
#define APFS_XATTR_NAME_RESOURCEFORK "com.apple.ResourceFork"
#define APFS_XATTR_NAME_SECURITY "com.apple.system.Security"
#define APFS_XATTR_NAME_SYMLINK "com.apple.fs.symlink"

// Flags for apfs_snap_metadata.flags
#define APFS_SNAP_METADATA_PENDING_DATALESS 0x00000001

typedef struct {
  uint64_t extentref_tree_oid;   // 0x00
  uint64_t sblock_oid;           // 0x08
  uint64_t create_time;          // 0x10
  uint64_t changed_time;         // 0x18
  uint64_t private_id;           // 0x20
  uint32_t extentref_tree_type;  // 0x28
  uint32_t flags;                // 0x2C
  uint16_t name_length;          // 0x30
  char name[0];                  // 0x32 (name_length bytes)
} apfs_snap_metadata;
static_assert(sizeof(apfs_snap_metadata) == 0x32, "improperly aligned struct");

// Bitfields for apfs_phys_extent.len_and_kind
#define APFS_PHYS_EXTENT_LEN_BITS 60
#define APFS_PHYS_EXTENT_LEN_SHIFT 0
#define APFS_PHYS_EXTENT_KIND_BITS 4
#define APFS_PHYS_EXTENT_KIND_SHIFT 60

typedef struct {
  uint64_t len_and_kind;   // 0x00
  uint64_t owning_obj_id;  // 0x08
  uint32_t refcnt;         // 0x10
} apfs_phys_extent;
static_assert(sizeof(apfs_phys_extent) == 0x14, "improperly aligned struct");

// Flags for apfs_inode.flags
#define APFS_INODE_IS_APFS_PRIVATE = 0x00000001
#define APFS_INODE_MAINTAIN_DIR_STATS = 0x00000002
#define APFS_INODE_DIR_STATS_ORIGIN = 0x00000004
#define APFS_INODE_PROT_CLASS_EXPLICIT = 0x00000008
#define APFS_INODE_WAS_CLONED = 0x00000010
#define APFS_INODE_FLAG_UNUSED = 0x00000020
#define APFS_INODE_HAS_SECURITY_EA = 0x00000040
#define APFS_INODE_BEING_TRUNCATED = 0x00000080
#define APFS_INODE_HAS_FINDER_INFO = 0x00000100
#define APFS_INODE_IS_SPARSE = 0x00000200
#define APFS_INODE_WAS_EVER_CLONED = 0x00000400
#define APFS_INODE_ACTIVE_FILE_TRIMMED = 0x00000800
#define APFS_INODE_PINNED_TO_MAIN = 0x00001000
#define APFS_INODE_PINNED_TO_TIER2 = 0x00002000
#define APFS_INODE_HAS_RSRC_FORK = 0x00004000
#define APFS_INODE_NO_RSRC_FORK = 0x00008000
#define APFS_INODE_ALLOCATION_SPILLEDOVER = 0x00010000

// Bitfields for apfs_inode.mode_and_type
#define APFS_INODE_MODE_BITS 12
#define APFS_INODE_MODE_SHIFT 0
#define APFS_INODE_TYPE_BITS 4
#define APFS_INODE_TYPE_SHIFT 12

typedef struct {
  uint64_t parent_id;      // 0x00
  uint64_t private_id;     // 0x08
  uint64_t create_time;    // 0x10
  uint64_t modified_time;  // 0x18
  uint64_t changed_time;   // 0x20
  uint64_t accessed_time;  // 0x28
  uint64_t flags;          // 0x30
  union {                  // 0x38
    int32_t nlink;
    int32_t nchildren;
  };
  uint32_t default_protection_class;  // 0x3C
  uint32_t write_generation_counter;  // 0x40
  uint32_t bsdflags;                  // 0x44
  uint32_t owner;                     // 0x48
  uint32_t group;                     // 0x4C
  uint16_t mode_and_type;             // 0x50
  uint16_t padding52;                 // 0x52
  uint64_t padding54;                 // 0x54
  uint8_t xfields[0];                 // 0x5C
} apfs_inode;
static_assert(sizeof(apfs_inode) == 0x5C, "improperly aligned struct");

// Flags for apfs_xattr.flags
#define APFS_XATTR_DATA_STREAM = 0x0001
#define APFS_XATTR_DATA_EMBEDDED = 0x0002
#define APFS_XATTR_FILE_SYSTEM_OWNED = 0x0004
#define APFS_XATTR_RESERVED_8 = 0x0008

typedef struct {
  uint16_t flags;      // 0x00
  uint16_t xdata_len;  // 0x02
  uint8_t xdata[0];    // 0x04
} apfs_xattr;
static_assert(sizeof(apfs_xattr) == 0x04, "improperly aligned struct");

typedef struct {
  uint64_t parent;       // 0x00
  uint16_t name_length;  // 0x08
  char name[0];          // 0x0C (name_length bytes)
} apfs_sibling_link;
static_assert(sizeof(apfs_sibling_link) == 0x0A, "improperly aligned struct");

typedef struct {
  uint32_t refcnt;  // 0x00
} apfs_dstream_id;
static_assert(sizeof(apfs_dstream_id) == 0x04, "improperly aligned struct");

typedef struct {
  uint32_t refcount;  // 0x00
  struct {
    uint16_t major_version;     // 0x04
    uint16_t minor_version;     // 0x06
    uint32_t cpflags;           // 0x08
    uint32_t persistent_class;  // 0x0C
    uint32_t key_os_version;    // 0x10
    uint16_t key_revision;      // 0x14
    uint16_t key_len;           // 0x16
    uint8_t persistent_key[0];  // 0x18
  } state;
} apfs_crypto_state;
static_assert(sizeof(apfs_crypto_state) == 0x18, "improperly aligned struct");

// Bitfield values from apfs_file_extent.len_and_flags;
#define APFS_FILE_EXTENT_LEN_BITS 56
#define APFS_FILE_EXTENT_LEN_SHIFT 0
#define APFS_FILE_EXTENT_FLAGS_BITS 8
#define APFS_FILE_EXTENT_FLAGS_SHIFT 56

typedef struct {
  uint64_t len_and_flags;  // 0x00
  uint64_t phys;           // 0x08
  uint64_t crypto;         // 0x10
} apfs_file_extent;
static_assert(sizeof(apfs_file_extent) == 0x18, "improperly aligned struct");

// Bitfield values for apfs_dir_record.type_and_flags
#define APFS_DIR_RECORD_TYPE_BITS 4
#define APFS_DIR_RECORD_TYPE_SHIFT 0
#define APFS_DIR_RECORD_FLAGS_BITS 12
#define APFS_DIR_RECORD_FLAGS_SHIFT 4

typedef struct {
  uint64_t file_id;         // 0x00
  uint64_t date_added;      // 0x08
  uint16_t type_and_flags;  // 0x10
} apfs_dir_record;
static_assert(sizeof(apfs_dir_record) == 0x12, "improperly aligned struct");

typedef struct {
  uint64_t num_children;  // 0x00
  uint64_t total_size;    // 0x08
  uint64_t chained_key;   // 0x10
  uint64_t gen_count;     // 0x18
} apfs_dir_stats;
static_assert(sizeof(apfs_dir_stats) == 0x20, "improperly aligned struct");

typedef struct {
  uint64_t snap_xid;  // 0x00
} apfs_snap_name;
static_assert(sizeof(apfs_snap_name) == 0x08, "improperly aligned struct");

typedef struct {
  uint64_t orig_file_id;  // 0x00
} apfs_sibling_map;
static_assert(sizeof(apfs_sibling_map) == 0x08, "improperly aligned struct");

typedef enum {
  APFS_XFIELD_TYPE_SIBLING_ID = 0x01,  // used in drecs
  APFS_XFIELD_TYPE_SNAP_XID = 0x01,    // used in inodes
  APFS_XFIELD_TYPE_DELTA_TREE_OID = 0x02,
  APFS_XFIELD_TYPE_DOCUMENT_ID = 0x03,
  APFS_XFIELD_TYPE_NAME = 0x04,
  APFS_XFIELD_TYPE_PREV_FSIZE = 0x05,
  APFS_XFIELD_TYPE_FINDER_INFO = 0x07,
  APFS_XFIELD_TYPE_DSTREAM = 0x08,
  APFS_XFIELD_TYPE_DIR_STATS_KEY = 0x0A,
  APFS_XFIELD_TYPE_FS_UUID = 0x0B,
  APFS_XFIELD_TYPE_SPARSE_BYTES = 0x0D,
  APFS_XFIELD_TYPE_DEVICE = 0x0E,
} APFS_XFIELD_TYPE;

// Flags for apfs_xfield_entry.flags
#define APFS_XFIELD_ENTRY_DATA_DEPENDENT 0x01
#define APFS_XFIELD_ENTRY_DO_NOT_COPY 0x02
#define APFS_XFIELD_ENTRY_RESERVED_4 0x04
#define APFS_XFIELD_ENTRY_CHILDREN_INHERIT 0x08
#define APFS_XFIELD_ENTRY_USER_FIELD 0x10
#define APFS_XFIELD_ENTRY_SYSTEM_FIELD 0x20
#define APFS_XFIELD_ENTRY_RESERVED_40 0x40
#define APFS_XFIELD_ENTRY_RESERVED_80 0x80

typedef struct {
  uint8_t type;   // 0x00
  uint8_t flags;  // 0x01
  uint16_t len;   // 0x02
} apfs_xfield_entry;
static_assert(sizeof(apfs_xfield_entry) == 0x04, "improperly aligned struct");

typedef struct {
  uint16_t num_exts;
  uint16_t used_data;
  apfs_xfield_entry entries[0];
} apfs_xfield;
static_assert(sizeof(apfs_xfield) == 0x04, "improperly aligned struct");

typedef struct {
  uint64_t size;
  uint64_t alloced_size;
  uint64_t default_crypto_id;
  uint64_t total_bytes_written;
  uint64_t total_bytes_read;
} apfs_dstream;
static_assert(sizeof(apfs_dstream) == 0x28, "improperly aligned struct");

// End on-disk structures
#pragma pack(pop)

// TSK API

typedef struct {
  char name[0x80];
  char uuid[16];
  char password_hint[0x100];
  char formatted_by[0x20];
  apfs_block_num apsb_block_num;
  uint64_t apsb_oid;
  uint64_t apsb_xid;
  uint64_t capacity_consumed;
  uint64_t capacity_reserved;
  uint64_t capacity_quota;
  uint64_t created;
  uint64_t changed;
  struct {
    char kext_ver_str[0x20];
    uint64_t timestamp;
    uint64_t last_xid;
  } unmount_logs[8];
  APFS_VOLUME_ROLE role;
  char case_sensitive;
  char encrypted;
} apfs_fsstat_info;

extern uint8_t tsk_apfs_fsstat(TSK_FS_INFO *fs_info, apfs_fsstat_info *info);

typedef struct {
  uint64_t date_added;
  uint64_t cloned_inum;
  uint32_t bsdflags;
} apfs_istat_info;

extern uint8_t tsk_apfs_istat(TSK_FS_FILE *fs_file, apfs_istat_info *info);

typedef struct {
  uint64_t snap_xid;
  uint64_t timestamp;
  char *name;
  int dataless;
} apfs_snapshot;

typedef struct {
  int num_snapshots;
  int _reserved;  // unused (ensures consistant alignment)
  apfs_snapshot snapshots[0];
} apfs_snapshot_list;

extern uint8_t tsk_apfs_list_snapshots(TSK_FS_INFO *fs_info,
                                       apfs_snapshot_list **list);
extern uint8_t tsk_apfs_free_snapshot_list(apfs_snapshot_list *list);
extern uint8_t tsk_apfs_set_snapshot(TSK_FS_INFO *fs_info, uint64_t snap_xid);

#ifdef __cplusplus
}
#else
#undef static_assert
#endif
