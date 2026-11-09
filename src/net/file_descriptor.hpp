#pragma once

#include <string>

namespace tether {

// Owns one POSIX file descriptor. The destructor closes it.
// Copying is forbidden: two owners would both call close() on the same int,
// and the kernel reuses descriptor numbers, so the second close can close
// an unrelated file that was opened later.
class FileDescriptor {
public:
    FileDescriptor() noexcept = default;
    explicit FileDescriptor(int fd) noexcept;

    ~FileDescriptor();

    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(FileDescriptor&& other) noexcept;
    FileDescriptor& operator=(FileDescriptor&& other) noexcept;

    // Opens path with the given open(2) flags. Throws std::system_error
    // if open fails. The returned object owns the new descriptor.
    static FileDescriptor open(const std::string& path, int flags);

    int get() const noexcept;
    explicit operator bool() const noexcept;

    // Gives up ownership without closing. The caller must close the result.
    int release() noexcept;

    // Closes the current descriptor, then owns fd. Pass -1 to just close.
    void reset(int fd = -1) noexcept;

private:
    int fd_ = -1;
};

}  // namespace tether
