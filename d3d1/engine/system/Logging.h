#pragma once

#include <iostream>
#include <string>

extern FILE *ENGLogOutputFile;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define ENGLog(...) (emscripten_log(EM_LOG_NO_PATHS, __VA_ARGS__))
#else
#define ENGLog(fmt, ...) fprintf(ENGLogOutputFile, fmt, __VA_ARGS__);fprintf(ENGLogOutputFile, "%s", "\n");fflush(ENGLogOutputFile);
#endif

void ENGLogSetOutputFile(const std::string &filename);
