# MP3 Tag Reader and Editor (C)

A command-line MP3 metadata reader and editor written in C

## Features
- Reads ID3v2 metadata
- Displays title, artist, album, year, and genre
- Edits metadata tags
- Rewrites MP3 file safely using a temporary file

## Usage

Compile:
gcc main.c id3_reader.c id3_writer.c id3_utils.c error_handling.c -o mp3tagreader

View tags:
./mp3tagreader -v song.mp3

Edit tags:
./mp3tagreader -e -t song.mp3 "New Title"
./mp3tagreader -e -a song.mp3 "New Artist"

## Concepts Used
- File I/O
- Binary parsing
- Memory management
- Struct-based data handling
- Command-line arguments
