#include <cubos/engine/data/asset_manager.hpp>

#include <cubos/core/data/yaml_deserializer.hpp>

using namespace cubos;
using namespace cubos::engine::data;
using namespace core::data;
using namespace core::memory;

AssetManager::Info::Info(Meta&& meta) : meta(std::move(meta))
{
    this->data = nullptr;
    this->refCount = 0;
    this->stored = false;
}

AssetManager::Info::Info(Meta&& meta, const void* data, std::function<void(const void*)> deleter)
    : meta(std::move(meta)), deleter(std::move(deleter))
{
    this->data = data;
    this->refCount = 0;
    this->stored = true;
}

AssetManager::~AssetManager()
{
    for (auto& l : this->loaders)
    {
        delete l.second;
    }
}

void AssetManager::importMeta(File::Handle file)
{
    if (file->isDirectory())
    {
        // Import recursively.
        auto c = file->getChild();
        while (c != nullptr)
        {
            this->importMeta(c);
            c = c->getSibling();
        }
    }
    else if (file->getName().ends_with(".meta"))
    {
        // Open and parse the file.
        auto stream = file->open(File::OpenMode::Read);
        Deserializer* deserializer = new YAMLDeserializer(*stream);
        std::vector<Meta> metas;
        deserializer->read(metas);
        delete deserializer;

        // Add the metas to the asset manager.
        for (auto& meta : metas)
        {
            if (this->infos.find(meta.getId()) != this->infos.end())
            {
                core::logCritical("AssetManager::importMeta(): found asset with duplicate ID '{}' defined in file '{}'",
                                  meta.getId(), file->getPath());
                abort();
            }
            else
            {
                std::string id = meta.getId();
                this->infos.emplace(id, std::move(meta));
                core::logInfo("AssetManager::importMeta(): imported '{}'", id);
            }
        }
    }
}

void AssetManager::loadStatic()
{
    for (auto& it : this->infos)
    {
        if (it.second.meta.getUsage() == Usage::Static)
        {
            std::lock_guard lock(it.second.mutex);
            if (it.second.data == nullptr)
            {
                auto lit = this->loaders.find(it.second.meta.getType());
                if (lit == this->loaders.end())
                {
                    core::logWarning("AssetManager::importMeta(): no loader registered for loading asset of type '{}'",
                                     it.second.meta.getType());
                }
                else
                {
                    it.second.data = lit->second->load(it.second.meta);
                    if (it.second.data == nullptr)
                    {
                        core::logError("AssetManager::loadStatic(): couldn't load '{}'", it.second.meta.getId());
                    }
                    else
                    {
                        core::logInfo("AssetManager::loadStatic(): loaded '{}'", it.second.meta.getId());
                    }
                }
            }
        }
    }
}

void AssetManager::cleanup()
{
    for (auto& it : this->infos)
    {
        if (it.second.meta.getUsage() == Usage::Dynamic)
        {
            std::lock_guard lock(it.second.mutex);
            if (it.second.data != nullptr && it.second.refCount == 0)
            {
                if (it.second.stored)
                {
                    it.second.deleter(it.second.data);
                }
                else
                {
                    auto loader = this->loaders[it.second.meta.getType()];
                    loader->unload(it.second.meta, it.second.data);
                }

                core::logInfo("AssetManager::cleanup(): unloaded '{}'", it.second.meta.getId());
                it.second.data = nullptr;
            }
        }
    }
}

Handle AssetManager::loadAny(const std::string& id)
{
    auto it = this->infos.find(id);
    if (it == this->infos.end())
    {
        core::logError("AssetManager::loadAny(): couldn't load asset '{}' because it wasn't found", id);
        return nullptr;
    }

    // Check if it's loaded and load it if it isn't.
    std::lock_guard lock(it->second.mutex);
    auto& type = it->second.meta.getType();

    if (it->second.data == nullptr)
    {
        auto lit = this->loaders.find(type);
        if (lit == this->loaders.end())
        {
            core::logCritical(
                "AssetManager::loadAny(): couldn't load asset '{}' because the loader for type '{}' wasn't "
                "found",
                id, type);
            abort();
        }

        it->second.data = lit->second->load(it->second.meta);
        if (it->second.data == nullptr)
        {
            core::logError("AssetManager::loadAny(): couldn't load '{}'", it->second.meta.getId());
            return nullptr;
        }
        else
        {
            core::logInfo("AssetManager::loadAny(): loaded '{}'", it->second.meta.getId());
        }
    }

    return Handle(type.c_str(), &it->second.refCount, const_cast<void*>(it->second.data), &it->first);
}
