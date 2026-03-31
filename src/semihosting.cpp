// ARM semihosting syscall stubs for bare-metal Renode targets.
//
// Implements the newlib/picolibc syscall layer using ARM semihosting
// (BKPT #0xAB). Renode intercepts these traps and forwards I/O to the
// host console; SYS_EXIT halts the simulated CPU with the exit code so
// Meson/Robot Framework can read pass/fail.
//
// Reference: ARM IHI0033B "Semihosting for AArch32 and AArch64"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <sys/stat.h>

extern "C" {

// --------------------------------------------------------------------------
// Semihosting operation numbers (AArch32)
// --------------------------------------------------------------------------

static constexpr int SYS_WRITE = 0x05;
static constexpr int SYS_READ  = 0x06;
static constexpr int SYS_EXIT  = 0x18;

// ADP_Stopped reason codes
static constexpr int ADP_STOPPED_APPLICATION_EXIT = 0x20026;
static constexpr int ADP_STOPPED_RUN_TIME_ERROR   = 0x20023;

// --------------------------------------------------------------------------
// Raw semihosting call
// --------------------------------------------------------------------------

static inline int semihosting_call(int op, void* arg) {
    register int r0 asm("r0") = op;
    register void* r1 asm("r1") = arg;
    asm volatile("bkpt #0xAB" : "+r"(r0) : "r"(r1) : "memory");
    return r0;
}

// --------------------------------------------------------------------------
// Newlib syscall implementations
// --------------------------------------------------------------------------

int _write([[maybe_unused]] int fd, const char* buf, int len) {
    struct { int fd; const char* buf; int len; } args = { fd, buf, len };
    int remaining = semihosting_call(SYS_WRITE, &args);
    return len - remaining;
}

[[noreturn]] void _exit(int status) {
    // Two-word parameter block: reason + subcode
    struct { int reason; int subcode; } args = {
        (status == 0) ? ADP_STOPPED_APPLICATION_EXIT
                      : ADP_STOPPED_RUN_TIME_ERROR,
        status
    };
    semihosting_call(SYS_EXIT, &args);
    while (true) {}  // unreachable — keeps [[noreturn]] happy
}

int _read([[maybe_unused]] int fd, char* buf, int len) {
    struct { int fd; char* buf; int len; } args = { fd, buf, len };
    int remaining = semihosting_call(SYS_READ, &args);
    return len - remaining;
}

int _close([[maybe_unused]] int fd) {
    errno = ENOSYS;
    return -1;
}

int _fstat([[maybe_unused]] int fd, struct stat* st) {
    st->st_mode = S_IFCHR;  // treat all fds as character devices
    return 0;
}

int _isatty([[maybe_unused]] int fd) {
    return 1;
}

int _lseek([[maybe_unused]] int fd, [[maybe_unused]] int offset,
           [[maybe_unused]] int whence) {
    errno = ENOSYS;
    return -1;
}

// Static heap for newlib's malloc — sized to hold boost::ut's test registry.
// Increase if linker reports heap exhaustion.
void* _sbrk(int incr) {
    static constexpr std::size_t heap_size = 8192;
    static std::uint8_t heap[heap_size];
    static std::uint8_t* heap_ptr = heap;

    if (heap_ptr + incr > heap + heap_size) {
        errno = ENOMEM;
        return reinterpret_cast<void*>(-1);
    }
    void* prev = heap_ptr;
    heap_ptr += incr;
    return prev;
}

} // extern "C"
