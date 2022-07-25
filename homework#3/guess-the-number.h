#pragma once

// OTUS C++ basic course homework#3
// definitions for "guess number" game program

void play_the_game (int);

// high scoring

const std::string high_scores_filename = "high_scores.txt";

int  print_table ();
std::streamoff  find_record (std::string& name, int& score);
bool update_record (std::string& user, int score);
