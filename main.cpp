#include <string>
#include <vector>
#include <unordered_set>

#include "reflection.hpp"

// Use REFLECTABLE macro to indicate what data members u want to reflect.
struct Point {
	float x = 0;
	float y = 0;

	REFLECTABLE(x, y)
};

// The reflection library has access to private data members, regardless of where MACRO is placed.
class Private {
public: 
	Private() = default;

private:
	int x[1] = { 0 };
	std::size_t y = 0;

	REFLECTABLE(x, y)
};

// You can selectively choose what data member to reflect
struct Data {
	std::string foo;
	std::vector<int> bar;
	std::unordered_set<float> baz;

	REFLECTABLE(foo, baz)
};

// Reflection works recursively too, provided that the data members are also reflectable.
struct Points {
	Point pt1 {};
	Point pt2 {};
	Point pt3 {};

	REFLECTABLE(pt1, pt2, pt3)
};

struct ManyPoints {
	Points points1 { {6.0f, 5.0f}, {4.0f, 3.0f}, {2.0f, 1.0f} };
	Point pt { 5.f, 5.f };
	
	int x = 10;
	int y = 20;

	Points points2{ {-16.0f, -15.0f}, {-14.0f, -13.0f}, {-12.0f, -11.0f} };

	REFLECTABLE(points1, pt, x, y, points2)
};

int main() {
	// =======================================================================
	// 1.0 Simple printing.
	Point point1;
	reflection::print(point1);

	std::cout << "\n";

	// 1.1 Constness is respected.
	Point const point2 {5, 10};
	reflection::print(point2);

	std::cout << "\n";

	// 1.2 Reflection library has access to private data members
	Private const foo;
	reflection::print(foo);

	std::cout << "\n";

	// 1.3 You can check if class is reflectable.
	if constexpr (reflection::isReflectable<Point>()) {
		std::cout << "Class is reflectable.\n";
	}
	else {
		std::cout << "Class is not reflectable.\n";
	}

	// =======================================================================
	// 2.0 Using your own lambda to iterate through a reflected class
	Data const data{ "Hello World!", {1, 2, 3}, {2.0f, 4.0f, 6.0f} };	// aggregate initialisation
	
	int counter = 0;

	reflection::visit([&](auto fieldData) {	
		auto& dataMember = fieldData.get();
		const char* dataMemberName = fieldData.name();
		
		(void) dataMemberName; // unused

		// From here you can do whatever you want with the data members.
		// from printing, de/serialization, etc..

		// Example printing...
		std::cout << counter << ". ";

		// assumes that data member implements .begin() and .end()
		for (auto const& element : dataMember) {
			std::cout << element << "-";
		}

		std::cout << "\n";
		++counter;
	}, data);

	// 2.1 You can use if constexpr and type traits to do certain things based on type. Similar to std::visit.
	reflection::visit([&](auto fieldData) {
		using DataMemberType = std::decay_t<decltype(fieldData)>;
		auto& dataMember = fieldData.get();

		(void) dataMember; // unused

		// Using if constexpr and type traits to do certain things based on type.
		if constexpr (std::same_as<DataMemberType, std::string>) {
			std::cout << "hey im a string!\n";
		}
	}, data);

	// (Rare) 2.2 Niche cases where you need pointer to the data member
	reflection::visit([](auto fieldData) {
		auto pointerToDataMember = fieldData.getPointerToMember();
		// do whatever u want with pointer to data member. ()

		(void) pointerToDataMember; // unused
	}, point2);

	// =======================================================================
	// 3.0 Recursive printing!
	std::cout << "\nRecursive printing..\n";

	Points const points{ {1.0f, 2.0f}, {3.0f, 4.0f}, {5.0f, 6.0f} };
	reflection::print(points);

	std::cout << "\n";

	// 3.1 Regardless of depth.
	ManyPoints manyPts;
	reflection::prettyPrint(manyPts);

	// 3.2 (Advanced) look at function definition of prettyPrint 
	// to see how you could provide functors that will be invoked before and after iterating through a reflectable data member recursively.
}