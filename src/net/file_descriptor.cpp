#include "net/file_descriptor.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>
#include <utility>

namespace tether {

FileDescriptor::FileDescriptor(int fd) noexcept : fd_(fd) {}

FileDescriptor::~FileDescriptor() {
    reset();
}

FileDescriptor::FileDescriptor(FileDescriptor&& other) noexcept
    : fd_(std::exchange(other.fd_, -1)) {}

FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) noexcept {
    // Self-move is a no-op. Closing fd_ and then reading it back from
    // other would drop the only descriptor this object owns.
    if (this != &other) {
        reset(std::exchange(other.fd_, -1));
    }
    return *this;
}

FileDescriptor FileDescriptor::open(const std::string& path, int flags) {
    const int fd = ::open(path.c_str(), flags);
    if (fd < 0) {
        // errno is read here, before any other call can overwrite it.
        throw std::system_error(errno, std::generic_category(), "open " + path);
    }
    return FileDescriptor(fd);
}

int FileDescriptor::get() const noexcept {
    return fd_;
}

FileDescriptor::operator bool() const noexcept {
    return fd_ >= 0;
}

int FileDescriptor::release() noexcept {
    return std::exchange(fd_, -1);
}

void FileDescriptor::reset(int fd) noexcept {
    if (fd_ >= 0) {
        // On Linux, close() drops the descriptor even when it returns
        // EINTR. Retrying would risk closing a number the kernel has
        // already handed to someone else. Destructors must not throw.
        ::close(fd_);
    }
    fd_ = fd;
}

}  // namespace tether
