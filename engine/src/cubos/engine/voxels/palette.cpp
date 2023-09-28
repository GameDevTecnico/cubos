#include <cstring>

#include <cubos/core/log.hpp>

#include <cubos/engine/voxels/palette.hpp>

using namespace cubos::engine;

VoxelPalette::VoxelPalette(std::vector<VoxelMaterial>&& materials)
    : mMaterials(std::move(materials))
{
}

const VoxelMaterial* VoxelPalette::data() const
{
    return mMaterials.data();
}

uint16_t VoxelPalette::size() const
{
    return static_cast<uint16_t>(mMaterials.size());
}

const VoxelMaterial& VoxelPalette::get(uint16_t index) const
{
    if (index == 0 || index > static_cast<uint16_t>(mMaterials.size()))
    {
        return VoxelMaterial::Empty;
    }
    return mMaterials[index - 1];
}

void VoxelPalette::set(uint16_t index, const VoxelMaterial& material)
{
    if (index == 0)
    {
        CUBOS_ERROR("Cannot set a palette material at index 0: reserved for empty voxels");
        return;
    }
    if (index > static_cast<uint16_t>(mMaterials.size()))
    {
        mMaterials.resize(index, VoxelMaterial::Empty);
    }

    mMaterials[index - 1] = material;
}

uint16_t VoxelPalette::find(const VoxelMaterial& material) const
{
    uint16_t bestI = 0;
    float bestS = material.similarity(VoxelMaterial::Empty);

    for (uint16_t i = 0; i < this->size(); ++i)
    {
        float s = material.similarity(this->get(i + 1));
        if (s > bestS)
        {
            bestS = s;
            bestI = i + 1;
        }
    }

    return bestI;
}

uint16_t VoxelPalette::add(const VoxelMaterial& material, float similarity)
{
    auto i = this->find(material);
    if (this->get(i).similarity(material) >= similarity)
    {
        return i;
    }

    for (uint16_t i = 0; i < this->size(); ++i)
    {
        if (this->get(i + 1).similarity(VoxelMaterial::Empty) >= 1.0F)
        {
            this->set(i + 1, material);
            return i + 1;
        }
    }

    if (this->size() == UINT16_MAX)
    {
        CUBOS_ERROR("Cannot add new material: palette is full");
        return i;
    }

    mMaterials.push_back(material);
    return this->size();
}

void VoxelPalette::merge(const VoxelPalette& palette, float similarity)
{
    for (uint16_t i = 0; i < palette.size(); ++i)
    {
        this->add(palette.get(i + 1), similarity);
    }
}

void cubos::core::data::serialize(Serializer& serializer, const VoxelPalette& palette, const char* name)
{
    // Count non-empty materials.
    std::size_t count = 0;
    for (const auto& material : palette.mMaterials)
    {
        if (material.similarity(VoxelMaterial::Empty) < 1.0F)
        {
            count++;
        }
    }

    serializer.beginDictionary(count, name);
    for (uint16_t i = 0; i < static_cast<uint16_t>(palette.mMaterials.size()); i++)
    {
        if (palette.mMaterials[i].similarity(VoxelMaterial::Empty) < 1.0F)
        {
            serializer.write<uint16_t>(i + 1, nullptr);
            serializer.write(palette.mMaterials[i], nullptr);
        }
    }
    serializer.endDictionary();
}

void cubos::core::data::deserialize(Deserializer& deserializer, VoxelPalette& palette)
{
    palette.mMaterials.clear();

    VoxelMaterial mat;
    uint16_t index;

    std::size_t count = deserializer.beginDictionary();
    for (std::size_t i = 0; i < count; i++)
    {
        deserializer.read<uint16_t>(index);
        deserializer.read(mat);

        if (index == 0)
        {
            CUBOS_WARN("Skipping invalid palette material index 0: reserved for empty voxels");
            continue;
        }
        if (index > palette.mMaterials.size())
        {
            palette.mMaterials.resize(index, VoxelMaterial::Empty);
        }
        palette.mMaterials[index - 1] = mat;
    }
    deserializer.endDictionary();
}
