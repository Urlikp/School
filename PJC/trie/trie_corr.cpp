#include "trie.hpp"

#include <utility>
#include <algorithm>
#include <cassert>
#include <iostream>

void removeNodes(trie_node* node);


trie::trie(const std::vector<std::string>& strings) {
	for (std::string s : strings) {
		this->insert(s);
	}
}

trie::trie() = default;

trie::trie(const trie& rhs) {
	trie::const_iterator iter = rhs.begin();
	while (iter != nullptr) {
		std::string s = *iter;
		this->insert(s);
		iter++;
	}
}
trie& trie::operator=(const trie& rhs) {
	if ((*this) == rhs) {
		return *this;
	}
	trie::const_iterator iter1 = begin();
	trie::const_iterator iter2 = rhs.begin();
	while (iter1 != nullptr) {
		this->erase(*iter1);
		iter1++;
	}

	while (iter2 != nullptr) {
		this->insert(*iter2);
		iter2++;
	}
	return *this;
}
trie::trie(trie&& rhs) {
	swap(rhs);
}
trie& trie::operator=(trie&& rhs) {
	swap(rhs);
	return *this;
}
//::~trie() = default;

trie::~trie() {
	removeNodes(m_root);

	
	/*std::vector<trie_node*> nodes;
	trie_node* cur = m_root;
	if (m_root == nullptr) {
		return;
	}
	while (true) {
		for (int x = 0; x < num_chars; x++) {
			if ((*cur).children[x] != 0) {
				nodes.push_back(cur->children[x]);
			}			
		}
		delete cur;
		cur = nodes.back();
		nodes.pop_back();
		if (nodes.empty()) {
			break;
		}
	}*/
	

}

void removeNodes(trie_node* node)
{
	if (node == nullptr) {
		return;
	}
	else {
		char c = node->payload;
		for (int i = 0; i < num_chars; i++) {
			if (node->children[i] != 0) {
				removeNodes(node->children[i]);
			}
			
		}
		delete node;
	}
}



bool trie::insert(const std::string& str) {
	if (this->contains(str)) {
		return false;
	}
	else {
		if (m_root == nullptr) {
			
			struct trie_node* newNode = new struct trie_node;
			m_root = newNode;
		}
		if (str == "") {
			m_root->is_terminal = true;
			m_size++;
			return true;
		}
		struct trie_node* currentNode = m_root;
		int indexTerminal = 0;
		int strLength = str.length();
		for (char c: str) {


			bool isThere = false;
			int index = 0;

			if ((*currentNode).children[(int)c] != 0) {
				currentNode = (*currentNode).children[(int)c];
				isThere = true;
			}

			if (!isThere) {
				struct trie_node* newNode = new struct trie_node;
				(*newNode).parent = currentNode;
				(*newNode).payload = c;
				(*currentNode).children[(int)c] = newNode;
				currentNode = newNode;				
			}
			if (indexTerminal == (strLength - 1)) {
				(*currentNode).is_terminal = true;
				m_size++;
			}
			indexTerminal++;
		}
	}
	return true;
}


bool trie::contains(const std::string& str) const {
	if (m_root == nullptr) {
		return false;
	}
	if (str == "") {
		if (m_root->is_terminal) {
			return true;
		}
		return false;
	}
	struct trie_node* current = m_root;
	int index = 0;
	for (char c : str) {
		bool containsChar = false;

		if ((*current).children[(int)c] != 0) {
			containsChar = true;
			current = (*current).children[(int)c];
		}
		if (!containsChar) {
			return false;
		}
		if (index == (str.length() - 1)) {
			if ((*current).is_terminal) {
				return true;
			}
		}
		index++;
	}
	return false;
}

bool trie::empty() const {
	return m_size == 0;
}

bool trie::erase(const std::string& str) {
	if (!contains(str)) {
		return false;
	}
	else {
		if (m_root == nullptr) {
			return false;
		}
		if (str == "") {
			m_root->is_terminal = false;
			m_size--;
		}
		struct trie_node* current = m_root;
		int index = 0;
		for (char c : str) {

			if ((*current).children[(int)c] != 0) {
				current = (*current).children[(int)c];
			}
			if (index == (str.length() - 1)) {
				(*current).is_terminal = false;
				m_size--;
			}
			index++;
		}
	}
	return true;
}

