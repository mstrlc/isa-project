/*
 * server.h
 * 2023-11-20t
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

/**
 * @brief Server to handle connections
 *
 * @param port Port to listen on
 * @param data Database of users
 * @return int Status code
 */
int server(int port, std::vector <std::vector <std::string> > data);
