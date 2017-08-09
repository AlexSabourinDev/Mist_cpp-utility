#include "../../include/data-structures/RingBuffer.h"
#include "../../include/algorithms/Sorting.h"
#include "../../include/common/UtilityMacros.h"
#include "../../include/utility/BitManipulations.h"
#include "../../include/reflection/Type.h"
#include "../../include/reflection/Delegate.h"
#include "../../include/utility/Hashing.h"
#include "../../include/reflection/MetaData.h"
#include "../../include/reflection/MethodInfo.h"
#include "../../include/reflection/MemberInfo.h"
#include "../../include/reflection/TypeInfo.h"
#include "../../include/reflection/Reflection.h"
#include "../../include/data-structures/SingleList.h"
#include "../../include/allocators/CppAllocator.h"
#include "../../include/data-structures/DynamicArray.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <limits>
#include <ctime>
#include <memory>

size_t g_SomeGlobal = 10;

// Simple timer methods
std::clock_t s_StartTime;

void BeginTimer() {
	s_StartTime = std::clock();
}

double EndTimer() {
	return (double)(std::clock() - s_StartTime) / (double)(CLOCKS_PER_SEC / 1000);
}

void Pause() {
	std::cout << "Press Any Key" << std::endl;
	char c;
	std::cin >> c;
}

size_t ReturnNumber(size_t num) {
	return num;
}

