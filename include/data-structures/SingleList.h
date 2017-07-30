#pragma once

#include "../common/UtilityMacros.h"
#include <type_traits>

MIST_NAMESPACE

// SingleList is a simple singly linked list
template< typename ValueType >
class SingleList {

public:

	class Node;

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
	void PushFront(WriteType&& writeValue);

	// Write a value into the single list at the front
	template< typename WriteType,
		// @Template Condition: the write type must be convertible to value type
		typename Condition = typename std::enable_if<std::is_convertible<WriteType, ValueType>::value>::type >
	void PushBack(WriteType&& writeValue);

	// Remove this node
	void Remove(Node* node);


	// Find the node that passes the find method
	template< typename CheckMethod >
	Node* Find(CheckMethod findMethod);

	// Find the node that passes the findMethod
	template< typename CheckMethod >
	bool TryFind(CheckMethod findMethod, Node** outNode);

	// Retrieve the value stored at index, this operation runs at O(n) time
	ValueType* Get(size_t index);

	// Retrieve the node at index, this operation runs at O(n) time
	Node* GetNode(size_t index);

	// Get the head of the list
	ValueType* Front();

	// Get thee back of the list
	ValueType* Back();

	// Get the front node of the list, this is the head
	Node* FrontNode();

	// Get the back node of the list
	Node* BackNode();

	size_t Size() const;

	void Clear();

	// -Structors-

	~SingleList();

	class Node {

	public:

		ValueType* operator->();

		// Retrieve the value of the node
		ValueType* Get();

		// Retrieve the next node
		Node* Next();

		friend SingleList<ValueType>;

	private:

		// Create a node with the designated value type
		template< typename WriteType,
			// @TemplateCondition: The write type must be of ValueType
			typename Condition = typename std::enable_if<std::is_convertible<WriteType, ValueType>::value>::type >
		Node(WriteType&& writeValue);

		Node* m_Next = nullptr;
		ValueType m_Value;
	};

private:

	Node* m_Head = nullptr;
	Node* m_Tail = nullptr;
};


// -Implementation-

// -SingleList-
template< typename ValueType >
// Write a value into the single list after the specified node
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType>::InsertAfter(Node* node, WriteType&& writeValue) {

	if (node == m_Tail) {
		PushBack(std::forward<WriteType>(writeValue));
		return;
	}

	MIST_ASSERT(node != nullptr);
	Node* newNode = new Node(std::forward<WriteType>(writeValue));
	newNode->m_Next = node->Next();
	node->m_Next = newNode;
}

template< typename ValueType >
// Write a value into the single list at the front
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType>::PushFront(WriteType&& writeValue) {

	if (m_Head == nullptr) {

		MIST_ASSERT(m_Tail == nullptr);
		m_Head = new Node(std::forward<WriteType>(writeValue));
		m_Tail = m_Head;
	}
	else {

		Node* newNode = new Node(std::forward<WriteType>(writeValue));
		newNode->m_Next = m_Head;
		m_Head = newNode;
	}
}

template< typename ValueType >
// Write a value into the single list at the front
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType>::PushBack(WriteType&& writeValue) {

	if (m_Tail == nullptr) {
		
		MIST_ASSERT(m_Head == nullptr);
		m_Tail = new Node(std::forward<WriteType>(writeValue));
		m_Head = m_Tail;
	}
	else {

		MIST_ASSERT(m_Tail->m_Next == nullptr);
		m_Tail->m_Next = new Node(std::forward<WriteType>(writeValue));
		m_Tail = m_Tail->Next();
	}
}

template< typename ValueType >
// Remove this node
void SingleList<ValueType>::Remove(Node* node) {

	MIST_ASSERT(node != nullptr);

	Node* previousNode = nullptr;
	Node* currentNode = m_Head;

	if (node == m_Head) {

		if (m_Head == m_Tail) {
			m_Tail = nullptr;
		}

		m_Head = node->Next();
		delete node;
		return;
	}
	else if (node == m_Tail) {

		if (m_Head == m_Tail) {
			m_Tail = nullptr;
		}
		else {
			m_Tail = GetNode(Size() - 2);
		}

		m_Tail->m_Next = nullptr;
		delete node;
		return;
	}

	while (currentNode != nullptr) {

		if (currentNode == node) {
			
			delete currentNode;
			previousNode->m_Next = nullptr;
			break;
		}

		previousNode = currentNode;
		currentNode = previousNode->Next();
	}
}


