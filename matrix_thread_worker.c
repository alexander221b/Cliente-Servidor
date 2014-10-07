#include <iostream>
#include <czmq.h>
#include <string.h>
#include <vector>
#include <thread>

using namespace std;
typedef vector<vector <int>> Matrix;

Matrix char_to_int(char* string, int* rows, int* columns) {
  int size = strlen(string), counter_columns;
  Matrix matrix_int;
  vector<int> row;
  for(int i=0; i<size; i++) {
    if(string[i] != ',' && string[i] != ';') {
      row.push_back(atoi(&string[i]));
    }
    if(string[i] == ';') {
      matrix_int.push_back(row);
      counter_columns = row.size();
      row.clear();
    } 
  }
  *rows = matrix_int.size();
  *columns = counter_columns;
  return matrix_int;
}

Matrix result;

void multiply (Matrix matrix1, Matrix matrix2, int fila, int columns_matrix1, int columns_matrix2) {
  for(int i=fila; i<=fila; i++) {
    for(int j=0; j<columns_matrix1; j++) {
      for(int k=0; k<columns_matrix2; k++) {
        result[i][j] = result[i][j] + matrix1[i][k] * matrix2[k][j];
      }
    }
  }
}

char* int_to_char(Matrix result, int rows, int columns) {
  string result_string;
  for (int i=0; i<rows; i++) {
    for (int j=0; j<columns; j++) {
      string buffer;
      buffer = to_string(result[i][j]);
      result_string.append(buffer);
      if (j != columns - 1) {
        result_string.append(1u, ',');
      } 
    }
    result_string.append(1u, ';'); 
  } 
  char* result_char = (char*) malloc(result_string.size() + 1);
  strcpy (result_char, result_string.c_str()); 
  return result_char;
}

int main(int argc, char** argv) {
  zctx_t* context = zctx_new();
  void* worker = zsocket_new(context, ZMQ_DEALER);
  zsocket_connect(worker, "tcp://localhost:5555");
  if(argc == 1) {   
    zstr_sendx(worker, "hola", NULL);
  }
  else {
    cout<<"this is not the format"<<endl;
    return 0;
  }  
  zmq_pollitem_t items[] = {{worker, 0, ZMQ_POLLIN, 0}};
  while(true) { 
    zmq_poll(items, 1, 10 * ZMQ_POLL_MSEC);
    if(items[0].revents & ZMQ_POLLIN) {
      zmsg_t* msg = zmsg_recv(worker); 
      zframe_t* identity_client = zmsg_pop(msg);
      char* string1 = zmsg_popstr(msg);
      char* string2 = zmsg_popstr(msg);
      int rows_matrix1, rows_matrix2, columns_matrix1, columns_matrix2;
      Matrix matrix1 = char_to_int(string1, &rows_matrix1, &columns_matrix1);
      Matrix matrix2 = char_to_int(string2, &rows_matrix2, &columns_matrix2);
      if(columns_matrix1 == rows_matrix2) {
        result.resize(rows_matrix1, vector<int>(columns_matrix2));
        vector <thread> threads;
        for(int row_to_evaluate=0; row_to_evaluate<rows_matrix1; row_to_evaluate++) {
          threads.push_back(thread(multiply, matrix1, matrix2, row_to_evaluate, columns_matrix1, columns_matrix2));
        }
        for(int i=0; i<rows_matrix1; i++){
          threads[i].join();
        }
        for(int l=0; l<rows_matrix1; l++) {
          for(int j=0; j<columns_matrix2; j++) {
            cout<<result[l][j]<<" ";
          }
          cout<<endl; 
        }
        char* resultado_char;
        resultado_char = int_to_char(result, rows_matrix1, columns_matrix2);
        result.clear();
        cerr<<"la matriz resultado es: "<<resultado_char;
        zframe_t* resultado_frame = zframe_new_empty();
  	zframe_reset(resultado_frame, resultado_char, strlen(resultado_char));
  	zframe_send(&resultado_frame, worker, ZFRAME_REUSE + ZFRAME_MORE);
   	zframe_send(&identity_client, worker, ZFRAME_REUSE);
      }
      else {
        string resultado_string = "las columnas de m1 no son iguales a las filas de m2";
        char* resultado_char = new char [resultado_string.size() + 1];
        strcpy (resultado_char, resultado_string.c_str()); 
        zframe_t* resultado_frame = zframe_new_empty();
  	zframe_reset(resultado_frame, resultado_char, strlen(resultado_char));
  	zframe_send(&resultado_frame, worker, ZFRAME_REUSE + ZFRAME_MORE);
   	zframe_send(&identity_client, worker, ZFRAME_REUSE);
      }
    zmsg_destroy(&msg);
    }
  }
 return 0;
}
