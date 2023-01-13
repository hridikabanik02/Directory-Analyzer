/// =========================================================================
/// Written by pfederl@ucalgary.ca in 2020, for CPSC457.
/// =========================================================================
/// You need to edit this file.
///
/// You can delete all contents of this file and start from scratch if
/// you wish, as long as you implement the analyzeDir() function as
/// defined in "analyzeDir.h".
//Name: Hridika Banik
//UCID: 30123716
//Email: hridika.banik@ucalgary.ca
#include "analyzeDir.h"

#include <cassert>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <stack>
#include <iostream>
constexpr int MAX_WORD_SIZE = 1024;

using namespace std;


std::unordered_map<std::string, int> hist;
std::vector<ImageInfo> largest_images;
std::vector<int> pixelsize; 


/// check if path refers to a directory
static bool is_dir(const std::string & path)
{
  struct stat buff;
  if (0 != stat(path.c_str(), &buff)) return false;
  return S_ISDIR(buff.st_mode);
}

/// check if path refers to a file
static bool is_file(const std::string & path)
{
  struct stat buff;
  if (0 != stat(path.c_str(), &buff)) return false;
  return S_ISREG(buff.st_mode);
}

/// check if string ends with another string
static bool ends_with(const std::string & str, const std::string & suffix)
{
  if (str.size() < suffix.size()) return false;
  else
    return 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}
