#pragma once
#include"CompressFile.h"
#include <stdio.h>
#include<iostream>
#include<vector>
#include<sstream>
#include<bitset>
using namespace std;



bool isLeaf(NODE* root)
{
	return !(root->pLeft) && !(root->pRight);

}

void ArrayOutput(HuffMap &map , int index , int a[], int n)
{
	int i = 0;
	for (i; i < n; ++i)
	{
		map.BitArray[index][i] = a[i];
	}
	map.BitArray[index][i] = 2;

}



//Duyệt cây lưu dữ liệu vào map
void CompressFile(HuffMap &map , NODE* root, int arr[], int top , int &index)
{
	// Assign 0 to left edge and recur 
	if (root->pLeft) 
	{

		arr[top] = 0;
		CompressFile(map ,root->pLeft, arr, top + 1,index);
	}

	// Assign 1 to right edge and recur 
	if (root->pRight) 
	{

		arr[top] = 1;
		CompressFile(map,  root->pRight, arr, top + 1, index);
	}

	
	//Index là biến đếm những node lá ứng với các kí tự
	if (isLeaf(root)) 
	{ 
		map.charater[index] = root->_text;
		map.BitArray[index] = new int[top];
		ArrayOutput(map, index , arr, top);
		index++;
	}
}


int convertBinaryToDecimal(long long n)
{
	int decimalNumber = 0, i = 0, remainder;
	while (n != 0)
	{
		remainder = n % 10;
		n /= 10;
		decimalNumber += remainder * pow(2, i);
		++i;
	}
	return decimalNumber;
}

string getType(char* s)
{
	string type;
	for (int i = strlen(s) - 1; i >= 0; i--)
	{
		if (s[i] == '.')
			break;

		type += s[i];

	}
	reverse(type.begin(), type.end());
	return type;
}

char* ToCharArray(string name)
{
	char* fileName = new char[name.length() + 1];
	strcpy(fileName, name.c_str());
	return fileName;
}
//Lưu đống dữ liệu vào map
void HuffmanCompress(FILE *fileInput , FILE *fileOut , HuffMap &map , HuffData  data, int size , char*nameFile)
{
	// Construct Huffman Tree 
	NODE* root = builfHuffmanTree(data, size);
	// Print Huffman codes using 
	// the Huffman tree built above 
	int arr[1000];
	long long top = 0;
	map.BitArray = new int* [size];

	map.charater = new char[size];
	
	string typeFile = getType(nameFile);
	
	char* type = ToCharArray(typeFile);
	type[strlen(type)] = '\0';

	int index = 0;
	CompressFile(map , root, arr, top , index);
	
	//Write type
	char soluongDuoi = char(strlen(type) + 48);
	fwrite(&soluongDuoi, sizeof(char), 1, fileOut);
	fwrite(type, sizeof(char), strlen(type), fileOut);

	WriteHeaderFile(fileOut, data);
	rewind(fileInput);
	//Lưu vào file
	char ch;
	fread(&ch, sizeof(char), 1, fileInput);



	//Dem so luong cac bit da ma hoa
	long long countBit8 = 0;
	//Tong cac bit da ghi ra
	long long countSumBit = 0;


	long long countBit = 0;
	long long sumBitOut = 0;
	char bit8[9];

	while (!feof(fileInput))
	{
		
		int j = 0;
		for (int i = 0; i < size; i++)
		{
			
			if (ch == map.charater[i])
			{
				while (map.BitArray[i][j] != 2)
				{
					countSumBit++;
					bit8[countBit++] = char(map.BitArray[i][j] + 48);
					if (countBit == 8)
					{

						stringstream sstream(bit8);
						std::bitset<8> bits;
						sstream >> bits;
						char textOutput = char(bits.to_ulong());
						fwrite(&textOutput, sizeof(char), 1, fileOut);
						countBit = 0;
						countBit8 += 8;
					}

					j++;
				}

				break;
			}
			
		}
		fread(&ch, sizeof(char), 1, fileInput);
	}
	
	int soBitLe = 0;
	for (int z = 0; z < countBit; z++)
	{
		//fprintf(fileOut, "%c", bit8[z]);
		fwrite(&bit8[z], sizeof(char), 1, fileOut);
		soBitLe++;
	}
	char bl = char(soBitLe + 48);
	fwrite(&bl, sizeof(char), 1, fileOut);
	
	/*delete root;
	delete[] map.BitArray;
	delete[] map.charater;
	delete type;
	rewind(fileInput);*/


	//fprintf(fileOut, "%d", soBitLe);

	
}



