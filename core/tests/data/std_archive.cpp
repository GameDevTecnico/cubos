#include <fstream>

#include <doctest/doctest.h>

#include <cubos/core/data/std_archive.hpp>

#include "utils.hpp"

using cubos::core::data::File;
using cubos::core::data::STDArchive;
using cubos::core::memory::SeekOrigin;
using cubos::core::memory::Stream;

TEST_CASE("data::STDArchive")
{
    SUBCASE("single file")
    {
        bool readOnly;
        bool preCreateFile;

        SUBCASE("read-only")
        {
            readOnly = true;
            preCreateFile = true;
        }

        SUBCASE("writeable")
        {
            readOnly = false;

            SUBCASE("without pre-existing file")
            {
                preCreateFile = false;
            }

            SUBCASE("with pre-existing file")
            {
                preCreateFile = true;
            }
        }

        // Get a temporary file path.
        auto tmp = std::filesystem::temp_directory_path();
        auto path = tmp / "cubos-core-tests-std-archive-single-file.txt";

        // Pre-create the file if necessary.
        if (preCreateFile)
        {
            std::ofstream file{path};
            file << "ab";
            file.close();
        }
        else
        {
            // Make sure the file does not exist.
            std::filesystem::remove(path);
        }

        STDArchive archive{path, false, readOnly};
        REQUIRE(std::filesystem::exists(path));
        REQUIRE(std::filesystem::is_regular_file(path));

        // Check if the structure is correct.
        CHECK(archive.readOnly() == readOnly);
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

    SUBCASE("multiple files")
    {
        // Get a temporary directory path.
        auto tmp = std::filesystem::temp_directory_path();
        auto path = tmp / "cubos-core-tests-std-archive-multiple-files";

        // Erase any previously existing directory.
        std::filesystem::remove_all(path);

        SUBCASE("generated with std")
        {
            std::filesystem::create_directory(path);
            std::filesystem::create_directory(path / "bar");
            std::ofstream{path / "foo"} << "foo";
            std::ofstream{path / "bar" / "baz"}; // Don't write anything to "baz", but create it.
        }

        SUBCASE("generated with an archive")
        {
            // Check initial structure.
            STDArchive archive{path, true, false};
            CHECK(archive.readOnly() == false);
            CHECK(archive.parent(1) == 0);
            CHECK(archive.sibling(1) == 0);
            CHECK(archive.child(1) == 0);
            REQUIRE(archive.directory(1));
            REQUIRE(std::filesystem::is_directory(path));

            // Add a file "foo" under the root.
            auto foo = archive.create(1, "foo");
            CHECK(archive.child(1) == foo);
            CHECK(archive.parent(foo) == 1);
            CHECK(archive.sibling(foo) == 0);
            CHECK(archive.child(foo) == 0);
            CHECK(archive.name(foo) == "foo");
            REQUIRE_FALSE(archive.directory(foo));
            REQUIRE(std::filesystem::is_regular_file(path / "foo"));

            // Add a directory "bar" under the root.
            auto bar = archive.create(1, "bar", true);
            CHECK(archive.child(1) == bar);
            CHECK(archive.parent(bar) == 1);
            CHECK(archive.sibling(bar) == foo);
            CHECK(archive.child(bar) == 0);
            CHECK(archive.sibling(foo) == 0);
            CHECK(archive.name(bar) == "bar");
            REQUIRE(archive.directory(bar));
            REQUIRE(std::filesystem::is_directory(path / "bar"));

            // Add a file "baz" under "bar".
            auto baz = archive.create(bar, "baz");
            CHECK(archive.child(bar) == baz);
            CHECK(archive.parent(baz) == bar);
            CHECK(archive.sibling(baz) == 0);
            CHECK(archive.child(baz) == 0);
            CHECK(archive.name(baz) == "baz");
            REQUIRE_FALSE(archive.directory(baz));
            REQUIRE(std::filesystem::is_regular_file(path / "bar" / "baz"));

            // Add a file "trash" under "bar".
            auto trash = archive.create(bar, "trash");
            CHECK(archive.child(bar) == trash);
            CHECK(archive.parent(trash) == bar);
            CHECK(archive.sibling(trash) == baz);
            CHECK(archive.child(trash) == 0);
            CHECK(archive.name(trash) == "trash");
            REQUIRE_FALSE(archive.directory(trash));
            REQUIRE(std::filesystem::is_regular_file(path / "bar" / "trash"));

            // Destroy "trash".
            archive.destroy(trash);
            CHECK(archive.child(bar) == baz);
            CHECK(archive.sibling(baz) == 0);
            REQUIRE_FALSE(std::filesystem::exists(path / "bar" / "trash"));

            // Write to "foo" to "foo".
            auto stream = archive.open(foo, nullptr, File::OpenMode::Write);
            stream->print("foo");
            CHECK(stream->tell() == 3);

            // Keep "baz" empty.
        }

        // Create a read-only archive on the generated directory.
        STDArchive archive{path, true, true};

        // Check structure.
        CHECK(archive.readOnly());
        CHECK(archive.parent(1) == 0);
        CHECK(archive.sibling(1) == 0);
        REQUIRE(archive.child(1) != 0);
        REQUIRE(archive.directory(1));
        REQUIRE(archive.sibling(archive.child(1)) != 0);
        REQUIRE(archive.sibling(archive.sibling(archive.child(1))) == 0);

        // Figure out which of the children is which.
        auto child = archive.child(1);
        auto foo = archive.name(child) == "foo" ? child : archive.sibling(child);
        auto bar = archive.name(child) == "bar" ? child : archive.sibling(child);
        REQUIRE(foo != bar);
        REQUIRE_FALSE(archive.directory(foo));
        REQUIRE(archive.directory(bar));

        // Check if "foo" is correct.
        CHECK(archive.parent(foo) == 1);
        if (archive.sibling(foo) != bar)
        {
            CHECK(archive.sibling(foo) == 0);
        }
        CHECK(archive.child(foo) == 0);
        REQUIRE_FALSE(archive.directory(foo));

        // Check if "bar" is correct.
        CHECK(archive.parent(bar) == 1);
        if (archive.sibling(bar) != foo)
        {
            CHECK(archive.sibling(bar) == 0);
        }
        auto baz = archive.child(bar);
        REQUIRE(baz != 0);
        REQUIRE(archive.directory(bar));

        // Check if "baz" is correct.
        CHECK(archive.name(baz) == "baz");
        REQUIRE_FALSE(archive.directory(baz));

        // Check if "foo" has "foo" written on it.
        auto stream = archive.open(foo, nullptr, File::OpenMode::Read);
        CHECK(dump(*stream) == "foo");

        // Check if "baz" is empty.
        stream = archive.open(baz, nullptr, File::OpenMode::Read);
        CHECK(dump(*stream) == "");
    }
}
