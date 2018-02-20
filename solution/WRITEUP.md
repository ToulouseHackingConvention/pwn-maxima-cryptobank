# Writeup

Le programme permet de créer des comptes et de faire des transactions entre
les comptes.

Il est compilé avec Clang Address Sanitizer et Undefined Behavior Sanitizer,
qui ajoute énormément de vérification à l'exécution pour attraper la plupart
des "Undefined Behavior".

Le but est donc de trouver l'undefined behavior qui n'est pas attrapé. Il y a
plein d'erreurs de programmation dans le code, mais normalement une seule
d'entre elle n'est pas attrapé par ASAN/UBSAN et permet l'exploitation.

Il s'agit d'un heap buffer overflow ligne 320, grâce à la variable "index"
qui est contrôlée par l'utilisateur. Il s'agit de la fonction de modification
d'un commentaire.

On peut donc faire:
```
fgets(account->transactions.ptr[index].comment, 30, stdin);
```
avec un index contrôlée.

Ma solution consiste à prendre `index` tel que
account->transactions.ptr[index].comment = accounts.ptr[2].transactions.ptr
De cette façon, on peut réécrire complètement le tableau des transactions.
On peut donc mettre une transaction avec un pointeur de commentaire contrôlé.

Une fois ceci fait, on peut lire avec l'option "show statement" (qui affiche
les commentaires) et écrire avec l'option de modification des commentaires.

On a donc un read-write everywhere. À partir de la, il est simple de terminer.
