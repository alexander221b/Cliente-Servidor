# include <iostream>
# include <chrono>
# include <ctime>
# include <thread>

using namespace std;

int fibonacci(int);

int main ()
{
int n, p;

 cout << endl << "> SUCESION FIBONACCI: Recursivo y Secuencial"  << endl << endl;
 cout << "Digite el valor enesimo que desea conocer: ";
 cin >> n;
 
std::chrono::time_point<std::chrono::system_clock>start, end;

start= std::chrono::system_clock::now();

p= fibonacci (n);

end= std::chrono::system_clock::now();

int elapsed_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();

 cout << "El valor correspondiente en la susesion es: " << p << endl;
 cout << "Tiempo de procesamiento: " << elapsed_microseconds <<" Microsegundos " << endl << endl;

return 0;
}

int fibonacci (int n)
{
  

 if (n<2)
     return n;
  else
     return fibonacci(n-1) + fibonacci(n-2);
}
