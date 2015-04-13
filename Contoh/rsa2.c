/*
    rsa.c = Demo os RSA public key encryption
    This code uses small (i.e. bad) primes, reduces the ASCII
    upper-case letters to 1, 2, 3, ..., and encrypts each character
    independently.  This results in rotten security, but keeps the
    numbers small enough to calculate without using complex algorithms.
    This code also doesn't compute inverses efficiently (in the
    interest of clear code), and picks a small fixed encryption
    key, rather than a random one.

    It is worth remembering that the strength of this method is that
    it is very hard to compute d from e and n.  This in turn depends
    on n being hard to factor.  That's why you should use huge primes,
    not 3, 5, 7, 11, or other small primes.

    Written 10/2006 by Wayne Pollock, Tampa Florida USA.

    [Example was adopted from page 104 of "Cryptography and Data Security"
     by Dorothy Denning, (C)1982 Addison-Wesley.]
*/

#include <math.h>
#include <stdio.h>
#include <string.h>

unsigned int pick_e ( unsigned int phi );
unsigned int inv ( unsigned int num, unsigned int phi );
unsigned int encrypt ( unsigned int num, unsigned int key, unsigned int n );

int main ( void )
{
    char msg[] = "HELLO";
    unsigned int cyphertext[sizeof(msg)];

    unsigned int p = 3;   // Too small for real security!
    unsigned int q = 11;  // Too small for real security!

    unsigned int i, n, phi, e, d, result;

    n = p * q;  // This is the step that is hard to reverse!

    phi = (p-1) * (q-1);  // Euler's "totient" function.

    e = pick_e( phi );  // pick one of the pair of keys, ...
    d = inv( e, phi );  // ... and calculate the other.

    printf( "p=%u, q=%u, n=%u, phi(n)=%u, e=%u, d=%u\n", p, q, n, phi, e, d );

    // Encrypting the message:

    printf( "\n\tEncrypt msg \"%s\" using key (%u,%u):\n", msg, e, n );
    for ( i=0; i < strlen(msg); ++i )
    {   cyphertext[i] = encrypt( msg[i]-64, e, n );
        printf( "\t%c (%2u) --> %c (%2u)\n", msg[i], msg[i]-64,
           (char)(cyphertext[i]+64), cyphertext[i] );
    }

    // Decrypt that message:

    printf( "\n\tDecrypt cyphertext using key (%u,%u):\n", d, n );
    for ( i=0; i < strlen(msg); ++i )
    {   
        result = encrypt( cyphertext[i], d, n );
        printf( "\t%c (%2u)--> %c (%2u)\n", (char)(cyphertext[i]+64),
            cyphertext[i], (char)(result+64), result );
    }

    return 0;
}


/* Return some integer relatively prime to phi
   (should pick a random one, not the first one found! */

unsigned int pick_e ( unsigned int phi )
{
    return 7;  // The gcd code isn't worth implementing for this demo!
}


/* Compute the inverse of num modulus phi (Note there is a better way!) */
unsigned int inv ( unsigned int num, unsigned int phi )
{
    unsigned int i = 0;

    while ( (num * i ) % phi != 1 )
        ++i;
    return i;
}


/* Compute the encryption of some num using key (key, n) */

unsigned int encrypt ( unsigned int num, unsigned int key, unsigned int n )
{
    int result = (int) (fmodl( powl(num, key), n ) + 0.5);
    if ( result < 0 )   result += n;
    return (unsigned int) result;
}
