// JAGBE++.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include "cpu.h"
#include "memory.h"
#include <cassert>
#include "flags.h"
#include <filesystem>
#include <fstream>
#include "jagbe++exceptions.h"
int main()
{
    assert(flags::f_getCF8(0xFF, 0x01) == flags::CARRYBIT);

    std::ifstream fin;
    std::string fPath;
    std::cin >> fPath;
    std::cin.ignore();
    fin.open(fPath, std::ios::binary | std::ios::ate);
    auto end = fin.tellg();
    fin.seekg(0, std::ios::beg);

    std::cerr << "BOOTROM LEN: " << end << std::endl;
    if (int(end) != 256) {
        std::cerr << "Invalid boot rom.";
        return 1;
    }
    uint8_t* bootRom = new uint8_t[0x100];
    fin.read((char*)bootRom, 0x100);
    cpu l_cpu = cpu(bootRom);
    bootRom = nullptr; // the cpu owns this now, so make sure no wierd things happen.
    try
    {
        while (true)
        {
            l_cpu.f_tick(MCYCLE * 64);
        }
    }
    catch (const unimplementedInstructionException& e)
    {
        std::cerr << e.what() << std::endl;
        throw;
    }
    return 0;
}
