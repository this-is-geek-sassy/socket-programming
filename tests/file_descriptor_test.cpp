#include "net/file_descriptor.hpp"

#include <fcntl.h>

#include <cerrno>
#include <system_error>
#include <utility>

#include <gtest/gtest.h>

TEST(FileDescriptor, OpenDevNullIsValid) {
    const tether::FileDescriptor fd = tether::FileDescriptor::open("/dev/null", O_RDONLY);
    EXPECT_TRUE(fd);
    EXPECT_GE(fd.get(), 0);
}

TEST(FileDescriptor, MoveLeavesSourceEmpty) {
    tether::FileDescriptor source = tether::FileDescriptor::open("/dev/null", O_RDONLY);
    const int raw = source.get();

    const tether::FileDescriptor destination(std::move(source));

    EXPECT_FALSE(source);
    EXPECT_EQ(source.get(), -1);
    EXPECT_TRUE(destination);
    EXPECT_EQ(destination.get(), raw);
}

TEST(FileDescriptor, DestructorClosesDescriptor) {
    int raw = -1;
    {
        tether::FileDescriptor fd = tether::FileDescriptor::open("/dev/null", O_RDONLY);
        raw = fd.get();
    }

    // F_GETFD fails with EBADF once the destructor has closed the number.
    errno = 0;
    EXPECT_EQ(::fcntl(raw, F_GETFD), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(FileDescriptor, MissingPathThrowsSystemError) {
    try {
        tether::FileDescriptor::open("/no/such/tether/path", O_RDONLY);
        FAIL() << "open of a missing path must throw";
    } catch (const std::system_error& error) {
        EXPECT_EQ(error.code().value(), ENOENT);
    }
}
