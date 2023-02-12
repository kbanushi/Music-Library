/// TODO Update File Header
///
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

//
// NOTE TO STUDENTS:
// You may use the functions in the helpers file,
// but you may not modify the file or its contents in any way whatsoever.
//

struct Album{ //Keep running total of all albums we've seen with the information of who the artists are and what the songs are
    string name;
    set<string> artists;
    set<string> songs;
};

//
// STUDENT CODE MAY GO BELOW THIS LINE
//

bool ContainsLetter(string word){
    for (char letter : word){
        if (!isdigit(letter))
            return true;
    }
    return false;
}

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
            if (!ContainsLetter(firstWord))
                albums[albumName].insert(line);
            else if (line != albumName)
                artists[line].insert(albumName);
        }
    }
}

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
}

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

set<string> IntersectSet(set<string>& cumulativeSet, set<string>& matches){
    set<string> results;

    set_intersection(cumulativeSet.begin(), cumulativeSet.end(), matches.begin(), matches.end(), inserter(results, results.begin()));

    return results;
}

set<string> SubtractSet(set<string>& cumulativeSet, set<string>& matches){
    set<string> results;

    set_difference(cumulativeSet.begin(), cumulativeSet.end(), matches.begin(), matches.end(), inserter(results, results.begin()));

    return results;
}

void UnionSet(set<string>& allResults, set<string>& resultOfSingleSearch){
    for (auto& element : resultOfSingleSearch){
        allResults.insert(element);
    }
}

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
}

int main()
{
    string userEntry;
    string command, remains;
    map<string, set<string>> albums; //album name is key, set of songs are value
    map<string, set<string>> artists; //artist name is key, set of albums are value

    // TODO: Declare any necessary variables for the music libary

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
            cout << endl;
        }          
        else if (command == "search")
        {
            if (remains.size() == 0)
                cout << "Error: Search terms cannot be empty." << endl;
            Search(albums, artists, remains);
            cout << endl;
        }

    }while(command != "exit");    

    cout << "Thank you for using the Music Library App" << endl;
    return 0;
}
