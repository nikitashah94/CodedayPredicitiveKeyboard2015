# OpenScribe - Mk 0.0.1
Autocomplete for the english language!

--Update Notes--
* Now learns as you type!
* Changed tree traversal to be somewhat breadth-first but processor cache friendly
* Reduced memory usage by literally (not figurativley) 3000% -> from 300mb to 9.3mb for approx. 25,000 indexed words -> 250,000+ words now takes up only 153mb
* Removed memory leaks!
* limited search to only start after 2 letters have been entered by user for speed reasons -> hope to remove this for commonly used words e.g (the, because, etc...)

--TODO(Short-Term)--
*Handle user defined dictionaries.
*Predict based on previous word.
*Improve search and sort speed.
*Reduct resource impact.
*Create terminal based interface -> similar to vim/emacs.
*Reduce reliance on external libraries.
*Re-impliment across different platforms.

--TODO(Long-Term)--
*interface with OS to predict in any text-editable field.
*encrypt user-defined dictionary to prevent personal information loss.
