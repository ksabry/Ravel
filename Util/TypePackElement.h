#pragma once

#include <utility>

// Get nth type parameter from pack, this method is supposedly more efficient than alternatives
// see https://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/

namespace Ravel
{
    template <size_t I, typename T>
    struct indexed {
        using type = T;
    };

    template <typename Is, typename ...Ts>
    struct indexer;

    template <size_t ...Is, typename ...Ts>
    struct indexer<std::index_sequence<Is...>, Ts...>
        : indexed<Is, Ts>...
    { };

    // Does NOT need and implementation
    template <size_t I, typename T>
    static indexed<I, T> select(indexed<I, T>);

    template <size_t I, typename ...Ts>
    using type_pack_element = typename decltype(select<I>(
        indexer<std::index_sequence_for<Ts...>, Ts...>{}
    ))::type;
}
