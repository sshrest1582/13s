#Assighnment 5: Public Key Cryptography (README.md)

## Short desription:
    This program has three main executables keygen, encrypt and decrypt.

    The the keygen.c creates the rsa public, private keys and the signature
    via the users username. The private and public keys are sent to their 
    respective files, and if wanted the user can see all the values bit
    lengths of the s(signature), n(rsa public key), e(public exponent).
    The program is preset to make keys with over 256 bits long and 50 
    iteration for the Miller Rabin however they can both be changed along
    with the seed for the random function. The file for the private key
    file will only be viewable and changeable by the user  

    The encrypt.c uses the encrypted public key in the specified file. The
    key is then used to encrypt the infile and send the encrypted message to
    the specified outfile. This is only done if the signature has been verified
    properly otherwise it will send an error message. The program aswell allows
    the user to see the bit length and value of the signature, public key and 
    the public exponent. This program only properly run once it has an infile 
    with text within it.
   
    The decrypt.c uses the decrypted public key in the specified file. The key
    is then used to decrypt the infile and send the decrypted message to the 
    specified outfile. However, if no outfile is specified it will then print
    the decrypted message on the terminal. 

## Build
    To build manually, type "clang -Wall -Wextra -Werror -Wpedantic -o encrypt.o
    decrypt.o keygen.o encrypt.c decrypt.c keygen.c" on the command line. 
    Alternatively, type "make" on the command line with the Makefile provided 
    in the directory.

## Running
    ./keygen -v
    ./encrypt -v -i file_txt.txt -o file_encrypted.txt
    ./decrypt -v -i file_encrypted -o file_decrypted.txt

## Cleaning
    To clean manually, type "rm -f randstate.o numtheory.o rsa.o encrypt.o decrypt.o
    keygen.o keygen decrypt encrypt" on the command line. Alternatively type 
    "make clean" on the command line, using Makefile provided.

## Error Handling:  
    If the user doesn't input a proper input file the program will not be able 
    finish executing. I was currently unable to fix this issue.
    
    If the user doesn't input a .txt file that exists already for any of the 
    programs they will get an error message saying ""Error signature could not be 
    verified" along with a return value of an EXIT_FAILURE.                  
                   
                            
                               
                         
                          
                  
               

