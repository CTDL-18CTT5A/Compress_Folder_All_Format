#include<iostream>
#include<vector>
#include<sstream>
#include<fstream>
#include<bitset>
#include<map>
#include"Huffman.h"
#include <iterator>
using namespace std;
NODE* newNode(char data, int freq)
{
	NODE* temp = new NODE;
	temp->pLeft = NULL;
	temp->pRight = NULL;
	temp->_text = data;
	temp->_freq = freq;
	return temp;

}




bool IsExist(char s, vector<char> str)
{
	for (vector<char>::iterator iter_name = str.begin(); iter_name != str.end(); iter_name++)
	{
		if (s == *iter_name)
			return true;
	}
	return false;

}

//Đếm số lương kí tự khác nhau trong file
long long NumberOfCharFile(FILE* p)
{
	if (p == NULL)
		return 0;
	int count = 0;
	fseek(p, 0, SEEK_END);
	count = ftell(p);
	return count;
}

HuffData ReadFileText(FILE* p)
{
	if (p == NULL)
	{
		cout << "Khong the doc file";
		exit(0);
	}
	HuffData haf;
	long long n = 500;
	rewind(p);
	haf.s = new char[n];
	haf.wei = new int[n];

	vector<char> s;
	int row = 0;


	int i = 0;

	char* tempMemory = new char[2025];


	while (!feof(p))
	{
		fgets(tempMemory, 1025, p);

		int cc = 0;
		for (int i = 0; i < strlen(tempMemory); i++)
		{
			if (IsExist(tempMemory[i], s) == false)
			{
				s.push_back(tempMemory[i]);
				haf.wei[row] = 1;
				haf.s[row++] = tempMemory[i];

			}
			else
			{
				int rowtemp = 0;
				for (vector<char>::iterator iter_name = s.begin(); iter_name != s.end(); iter_name++)
				{
					if (tempMemory[i] == *iter_name)
					{
						break;
					}
					rowtemp++;
				}
				haf.wei[rowtemp] += 1;
			}
		}



	}
	haf.s[row] = NULL;
	return haf;

}

HuffData ReadFileBin(FILE* p)
{
	if (p == NULL)
	{
		cout << "Khong the doc file";
		exit(0);
	}
	HuffData haf;
	long long n = 500;
	rewind(p);
	haf.s = new char[n];
	haf.wei = new int[n];

	vector<char> s;
	int row = 0;




	int size = NumberOfCharFile(p);



	rewind(p);

	char* Memory = new char[size];
	fread(Memory, sizeof(char) , size, p);
	Memory[size] = '\0';

	map<char, int> chars;
	for (int k = 0; k < strlen(Memory); k++) 
	{
		chars[Memory[k]]++;
	}
	int Count = 0;
	for (map<char, int>::iterator it = chars.begin(); it != chars.end(); it++)
	{
		haf.s[Count] = it->first;
		haf.wei[Count] = it->second;
		

		Count++;
	}

	haf.s[Count] = '\0';
	return haf;

}



//Read data file exe
vector<char> readfile(const char* filename)
{
	ifstream infile(filename, ios::binary);
	vector<char> result;
	copy(istream_iterator<char>(infile),
		istream_iterator<char>(),
		back_inserter(result));
	return result;
}


HuffData ReadFileExe(FILE* p , char * filename)
{
	if (p == NULL)
	{
		cout << "Khong the doc file";
		exit(0);
	}
	HuffData haf;
	long long n = 500;
	rewind(p);
	haf.s = new char[n];
	haf.wei = new int[n];



	int size = NumberOfCharFile(p);


	rewind(p);

	vector<char> data = readfile(filename);
	cout << data.size() << endl;//how do i edit the data read from exe file?

	map<char, int> chars;
	for (int k = 0; k < data.size(); k++)
	{
		chars[data[k]]++;
	}
	int Count = 0;
	for (map<char, int>::iterator it = chars.begin(); it != chars.end(); it++)
	{
		haf.s[Count] = it->first;
		haf.wei[Count] = it->second;


		Count++;
	}


	haf.s[Count] = '\0';
	return haf;
}

HuffmanTree* InitHuffTree(int size)
{
	HuffmanTree* hufftree = new HuffmanTree;

	hufftree->size = 0;

	hufftree->Array = (NODE**)malloc(size * sizeof(NODE*));

	return hufftree;

}

void SwapNode(NODE*& a, NODE*& b)
{
	NODE* temp = a;
	a = b;
	b = temp;
}

void MinHeapify(HuffmanTree* hufftree, int index)
{
	int min = index;
	int left = 2 * index + 1;
	int right = 2 * index + 2;

	if (left < hufftree->size && hufftree->Array[left]->_freq < hufftree->Array[min]->_freq)
	{
		min = left;
	}
	if (right < hufftree->size && hufftree->Array[right]->_freq < hufftree->Array[min]->_freq)
	{
		min = right;
	}
	if (min != index)
	{
		SwapNode(hufftree->Array[min], hufftree->Array[index]);
		MinHeapify(hufftree, min);
	}
}

NODE* getNodeMin(HuffmanTree* hufftree)
{
	NODE* node = hufftree->Array[0];
	//Lấy ra thì phải đưa cuối lên đầu và trừ bớt size(giống heap sort)
	hufftree->Array[0] = hufftree->Array[hufftree->size - 1];

	--hufftree->size;
	MinHeapify(hufftree, 0);

	return node;
}

void insertHuffTree(HuffmanTree* hufftree, NODE* newnode)
{
	++hufftree->size;
	//Vị trí cuối cùng để thêm vào phần tử mới
	int posInsert = hufftree->size - 1;

	//Nếu node insert nhỏ hơn root thì phải chuyển lên đầu để tiếp tục tạo root rồi chèn vào
	 //Nó sẽ so với root mà chưa cây con đó(tại vì khi thêm vào chỉ cần check tại nhanh đó thôi)
	//Đẩu thằng root nhỏ nhất luôn lên vị trí pos = 0;
	while (posInsert && newnode->_freq < hufftree->Array[(posInsert - 1) / 2]->_freq)
	{
		hufftree->Array[posInsert] = hufftree->Array[(posInsert - 1) / 2];
		posInsert = (posInsert - 1) / 2;
	}

	hufftree->Array[posInsert] = newnode;

}



void BuildMinHeap(HuffmanTree* hufftree)
{
	int n = hufftree->size - 1;
	for (int i = (n - 1) / 2; i >= 0; i--)
	{
		MinHeapify(hufftree, i);
	}
}

bool isMinSize(HuffmanTree* hufftree)
{
	return (hufftree->size == 1);
}


HuffmanTree* CreateHeapHuffman(HuffData map, int size)
{
	HuffmanTree* hufftree = InitHuffTree(size);
	hufftree->size = size;

	for (int i = 0; i < size; i++)
	{
		hufftree->Array[i] = newNode(map.s[i], map.wei[i]);
	}

	BuildMinHeap(hufftree);

	return hufftree;
}




NODE* builfHuffmanTree(HuffData map, int size)
{

	NODE* left, * right, * root;

	HuffmanTree* hufftree = CreateHeapHuffman(map, size);

	while (!isMinSize(hufftree))
	{
		left = getNodeMin(hufftree);
		right = getNodeMin(hufftree);

	

		root = newNode(' ', left->_freq + right->_freq);

		root->pLeft = left;
		root->pRight = right;

		insertHuffTree(hufftree, root);
	}

	//Sau khi đã liên kết lại hết với nhau thì chỉ cần lấy ra root

	return getNodeMin(hufftree);

}