void TestReflection() {

	std::cout << "Reflection Test" << std::endl;

	// -Type-

	Mist::Type type;
	type.DefineTypes<int>();

	MIST_ASSERT(type.Is<int>());
	MIST_ASSERT(type.Is<float>() == false);

	type.DefineTypes<float>();

	MIST_ASSERT(type.Is<int>() == false);
	MIST_ASSERT(type.Is<float>());

	std::unique_ptr<Mist::Detail::Callback> callback(Mist::Detail::MakeCallback(Pause));
	MIST_ASSERT(Mist::Detail::HasDefinition<void>(callback.get()));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(callback.get())) == false);

	// -Callback-

	const size_t CHANGE_TARGET = 1;
	size_t changed;
	std::unique_ptr<Mist::Detail::Callback> lambda(Mist::Detail::MakeCallback([=,&changed]() { changed = CHANGE_TARGET; }));

	MIST_ASSERT(Mist::Detail::HasDefinition<void>(lambda.get()));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(lambda.get())) == false);

	Mist::Detail::CallbackInterface<void>* call = Mist::Detail::Cast<void>(lambda.get());
	call->Invoke();

	MIST_ASSERT(changed == CHANGE_TARGET);

	changed = 0;
	auto l = [&changed](size_t j) { changed = j; };
	std::unique_ptr<Mist::Detail::Callback> savedLambda(Mist::Detail::MakeCallback(l));

	MIST_ASSERT((Mist::Detail::HasDefinition<void, size_t>(savedLambda.get())));
	MIST_ASSERT((Mist::Detail::HasDefinition<size_t, size_t>(savedLambda.get())) == false);

	Mist::Detail::CallbackInterface<void, size_t>* c = Mist::Detail::Cast<void, size_t>(savedLambda.get());
	c->Invoke(CHANGE_TARGET);

	MIST_ASSERT(changed == CHANGE_TARGET);
	
	// -Delegate-

	size_t data = 0;
	Mist::Delegate del([&]() { data = CHANGE_TARGET; });
	del.Invoke();

	MIST_ASSERT(data == CHANGE_TARGET);

	data = 0;
	Mist::Delegate del2([](size_t j) { return j; });
	data = del2.Invoke<size_t, size_t>(CHANGE_TARGET);

	MIST_ASSERT(data == CHANGE_TARGET);

	data = 0;
	Mist::Delegate del3(&ReturnNumber);
	MIST_ASSERT((del3.HasSignature<size_t, size_t>()) == true);
	data = del3.Invoke<size_t, size_t>(CHANGE_TARGET);

	MIST_ASSERT(data == CHANGE_TARGET);

	// MetaData

	const size_t META_DATA_VALUE = 5;
	
	Mist::MetaData metaData;

	MIST_ASSERT(metaData.Has("Lol") == false);
	MIST_ASSERT((*metaData.Add<size_t>("Foo", META_DATA_VALUE)) == META_DATA_VALUE);
	MIST_ASSERT(metaData.Has("Foo"));
	MIST_ASSERT((*metaData.Get<size_t>("Foo")) == META_DATA_VALUE);

	metaData.Add("Hi", META_DATA_VALUE);
	metaData.Add("This", META_DATA_VALUE);

	for (auto& i : metaData) {
		std::cout << i.first << ":" << *Mist::Cast<size_t>(i.second) << std::endl;
		MIST_ASSERT(*Mist::Cast<size_t>(i.second) == META_DATA_VALUE);
	}

	struct TestClass {
		size_t data = META_DATA_VALUE;
	};

	MIST_ASSERT(metaData.Add(Mist::HashID(1), TestClass())->data == META_DATA_VALUE);
	MIST_ASSERT(metaData.Has(Mist::HashID(1)));
	MIST_ASSERT(metaData.Get<TestClass>(Mist::HashID(1))->data == META_DATA_VALUE);
	
	// -MethodInfo-

	struct TestMethodInfo {

		size_t Value() { std::cout << "TestMethodInfo::Value" << std::endl; return CHANGE_TARGET; }
		size_t Repeat(size_t value) { return value; }

		size_t m_Value = CHANGE_TARGET;
	};

	TestMethodInfo obj;

	Mist::MethodInfo method(&TestMethodInfo::Value);
	MIST_ASSERT(method.Invoke<size_t>(&obj) == CHANGE_TARGET);

	Mist::MethodInfo repeatMethod(&TestMethodInfo::Repeat);
	MIST_ASSERT(repeatMethod.Invoke<size_t>(&obj, CHANGE_TARGET) == CHANGE_TARGET);

	Mist::MetaData* meta = repeatMethod.GetMetaData();
	meta->Add<size_t>("Property", CHANGE_TARGET);

	MIST_ASSERT(*meta->Get<size_t>("Property") == CHANGE_TARGET);

	// -MemberInfo-

	TestMethodInfo memberTest;

	Mist::MemberInfo member(&TestMethodInfo::m_Value);
	MIST_ASSERT(*member.Get<size_t>(&memberTest) == CHANGE_TARGET);

	Mist::MetaData* memberMera = member.GetMetaData();
	memberMera->Add<size_t>("Property", CHANGE_TARGET);

	MIST_ASSERT(*memberMera->Get<size_t>("Property") == CHANGE_TARGET);


	// -TypeInfo-

	Mist::Type testType;
	testType.DefineTypes<TestMethodInfo>();
	Mist::TypeInfo typeInfo(std::move(testType));

	Mist::MethodInfo* repeatMethodInfo = typeInfo.AddMethod("Repeat", &TestMethodInfo::Repeat);
	Mist::MemberInfo* valueMemberInfo = typeInfo.AddMember("m_Value", &TestMethodInfo::m_Value);

	MIST_ASSERT(typeInfo.HasConstructor<TestMethodInfo>() == false);
	typeInfo.AddConstructor<TestMethodInfo>();
	MIST_ASSERT(typeInfo.HasConstructor<TestMethodInfo>());

	MIST_ASSERT(typeInfo.HasMethod("Repeat"));
	repeatMethodInfo = typeInfo.GetMethodInfo("Repeat");

	MIST_ASSERT(typeInfo.HasMethod("Lol") == false);

	MIST_ASSERT(typeInfo.HasMember("m_Value"));
	valueMemberInfo = typeInfo.GetMemberInfo("m_Value");

	MIST_ASSERT(typeInfo.HasMember("Lol") == false);

	TestMethodInfo* result = typeInfo.Create<TestMethodInfo>(5);
	// These constructors aren't available
	MIST_ASSERT(result == nullptr);
	MIST_ASSERT(typeInfo.TryCreate<TestMethodInfo>(&result, 5) == false);

	result = typeInfo.Create<TestMethodInfo>();
	MIST_ASSERT(result != nullptr);

	TestMethodInfo* otherResult;
	MIST_ASSERT(typeInfo.TryCreate<TestMethodInfo>(&otherResult));
	MIST_ASSERT(otherResult != nullptr);
	delete otherResult;

	MIST_ASSERT((repeatMethodInfo->Invoke<size_t, TestMethodInfo, size_t>(result, CHANGE_TARGET)) == CHANGE_TARGET);
	MIST_ASSERT(*valueMemberInfo->Get<size_t>(result) == CHANGE_TARGET);
	delete result;

	Mist::MetaData* typeMeta = typeInfo.GetMetaData();
	MIST_ASSERT(typeMeta->Has("Strength") == false);
	typeMeta->Add("Strength", 1.0f);
	MIST_ASSERT(typeMeta->Has("Strength"));
	float* strength = typeMeta->Get<float>("Strength");
	MIST_ASSERT(*strength == 1.0f);
	*strength = 10.0f;
	MIST_ASSERT(*typeMeta->Get<float>("Strength") == 10.0f);

	struct NonDefault {
		NonDefault() {
			m_Data = CHANGE_TARGET;
		}

		NonDefault(size_t value) {
			m_Data = value;
		}

		size_t m_Data;
	};

	Mist::Type ndType;
	ndType.DefineTypes<NonDefault>();

	Mist::TypeInfo nonDefaultType(std::move(ndType));
	nonDefaultType.AddConstructor<NonDefault>();
	nonDefaultType.AddConstructor<NonDefault, size_t>();

	// Assure that non default constructors work
	NonDefault* d = nonDefaultType.Create<NonDefault>(CHANGE_TARGET);
	MIST_ASSERT(d != nullptr);
	delete d;

	d = nonDefaultType.Create<NonDefault>();
	MIST_ASSERT(d != nullptr);
	delete d;

	Mist::Reflection reflection;
	Mist::TypeInfo* reflectedType = reflection.AddType<NonDefault>("NonDefault");
	MIST_ASSERT(reflectedType != nullptr);
	Mist::TypeInfo* otherReflected = reflection.GetType("NonDefault");

	MIST_ASSERT(reflectedType == otherReflected);

	Mist::GlobalFunction* global = reflection.AddGlobalFunction("ReturnNumber", &ReturnNumber);
	global->GetMetaData()->Add("Meta", (size_t)10);
	MIST_ASSERT(*global->GetMetaData()->Get<size_t>("Meta") == 10);

	Mist::GlobalFunction* otherGlobal = reflection.GetGlobalFunction("ReturnNumber");
	MIST_ASSERT(global == otherGlobal);

	MIST_ASSERT((global->Invoke<size_t, size_t>(CHANGE_TARGET)) == CHANGE_TARGET);

	Mist::GlobalObject* gAny = reflection.AddGlobalObject("Any", &g_SomeGlobal);
	gAny->GetMetaData()->Add("Meta", (size_t)10);
	MIST_ASSERT(*gAny->GetMetaData()->Get<size_t>("Meta") == 10);

	Mist::GlobalObject* otherAny = reflection.GetGlobalObject("Any");

	MIST_ASSERT(gAny == otherAny);
	MIST_ASSERT((*gAny->Get<size_t>()) == 10);

	Mist::Reflection mergedTarget;
	Mist::Merge(std::move(reflection), &mergedTarget);

	global = mergedTarget.GetGlobalFunction("ReturnNumber");
	MIST_ASSERT((global->Invoke<size_t, size_t>(CHANGE_TARGET)) == CHANGE_TARGET);

	std::cout << "Reflection Test Passed!" << std::endl;
}

