#ifndef CUBOS_CORE_DATA_PACKAGE_HPP
#define CUBOS_CORE_DATA_PACKAGE_HPP

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/log.hpp>

#include <variant>
#include <stack>
#include <map>

namespace cubos::core::data
{
    namespace impl
    {
        class Packager;
        class Unpackager;
    } // namespace impl

    /// @brief A utility object which is capable of storing the data of any trivially
    /// serializable object. One way to understand this class is to think of it
    /// as if it were a JSON representation of an object.
    ///
    /// @details While packages are serialiable, they are not deserializable.
    /// This is because there's no way to know what type the package was serialized
    /// from. If you know what type the package was serialized from, and want
    /// to deserialize it, you can use the type's deserialization function.
    ///
    /// Usage example:
    ///
    ///     auto pkg = Package::from(mySerializableObject);
    ///     pkg.field("position").field("x").set(1.0f);
    ///     pkg.field("position").field("y").set(2.0f);
    ///     pkg.field("position").field("z").set(3.0f);
    ///     pkg.into(myDeserializableObject);
    ///     // myDeserializableObject will hold the same data as
    ///     // mySerilizableObject but with the position set to (1, 2, 3).
    ///
    class Package final
    {
    public:
        /// Used to identify the type of the packaged data.
        enum class Type
        {
            None,       ///< The package doesn't contain anything.
            I8,         ///< The package contains a signed 8-bit integer.
            I16,        ///< The package contains a signed 16-bit integer.
            I32,        ///< The package contains a signed 32-bit integer.
            I64,        ///< The package contains a signed 64-bit integer.
            U8,         ///< The package contains an unsigned 8-bit integer.
            U16,        ///< The package contains an unsigned 16-bit integer.
            U32,        ///< The package contains an unsigned 32-bit integer.
            U64,        ///< The package contains an unsigned 64-bit integer.
            F32,        ///< The package contains a 32-bit floating point number.
            F64,        ///< The package contains a 64-bit floating point number.
            Bool,       ///< The package contains a boolean value.
            String,     ///< The package contains a string.
            Object,     ///< The package contains an object with fields.
            Array,      ///< The package contains an array.
            Dictionary, ///< The packages contains a dictionary.
        };

        /// Type alias for the map used to store the object fields.
        using Fields = std::vector<std::pair<std::string, Package>>;
        /// Type alias for the vector used to store the array elements.
        using Elements = std::vector<Package>;
        /// Type alias for the vector used to store the dictionary pairs.
        using Dictionary = std::vector<std::pair<Package, Package>>;
        /// Type alias for the variant used to store the package's data.
        /// The types must be defined in the same order as the Type enum.
        using Data = std::variant<std::monostate, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t,
                                  uint64_t, float, double, bool, std::string, Fields, Elements, Dictionary>;

        Package(Type type = Type::None);
        Package(Package&& rhs) = default;
        Package(const Package& rhs) = default;
        ~Package() = default;

        Package& operator=(Package&& rhs) = default;
        Package& operator=(const Package& rhs) = default;

        /// Packages serializable data and returns the result.
        /// @tparam T The type of the data to package.
        /// @param data The data to package.
        /// @param context Optional context to use when packaging the data.
        template <typename T>
        static inline Package from(const T& data, Context* context = nullptr)
        {
            Package pkg;
            pkg.set(data, context);
            return std::move(pkg);
        }

        /// Packages the specified data into this package. This will change the type of the
        /// package. This may be a problem in some cases. For example, if an uint8_t is packaged,
        /// then set to a uint16_t, and then binary serialized, it won't be deserializable back to
        /// an uint8_t (only to a uint16_t). One way to avoid this problem is to use
        /// `Package::change` instead.
        /// @see Package::change
        /// @tparam T The type of the data to package.
        /// @param data The data to package.
        /// @param context Optional context to use when packaging the data.
        template <typename T>
        inline void set(const T& data, Context* context = nullptr);

        /// Packages the specified int64_t into this package, without changing
        /// the type of the package. If the types aren't compatible, this will
        /// fail. For example, 255 would be compatible with uint8_t, but -1 or
        /// 256 would not.
        /// @see Package::set
        /// @param data The data to package.
        /// @returns True on success, false on failure.
        bool change(int64_t data);

