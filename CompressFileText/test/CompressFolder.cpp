#include"SupportOpenFolder.h"
#include"CompressFolder.h"
#include<vector>
#include<iostream>
using namespace std;

string subFileName(string str)
{
	string word = "";
	for (auto x : str)
	{
		if (x == '/')
		{
			word = "";
		}
		else
		{
			word = word + x;
		}
	}

	//Bo di duoi type
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
		if (x == '\\')
		{
			word = "";
		}
		else
		{
			word = word + x;
		}
	}
	string newWord; //Muc tieu la bo di cai link tap tin cuoi cung (/....)
	/*int i = 0;
	newWord[i] = word
	while (newWord)
	{

	}*/
	return word;
}

void ConvertToBinArrayOfFolder(FILE* in, int indexStart, int* a, long long& k, long long viTriLe, long long endpos)
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

//Lay size byte
long long getNumberOfFileAtIndex(FILE* in, int index, int posEnd)
{
	fseek(in, index, SEEK_SET);
	int thisPoint = ftell(in);
	return posEnd - thisPoint;

}


void EncodeFolder(string filename)
{
	char* FolderName;
	cout << filename << endl;
	vector<string> allName = GetAllNameFILE(FolderName , filename);

	FolderName = ToCharArray(subFolderName(filename));
	cout << "FolderName : " << FolderName << endl;
	FILE* fileCompresss = fopen("InputCompress.huf", "ab+");
	fprintf(fileCompresss, "2");
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
		cout << subFolderName(allName[i]) << endl;
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



	//Sau khi lưu số lượng bit lẻ vì vtLe đang nằm ở cuối file , ta trừ đi số bit lẻ => ra đc vị trí số lẻ đầu tiên.
	for (int z = 0; z < SoLuongBitLe; z++)
		vtLe -= 1;

	fseek(in, curPos, SEEK_SET);


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

void EncodeMultiFile(char* filename)
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
		data = ReadFileExe(file, filename);
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
void ExportFolder(string namefolder)
{

#pragma region Lấy ra tên file và tìm những vị trí mà file kết thúc để dừng đọc đúng lúc.

	
	vector<int> posEnd = posStop(ToCharArray(namefolder));

#pragma endregion

	//Neu chi co duy nhat mot file tuc la size = 0;
	if (posEnd.empty())
	{
		ExportFile(namefolder);
		return;
	}

	FILE* header = fopen(ToCharArray(namefolder), "rb");



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





	int curpos = ftell(header);


	fseek(header, curpos, SEEK_SET);

	//Lưu lại vị trí cũ để thư mục cuối cùng có thể mang ra dùng vì posEnd bị giới hạn

	for (int i = 0; i < posEnd.size(); i++)
	{

		//Lấy từng tên file trong folder cũ từ header
		string nameOut;

		//Doc lay ten
		char ch;
		fread(&ch, 1, 1, header);
		while (ch != '†')
		{
			nameOut += ch;
			fread(&ch, 1, 1, header);
		}


		//Get type
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

		FILE* out = fopen(ToCharArray(folderName + "/" + nameOut), "wb");


		DecodeFolder(header, out, posEnd[i]);

		fclose(out);


		//Bo qua 3 byte ngan cach:

		char temp[4];
		fread(temp, sizeof(char), 3, header);

	}



}