void TestHash() {
	
	std::cout << "Hashing Test" << std::endl;
	
	std::vector<uint32_t> results;
	results.push_back(Mist::Hash_djb2("lol"));
	results.push_back(Mist::Hash_djb2("lol0"));
	results.push_back(Mist::Hash_djb2("loldfsdaf"));
	results.push_back(Mist::Hash_djb2("loldsafdsafdsafdsafdsafdsa"));
	results.push_back(Mist::Hash_djb2("logfrwgvcxzgrl"));
	results.push_back(Mist::Hash_djb2("lothrtjn xgtbtbreyl"));
	results.push_back(Mist::Hash_djb2("logdsafhudesrv jklb l"));
	results.push_back(Mist::Hash_djb2("lot5nbunel"));
	results.push_back(Mist::Hash_djb2("lobtyrbtsbgtdsabdtl"));
	results.push_back(Mist::Hash_djb2("loniuvgtehrgpb5gs8yniotbsrl"));
	results.push_back(Mist::Hash_djb2("lobtrenbpvznurfbhobntrwgal"));
	results.push_back(Mist::Hash_djb2("lolngjurenpwijnjivupr"));
	results.push_back(Mist::Hash_djb2("looooofodfsaofdsafodasfodl"));
	results.push_back(Mist::Hash_djb2("lonyetnusjrnfioNSfNOUfol"));
	results.push_back(Mist::Hash_djb2("enbpvznurfbh"));
	results.push_back(Mist::Hash_djb2("odfsaofdsafodasfodl"));
	results.push_back(Mist::Hash_djb2("lonyetnusjrnfioN"));
	results.push_back(Mist::Hash_djb2("loetnusupr"));
	results.push_back(Mist::Hash_djb2("lngjureol"));
	results.push_back(Mist::Hash_djb2("odfiukmmtnrhb"));
	results.push_back(Mist::Hash_djb2("bytbdsfaazrs4b z"));
	results.push_back(Mist::Hash_djb2("jkoytmnkodtynd"));
	results.push_back(Mist::Hash_djb2("vrehivuorsabeFORNZeu i"));
	results.push_back(Mist::Hash_djb2("foo"));
	results.push_back(Mist::Hash_djb2("bar"));
	results.push_back(Mist::Hash_djb2("vector"));
	results.push_back(Mist::Hash_djb2("string"));
	results.push_back(Mist::Hash_djb2("hash"));
	results.push_back(Mist::Hash_djb2("m_hello"));
	results.push_back(Mist::Hash_djb2("m_Lol"));
	results.push_back(Mist::Hash_djb2("m_Hi"));
	results.push_back(Mist::Hash_djb2("m_Value"));
	results.push_back(Mist::Hash_djb2("m_Result"));
	results.push_back(Mist::Hash_djb2("m_ShouldRun"));
	results.push_back(Mist::Hash_djb2("m_IsActive"));
	results.push_back(Mist::Hash_djb2("m_HasLife"));
	results.push_back(Mist::Hash_djb2("m_ShouldBe"));
	results.push_back(Mist::Hash_djb2("aaaaaaa"));
	results.push_back(Mist::Hash_djb2("aaaa"));
	results.push_back(Mist::Hash_djb2("aaaaaaaaaaa"));
	results.push_back(Mist::Hash_djb2("aaaaaaaaa"));

	
	for(auto& i : results) {
		for(auto& j : results) {
			if(&i != &j)
			{
				MIST_ASSERT(i != j);
			}
		}
	}
	
	std::cout << "Hashing Test Passed!" << std::endl;
	
}