        /// Packages the specified uint64_t into this package, without changing
        /// the type of the package. If the types aren't compatible, this will
        /// fail. For example, 255 would be compatible with uint8_t, but 256
        /// would not.
        /// @see Package::set
        /// @param data The data to package.
        /// @returns True on success, false on failure.
        bool change(uint64_t data);

        /// Packages the specified double into this package, without changing
        /// the type of the package. If the types aren't compatible, this will
        /// fail. If the underlying type is a float, the precision will be
        /// lowered.
        /// @see Package::set
        /// @param data The data to package.
        /// @returns True on success, false on failure.
        bool change(double data);

        /// Packages the specified bool into this package, without changing
        /// the type of the package. If the package isn't a bool, this will
        /// fail.
        /// @see Package::set
        /// @param data The data to package.
        /// @returns True on success, false on failure.
        bool change(bool data);

        /// Packages the specified string into this package, without changing
        /// the type of the package. If the package isn't a string already,
        /// this will fail.
        /// @see Package::set
        /// @param data The data to package.
        /// @returns True on success, false on failure.
        bool change(const std::string& data);

        /// Unpackages the package into data.
        /// If there is a field missing or there's a type mismatch, the
        /// unpackaging may fail. It is guaranteed that unpackaging a package
        /// created from a certain type to the same type never fails.
        /// @tparam T The type of the data to unpackage.
        /// @param data The data to write to.
        /// @param context Optional context to use when unpackaging the data.
        /// @returns True if the unpackaging succeeded, false otherwise.
        template <typename T>
        inline bool into(T& data, Context* context = nullptr) const;

        /// Alternative to Package::into for types which are default
        /// constructible. Unpackages the package and returns the data, and
        /// aborts if the type isn't the expected. Should only be used when
        /// you're sure the type is correct.
        /// @tparam T The type of the data to unpackage.
        /// @param context Optional context to use when unpackaging the data.
        /// @returns The data.
        template <typename T>
        requires std::default_initializable<T>
        inline T get(Context* context = nullptr) const
        {
            T data;
            if (!this->into(data, context))
            {
                CUBOS_CRITICAL("Couldn't unpack package into type {}", typeid(T).name());
                abort();
            }
            return data;
        }

        /// @returns The type of the packaged data.
        Type type() const;

        /// Gets the size of the package.
        /// For empty packages, or scalars (including strings), this is always
        /// 0, or 1, respectively.
        /// For objects, arrays and dictionaries, this is, respectively, the
        /// field count, the element count and the key-value pair count.
        /// @returns The size of the packaged
        size_t size() const;

        /// Checks if the package is storing a structured data type (object, array or dictionary).
        /// @returns True if the package is storing a structured data type, false otherwise.
        bool isStructured() const;

        /// Gets a reference to a field of this package.
        /// If the package isn't storing an object or if the field doesn't exist, this method will abort.
        /// @param name The name of the field.
        /// @returns A reference to the field.
        Package& field(const std::string& name);

        /// Gets all of the fields in this package.
        /// If the package isn't storing an object, this method will abort.
        /// @returns A map associating the field name to the field package.
        Fields& fields();

        /// Gets all of the fields in this package.
        /// If the package isn't storing an object, this method will abort.
        /// @returns A map associating the field name to the field package.
        const Fields& fields() const;

        /// Gets an element of the array stored in this package.
        /// If the package isn't an array, this method will abort.
        /// If the index is out of bounds, this method will abort.
        /// @param index The index of the element in the array.
        /// @returns A reference to the element.
        Package& element(size_t index);

        /// Gets the array stored in this package.
        /// If the package isn't an array, this method will abort.
        /// @returns A vector of the packaged elements.
        Elements& elements();

        /// Gets the array stored in this package.
        /// If the package isn't an array, this method will abort.
        /// @returns A vector of the packaged elements.
        const Elements& elements() const;

        /// Gets the dictionary stored in this package.
        /// The dictionary is returned as a vector of the packaged key-value
        /// pairs.
        /// If the package isn't a dictionary, this method will abort.
        Dictionary& dictionary();

        /// Gets the dictionary stored in this package.
        /// The dictionary is returned as a vector of the packaged key-value
        /// pairs.
        /// If the package isn't a dictionary, this method will abort.
        const Dictionary& dictionary() const;