void WriteHeaderFile(FILE* Output, HuffData data)
{
	int size = strlen(data.s);
	//fwrite(&(size), sizeof(int), 1, Output);.
	//fprintf(Output, "%d", size);
	//fwrite("$", 1, 1, Output);
	fprintf(Output, "%d", size);
	fprintf(Output, "†");


	for (int i = 0; i < size; i++)
	{
		//fwrite(&(data.s[i]), sizeof(char), 1, Output);

		fprintf(Output, "%c", data.s[i]);
		fprintf(Output, "%d", data.wei[i]);
		if (i != size - 1)
			fprintf(Output, "†");
		else
			fprintf(Output, "˜");
	}

}


void ReadHeaderFile(FILE* fileIN, HuffData& data)
{
	char ch;
	//ch = fgetc(fileIN);
	//Get size
	int size = 0;


	//get Type


	fread(&ch, 1, 1, fileIN);



	while (ch!= '†')
	{
		int t = int(ch - 48);
		size = size * 10 + t;
		fread(&ch, 1, 1, fileIN);
	}
	cout << "Size = " << size << endl;
	data.s = new char[size];
	data.wei = new int[size];

	fread(&ch, 1, 1, fileIN);
	//Save vào data
	int i = 0;
	int maxBit = 0;
	while (ch != '˜')
	{
		/*if (ch == '\n')
			ch = getc(fileIN);*/
		data.s[i] = ch;
		
		//Save wei
		int wei = 0;
		fread(&ch, 1, 1, fileIN);
		while (ch != '†')
		{
			int t = int(ch - 48);
			wei = wei * 10 + t;
			fread(&ch, 1, 1, fileIN);
			if (ch == '˜')
			{
				cout << ftell(fileIN) << endl;

				break;
			}

		}
		data.wei[i] = wei;
		//cout << data.wei[i] << endl;

		i++;
		if (ch == '˜')
			break;	
		fread(&ch, 1, 1, fileIN);

	}
	data.s[i] = '\0';
}




long long getNumberOfFileAtIndex(FILE* in, int index)
{
	fseek(in, index, SEEK_SET);
	int thisPoint = ftell(in);
	fseek(in, 0, SEEK_END);
	long long maxsize = ftell(in);
	return maxsize - thisPoint;

}

long long getNumberOfFileAtIndex(FILE* in, int index , int posEnd)
{
	fseek(in, index, SEEK_SET);
	int thisPoint = ftell(in);
	return posEnd - thisPoint;

}


//Hàm trả về vị trí cuối bit lưu số bit lẻ trả vệ  vị trí con trỏ ở bit lẻ
long long VitriLe(FILE* fileIN)
{

	fseek(fileIN, 0, SEEK_END);
	return ftell(fileIN) - 1;

}

void ConvertToBinArray(FILE* in, int indexStart, int* a , long long &k , long long viTriLe)
{
	long long EndFile = VitriLe(in);
	fseek(in, indexStart, SEEK_SET);
	int count = ftell(in);
	/////////////DANH DAU
	char ch;
	fread(&ch, sizeof(char), 1, in);
	while (count != viTriLe)
	{

		for (int i = 7; i >= 0; --i)
		{
			char s = ((ch & (1 << i)) ? '1' : '0');
			s -= 48;
			a[k] = s;
			k++;
		}
		fread(&ch, sizeof(char), 1, in);

		count++;
	}

	while (count !=EndFile)
	{
	
		a[k] = int(ch - 48);
		k++;
		fread(&ch, sizeof(char), 1, in);
		count++;
	}
}

