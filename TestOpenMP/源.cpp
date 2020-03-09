#include<iostream>
#include<omp.h>
using namespace std;

int main() {
	cout << "parallel test begin" << endl;
	#pragma omp parallel num_threads(4)
	{
		cout << omp_get_thread_num() << endl;
	}
	cout << "parallel test end" << endl;

	return 0;
	system("pasue");
}