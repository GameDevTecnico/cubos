#ifndef CUBOS_ENGINE_DATA_QB_MODEL_HPP
#define CUBOS_ENGINE_DATA_QB_MODEL_HPP

#include <cubos/engine/data/loader.hpp>

#include <cubos/core/data/qb_parser.hpp>

#include <unordered_map>

namespace cubos::engine::data
{
    namespace impl
    {
        class QBModelLoader;
    } // namespace impl

    /// Asset that stores core::data::QBMatrix's loaded from a QB model.
    struct QBModel
    {
        static constexpr const char* TypeName = "QBModel";
        using Loader = impl::QBModelLoader;

        std::vector<core::data::QBMatrix> matrices; ///< The loaded QBMatrix's.
    };

    namespace impl
    {
        /// Loader for QBModel assets.
        class QBModelLoader : public Loader
        {
        public:
            QBModelLoader() = default;
            virtual ~QBModelLoader() override = default;

            virtual const void* load(const Meta& meta) override;
            virtual std::future<const void*> loadAsync(const Meta& meta) override;
            virtual void unload(const Meta& meta, const void* asset) override;
        };
    } // namespace impl
} // namespace cubos::engine::data

#endif // CUBOS_ENGINE_DATA_QB_MODEL_HPP
