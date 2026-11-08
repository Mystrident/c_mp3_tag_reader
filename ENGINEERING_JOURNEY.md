# MP3 Tag Reader — Engineering Journey

## Repository status
The visible history in this local clone is shallow and contains a single commit: `Added README`. Because of that, the strongest evidence again comes from the current implementation rather than a long chronological series of refinements.

## Challenge matrix

| Challenge | Evidence from code/history | What changed | Engineering reasoning | Learning outcome |
|---|---|---|---|---|
| ID3 frame parsing | `id3_reader.c` reconstructs frame size from 4 big-endian bytes and reads frame IDs | Raw MP3 metadata is parsed manually instead of through a library | Correct parsing of binary tag metadata is required before any value can be displayed | Learned binary metadata parsing |
| Correct metadata offset handling | The code reads 10-byte header then loops across frame blocks | File offsets are manually advanced using `fseek()` and structured reading | Tags are stored in a nontrivial binary layout | Learned how MP3 metadata is structured |
| Handling text encoding in tag values | `data->title = strdup(frame_data + 1)` skips the first byte of text-encoding metadata | The parser strips the encoding byte before storing actual text | Text frames include a leading control byte that must be skipped | Learned to avoid incorrect string interpretation |
| Safe update workflow | `write_id3_tags()` creates a temp file and renames it over the original | The source file is not blindly rewritten in place | Prevents file corruption and preserves the remaining audio section | Learned a safer update strategy |
| Avoiding invalid or malformed tags | The code checks `frame_size > 100000`, `frame_size <= 0`, and missing frame IDs before continuing | Reader is defensive around malformed file content | Real media files can be incomplete or malformed | Learned defensive parsing in lower-level systems |

## Interview questions and evidence-backed answers
1. Why is the ID3 parser reading bytes manually instead of using a helper library?
   Answer: The project operates at the binary file level and explicitly reconstructs frame sizes from raw bytes in `id3_reader.c`.

2. What makes MP3 tag parsing tricky?
   Answer: ID3 frames store their size fields in big-endian format, while the machine reads memory in native endianness, so the implementation must convert values correctly.

3. Why does the code use `size_bytes[0] << 24` pattern?
   Answer: It rebuilds the integer value from four bytes in big-endian representation before reading the frame payload.

4. How does the parser avoid reading invalid frames?
   Answer: It checks for zero-filled frame IDs and rejects extreme or invalid frame sizes before continuing.

5. Why is the first byte of the frame data skipped when storing the text?
   Answer: ID3 text frames include a text-encoding byte at the start; the implementation strips it so the final string is human-readable.

6. What was the main risk when updating metadata?
   Answer: Corrupting the MP3 file or destroying the audio portion while changing the tags.

7. Why is the project writing to a temporary file instead of editing the original file in place?
   Answer: `write_id3_tags()` creates `temp.mp3`, writes the updated metadata into it, and renames it to the original filename, which is a safer rewrite pattern.

8. Why does the code free strings before reassigning them?
   Answer: The project avoids dangling pointers and stale data when a title, artist, or album field is replaced.

9. What is the role of `display_metadata()`?
   Answer: It prints a readable summary of the parsed tag values: title, artist, album, year, and genre.

10. How does the implementation decide whether a frame is a title or an artist field?
   Answer: It compares the 4-character frame IDs: `TIT2` for title, `TPE1` for artist, `TALB` for album, `TCON` for genre, and `TYER`/`TDRC` for year.

11. Why is `check_id3_tag_presence()` important?
   Answer: It prevents the parser from failing silently on files that do not contain ID3 metadata.

12. What did the code teach you about real binary parsing?
   Answer: It reinforced the importance of offsets, byte ordering, and careful validation when reading file formats directly.

13. Why are the tag-edit functions defensive in how they validate user input?
   Answer: The tool checks whether the tag option is valid and stops with an error if it is not, instead of writing malformed metadata.

14. What is the strongest learning from this project?
   Answer: Managing memory, parsing binary blocks, and rewriting a file safely are all critical when working with real media formats.

15. What would you change if rebuilding it?
   Answer: I would likely add stronger validation around unsupported tag versions and multi-language text encodings, but that enhancement is not directly evidenced in the current repo.

## Strongest evidence-backed takeaways
- This project is a focused demonstration of low-level metadata parsing and safe file rewrite logic.
- The main engineering challenge is understanding the raw binary layout of ID3 metadata and reconstructing it correctly on the fly.
- The code clearly prioritizes correctness and file safety over convenience or abstraction.