void TestRingBuffer() {
	// -Test-
	std::cout << "RingBuffer Test" << std::endl;

	Mist::RingBuffer<size_t, 6> buffer;
	// Assure that the returned size is correct
	MIST_ASSERT(buffer.Size() == 6);


	// Assure that can write and try write return the same value
	MIST_ASSERT(buffer.CanWrite());
	// Assure that you can write into the circular buffer
	MIST_ASSERT(buffer.TryWrite(0));
	// Assure that you can write a convertible type into the buffer
	MIST_ASSERT(buffer.TryWrite(10l));
	

	for (size_t i = 0; i < 3; i++) {
		size_t lValue = 10;
		// Assure that can write and try write return the same value
		MIST_ASSERT(buffer.CanWrite());
		// Assure that you can write into the circular buffer, assure that you can write lvalues
		MIST_ASSERT(buffer.TryWrite(lValue));
	}

	for (size_t i = 0; i < 4; i++) {
		// Assure that can write and try write return the same value
		MIST_ASSERT(buffer.CanWrite() == false);
		// Assure that we can't write when the buffer has been filled
		// The known amount you write is currently bound to be tSize - 1, 
		// this might be changed at a later time
		MIST_ASSERT(buffer.TryWrite(10) == false);
	}

	size_t result = 0;
	for (size_t i = 0; i < 5; i++) {
		// Assure that can read implies the same as try read
		MIST_ASSERT(buffer.CanRead());
		
		size_t otherResult = 0;
		// Assure that we can peek the value
		MIST_ASSERT(buffer.TryPeek(&otherResult));
		// Attempt to read the amount written to the buffer
		MIST_ASSERT(buffer.TryRead(&result));

		// Assure that the read and the peek returned the same value
		MIST_ASSERT(otherResult == result);

		std::cout << result << std::endl;
	}

	// Assure that can read implies the same as try read
	MIST_ASSERT(buffer.CanRead() == false);
	// Attempt to read more than was written, Assure that it returns false
	MIST_ASSERT(buffer.TryRead(&result) == false);

	// Notify that the ring buffer tests have passed
	std::cout << "RingBuffer Tests Passed!" << std::endl;


	// -Example use-
	Mist::RingBuffer<size_t, 10> exampleBuffer;
	size_t exampleResult = 0;
	if(exampleBuffer.TryWrite(20)) {
		MIST_ASSERT(exampleBuffer.TryRead(&exampleResult));
		std::cout << exampleResult << std::endl;
	}
	if(exampleBuffer.TryRead(&exampleResult) == false) {
		std::cout << "Nothing left to read" << std::endl;
	}
}

