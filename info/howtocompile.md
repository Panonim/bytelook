# How to compile bytelook from source?
1. Download the official file from github repo.
  ```
  curl -S https://raw.githubusercontent.com/Panonim/bytelook/main/bytelook.c
  ```
2. Compile it using e.g. GCC.
  ```
  gcc bytelook.c -o bytelook
  ```
3. Move compiled file to the /usr/local/bin.
  ```
  mv ./bytelook /usr/local/bin
  ```
And from now on you are ready to use bytelook!
If you want you can remove the bytelook.c file - not the one inside /usr/local/bin - because bytelook have built-in update command.
