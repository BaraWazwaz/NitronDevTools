#pragma once

#include <type_traits>
#include <typeindex>
#include <assert.h>

namespace nitron
{

template <typename T>
class TypeInfo
{
public:

    inline static constexpr bool is_const     = std::is_const_v<T>;
    inline static constexpr bool is_mutable   = !std::is_const_v<T>;
    inline static constexpr bool is_volatile  = std::is_volatile_v<T>;
    inline static constexpr bool is_local     = !std::is_volatile_v<T>;

    inline static constexpr bool is_void      = std::is_void_v<T>;
    inline static constexpr bool is_function  = std::is_function_v<T>;
    inline static constexpr bool is_reference = std::is_reference_v<T>;
    inline static constexpr bool is_object    = std::is_object_v<T>;

    inline static constexpr bool is_enum      = std::is_enum_v<T>;
    inline static constexpr bool is_array     = std::is_array_v<T>;

    inline static constexpr bool is_class     = std::is_class_v<T>;
    inline static constexpr bool is_primitive = !std::is_class_v<T>;

    using clean_type = std::remove_cvref_t<T>;
    using decay_type = std::decay_t<T>;

    inline static std::type_index type_id() { return std::type_index(typeid(clean_type)); }

    using constant        = clean_type const;
    using reference       = clean_type&;
    using const_reference = clean_type const&;
    using array           = clean_type[];
    using const_array     = clean_type const[];

    using pointer                = clean_type*;
    using const_pointer          = clean_type* const;
    using pointer_to_const       = clean_type const*;
    using const_pointer_to_const = clean_type const* const;

private:

    template <typename U, bool>
    struct MemberAccessHelper;

    template <typename U>
    struct MemberAccessHelper<U, true>
    {
        template <typename DataType>
        using data     = DataType clean_type::*;
        template <typename ResultType, typename... ArgsType>
        using function = ResultType (clean_type::*) (ArgsType...);
    };

    template <typename U>
    struct MemberAccessHelper<U, false>
    {
        template <typename DataType>
        using data     = void;
        template <typename ResultType, typename... ArgsType>
        using function = void;
    };

public:

    template <typename DataType>
    using member_data_pointer     = typename MemberAccessHelper<clean_type, is_class>::template data<DataType>;
    template <typename ResultType, typename... ArgsType>
    using member_function_pointer = typename MemberAccessHelper<clean_type, is_class>::template function<ResultType, ArgsType...>;
    
private:

    TypeInfo()                      = delete;
    TypeInfo(TypeInfo&)             = delete;
    TypeInfo(TypeInfo&&)            = delete;
    TypeInfo& operator=(TypeInfo&)  = delete;
    TypeInfo& operator=(TypeInfo&&) = delete;
};

} // namespace nitron