void TestSorting() {
	const size_t SORTING_ITERATIONS = 100;
	const size_t ELEMENT_COUNT = 100;

	srand((unsigned int)time(0));

	// -Test-
	std::cout << "Sorting Test" << std::endl;

	std::cout << "Merge Sort" << std::endl;
	// Test a vector's sorting
	std::vector<size_t> m;
	
	// Test an array's sorting
	size_t arr[ELEMENT_COUNT];
	for (size_t i = 0; i < ELEMENT_COUNT; i++) {
		arr[i] = rand() % ELEMENT_COUNT;
	}
	
	// Merge sort the array
	Mist::MergeSort(&arr[0], &arr[0] + ELEMENT_COUNT);
	MIST_ASSERT(Mist::IsSorted(std::begin(arr), std::end(arr)));


	double totalSortTime = 0.0;
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		BeginTimer();
		// Merge sort the vector
		Mist::MergeSort(&m);
		totalSortTime += EndTimer();
		
		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));

	}
	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Quick Sort" << std::endl;

	
	for (size_t i = 0; i < ELEMENT_COUNT; i++) {
		arr[i] = rand() % ELEMENT_COUNT;
	}
	
	// quick sort the vector
	Mist::QuickSort(std::begin(arr), std::end(arr));
	MIST_ASSERT(Mist::IsSorted(std::begin(arr), std::end(arr)));
	
	totalSortTime = 0.0;
	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// clear the vector and restart
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		BeginTimer();
		// quick sort the vector
		Mist::QuickSort(&m);
		totalSortTime += EndTimer();
		
		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));
	}

	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Insertion Sort" << std::endl;

	totalSortTime = 0.0;
	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// create a sorted vector of n elements
		std::vector<size_t> sortedVector;
		const size_t SORTED_ELEMENT_COUNT = ELEMENT_COUNT;
		for (size_t i = 0; i < SORTED_ELEMENT_COUNT; i++) {
			sortedVector.push_back(i);
		}

		// create a vector to insert into the vector
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		BeginTimer();
		Mist::InsertionSort(m, &sortedVector);
		totalSortTime += EndTimer();
		
		MIST_ASSERT(Mist::IsSorted(std::begin(sortedVector), std::end(sortedVector)));

	}

	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Bucket Sort" << std::endl;



	for (size_t i = 0; i < ELEMENT_COUNT; i++) {
		arr[i] = rand() % ELEMENT_COUNT + 10;
	}

	// quick sort the vector
	Mist::BucketSort(std::begin(arr), std::end(arr), (size_t)10, ELEMENT_COUNT + 10);
	MIST_ASSERT(Mist::IsSorted(std::begin(arr), std::end(arr)));

	totalSortTime = 0.0;
	// Run the simulation multiple times
	for (size_t j = 0; j < SORTING_ITERATIONS; j++) {
		// clear the vector and restart
		m.clear();
		for (size_t i = 0; i < ELEMENT_COUNT; i++) {
			m.push_back(rand() % ELEMENT_COUNT);
		}

		BeginTimer();
		// quick sort the vector
		Mist::BucketSort(&m, (size_t)0, ELEMENT_COUNT);
		totalSortTime += EndTimer();

		MIST_ASSERT(Mist::IsSorted(std::begin(m), std::end(m)));
	}

	std::cout << totalSortTime << "ms" << std::endl;

	std::cout << "Sorting Tests Passed!" << std::endl;
}

