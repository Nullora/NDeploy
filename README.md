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
Pushes an update to local repository objects, and update current version index. Past objects aren't overwritten, they are just deleted from the index (later i will keep track of them to add a revert function)

```bash
ndep update {library_name}
```
Updates current imported library from local repository objects. (later ill track all updates to a specific imported library for statistics)
(**WARNING:** No smart-merging yet, if theres any file-specific changes made, `ndep update` will completely overwrite them. use with caution and keep your file-specific changes in a separate file for now)

## Features
The design takes a lot of inspiration from Git's architecture: content addressed objects, hashes as identity, a tiny local index, and keeping the actual stored data separate from the files people work on. Borrowing the parts that make Git feel so reliable is pretty fun :3

The main feature is NTFS Alternate Data Streams. Ndep can attach its identity and tracking information directly to a file without touching the file's normal bytes, so the metadata follows the file around while the content stays clean.

Files are hashed with SHA-256 and stored in a Git-ish `objects` layout, with compression handled by zlib. The repository also keeps a simple library name to hash index, which makes importing a local copy quick and predictable.

It is also deliberately small: no server, no account, no massive config setup, just a local repository for the reusable files that would otherwise get copy-pasted everywhere. The current implementation is focused on Windows and single-header-style libraries, where this workflow makes the most sense.

## For Who
If you have single header libraries that you share across multiple projects (like me) you probably wanna keep them up to date and not manually fix bugs everywhere once u spread them. Ofc you could just make a quick copy-paste script, or you could make it much fancier and cleaner using Ndep!

## Note
Im really proud of this project cz i actually looked at my old code, saw flaws, and made it much much better!
I also think using the hashed content and Git-inspired object storage is really clever, i copied the idea from Git itself so it made me feel really good.
I hope this project benefits you just as it's benefiting me rn!

## AI usage
I use AI in this project, heres where exactly:
- code/include/hash.hpp
Thats it lol. I did it cz honestly, im not gonna dive into cryptography. This was supposed to be a quick tool to help me manage annoying hanging reused header files, not a deep dive into SHA256 and how to make one from scratch.
