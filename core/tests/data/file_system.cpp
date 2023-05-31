#include <doctest/doctest.h>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>

#include "utils.hpp"

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::data::STDArchive;
using cubos::core::memory::SeekOrigin;

/// Returns an archive to a temporary location to use for testing.
static std::unique_ptr<STDArchive> genTempArchive(bool directory, std::string name = "")
{
    return std::make_unique<STDArchive>(genTempPath(name), directory, false);
}

/// Runs tests on a file.
static void testFile(std::string_view path)
{
    // Get the path of the parent directory and the name of the file.
    auto parentPath = path.substr(0, path.find_last_of('/'));
    if (parentPath.empty())
    {
        parentPath = "/";
    }
    auto name = path.substr(path.find_last_of('/') + 1);

    // Check if the file exists and has the correct properties.
    auto file = FileSystem::find(path);
    REQUIRE(file != nullptr);
    CHECK(file->name() == name);
    CHECK(file->path() == path);
    CHECK_FALSE(file->directory());
    CHECK(file->archive() != nullptr);
    CHECK(file->parent() == FileSystem::find(parentPath));
    CHECK(file->child() == nullptr);

    // Check if the file can be opened and written to.
    auto stream = FileSystem::open(path, File::OpenMode::Write);
    REQUIRE(stream != nullptr);
    stream->print("bar");
    stream->seek(2, SeekOrigin::Begin);
    stream->print("z");
    CHECK(stream->tell() == 3);

    // Check if the file can be read from.
    stream = FileSystem::open(path, File::OpenMode::Read);
    REQUIRE(stream != nullptr);
    CHECK(stream->peek() == 'b');
    CHECK(dump(*stream) == "baz");
    CHECK(stream->tell() == 3);

    // Make sure that you cannot create a file under a regular file.
    CHECK(FileSystem::create(std::string(path) + "/foo") == nullptr);
}

static void testDirectory(std::string_view path)
{
    // Get the path of the parent directory and the name of the directory.
    auto parentPath = path.substr(0, path.find_last_of('/'));
    if (parentPath.empty())
    {
        parentPath = "/";
    }
    auto name = path.substr(path.find_last_of('/') + 1);

    // Check if the directory exists and has the correct properties.
    auto dir = FileSystem::find(path);
    REQUIRE(dir != nullptr);
    CHECK(dir->name() == name);
    CHECK(dir->path() == path);
    CHECK(dir->directory());
    CHECK(dir->archive() != nullptr);
    CHECK(dir->parent() == FileSystem::find(parentPath));
    REQUIRE(dir->child() == nullptr); // We haven't created any files yet.

    // Directories cannot be opened.
    CHECK(FileSystem::open(path, File::OpenMode::Read) == nullptr);

    // Try creating a file with an invalid path in the directory, and check if anything changed.
    REQUIRE(FileSystem::create(std::string(path) + "/foo//bar") == nullptr);
    REQUIRE(dir->child() == nullptr);

    // Create a new file in the directory.
    auto stream = FileSystem::open(std::string(path) + "/foo", File::OpenMode::Write);
    REQUIRE(stream != nullptr);
    stream = nullptr;

    // Check if the file exists and has the correct properties.
    testFile(std::string(path) + "/foo");

    // Create a new directory in the directory, and a file in that directory.
    REQUIRE(FileSystem::create(std::string(path) + "/bar", true) != nullptr);
    REQUIRE(FileSystem::create(std::string(path) + "/bar/baz") != nullptr);

    // Check if the file exists and has the correct properties.
    testFile(std::string(path) + "/bar/baz");

    // Remove bar.
    REQUIRE(FileSystem::destroy(std::string(path) + "/bar"));
    CHECK(FileSystem::find(std::string(path) + "/bar") == nullptr);
}

TEST_CASE("data::FileSystem")
{
    SUBCASE("without anything mounted")
    {
        // Try executing operations with a non-absolute path.
        CHECK(FileSystem::find("foo") == nullptr);
        CHECK(FileSystem::create("foo") == nullptr);
        CHECK_FALSE(FileSystem::mount("foo", nullptr));
        CHECK_FALSE(FileSystem::unmount("foo"));
        CHECK_FALSE(FileSystem::destroy("foo"));
        CHECK(FileSystem::open("foo", File::OpenMode::Read) == nullptr);

        // Try executing operations on the root with a non-relative path.
        auto root = FileSystem::root();
        REQUIRE(root != nullptr);
        CHECK(root->find("/foo") == nullptr);
        CHECK(root->create("/foo") == nullptr);
        CHECK_FALSE(root->mount("/foo", nullptr));
        CHECK_FALSE(root->unmount("/foo"));

        // Try creating a file outside of an archive.
        CHECK(FileSystem::find("/foo") == nullptr);
        CHECK(FileSystem::create("/foo") == nullptr);
        CHECK(FileSystem::find("/foo") == nullptr); // Create did not do anything.

        // Try executing operations on a file that does not exist.
        CHECK_FALSE(FileSystem::unmount("/foo"));
        CHECK_FALSE(FileSystem::destroy("/foo"));
        CHECK(FileSystem::open("/foo", File::OpenMode::Read) == nullptr);
        CHECK(FileSystem::open("/foo", File::OpenMode::Write) == nullptr);

        // Try mounting an archive to the root.
        REQUIRE_FALSE(FileSystem::mount("/", genTempArchive(true)));

        // Try unmounting the root.
        CHECK_FALSE(FileSystem::unmount("/"));
    }

    SUBCASE("with an archive mounted on a directory")
    {
        REQUIRE(FileSystem::mount("/dir/sub", genTempArchive(true)));

        // An archive cannot be mounted on a already existing file - even if it isn't part of an
        // archive.
        REQUIRE_FALSE(FileSystem::mount("/dir", genTempArchive(false, "failed")));

        // An archive cannot be mounted on the same path as another archive.
        REQUIRE_FALSE(FileSystem::mount("/dir/sub", genTempArchive(true, "failed")));

        // An archive cannot be mounted on a path that is already part of another archive.
        REQUIRE_FALSE(FileSystem::mount("/dir/sub/abc", genTempArchive(true, "failed")));

        // Test operations on the directory.
        testDirectory("/dir/sub");

        SUBCASE("unmounted the only archive beneath /dir")
        {
            // Unmount the archive - this should remove both files from the file system, as there
            // are lno other files below them.
            REQUIRE(FileSystem::unmount("/dir/sub"));
            CHECK(FileSystem::find("/dir") == nullptr);
        }

        SUBCASE("there is another archive beneath /dir")
        {
            // Mount another archive beneath the directory.
            REQUIRE(FileSystem::mount("/dir/other", genTempArchive(false)));

            // Unmount the archive - this should only remove the file from the file system, as there
            // is another file below it.
            REQUIRE(FileSystem::unmount("/dir/sub"));
            CHECK(FileSystem::find("/dir/sub") == nullptr);
            CHECK(FileSystem::find("/dir") != nullptr);

            // Unmount the other archive - now the directory should be removed as well.
            REQUIRE(FileSystem::unmount("/dir/other"));
            CHECK(FileSystem::find("/dir") == nullptr);
        }
    }

    SUBCASE("with an archive mounted on a regular file")
    {
        REQUIRE(FileSystem::mount("/file", genTempArchive(false)));
        REQUIRE_FALSE(FileSystem::destroy("/file"));
        testFile("/file");
        REQUIRE(FileSystem::unmount("/file"));
    }
}