template< typename ValueType >
// Find the node that passes the find method
template< typename CheckMethod >
typename SingleList<ValueType>::Node* SingleList<ValueType>::Find(CheckMethod findMethod) {

	Node* currentNode = m_Head;

	// Loop through every node and determine the node that matches
	while (currentNode != nullptr) {
		
		if (findMethod(*currentNode->Get())) {
			return currentNode;
		}

		currentNode = currentNode->Next();
	}
	return nullptr;
}

template< typename ValueType >
// Find the node that passes the findMethod
template< typename CheckMethod >
bool SingleList<ValueType>::TryFind(CheckMethod findMethod, Node** outNode) {

	MIST_ASSERT(outNode != nullptr);

	*outNode = Find(findMethod);
	return *outNode != nullptr;
}

template< typename ValueType >
// Retrieve the value stored at index, this operation runs at O(n) time
ValueType* SingleList<ValueType>::Get(size_t index) {

	return GetNode(index)->Get();
}

template< typename ValueType >
// Retrieve the node at index, this operation runs at O(n) time
typename SingleList<ValueType>::Node* SingleList<ValueType>::GetNode(size_t index) {

	MIST_ASSERT(index < Size());
	
	// Move through the nodes until the specified index
	Node* next = m_Head;
	for (size_t i = 0; i < index; i++) {

		next = next->Next();
	}
	return next;
}

template< typename ValueType >
// Get the head of the list
ValueType* SingleList<ValueType>::Front() {

	MIST_ASSERT(m_Head != nullptr);
	return m_Head->Get();
}

template< typename ValueType >
// Get thee back of the list
ValueType* SingleList<ValueType>::Back() {

	MIST_ASSERT(m_Tail != nullptr);
	return m_Tail->Get();
}

template< typename ValueType >
// Get the front node of the list, this is the head
typename SingleList<ValueType>::Node* SingleList<ValueType>::FrontNode() {

	MIST_ASSERT(m_Head != nullptr);
	return m_Head;
}


template< typename ValueType >
typename SingleList<ValueType>::Node* SingleList<ValueType>::BackNode() {

	MIST_ASSERT(m_Tail != nullptr);
	return m_Tail;
}

template< typename ValueType >
// Get the back node of the list
size_t SingleList<ValueType>::Size() const {

	// If we have nothing, return 0
	if (m_Head == nullptr) {
		return 0;
	}

	size_t count = 0;
	Node* next = m_Head;
	while (next != nullptr) {

		count++;
		next = next->Next();
	}

	return count;
}

template< typename ValueType >
void SingleList<ValueType>::Clear() {

	Node* currentNode = m_Head;
	
	// Loop through all the nodes and delete them
	while (currentNode != nullptr) {
		
		Node* nextNode = currentNode->Next();
		delete currentNode;
		currentNode = nextNode;
	}

	m_Head = nullptr;
	m_Tail = nullptr;
}


template< typename ValueType >
SingleList<ValueType>::~SingleList() {

	Clear();
}

// -Node-

template< typename ValueType >
ValueType* SingleList<ValueType>::Node::operator->() {
	return &m_Value;
}

// Retrieve the value of the node
template< typename ValueType >
ValueType* SingleList<ValueType>::Node::Get() {

	return &m_Value;
}

// Retrieve the next node
template< typename ValueType >
typename SingleList<ValueType>::Node* SingleList<ValueType>::Node::Next() {

	return m_Next;
}


// Create a node with the designated value type
template< typename ValueType >
template< typename WriteType,
	// @TemplateCondition: The write type must be of ValueType
	typename Condition >
SingleList<ValueType>::Node::Node(WriteType&& writeValue) : m_Value(std::forward<WriteType>(writeValue)) {}


MIST_NAMESPACE_END
