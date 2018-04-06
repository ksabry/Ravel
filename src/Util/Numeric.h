namespace Ravel
{
	template<typename T>
	T Min(T a, T b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	T Max(T a, T b)
	{
		return a > b ? a : b;
	}
}