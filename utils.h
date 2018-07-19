#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <QJsonObject>
#include <vector>
#include <QJsonDocument>
#include <QStringList>
#include <QTime>
using namespace std;

//const string WELCOME_DISPLAY = "WELCOME TO CHADA (CHAsing your Dreams and Adventures)";

//const string SIGN_OUT_MSG = "SIGN OUT";
//const string SIGN_IN_MSG = "SIGN IN";

//const string EMAIL_CONFIRM = "We advise that you check your email for updates about your purchase/s.";

//const string TEXT_ART = "                                  .-.\n"
//                        "                                 /___\\\n"
//                        "                                 |___|\n"
//                        "                                 |]_[|\n"
//                        "                                 / I \\\n"
//                        "                              JL/  |  \JL\n"
//                        "   .-.                    i   ()   |   ()   i                    .-.\n"
//                        "   |_|     .^.           /_\  LJ=======LJ  /_\           .^.     |_|\n"
//                        "._/___\._./___\_._._._._.L_J_/.-.     .-.\_L_J._._._._._/___\._./___\._._._\n"
//                        "       ., |-,-| .,       L_J  |_| [I] |_|  L_J       ., |-,-| .,        .,\n"
//                        "       JL |-O-| JL       L_J%%%%%%%%%%%%%%%L_J       JL |-O-| JL        JL\n"
//                        "IIIIII_HH_'-'-'_HH_IIIIII|_|=======H=======|_|IIIIII_HH_'-'-'_HH_IIIIII_HH_\n"
//                        "-------[]-------[]-------[_]----\.=I=./----[_]-------[]-------[]--------[]-\n"
//                        " _/\_  ||\\\\_I_//||  _/\\_ [_] []_/_L_J_\\_[] [_] _/\\_  ||\\\\_I_//||  _/\\_  ||\\\n"
//                        " |__|  ||=/_|_\=||  |__|_|_|   _L_L_J_J_   |_|_|__|  ||=/_|_\=||  |__|  ||-\n"
//                        " |__|  |||__|__|||  |__[___]__--__===__--__[___]__|  |||__|__|||  |__|  |||\n"
//                        "IIIIIII[_]IIIII[_]IIIIIL___J__II__|_|__II__L___JIIIII[_]IIIII[_]IIIIIIII[_]\n"
//                        " \_I_/ [_]\_I_/[_] \_I_[_]\II/[]\_\I/_/[]\II/[_]\_I_/ [_]\_I_/[_] \_I_/ [_]\n"
//                        "./   \.L_J/   \L_J./   L_JI  I[]/     \[]I  IL_J    \.L_J/   \L_J./   \.L_J\n"
//                        "|     |L_J|   |L_J|    L_J|  |[]|     |[]|  |L_J     |L_J|   |L_J|     |L_J\n"
//                        "|_____JL_JL___JL_JL____|-||  |[]|     |[]|  ||-|_____JL_JL___JL_JL_____JL_J\n"
//                        "by Valkyrie\n\n\n";

//const string TEST_ITEM_STATUS = "TEST ITEM";
//const string DENIED_STATUS = "DENIED";
//const string PENDING_STATUS = "PENDING";
//const string OUT_OF_STOCK_STATUS = "OUT OF STOCK";
//const string ON_SALE_STATUS = "ON SALE";
//const string NOT_ON_SALE_STATUS = "NOT ON SALE";

void clearScreen();
void enterToContinue();
string getCurrentTime();
string substringAtPos(string str, int pos, char delim); //returned string does not include the delim char
string convertCharToString(char c);
void removeSubstring(string &str, string substr);
bool isContactNumFormat(string str);
bool isLettersOnly(string str);
bool isLettersAndNumbersOnly(string str);
bool ifStringHasSubstring(string str, string substr);
bool checkEmailHasCorrectFormat(string email);
bool fileExists(string &name);
void genRandomID(char *s, const int len);
string toLowerString(string str);
bool isFileEmpty(ifstream& pFile);
string limitStringSize(string str, int size);
string numberToString(float number, int prec);
void createFileIfNotExist(string filename);
bool deleteFileOnly(string fileName);
bool checkIfVectorHasItem(vector<string> items, string item);

string arrayToString(vector<string> &strArray);
bool hasStringInArray(string strV, string str);
vector<string> arrayToVector(string strV);
string url_encode(const string &value);
QString jsonObjectToQString(QJsonObject &obj);
QJsonObject qstringToJSONObject(QString str);
void removeItemFromVec(vector<QString> &vec, QString itemToRemove);
void shuffleList(QStringList &list);


#endif // UTILS_H
