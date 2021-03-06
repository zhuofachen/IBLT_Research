#include <iostream>
#include "testFunctions.h"
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include "iblt.h"



using namespace std;


template<typename T>
std::uint64_t Toint(T vector)
{
  std::uint64_t num;
  num=vector[1]*256+vector[0];
  return num;
}

//get the transaction ID from a .txt file and 
//creates an array of pointers to arrays of key value pairs
//input "filename.txt", note "" and .txt should be included, and no space at the end of the file. 
//input numEntries = number of key-value pairs to be created
//here just input a numEntries as a variable and the function will count the numbers. 

vector<uint8_t>** entryArray(string Filename, int &numEntry)    //KeyTxIDFile3.txt
 {


    ifstream getFile;
    getFile.open(Filename);
    string getID;
    numEntry=0;
    std::vector<std::string> IDvec; 
    if(getFile.is_open())
    {
       while (std::getline(getFile, getID))
       {
         std::istringstream ss(getID);

         std::string token;
     
         // Remove commas. 
        getline(ss,token,',');
        IDvec.push_back(token);
        numEntry++;

        }
     }
       
     //remove quoation marks
    for (unsigned int i = 0; i < IDvec.size(); i++) {
        auto it = std::remove_if(IDvec[i].begin(), IDvec[i].end(), [&] (char c) { return c == '"'; });
        IDvec[i] = std::string(IDvec[i].begin(), it);
    }  
     //Remove the first two space in every string. 
     std::vector<std::string> IDvec2;
     IDvec2.assign(IDvec.size(), "0");

     for(int i=0;i<IDvec.size();i++)
     {       
          IDvec2[i]=IDvec[i].substr(2,64);

     }
      vector<uint8_t> v;
      vector<uint8_t> **valueArray=new vector<uint8_t> *[numEntry];
     for(int i=0;i<numEntry;i++)
     {
        for (unsigned int j=0; j<64; j++)
            {
              char temp=IDvec2[i][j];
              uint8_t tempInt=static_cast<uint8_t>(temp);     
               v.push_back(tempInt); 
            }
            int k=i;
            valueArray[i]=new vector<uint8_t>[2];
            valueArray[i][0]=std::move(ToVec(k));
            valueArray[i][1]=std::move(v);
     }
 
    return valueArray; 
}

// print the key and transaction ID stored in the Array. 
//the key vector is converted into uint64_t by using the template Toint(). 
void printArray(vector<uint8_t> ** valueArray,int numEntries)
{
     std::cout<<"output the data store in entry array"<<std::endl;
      for (int j=0; j<numEntries; j++)
         {    
           std::cout<<"the key is: "<<Toint(valueArray[j][0])<<std::endl;
           for (unsigned int i=0; i<valueArray[j][1].size(); i++)
          {   
              std::cout<<unsigned (valueArray[j][1].at(i))<<"  ";

          }
          std::cout<<std::endl;
         }  
         std::cout<<"finish outputing entry array"<<std::endl;
}

//inserts the entries generated by createValues() into the IBLT
//input: entries = array of arrays of key value pairs
//input: iblt = pointer to the iblt object
//input: numEntries = the number of key-value pairs being inserted into the iblt
void addEntries(vector<uint8_t>** entries, IBLT* iblt, int numEntries)
{
  for (int i = 0; i < numEntries; i++)
  {  
    iblt->insert(Toint(entries[i][0]), entries[i][1]);
  }
}

