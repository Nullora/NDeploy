# Novus Deploy - Windows Edition
This is a Windows Version of [NovusDeploy](https://github.com/Nullora/NovusDeploy),
a tiny local project manager for keeping reusable code up to date across multiple projects.

- It uses NTFS Alternate Data Streams to staple metadata onto files, letting ndep recognize them even if they change names or locations.

## Requirements
### Executable
- You just need Windows, since NTFS ADS is specific to it. Download `code/build/ndep.exe` and put it in PATH. [tutorial](https://windowsloop.com/how-to-add-to-windows-path/).
### Actual Uncompiled Code
- You need `Make`, `gcc`, zlib, and a Windows environment with BCrypt support.

## Functions
```bash
ndep sign {file_name} {library_name}
```
Signs a file using NTFS Alternate Data Streams (a fancy way of attaching data to a file without changing its actual contents). It stores the file's name, version, copy information, and SHA-256 hash in the metadata, then compresses a copy into the local `.ndep` repository.

```bash
ndep unsign {file_name}
```
Unsigns a file by deleting the ndep metadata streams from it. The repository object stays where it is, only the index of the object gets deleted.

```bash
ndep import {library_name} {filename}
```
Looks up a library `{library_name}` in the local index, decompresses its repository copy, and writes it into the current working directory as `{filename}`.
(Later ill track every single imported file so it can know where its copies live, and if we `ndep unsign` a library, all its copies are unsigned, and only their contents will remain)
(**WARNING:** Make sure `{filename}` isnt already used, as ndep will simply overwrite the current existing file.)

```bash
ndep push {library_name} {source_of_push}
```
Pushes an update to local repository objects, and update current version index. Past objects aren't overwritten or deleted from the repo, they are just deleted from the index (later i will keep track of them to add a revert function)

```bash
ndep update {library_name}
```
Updates current imported library from local repository objects. (later ill track all updates to a specific imported library for statistics)
(**WARNING:** No smart-merging yet, if theres any file-specific changes made, `ndep update` will completely overwrite them. use with caution and keep your file-specific changes in a separate file for now)

```bash
ndep all
```
Shows every signed file in CWD along with every signed file in local hash index. (Mainly a debugger, you could use it to check library spelling if u forgot and remember every imported library in cwd)

```bash
ndep rcalc
```
Manually recalculate content hash for every signed file in working directory. (Mainly a debugger, useful for checking if a commands isnt updating metadata correctly)

```bash
ndep find {libname}
```
Find where every registered copy of {libname} currently is.


```bash
ndep rename {old_name} {new_name}
```
Rename a library and all of its registered distributed copies.

## Features
The design takes a lot of inspiration from Git's architecture: content addressed objects, hashes as identity, a tiny local index, and keeping the actual stored data separate from the files people work on. Borrowing the parts that make Git feel so reliable is pretty fun :3

The main feature is NTFS Alternate Data Streams. Ndep can attach its identity and tracking information directly to a file without touching the file's normal bytes, so the metadata follows the file around while the content stays clean.

Ndep also uses NTFS file IDs assigned by Windows to identify signed files. Instead of relying only on a filename or path, it records the Windows file ID and volume serial number for the file. Windows can then resolve that file back to a path with its file ID, meaning a signed file can still be found after being renamed or moved anywhere on the C: drive.

The file ID tracking is kept persistently in the local `.ndep` data index, so ndep can keep tracking the same signed file across different runs of the program. This gives signed files a constant identity no matter what their name or location becomes.

Files are hashed with SHA-256 and stored in a Git-ish `objects` layout, with compression handled by zlib. The repository also keeps a simple library name to hash index, which makes importing a local copy quick and predictable.

Other features include:

- NTFS Alternate Data Streams for storing library names, version information, copy information, and content hashes without modifying the files normal contents
- Windows NTFS File ID tracking for locating a signed file by its Windows-assigned identity instead of depending on its current filename
- Persistent tracking across program runs through the `.ndep` file-ID and library-hash indexes
- SHA-256 content hashing for identifying library versions and detecting changed content (Copied from git)
- content addressed repository objects, where each stored version is placed using its hash
- zlib compression for repository objects, keeping stored library copies smaller (Copied from git)
- A completely local workflow with no server, account, or external service required

## For Who
If you have single header libraries that you share across multiple projects (like me) you probably wanna keep them up to date and not manually fix bugs everywhere once u spread them. ofc you could just make a quick copy paste script, or you could make it much fancier and cleaner using ndep :3

## Note
Im really proud of this project cz i actually looked at my old code, saw flaws, and made it much much better !!
I also ended up learning a shitton of new stuff and new win32 functions, so thats super useful in the future :3

## AI usage
I use AI in this project, heres where exactly:
- code/include/hash.hpp
- code/include/compression.hpp
- I did it cz honestly i dont wanna dive in sha256 and cryptography, and cz i dont wanna read zlib and their old hag docs, so ai did the bs for me and i focused on the hard parts (i think)
