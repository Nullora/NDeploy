# Novus Deploy, Windows Edition
I already have an Ndep project for linux and never thought to make one for windows cz i never needed it, now i kinda do cz im working on a big project so here it is!
It's much much better than the linux ndep as i actually got better at coding and didn't vibecode the shit out of this one lol.
## Functions
```bash
ndep sign {file_name} {library_name}
```
This function signs a specific file with a library name using NTFS Alternate Data Streams (big fancy word for attaching data to file without changing its contents). This makes it so even if the file changes names or location, it will always maintain the sign.
```bash
ndep import {library_name}
```
This function imports a library into current working directory and adds it to a local database. Ndep always knows how many copies of the library have been distributed and where exactly.
```bash
ndep update {library_name}
```
This function updates an imported library. It pulls the update from the libraries database.
```bash
ndep push {library_name}
```
This function pushes a new update to the main libraries database.
```bash
ndep rename {library_name} {new_name}
```
This function renames the library and updates the signing on every single tracked file.