///@author Krenar Banushi
///@date Feburary 14, 2023
///@brief This program will load music libraries and provide functionality to search for albums by name, artist name, or song name.
///       Additionally we can display the combined libraries to the console, stats about number of albums, aritsts, and songs, and export the combined library to a readable textfile.
/// Assignment details and provided code are created and
/// owned by Adam T Koehler, PhD - Copyright 2023.
/// University of Illinois Chicago - CS 251 Spring 2023

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// INSERTS GIVEN HELPERS CODE BELOW - DO NOT REMOVE
#include "helpers.cpp"
// INSERTS GIVEN HELPERS CODE ABOVE - DO NOT REMOVE

////////////////////////////////////FUNCTION PROTOTYPES////////////////////////////////////
bool ContainsLetter(string word);
void LoadLibrary(map<string, set<string>>& albums, map<string, set<string>>& artists, string fileName);
void PrintLibrary(map<string, set<string>>& albums, map<string, set<string>>& artists);
void DisplayStats(map<string, set<string>>& albums, map<string, set<string>>& artists, string remains);
void ExportToFile(map<string, set<string>>& albums, map<string, set<string>>& artists, string fileName);
set<string> IntersectSet(set<string>& cumulativeSet, set<string>& matches);
set<string> SubtractSet(set<string>& cumulativeSet, set<string>& matches);
void UnionSet(set<string>& allResults, set<string>& resultOfSingleSearch);
set<string> GenerateAlbumsByWord(string keyword, map<string, set<string>>& albums);
set<string> SearchByAlbums(string keywords, map<string, set<string>>& albums);
set<string> GenerateAlbumsByArtist(string keyword, map<string, set<string>>& artists);
set<string> SearchByArtists(string keywords, map<string, set<string>>& artists);
set<string> GenerateAlbumsWithSong(string keyword, map<string, set<string>>& albums);
set<string> SearchBySong(string keywords, map<string, set<string>>& albums);
void Search(map<string, set<string>>& albums, map<string, set<string>>& artists, string remains);
////////////////////////////////////HELPER FUNCTIONS////////////////////////////////////
///@brief checks if a given string contains any letters
///@param word the string to check
///@return true if the word contains a letter, false otherwise
bool ContainsLetter(string word){
    for (char letter : word){
        if (!isdigit(letter))
            return true;
    }
    return false;
}

/// @brief Produce resulting set after intersecting two input sets
/// @param cumulativeSet Left hand side of intersection
/// @param matches Right hand side of intersection
/// @return set of type string of cumulativeSet + matches
set<string> IntersectSet(set<string>& cumulativeSet, set<string>& matches){
    set<string> results;

    set_intersection(cumulativeSet.begin(), cumulativeSet.end(), matches.begin(), matches.end(), inserter(results, results.begin()));

    return results;
}

/// @brief Produce resulting string set after subtracting two string sets
/// @param cumulativeSet Left hand side of subtraction
/// @param matches Right hand side of subtraction
/// @return set of type string of cumulativeSet - matches
set<string> SubtractSet(set<string>& cumulativeSet, set<string>& matches){
    set<string> results;

    set_difference(cumulativeSet.begin(), cumulativeSet.end(), matches.begin(), matches.end(), inserter(results, results.begin()));

    return results;
}

/// @brief Will insert results of second string set to the first string set
/// @param allResults Sets to insert elements to
/// @param resultOfSingleSearch Sets to take elements from
///        Return by reference through allResults
void UnionSet(set<string>& allResults, set<string>& resultOfSingleSearch){
    for (auto& element : resultOfSingleSearch){
        allResults.insert(element);
    }
}

/// @brief Generate set of type string with all albums that contain a keyword
/// @param keyword string specifying keyword
/// @param albums map of albums with album name as key and songs in set as value
/// @return set of type string with all albums containing the keyword
set<string> GenerateAlbumsByWord(string keyword, map<string, set<string>>& albums){
    set<string> matchingAlbums;
    string lowercaseName;

    for (auto& album : albums){
        lowercaseName = album.first;
        tolower(lowercaseName);
        if (int(lowercaseName.find(keyword)) >= 0){
            matchingAlbums.insert(album.first);
        }
    }
    return matchingAlbums;
}

/// @brief Generate set of type string with all all the albums containing list of keywords
/// @param keywords string of keywords with or without modifiers
/// @param albums map of albums with album name as key and songs in set as value
/// @return set of type string with album names that match the search criteria
set<string> SearchByAlbums(string keywords, map<string, set<string>>& albums){
    set<string> resultOfSingleSearch;
    set<string> allResults;
    stringstream ss;
    string word;

    ss << keywords;
    while (ss >> word){
        if (word.at(0) == '+'){
            resultOfSingleSearch = GenerateAlbumsByWord(word.substr(1,word.size()), albums);
            allResults = IntersectSet(allResults, resultOfSingleSearch);
        }
        else if (word.at(0) == '-'){
            resultOfSingleSearch = GenerateAlbumsByWord(word.substr(1,word.size()),albums);
            allResults = SubtractSet(allResults, resultOfSingleSearch);
        }
        else{
            resultOfSingleSearch = GenerateAlbumsByWord(word, albums);
            UnionSet(allResults, resultOfSingleSearch);
        }
    }

    return allResults;
}

