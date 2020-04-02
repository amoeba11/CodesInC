/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

long long int gcd(long long int a, long long int h) 
{ 
    long long int temp; 
    while (1) 
    { 
        temp = a%h; 
        if (temp == 0) 
          return h; 
        a = h; 
        h = temp; 
    } 
}

long long int isprime(long long int x)
{
    for(long long int i=2; i<=sqrt(x); i++)
    {
    if(x%i==0) return 0;
    }
return 1;
}

long long int inverse(long long int e, long long int phi_n)
{
    for(long long int k=1; ;k++)
    {
    long long int rhs = phi_n*k + 1;
    if(rhs%e==0) return rhs/e;
    }
}

long long int pow1(long long a, long long b, long long m) 
{
    a %= m;
    long long int res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a % m;
        a = a * a % m;
        b >>= 1;
    }
    return res;
}

long long int encrypt(long long int m, long long int e, long long int n)
{
    return pow1(m,e,n);
}

long long int decrypt(long long int c, long long int d, long long int n)
{
  return pow1(c,d,n);
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

  // Calculating keys.
    long long int p = 11;
    long long int q = 13;
    long long int n = p * q;
    long long int phi_n = (p-1)*(q-1);

    long long int e = 2;
    while(e<phi_n)
    {
        if(gcd(e,phi_n)==1 && isprime(e)==1)
        break;
        e++;
    }

    long long int d = inverse(e,phi_n);


     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n1;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);

     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

     if (newsockfd < 0) 
          error("ERROR on accept");

     bzero(buffer,256);

     // Reading the length of string received.
     n1 = read(newsockfd,buffer,255);
     if (n1 < 0) error("ERROR reading from socket");
     printf("Here is the length of message: %s\n",buffer);
     long long int len = atoi(buffer);
//     printf("Here is the length of sting: %lld\n", len);

     printf("\nPlease wait for a few moments... \n\n");

    // Sending the public key and n to client.
    sprintf(buffer,"%lld",e);
    n1 = write(newsockfd,buffer,strlen(buffer));
    if (n1 < 0) error("ERROR writing to socket");       
    sprintf(buffer,"%lld",n);
    n1 = write(newsockfd,buffer,strlen(buffer));
    if (n1 < 0) error("ERROR writing to socket");

    long long int decrypted[len];
    char res[len];

    printf("Here is the encrypted message as received: " );
    //Reading the encrypted message from client.
    for(int i=0; i<len; i++)
    {
      bzero(buffer,256);
      n1 = read(newsockfd,buffer,255);
      if (n1 < 0) error("ERROR reading from socket");
      printf("%s",buffer);
      long long int rcvd = atoi(buffer);
      decrypted[i] = decrypt(rcvd,d,n);
      res[i] = decrypted[i];
    }
    printf("\n");

    printf("Here is the numeric message decrypted: ");
    for(int i=0; i<len; i++)
    {
      printf("%lld",decrypted[i]);
    }
    printf("\n");

    printf("Here is the original text message decrypted: ");
    for(int i=0; i<len; i++)
    {
      printf("%c",res[i]);
    }
    printf("\n");
    
  return 0; 
}