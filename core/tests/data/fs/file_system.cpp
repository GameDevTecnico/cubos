#include <cstring>

#include <doctest/doctest.h>

#include <cubos/core/data/fs/archive.hpp>
#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/tel/logging.hpp>

#include "../utils.hpp"

using cubos::core::data::Archive;
using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::memory::BufferStream;
using cubos::core::memory::SeekOrigin;
using cubos::core::memory::Stream;
using cubos::core::tel::Logger;

/// Mock implementation of the Archive interface class.
class MockArchive : public Archive
{
public:
    MOCK_OVERRIDE(std::size_t, create, std::size_t, std::string_view, bool)
    MOCK_OVERRIDE(bool, destroy, std::size_t)
    MOCK_CONST_OVERRIDE(std::string, name, std::size_t)
    MOCK_CONST_OVERRIDE(bool, directory, std::size_t)
    MOCK_CONST_OVERRIDE(bool, readOnly)
    MOCK_CONST_OVERRIDE(std::size_t, parent, std::size_t)
    MOCK_CONST_OVERRIDE(std::size_t, sibling, std::size_t)
    MOCK_CONST_OVERRIDE(std::size_t, child, std::size_t)
    MOCK_OVERRIDE(std::unique_ptr<Stream>, open, std::size_t, File::Handle, File::OpenMode)
};

/// Returns a dummy Archive which fails on all operations.
inline auto dummyArchive()
{
    return std::make_unique<MockArchive>();
}

/// Returns a mocked archive with the given read-only state.
inline auto mockArchive(bool readOnly)
{
    auto archive = dummyArchive();
    archive->readOnlyWhen = [readOnly]() { return readOnly; };
    return archive;
}

/// Resets the file system to its initial state.
inline void unmountAll(const File::Handle& handle)
{
    while (handle->child() != nullptr)
    {
        if (handle->child()->archive() != nullptr)
        {
            handle->unmount(handle->child()->name());
        }
        else
        {
            unmountAll(handle->child());
        }
    }
}

