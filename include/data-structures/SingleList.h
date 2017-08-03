#pragma once

#include "../common/UtilityMacros.h"
#include <type_traits>

MIST_NAMESPACE

// SingleList is a simple singly linked list
template< typename ValueType >
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


	// Find the node that meets the requirements of the predicate delegate
	template< typename Predicate >
	Node* FindNode(Predicate predicate);

	// Find the node that passes the findMethod
	template< typename Predicate >
	bool TryFindNode(Predicate predicate, Node** outNode);

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

	~SingleList();

	class Node {

	public:

		ValueType* operator->();

		// Retrieve the value of the node
		ValueType* GetValue();

		// Retrieve the next node
		Node* NextNode();

		Node* operator++();

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
template< typename ValueType >
// Write a value into the single list after the specified node
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType>::InsertAfter(Node* node, WriteType&& writeValue) {

	if (node == m_Tail) {
		InsertAsLast(std::forward<WriteType>(writeValue));
		return;
	}

	MIST_ASSERT(node != nullptr);
	Node* newNode = new Node(std::forward<WriteType>(writeValue));
	newNode->m_Next = node->NextNode();
	node->m_Next = newNode;
}

template< typename ValueType >
// Write a value into the single list at the front
template< typename WriteType,
	// @Template Condition: the write type must be convertible to value type
	typename Condition >
void SingleList<ValueType>::InsertAsFirst(WriteType&& writeValue) {

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
void SingleList<ValueType>::InsertAsLast(WriteType&& writeValue) {

	if (m_Tail == nullptr) {
		
		MIST_ASSERT(m_Head == nullptr);
		m_Tail = new Node(std::forward<WriteType>(writeValue));
		m_Head = m_Tail;
	}
	else {

		MIST_ASSERT(m_Tail->m_Next == nullptr);
		m_Tail->m_Next = new Node(std::forward<WriteType>(writeValue));
		m_Tail = m_Tail->NextNode();
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

		m_Head = node->NextNode();
		delete node;
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
		currentNode = previousNode->NextNode();
	}
}


template< typename ValueType >
// Find the node that passes the find method
template< typename Predicate >
typename SingleList<ValueType>::Node* SingleList<ValueType>::FindNode(Predicate findMethod) {

	Node* currentNode = m_Head;

	// Loop through every node and determine the node that matches
	while (currentNode != nullptr) {
		
		if (findMethod(*currentNode->GetValue())) {
			return currentNode;
		}

		currentNode = currentNode->NextNode();
	}
	return nullptr;
}

template< typename ValueType >
// Find the node that passes the findMethod
template< typename Predicate >
bool SingleList<ValueType>::TryFindNode(Predicate findMethod, Node** outNode) {

	MIST_ASSERT(outNode != nullptr);

	*outNode = FindNode(findMethod);
	return *outNode != nullptr;
}

template< typename ValueType >
// Retrieve the value stored at index, this operation runs at O(n) time
ValueType* SingleList<ValueType>::RetrieveValueAt(size_t index) {

	return RetrieveNodeAt(index)->GetValue();
}

template< typename ValueType >
// Retrieve the node at index, this operation runs at O(n) time
typename SingleList<ValueType>::Node* SingleList<ValueType>::RetrieveNodeAt(size_t index) {

	MIST_ASSERT(index < Size());
	
	// Move through the nodes until the specified index
	Node* next = m_Head;
	for (size_t i = 0; i < index; i++) {

		next = next->NextNode();
	}
	return next;
}

template< typename ValueType >
// Get the head of the list
ValueType* SingleList<ValueType>::FirstValue() {

	MIST_ASSERT(m_Head != nullptr);
	return m_Head->GetValue();
}

template< typename ValueType >
// Get thee back of the list
ValueType* SingleList<ValueType>::LastValue() {

	MIST_ASSERT(m_Tail != nullptr);
	return m_Tail->GetValue();
}

template< typename ValueType >
// Get the front node of the list, this is the head
typename SingleList<ValueType>::Node* SingleList<ValueType>::FirstNode() {

	MIST_ASSERT(m_Head != nullptr);
	return m_Head;
}


template< typename ValueType >
typename SingleList<ValueType>::Node* SingleList<ValueType>::LastNode() {

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
		next = next->NextNode();
	}

	return count;
}

template< typename ValueType >
void SingleList<ValueType>::Clear() {

	Node* currentNode = m_Head;
	
	// Loop through all the nodes and delete them
	while (currentNode != nullptr) {
		
		Node* nextNode = currentNode->NextNode();
		delete currentNode;
		currentNode = nextNode;
	}

	m_Head = nullptr;
	m_Tail = nullptr;
}

template< typename ValueType >
typename SingleList<ValueType>::Iterator SingleList<ValueType>::begin() {

	return Iterator(FirstNode());
}

template< typename ValueType >
typename SingleList<ValueType>::Iterator SingleList<ValueType>::end() {

	return Iterator(nullptr);
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
ValueType* SingleList<ValueType>::Node::GetValue() {

	return &m_Value;
}

// Retrieve the next node
template< typename ValueType >
typename SingleList<ValueType>::Node* SingleList<ValueType>::Node::NextNode() {

	return m_Next;
}

template< typename ValueType >
typename SingleList<ValueType>::Node* SingleList<ValueType>::Node::operator++() {
	
	return NextNode();
}


// Create a node with the designated value type
template< typename ValueType >
template< typename WriteType,
	// @TemplateCondition: The write type must be of ValueType
	typename Condition >
SingleList<ValueType>::Node::Node(WriteType&& writeValue) : m_Value(std::forward<WriteType>(writeValue)) {}


template< typename ValueType >
typename SingleList<ValueType>::Iterator SingleList<ValueType>::Iterator::operator++() {

	m_TargetNode = m_TargetNode->NextNode();
	return *this;
}

template< typename ValueType >
bool SingleList<ValueType>::Iterator::operator!=(const Iterator& rhs) {

	return rhs.m_TargetNode != m_TargetNode;
}

template< typename ValueType >
SingleList<ValueType>::Iterator::Iterator(typename SingleList<ValueType>::Node* node) : m_TargetNode(node) {}

template< typename ValueType >
typename SingleList<ValueType>::Node& SingleList<ValueType>::Iterator::operator*() {

	return *m_TargetNode;
}

template< typename ValueType >
typename SingleList<ValueType>::Node* SingleList<ValueType>::Iterator::operator->() {

	return m_TargetNode;
}


MIST_NAMESPACE_END
