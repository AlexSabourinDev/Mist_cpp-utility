#pragma once

#include "../../Mist_common/include/common/UtilityMacros.h"
#include "../allocators/CppAllocator.h"
#include <type_traits>

MIST_NAMESPACE

// SingleList is a simple singly linked list
template< typename ValueType, typename Allocator = CppAllocator >
class SingleList {

public:

	class Node;
	class Iterator;

	// -Public API-

	// Write a value into the single list after the specified node
	template< typename WriteType,
		// @Template Condition: the write type must be convertible to value type
		typename Condition = typename std::enable_if<std::is_convertible<WriteType, ValueType>::value>::type >
	void InsertAfter(Node* node, WriteType&& writeValue);

	// Write a value into the single list at the front
	template< typename WriteType,
		// @Template Condition: the write type must be convertible to value type
		typename Condition = typename std::enable_if<std::is_convertible<WriteType, ValueType>::value>::type >
	void InsertAsFirst(WriteType&& writeValue);

	// Write a value into the single list at the back
	template< typename WriteType,
		// @Template Condition: the write type must be convertible to value type
		typename Condition = typename std::enable_if<std::is_convertible<WriteType, ValueType>::value>::type >
	void InsertAsLast(WriteType&& writeValue);

	void Remove(Node* node);



	// Retrieve the value stored at index, this operation runs at O(n) time
	ValueType* RetrieveValueAt(size_t index);

	// Retrieve the node at index, this operation runs at O(n) time
	Node* RetrieveNodeAt(size_t index);

	ValueType* FirstValue();

	ValueType* LastValue();

	Node* FirstNode();

	Node* LastNode();

	size_t Size() const;

	void Clear();

	// -Iterators-

	Iterator begin();
	Iterator end();

	// -Structors-

	SingleList() = default;
	~SingleList();

	// Copying is currently disalllowed in the singlelist this is to avoid accidental copying, if it is desired,
	// an explicit copy method would be prefered, preferably outside this class in order to avoid
	// cluttering the api
	SingleList(const SingleList&) = delete;
	SingleList& operator=(const SingleList&) = delete;

	SingleList(SingleList&& rhs);
	SingleList& operator=(SingleList&& rhs);

	class Node {

	public:

		ValueType* operator->();

		// Retrieve the value of the node
		ValueType* GetValue();

		// Retrieve the next node
		Node* NextNode();

		Node* operator++();

		friend SingleList<ValueType>;

		// Create a node with the designated value type
		template< typename WriteType,
			// @TemplateCondition: The write type must be of ValueType
			typename Condition = typename std::enable_if<std::is_convertible<WriteType, ValueType>::value>::type >
		Node(WriteType&& writeValue);

	private:

		Node* m_Next = nullptr;
		ValueType m_Value;
	};

	class Iterator {

	public:
		
		// -Public API-

		// Advance the iterator forward
		Iterator operator++();

		bool operator!=(const Iterator& rhs);

		Node& operator*();
		Node* operator->();

		// -Structors-
		Iterator(Node* node);

	private:

		Node* m_TargetNode = nullptr;
	};

private:

	Node* m_Head = nullptr;
	Node* m_Tail = nullptr;
};


// -Implementation-

// -SingleList-
template< typename ValueType, typename Allocator >
// Write a value into the single list after the specified node
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType, Allocator>::InsertAfter(Node* node, WriteType&& writeValue) {

	if (node == m_Tail) {
		InsertAsLast(std::forward<WriteType>(writeValue));
		return;
	}

	MIST_ASSERT(node != nullptr);
	Node* newNode = Allocator::template Alloc<Node>(std::forward<WriteType>(writeValue));
	newNode->m_Next = node->NextNode();
	node->m_Next = newNode;
}

template< typename ValueType, typename Allocator >
// Write a value into the single list at the front
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType, Allocator>::InsertAsFirst(WriteType&& writeValue) {

	if (m_Head == nullptr) {

		MIST_ASSERT(m_Tail == nullptr);
		m_Head = Allocator::template Alloc<Node>(std::forward<WriteType>(writeValue));
		m_Tail = m_Head;
	}
	else {

		Node* newNode = Allocator:: template Alloc<Node>(std::forward<WriteType>(writeValue));
		newNode->m_Next = m_Head;
		m_Head = newNode;
	}
}

template< typename ValueType, typename Allocator >
// Write a value into the single list at the front
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType, Allocator>::InsertAsLast(WriteType&& writeValue) {

	if (m_Tail == nullptr) {
		
		MIST_ASSERT(m_Head == nullptr);
		m_Tail = Allocator:: template Alloc<Node>(std::forward<WriteType>(writeValue));
		m_Head = m_Tail;
	}
	else {

		MIST_ASSERT(m_Tail->m_Next == nullptr);
		m_Tail->m_Next = Allocator:: template Alloc<Node>(std::forward<WriteType>(writeValue));
		m_Tail = m_Tail->NextNode();
	}
}

template< typename ValueType, typename Allocator >
// Remove this node
void SingleList<ValueType, Allocator>::Remove(Node* node) {

	MIST_ASSERT(node != nullptr);

	Node* previousNode = nullptr;
	Node* currentNode = m_Head;

	if (node == m_Head) {

		if (m_Head == m_Tail) {
			m_Tail = nullptr;
		}

		m_Head = node->NextNode();
		Allocator::Free(node);
		return;
	}
	else if (node == m_Tail) {

		if (m_Head == m_Tail) {
			m_Tail = nullptr;
		}
		else {
			m_Tail = RetrieveNodeAt(Size() - 2);
		}

		m_Tail->m_Next = nullptr;
		Allocator::Free(node);
		return;
	}

	while (currentNode != nullptr) {

		if (currentNode == node) {
			
			Allocator::Free(currentNode);
			previousNode->m_Next = nullptr;
			break;
		}

		previousNode = currentNode;
		currentNode = previousNode->NextNode();
	}
}