void ConvertToBinArrayOfFolder(FILE* in, int indexStart, int* a, long long& k, long long viTriLe , long long endpos)
{
	long long EndFile = endpos;
	fseek(in, indexStart, SEEK_SET);
	int count = ftell(in);
	/////////////DANH DAU
	char ch;
	fread(&ch, sizeof(char), 1, in);
	while (count != viTriLe)
	{
		for (int i = 7; i >= 0; --i)
		{
			char s = ((ch & (1 << i)) ? '1' : '0');
			s -= 48;
			a[k] = s;
			k++;
		}
		fread(&ch, sizeof(char), 1, in);

		count++;
	}

	while (count != EndFile)
	{

		a[k] = int(ch - 48);
		k++;
		fread(&ch, sizeof(char), 1, in);
		count++;
	}
	
	cout << "Ftell : " << ftell(in) << endl;
}


void Decode(FILE* in, FILE* out)
{
	long long curpos = ftell(in);
	//Lay Bit Le
	//______________Gán vtLe để lấy ra số bit lẻ_______________
	long long vtLe = VitriLe(in);
	fseek(in, vtLe, SEEK_SET);
	char ch[2];
	ch[1] = '\0';
	fread(ch, sizeof(char), 1, in);

	int SoLuongBitLe = int(ch[0] - 48);
	//Sau khi lưu số lượng bit lẻ vì vtLe đang nằm ở cuối file , ta trừ đi số bit lẻ => ra đc vị trí số lẻ đầu tiên.
	for(int z = 0; z < SoLuongBitLe; z++)
		vtLe -= 1;

	fseek(in, curpos, SEEK_SET);


	HuffData data;
	ReadHeaderFile(in, data);
	


	//____________Define max size of bit array_____________
	int StartIndex = ftell(in);
	long long sizeOfBit = getNumberOfFileAtIndex(in, StartIndex)*8;


	int* bit = new int[sizeOfBit];
	long long n = 0;
	//rewind(in);
	fseek(in, StartIndex, SEEK_SET);



	//__________Chuyển kí tự về bit và lưu hết vào trong mảng bit____________
	ConvertToBinArray(in, StartIndex, bit , n , vtLe);
	//Tiếp tục lưu các bia thừa.

	int size = strlen(data.s);
	//_________Tạo ra map dữ liệu____________

	NODE* root = builfHuffmanTree(data, size);	
	HuffMap map;
	map.BitArray = new int* [size]; //SS
	map.charater = new char [size]; //SS
	int arr[500];
	long long top = 0;

	int index = 0;
	CompressFile(map, root, arr, top, index);

	//Nếu cây huffman không tồn tại(Tức chỉ có 1 phần tử trong file)
	if (size == 1)
	{
		int temp = 0;
		while (temp < data.wei[0])
		{
			fprintf(out, "%c",data.s[0]);
			temp++;
		}
		return;
	}
	cout << "____________________________" << endl;

	//________Chuyển từ dãy bit sang string để dễ so sánh_________
	string* BitArray = new string[size];
	for (int i = 0; i < size; i++)
	{
		int j = 0;
		while (map.BitArray[i][j]!=2)
		{
			char test = char(map.BitArray[i][j]+48);
			BitArray[i] += test;

			j++;
		}
	}

	
	//Bắt đầu so sánh và ghi ra file
	string compare;
	int byte = 0;
	for (int i = 0; i < n; i++)
	{

		compare += char(bit[i]+48);
		for (int k = 0; k < size; k++)
		{
			if (compare == BitArray[k])
			{

				char op[2];
				op[1] = '\0';
				op[0] = map.charater[k];
				fwrite(op, sizeof(char), 1, out);
				byte++;
				compare.clear();
				break;
			}
		}
	}






}



