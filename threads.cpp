#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <deque>
using namespace std;

mutex mtx_fq;
mutex mtx_cq;
void countWork(deque<int> &countDeq)
{
	while(countDeq.size() > 1)
	{
		int number_one = 0;
		int number_two = 0;
		mtx_cq.lock();
		if(countDeq.size() > 1)
		{
			number_one = countDeq.front();
			countDeq.pop_front();
			number_two = countDeq.front();
			countDeq.pop_front();
			countDeq.push_back(number_one + number_two);
		}
		mtx_cq.unlock();
	}
}

void readWork(deque<string> &fileDeq, deque<int> &countDeq, string w)
{
	string topFile;
	string fWord;
	while(fileDeq.size() > 0)
	{	
		//lock files que
		mtx_fq.lock();
		if(fileDeq.size() > 0)
		{
			//pop file
			topFile = fileDeq.front();
			fileDeq.pop_front();
		}
		//unlock file que
		mtx_fq.unlock();
		//count 'word' in file
		ifstream fileIn(topFile);
		if(fileIn.is_open())
		{
			int count = 0;
			while(fileIn >> fWord)
			{	
				if(fWord == w)
					count++;
			}
			//lock count que
			mtx_cq.lock();
			//close file
			fileIn.close();
			//push count
			countDeq.push_back(count);
			//unlock count que
			mtx_cq.unlock();
		}
	}
	return;
}

int main(int argc, char** argv) 
{
	string word;
	string fileName;
	deque<string> files;
	deque<int> counts;

	cout  << "Enter a word you would like to count: ";
	cin >> word;
	ifstream datFile("files.dat");
	if(datFile.is_open())
	{
		while(getline(datFile, fileName))
		{
			files.push_back(fileName);
		}
		datFile.close();
	}
	else
	{
		cout<< "File did not open." << endl;
	}

	thread m1(readWork, ref(files), ref(counts), word);
	thread m2(readWork, ref(files), ref(counts), word);
	thread m3(readWork, ref(files), ref(counts), word);
	thread m4(readWork, ref(files), ref(counts), word);
	m1.join();
	m2.join();
	m3.join();
	m4.join();

	thread r1(countWork, ref(counts));
	thread r2(countWork, ref(counts));
	thread r3(countWork, ref(counts));
	thread r4(countWork, ref(counts));
	r1.join();
	r2.join();
	r3.join();
	r4.join();

	cout << "Count: " << counts.front() << endl;
	
	return 0;
}