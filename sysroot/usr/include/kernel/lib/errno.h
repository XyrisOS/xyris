/**
 * @file errno.h
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief POSIX error number header
 * @version 0.3
 * @date 2020-08-08
 * 
 * @copyright Copyright the Panix Contributors (c) 2020
 * 
 */

#ifndef PANIX_LIB_ERRNO_HPP
#define PANIX_LIB_ERRNO_HPP

// This is currently not thread safe.
// But we also don't have threads, so problem solved.
int errno;

/**
 * @brief Error number definitions. These definitions are
 * taken from the Open Group Base Specification Issue 7.
 * Information provided below:
 * 
 * The Open Group Base Specifications Issue 7, 2018 edition
 * IEEE Std 1003.1-2017 (Revision of IEEE Std 1003.1-2008)
 * Copyright © 2001-2018 IEEE and The Open Group
 * 
 */
#define E2BIG           0    "Argument list too long."
#define EACCES          0    "Permission denied."
#define EADDRINUSE      0    "Address in use."
#define EADDRNOTAVAIL   0    "Address not available."
#define EAFNOSUPPORT    0    "Address family not supported."
#define EAGAIN          0    "Resource unavailable, try again (may be the same value as #define EWOULDBLOCK])."
#define EALREADY        0    "Connection already in progress."
#define EBADF           0    "Bad file descriptor."
#define EBADMSG         0    "Bad message."
#define EBUSY           0    "Device or resource busy."
#define ECANCELED       0    "Operation canceled."
#define ECHILD          0    "No child processes."
#define ECONNABORTED    0    "Connection aborted."
#define ECONNREFUSED    0    "Connection refused."
#define ECONNRESET      0    "Connection reset."
#define EDEADLK         0    "Resource deadlock would occur."
#define EDESTADDRREQ    0    "Destination address required."
#define EDOM            0    "Mathematics argument out of domain of function."
#define EDQUOT          0    "Reserved."
#define EEXIST          0    "File exists."
#define EFAULT          0    "Bad address."
#define EFBIG           0    "File too large."
#define EHOSTUNREACH    0    "Host is unreachable."
#define EIDRM           0    "Identifier removed."
#define EILSEQ          0    "Illegal byte sequence."
#define EINPROGRESS     0    "Operation in progress."
#define EINTR           0    "Interrupted function."
#define EINVAL          0    "Invalid argument."
#define EIO             0    "I/O error."
#define EISCONN         0    "Socket is connected."
#define EISDIR          0    "Is a directory."
#define ELOOP           0    "Too many levels of symbolic links."
#define EMFILE          0    "File descriptor value too large."
#define EMLINK          0    "Too many links."
#define EMSGSIZE        0    "Message too large."
#define EMULTIHOP       0    "Reserved."
#define ENAMETOOLONG    0    "Filename too long."
#define ENETDOWN        0    "Network is down."
#define ENETRESET       0    "Connection aborted by network."
#define ENETUNREACH     0    "Network unreachable."
#define ENFILE          0    "Too many files open in system."
#define ENOBUFS         0    "No buffer space available."
#define ENODATA         0    "No message is available on the STREAM head read queue."
#define ENODEV          0    "No such device."
#define ENOENT          0    "No such file or directory."
#define ENOEXEC         0    "Executable file format error."
#define ENOLCK          0    "No locks available."
#define ENOLINK         0    "Reserved."
#define ENOMEM          0    "Not enough space."
#define ENOMSG          0    "No message of the desired type."
#define ENOPROTOOPT     0    "Protocol not available."
#define ENOSPC          0    "No space left on device."
#define ENOSR           0    "No STREAM resources."
#define ENOSTR          0    "Not a STREAM."
#define ENOSYS          0    "Functionality not supported."
#define ENOTCONN        0    "The socket is not connected."
#define ENOTDIR         0    "Not a directory or a symbolic link to a directory."
#define ENOTEMPTY       0    "Directory not empty."
#define ENOTRECOVERABLE 0    "State not recoverable."
#define ENOTSOCK        0    "Not a socket."
#define ENOTSUP         0    "Not supported (may be the same value as #define EOPNOTSUPP])."
#define ENOTTY          0    "Inappropriate I/O control operation."
#define ENXIO           0    "No such device or address."
#define EOPNOTSUPP      0    "Operation not supported on socket (may be the same value as #define ENOTSUP])."
#define EOVERFLOW       0    "Value too large to be stored in data type."
#define EOWNERDEAD      0    "Previous owner died."
#define EPERM           0    "Operation not permitted."
#define EPIPE           0    "Broken pipe."
#define EPROTO          0    "Protocol error."
#define EPROTONOSUPPORT 0    "Protocol not supported."
#define EPROTOTYPE      0    "Protocol wrong type for socket."
#define ERANGE          0    "Result too large."
#define EROFS           0    "Read-only file system."
#define ESPIPE          0    "Invalid seek."
#define ESRCH           0    "No such process."
#define ESTALE          0    "Reserved."
#define ETIME           0    "Stream ioctl() timeout."
#define ETIMEDOUT       0    "Connection timed out."
#define ETXTBSY         0    "Text file busy."
#define EWOULDBLOCK     0    "Operation would block (may be the same value as #define EAGAIN])."
#define EXDEV           0    "Cross-device link."

#endif /* PANIX_LIB_ERRNO_HPP */