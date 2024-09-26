# Kdy co použít?
## Prefix sum
Hledám pozici pro jednu věc. Mám mapu a v ní hledám ideální místo.

## Backtracking
Když chci umístit sadu věcí někam. Položím první na nejlepší místo. Pak najdu nejlepší pro druhou věc a tak dále a tak dále.

## Procházení stromu
Mám něco co má kořen a jasné potomky. Rozdíl od grafu v počtu potomků a cyklů.

## Procházení grafu
Může být 2D mapa, několik různých potomků (bez kořene).

## Dynamické programování
Dělám něco víckrát. Předávám dál už vypočítanou hodnotu.



# Šikovný vychytávky
## Z úlohy 1 - Prefix sum
### [Jak udělat prefix v matici/2D poli:](/01_du_new/algorithm.cpp)
```cpp
int index = row * in.n + col;

if (row > 0)
   prefix_sum[index] += prefix_sum[index - in.n];

if (col > 0)
   prefix_sum[index] += prefix_sum[index - 1];

if (row > 0 && col > 0)
prefix_sum[index] -= prefix_sum[index - in.n - 1];
```
### [Jak vypočítat z prefixu sumu](/01_du_new/algorithm.cpp)
```cpp
int sum = prefix_sum[down_row * n + down_col]; // Sum of the entire rectangle

if (up_row > 0) {
   sum -= prefix_sum[(up_row - 1) * n + down_col]; // Subtract the top part
}
if (up_col > 0) {
   sum -= prefix_sum[down_row * n + (up_col - 1)]; // Subtract the left part
}
if (up_row > 0 && up_col > 0) {
   sum += prefix_sum[(up_row - 1) * n + (up_col - 1)]; // Add back the top-left part
}
```
## Z úlohy 2 - Backtracking
Ukončovací podmínka na začátku v rekurzi.
Procházení všech možností, do kterých pošleš další rekurzi.

## Z úlohy 3 - Stromy
### Postavení stromu z preorder:
1. Vzít první prvek, nastavit ho jako root.
2. Procházet další prvky
   1. Pokud menší, tak doleva do pole
   2. Pokud větší, doprava
3. Poslat do levého/pravého potomka levý/pravý podstrom

> Pokud chci na tom něco dělat, záleží kdy:
> - preorder -> předtím, než pošlu rekurzi do dalších potomků
> - inorder -> mezi potomky
> - postorder -> po poslání do potomků

## Z úlohy 4 - Graf
### Okolní pole v 2D poli
```cpp
int neighbors[4][2] =  {{-1, 0},
                        {0, 1},
                        {1, 0},
                        {0, -1}};
```

### BFS
```cpp
std::queue<cell_t> q; // Define queue

q.push(start_position); 
visited[start_position] = true;

while (!q.empty())
{
   // Get the first element from the queue
   cell_t curr = q.front();
   q.pop();

   for (auto &neighbor : neighbors)
   {
      if(is_valid(new_position) && not_visited(new_position)){
         visited[new_position] = true;
         q.push(new_position);
      }
   }
}
```
### DFS
```cpp
stack<int> stack; // Create a stack for DFS
stack.push(s); // Push the current source node.
while (!stack.empty())
{
   // Pop a vertex from stack and print it
   int current = stack.top();
   stack.pop();

   if (!visited[s])
      visited[s] = true;

   // Get all adjacent vertices of the popped vertex s
   // If a adjacent has not been visited, then push it
   for (auto &neighbor : neighbors)
      if (!visited[neighbor])
            stack.push(*neighbor);
}
```

## Z úlohy 7 - Dynamické Programování
Vytvořit pomocnou tabulku (*KDO*), ze které se počítají mezivýpočty.

## Z Úlohy 8 - DP
Tabulka, kterou procházím sloupec po sloupci.