template< typename ValueType, typename Allocator >
// Retrieve the value stored at index, this operation runs at O(n) time
ValueType* SingleList<ValueType, Allocator>::RetrieveValueAt(size_t index) {

	return RetrieveNodeAt(index)->GetValue();
}

template< typename ValueType, typename Allocator >
// Retrieve the node at index, this operation runs at O(n) time
typename SingleList<ValueType, Allocator>::Node* SingleList<ValueType, Allocator>::RetrieveNodeAt(size_t index) {

	MIST_ASSERT(index < Size());
	
	// Move through the nodes until the specified index
	Node* next = m_Head;
	for (size_t i = 0; i < index; i++) {

		next = next->NextNode();
	}
	return next;
}

template< typename ValueType, typename Allocator >
// Get the head of the list
ValueType* SingleList<ValueType, Allocator>::FirstValue() {

	MIST_ASSERT(m_Head != nullptr);
	return m_Head->GetValue();
}

template< typename ValueType, typename Allocator >
// Get thee back of the list
ValueType* SingleList<ValueType, Allocator>::LastValue() {

	MIST_ASSERT(m_Tail != nullptr);
	return m_Tail->GetValue();
}

template< typename ValueType, typename Allocator >
// Get the front node of the list, this is the head
typename SingleList<ValueType, Allocator>::Node* SingleList<ValueType, Allocator>::FirstNode() {

	MIST_ASSERT(m_Head != nullptr);
	return m_Head;
}


template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Node* SingleList<ValueType, Allocator>::LastNode() {

	MIST_ASSERT(m_Tail != nullptr);
	return m_Tail;
}

template< typename ValueType, typename Allocator >
// Get the back node of the list
size_t SingleList<ValueType, Allocator>::Size() const {

	// If we have nothing, return 0
	if (m_Head == nullptr) {
		return 0;
	}

	size_t count = 0;
	Node* next = m_Head;
	while (next != nullptr) {

		count++;
		next = next->NextNode();
	}

	return count;
}

template< typename ValueType, typename Allocator >
void SingleList<ValueType, Allocator>::Clear() {

	typename SingleList<ValueType, Allocator>::Node* currentNode = m_Head;
	
	// Loop through all the nodes and delete them
	while (currentNode != nullptr) {
		
		Node* nextNode = currentNode->NextNode();
		Allocator::Free(currentNode);
		currentNode = nextNode;
	}

	m_Head = nullptr;
	m_Tail = nullptr;
}

template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Iterator SingleList<ValueType, Allocator>::begin() {

	return Iterator(FirstNode());
}

template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Iterator SingleList<ValueType, Allocator>::end() {

	return Iterator(nullptr);
}

template< typename ValueType, typename Allocator >
SingleList<ValueType, Allocator>::SingleList(SingleList&& rhs) {

	std::swap(m_Head, rhs.m_Head);
	std::swap(m_Tail, rhs.m_Tail);
}

template< typename ValueType, typename Allocator >
SingleList<ValueType, Allocator>& SingleList<ValueType, Allocator>::operator=(SingleList&& rhs) {

	std::swap(m_Head, rhs.m_Head);
	std::swap(m_Tail, rhs.m_Tail);

	return *this;
}

template< typename ValueType, typename Allocator >
SingleList<ValueType, Allocator>::~SingleList() {

	Clear();
}

// -Node-

template< typename ValueType, typename Allocator >
ValueType* SingleList<ValueType, Allocator>::Node::operator->() {
	return &m_Value;
}

// Retrieve the value of the node
template< typename ValueType, typename Allocator >
ValueType* SingleList<ValueType, Allocator>::Node::GetValue() {

	return &m_Value;
}

// Retrieve the next node
template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Node* SingleList<ValueType, Allocator>::Node::NextNode() {

	return m_Next;
}

template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Node* SingleList<ValueType, Allocator>::Node::operator++() {
	
	return NextNode();
}


// Create a node with the designated value type
template< typename ValueType, typename Allocator >
template< typename WriteType,
	// @TemplateCondition: The write type must be of ValueType
	typename Condition >
SingleList<ValueType, Allocator>::Node::Node(WriteType&& writeValue) : m_Value(std::forward<WriteType>(writeValue)) {}


template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Iterator SingleList<ValueType, Allocator>::Iterator::operator++() {

	m_TargetNode = m_TargetNode->NextNode();
	return *this;
}

template< typename ValueType, typename Allocator >
bool SingleList<ValueType, Allocator>::Iterator::operator!=(const Iterator& rhs) {

	return rhs.m_TargetNode != m_TargetNode;
}

template< typename ValueType, typename Allocator >
SingleList<ValueType, Allocator>::Iterator::Iterator(typename SingleList<ValueType, Allocator>::Node* node) : m_TargetNode(node) {}

template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Node& SingleList<ValueType, Allocator>::Iterator::operator*() {

	return *m_TargetNode;
}

template< typename ValueType, typename Allocator >
typename SingleList<ValueType, Allocator>::Node* SingleList<ValueType, Allocator>::Iterator::operator->() {

	return m_TargetNode;
}



MIST_NAMESPACE_END
