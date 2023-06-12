#include <filesystem>

#include <cubos/core/data/fs/file_system.hpp>
#include <cubos/core/data/fs/standard_archive.hpp>
#include <cubos/core/memory/stream.hpp>

using cubos::core::data::File;
using cubos::core::data::FileSystem;
using cubos::core::data::StandardArchive;
using cubos::core::memory::Stream;

static void wait()
{
    Stream::stdOut.printf("Press enter to continue...");
    Stream::stdIn.get();
}

int main()
{
    Stream::stdOut.printf("This sample showcases the standard archive and how it can be used with the file system.\n");

    // Get a path to a temporary directory for the sample.
    auto path = std::filesystem::temp_directory_path() / "standard_archive_sample";
    std::filesystem::remove_all(path); // Make sure it does not exist.

    FileSystem::mount("/my/archive", std::make_unique<StandardArchive>(path, /*isDirectory=*/true, /*readOnly=*/false));
    Stream::stdOut.printf("Mounted standard archive at '{}'\n", path.string());
    wait();

    Stream::stdOut.print("Creating file '/my/archive/bar/foo.txt'...\n");
    auto file = FileSystem::create("/my/archive/bar/foo.txt", /*directory=*/false);
    Stream::stdOut.printf("An empty file was created at '{}'\n", (path / "bar" / "foo.txt").string());
    wait();

    file->open(File::OpenMode::Write)->print("Hello, world!");
    Stream::stdOut.print("Wrote 'Hello, world!' to '/my/archive/bar/foo.txt'...\n");
    wait();

    Stream::stdOut.print("Destroying file '/my/archive/bar/foo.txt'...\n");
    file->destroy();
    file = nullptr; // The file is only destroyed when all handles to it are dropped.
    Stream::stdOut.printf("The file at '{}' was destroyed.\n", (path / "bar" / "foo.txt").string());
    wait();

    return 0;
}
