# include <iostream>
# include <chrono>
# include <ctime>
# include <thread>

using namespace std;

void fibonacci (int n, int * aux)
{
 if (n<2)
 {
     *aux=n;
 }
 else
 {
     int a,b;
     fibonacci(n-1, &a);
     fibonacci(n-2, &b);
     *aux=a+b;  
 }  
}

int main (void)
{
 int n;
 
 cout << endl << "> SUCESION FIBONACCI: Recursivo y Concurrente"  << endl << endl;
 cout << "Digite el valor enesimo que desea conocer: ";
 cin >> n;
 
 std::chrono::time_point<std::chrono::system_clock>start, end;
 
 start= std::chrono::system_clock::now();
 
 int aux1,aux2,total; 

 thread b(fibonacci, n-1, &aux1);
 thread c(fibonacci, n-2, &aux2);

 b.join();
 c.join();

 total= aux1+aux2;
 
 end= std::chrono::system_clock::now();

 int elapsed_microseconds =      std::chrono::duration_cast<std::chrono::microseconds>(end-  start).count();

 cout << "El valor correspondiente en la susesion es: " << total << endl;
 cout << "Tiempo de procesamiento: " << elapsed_microseconds <<" Microsegundos " << endl << endl;

 return 0;
}


