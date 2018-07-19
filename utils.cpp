#include<iostream>
#include <time.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "utils.h"
#include <vector>
#include "dirent.h"
using namespace std;

void clearScreen(){
    if (system("CLS")) system("clear");
}

void enterToContinue(){
    do {
        cout << "Press enter to continue.......(If does not continue, press enter again.)" << endl;
        cin.ignore();
        string temp;
        getline(cin, temp);
        if (temp.empty()) break;
    } while (1);
}

string getCurrentTime(){
    time_t theTime = time(NULL);
    struct tm * aTime = localtime(&theTime);

    int day = aTime -> tm_mday;
    int month = aTime -> tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    int year = aTime -> tm_year + 1900; // Year is # years since 1900

    ostringstream strs1;
    strs1 << day;
    string strDate = strs1.str();

    ostringstream strs2;
    strs2 << month;
    string strMonth = strs2.str();

    ostringstream strs3;
    strs3 << year;
    string strYear = strs3.str();
    return strDate + "/" + strMonth + "/" + strYear;
}

//returned string does not include the delim char
string substringAtPos(string str, int pos, char delim){

    string finalStr = "";
    for (int i = pos; i < str.length(); i++){
        if (str[i] != delim){
            finalStr += str[i];
        }
        else break;
    }
    return finalStr;
}

string convertCharToString(char c){
    stringstream ss;
    string target;
    ss << c;
    ss >> target;
    return target;
}

void removeSubstring(string &str, string substr){
    if (str.size() >= substr.size()){
        string::size_type i = str.find(substr);

        if (i != std::string::npos)
            str.erase(i, substr.length());
    }
}

bool isContactNumFormat(string str){
    return str.find_first_not_of("0123456789") == std::string::npos;
}

bool isLettersOnly(string str){
    return str.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos;
}

bool isLettersAndNumbersOnly(string str){
    return str.find_first_not_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") == std::string::npos;
}

bool ifStringHasSubstring(string str, string substr){
    if (str.find(substr) != std::string::npos) {
        return true;
    }
    return false;
}

bool checkEmailHasCorrectFormat(string email){
    if (ifStringHasSubstring(email, "@gmail.com") || ifStringHasSubstring(email, "@yahoo.com") || ifStringHasSubstring(email, "@outlook.com")
            || ifStringHasSubstring(email, "@ymail.com") || ifStringHasSubstring(email, "@aol.com") || ifStringHasSubstring(email, "@zoho.com")
            || ifStringHasSubstring(email, "@mail.com")){
        if (isLettersAndNumbersOnly(substringAtPos(email, 0, '@'))) return true;
    }
    return false;
}

bool fileExists(string &name){
    ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

void genRandomID(char *s, const int len){
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
}

string toLowerString(string str){
    string lower = str;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

bool isFileEmpty(ifstream& pFile){
    return pFile.peek() == ifstream::traits_type::eof();
}

string limitStringSize(string str, int size){
    string newStr = "";
    if (str.length() > size){
        for (int i = 0; i < size; i++){
            if (i >= size - 3){
                newStr += ".";
            }
            else newStr += str[i];
        }
    }
    else return str;
    return newStr;
}

string numberToString(float number, int prec){
    ostringstream strs;
    strs << setprecision(prec) << fixed << number;
    return strs.str();
}

void createFileIfNotExist(string filename){
    if (!fileExists(filename)){
        ofstream ofs;
        ofs.open(filename.data(), std::ofstream::out);
        ofs.close();
    }
}

bool checkIfVectorHasItem(vector<string> items, string item){
    for (string type : items){
        if (type == item) return true;
    }
    return false;
}

bool deleteFileOnly(string fileName){
    remove(fileName.c_str());
}


string arrayToString(vector<string> &strArray)
{
    string arrayStr = "[";

    for (int i = 1; i <= strArray.size(); i++){
        string str = strArray[i - 1];
        if (i != strArray.size()){
            arrayStr += str + ",";
        }
        else arrayStr += str;
    }
    arrayStr += ",]";
    return arrayStr;
}


bool hasStringInArray(string strV, string str)
{
    string strArray = strV;
    vector<string> strArrayV;
    if (strV == "[,]") return false;
    removeSubstring(strArray, "[");
    removeSubstring(strArray, "]");
    while (!strArray.empty()){
        string nameAtFirst = substringAtPos(strArray, 0, ',');
        removeSubstring(strArray, nameAtFirst + ",");
        strArrayV.push_back(nameAtFirst);
    }
    for (string &value : strArrayV){
        if (str == value){
            return true;
        }
    }
    return false;
}


vector<string> arrayToVector(string strV)
{
    string strArray = strV;
    vector<string> strArrayV;
    if (strV == "[,]") return strArrayV;
    removeSubstring(strArray, "[");
    removeSubstring(strArray, "]");
    while (!strArray.empty()){
        string nameAtFirst = substringAtPos(strArray, 0, ',');
        removeSubstring(strArray, nameAtFirst + ",");
        strArrayV.push_back(nameAtFirst);
    }
    return strArrayV;
}


string url_encode(const string &value)
{
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        // make sure c is positive for msvc assertion
        if (c >= 0 && (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')) {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char) c);
        escaped << nouppercase;
    }

    return escaped.str();
}


QString jsonObjectToQString(QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    return strJson;
}


void removeItemFromVec(vector<QString> &vec, QString itemToRemove)
{
    int ind = -1;
    for (int i = 0; i < vec.size(); i++){
        QString item = vec.at(i);
        if (item == itemToRemove){
            ind = i;
            break;
        }
    }
    if (ind != -1) vec.erase(vec.begin() + ind);
}


QJsonObject qstringToJSONObject(QString str)
{
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
    return doc.object();
}


void shuffleList(QStringList &list)
{
    qsrand(QTime::currentTime().msec());
    QStringList tempList;
    for (QString str : list){
        tempList.push_back(str);
    }
    int size = list.size();
    list.clear();
    while (list.size() < size){
        int randInd = qrand() % tempList.size();
        QString selectedStr = tempList.at(randInd);
        list.push_back(selectedStr);
        tempList.removeOne(selectedStr);
    }
}