TEST_CASE("data::FileSystem") // NOLINT(readability-function-size)
{
    Logger::level(Logger::Level::Critical);
    unmountAll(FileSystem::root());

    SUBCASE("without anything mounted")
    {
        // Initially the root directory exists and is empty.
        auto root = FileSystem::root();
        REQUIRE(FileSystem::find("/") == root);
        REQUIRE(root->name() == "");
        REQUIRE(root->path() == "");
        REQUIRE(root->directory());
        REQUIRE(root->archive() == nullptr);
        REQUIRE(root->id() == 0);
        REQUIRE(root->parent() == nullptr);
        REQUIRE(root->sibling() == nullptr);
        REQUIRE(root->child() == nullptr);

        // It cannot be destroyed.
        REQUIRE_FALSE(root->destroy());
        REQUIRE_FALSE(FileSystem::destroy("/"));

        // Files cannot be created under it (files must be created under archives).
        REQUIRE_FALSE(root->create("foo", false));
        REQUIRE_FALSE(FileSystem::create("/foo", true));
        REQUIRE(FileSystem::find("/foo") == nullptr);

        // Cannot create files on non-absolute paths.
        REQUIRE_FALSE(FileSystem::create("foo", false));

        // Cannot destroy files that do not exist.
        REQUIRE_FALSE(FileSystem::destroy("/foo"));

        // Cannot destroy files on non-absolute paths.
        REQUIRE_FALSE(FileSystem::destroy("foo"));

        // Cannot open invalid paths, the root or files that do not exist.
        REQUIRE(FileSystem::open("", File::OpenMode::Read) == nullptr);
        REQUIRE(FileSystem::open("foo/", File::OpenMode::Read) == nullptr);
        REQUIRE(FileSystem::open("/", File::OpenMode::Read) == nullptr);
        REQUIRE(FileSystem::open("/foo", File::OpenMode::Read) == nullptr);

        // Cannot open and create files on paths which are not within archives.
        REQUIRE(FileSystem::open("/foo", File::OpenMode::Write) == nullptr);

        // Cannot mount archives on invalid paths - nothing should change.
        REQUIRE_FALSE(FileSystem::mount("", dummyArchive()));
        REQUIRE_FALSE(FileSystem::mount("/foo//bar", dummyArchive()));
        REQUIRE(FileSystem::find("/foo") == nullptr);

        // Cannot mount archives on the root file.
        REQUIRE_FALSE(FileSystem::mount("/", dummyArchive()));

        // Cannot unmount archives from invalid paths or non-mount points.
        REQUIRE_FALSE(FileSystem::unmount(""));
        REQUIRE_FALSE(FileSystem::unmount("/"));
        REQUIRE_FALSE(FileSystem::unmount("/foo"));
    }

    SUBCASE("with an archive mounted as a regular file")
    {
        bool readOnly = false;
        PARAMETRIZE_TRUE_OR_FALSE("read-only", readOnly);

        auto root = FileSystem::root();

        // Mount a single file archive on /foo.
        char buf[5] = "text";
        auto archive = mockArchive(readOnly);
        archive->directoryWhen = [](auto) { return false; };
        archive->childWhen = [](auto) { return 0; };
        archive->openWhen = [&buf](auto, auto, auto) { return std::make_unique<BufferStream>(buf, 4); };
        REQUIRE(FileSystem::mount("/foo", std::move(archive)));

        // Mounting another archive on the same path should fail.
        REQUIRE_FALSE(FileSystem::mount("/foo", dummyArchive()));

        // To get the file we must use an absolute and correct path.
        REQUIRE(FileSystem::find("foo") == nullptr);
        REQUIRE(FileSystem::find("/fo") == nullptr);
        REQUIRE(FileSystem::find("/foo/") == nullptr);

        // The archive should be mounted on /foo.
        auto foo = FileSystem::find("/foo");
        REQUIRE(foo != nullptr);
        REQUIRE(foo->name() == "foo");
        REQUIRE(foo->path() == "/foo");
        REQUIRE_FALSE(foo->directory());
        REQUIRE(foo->archive() != nullptr);
        REQUIRE(foo->id() == 1);
        REQUIRE(foo->parent() == root);
        REQUIRE(foo->sibling() == nullptr);
        REQUIRE(foo->child() == nullptr);

        // File can be read.
        auto stream = FileSystem::open("/foo", File::OpenMode::Read);
        REQUIRE(stream != nullptr);
        REQUIRE(dump(*stream) == "text");
        stream = nullptr; // Close the file.

        if (readOnly)
        {
            // But not written.
            REQUIRE(foo->open(File::OpenMode::Write) == nullptr);
            REQUIRE(foo->open(File::OpenMode::ReadWrite) == nullptr);
        }
        else
        {
            // And written.
            stream = foo->open(File::OpenMode::Write);
            REQUIRE(stream != nullptr);
            stream->print("abcd");
            stream = nullptr; // Close the file.

            stream = FileSystem::open("/foo", File::OpenMode::ReadWrite);
            REQUIRE(stream != nullptr);
            REQUIRE(dump(*stream) == "abcd");
            stream->seek(0, SeekOrigin::Begin);
            stream->print("text");
            stream = nullptr; // Close the file.
        }

        // Creating a file with the same name and type should return the same file.
        REQUIRE(FileSystem::create("/foo", false) == foo);

        // Creating a file with the same name but different type should fail.
        REQUIRE(FileSystem::create("/foo", true) == nullptr);

        // Cannot create a file under a regular file.
        REQUIRE(FileSystem::create("/foo/bar", false) == nullptr);

        // Cannot unmount the archive with an invalid path or non-mount point.
        REQUIRE_FALSE(FileSystem::unmount("/"));
        REQUIRE_FALSE(FileSystem::unmount("/fo"));
        REQUIRE_FALSE(FileSystem::unmount("/foo/"));
        REQUIRE_FALSE(FileSystem::unmount("/foo/bar"));

        // File cannot be destroyed, but can be unmounted.
        REQUIRE_FALSE(foo->destroy());
        REQUIRE_FALSE(FileSystem::destroy("/foo"));
        REQUIRE(FileSystem::unmount("/foo"));
        REQUIRE(FileSystem::find("/foo") == nullptr);
        REQUIRE(root->child() == nullptr);
        REQUIRE(foo->parent() == nullptr);

        // Even after unmounting, since there is still a handle to the file, it should still be readable.
        // However, it will not be out of the tree.
        stream = foo->open(File::OpenMode::Read);
        REQUIRE(stream != nullptr);
        REQUIRE(dump(*stream) == "text");
        stream = nullptr; // Close the file.

        foo = nullptr; // Only now is the file really inaccessible.
    }

    SUBCASE("with an archive mounted as a directory")
    {
        bool readOnly = false;
        PARAMETRIZE_TRUE_OR_FALSE("read-only", readOnly);

        // Prepare a mock archive with the following structure:
        // 1: /        (directory)
        // 2: /bar     (directory)
        // 3: /foo     (file)
        // 4: /bar/baz (file)
        bool directories[] = {true, true, false, false, false};
        std::string names[] = {"", "bar", "foo", "baz", "qux", "newbaz"};
        std::size_t parents[] = {0, 1, 1, 2, 2, 2};
        std::size_t siblings[] = {0, 3, 0, 0, 4, 5};
        std::size_t children[] = {2, 4, 0, 0, 0, 0};

        char buffers[7][4096];       // file data buffers
        bool fillFileBuffers = true; // flag to not generate file data buffers to test if ::copy works

        auto archive = mockArchive(readOnly);
        archive->directoryWhen = [directories](std::size_t id) { return directories[id - 1]; };
        archive->nameWhen = [names](std::size_t id) { return names[id - 1]; };
        archive->parentWhen = [parents](std::size_t id) { return parents[id - 1]; };
        archive->siblingWhen = [siblings](std::size_t id) { return siblings[id - 1]; };
        archive->childWhen = [children](std::size_t id) { return children[id - 1]; };
        archive->createWhen = [&children](std::size_t parent, std::string_view name, bool directory) {
            // Should only be called for creating "qux" under "bar"
            if (name == "qux")
            {
                REQUIRE(parent == 2);
                REQUIRE_FALSE(directory);
                children[2] = 5;
                return 5;
            }

            REQUIRE(name == "newbaz");
            REQUIRE(parent == 2);
            REQUIRE_FALSE(directory);
            children[2] = 6;
            return 6;
        };
        archive->openWhen = [&buffers, &fillFileBuffers](std::size_t id, auto, auto) {
            bool correctId = id == 6 || id == 5 || id == 4;
            REQUIRE(correctId);

            // Fill file with chunks of data only if requested
            if (fillFileBuffers)
            {
                for (int i = 0; i < 4095; i++)
                {
                    buffers[id][i] = std::to_string(id).c_str()[0]; // ¯\_(ツ)_/ ...
                }
                buffers[id][4095] = '\0';
            }

            return std::make_unique<BufferStream>(buffers[id], 4096);
        };
        archive->destroyWhen = [](auto) { return true; };

        // Mount it on /dir/sub/arc
        REQUIRE(FileSystem::mount("/dir/sub/arc", std::move(archive)));
        REQUIRE(FileSystem::find("/dir/sub") != nullptr);
        REQUIRE(FileSystem::find("/dir/sub")->archive() == nullptr);
        REQUIRE(FileSystem::find("/dir/sub")->directory());

        // The directory exists and is not empty.
        auto arc = FileSystem::find("/dir/sub/arc");
        REQUIRE(arc != nullptr);
        REQUIRE(arc->name() == "arc");
        REQUIRE(arc->path() == "/dir/sub/arc");
        REQUIRE(arc->directory());
        REQUIRE(arc->archive() != nullptr);
        REQUIRE(arc->id() == 1);
        REQUIRE(arc->parent() == FileSystem::find("/dir/sub"));
        REQUIRE(arc->sibling() == nullptr);
        auto bar = arc->child();
        REQUIRE(bar != nullptr);

        // Figure out which one is really "bar" and which one is "foo".
        auto foo = bar->sibling();
        REQUIRE(foo != nullptr);
        REQUIRE(foo->sibling() == nullptr);
        if (bar->name() == "foo")
        {
            std::swap(bar, foo);
        }

        // "bar" is a directory and contains a file.
        REQUIRE(bar->name() == "bar");
        REQUIRE(bar->path() == "/dir/sub/arc/bar");
        REQUIRE(bar->directory());
        REQUIRE(bar->archive() != nullptr);
        REQUIRE(bar->id() == 2);
        REQUIRE(bar->parent() == arc);
        auto baz = bar->child();
        REQUIRE(baz != nullptr);

        // "foo" is a file.
        REQUIRE(foo->name() == "foo");
        REQUIRE(foo->path() == "/dir/sub/arc/foo");
        REQUIRE_FALSE(foo->directory());
        REQUIRE(foo->archive() != nullptr);
        REQUIRE(foo->id() == 3);
        REQUIRE(foo->parent() == arc);
        REQUIRE(foo->child() == nullptr);

        // "baz" is a file.
        REQUIRE(baz->name() == "baz");
        REQUIRE(baz->path() == "/dir/sub/arc/bar/baz");
        REQUIRE_FALSE(baz->directory());
        REQUIRE(baz->archive() != nullptr);
        REQUIRE(baz->id() == 4);
        REQUIRE(baz->parent() == bar);
        REQUIRE(baz->sibling() == nullptr);
        REQUIRE(baz->child() == nullptr);

        if (readOnly)
        {
            // Cannot create a file under a read-only archive.
            REQUIRE(FileSystem::create("/dir/sub/arc/bar/qux", false) == nullptr);
            REQUIRE(FileSystem::open("/dir/sub/arc/bar/qux", File::OpenMode::Write) == nullptr);

            // Cannot overwrite a read-only file
            REQUIRE_FALSE(FileSystem::copy(baz->path(), "/dir/sub/arc/bar/qux"));
        }
        else
        {
            // Test copying files
            REQUIRE_FALSE(FileSystem::copy("/dir/sub/arc/bar/nonexistent", "/dir/sub/arc/bar/newfiledoesntmatter"));

            // Test if correct content was copied.
            {
                REQUIRE(FileSystem::copy(baz->path(), "/dir/sub/arc/bar/newbaz"));

                fillFileBuffers = false;

                auto stream = FileSystem::open("/dir/sub/arc/bar/newbaz", File::OpenMode::Read);
                REQUIRE(stream != nullptr);

                std::string result = dump(*stream);
                REQUIRE(result.size() == strlen(buffers[baz->id()]));
                REQUIRE(result == buffers[baz->id()]);

                fillFileBuffers = true;
            }

            // Cannot create a file with an invalid path.
            REQUIRE(bar->create("/qux", false) == nullptr);

            // Open "qux" for writing under "bar", creating it.
            REQUIRE(FileSystem::open("/dir/sub/arc/bar/qux", File::OpenMode::Write) != nullptr);

            // Check that "qux" was created correctly.
            auto qux = FileSystem::find("/dir/sub/arc/bar/qux");
            REQUIRE(qux != nullptr);
            REQUIRE(qux->name() == "qux");
            REQUIRE(qux->path() == "/dir/sub/arc/bar/qux");
            REQUIRE_FALSE(qux->directory());
            REQUIRE(qux->archive() != nullptr);
            REQUIRE(qux->id() == 5);
            REQUIRE(qux->parent() == bar);
            REQUIRE(qux->child() == nullptr);
            REQUIRE(bar->child() == qux);

            // Destroy "bar" and check that "qux" and "baz" are gone.
            REQUIRE(FileSystem::destroy("/dir/sub/arc/bar"));
            REQUIRE(FileSystem::find("/dir/sub/arc/bar") == nullptr);
            REQUIRE(FileSystem::find("/dir/sub/arc/bar/baz") == nullptr);
            REQUIRE(FileSystem::find("/dir/sub/arc/bar/qux") == nullptr);
            REQUIRE(bar->destroy()); // Should be a no-op.

            // Check that "qux" can still be accessed.
            REQUIRE(qux->open(File::OpenMode::Read) != nullptr);
        }

        // Mount another archive under "/dir".
        archive = mockArchive(readOnly);
        archive->directoryWhen = [](auto) { return false; };
        archive->childWhen = [](auto) { return 0; };
        REQUIRE(FileSystem::mount("/dir/other", std::move(archive)));

        // Try to unmount the archive not from its root.
        REQUIRE_FALSE(FileSystem::unmount("/dir/sub/arc/bar"));

        // Unmount the archive.
        REQUIRE(FileSystem::unmount("/dir/sub/arc"));

        // "/dir/sub" should no longer exist, but "/dir" should.
        REQUIRE(FileSystem::find("/dir/sub") == nullptr);
        REQUIRE(FileSystem::find("/dir") != nullptr);

        // After unmounting, "/dir/other", "/dir" should no longer exist.
        REQUIRE(FileSystem::unmount("/dir/other"));
        REQUIRE(FileSystem::find("/dir") == nullptr);
        REQUIRE(FileSystem::root()->child() == nullptr);
    }
}
