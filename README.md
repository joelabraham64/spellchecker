# Spell Checker

A dictionary-based spell checker implemented in C using a hash table for fast word lookup. The program scans an input text file, identifies misspelled words, and generates simple correction suggestions.

## Build & Run
```bash
gcc *.c -o spellcheck
./spellcheck <dictionaryFile> <inputFile> <add|ignore>
