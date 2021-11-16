# '-' je jen zápočet, A, B, C, D, E, F jsou normálně ....
# SEM NAPIŠTE ZNÁMKY
ZnamkyKredity = [ 
    [               # první semestr
        ('A', 6),
        ('A', 5),
        ('-', 3),
        ('D', 6),
        ('A', 8)
    ],
    [               # druhý semestr
        ('D', 5),
        ('B', 6),
        ('B', 5),
        ('C', 7),
        ('D', 5)
    ],
    [               # třetí semestr
        ('A', 4),
        ('A', 6),
        ('B', 4),
        ('A', 6),
        ('E', 7),
        ('A', 7),
        ('-', 2)
    ],
    [               # čtvrtý semestr
        ('-', 1),
        ('C', 6),
        ('C', 6),
        ('D', 6),
        ('C', 6),
        ('C', 7),
        ('-', 3)
    ],
    [               # patý semestr
        #('C', 6),
        #('C', 6),
        #('C', 4),
        #('C', 6),
        #('C', 6)
    ]
]

Preklad = {   # Hodnota / číslo
    '-': 3,
    'A': 1,
    'B': 1.5,
    'C': 2,
    'D': 2.5,
    'E': 3,
    'F': 0
}

pocet_semestru = len(ZnamkyKredity)
vazene_kredity = [0] * pocet_semestru
suma_kredity = 0
vazene_znamky = 0
index = 0
for semestr in ZnamkyKredity:
    for predmet in semestr:
        vazene_kredity[index] += predmet[1] * (4 - Preklad[predmet[0]])
        if predmet[0] != "-":
            suma_kredity += predmet[1]
            vazene_znamky += Preklad[predmet[0]] * predmet[1]
    index += 1

print("Vazene kredity pro 2. semestr: ")
print(vazene_kredity[0])
for semestr in range(1, pocet_semestru):
    print("Vazene kredity pro " + (semestr + 2).__str__() + ". semestr: ")
    print(vazene_kredity[semestr] + vazene_kredity[semestr - 1])

print("Vazeny prumer: ")
print(vazene_znamky / suma_kredity)
