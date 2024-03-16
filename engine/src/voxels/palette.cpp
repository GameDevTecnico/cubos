#include <cstring>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/endianness.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/voxels/palette.hpp>

using cubos::core::memory::fromBigEndian;
using cubos::core::memory::Stream;
using cubos::core::memory::toBigEndian;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(VoxelPalette)
{
    using namespace cubos::core::reflection;

    return Type::create("cubos::engine::VoxelPalette")
        .with(ConstructibleTrait::typed<VoxelPalette>().withDefaultConstructor().withMoveConstructor().build());
}

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

uint16_t VoxelPalette::push(const VoxelMaterial& material)
{
    if (this->size() == UINT16_MAX)
    {
        CUBOS_FAIL("Cannot add new material: palette is full");
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

VoxelPalette::Iterator::Iterator(std::vector<VoxelMaterial>::iterator it)
    : mIter(it)
{
}

// Define the iterator operations
VoxelPalette::Iterator& VoxelPalette::Iterator::operator++()
{
    ++mIter;
    return *this;
}

VoxelMaterial& VoxelPalette::Iterator::operator*()
{
    return *mIter;
}

bool VoxelPalette::Iterator::operator==(const Iterator& other) const
{
    return mIter == other.mIter;
}

VoxelPalette::Iterator VoxelPalette::begin()
{
    return {mMaterials.begin()};
}

VoxelPalette::Iterator VoxelPalette::end()
{
    return {mMaterials.end()};
}

bool VoxelPalette::loadFrom(Stream& stream)
{
    while (!stream.eof())
    {
        float r, g, b, a;
        stream.read(&r, sizeof(float));
        stream.read(&g, sizeof(float));
        stream.read(&b, sizeof(float));
        stream.read(&a, sizeof(float));
        r = fromBigEndian(r);
        g = fromBigEndian(g);
        b = fromBigEndian(b);
        a = fromBigEndian(a);
        printf("(%f,%f,%f,%f)\n", r, g, b, a);
        mMaterials.push_back({{r, g, b, a}});
    }
    return true;
}

bool VoxelPalette::writeTo(Stream& stream) const
{
    for (const auto& material : mMaterials)
    {
        float r = toBigEndian(material.color.r);
        float g = toBigEndian(material.color.g);
        float b = toBigEndian(material.color.b);
        float a = toBigEndian(material.color.a);
        stream.write(&r, sizeof(float));
        stream.write(&g, sizeof(float));
        stream.write(&b, sizeof(float));
        stream.write(&a, sizeof(float));
    }
    return true;
}

PaletteBridge::PaletteBridge()
    : FileBridge(core::reflection::reflect<VoxelPalette>())
{
}

bool PaletteBridge::loadFromFile(Assets& assets, const AnyAsset& handle, Stream& stream)
{
    VoxelPalette palette{};
    if (palette.loadFrom(stream))
    {
        assets.store(handle, std::move(palette));
        return true;
    }
    return false;
}

bool PaletteBridge::saveToFile(const Assets& assets, const AnyAsset& handle, Stream& stream)
{
    auto palette = assets.read<VoxelPalette>(handle);
    return palette->writeTo(stream);
}
