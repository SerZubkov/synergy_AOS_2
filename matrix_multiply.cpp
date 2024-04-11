#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <cstring>

using namespace std;

// перемножение строк матрицы A на матрицу B в результат C
void multiplyRowByMatrix(int row, const vector<vector<int> > &A, const vector<vector<int> > &B, int *C, int B_cols)
{
  for (int j = 0; j < B_cols; ++j)
  {
    C[row * B_cols + j] = 0;
    for (int k = 0; k < B.size(); ++k)
    {
      C[row * B_cols + j] += A[row][k] * B[k][j];
    }
  }
}

int main()
{
  // Для примера
  vector<vector<int> > A = {{1, 2}, {3, 4}, {5, 6}};
  vector<vector<int> > B = {{1, 2}, {3, 4}};

  if (A[0].size() != B.size())
  {
    cerr << "Матрицы не могут быть перемножены" << endl;
    return 1;
  }

  int A_rows = A.size();
  int B_cols = B[0].size();

  
  size_t result_size = A_rows * B_cols * sizeof(int);
  // Выделение общей памяти
  int *C = (int *)mmap(NULL, result_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  for (int i = 0; i < A_rows; ++i)
  {
    pid_t pid = fork();
    if (pid == 0)
    {
      multiplyRowByMatrix(i, A, B, C, B_cols);
      munmap(C, result_size); // Освобождение памяти
      return 0;
    }
    else if (pid < 0)
    {
      cerr << "Ошибка при создании процесса" << endl;
      return 1;
    }
  }

  // Ожидание завершения всех дочерних процессов
  while (wait(NULL) > 0);

  // Вывод результата
  for (int i = 0; i < A_rows; ++i)
  {
    for (int j = 0; j < B_cols; ++j)
    {
      cout << C[i * B_cols + j] << " ";
    }
    cout << endl;
  }

  // Освобождение ресурсов
  munmap(C, result_size);

  return 0;
}