/// @brief Generate set of type string of all the albums created by the artist(s) with the keyword in their name
/// @param keyword string of keyword to search for
/// @param artists map of artists with artist name as key and albums in set as value
/// @return set of type string of all the albums worked on by the artist(s) containing the keyword in their name
set<string> GenerateAlbumsByArtist(string keyword, map<string, set<string>>& artists){
    set<string> resultingAlbums;
    string lowercaseName;

    for (auto& artist : artists){
        lowercaseName = artist.first;
        tolower(lowercaseName);
        if (int(lowercaseName.find(keyword)) >= 0){
            for (auto& album : artist.second){
                resultingAlbums.insert(album);
            }
        }
    }

    return resultingAlbums;
}

/// @brief return set of type string of all the albums worked on by artist(s) containing keywords and modifiers
/// @param keywords string of keywords and modifiers
/// @param artists map of artists with artist name as key and albums in set as value
/// @return set of type string of albums the artist(s) have worked on
set<string> SearchByArtists(string keywords, map<string, set<string>>& artists){
    set<string> resultOfSingleSearch;
    set<string> allResults;
    stringstream ss;
    string word;

    ss << keywords;
    while (ss >> word){
        if (word.at(0) == '+'){
            resultOfSingleSearch = GenerateAlbumsByArtist(word.substr(1,word.size()), artists);
            allResults = IntersectSet(allResults, resultOfSingleSearch);
        }
        else if (word.at(0) == '-'){
            resultOfSingleSearch = GenerateAlbumsByArtist(word.substr(1,word.size()), artists);
            allResults = SubtractSet(allResults, resultOfSingleSearch);
        }
        else{
            resultOfSingleSearch = GenerateAlbumsByArtist(word, artists);
            UnionSet(allResults, resultOfSingleSearch);
        }
    }

    return allResults;
}

/// @brief return set of type string of albums containing song with keyword in its name
/// @param keyword string of keyword to search for
/// @param albums map of albums with album name as key and songs in set as value
/// @return set of type string of albums
set<string> GenerateAlbumsWithSong(string keyword, map<string, set<string>>& albums){
    set<string> resultingAlbums;
    string lowercaseName;

    for (auto& album : albums){
        for (auto& song : album.second){
            lowercaseName = song;
            tolower(lowercaseName);
            if (int(lowercaseName.find(keyword)) >= 0){
                resultingAlbums.insert(album.first);
            }
        }
    }
    return resultingAlbums;
}

/// @brief return set of type string of albums containing song(s) with keywords in its name
/// @param keywords string of keywords and modifiers
/// @param albums map of albums with album name as key and songs in set as value
/// @return set of string of albums
set<string> SearchBySong(string keywords, map<string, set<string>>& albums){
    set<string> resultOfSingleSearch;
    set<string> allResults;
    stringstream ss;
    string word;

    ss << keywords;
    while (ss >> word){
        if (word.at(0) == '+'){
            resultOfSingleSearch = GenerateAlbumsWithSong(word.substr(1,word.size()), albums);
            allResults = IntersectSet(allResults, resultOfSingleSearch);
        }
        else if (word.at(0) == '-'){
            resultOfSingleSearch = GenerateAlbumsWithSong(word.substr(1,word.size()), albums);
            allResults = SubtractSet(allResults, resultOfSingleSearch);
        }
        else{
            resultOfSingleSearch = GenerateAlbumsWithSong(word, albums);
            UnionSet(allResults, resultOfSingleSearch);
        }
    }

    return allResults;
}

////////////////////////////////////FUNCTIONS////////////////////////////////////
/// @brief loads the music library into map of albums and map of strings using a file name
/// @param albums map of albums with album name as key and songs in set as value
/// @param artists map of artists with artist name as key and albums in set as value
/// @param fileName string of file name
void LoadLibrary(map<string, set<string>>& albums, map<string, set<string>>& artists, string fileName){
    if (fileName.size() == 0)
        fileName = "musicdatabase.txt";
    ifstream inFS(fileName);
    string albumName, line, firstWord, remainder;
    if (!inFS.is_open()){
        cout << "Error: Could not open music library file - " << fileName << endl;
        return;
    }

    while (getline(inFS,albumName)){
        while (line != albumName){
            getline(inFS, line);
            splitFirstWord(line, firstWord, remainder);
            if (!ContainsLetter(firstWord)) //If it is a song name
                albums[albumName].insert(line);
            else if (line != albumName) //If it is an artist name
                artists[line].insert(albumName);
        }
    }
}