void EncodeFile()
{
	string namefile;
	cout << "Nhap vao ten file muon nen : ";
	cin >> namefile;

	string DuoiFile;
	int pos = namefile.find(".")+1;
	DuoiFile = namefile.substr(pos, namefile.size());

	FILE* file = fopen(ToCharArray(namefile), "rb");
	FILE* fileCompresss = fopen("InputCompress.huf", "wb");
	HuffData data;
	if (DuoiFile == "exe")
	{
		data = ReadFileExe(file, ToCharArray(namefile+"."+DuoiFile));
	}
	else
	{
		data = ReadFileBin(file);
	}
	cout << "read complete" << endl;
	HuffMap map;
	int size = strlen(data.s);
	HuffmanCompress(file, fileCompresss, map, data, size, ToCharArray(namefile));
}

void EncodeMultiFile(char *filename )
{
	string namefile;
	
	for (int i = 0; i < strlen(filename); i++)
	{
		namefile += filename[i];
	}

	string DuoiFile;
	int pos = namefile.find(".") + 1;
	DuoiFile = namefile.substr(pos, namefile.size());
	FILE* file = fopen(filename, "rb+");
	FILE* fileCompresss = fopen("InputCompress.huf", "ab+");
	HuffData data;
	if (DuoiFile == "exe")
	{
		data = ReadFileExe(file , filename);
	}
	else
	{
		data = ReadFileBin(file);
	}

	HuffMap map;
	int size = strlen(data.s);
	HuffmanCompress(file, fileCompresss, map, data, size, filename);
	_fcloseall();

	/*fclose(file);
	fclose(fileCompresss);*/
	return;

}

void ExportFile()
{
	string namefile;
	cout << "Nhap ten file can giai nen(.huf) : ";
	cin >> namefile;
	FILE* header = fopen(ToCharArray(namefile + ".huf"), "rb");

	char NumberOfType;
	fread(&NumberOfType, sizeof(char), 1, header);
	int sz = int(NumberOfType - 48);
	char* type = new char[sz];
	fread(type, sizeof(char), sz, header);

	string nameOut;
	nameOut = namefile;
	nameOut += ".";
	//Cong them duoi vao
	for (int i = 0; i < sz; i++)
	{
		nameOut += type[i];
	}
	cout << nameOut << endl;

	FILE* out = fopen(ToCharArray(nameOut), "wb");
	Decode(header, out);
}

void ExportFile(string filename)
{


	FILE* header = fopen(ToCharArray(filename + ".huf"), "rb");


	//bo qua 2 byte dau tien trong file neu no la file dung mot minh
	char pp;
	fread(&pp, 1, 1, header);
	fread(&pp, 1, 1, header);


	char NumberOfType;
	fread(&NumberOfType, sizeof(char), 1, header);
	int sz = int(NumberOfType - 48);
	char* type = new char[sz];
	fread(type, sizeof(char), sz, header);

	string nameOut;
	nameOut = filename;
	nameOut += ".";
	//Cong them duoi vao
	for (int i = 0; i < sz; i++)
	{
		nameOut += type[i];
	}
	cout << nameOut << endl;

	FILE* out = fopen(ToCharArray(nameOut), "wb");
	Decode(header, out);
}


#include"SupportOpenFolder.h"

string subFileName(string str)
{
	string word = "";
	for (auto x : str)
	{
		if (x == '/')
		{
			cout << word << endl;
			word = "";
		}
		else
		{
			word = word + x;
		}
	}
	string newWord;
	for (int i = 0; i < word.size(); i++)
	{
		if (word[i] == '.')
		{
			break;
		}
		newWord += word[i];
	}
	return newWord;
}

string subFolderName(string folderName)
{
	string word = "";
	for (auto x : folderName)
	{
		if (x == '/')
		{
			break;
		}
		else
		{
			word = word + x;
		}
	}
	return word;
}