//This code has been adapted from the link provided in the assignment:
//https://gitlab.com/cpsc457/public/word-histogram/-/blob/master/main.cpp
//This code reads the words from the file and checks for the MAX_WORD_SIZE
//If every requirement is met, the function pushes back the letter of words
std::string next_word(FILE *fp)
{
  std::string result;
  while(1) {
    int c = fgetc(fp); //returns the characters pointed at by the file pointer
    if(c == EOF) break;  //checks till end of file
    c = tolower(c);  //converts it to lower case
    if(! isalpha(c)) {
      if(result.size() == 0)
        continue;
      else
        break;
    }
    else {
      if(result.size() >= MAX_WORD_SIZE) {
        printf("input exceeded %d word size, aborting...\n", MAX_WORD_SIZE);
        exit(-1);
      }
      result.push_back(c);
    }
  }
  return result;
}
//I have defined this function 'fnamefixer' so as to remove the './' so that my program only displays the file name without the ./ as mentioned in
//the assignment document. My function removes the first two characters from the path provided, whoch according to my code always has ./ and it returns
// a string which contains the file name. 
std::string fnamefixer(std::string dirN){
  auto direc = dirN; //stores it in another variable
  direc.erase(0,2); //removes the first two elements, in this case, '.' and '/'
  return direc;

  
}
//This is a selection sorting method that I used to sort the pixel sizes of my images
//The function Imageorder takes in two vectors one of which displays the name of the images 
//and the other one which contains the pixel sizes of the image
void Imageorder(vector<ImageInfo> &arr, vector <int> &num)
{
      int i;
      int j;
      int f1;
      int temp; //temporary variable to hold
      ImageInfo temp2;      
      int numLength = num.size( );
      for (i= numLength - 1; i > 0; i= i-1)
     {f1 = 0;            
      for (j=1; j<=i; j= j+1)   
        {
            if (num[j] < num[f1])
            f1 = j;
        }
        temp = num[f1];   //
        temp2 = arr[f1];  //
        num[f1] = num[i]; //
        arr[f1] = arr[i]; //
        num[i] = temp;       //
        arr[i] = temp2;      //
     }
     return;
}
// ======================================================================
// You need to re-implement this function !!!!
// ======================================================================
//
// analyzeDir(n) computes stats about current directory
//   n = how many words and images to report in restuls
//
// The code below is incomplate/incorrect. Feel free to re-use any of it
// in your implementation, or delete all of it.
Results analyzeDir(int n)
{
  // The code below does a subset of the functionality you need to implement.
  // It also contains several debugging printf() statements. When you submit
  // your code for grading, please remove any dubugging pritf() statements.
    Results res;
    res.largest_file_path = "";
    res.largest_file_size = -1;
    res.n_files = 0;
    res.n_dirs = 0;
    res.all_files_size = 0;
    

    std::stack<std::string> stk;  //creates a stack
    stk.push(".");  //pushes a . into the stack that shows the current directory

    while( ! stk.empty()) { //checks if stack is empty
      auto dirN = stk.top(); //references to the last elemnt in the stack 
      stk.pop(); // removes the last element from the stack since we will examine this directory
      if(is_dir(dirN)) { //checks if the path gotten from above is a directory
        res.n_dirs++;  //increments the total count of directory
      }
      if (is_file(dirN)){ //checks if the path provided is a file or directory
        res.n_files++;  // increments the total count of files
        if (ends_with(dirN, ".txt")) { //checks if the file provided is a .txt since the words from a .txt file can be only included in the final count of repeated words
        FILE* fp; //pointer to a file structure so that operation like read,open,close can be performed
        fp = fopen(dirN.c_str(), "r"); //opens the file of the provided path to read it
        if(fp != NULL) {  //checks if the file open function succeeded
          //This code has been adapted from the link provided in the assignment:
          //https://gitlab.com/cpsc457/public/word-histogram/-/blob/master/main.cpp
           while(1) {
              auto w = next_word(fp);
              if(w.size() == 0){ //checks 
                break;}
              else if(w.size() >= 5){ //word is a sequence of 5 or more alphabetic characters, converted to lower case.
                hist[w] ++;}
          }fclose(fp); //closes the file that has already been processed.
        
      }
      
        }
        struct stat buff;
        if (stat(dirN.c_str(), &buff) == -1){ //Checks if the stat is sucessfully implemented
          printf("Error in implementing struct");
          exit(-1); //exits the code
        }
        else{
          int fsize =  (int)buff.st_size; //uses st_size to get the size of the file and storing it in a variable
          res.all_files_size = res.all_files_size + fsize; //calculates the cumulative size of files

           if (fsize > res.largest_file_size) { //checks if the file size tha we got is bigger than the orevious largest file size
                res.largest_file_size = fsize;  //changes the value of largest file size to the current file size
                auto fname1= fnamefixer(dirN);  //uses the function fnamefixer to remove the ./ from the from the path
                res.largest_file_path = fname1; //store the value of the path to the current file as the largest file path
            }

          }
  

          // For Image Processing
          { 
          //This code has been adapted from the link provided by the professor in the latest email and the original assignment code: 
          // https://replit.com/@pfederl/str22ints#main.cpp
          std::string cmd = "identify -format '%wx%h' " + dirN + " 2> /dev/null";
          std::string img_size; 
          auto fp = popen(cmd.c_str(), "r"); //opens the file for reading
          assert(fp); 
          char buff[PATH_MAX];
          if( fgets(buff, PATH_MAX, fp) != NULL) {
            img_size = buff;
            long n1, n2;
            sscanf(img_size.c_str(), "%ldx%ld", &n1, &n2);
            auto fname2= fnamefixer(dirN);  //removes the ./ so as to only show file name
            ImageInfo info1 { fname2, n1, n2 }; //adds it to the struct
            res.largest_images.push_back(info1);
            pixelsize.push_back(n1*n2); //pushes it back to the vector
          }
          pclose(fp);
            
          }
      }
      //This code is used to change the directory stream and lead to other filestream
      DIR *dir; //pointer to a DIR struct
      dir = opendir( dirN.c_str()); //opens directory stream of the given path
      if( dir) { //checks if directory was opened
        while(!0) {
          dirent * entity; //pointer to iterate over all the elements
          entity = readdir( dir); //starting reading in the directory that we opened
          if( ! entity) break;
          std::string name = entity-> d_name; //accessing the name of the file being read
          if( name == "." || name == "..") continue; 
          std::string path = dirN + "/" + entity-> d_name; //creates the path by joining the dirN and file 
          stk.push( path); //pushing the path into the stack that was created earlier
        }
        closedir( dir);
      }



    }
    //This code has been adapted from the link provided by the professor in the assignment document:
    // https://gitlab.com/cpsc457/public/word-histogram/-/blob/master/main.cpp
    std::vector<std::pair<int,std::string>> arr;
    for(auto & h : hist){
      arr.emplace_back(-h.second, h.first);}
      // if we have more than N entries, we'll sort partially, since
      // we only need the first N to be sorted
    if(arr.size() > size_t(n)) {
      std::partial_sort(arr.begin(), arr.begin() + n, arr.end());
      
      // drop all entries after the first n
      arr.resize(n);} 
    else {
      std::sort(arr.begin(), arr.end());}
      // now we can print the n entries
      //printf("%d most common words:\n", n);
    for(auto & b : arr){
      res.most_common_words.emplace_back(b.second, -b.first);}
    //Sorting for images in descending order
    Imageorder(res.largest_images, pixelsize); //calling the function to sort the vector according to descending order
    if(res.largest_images.size() > size_t(n)){ //Only displays the number of order of images as inputted
      res.largest_images.resize(n); //resizing the vector according to the users input
    }
  //The vacant directory part of the assignment is not completed.
  res.vacant_dirs.push_back("path1/subdir1");
  res.vacant_dirs.push_back("test2/xyz");
  return res;
}
