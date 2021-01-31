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

## Porovnání

|          | Single                                        | Multi                                         |
| -------- | :-------------------------------------------: | :-------------------------------------------: |
| vstup_00 | real: 0m0.019s, user: 0m0.008s, sys: 0m0.008s | real: 0m0.054s, user: 0m0.008s, sys: 0m0.010s |
| vstup_01 | real: 0m0.023s, user: 0m0.015s, sys: 0m0.002s | real: 0m0.020s, user: 0m0.011s, sys: 0m0.007s |
| vstup_02 | real: 0m0.021s, user: 0m0.010s, sys: 0m0.006s | real: 0m0.020s, user: 0m0.007s, sys: 0m0.012s |
| vstup_03 | real: 0m0.022s, user: 0m0.010s, sys: 0m0.006s | real: 0m0.018s, user: 0m0.011s, sys: 0m0.005s |
| vstup_04 | real: 0m0.021s, user: 0m0.008s, sys: 0m0.007s | real: 0m0.022s, user: 0m0.014s, sys: 0m0.009s |
| test     | real: 0m0.019s, user: 0m0.016s, sys: 0m0.001s | real: 0m0.027s, user: 0m0.014s, sys: 0m0.010s |

