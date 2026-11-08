# MP3 Tag Reader and Editor

## Overview
This project is a small C-based command-line tool for reading and editing ID3v2 metadata in MP3 files. It focuses on the common metadata fields used in music files: title, artist, album, year, and genre.

The project demonstrates how to parse raw MP3 tag frames, update selected values, and rewrite the file in a controlled way without losing the audio payload.

## Problem statement
MP3 files often contain metadata describing the song and artist, but there is no easy built-in command-line tool in the project itself to inspect or edit those fields safely. This tool solves that by reading tag frames, exposing the fields to the user, and writing updated values back into the file.

## Architecture and implementation
The program reads the MP3 file in binary mode and looks for an ID3v2 header followed by frame blocks. Each frame is identified by a four-character ID such as:

- `TIT2` for title
- `TPE1` for artist
- `TALB` for album
- `TYER` or `TDRC` for year
- `TCON` for genre

The parser reconstructs the correct frame sizes from the big-endian byte layout used by ID3v2 and stores the values in a `TagData` structure. After modification, the program writes the updated metadata back to a temporary file and renames it to replace the original file.

## Features actually implemented
- reads ID3v2 metadata from MP3 files
- displays title, artist, album, year, and genre
- updates title, artist, album, year, and genre values from the console
- writes updated metadata back to the source MP3 file
- validates command-line usage for viewing and editing tags
- handles missing or invalid metadata gracefully

## Technology stack
- C
- standard file I/O
- binary parsing
- memory allocation and cleanup
- command-line argument processing
- ID3v2 frame logic

## Engineering challenges faced
- ID3v2 frame sizes are stored in big-endian format, so the code must reconstruct the correct integer values before reading the frame body
- metadata is stored as encoded strings, which requires careful handling of the leading text-encoding byte
- updating a tag must preserve the rest of the file and avoid corrupting the audio data
- temporary file replacement is required to safely rewrite the ID3 tag block without destroying the MP3 content

## Important implementation decisions
- The program reads metadata only from the ID3v2 section instead of assuming every MP3 file follows the same tag pattern.
- Each frame is processed individually, which keeps the code modular and avoids rewriting unrelated metadata fields.
- Existing tag values are freed and replaced with newly allocated strings before writing back to the file.
- The writing logic reconstructs frame headers and payloads to keep the resulting MP3 file structurally valid.

## Learning outcomes
This project reinforces:

- binary file parsing
- structured programming in C
- memory safety and clean resource management
- command-line application design
- understanding how metadata is encoded in real media files

## Installation and execution
### Compile
```bash
gcc main.c id3_reader.c id3_writer.c id3_utils.c error_handling.c -o mp3tagreader
```

### View metadata
```bash
./mp3tagreader -v song.mp3
```

### Update metadata
```bash
./mp3tagreader -e -t song.mp3 "New Title"
./mp3tagreader -e -a song.mp3 "New Artist"
./mp3tagreader -e -A song.mp3 "New Album"
./mp3tagreader -e -y song.mp3 "2024"
./mp3tagreader -e -g song.mp3 "Rock"
```

### Help
```bash
./mp3tagreader -h
```

## Screenshots
No screenshots are included in the repository snapshot. The project is documented through the source structure and the metadata operations it supports.

## Repository information
- Repository: `c_mp3_tag_reader`
- Stack: C, binary I/O, ID3v2 parsing, metadata editing
- Primary focus: MP3 metadata inspection and safe in-place tag editing
