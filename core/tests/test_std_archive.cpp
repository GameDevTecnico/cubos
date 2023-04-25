#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>

using namespace cubos::core::data;

TEST(Cubos_Std_Archive_Tests, Read_Directory)
{
    using namespace cubos::core::data;

    // Create a temporary directory.
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "cubos_std_archive_tests";
    std::filesystem::remove_all(tempDir);
    std::filesystem::create_directory(tempDir);

    // Create a file in the directory.
    std::filesystem::path filePath = tempDir / "file.txt";
    std::ofstream file(filePath);
    file << "Hello world!";
    file.close();

    // Create a directory in the directory.
    std::filesystem::path dirPath = tempDir / "dir";
    std::filesystem::create_directory(dirPath);

    // Mount the temporary directory as a STDArchive.
    FileSystem::mount("///tmp//", std::make_shared<STDArchive>(tempDir, true, false));

    // Find the file.
    File::Handle fileHandle = FileSystem::find("///tmp//file.txt");
    ASSERT_NE(fileHandle, nullptr);
    ASSERT_EQ(fileHandle->getPath(), "/tmp/file.txt");

    // Find the directory.
    File::Handle dirHandle = FileSystem::find("///tmp//dir");
    ASSERT_NE(dirHandle, nullptr);
    ASSERT_EQ(dirHandle->getPath(), "/tmp/dir");

    // Create another file in the directory.
    File::Handle file2Handle = dirHandle->create("file2.txt");
    ASSERT_NE(file2Handle, nullptr);
    ASSERT_EQ(file2Handle->getPath(), "/tmp/dir/file2.txt");

    // Write some data to the file.
    {
        auto stream = file2Handle->open(File::OpenMode::Write);
        ASSERT_NE(stream, nullptr);
        stream->print("Hello file2!");
    }

    // Read from the file.
    {
        auto stream = fileHandle->open(File::OpenMode::Read);
        ASSERT_NE(stream, nullptr);
        std::string content;
        stream->readUntil(content, nullptr);
        ASSERT_EQ(content, "Hello world!");
    }

    // Unmount the archive.
    FileSystem::unmount("///tmp//");

    // Check data in file2.txt
    {
        std::ifstream file(dirPath / "file2.txt");
        std::string content;
        std::getline(file, content);
        ASSERT_EQ(content, "Hello file2!");
    }
}
