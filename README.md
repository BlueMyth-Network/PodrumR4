# Podrum

## Introduction
Podrum is a server software for Minecraft Bedrock Edition written in C and aims to be fast, stable and secure.

This project is still in early development.

## Building

Arch base Linux:
```
sudo pacman -S openssl
cmake CMakeLists.txt
make
```
Debian base Linux:
```
sudo apt install openssl
cmake CMakeLists.txt
make
```
MacOS:
```
brew install openssl
cmake CMakeLists.txt
make
```

Windows:
```
cmake CMakeLists.txt
msbuild PodrumR4.sln
```
