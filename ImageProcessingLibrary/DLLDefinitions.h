#pragma once

#ifdef IMAGEPROCESSINGLIBRARY_EXPORTS
#define IMPROC_API __declspec(dllexport)
#else
#define IMPROC_API __declspec(dllimport)
#endif