//compares the set made by the IBLT during list values to the key-value pairs generated by createValues()
//input: positiveValues = set of key value pairs generated by IBLT during list values operation
//input: myEntries = multidimensional array of key value pairs that were inserted into the iblt
//input: numEntries = the number of entries originally inserted into the iblt
void compare(std::set<std::pair<uint64_t,std::vector<uint8_t> > > positiveValues, vector<uint8_t>** myEntries, int numEntries)
{
  //creates new set with the key value pairs from myEntries
  std::set<std::pair<uint64_t, std::vector<uint8_t> > > entrySet;

  for (int i = 0; i < numEntries; i++)
  {
    entrySet.insert(std::make_pair(Toint(myEntries[i][0]), myEntries[i][1]));
  }

  //compares manually created set to iblt generated set
  //gives corresponding output to indicate success of list entries
  //states that listing entries was successful or counts the number of missing entries
  if (entrySet == positiveValues)
  {
    cout << "Successful Listing of All Entries" << endl;
  }
  else
  {
    int missingEntries = entrySet.size() - positiveValues.size();

    cout << missingEntries << " Entries were not listed." << endl;
  }

}

//compares the set made by the IBLT during list values to the key-value pairs generated by createValues()
//input: positiveValues = set of key value pairs generated by IBLT during list values operation
//input: myEntries = multidimensional array of key value pairs that were inserted into the iblt
//input: numEntries = the number of entries originally inserted into the iblt
//output: returns true if all of the inserted key/value pairs were successfully listed out during list entries
bool compare2(std::set<std::pair<uint64_t,std::vector<uint8_t> > > positiveValues, vector<uint8_t>** myEntries, int numEntries)
{
  std::set<std::pair<uint64_t, std::vector<uint8_t> > > entrySet;

  for (int i = 0; i < numEntries; i++)
  {
    entrySet.insert(std::make_pair(Toint(myEntries[i][0]), myEntries[i][1]));
  }

  if (entrySet == positiveValues)
  {
    return true;
  }
  else
  {
    return false;
  }

}

//Generates a vector of keys that are unique to one of the sets
//input: aVals, bVals = multidimensional arrays of key value pairs
//input: numEntries = number of pairs in each multidimensional array
//output: vector = keys that are only found in one of the sets



vector<uint64_t> makeDiffVector(vector<uint8_t>** aVals, vector<uint8_t>** bVals, int numEntriesA, int numEntriesB)
{
  //sort the array by key
  for (int i = 0; i < numEntriesA - 1; i++)
  {
    for (int j = i + 1; j < numEntriesA; j++)
    {
      if (Toint(aVals[i][0]) > Toint(aVals[j][0]))
      {
        vector<uint8_t>* tempArray = aVals[i];
        aVals[i] = aVals[j];
        aVals[j] = tempArray;
      }
    }
  }

  //sorts the array by key
  for (int i = 0; i < numEntriesB - 1; i++)
  {
    for (int j = i + 1; j < numEntriesB; j++)
    {
      if (Toint(bVals[i][0]) > Toint(bVals[j][0]))
      {
        vector<uint8_t>* tempArray = bVals[i];
        bVals[i] = bVals[j];
        bVals[j] = tempArray;
      }
    }
  }

  //creates a vector of keys that are only in one of the arrays
  vector<uint64_t> differences;


  bool unique = false;

  //iterates through all keys in aVals to check if they are also in bVals
  for (int k = 0; k < numEntriesA; k++)
  {
    for (int m = 0; m < numEntriesB; m++)
    {
      if (Toint(aVals[k][0])== Toint(bVals[m][0]))
      {
        unique = false;

        break;
      }
      else
      {
        unique = true;
      }
    }

    //if value is only in aVal, adds the key to differences
    if (unique)
    { 
      differences.push_back(Toint(aVals[k][0]));
    }
  }

  //ierates through all keys in bVals to check if they are also in aVals
  for (int k = 0; k < numEntriesB; k++)
  {
    for (int m = 0; m < numEntriesA; m++)
    {
      if (Toint(bVals[k][0]) == Toint(aVals[m][0]))
      {
        unique = false;

        break;
      }
      else
      {
        unique = true;
      }
    }

    //if the key is only in bVals, it adds it ot differences
    if (unique)
    {
     
      // differences.push_back(bVals[k]);
      differences.push_back(Toint(bVals[k][0]));
    }
  }

  return differences;

}

