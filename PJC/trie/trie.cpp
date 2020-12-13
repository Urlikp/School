#include "trie.hpp"

#include <iostream>
#include <utility>
#include <algorithm>
#include <stack>

/**
 * Vytvoří trii obsahující všechny řetězce z daného vektoru
 */
trie::trie(const std::vector<std::string> &strings)
{
    for(const std::string& str : strings) {
        this->insert(str);
    }
}

trie::trie() = default;

trie::~trie()
{
    if (m_root != nullptr) {
        std::stack<trie_node*> nodes;
        nodes.push(m_root);

        while (!nodes.empty()) {
            auto current = nodes.top();
            nodes.pop();
            for (auto & child : current->children) {
                if (child) {
                    nodes.push(child);
                }
            }
            delete current;
        }
    }
}

/**
 * Odstraní daný řetězec z trie.
 * Vrátí true, pokud byl řetězec odstraněn (byl předtím v trii), jinak false.
 */
bool trie::erase(const std::string& str)
{
    if (contains(str)) {
        trie_node* ptr = m_root;

        for (char c : str) {
            ptr = ptr->children[(int)c];
        }

        ptr->is_terminal = false;

        bool used = false;
        trie_node* ptr_par = ptr->parent;

        while (ptr != m_root) {
            for (size_t i = 0; !used && i < num_chars; i++) {
                if (ptr->children[i]) {
                    used = true;
                }
            }
            if (!used) {
                char c = ptr->payload;
                delete ptr_par->children[(int)c];
                ptr_par->children[(int)c] = nullptr;
            } else {
                break;
            }
            ptr = ptr_par;
            ptr_par = ptr->parent;
        }

        m_size--;
        return true;
    }
    return false;
}

/**
 * Vloží řetězec do trie.
 * Vrátí true, pokud byl řetězec vložen (nebyl předtím v trii), jinak false.
 */
bool trie::insert(const std::string& str)
{
    if (!this->contains(str)) {
        if (m_root == nullptr) {
            trie_node* new_ptr = new trie_node;
            m_root = new_ptr;
        }

        trie_node* ptr = m_root;
        for (char c : str) {
            if (ptr->children[(int)c]) {
                ptr = ptr->children[(int)c];
            } else {
                ptr->children[(int)c] = new trie_node;
                ptr->children[(int)c]->parent = ptr;
                ptr->children[(int)c]->payload = c;
                ptr = ptr->children[(int)c];
            }
        }

        ptr->is_terminal = true;
        m_size++;
        return true;
    }
    return false;
}

/**
 * Vrátí true, pokud je řetězec v trii, jinak false.
 */
bool trie::contains(const std::string& str) const
{
    if (m_root == nullptr) {
        return false;
    }

    trie_node* ptr = m_root;
    for (char c : str) {
        if (ptr->children[(int)c]) {
            ptr = ptr->children[(int)c];
        } else {
            return false;
        }
    }

    return ptr->is_terminal;
}

/**
 * Vrátí počet unikátních řetězců v trii.
 */
size_t trie::size() const
{
    return m_size;
}

/**
 * Vrací `true` pokud v listu nejsou žádné prvky.
 */
bool trie::empty() const
{
    return m_size == 0;
}

trie::trie(const trie& rhs)
{
    for (auto str : rhs) {
        this->insert(str);
    }
}

trie& trie::operator=(const trie& rhs)
{
    if (this != &rhs) {
        trie tmp(rhs);
        swap(tmp);
    }
    return *this;
}

trie::trie(trie&& rhs)
{
    swap(rhs);
}

trie& trie::operator=(trie&& rhs)
{
    swap(rhs);
    return *this;
}

trie::const_iterator::const_iterator(const trie_node *node)
{
    this->current_node = node;
}

bool trie::const_iterator::operator==(const trie::const_iterator &rhs) const
{
    return current_node == rhs.current_node;
}

bool trie::const_iterator::operator!=(const trie::const_iterator &rhs) const
{
    return current_node != rhs.current_node;
}

