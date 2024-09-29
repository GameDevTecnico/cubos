#include <doctest/doctest.h>

#include <cubos/core/data/fs/embedded_archive.hpp>
#include <cubos/core/tel/logging.hpp>

#include "../utils.hpp"

using cubos::core::data::EmbeddedArchive;
using cubos::core::data::File;
using cubos::core::memory::SeekOrigin;
using cubos::core::memory::Stream;
using cubos::core::tel::Level;
using cubos::core::tel::Logger;

// Fear not - we aren't supposed do this manually - usually quadrados would do this for us.

// Embedded data with a single regular file which has the data "ab".
static const EmbeddedArchive::Data::Entry SingleEntry{"root", false, 0, 0, 0, "ab", 2};
static const EmbeddedArchive::Data SingleData = {&SingleEntry, 1};

// Embedded data with four files:
// - root directory
//   - foo "foo"
//   - bar directory
//     - baz "" (empty file)
static const EmbeddedArchive::Data::Entry MultipleEntries[] = {
    {"root", true, 0, 0, 2, nullptr, 0},
    {"foo", false, 1, 3, 0, "foo", 3},
    {"bar", true, 1, 0, 4, nullptr, 0},
    {"baz", false, 3, 0, 0, nullptr, 0},
};
static const EmbeddedArchive::Data MultipleData = {MultipleEntries,
                                                   sizeof(MultipleEntries) / sizeof(MultipleEntries[0])};

TEST_CASE("data::EmbeddedArchive")
{
    cubos::core::tel::level(Level::Critical);

    // Must use static since there are multiple subcases.
    static bool registered = false;
    if (!registered)
    {
        // Register embedded data so that we can mount it later.
        registered = true;
        EmbeddedArchive::registerData("single", SingleData);
        EmbeddedArchive::registerData("multiple", MultipleData);
    }

    SUBCASE("single file")
    {
        EmbeddedArchive archive{"single"};

        // Check if the structure is correct.
        REQUIRE(archive.readOnly());

        REQUIRE_FALSE(archive.directory(1));
        REQUIRE(archive.parent(1) == 0);
        REQUIRE(archive.sibling(1) == 0);
        REQUIRE(archive.child(1) == 0);

        // Open the file and check its contents.
        auto stream = archive.open(1, nullptr, File::OpenMode::Read);
        REQUIRE(stream != nullptr);
        CHECK(stream->tell() == 0);
        CHECK_FALSE(stream->eof());
        CHECK(dump(*stream) == "ab");
        CHECK(stream->tell() == 2);
        CHECK(stream->eof());

        // Seek to the middle of the file and check its contents.
        stream->seek(1, SeekOrigin::Begin);
        CHECK(stream->tell() == 1);
        CHECK_FALSE(stream->eof());
        CHECK(dump(*stream) == "b");
        CHECK(stream->tell() == 2);
        CHECK(stream->eof());
    }

    SUBCASE("multiple files")
    {
        EmbeddedArchive archive{"multiple"};

        // Check if the structure is correct.
        REQUIRE(archive.readOnly());

        REQUIRE(archive.directory(1));
        REQUIRE(archive.parent(1) == 0);
        REQUIRE(archive.sibling(1) == 0);
        REQUIRE(archive.child(1) == 2);

        REQUIRE_FALSE(archive.directory(2));
        REQUIRE(archive.parent(2) == 1);
        REQUIRE(archive.sibling(2) == 3);
        REQUIRE(archive.child(2) == 0);
        REQUIRE(archive.name(2) == "foo");

        REQUIRE(archive.directory(3));
        REQUIRE(archive.parent(3) == 1);
        REQUIRE(archive.sibling(3) == 0);
        REQUIRE(archive.child(3) == 4);
        REQUIRE(archive.name(3) == "bar");

        REQUIRE_FALSE(archive.directory(4));
        REQUIRE(archive.parent(4) == 3);
        REQUIRE(archive.sibling(4) == 0);
        REQUIRE(archive.child(4) == 0);
        REQUIRE(archive.name(4) == "baz");

        // Open the files and check their contents.
        auto foo = archive.open(2, nullptr, File::OpenMode::Read);
        REQUIRE(foo != nullptr);
        CHECK(foo->tell() == 0);
        CHECK_FALSE(foo->eof());
        CHECK(dump(*foo) == "foo");
        CHECK(foo->tell() == 3);
        CHECK(foo->eof());

        auto baz = archive.open(4, nullptr, File::OpenMode::Read);
        REQUIRE(baz != nullptr);
        CHECK(baz->tell() == 0);
        CHECK_FALSE(baz->eof());
        CHECK(dump(*baz) == "");
        CHECK(baz->tell() == 0);
        CHECK(baz->eof());
    }
}