        /// Converts a package type into its string representation.
        /// @param type The type to convert.
        /// @returns The string representation of the type.
        static const char* typeToString(Type type);

        void serialize(Serializer& ser, const char* name) const;

    private:
        friend impl::Packager;
        friend impl::Unpackager;

        /// The packaged data.
        Data data;
    };

    /// Implementation specific classes for the above functions are hidden in
    /// this namespace.
    namespace impl
    {
        /// Responsible for serializing types into packages.
        /// Should never be used directly.
        class Packager : public Serializer
        {
        public:
            // Implement interface methods.

            virtual void writeI8(int8_t value, const char* name) override;
            virtual void writeI16(int16_t value, const char* name) override;
            virtual void writeI32(int32_t value, const char* name) override;
            virtual void writeI64(int64_t value, const char* name) override;
            virtual void writeU8(uint8_t value, const char* name) override;
            virtual void writeU16(uint16_t value, const char* name) override;
            virtual void writeU32(uint32_t value, const char* name) override;
            virtual void writeU64(uint64_t value, const char* name) override;
            virtual void writeF32(float value, const char* name) override;
            virtual void writeF64(double value, const char* name) override;
            virtual void writeBool(bool value, const char* name) override;
            virtual void writeString(const char* value, const char* name) override;
            virtual void beginObject(const char* name) override;
            virtual void endObject() override;
            virtual void beginArray(size_t length, const char* name) override;
            virtual void endArray() override;
            virtual void beginDictionary(size_t length, const char* name) override;
            virtual void endDictionary() override;

        private:
            friend Package;

            /// @param pkg The package to write to.
            Packager(Package& pkg);

            /// Pushes data to the current package.
            /// @param data The data to push.
            /// @param name The name of the field, if any.
            /// @returns Pointer to the pushed data.
            Package* push(Package::Data&& data, const char* name);

            /// Stack used to keep the state of the package.
            /// The package pointer points to the current
            /// object/array/dictionary being written to.
            /// The bool is true if we're writing to a dictionary and the next
            /// element is a key.
            std::stack<std::pair<Package*, bool>> stack;

            /// The root package being written to.
            Package& pkg;
        };

        /// Responsible for deserializing packages into types.
        /// Should never be used directly.
        class Unpackager : public Deserializer
        {
        public:
            // Implement interface methods.

            virtual void readI8(int8_t& value) override;
            virtual void readI16(int16_t& value) override;
            virtual void readI32(int32_t& value) override;
            virtual void readI64(int64_t& value) override;
            virtual void readU8(uint8_t& value) override;
            virtual void readU16(uint16_t& value) override;
            virtual void readU32(uint32_t& value) override;
            virtual void readU64(uint64_t& value) override;
            virtual void readF32(float& value) override;
            virtual void readF64(double& value) override;
            virtual void readBool(bool& value) override;
            virtual void readString(std::string& value) override;
            virtual void beginObject() override;
            virtual void endObject() override;
            virtual size_t beginArray() override;
            virtual void endArray() override;
            virtual size_t beginDictionary() override;
            virtual void endDictionary() override;

        private:
            friend Package;

            /// @param pkg The package to read from.
            Unpackager(const Package& pkg);

            /// Pops data from the current package.
            /// @returns Pointer to the popped data.
            const Package* pop();

            /// Stack used to keep the state of the package.
            /// The package pointer points to the current
            /// object/array/dictionary being read from.
            /// The size_t is the index of the next field/element to read.
            std::stack<std::pair<const Package*, size_t>> stack;

            /// The package being read from.
            const Package& pkg;
        };
    } // namespace impl

    // Implementation.

    template <typename T>
    inline void Package::set(const T& data, Context* context)
    {
        auto packager = impl::Packager(*this);
        if (context != nullptr)
        {
            packager.context().pushSubContext(*context);
        }
        packager.write(data, nullptr);
    }

    template <typename T>
    inline bool Package::into(T& data, Context* context) const
    {
        auto unpackager = impl::Unpackager(*this);
        if (context != nullptr)
        {
            unpackager.context().pushSubContext(*context);
        }
        unpackager.read(data);
        return !unpackager.failed();
    }

} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_PACKAGE_HPP
