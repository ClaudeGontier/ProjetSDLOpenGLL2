# ProjetSDLOpenGLL2

## Librairies
- KDialog
- SDL2
- SDL2 Image
- SDL2 TTF

## Installation
### Dependencies
Debian based distro: ``sudo apt install build-essential kdialog libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev``
Arch based distro: ``pacman -S --needed base-devel kdialog sdl2 sdl2_image sdl2_ttf sdl2_mixer``
### Clone repository
```sh
cd <EMPLACEMENT_DU_PROJET>
git clone https://github.com/Estorc/ProjetSDLOpenGLL2.git
```

## Compilation
```
make            # Construit le projet avec la configuration par défaut.
make debug      # Construit le projet avec la configuration de déboggage.
make tools      # Construit les outils de compilation.
```

## GIT
### Récupérer les changements :
```git
git pull                # Permet de récupérer les dernières mises à jour du projet.
```
### Envoyer les changements :
```git
git commit -a           # Prépare le commit (utilisez "git commit -am uncommented" pour passer le commentaire).
git push -u origin main # Permet d'envoyer les changements locaux au git.
```