void TestBitManipulations() {
	Mist::BitField mask = 0;
	// All the bits in the mask should be set, thus it's value should be max value
	MIST_ASSERT(Mist::SetLowerBitRange(0) == 0);
	MIST_ASSERT(Mist::SetLowerBitRange(1) == 1);
	MIST_ASSERT(Mist::SetUpperBitRange(sizeof(Mist::BitField) * 8) == std::numeric_limits<Mist::BitField>::max());
	MIST_ASSERT(Mist::SetBitRange(0, 2) == 3);
	MIST_ASSERT(Mist::SetBitRange(0, 3) == 7);
	MIST_ASSERT(Mist::SetBitRange(1, 3) == 6);
	MIST_ASSERT(Mist::CountBitsSet(mask) == 0);
	MIST_ASSERT(Mist::CountBitsSet(std::numeric_limits<Mist::BitField>::max()) == sizeof(Mist::BitField) * 8);
	
	Mist::BitField indices[] = { 0, 1 };
	MIST_ASSERT(Mist::GetBitMask(&indices[0], 1) == 1);
	MIST_ASSERT(Mist::GetBitMask(&indices[0], 2) == 3);
	MIST_ASSERT(Mist::GetBitFlag(1) == 2);

	mask = 0;
	MIST_ASSERT(Mist::SetBit(mask, 0) == 1);
	MIST_ASSERT(Mist::SetBit(mask, 1) == 2);
	mask = Mist::SetBit(mask, 0);
	MIST_ASSERT(Mist::SetBit(mask, 1) == 3);

	MIST_ASSERT(Mist::UnsetBit(mask, 0) == 0);
	MIST_ASSERT(Mist::ToggleBit(mask, 0) == 0);
	MIST_ASSERT(Mist::ToggleBit(mask, 1) == 3);

	MIST_ASSERT(Mist::IsBitSet(mask, 0) == true);
	MIST_ASSERT(Mist::IsBitSet(mask, 1) == false);

	MIST_ASSERT(Mist::GetBitRange(std::numeric_limits<Mist::BitField>::max(), 1, 3) == 6);
	MIST_ASSERT(Mist::GetBitRange(std::numeric_limits<Mist::BitField>::max(), 0, 2) == 3);

	size_t count;
	Mist::BitField ind[sizeof(size_t) * 8];
	Mist::GetIndividualBitIndices(mask, ind, &count);
	MIST_ASSERT(count == 1 && ind[0] == 0);
	Mist::GetIndividualBitIndices(3, ind, &count);
	MIST_ASSERT(count == 2 && ind[0] == 0 && ind[1] == 1);

	Mist::BitField masks[sizeof(size_t) * 8];
	Mist::GetIndividualBitFlags(mask, masks, &count);
	MIST_ASSERT(count == 1 && masks[0] == mask);

	Mist::GetIndividualBitFlags(3, masks, &count);
	MIST_ASSERT(count == 2 && masks[0] == 1 && masks[1] == 2);

	Mist::GetIndividualBitFlags(5, masks, &count);
	MIST_ASSERT(count == 2 && masks[0] == 1 && masks[1] == 4);

	MIST_ASSERT(Mist::GetMaskDifferences(3, 1) == 2);
	MIST_ASSERT(Mist::GetMaskDifferences(5, 3) == 2 + 4);
	MIST_ASSERT(Mist::GetMaskDifferences(8, 2) == 2 + 8);
}

