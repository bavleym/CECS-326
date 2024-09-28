FileCopy and child are two codes used for the Project "Warm up of Interprocess Communication" 

The goal of the project is to design and implement a file-copying program named filecopy.c that utilizes ordinary pipes for inter-process communication. The program will be passed two parameters: the name of the file to be copied and the name of the destination file. It will create a pipe (please find more info of pipe for interprocess communication by yourself), write the contents of the source file to the pipe, and then have
a child process read from the pipe and write to the destination file.Warm up of Interprocess Communication

To compile code, place FileCopy.c and child.c in the same directory.

Run the command prompt with admin privileges and type in "gcc -o (filename1).exe FileCopy.c" and "gcc -o (filename2).exe child.c".

Once the exe has been created in the directory, create a source or input file in the same directory. 

Enter in the command prompt "(filename1).exe source.txt destination.txt". 

If you have issues with that command try to be more specific with the pathname of the of the source and destination files like "(filename1).exe "C:\Users\YourUsername\Desktop\source.txt" "C:\Users\YourUsername\Desktop\destination.txt""