size_t trie::size() const {
	return m_size;
}

trie_node* getNextTerminal(const trie_node* lastNode) {
	trie_node* res = nullptr;
	int lastIndex = 0;
	if (lastNode->parent == nullptr) {
		lastIndex = 0;
	}

	trie_node* actual = nullptr;
	for (int x = lastIndex; x < num_chars; x++) {
		if (lastNode->children[x] != 0) {
			actual = lastNode->children[x];
			break;
		}
	}
	int lastpayl = 0;
	bool up = true;
	if (actual == nullptr) {
		if (lastNode->parent == nullptr) {
			return nullptr;
		}
		else {
			actual = lastNode->parent;
			lastpayl = lastNode->payload + 1;
			up = false;
		}
	}
	else {
		lastpayl = 0;
	}
	if (actual->is_terminal && up) {
		return actual;
	}
	bool haveChild = false;

	while (true) {
		if (actual == nullptr) {
			return nullptr;
		}
		haveChild = false;
		for (int x = lastpayl; x < num_chars; x++) {
			if (actual->children[x] != nullptr) {
				haveChild = true;
				actual = actual->children[x];
				break;
			}
		}
		if (haveChild) {
			if (actual->is_terminal) {
				return actual;
			}
			else {
				lastpayl = 0;
			}
		}
		else {
			lastpayl = actual->payload + 1;
			actual = actual->parent;
		}
	}
	return nullptr;
}


trie::const_iterator::const_iterator(const trie_node* node) {
	node;
	this->current_node = node;
}
trie_node* getThisNodeNotConst(const trie_node* node) {
	char payl = node->payload;
	trie_node* res = nullptr;
	if (node->parent != nullptr) {
		trie_node* parent = node->parent;
		for (int x = 0; x < num_chars; x++) {
			if (parent->children[x]->payload == payl) {
				return parent->children[x];
			}
		}
	}
	else {
		for (int x = 0; x < num_chars; x++) {
			if (node->children[x] != nullptr) {
				res = node->children[x]->parent;
			}
		}
	}
	return res;
}

trie_node* getRoot(trie_node* node) {
	while (node->parent != nullptr) {
		node = node->parent;
	}
	return node;
}

trie::const_iterator& trie::const_iterator::operator++() {
	(*this) = trie::const_iterator(getNextTerminal(current_node));

	return (*this);
}

trie::const_iterator trie::const_iterator::operator++(int) {
	(*this) = trie::const_iterator(getNextTerminal(current_node));
	return (*this);
}

trie::const_iterator::reference trie::const_iterator::operator*() const {
	std::string res = "";
	if (this->current_node == nullptr) {
		return "";
	}
	if (this->current_node->payload == 0) {
		return "";
	}
	if (this->current_node->parent->payload == 0) {
		res += current_node->payload;
		return res;
	}
	res += this->current_node->payload;
	trie_node *node = this->current_node->parent;
	while (node->parent != nullptr) {
		res += node->payload;
		node = node->parent;
	}
	std::reverse(res.begin(), res.end());
	return res;
}

bool trie::const_iterator::operator==(const const_iterator& rhs) const {
	std::string s1 = *(*this);
	std::string s2 = *rhs;
	return (s1 == s2);
}

bool trie::const_iterator::operator!=(const const_iterator& rhs) const {
	return !(*this == rhs);
}

trie::const_iterator trie::begin() const{
	if (m_root == nullptr) return trie::const_iterator(nullptr);

	trie_node *node = m_root;
	if ((*node).is_terminal) {
		return trie::const_iterator(node);
	}
	trie_node* res = getNextTerminal(m_root);
	return trie::const_iterator(res);
}


trie::const_iterator trie::end() const{
	return nullptr;
}


std::vector<std::string> trie::search_by_prefix(const std::string& prefix) const {
	std::vector<std::string> res;
	return res;
}

/**
 * Vrátí všechny řetězce z trie, jež jsou prefixem daného řetězce.
 *
 * Prefixy jsou inkluzivní, tj. get_prefixes("abc") vrátí mezi výsledky
 * i "abc", pokud je "abc" v trii.
 */

std::vector<std::string> trie::get_prefixes(const std::string& str) const {
	std::vector<std::string> res;
	return res;
}

/**
 * Prohodí všechny prvky mezi touto trií a `rhs`.
 *
 * Složitost: O(1)
 */
 
