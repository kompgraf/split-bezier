# Split Bézier

Bézier-görbe két görbére osztása adott *t* paraméter értéknél.

Érdemes megnézni a commit historyt (`git log` parancs), és látni, ahogy fokozatosan formálódott a kódbázis.

## Az algoritmus

A két részre bontó algoritmus a de Casteljau-algoritmusra épít, annak kiegészítése. Pusztán annyit kell tennünk, hogy a de Casteljau-algoritmus során képzett köztes generációk első és utolsó pontját mindig megtartjuk. Előbbi a baloldali részgörbe egy kontrollpontja lesz, utóbbi a jobboldali részgörbe egy kontrollpontja lesz. Természetesen az utolsó generációt alkotó egyetlen pont, azaz a *t* paraméterértékhez tartozó görbepont mindkét részgörbének kontrollpontja lesz.

Ezután rendelkezésünkre áll két olyan, az eredetivel azonos fokszámú görbe, melyek összeillesztve kiadják az eredeti görbét.

## Rekurzió

A kódban is el van helyezve egy figyelmeztetés, de itt is fontosnak tartom kiemelni, hogy imperatív és objektumorientált nyelvek teljesítménykritikus kódjaiban ellenjavallott a rekurzió használata (ezért nem elfogadott bevgrafon a rekurzív de Casteljau-implementáció). Azonban ezúttal a könnyebb olvashatóság és érthetőség miatt mind a de Casteljau-, mind a két részre osztó algoritmus rekurzív módon adott.
