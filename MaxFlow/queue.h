#pragma once

template<typename T>
class Node {
private:
	T _data;
	Node *_next, *_prev;
public:
	Node(T newData) : _data(newData), _next(nullptr), _prev(nullptr) {}
	Node* next() const { return _next; }
	Node* prev() const { return _prev; }
	void next(Node* newNext) { _next = newNext; }
	void prev(Node* newPrev) { _prev = newPrev; }
	T data() const { return _data; }
};

template<typename T>
class Queue {
private:
	Node<T> *_head, *_tail;
	size_t _size;
public:
	Queue() : _size(0), _head(nullptr), _tail(nullptr) {}
	void push(T newData) {
		Node<T>* newNode =  new Node(newData);
		if (!_head) {
			_head = _tail = newNode;
		}
		else {
			_tail->next(newNode);
			_tail->next()->prev(_tail);
			_tail = _tail->next();
		}
		++_size;
	}
	T pop() {
		T toReturn = _head->data();
		Node<T>* toDel = _head;
		_head = _head->next();
		if (_head) _head->prev(nullptr);
		if (!_head) _tail = nullptr;
		delete toDel;
		--_size;
		return toReturn;
	}
	size_t size() const { return _size; }
	bool isEmpty() const { return _size == 0u; }
	bool contains(T el) const {
		auto curNode = _head;
		while (curNode != nullptr) {
			if (curNode->data() == el) return true;
			curNode = curNode->next();
		}
		return false;
	}

	T operator[](int i) const {
		if (i < 0) throw "Negative index in square brackets in extended queue!";
		auto curNode = _head;
		while (i > 0) { curNode = curNode->next(); --i; }
		return curNode->data();
	}
};