void EncodeFolder()
{
	char* FolderName;

	vector<string> allName = GetAllNameFILE(FolderName);

	FolderName = ToCharArray(subFolderName(allName[0]));

	FILE* fileCompresss = fopen("InputCompress.huf", "ab+");

	//Ghi ra ten folder can nén , lưu lại dùng cho giải nén
	fprintf(fileCompresss, "%s†", FolderName);
	//Ghi ra so luong file tồn tại trong folder nén
	fprintf(fileCompresss, "%d", allName.size());
	fprintf(fileCompresss, "†");

	fclose(fileCompresss);


	for (int i = 0; i < allName.size(); i++)
	{
		//Write name filete
		FILE* writeNametoFile = fopen("InputCompress.huf", "ab+");
		char* filename = ToCharArray(subFileName(allName[i]));
		puts(filename);
		fwrite(filename, sizeof(char), strlen(filename), writeNametoFile);
		fwrite("†", 1, 1, writeNametoFile);
		fclose(writeNametoFile);
		char* s = ToCharArray(allName[i]);
		EncodeMultiFile(s);
		FILE* fileCompresss = fopen("InputCompress.huf", "ab+");
		fwrite("|||", 3, 1, fileCompresss);
		fclose(fileCompresss);
	}
	_fcloseall();

	//for (int i = 0; i < allName.size(); i++)
	//{
	//	//Write name filete
	//	FILE *writeNametoFile = fopen("InputCompress.huf", "ab+");
	//	char* filename = ToCharArray(subFileName(allName[i]));
	//	puts(filename);
	//	fwrite(filename, sizeof(char), strlen(filename), writeNametoFile);
	//	fwrite("†", 1, 1, writeNametoFile);
	//	char* s = ToCharArray(allName[i]);
	//	EncodeMultiFile(s);
	//	FILE* fileCompresss = fopen("InputCompress.huf", "ab+");
	//	fwrite("|||", 3, 1, fileCompresss);
	//	
	//}
	//_fcloseall();
}

void DecodeFolder(FILE* in, FILE* out, int vtLE)
{
	int curPos = ftell(in);

	//______________Gán vtLe để lấy ra số bit lẻ_______________
	long long vtLe = vtLE;
	fseek(in, vtLe, SEEK_SET);
	char ch[2];
	ch[1] = '\0';
	fread(ch, sizeof(char), 1, in);
	int SoLuongBitLe = int(ch[0] - 48);

	cout << "So luong bit le : " << SoLuongBitLe << endl;


	//Sau khi lưu số lượng bit lẻ vì vtLe đang nằm ở cuối file , ta trừ đi số bit lẻ => ra đc vị trí số lẻ đầu tiên.
	for (int z = 0; z < SoLuongBitLe; z++)
		vtLe -= 1;

	fseek(in, curPos, SEEK_SET);

	cout << "cur pos : " << curPos << endl;

	HuffData data;
	ReadHeaderFile(in, data);



	//____________Define max size of bit array_____________
	int StartIndex = ftell(in);
	long long sizeOfBit = getNumberOfFileAtIndex(in, StartIndex, vtLE) * 8;


	int* bit = new int[sizeOfBit];
	long long n = 0;

	//fseek(in, curPos, SEEK_SET);
	fseek(in, StartIndex, SEEK_SET);



	//__________Chuyển kí tự về bit và lưu hết vào trong mảng bit____________
	ConvertToBinArrayOfFolder(in, StartIndex, bit, n, vtLe, vtLE);
	//Tiếp tục lưu các bia thừa.
	cout << "Size of bit : " << sizeOfBit << endl;
	int size = strlen(data.s);
	//_________Tạo ra map dữ liệu____________

	NODE* root = builfHuffmanTree(data, size);
	HuffMap map;
	map.BitArray = new int* [size]; //SS
	map.charater = new char[size]; //SS
	int arr[500];
	long long top = 0;

	int index = 0;
	CompressFile(map, root, arr, top, index);

	//Nếu cây huffman không tồn tại(Tức chỉ có 1 phần tử trong file)
	if (size == 1)
	{
		int temp = 0;
		while (temp < data.wei[0])
		{
			fprintf(out, "%c", data.s[0]);
			temp++;
		}
		return;
	}
	cout << "____________________________" << endl;

	//________Chuyển từ dãy bit sang string để dễ so sánh_________

	string* BitArray = new string[size];
	for (int i = 0; i < size; i++)
	{
		int j = 0;
		while (map.BitArray[i][j] != 2)
		{
			char test = char(map.BitArray[i][j] + 48);
			BitArray[i] += test;
			j++;
		}
	}


	//Bắt đầu so sánh và ghi ra file
	string compare;
	int byte = 0;
	for (int i = 0; i < n; i++)
	{
		compare += char(bit[i] + 48);
		for (int k = 0; k < size; k++)
		{
			if (compare == BitArray[k])
			{
				char op[2];
				op[1] = '\0';
				op[0] = map.charater[k];
				fwrite(op, sizeof(char), 1, out);
				byte++;
				compare.clear();
				break;
			}
		}
	}

	delete[] bit;
	delete[] BitArray;
	delete[] map.BitArray;
	delete[] map.charater;

	cout << "End tell : " << ftell(in) << endl;



}