void TestSingleList() {

	std::cout << "SingleList Tests" << std::endl;

	Mist::SingleList<size_t> list;
	
	MIST_ASSERT(list.Size() == 0);

	list.InsertAsFirst(0);
	MIST_ASSERT(list.Size() == 1);
	MIST_ASSERT(list.RetrieveNodeAt(0) == list.FirstNode());
	MIST_ASSERT(list.RetrieveNodeAt(0) == list.LastNode());
	MIST_ASSERT(list.FirstNode() == list.LastNode());

	MIST_ASSERT(*list.RetrieveValueAt(0) == *list.FirstValue());
	MIST_ASSERT(*list.RetrieveValueAt(0) == *list.LastValue());
	MIST_ASSERT(*list.FirstValue() == *list.LastValue());

	list.Remove(list.FirstNode());
	MIST_ASSERT(list.Size() == 0);

	list.InsertAsLast(2);
	MIST_ASSERT(*list.RetrieveValueAt(0) == 2);
	MIST_ASSERT(*list.FirstValue() == 2);
	MIST_ASSERT(*list.LastValue() == 2);

	list.InsertAsLast(10);
	MIST_ASSERT(*list.FirstValue() != *list.LastValue());
	MIST_ASSERT(list.FirstNode() != list.LastNode());
	MIST_ASSERT(*list.LastValue() == 10);
	MIST_ASSERT(*list.FirstValue() == 2);
	MIST_ASSERT(list.Size() == 2);

	list.InsertAfter(list.FirstNode(), 5);
	MIST_ASSERT(list.Size() == 3);
	MIST_ASSERT(*list.RetrieveValueAt(1) != *list.FirstValue());
	MIST_ASSERT(*list.RetrieveValueAt(1) != *list.LastValue());
	MIST_ASSERT(*list.RetrieveNodeAt(1)->GetValue() == *list.RetrieveValueAt(1));

	list.InsertAfter(list.LastNode(), 15);
	MIST_ASSERT(*list.LastValue() == 15);
	MIST_ASSERT(list.Size() == 4);
	
	list.InsertAsFirst(0);
	MIST_ASSERT(*list.FirstValue() == 0);
	MIST_ASSERT(list.Size() == 5);


	size_t previousValue = *list.FirstValue();
	Mist::SingleList<size_t>::Node* node = list.FirstNode();
	for (size_t i = 1; i < list.Size(); i++) {

		node = node->NextNode();
		MIST_ASSERT(*list.RetrieveValueAt(i) > previousValue);

		// Assure that the nodes match
		MIST_ASSERT(list.RetrieveNodeAt(i) == node);
	}

	list.InsertAfter(list.LastNode(), 20);
	MIST_ASSERT(*list.LastValue() == 20);

	MIST_ASSERT(list.Size() == 6);
	list.Remove(list.LastNode());
	MIST_ASSERT(list.Size() == 5);

	list.Clear();
	MIST_ASSERT(list.Size() == 0);

	list.InsertAsFirst(0);
	list.Remove(list.FirstNode());
	MIST_ASSERT(list.Size() == 0);
	for (size_t i = 0; i < 10; i++) {

		list.InsertAsLast(i);
	}
	MIST_ASSERT(list.Size() == 10);
	list.Remove(list.FirstNode());
	MIST_ASSERT(*list.FirstValue() == 1);
	MIST_ASSERT(*list.LastValue() == 9);

	for (auto& i : list) {
		std::cout << *i.GetValue() << std::endl;
	}

	// Assure that moving works
	Mist::SingleList<size_t> newList(std::move(list));

	std::cout << "Single List Tests Passed" << std::endl;
}

