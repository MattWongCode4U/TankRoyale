#pragma once
#include <vector>
#include <functional>

class Sort
{
public:
	template<typename T>
	static void ShellSort(std::vector<T> &list, std::function<int(T,T)> comparer);
};

const int gaps[] = { 701, 301, 132, 57, 23, 10, 4, 1 }; //Marcin Ciura's gap sequence

template<typename T>
inline void Sort::ShellSort(std::vector<T> &list, std::function<int(T, T)> comparer)
{
	const int n = list.size();

	//for each gap...
	for (int gap : gaps)
	{
		//do a gapped insertion sort
		for (int i = gap; i < n; ++i)
		{
			T temp = list[i];
			int j = i;
			for (; j >= gap && comparer(temp, list[j - gap]) > 0; j -= gap)
			{
				list[j] = list[j - gap];
			}
			list[j] = temp;
		}

	}
}