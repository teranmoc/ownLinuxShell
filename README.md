# ownLinuxShell
[CERI - L3] Interpréteur de commande


## Description
Ce projet de Licence Informatique est un interpréteur de commande Linux. 

## Compilation
Compiler avec **GCC** sur Linux :
> $ gcc shell.c -o Shell

## Utilisation
Lancer l'interpréteur de commande :
> $ ./Shell

Si le programme ne se lance pas, il faut donner les droits d'exécution :
> $ chmod +x Shell

Les commandes internes ne sont pas prises en charge, il s'agit des commandes comme *cd*, ...
De plus, il est impossible d'entrer plusieurs fois sur la touche Entrée (erreur sinon). 

