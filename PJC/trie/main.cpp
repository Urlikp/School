#include <iostream>
#include "trie.hpp"

int main(int argc, char const *argv[])
{
    trie trie_2({ "", "a", "ab", "abc", "abcd" });
    std::cout << trie_2.erase("ab") << std::endl;
    std::cout << (trie_2.size() == 4) << std::endl;
    std::cout << trie_2.contains("ab") << std::endl;

    std::cout << trie_2.erase("abc") << std::endl;
    std::cout << (trie_2.size() == 3) << std::endl;
    std::cout << trie_2.contains("abc") << std::endl;

    std::cout << trie_2.contains("abcd") << std::endl;

    trie trie_1;
    std::cout << trie_1.insert("a") << std::endl;
    std::cout << trie_1.erase("a") << std::endl;
    std::cout << trie_1.contains("a") << std::endl;
    std::cout << (trie_1.size() == 0) << std::endl;
    std::cout << trie_1.empty() << std::endl;

    std::cout << trie_1.insert("bcd") << std::endl;
    std::cout << trie_1.erase("bcd") << std::endl;
    std::cout << trie_1.contains("bcd") << std::endl;
    std::cout << (trie_1.size() == 0) << std::endl;
    std::cout << trie_1.empty() << std::endl;

    return 0;
}