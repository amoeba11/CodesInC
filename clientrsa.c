#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

long int long pow1(long long a, long long b, long long m) 
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

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n1;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    printf("Please enter the message to be encrypted and sent: ");  

    char str[50];
    scanf("%s",str);
    long long int len = strlen(str);
    long long int m[len];
    long long int encrypted[len];

    // I am going to send length of original word and the encrypted word.
    
    // Sending length of original word to server.
    char buffer[256];
    sprintf(buffer,"%lld",len);
    n1 = write(sockfd, buffer, strlen(buffer));
    if (n1 < 0) 
        error("ERROR writing to socket");

    // Reading the public key received from server.
    bzero(buffer,256);
    n1 = read(sockfd,buffer,255);
    if (n1 < 0) 
        error("ERROR reading from socket");
    printf("The public key as received is: %s\n",buffer);
    long long int e = atoi(buffer);

    bzero(buffer,256);
    n1 = read(sockfd,buffer,255);
    if (n1 < 0) 
        error("ERROR reading from socket");
    printf("The value of n as received is: %s\n",buffer);
    long long int n = atoi(buffer);

    // Sending encypted message to server character by character.
    printf("The encrypted message is: "); 
    for(int i=0; i<len; i++) 
    {
        m[i] = str[i];
        encrypted[i] = encrypt(m[i],e,n);
        printf("%lld",encrypted[i]);
        char temp[255];
        sprintf(temp, "%lld", encrypted[i]);
        n1 = write(sockfd, temp, strlen(temp));
        if (n1 < 0) 
            error("ERROR writing to socket");
        sleep(0.000010);
    }

    printf("\n");

    return 0;
}