# Poznámky k úlohám
## [Úloha 1](/01_du_new/main.cpp)
> [Zadání](https://cw.felk.cvut.cz/brute/data/ae/release/2022z_b4b33alg/alg2022_exam/evaluation/input.php?task=park)

Jde o prefixní sumu. 
### Postup řešení
1. Načíst vstup - mapa - [*read_input*](/01_du_new/utils.cpp) 
2. Spočítat celkový prefix mapy - [*calc_prefix_sum*](/01_du_new/algorithm.cpp) 
   1. Nejdříve první řádek/sloupec - přičíst vlevo/nahoře - *if(row/col > 0)*
   2. Následuje stejný výpočet, jen se od toho odečte diagonala nahoře vlevo - *if (row > 0 && col > 0)*
3. Následuje hledání vhodných pozic - [*suitable_locs*]((/01_du_new/algorithm.cpp) )
   1. Vytvoření vnitřního pole - *size_of_park*
   2. Vypočítání sumy v prefixu - počítám mimo centrum - [*calc_sum_in_pref*](/01_du_new/algorithm.cpp)
      1. Odečtení horní/levé části - *if (up_row/up_col > 0)*
      2. Přičtení diagonály - *if (up_row > 0 && up_col > 0)*
      3. Vrácení sumy
   3. Když je suma/hodnota_podmínky(kámen=2) >= podmínka počtu
   4. Spočítám stejným způsobem sumu i pro stromy, které mají větší plochu
   5. Zapíšu maximum

## [Úloha 2](/02_du/)
> [Zadání](https://cw.felk.cvut.cz/brute/data/ae/release/2022z_b4b33alg/alg2022_exam/evaluation/input.php?task=boxes)

Backtracing v rekurzi

### Postup řešení
1. Načíst vstup - [*read_input*](/02_du/utils.cpp)
   1. Načíst základní čísla
   2. Načíst strom (spojení)
      1. Načítám do pole na pozici podle ID
      2. Vezmu rodiče na pozici jeho ID
         1. Je prázdný? => Založím
      3. Podívám se na levého potomka
         1. Je prázdný? 
            1. Je => Založím
            2. Není => Založím pravého
      4. Cena dostat se sem:
         1. Vezmu cenu k rodiči a přičtu svou
2. Procházení místnostmi - [*algorithm*](/02_du/algorithm.cpp)
   1. Založení pole místností a údaje o počtech
   2. Vstup do rekurze se založenými vstupy - *go_through*
      1. Ukončovací podmínka - Kontrola položení všech boxů
         1. Spočítání rovnováh - *balance_calc*
         2. Spočítání časů, pokud rovnováhy stejné - *time_calc*
         3. Return
      2. Kopie místností - *memcpy*
      3. Procházení všech možností
         1. Validace podmínek - *is_valid_pos*
         2. Přidání dalšího stavu
         3. Volání rekurze - *go_through*
         4. Odebrání stavu

## [Úloha 3](/03_du/)
> [Zadání](https://cw.felk.cvut.cz/brute/data/ae/release/2022z_b4b33alg/alg2022_exam/evaluation/input.php?task=railways)

Procházení stromu
1. inorder
2. preorder - vstup
3. postorder - zpracování

### Postup řešení
1. Načtení vstupu - [*read_input*](/03_du/utils.cpp)
2. Zadání první hodnoty jako root - [*main*](/03_du/main.cpp)
3. Stavba stromu + počítání - [*new_tree*](/03_du/algorithm.cpp)
   1. Kontrola počtu nodů co jsme dostali
   2. Založení root podle hodnoty v poli stromu
   3. Průchod hodnotami obdrženého pole
      1. Přidávání do pole doleva/doprava
      2. Držení počtu kolik jsem kam dal
   4. Rekurzivní tvorba potomků
      1. Levý/Pravý potomek z levého/pravého pole 
   5. Vypočtení cest na aktuální pozici - [*calculate_roads*](/03_du/algorithm.cpp)
      1. Když list
         1. Počet měst = 1
         2. Hloubka = 0
         3. return;
      2. Jen jeden potomek
         1. Kontrola potomků - levý/pravý == null
            1. Počet měst = mesta_vpravo + 1
            2. Hloubka = hloubka_vpravo + 1
            3. return;
      3. Moje hloubka = max(hl_vlevo, hl_vpravo) + 1
      4. Sečtení left/right_max
      5. Porovnání left/right_max

## [Úloha 4](/04_du/)
> [Zadání](https://cw.felk.cvut.cz/brute/data/ae/release/2022z_b4b33alg/alg2022_exam/evaluation/input.php?task=compgame)

Procházení grafu
1. **BFS**
2. DFS

### Princip řešení
Jedna mapa, na kterou dávám filtry podle aktuální barvy co používám

### Postup řešení
1. Načíst vstup - [*read_input*](/04_du/utils.cpp)
   1. Založení pole v poli barev
   2. Přidávání do mapy a do filtrů
2. BFS - [*shortest_path*](/04_du/algorithm.cpp)
   1. Definice queue
   2. Push start pozice do queue
   3. Nastavení aktuální pozice na visited
   4. While queue not empty
      1. Pop prvního elementu
      2. Kontrola konce
      3. Kontrola tlačítka
         1. Změna aktuálního klíče podle tlačítka
      4. Průchod všemi možnými dalšími pozicemi
         1. Kontrola jestli je validní - [*is_valid*](/04_du/algorithm.cpp)
            1. Push do queue
            2. Nastavení na visited

## [Úloha 7](/07_du/)
> [Zadání](https://cw.felk.cvut.cz/brute/data/ae/release/2022z_b4b33alg/alg2022_exam/evaluation/input.php?task=simulator)

Dynamické programování

### Princip řešení
Vytvořit tabulku mezivýpočtů, kterou pak používáme na dopočítání dalších pozic.

### Postup řešení
1. Přijít na [tabulku mezivýpočtů](https://docs.google.com/spreadsheets/d/1260A74s4JP9oC-Ub24mH4EiwRFp0GHsv1wNX2198AmY/edit#gid=1595305524) - KDO arr
   1. Jak se dostat ze základního stavu dál
   2. Něco generuje něco jiného a tím to můžu dopočítat.
   3. ab -> bb+ba; bb->bb+ba; ba->ab
2. Načíst vstup - [*read_input*](/07_du/utils.cpp)
3. Vytvořit tabulku KDO - [*kdo_arr*](/07_du/algorithm.cpp)
   1. Zjištění kombinací
   2. Procházení kombinací a zjištění která generuje jakou jinou
4. Průchod dopočítání - [*get_result*](/07_du/algorithm.cpp)
   1. První vytvořen ze vstupu "ručně"
   2. Zbytek podle aktuálního sloupce vytvoří další
      1. Opakování dokud nenarazíme na konečný sloupec

## [Úloha 8](/08_du/)
> [Zadání](https://cw.felk.cvut.cz/brute/data/ae/release/2021z_b4b33alg/alg_cz_2021z/evaluation/input.php?task=garden)

Dynamické programování
### Princip řešení
Vytvoření hned ze startu mapu cen průchozích políček. Spočítání maximální možné hodnoty při příchodu zleva/zhora/zprava pro každý řádek.
Na posledním uložit hodnotu z nejlepšího políčka.

### Postup řešení
1. Načtení mapy a přitom i vytvoření pomocné tabulky - [*read_input*](/08_du/utils.cpp)
2. Průchod odshora dolů tabulkou - [*go_through*](/08_du/algorithm08.cpp)
   1. Nejdříve projít zleva a zkontrolovat levou a horní hodnotu.
   2. Potom zprava to samé
      1. Rovnou porovnat hodnoty nejlepší hodnoty ze všech stran a zapsat ji.
   3. Pokud na posledním řádku
      1. Zapsat maximální hodnotu atraktivity a cesty

## Starý zkoušky
Moje poznámky k zadáním starých zkoušek.
- [Zima 2021](/old_zkousky/Z21/)
- [Zima 2022](/old_zkousky/Z22/) - nemám poznámky