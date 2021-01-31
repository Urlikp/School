# Výpočet inverzní matice

Aplikace na vstupu dostane čtvercovou matici a na výstup vypíše matici inverzní, případně zprávu o tom, že inverzní matice neexistuje (zadaná matice je singulární). K implementaci bude použita [Gaussova eliminační metoda](https://cs.wikipedia.org/wiki/Gaussova_elimina%C4%8Dn%C3%AD_metoda), která spočívá v tom, že upravujete zadanou matici A do jednotkového tvaru, vedle matice A máte jednotkovou matici a na tuto matici provádíte stejné úpravy jako na matici A. Na konci výpočetního procesu máte matici A upravenou na jednotkovou matici a její inverzní matici.

## Použití
Použití: ./inverse_matrix [PŘEPÍNAČE]

Program pro výpočet inverzní matice ze vstupní matice uložené v textovém souboru (výchozí název: test.txt).

Přepínače:  
-h/--help  
Vytisknout použití a vypnout program  
-f/--file "filename"  
Nastavit "filename" jako název souboru se vstupní maticí  
-t/--thread  
Spustit program s více vlákny

## Vstupní matice

Matice by měla být v následujícím formátu: na prvním řádku se nachází dimenze čtvercové matice (počet řádků/sloupců), na následujících řádcích by měly být jednotlivé prvky matice, kde n-tý řádek matice by se měl nacházet na n+1-ním řádku souboru, jednotlivé prvky matice může oddělovat mezera, nebo tabulátor. Součástí projektu jsou i vzorové vstupy a jejich výstupy. Vstupní soubor by měl být ve stejné složce, jako binárka, případně, pokud se program spouští přes CLion, se dá nastavit working directory (Run -> Edit Configurations...) na složku, kde se nachází vstupní soubory.

## Poznámky

Program s více vlákny je pomalejší než jednovláknová varianta, pravděpodobně protože se dá paralelizovat pouze sčítání jednotlivých řádků a navíc dochází k uzamknutí mutexu, z důvodu sdílených proměnných. Pro příliš velké matice a příliš velká čísla může dojít k situaci, kdy se výsledné číslo nevejde do datového typu long double a program tedy vypíše místo čísla nan (not a number).
