#pragma once

namespace ut
{
	template<size_t ...>
	struct sequence {};

	template<size_t N, size_t ...S>
	struct make_sequence : make_sequence<N - 1, N - 1, S...> {};

	template<size_t ...S>
	struct make_sequence<0, S...> {
		typedef sequence<S...> type;
	};
}