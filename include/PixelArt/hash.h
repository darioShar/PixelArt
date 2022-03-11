#pragma once
namespace pa {
	/*template<typename T1, typename T2>
	struct PairHasher {
		size_t operator()(const std::pair<T1, T2>& p) const
		{
			return ((std::hash<T1>()(p.first)
				^ (std::hash<T2>()(p.second) << 1)) >> 1);
		}
	};*/

	template<typename T1, typename T2, template <class, class> class PairType = std::pair>
	struct PairHasher {
		size_t operator()(PairType<T1, T2>&& p) const
		{
			return ((std::hash<T1>()(p.first)
				^ (std::hash<T2>()(p.second) << 1)) >> 1);
		}
	};
}
