#pragma once

/// check that cpp is above c++17
/// @TODO: some compilers do not add this flag by default so this check may be a hinderance.
static_assert(__cplusplus >= 201703L, "minigraph needs at least c++17.");

// first declare of namespace
namespace mini {}
