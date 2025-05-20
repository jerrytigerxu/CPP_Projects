#include <iostream>
#include <vector>
#include <algorithm>


int main()
{
	std::vector<int> numbers = {1, 2, 3, 4, 5};

	// Lambda to print each number
	std::cout << "Numbers: ";
	std::for_each(numbers.begin(), numbers.end(), [](int n) {
			std::cout << n << " ";
			});
	std::cout << std::endl;

	// Lambda to check if a number is even
	auto isEven = [](int n) { return n % 2 == 0; };
}
