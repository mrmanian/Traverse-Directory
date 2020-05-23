# Traverse-Directory

I created a program that sorts all the files in a directory based on file sizes, including all the files under all the sub-directories, sub-sub-directories, etc. Pathname is given through user argument.  

To simplify the program I:  
--Only considered regular files with only one hard link,  
--Assuming longest path name does not exceed 256 characters  

I created two versions of this project:  
--One recursively  
--One non-recursively  