void trie::swap(trie& rhs) {
	trie_node* helpNode = this->m_root;
	size_t helpSize = this->m_size;
	this->m_root = rhs.m_root;
	this->m_size = rhs.m_size;
	rhs.m_root = helpNode;
	rhs.m_size = helpSize;
}

// Relační operátory

/**
 * Vrací `true` pokud je trie `rhs` roven této.
 *
 * Dvě trie si jsou rovny, pokud obsahují stejné prvky.
 */
 
bool trie::operator==(const trie& rhs) const {
	trie::const_iterator iter1 = begin();
	trie::const_iterator iter2 = rhs.begin();
	int pocet = 0;
	std::string s = *iter1;
	std::string s2 = *iter2;
	if (this->m_size != rhs.m_size) {
		return false;
	}
	while(iter1 != nullptr) {
		if (iter1 != iter2) {
			return false;
		}
		iter1++;
		iter2++;
	}
	return true;
}

/**
 * Vrací `true` pokud je tato trie menší než `rhs`.
 *
 * Trie jsou porovnávány tak, že všechny řetězce v lexikografickém pořadí
 * se porovnají lexikograficky.
 * To znamená, že ["abc"] < ["abc", "b"], ["aac", "b"] < ["abc", "b"].
 */

bool trie::operator<(const trie& rhs) const {
	trie::const_iterator iter1 = this->begin();
	trie::const_iterator iter2 = rhs.begin();
	while (iter1 != nullptr) {
		if (iter2 == nullptr) {
			return false;
		}
		if (iter1 != iter2) {
			std::string t1 = *iter1;
			std::string t2 = *iter2;
			int l = 0;
			if (t1.length() > t2.length()) {
				l = t1.length();
			}
			else {
				l = t2.length();
			}
			for (int x = 0; x < l; x++) {
				if (t1[x] == t2[x]) {

				}
				else if (t1[x] < t2[x]) {
					return true;
				}
				else{
					return false;
				}
			}
		}
		iter1++;
		iter2++;
	}
	if (iter2 != nullptr) {
		return true;
	}
	return false;
}

/**
 * Vrátí novou trii, která obsahuje průnik této a `rhs`, tj. řežezce
 * přítomné v obou.
 *
 * Složitost: Implementace by neměla zbytečně procházet prvky, o kterých už ví,
 *            že v druhé trii být nemohou (např. ["aa", "ab", "ac"...], ["x"]).
 */

trie trie::operator&(trie const& rhs) const {
	trie res;
	return res;
}

/**
 * Vrátí novou trii, která obsahuje sjednocení této a `rhs`, tj. řežezce
 * přítomné alespoň v jedné z nich.
 */

trie trie::operator|(trie const& rhs) const {
	trie res;
	return res;
}

//! 2 trie jsou si nerovné právě tehdy, když si nejsou rovné (viz operator==)
bool operator!=(const trie& lhs, const trie& rhs) {
	if (lhs == rhs) {
		return false;
	}
	return true;
}
//! Lexicografické uspořádání, viz operator<
bool operator<=(const trie& lhs, const trie& rhs) {
	if (lhs < rhs) {
		return true;
	}
	if (lhs == rhs) {
		return true;
	}
	return false;
}
//! Lexicografické uspořádání, viz operator<
bool operator>(const trie& lhs, const trie& rhs) {
	bool mensi = false;
	bool rovnase = false;
	if (lhs < rhs) {
		mensi = true;
	}
	if (lhs == rhs) {
		rovnase = true;
	}
	if (!mensi && !rovnase) {
		return true;
	}
	return false;
}
//! Lexicografické uspořádání, viz operator<
bool operator>=(const trie& lhs, const trie& rhs) {
	bool mensi = false;
	bool rovnase = false;
	if (lhs < rhs) {
		mensi = true;
	}
	if (lhs == rhs) {
		rovnase = true;
	}
	if (!mensi) {
		return true;
	}
	return false;
}

/**
 * ADL customization point pro std::swap.
 * Výsledek `swap(lhs, rhs);` by měl být ekvivalentní s výsledkem
 * `lhs.swap(rhs);`
 */

void swap(trie& lhs, trie& rhs) {
	lhs.swap(rhs);
}

std::ostream& operator<<(std::ostream& out, trie const& trie)
{
	std::cout << "";
	return out;
}
