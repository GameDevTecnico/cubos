#include <cubos/core/log.hpp>
#include <cubos/core/data/file_system.hpp>
#include <cubos/core/data/std_archive.hpp>

#include <cubos/engine/data/asset_manager.hpp>
#include <cubos/engine/data/grid.hpp>


using namespace cubos;
using namespace engine;
using namespace core::data;

class SampleTextLoader;

// Simple asset type which stores a text file's content.
class SampleText
{
public:
    static constexpr const char* TypeName = "SampleText";
    using Loader = SampleTextLoader;

    std::string content;
};

// The class that will load and manage the sample text assets.
class SampleTextLoader : public data::Loader
{
public:
    using data::Loader::Loader;
    virtual const void* load(const data::Meta& meta)
    {
        auto path = meta.getParameters().find("path");
        if (path == meta.getParameters().end())
        {
            core::logError("SampleTextLoader: No path specified");
            return nullptr;
        }

        auto file = FileSystem::find(path->second);
        if (!file)
        {
            core::logError("SampleTextLoader: File not found: {}", path->second);
            return nullptr;
        }

        auto stream = file->open(File::OpenMode::Read);
        if (!stream)
        {
            core::logError("SampleTextLoader: Failed to open file: {}", path->second);
            return nullptr;
        }

        auto asset = new SampleText();
        stream->readUntil(asset->content, nullptr);
        return asset;
    }

    virtual std::future<const void*> loadAsync(const data::Meta& meta)
    {
        return std::async(std::launch::async, [this, &meta] { return load(meta); });
    }

    virtual void unload(const data::Meta& meta, const void* asset)
    {
        auto a = (const SampleText*)asset;
        delete a;
    }
};

int main(int argc, char** argv)
{
    core::initializeLogger();
    FileSystem::mount("/assets/", std::make_shared<STDArchive>(SAMPLE_ASSETS_FOLDER, true, true));

    // Initialize the asset manager and register the asset types.
    data::AssetManager assetManager;
    assetManager.registerType<data::QBModel>();
    assetManager.registerType<data::Grid>();
    assetManager.registerType<SampleText>();

    // Import all asset meta datdirectorys in the assets directory.
    assetManager.importMeta(FileSystem::find("/assets/"));

    auto text = assetManager.load<SampleText>("sample-text");
    if (text)
    {
        core::memory::Stream::stdOut.printf("{}", text->content);
    }

    assetManager.load<data::Grid>("car/grid");
}
