# Get Next Line (GNL)

**Score:** 100/100

## About

The goal is simple on paper but tricky in C: write a function `get_next_line(int fd)` that returns a **single line** from a file descriptor every time it's called, without losing the reading position.

This project introduces a critical concept: **static variables**. Unlike standard local variables, static variables retain their value between function calls — essential for "remembering" the text left over after a newline is found.

## Development Journey

### The "Leak" Nightmare

My biggest struggle during GNL was memory leaks. Specifically, the management of the `stash` (the buffer that holds characters read but not yet returned).

**The issue:** Since `get_next_line` can be called in a loop, if the `stash` isn't properly freed or reallocated when a line is extracted, memory leaks accumulate rapidly.

**The "hidden" leaks:** I had many hidden leaks in edge cases — when the file ends without a newline, or when an error occurs mid-reading.

### The Integrity Decision

During development, I watched a tutorial video (Nikito's) to understand the concept. While it helped clarify linked lists, I felt that following the code structure too closely went against the 42 spirit of "learning by doing."

**My decision:** I scrapped my initial "influenced" code and rewrote the logic entirely from scratch. I wanted to prove to myself that I could handle buffer management and static variables without relying on external logic. It was harder, but the result is code I truly understand.

## Helper Functions

Since we can't use our `libft` for this project, I had to select and modify specific utility functions:

- `ft_strdup`: Duplicates strings without manual malloc sizing
- `ft_strjoin`: Essential for combining the `stash` with the newly read buffer
- `ft_strlen`: To calculate offsets
- `ft_strchr`: To detect the newline `\n` character
- `ft_strlcpy`: Safe string copying to prevent buffer overflows

## What I Learned

- **Static variables:** Understanding their lifecycle and scope
- **Buffer management:** Handling partial reads (reading 10 bytes when the line is only 4 bytes long)
- **File descriptors:** How the OS keeps track of open files
- **Integrity:** Sometimes it's better to start over than to build on shaky foundations

---

*Project developed at 42 Lausanne*