vector<int> posStop(char* filename)
{
	char buff[4];
	vector<int> pos;
	FILE* fs = fopen(filename, "rb");
	if (fs != NULL) {
		if (3 == fread(buff, 1, 3, fs)) {
			do {
				if (_strnicmp(buff, "|||", 3) == 0)
					pos.push_back(ftell(fs) - 4);
				memmove(buff, buff + 1, 3);
			} while (1 == fread(buff + 2, 1, 1, fs));
		}
	}
	fclose(fs);

	return pos;
}


//Export file ta chỉ cần đọc file cho tới khi gặp điểm quy ước thì dừng và xuất file đó ra , cứ như thế cho đến khi gặp điểm quy ước cuối cùng
void ExportFolder()
{

#pragma region Lấy ra tên file và tìm những vị trí mà file kết thúc để dừng đọc đúng lúc.

	string namefile;
	cout << "Nhap ten tap tin can giai nen : ";
	cin >> namefile;
	vector<int> posEnd = posStop(ToCharArray(namefile + ".huf"));

#pragma endregion

	//Neu chi co duy nhat mot file tuc la size = 0;
	if (posEnd.empty())
	{
		ExportFile(namefile);
		return;
	}

	FILE* header = fopen(ToCharArray(namefile + ".huf"), "rb");



#pragma region lấy ra tên folder cũ để tạo và ghi vào
	string folderName;
	char tempChar;
	fread(&tempChar, 1, 1, header);
	while (tempChar != '†')
	{
		folderName += tempChar;
		fread(&tempChar, 1, 1, header);
	}
	_mkdir(ToCharArray(folderName));
#pragma endregion




#pragma region Lấy ra số lượng file đã nén trong folder cũ
	long long numberOfFile = 0;
	char ch;
	fread(&ch, 1, 1, header);
	while (ch != '†')
	{
		numberOfFile = numberOfFile * 10 + int(ch - 48);
		fread(&ch, 1, 1, header);
	}
#pragma endregion



	

	//Kiem tra xem day co phai la file duy nhat khong.
	if (posEnd.empty())
	{
		ExportFile(namefile);
		return;
	}
	int curpos = ftell(header);


	fseek(header, curpos, SEEK_SET);

	//Lưu lại vị trí cũ để thư mục cuối cùng có thể mang ra dùng vì posEnd bị giới hạn

	for (int i = 0; i < posEnd.size(); i++)
	{

		//Lấy từng tên file trong folder cũ từ header
		string nameOut;

		char ch;
		fread(&ch, 1, 1, header);
		while (ch != '†')
		{
			nameOut += ch;
			fread(&ch, 1, 1, header);
		}


		//Get type
		cout << "LAN THU " << i << endl;
		char NumberOfType;
		fread(&NumberOfType, sizeof(char), 1, header);
		int sz = int(NumberOfType - 48);
		char* type = new char[sz];
		fread(type, sizeof(char), sz, header);

		nameOut += ".";
		//Cong them duoi vao
		for (int i = 0; i < sz; i++)
		{
			nameOut += type[i];
		}
		cout << nameOut << endl;

		FILE* out = fopen(ToCharArray(folderName +"/"+nameOut), "wb");


		DecodeFolder(header, out, posEnd[i]);

		fclose(out);
		

		//Bo qua 3 byte ngan cach:
		
		char temp[4];
		fread(temp, sizeof(char), 3, header);

	}



}