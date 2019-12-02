
#pragma once
#include<iostream>
#include<string>
#include"Huffman.h"
using namespace std;




bool isLeaf(NODE* root);

string getType(char* s);

void ArrayOutput(HuffMap& map, int index, int a[], int n);


void CompressFile(HuffMap& map, NODE* root, int arr[], int top , int &index);


void HuffmanCompress(FILE* fileInput, FILE* fileOut, HuffMap& map, HuffData  data, int size , char* nameFile);




//Decode


void ReadHeaderFile(FILE* fileIN, HuffData& data);


void WriteHeaderFile(FILE* Output, HuffData data);



void Decode(FILE* in, FILE* out);


void EncodeFile();

void EncodeMultiFile(char* filename);

void ExportFile();

void ExportFile(char *filename);


void EncodeFolder();

void DecodeFolder(FILE* in, FILE* out, int vtLE);

void ExportFolder();

//Suport

char* ToCharArray(string name);


