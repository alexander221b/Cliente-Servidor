#include <zmq.h>
#include <stdio.h>
#include <string.h>

int main (void)
{
    printf ("\nConnecting to hello world server...\n");

    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, "tcp://localhost:5555");
   
    char op[50], buffer[50];
    int size;
    
    while(1)
    {
    printf ("-------------------------------------- \n");  
    printf ("OPERACIÓN BÁSICA (+, -, *, /) <> CLIENTE  \n \n"); 
    printf("Operación: ");
    scanf("%s", op);  

    zmq_send (requester, op, 50, 0);
    zmq_recv (requester, buffer, 50, 0);

    printf ("Respuesta: %s\n", buffer);
    printf ("-------------------------------------- \n"); 
    }    

    zmq_close (requester);
    zmq_ctx_destroy (context);

    return 0;
}