trie::const_iterator &trie::const_iterator::operator++()
{
    if (current_node == nullptr) {
        return *this;
    }

    auto ptr = current_node;
    bool has_children = false;

    do {
        for (auto & child : ptr->children) {
            if (child) {
                ptr = child;
                has_children = true;
                break;
            }
        }
    } while (!ptr->is_terminal);

    if (has_children) {
        current_node = ptr;
        return *this;
    }

    int next_child = ((int) ptr->payload) + 1;
    ptr = ptr->parent;

    while (ptr != nullptr) {
        for (size_t i = next_child; i < num_chars; i++) {
            if (ptr->children[i]) {
                ptr = ptr->children[i];
                has_children = true;
                break;
            }
        }

        if (has_children) {
            while (!ptr->is_terminal) {
                for (auto & child : ptr->children) {
                    if (child) {
                        ptr = child;
                        //has_children = true;
                        break;
                    }
                }
            }

            current_node = ptr;
            return *this;
        }

        next_child = ((int) ptr->payload) + 1;
        ptr = ptr->parent;
    }

    current_node = nullptr;
    return *this;
}

trie::const_iterator trie::const_iterator::operator++(int)
{
    trie::const_iterator temp = *this;
    ++(*this);
    return temp;
}

trie::const_iterator::reference trie::const_iterator::operator*() const
{
    std::string str = "";
    auto* ptr = current_node;

    if (ptr == nullptr) {
        return str;
    }

    while(ptr->parent != nullptr) {
        str = ptr->payload + str;
        ptr = ptr->parent;
    }
    return str;
}

/**
 * Vrátí všechny prvky trie, které začínají daným prefixem.
 *
 * Prefixy jsou inkluzivní, tj. search_by_prefix("abc") vrátí mezi výsledky
 * i "abc", pokud je "abc" v trii.
 */
std::vector<std::string> trie::search_by_prefix(const std::string& prefix) const
{
    std::vector<std::string> res;

    if (m_root == nullptr) {
        return res;
    }

    const_iterator itr = begin();

    while (*itr < prefix && itr != end()) {
        itr++;
    }

    while ((*itr).find(prefix) == 0 && itr != end()) {
        res.push_back(*itr);
        itr++;
    }

    return res;
}

/**
 * Vrátí všechny řetězce z trie, jež jsou prefixem daného řetězce.
 *
 * Prefixy jsou inkluzivní, tj. get_prefixes("abc") vrátí mezi výsledky
 * i "abc", pokud je "abc" v trii.
 */
std::vector<std::string> trie::get_prefixes(const std::string& str) const
{
    std::vector<std::string> res;

    if (m_root == nullptr) {
        return res;
    }

    trie_node* ptr = m_root;

    if (ptr->is_terminal) {
        res.push_back("");
    }

    for (char c : str) {
        if (ptr->children[(int)c] == nullptr) {
            return res;
        }
        ptr = ptr->children[(int)c];

        if (ptr->is_terminal) {
            const_iterator itr(ptr);
            res.push_back(*itr);
        }
    }

    return res;
}

trie::const_iterator trie::begin() const
{
    if (empty()) {
        return const_iterator(nullptr);
    }

    trie_node* ptr = m_root;
    while (!ptr->is_terminal) {
        for (auto & i : ptr->children) {
            if (i) {
                ptr = i;
                break;
            }
        }
    }
    return const_iterator(ptr);
}

trie::const_iterator trie::end() const
{
    return const_iterator(nullptr);
}

/**
 * Prohodí všechny prvky mezi touto trií a `rhs`.
 *
 * Složitost: O(1)
 */
void trie::swap(trie& rhs)
{
    trie_node* tmp_root = this->m_root;
    size_t tmp_size = this->m_size;
    this->m_root = rhs.m_root;
    this->m_size = rhs.m_size;
    rhs.m_root = tmp_root;
    rhs.m_size = tmp_size;
}

// Relační operátory

/**
 * Vrací `true` pokud je trie `rhs` roven této.
 *
 * Dvě trie si jsou rovny, pokud obsahují stejné prvky.
 */
