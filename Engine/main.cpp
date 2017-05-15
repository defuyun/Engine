#include "engine.h"
#include "gen_sim.h"

using namespace std;
template<typename T>
void adder(T & third) {
	std::cout << 1 << '\n';
}

template<typename T>
void adder(T & second, T & third) {
	std::cout << 2 << '\n';
}

template<typename T>
void adder(T & first, T & second, T & third) {
	std::cout << 3 << " ";
}

template<typename T>
void adder(T & first, T & second, T & third, T & fouth) {
	std::cout << 4 << " ";
}

template<typename... Args>
void adder(Args... args) {
	std::cout <<  "unpack" << '\n';
}

int main() {
}