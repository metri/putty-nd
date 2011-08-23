/*
 * sftp.h: definitions for SFTP and the sftp.c routines.
 */

#include "int64.h"

#define SSH_FXP_INIT                              1	/* 0x1 */
#define SSH_FXP_VERSION                           2	/* 0x2 */
#define SSH_FXP_OPEN                              3	/* 0x3 */
#define SSH_FXP_CLOSE                             4	/* 0x4 */
#define SSH_FXP_READ                              5	/* 0x5 */
#define SSH_FXP_WRITE                             6	/* 0x6 */
#define SSH_FXP_LSTAT                             7	/* 0x7 */
#define SSH_FXP_FSTAT                             8	/* 0x8 */
#define SSH_FXP_SETSTAT                           9	/* 0x9 */
#define SSH_FXP_FSETSTAT                          10	/* 0xa */
#define SSH_FXP_OPENDIR                           11	/* 0xb */
#define SSH_FXP_READDIR                           12	/* 0xc */
#define SSH_FXP_REMOVE                            13	/* 0xd */
#define SSH_FXP_MKDIR                             14	/* 0xe */
#define SSH_FXP_RMDIR                             15	/* 0xf */
#define SSH_FXP_REALPATH                          16	/* 0x10 */
#define SSH_FXP_STAT                              17	/* 0x11 */
#define SSH_FXP_RENAME                            18	/* 0x12 */
#define SSH_FXP_STATUS                            101	/* 0x65 */
#define SSH_FXP_HANDLE                            102	/* 0x66 */
#define SSH_FXP_DATA                              103	/* 0x67 */
#define SSH_FXP_NAME                              104	/* 0x68 */
#define SSH_FXP_ATTRS                             105	/* 0x69 */
#define SSH_FXP_EXTENDED                          200	/* 0xc8 */
#define SSH_FXP_EXTENDED_REPLY                    201	/* 0xc9 */

#define SSH_FX_OK                                 0
#define SSH_FX_EOF                                1
#define SSH_FX_NO_SUCH_FILE                       2
#define SSH_FX_PERMISSION_DENIED                  3
#define SSH_FX_FAILURE                            4
#define SSH_FX_BAD_MESSAGE                        5
#define SSH_FX_NO_CONNECTION                      6
#define SSH_FX_CONNECTION_LOST                    7
#define SSH_FX_OP_UNSUPPORTED                     8

#define SSH_FILEXFER_ATTR_SIZE                    0x00000001
#define SSH_FILEXFER_ATTR_UIDGID                  0x00000002
#define SSH_FILEXFER_ATTR_PERMISSIONS             0x00000004
#define SSH_FILEXFER_ATTR_ACMODTIME               0x00000008
#define SSH_FILEXFER_ATTR_EXTENDED                0x80000000

#define SSH_FXF_READ                              0x00000001
#define SSH_FXF_WRITE                             0x00000002
#define SSH_FXF_APPEND                            0x00000004
#define SSH_FXF_CREAT                             0x00000008
#define SSH_FXF_TRUNC                             0x00000010
#define SSH_FXF_EXCL                              0x00000020

#define SFTP_PROTO_VERSION 3

#include "putty.h"
typedef struct {
    Backend *back;
    void *backhandle;
    Config cfg;
    char *pwd, *homedir;
    /* ----------------------------------------------------------------------
     * Dirty bits: integration with PuTTY.
     */
    int verbose;
    
    unsigned char *outptr;	       /* where to put the data */
    unsigned outlen;		       /* how much data required */
    unsigned char *pending;  /* any spare data */
    unsigned pendlen, pendsize;	/* length and phys. size of buffer */

    int fxp_errtype;
    const char *fxp_error_message;
    tree234 *sftp_requests;

    void* log_handle;
} sftp_handle;

struct sftp_command {
    char **words;
    int nwords, wordssize;
    int (*obey) (sftp_handle* sftp, struct sftp_command *);	/* returns <0 to quit */
};

void sftp_handle_init(sftp_handle* sftp);
void sftp_handle_fini(sftp_handle* sftp);
/*
 * External references. The sftp client module sftp.c expects to be
 * able to get at these functions.
 * 
 * sftp_recvdata must never return less than len. It either blocks
 * until len is available, or it returns failure.
 * 
 * Both functions return 1 on success, 0 on failure.
 */
int sftp_senddata(sftp_handle* sftp, char *data, int len);
int sftp_recvdata(sftp_handle* sftp, char *data, int len);

/*
 * Free sftp_requests
 */
void sftp_cleanup_request(sftp_handle* sftp);

struct fxp_attrs {
    unsigned long flags;
    uint64 size;
    unsigned long uid;
    unsigned long gid;
    unsigned long permissions;
    unsigned long atime;
    unsigned long mtime;
};

struct fxp_handle {
    char *hstring;
    int hlen;
};

struct fxp_name {
    char *filename, *longname;
    struct fxp_attrs attrs;
};

struct fxp_names {
    int nnames;
    struct fxp_name *names;
};

struct sftp_request;
struct sftp_packet;

const char *fxp_error(sftp_handle* sftp);
int fxp_error_type(sftp_handle* sftp);

/*
 * Perform exchange of init/version packets. Return 0 on failure.
 */
int fxp_init(sftp_handle* sftp);

/*
 * Canonify a pathname. Concatenate the two given path elements
 * with a separating slash, unless the second is NULL.
 */