bool trie::operator==(const trie& rhs) const
{
    if (m_size != rhs.m_size) {
        return false;
    }

    for (auto itr_l = begin(), itr_r = rhs.begin(); itr_l != end() && itr_r != rhs.end(); itr_l++, itr_r++) {
        if (*itr_l != *itr_r) {
            return false;
        }
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
bool trie::operator<(const trie& rhs) const
{
    auto itr_l = begin(), itr_r = rhs.begin();
    for (; itr_l != end() && itr_r != rhs.end() ; itr_l++, itr_r++) {
        if (*itr_l < *itr_r) {
            return true;
        } else if (*itr_l > *itr_r) {
            return false;
        }
    }

    if (itr_l == end() && itr_r != rhs.end()) {
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
trie trie::operator&(trie const& rhs) const
{
    std::vector<std::string> strings_l, strings_r, strings;

    const_iterator itr_l = begin(), itr_r = rhs.begin();

    if (this->size() <= rhs.size()) {
        while (itr_l != end()) {
            std::vector<std::string> strings_tmp = rhs.search_by_prefix(*itr_l);
            strings_r.insert(strings_r.end(), strings_tmp.begin(), strings_tmp.end());
            strings_l.push_back(*itr_l);
            auto itr_tmp = itr_l++;
            while ((*itr_l).find(*itr_tmp) == 0) {
                strings_l.push_back(*itr_l);
                itr_l++;
            }
        }
    } else {
        while (itr_r != rhs.end()) {
            std::vector<std::string> strings_tmp = this->search_by_prefix(*itr_r);
            strings_l.insert(strings_l.end(), strings_tmp.begin(), strings_tmp.end());
            strings_r.push_back(*itr_r);
            auto itr_tmp = itr_r++;
            while ((*itr_r).find(*itr_tmp) == 0) {
                strings_r.push_back(*itr_r);
                itr_r++;
            }
        }
    }

    std::set_intersection(strings_l.begin(), strings_l.end(), strings_r.begin(), strings_r.end(), back_inserter(strings));

    return trie(strings);
}

/**
 * Vrátí novou trii, která obsahuje sjednocení této a `rhs`, tj. řežezce
 * přítomné alespoň v jedné z nich.
 */
trie trie::operator|(trie const& rhs) const
{
    std::vector<std::string> strings_l, strings_r, strings;

    for (auto && itr_l : *this) {
        strings_l.push_back(itr_l);
    }

    for (auto && itr_r : rhs) {
        strings_r.push_back(itr_r);
    }

    std::set_union(strings_l.begin(), strings_l.end(), strings_r.begin(), strings_r.end(), back_inserter(strings));

    return trie(strings);
}

//! 2 trie jsou si nerovné právě tehdy, když si nejsou rovné (viz operator==)
bool operator!=(const trie& lhs, const trie& rhs)
{
    return !(lhs == rhs);
}

//! Lexicografické uspořádání, viz operator<
bool operator<=(const trie& lhs, const trie& rhs)
{
    return lhs == rhs || lhs < rhs;
}

//! Lexicografické uspořádání, viz operator<
bool operator>(const trie& lhs, const trie& rhs)
{
    return !(lhs == rhs || lhs < rhs);
}

//! Lexicografické uspořádání, viz operator<
bool operator>=(const trie& lhs, const trie& rhs)
{
    return lhs == rhs || lhs > rhs;
}

/**
 * ADL customization point pro std::swap.
 * Výsledek `swap(lhs, rhs);` by měl být ekvivalentní s výsledkem
 * `lhs.swap(rhs);`
 */
void swap(trie& lhs, trie& rhs)
{
    lhs.swap(rhs);
}

/**
 * Operátor výpisu do proudu.
 *
 * Tuto funkci netestujeme, ale pokud ji vhodně implementujete, budete mít
 * ve výstupech z testů užitěčně vypsaný obsah trie.
 */
std::ostream& operator<<(std::ostream& out, trie const& trie)
{
    auto itr = trie.begin();
    out << *itr;
    itr++;
    while (itr != trie.end()) {
        out << " " << *itr;
        itr++;
    }
    return out;
}