/// @brief Prints the contents of the libray to the console using the album and artist maps
/// @param albums map of albums with album name as key and songs in set as value
/// @param artists map of artists with artist name as key and albums in set as value
///                return by output to console
void PrintLibrary(map<string, set<string>>& albums, map<string, set<string>>& artists){
    cout << endl;
    cout << "Your Current Music Library Includes" << endl
         << "===================================" << endl;
    for (auto& album : albums){
        cout << album.first << endl;
        for (auto& artist : artists){
            if (artist.second.count(album.first) > 0){
                cout << " " << artist.first << endl;
            }
        }
        for (auto& song : album.second){
            cout << "   " << song << endl;
        }
    }
}

/// @brief Prints the stats of the music library to console
/// @param albums map of albums with album name as key and songs in set as value
/// @param artists map of artists with artist name as key and albums in set as value
/// @param remains Modifier to specify if we should also print the library to console (-d)
void DisplayStats(map<string, set<string>>& albums, map<string, set<string>>& artists, string remains){
    int recordCount = albums.size(), artistCount = artists.size(), songCount = 0;

    set<string> songs;
    for (auto& album : albums){
        for (auto& song : album.second){
            songs.insert(song);
        }
    }
    songCount = songs.size();

    cout << "Overall Music Library Stats" << endl
         << "===========================" << endl
         << "Total Records: " << recordCount << endl
         << "Total Unique Artists: " << artistCount << endl
         << "Total Songs: " << songCount << endl;

    if (remains.size() >= 2 && remains.at(0) == '-' && remains.at(1) == 'd'){
        PrintLibrary(albums, artists);
    }
    cout << endl;
}

/// @brief Export music library information to a text file
/// @param albums map of albums with album name as key and songs in set as value
/// @param artists map of artists with artist name as key and albums in set as value
/// @param fileName name of file to export to
void ExportToFile(map<string, set<string>>& albums, map<string, set<string>>& artists, string fileName){
    if (fileName.size() == 0)
        fileName = "musicdatabase.txt";
    ofstream outFS(fileName);
    if (!outFS.is_open()){
        cout << "error" << endl;
    }

    for (auto& album : albums){
        outFS << album.first << endl;
        for (auto& artist : artists){
            if (artist.second.count(album.first) > 0)
                outFS << artist.first << endl;
        }
        for (auto& song : album.second){
            outFS << song << endl;
        }
        outFS << album.first << endl;
    }
}

/// @brief Search and display albums based on album name, artist name, or song name
/// @param albums map of albums with album name as key and songs in set as value
/// @param artists map of artists with artist name as key and albums in set as value
/// @param remains keywords and modifiers the user is searching for
void Search(map<string, set<string>>& albums, map<string, set<string>>& artists, string remains){
    string type, keywords;
    tolower(remains);
    splitFirstWord(remains, type, keywords);
    set<string> searchResults;

    if (type == "album")
        searchResults = SearchByAlbums(keywords, albums);
    else if (type == "artist")
        searchResults = SearchByArtists(keywords, artists);
    else if (type == "song")
        searchResults = SearchBySong(keywords, albums);

    if (searchResults.size() == 0)
        cout << "No results found." << endl;
    else{
        cout << "Your search results exist in the following albums: " << endl;
        for (auto& album : searchResults){
            cout << album << endl;
        }
    }
    cout << endl;
}

int main()
{
    string userEntry;
    string command, remains;
    map<string, set<string>> albums; //album name is key, set of songs are value
    map<string, set<string>> artists; //artist name is key, set of albums are value

    // Display welcome message once per program execution
    cout << "Welcome to the Music Library App" << endl;
    cout << "--------------------------------" << endl;

    // Stay in menu until exit command
    do
    {
        cout << endl;
        cout << "Enter a command (help for help): " << endl;
        getline(cin, userEntry);
        cout << endl;

        // Split the user entry into two pieces
        splitFirstWord(userEntry, command, remains);
        tolower(command);

        // take an action, one per iteration, based on the command
        if (command == "help")
        {
            helpCommand();
        }
        else if (command == "clear")
        {
            albums.clear();
            artists.clear();
        }
        else if (command == "export")
        {
            ExportToFile(albums, artists, remains);
        }
        else if (command == "load")
        {
            LoadLibrary(albums, artists, remains);
        }
        else if (command == "stats")
        {
            DisplayStats(albums, artists, remains);
            
        }          
        else if (command == "search")
        {
            if (remains.size() == 0)
                cout << "Error: Search terms cannot be empty." << endl;
            Search(albums, artists, remains);
        }

    }while(command != "exit");    

    cout << "Thank you for using the Music Library App" << endl;
    return 0;
}
