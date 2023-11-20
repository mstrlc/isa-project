/*
 * csv.h
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>

// Name, mail, login are strings
// Rows are vectors of strings
// Data is vector of rows
std::vector<std::vector<std::string> > read_csv(std::string filename);
