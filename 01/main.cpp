#include <iostream>
#include "matrix.h"

/// Зато без <code> using namespace std; </code> ))))))))
using std::cout;
using std::cin;
using std::endl;

/*
 * © Лабараторная работа №1. Вариант №14. Студент Фиреръ Анатолий (Б19-513)
 *
 * программа позволяет ввести прямоугольную разреженную матрицу в следующем формате:
 * <число строк m> <число столбцов n> <m*n целых чисел>
 *
 * в ответ выводится сама матрица и вектор b, каждый iй элемент которого есть сумма
 * удовлетворяющих предикату чисел iй строки.
 * как и было указано в условии, есть возможность быстро сменить предикат, задающий условие.
 * Это достигнуто путем того, что предикат задается ссылкой на функцию. Достатоно написать
 * новую функцию в main и во время отладки менять передаваемую функцию, тем самым быстро
 * меня предикат.
 */

/// предикат, используемый для отбора чисел, входящих в сумму.
/// Данный предикат возвращает истину для чётных чисел
bool pred1(int a) {
  return (a % 2) == 0;
}
/// предикат, используемый для отбора чисел, входящих в сумму.
/// Данный предикат возвращает истину для чисел с более чем 2 десятичными знаками в записи
bool pred2(int a) {
  return a >= 100;
}
// этот комментарий написан из google chrome на смартфоне. 
// возможно так будет проще кодить, чем через vnc
int main() {
  cout << "Type count of coulms and rows in matrix: " << endl;
  int M, N;
  cin >> M >> N;
  matrix mt(M, N);
  mt.set_comparator(pred1);
  // ввод матрицы
  cout << "Type " << N * M << " integers:" << endl;
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      int tmp;
      cin >> tmp;
      mt[i]->insert(j, tmp);
    }
  }
  cout << "----------|  MATRIX  |--------------" << endl;
  mt.print();
  mt.calculate();
  cout << "----------| VECTOR B |--------------" << endl;
  mt.print_b();
  return 0;
}