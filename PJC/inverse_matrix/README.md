# Výpočet inverzní matice

Aplikace na vstupu dostane čtvercovou matici a na výstup vypíše matici inverzní, případně zprávu o tom, že inverzní matice neexistuje (zadaná matice je singulární). K implementaci bude použita [Gaussova eliminační metoda](https://cs.wikipedia.org/wiki/Gaussova_elimina%C4%8Dn%C3%AD_metoda), která spočívá v tom, že upravujete zadanou matici A do jednotkového tvaru, vedle matice A máte jednotkovou matici a na tuto matici provádíte stejné úpravy jako na matici A. Na konci výpočetního procesu máte matici A upravenou na jednotkovou matici a její inverzní matici.

## Použití
Použití: ./inverse_matrix [OPTIONS]

Program pro výpočet inverzní matice ze vstupní matice uložené v textovém souboru (výchozí název: test.txt).

Options:  
-h				Print help and exit program  
-f <filename>	Specify that input matrix is in file <filename>  