void TestAllocator()
{
	std::cout << "Cpp Allocator Tests" << std::endl;

	size_t* pointer = Mist::CppAllocator::Alloc<size_t>(5);
	MIST_ASSERT(pointer != nullptr);
	Mist::CppAllocator::Free(pointer);

	void* block = Mist::CppAllocator::Alloc(100);

	*(size_t*)block = 10;
	MIST_ASSERT(block != nullptr);
	void* newBlock = Mist::CppAllocator::Realloc(block, 200);

	block = nullptr;

	MIST_ASSERT(newBlock != nullptr);
	// Assure that the memory got copied
	MIST_ASSERT(*(size_t*)newBlock == 10);

	void* newnewBlock = Mist::CppAllocator::Realloc(newBlock, 50);

	newBlock = nullptr;

	MIST_ASSERT(newnewBlock != nullptr);

	// Assure that the memory got copied
	MIST_ASSERT(*(size_t*)newnewBlock == 10);

	Mist::CppAllocator::Free(newnewBlock);
	
	std::cout << "Cpp Allocator Tests passed" << std::endl;
}

void TestDynamicArray() {

	std::cout << "Testing Dynamic Array" << std::endl;

	{
		Mist::DynamicArray<size_t> testArray;
		MIST_ASSERT(testArray.Size() == 0);

		testArray.Resize(10, 0);

		MIST_ASSERT(testArray.Size() == 10);

		for (auto i : testArray) {
			MIST_ASSERT(i == 0);
		}

		testArray.Clear();
		MIST_ASSERT(testArray.Size() == 0);

		testArray.InsertAsLast(10);
		MIST_ASSERT(testArray.Size() == 1);
		MIST_ASSERT(*testArray.LastValue() == 10);
		MIST_ASSERT(*testArray.FirstValue() == 10);
		MIST_ASSERT(*testArray.GetValue(0) == 10);
		MIST_ASSERT(testArray[0] == 10);

		testArray.RemoveLast();
		MIST_ASSERT(testArray.Size() == 0);

		testArray.Resize(10);
		for (size_t i = 0; i < 10; ++i) {
			MIST_ASSERT(*testArray.GetValue(i) == 0);
			MIST_ASSERT(testArray[i] == 0);
		}
	}

	{
		size_t constructionCount = 0;
		struct MemoryTracker {

			MemoryTracker(size_t* counter) {
				(*counter)++;
				c = counter;
			}

			MemoryTracker(const MemoryTracker& tracker) {
				c = tracker.c;
				(*c)++;
			}

			~MemoryTracker() {
				(*c)--;
			}

			size_t* c;
		};

		Mist::DynamicArray<MemoryTracker> testArray(10);
		MIST_ASSERT(testArray.ReservedSize() >= 10);

		// loop through the whole allocated raw array, even if the memory might be garbage set all the values to zero
		// this is to assure it doesnt crash
		for (size_t i = 0; i < testArray.ReservedSize(); i++) {
			memset(testArray.AsRawArray() + i, 0, sizeof(MemoryTracker));
		}

		testArray.InsertAsLast(MemoryTracker(&constructionCount));
		MIST_ASSERT(constructionCount == 1);

		testArray.Resize(10, &constructionCount);
		MIST_ASSERT(constructionCount == 10);

		MIST_ASSERT(testArray.Size() == 10);
		MIST_ASSERT(testArray.ReservedSize() <= testArray.Size());

		testArray.ReserveAdditional(10);
		MIST_ASSERT(testArray.ReservedSize() >= testArray.Size() + 10);
		testArray.ShrinkToSize();
		MIST_ASSERT(testArray.ReservedSize() == testArray.Size());

		Mist::DynamicArray<MemoryTracker> newTracker = std::move(testArray);

		MIST_ASSERT(constructionCount > 0);
		
		newTracker.Clear();
		MIST_ASSERT(constructionCount == 0);
	}



	std::cout << "Dynamic Array Tests Passed" << std::endl;
}

int main() {

	TestRingBuffer();
	TestSorting();
	TestBitManipulations();
	TestReflection();
	TestHash();
	TestSingleList();
	TestAllocator();
	TestDynamicArray();

	Pause();
	return 0;
}