struct sftp_request *fxp_realpath_send(sftp_handle* sftp, char *path);
char *fxp_realpath_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Open a file.
 */
struct sftp_request *fxp_open_send(sftp_handle* sftp, char *path, int type);
struct fxp_handle *fxp_open_recv(sftp_handle* sftp, struct sftp_packet *pktin,
				 struct sftp_request *req);

/*
 * Open a directory.
 */
struct sftp_request *fxp_opendir_send(sftp_handle* sftp, char *path);
struct fxp_handle *fxp_opendir_recv(sftp_handle* sftp, struct sftp_packet *pktin,
				    struct sftp_request *req);

/*
 * Close a file/dir.
 */
struct sftp_request *fxp_close_send(sftp_handle* sftp, struct fxp_handle *handle);
void fxp_close_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Make a directory.
 */
struct sftp_request *fxp_mkdir_send(sftp_handle* sftp, char *path);
int fxp_mkdir_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Remove a directory.
 */
struct sftp_request *fxp_rmdir_send(sftp_handle* sftp, char *path);
int fxp_rmdir_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Remove a file.
 */
struct sftp_request *fxp_remove_send(sftp_handle* sftp, char *fname);
int fxp_remove_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Rename a file.
 */
struct sftp_request *fxp_rename_send(sftp_handle* sftp, char *srcfname, char *dstfname);
int fxp_rename_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Return file attributes.
 */
struct sftp_request *fxp_stat_send(sftp_handle* sftp, char *fname);
int fxp_stat_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req,
		  struct fxp_attrs *attrs);
struct sftp_request *fxp_fstat_send(sftp_handle* sftp, struct fxp_handle *handle);
int fxp_fstat_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req,
		   struct fxp_attrs *attrs);

/*
 * Set file attributes.
 */
struct sftp_request *fxp_setstat_send(sftp_handle* sftp, char *fname, struct fxp_attrs attrs);
int fxp_setstat_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);
struct sftp_request *fxp_fsetstat_send(sftp_handle* sftp, struct fxp_handle *handle,
				       struct fxp_attrs attrs);
int fxp_fsetstat_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Read from a file.
 */
struct sftp_request *fxp_read_send(sftp_handle* sftp, struct fxp_handle *handle,
				   uint64 offset, int len);
int fxp_read_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req,
		  char *buffer, int len);

/*
 * Write to a file. Returns 0 on error, 1 on OK.
 */
struct sftp_request *fxp_write_send(sftp_handle* sftp, struct fxp_handle *handle,
				    char *buffer, uint64 offset, int len);
int fxp_write_recv(sftp_handle* sftp, struct sftp_packet *pktin, struct sftp_request *req);

/*
 * Read from a directory.
 */
struct sftp_request *fxp_readdir_send(sftp_handle* sftp, struct fxp_handle *handle);
struct fxp_names *fxp_readdir_recv(sftp_handle* sftp, struct sftp_packet *pktin,
				   struct sftp_request *req);

/*
 * Free up an fxp_names structure.
 */
void fxp_free_names(sftp_handle* sftp, struct fxp_names *names);

/*
 * Duplicate and free fxp_name structures.
 */
struct fxp_name *fxp_dup_name(sftp_handle* sftp, struct fxp_name *name);
void fxp_free_name(sftp_handle* sftp, struct fxp_name *name);

/*
 * Store user data in an sftp_request structure.
 */
void *fxp_get_userdata(sftp_handle* sftp, struct sftp_request *req);
void fxp_set_userdata(sftp_handle* sftp, struct sftp_request *req, void *data);

/*
 * These functions might well be temporary placeholders to be
 * replaced with more useful similar functions later. They form the
 * main dispatch loop for processing incoming SFTP responses.
 */
void sftp_register(sftp_handle* sftp, struct sftp_request *req);
struct sftp_request *sftp_find_request(sftp_handle* sftp, struct sftp_packet *pktin);
struct sftp_packet *sftp_recv(sftp_handle* sftp);

/*
 * A wrapper to go round fxp_read_* and fxp_write_*, which manages
 * the queueing of multiple read/write requests.
 */

struct fxp_xfer;

struct fxp_xfer *xfer_download_init(sftp_handle* sftp, struct fxp_handle *fh, uint64 offset);
void xfer_download_queue(sftp_handle* sftp, struct fxp_xfer *xfer);
int xfer_download_gotpkt(sftp_handle* sftp, struct fxp_xfer *xfer, struct sftp_packet *pktin);
int xfer_download_data(sftp_handle* sftp, struct fxp_xfer *xfer, void **buf, int *len);

struct fxp_xfer *xfer_upload_init(sftp_handle* sftp, struct fxp_handle *fh, uint64 offset);
int xfer_upload_ready(sftp_handle* sftp, struct fxp_xfer *xfer);
void xfer_upload_data(sftp_handle* sftp, struct fxp_xfer *xfer, char *buffer, int len);
int xfer_upload_gotpkt(sftp_handle* sftp, struct fxp_xfer *xfer, struct sftp_packet *pktin);

int xfer_done(sftp_handle* sftp, struct fxp_xfer *xfer);
void xfer_set_error(sftp_handle* sftp, struct fxp_xfer *xfer);
void xfer_cleanup(sftp_handle* sftp, struct fxp_xfer *xfer);
