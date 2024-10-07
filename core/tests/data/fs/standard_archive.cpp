#include <fstream>

#include <doctest/doctest.h>

#include <cubos/core/data/fs/standard_archive.hpp>

#include "../utils.hpp"

using cubos::core::data::File;
using cubos::core::data::StandardArchive;
using cubos::core::memory::SeekOrigin;
using cubos::core::memory::Stream;

/// Asserts that every call to the given archive fails.
/// Used for checking if the archive is handling failed initialization correctly.
static void assertInitializationFailed(StandardArchive& archive)
{
    REQUIRE_FALSE(archive.directory(1)); // Independently of it was created as a directory or not.
    REQUIRE_FALSE(archive.initialized());
    REQUIRE(archive.parent(1) == 0);
    REQUIRE(archive.sibling(1) == 0);
    REQUIRE(archive.child(1) == 0);
    REQUIRE(archive.open(1, nullptr, File::OpenMode::Read) == nullptr);
}

TEST_CASE("data::StandardArchive") // NOLINT(readability-function-size)
{
    auto path = genTempPath();

    SUBCASE("single file archive works correctly")
    {
        bool readOnly = false;
        bool preCreateFile = true;

        SUBCASE("read-only")
        {
            readOnly = true;
        }

        SUBCASE("writeable")
        {
            SUBCASE("with pre-existing file")
            {
            }

            SUBCASE("without pre-existing file")
            {
                preCreateFile = false;
            }
        }

        // Pre-create the file if necessary.
        if (preCreateFile)
        {
            std::ofstream file{path};
            file << "ab";
            file.close();
        }

        StandardArchive archive{path, false, readOnly};
        REQUIRE(std::filesystem::exists(path));
        REQUIRE(std::filesystem::is_regular_file(path));

        // Check if the structure is correct.
        CHECK(archive.readOnly() == readOnly);
        CHECK(archive.initialized());
        CHECK(archive.parent(1) == 0);
        CHECK(archive.sibling(1) == 0);
        CHECK(archive.child(1) == 0);

        if (!preCreateFile)
        {
            // Make sure the file is empty.
            auto stream = archive.open(1, nullptr, File::OpenMode::Read);
            REQUIRE(stream != nullptr);
            CHECK(stream->tell() == 0);
            CHECK_FALSE(stream->eof());
            CHECK(dump(*stream) == "");
            CHECK(stream->tell() == 0);
            CHECK(stream->eof());

            // Write to the file.
            stream = archive.open(1, nullptr, File::OpenMode::Write);
            REQUIRE(stream != nullptr);
            stream->print("c");
            REQUIRE(stream->tell() == 1);
            stream->seek(-1, SeekOrigin::Current);
            stream->print("ab");
            REQUIRE(stream->tell() == 2);
        }

        // Open the file and check its contents.
        auto stream = archive.open(1, nullptr, File::OpenMode::Read);
        REQUIRE(stream != nullptr);
        CHECK(stream->tell() == 0);
        CHECK_FALSE(stream->eof());
        CHECK(dump(*stream) == "ab");
        CHECK(stream->tell() == 2);
        CHECK(stream->eof());

        // Seek back to the the middle of the file and check again.
        stream->seek(1, SeekOrigin::Begin);
        CHECK(stream->tell() == 1);
        CHECK_FALSE(stream->eof());
        CHECK(dump(*stream) == "b");
        CHECK(stream->tell() == 2);
        CHECK(stream->eof());

        if (!readOnly)
        {
            // Change the 'b' to a 'c'.
            stream = archive.open(1, nullptr, File::OpenMode::ReadWrite);
            REQUIRE(stream != nullptr);
            stream->seek(-1, SeekOrigin::End);
            stream->print("c");
            CHECK(stream->tell() == 2);

            // Check the contents again.
            stream->seek(0, SeekOrigin::Begin);
            REQUIRE(stream != nullptr);
            CHECK(stream->tell() == 0);
            CHECK_FALSE(stream->eof());
            CHECK(dump(*stream) == "ac");
            CHECK(stream->tell() == 2);
        }
    }

    SUBCASE("directory archive works correctly")
    {
        std::unique_ptr<StandardArchive> archive = nullptr;
        std::size_t foo = 0;
        std::size_t bar = 0;

        SUBCASE("generated with an archive")
        {
            archive = std::make_unique<StandardArchive>(path, true, false);
            CHECK_FALSE(archive->readOnly());
            CHECK(archive->initialized());

            // Check initial structure.
            CHECK(archive->parent(1) == 0);
            CHECK(archive->sibling(1) == 0);
            CHECK(archive->child(1) == 0);
            REQUIRE(archive->directory(1));
            REQUIRE(std::filesystem::is_directory(path));

            // Add a file "trash" under the root.
            auto trash = archive->create(1, "trash");
            CHECK(archive->child(1) == trash);
            CHECK(archive->parent(trash) == 1);
            CHECK(archive->sibling(trash) == 0);
            CHECK(archive->child(trash) == 0);
            CHECK(archive->name(trash) == "trash");
            REQUIRE_FALSE(archive->directory(trash));

            // Add a file "foo" under the root.
            foo = archive->create(1, "foo");
            CHECK(archive->child(1) == foo);
            CHECK(archive->parent(foo) == 1);
            CHECK(archive->sibling(foo) == trash);
            CHECK(archive->child(foo) == 0);
            CHECK(archive->name(foo) == "foo");
            REQUIRE_FALSE(archive->directory(foo));
            REQUIRE(std::filesystem::is_regular_file(path / "foo"));

            // Remove "trash".
            REQUIRE(archive->destroy(trash));
            REQUIRE(archive->sibling(foo) == 0);

            // Add a directory "bar" under the root.
            bar = archive->create(1, "bar", true);
            CHECK(archive->child(1) == bar);
            CHECK(archive->parent(bar) == 1);
            CHECK(archive->sibling(bar) == foo);
            CHECK(archive->child(bar) == 0);
            CHECK(archive->sibling(foo) == 0);
            CHECK(archive->name(bar) == "bar");
            REQUIRE(archive->directory(bar));
            REQUIRE(std::filesystem::is_directory(path / "bar"));

            // Add a file "baz" under "bar".
            auto baz = archive->create(bar, "baz");
            CHECK(archive->child(bar) == baz);
            CHECK(archive->parent(baz) == bar);
            CHECK(archive->sibling(baz) == 0);
            CHECK(archive->child(baz) == 0);
            CHECK(archive->name(baz) == "baz");
            REQUIRE_FALSE(archive->directory(baz));
            REQUIRE(std::filesystem::is_regular_file(path / "bar" / "baz"));

            // Add a file "trash" under "bar".
            trash = archive->create(bar, "trash");
            CHECK(archive->child(bar) == trash);
            CHECK(archive->parent(trash) == bar);
            CHECK(archive->sibling(trash) == baz);
            CHECK(archive->child(trash) == 0);
            CHECK(archive->name(trash) == "trash");
            REQUIRE_FALSE(archive->directory(trash));
            REQUIRE(std::filesystem::is_regular_file(path / "bar" / "trash"));

            // Destroy "trash".
            REQUIRE(archive->destroy(trash));
            CHECK(archive->child(bar) == baz);
            CHECK(archive->sibling(baz) == 0);
            REQUIRE_FALSE(std::filesystem::exists(path / "bar" / "trash"));

            // Write to "foo" to "foo".
            auto stream = archive->open(foo, nullptr, File::OpenMode::Write);
            stream->print("foo");
            CHECK(stream->tell() == 3);

            // Keep "baz" empty.
        }

        SUBCASE("generated with std")
        {
            std::filesystem::create_directory(path);
            std::filesystem::create_directory(path / "bar");
            std::ofstream{path / "foo"} << "foo";
            // NOLINTNEXTLINE(bugprone-unused-raii)
            std::ofstream{path / "bar" / "baz"}; // Don't write anything to "baz", but create it.

            // Create a read-only archive on the generated directory.
            archive = std::make_unique<StandardArchive>(path, true, true);
            CHECK(archive->readOnly());
            CHECK(archive->initialized());

            // Check root.
            CHECK(archive->directory(1));
            CHECK(archive->parent(1) == 0);
            CHECK(archive->sibling(1) == 0);
            REQUIRE(archive->child(1) != 0);
            REQUIRE(archive->sibling(archive->child(1)) != 0);
            REQUIRE(archive->sibling(archive->sibling(archive->child(1))) == 0); // 2 children.

            // Figure out which of the children of the root is which.
            if (archive->name(archive->child(1)) == "foo")
            {
                foo = archive->child(1);
                bar = archive->sibling(foo);
            }
            else
            {
                bar = archive->child(1);
                foo = archive->sibling(bar);
            }
        }

        // Check if "foo" is correct.
        REQUIRE_FALSE(archive->directory(foo));
        CHECK(archive->parent(foo) == 1);
        CHECK(archive->child(foo) == 0);
        CHECK(archive->name(foo) == "foo");

        // Check if "bar" is correct.
        REQUIRE(archive->directory(bar));
        CHECK(archive->parent(bar) == 1);
        auto baz = archive->child(bar);
        CHECK(baz != 0);
        CHECK(archive->name(bar) == "bar");

        // Check if "baz" is correct.
        REQUIRE_FALSE(archive->directory(baz));
        CHECK(archive->parent(baz) == bar);
        CHECK(archive->sibling(baz) == 0);
        CHECK(archive->child(baz) == 0);
        CHECK(archive->name(baz) == "baz");

        // Check if "foo" has "foo" written on it.
        auto stream = archive->open(foo, nullptr, File::OpenMode::Read);
        CHECK(stream->peek() == 'f');
        CHECK(dump(*stream) == "foo");

        // Check if "baz" is empty.
        stream = archive->open(baz, nullptr, File::OpenMode::Read);
        CHECK(dump(*stream) == "");
    }

    SUBCASE("read-only archive on non existing file fails")
    {
        bool wantedDir = false;
        PARAMETRIZE_TRUE_OR_FALSE("wanted directory", wantedDir);

        // Initialization will fail since 'path' doesn't exist and this is read-only.
        StandardArchive archive{path, wantedDir, true};
        REQUIRE(archive.readOnly());
        assertInitializationFailed(archive);
    }

    SUBCASE("creating an archive on a directory which does not exist fails")
    {
        bool wantedDir = false;
        PARAMETRIZE_TRUE_OR_FALSE("wanted directory", wantedDir);

        // Initialization will fail since 'path' doesn't exist.
        StandardArchive archive{path / "archive", wantedDir, false};
        REQUIRE_FALSE(archive.readOnly());
        assertInitializationFailed(archive);
    }

    SUBCASE("creating an archive on a file which doesn't match the expected type")
    {
        // If we're expecting a directory, create a regular file and vice versa.
        bool wantedDir = false;
        PARAMETRIZE_TRUE_OR_FALSE("wanted directory", wantedDir);
        if (wantedDir)
        {
            std::ofstream{path}; // Create regular file.
        }
        else
        {
            std::filesystem::create_directory(path); // Create directory.
        }

        // Initialization will fail since 'path' is not the expected type.
        StandardArchive archive{path, wantedDir, true};
        REQUIRE(archive.readOnly());
        assertInitializationFailed(archive);
    }
}
