/// @file
/// @brief Sample which showcases how the EmbeddedArchive class can be used.
///
/// @details The `assets` directory on the same directory as this file was embedded into
/// `embed.cpp` using the command `cubinhos embed -r assets > embed.cpp`. Since no further options
/// were specified, `cubinhos` registered the data with the name of the embedded file - `assets`.

#include <cubos/core/data/embedded_archive.hpp>
#include <cubos/core/data/file_system.hpp>

using cubos::core::data::EmbeddedArchive;
using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::memory::Stream;

int main()
{
    // Mount the embedded archive registered as "assets" to the "/assets" directory.
    FileSystem::mount("/assets", std::make_unique<EmbeddedArchive>("assets"));

    // Now we can read the embedded data as if they were normal files.
    std::string contents;
    auto stream = FileSystem::open("/data/foo.txt", File::OpenMode::Read);
    stream->readUntil(contents, nullptr);
    Stream::stdOut.printf("Read \"{}\" from '/data/foo.txt'.\n", contents);

    stream = FileSystem::open("/data/bar/baz.txt", File::OpenMode::Read);
    stream->readUntil(contents, nullptr);
    Stream::stdOut.printf("Read \"{}\" from '/data/bar/baz.txt'.\n", contents);

    FileSystem::unmount("/data");

    return 0;
}
